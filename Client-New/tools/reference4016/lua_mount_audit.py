"""Read-only Win32 JX1 PACK/source availability; never executes Lua or edits GS.

Layout and ASCII path hash follow Engine/Src/XPackFile.h and KPakList.cpp.
An ID match is a candidate, not proof that a function compiled or ran.
"""
import argparse
import configparser
import hashlib
import json
import struct
from pathlib import Path

TARGETS = (r'\script\system\online.lua',
           r'\script\system\player\playerlogin.lua',
           r'\script\system\player\playerlogout.lua')


def file_id(path):
    value = 0
    for i, byte in enumerate(path.lower().encode('ascii'), 1):
        value = (((value + i*byte) & 0xffffffff) % 0x8000000b * 0xffffffef) & 0xffffffff
    return value ^ 0x12345678


def pack_ids(blob):
    if len(blob) < 32 or blob[:4] != b'PACK':
        raise ValueError('Invalid PACK header')
    count, index, data = struct.unpack_from('<III', blob, 4)
    if count > 1000000 or not 32 <= data <= index <= len(blob) or index+count*16 > len(blob):
        raise ValueError('PACK index outside file')
    ids = set()
    for entry_id, offset, size, flags in struct.iter_unpack('<IIII', blob[index:index+count*16]):
        stored = flags & 0xffffff
        if not data <= offset <= index or offset+stored > index or size > 0x7fffffff:
            raise ValueError('PACK entry outside data')
        if entry_id in ids:
            raise ValueError('Duplicate PACK ID')
        ids.add(entry_id)
    return ids


def inside(root, relative):
    path = root.joinpath(relative.lstrip('\\/').replace('\\', '/')).resolve()
    if not path.is_relative_to(root):
        raise ValueError('Path escapes audit root')
    return path


def audit(root, targets=TARGETS):
    root = Path(root).resolve()
    config = configparser.ConfigParser(interpolation=None)
    config.read(root/'package.ini', encoding='utf-8-sig')
    section = config['Package'] if config.has_section('Package') else {}
    pack_root = inside(root, section.get('Path', r'\pak'))
    mounted = {inside(pack_root, value).name.lower() for key, value in section.items() if key.isdigit()}
    archives, errors, ids = [], [], {}
    for path in sorted(pack_root.glob('*')):
        if not path.is_file():
            continue
        blob = path.read_bytes()
        try:
            entries = pack_ids(blob)
        except ValueError as error:
            errors.append({'file': path.name, 'error': str(error)})
            continue
        ids[path.name] = entries
        archives.append({'file': path.name, 'entries': len(entries),
                         'mounted_by_config': path.name.lower() in mounted,
                         'sha256': hashlib.sha256(blob).hexdigest()})
    scripts = []
    for target in targets:
        key = file_id(target)
        candidates = [name for name, entries in ids.items() if key in entries]
        active = [name for name in candidates if name.lower() in mounted]
        loose = inside(root, target).is_file()
        scripts.append({'path': target, 'file_id': f'{key:08x}', 'loose_exists': loose,
                        'archive_candidates': candidates, 'mounted_candidates': active,
                        'source_available': bool(loose or active)})
    return {'schema': 1, 'archives': archives, 'archive_errors': errors, 'scripts': scripts,
            'limits': 'Configured disk mounts only, not live process mount/cache proof. ASCII target paths. ID matches do not prove Lua hook compilation/execution.'}


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('root', type=Path)
    parser.add_argument('--output', type=Path)
    args = parser.parse_args()
    report = audit(args.root)
    output = json.dumps(report, indent=2)
    if args.output:
        args.output.write_text(output+'\n', encoding='utf-8')
    print(output)
