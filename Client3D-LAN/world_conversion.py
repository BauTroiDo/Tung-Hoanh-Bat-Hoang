"""Legacy initial-state decoder and explicitly anchored 3D coordinate conversion."""
import json
import math
import pathlib
import struct
from functools import lru_cache

REFERENCE = pathlib.Path(__file__).resolve().parent/'reference'
SCHEMA = json.loads((REFERENCE/'native-layout.json').read_text())
# g_InitProtocol sets these extension envelopes to variable-length at runtime.
for _opcode in range(250,255):
    SCHEMA['protocols'][str(_opcode)]={'size':-1}


def split_batch(batch):
    offset = 0
    while offset < len(batch):
        opcode = batch[offset]
        spec = SCHEMA['protocols'].get(str(opcode))
        if spec is None:
            raise ValueError(f'Unmapped legacy opcode {opcode}')
        size = spec['size']
        if size == -1:
            if len(batch)-offset < 3:
                raise ValueError('Truncated variable-length protocol header')
            size = struct.unpack_from('<H',batch,offset+1)[0]+1
            if size < 3:
                raise ValueError('Invalid variable-length protocol')
        if size <= 0 or offset+size > len(batch):
            raise ValueError(f'Truncated legacy protocol {opcode}')
        yield opcode, batch[offset:offset+size]
        offset += size


def field(packet, type_name, field_name, fmt='<I'):
    offset = SCHEMA['layouts'][type_name]['fields'][field_name]['offset']
    return struct.unpack_from(fmt,packet,offset)[0]


def decode_npc_metadata(packet):
    """Extract legacy NPC identity and position without guessing a 3D asset.

    NPC_SYNC is variable length. NPC setting indexes refer to the original
    SwordOnline world, not the 3D client's cha_list/excel IDs.
    """
    layout = SCHEMA['layouts']['NPC_SYNC']
    name_offset = layout['fields']['m_szName']['offset']
    if not name_offset <= len(packet) <= layout['size'] or packet[0] != 76:
        raise ValueError('Invalid NPC_SYNC packet')
    claimed_length = struct.unpack_from('<H', packet, 1)[0] + 1
    if len(packet) != claimed_length:
        raise ValueError('NPC_SYNC length mismatch')
    raw_name = packet[name_offset:].split(b'\0', 1)[0]
    packed_template = field(packet, 'NPC_SYNC', 'NpcSettingIdx')
    return {
        'id': field(packet, 'NPC_SYNC', 'ID'),
        'name': raw_name.decode('cp1258', errors='replace'),
        'legacy_template_id': (packed_template >> 16) & 0xffff,
        'legacy_level': packed_template & 0xffff,
        'legacy_position': [field(packet, 'NPC_SYNC', 'MapX', '<i'),
                            field(packet, 'NPC_SYNC', 'MapY', '<i')],
        'legacy_kind': field(packet, 'NPC_SYNC', 'm_btKind', '<B'),
        'model_mapping': 'unverified',
    }


def read_world_id(packet):
    """Return the map from a fully validated WORLD_SYNC packet."""
    if len(packet) != SCHEMA['layouts']['WORLD_SYNC']['size'] or packet[0] != 73:
        raise ValueError('Invalid WORLD_SYNC packet')
    return field(packet, 'WORLD_SYNC', 'SubWorld', '<i')


def decode_live_position(opcode, packet):
    """Read a server-authored NPC position using the matching PDB wire layout."""
    if opcode == 77:
        layout, id_key, x_key, y_key = 'NPC_NORMAL_SYNC', 'ID', 'MapX', 'MapY'
    elif opcode in (78, 145, 206):
        layout, id_key, x_key, y_key = ('NPC_PLAYER_TYPE_NORMAL_SYNC',
                                      'm_dwNpcID', 'm_dwMapX', 'm_dwMapY')
    else:
        raise ValueError(f'Not a live NPC position opcode: {opcode}')
    if len(packet) != SCHEMA['layouts'][layout]['size'] or packet[0] != opcode:
        raise ValueError(f'Invalid live position packet for opcode {opcode}')
    return (field(packet, layout, id_key), field(packet, layout, x_key, '<i'),
            field(packet, layout, y_key, '<i'))


class CoordinateTransform:
    """Invertible horizontal transform; height comes from the destination scene.

    The spawn anchor is verified against revivepos.ini and BeginPoint01. This
    establishes placement, not equivalent collision/portal topology: additional
    map landmarks must be surveyed before enabling authoritative movement.
    """
    def __init__(self, legacy_origin, scene_origin, scale=1/32, yaw=0):
        if not math.isfinite(scale) or scale <= 0:
            raise ValueError('Invalid map scale')
        self.legacy_origin=legacy_origin
        self.scene_origin=scene_origin
        self.scale=scale
        self.cos=math.cos(yaw)
        self.sin=math.sin(yaw)

    def to_scene(self,x,y):
        dx=(x-self.legacy_origin[0])*self.scale
        dz=(y-self.legacy_origin[1])*self.scale
        ox,oy,oz=self.scene_origin
        return (ox+self.cos*dx-self.sin*dz,oy,oz+self.sin*dx+self.cos*dz)

    def to_legacy(self,x,z):
        dx=x-self.scene_origin[0]
        dz=z-self.scene_origin[2]
        return (round(self.legacy_origin[0]+(self.cos*dx+self.sin*dz)/self.scale),
                round(self.legacy_origin[1]+(-self.sin*dx+self.cos*dz)/self.scale))


@lru_cache(maxsize=1)
def map_53_transform():
    markers=json.loads((REFERENCE/'world_baling.markers.json').read_text(encoding='utf-8'))
    spawn=next(p for p in markers['point'] if p['n']=='BeginPoint01')['v'][0]['pos']
    return CoordinateTransform((52032,101696),(spawn['x'],spawn['y'],spawn['z']))


def decode_initial_sync(batches):
    packets=[p for b in batches for p in split_batch(b)]
    worlds=[p for op,p in packets if op==73]
    players=[p for op,p in packets if op==68]
    ends=[p for op,p in packets if op==67]
    if len(worlds)!=1 or len(players)!=1 or len(ends)!=1:
        raise ValueError('Initial state must contain one world, player and sync-end')
    world,player=worlds[0],players[0]
    map_id=read_world_id(world)
    player_id=field(player,'CURPLAYER_SYNC','m_dwID')
    npcs=[p for op,p in packets if op==76 and field(p,'NPC_SYNC','ID')==player_id]
    if len(npcs)!=1:
        raise ValueError('Player position is missing or ambiguous')
    npc=npcs[0]
    name_offset=SCHEMA['layouts']['NPC_SYNC']['fields']['m_szName']['offset']
    name=npc[name_offset:].split(b'\0',1)[0].decode('cp1258')
    x=field(npc,'NPC_SYNC','MapX','<i')
    y=field(npc,'NPC_SYNC','MapY','<i')
    sex=field(player,'CURPLAYER_SYNC','m_btSex','<B')
    series=field(player,'CURPLAYER_SYNC','m_btSeries','<B')
    if sex not in (0,1) or series not in range(5):
        raise ValueError('Unsupported player appearance')
    if map_id!=53:
        raise ValueError(f'Map {map_id} has no verified scene mapping')
    position=map_53_transform().to_scene(x,y)
    # Retain AOI identities for future model/interaction mapping, but never
    # invent 3D NPC assets: NPC setting indexes are not 3D excel IDs.
    others = {}
    aoi_truncated = 0
    for packet in (p for op, p in packets if op == 76):
        # The player can use a shorter legacy NPC_SYNC than the full PDB
        # structure. Main-player coordinates were already decoded above.
        if field(packet, 'NPC_SYNC', 'ID') == player_id:
            continue
        try:
            entity = decode_npc_metadata(packet)
        except (ValueError, struct.error):
            # Optional NPC preview data must never block world entry.
            aoi_truncated += 1
            continue
        if not entity['name'] or entity['legacy_template_id'] <= 0:
            aoi_truncated += 1
            continue
        if len(others) >= 128 and entity['id'] not in others:
            aoi_truncated += 1
            continue
        entity['position'] = list(map_53_transform().to_scene(*entity['legacy_position']))
        others[entity['id']] = entity
    # cha_pic 1/2 are the client's standard male/female models, not legacy
    # equipment IDs. Keep equipment unmapped rather than inventing appearance.
    return {'legacy_map_id':map_id,'scene_id':1,'scene_asset':'world_baling',
            'coordinate_alignment':'spawn_anchor','movement_calibrated':False,
            'nearby_legacy_npcs':list(others.values()),'npc_models_mapped':False,
            'aoi_inventory_truncated':aoi_truncated,
            'player':{'id':player_id,'name':name,'sex':sex,'series':series,
                      'career_id':series+1,'model_id':sex+1,
                      'level':field(player,'CURPLAYER_SYNC','m_btLevel','<H'),
                      'legacy_position':[x,y],'position':list(position),
                      'life_max':field(player,'CURPLAYER_SYNC','m_wLifeMax'),
                      'mana_max':field(player,'CURPLAYER_SYNC','m_wManaMax'),
                      'stamina_max':field(player,'CURPLAYER_SYNC','m_wStaminaMax')}}
