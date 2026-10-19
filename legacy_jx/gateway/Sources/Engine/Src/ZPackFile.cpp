#include "KWin32.h"
#include "ZPackFile.h"
#include "ucl/ucl.h"

ZMapFile::ZMapFile(const char *file_name) {
	m_Ptr = 0;
	old_offset = 0;
	// the length is only ever measured from an open handle, and the new ceiling in map()
	// reads it whether or not the open succeeded: an unopenable pack would be weighed
	// against whatever the allocation happened to hold.  Zero is the one value no window
	// can be inside, so every request against a file that is not there is refused.
	m_Size = 0;
	old_size = 0;
	m_hMap = 0;
	m_hFile = CreateFile(file_name, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(m_hFile != INVALID_HANDLE_VALUE) {
		m_Size = GetFileSize(m_hFile, NULL);
		if(m_Size) {
			m_hMap = CreateFileMapping(m_hFile, NULL, PAGE_READONLY, 0, m_Size, NULL);
			if(m_hMap) return;
		}
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
}

char *ZMapFile::map(unsigned long offset, unsigned long size) {
	unsigned long align = offset % 0x10000;									//64K边界
	if(m_Ptr) {
		// this answers from the window that was granted, not the one that was asked for:
		// MINIMIZE_MAP_SIZE is nominal, and a view taken near the end of a pack is shorter,
		// so a request judged inside the nominal window can sit past the end of the mapping.
		if(offset > old_offset && (offset + size < old_offset + old_size)) {
			return m_Ptr + offset - old_offset;
		}
		UnmapViewOfFile(m_Ptr);
		// the pointer dangles the moment the view is released: left set, the next call
		// takes the fast path above and returns an address inside a view that is gone.
		m_Ptr = 0;
		old_size = 0;
	}
	// the pack has to carry the window before it is asked for.  `m_Size' is the file's
	// own measured length and an `unsigned long' at both widths, so this ceiling is
	// shared by the x86 and the x64 build; what it refuses here is a record that points
	// outside the pack, which used to be mapped anyway and read as though it were there.
	if(offset >= m_Size || size > m_Size - offset) return NULL;
	// the view starts at the 64K boundary below the request, so it must leave room for
	// those bytes too, and a request one window cannot span is refused rather than
	// shortened behind the caller's back - which is what made a truncated read invisible.
	if(size > MINIMIZE_MAP_SIZE - align) return NULL;
	old_offset = offset - align;
	size = MINIMIZE_MAP_SIZE;
	// what the file grants may be less than the window asked for, and the granted length
	// is the number the fast path above has to measure against, so it is kept.
	if(old_offset + size > m_Size) size = m_Size - old_offset;
	old_size = size;
	m_Ptr = (char *)MapViewOfFile(m_hMap, FILE_MAP_READ, 0, old_offset, size);		
	if(m_Ptr) return m_Ptr + align;					//成功了！
	else return NULL;
}

void ZMapFile::unmap() {
	if(!m_Ptr) return;
	UnmapViewOfFile(m_Ptr);
	m_Ptr = 0;
}

ZMapFile::~ZMapFile() {
	if(m_Ptr) UnmapViewOfFile(m_Ptr);
	if(m_hMap) CloseHandle(m_hMap);
	if(m_hFile != INVALID_HANDLE_VALUE) CloseHandle(m_hFile);
}

ZCache::ZCache(long size) {
	long align = size % 0x10000;				//必须是64k对齐的，不然会降低效率
	if(align) size += 0x10000 - align;
	buffer = new char[size];
	cache_size = size;
	free_items = (item_info *)buffer;
	free_items->offset = 0;
	free_items->size = cache_size;
	free_items->index_high = 0;
	memset((void *)&last_item[0], 0, LAST_ITEM * sizeof(item_info));
	last = 0;
}

ZCache::~ZCache() 
{
	if(buffer) 
	{
	   delete[] buffer;
	   buffer=NULL;
	}
}

//进行优化,记录最近使用的5项

char *ZCache::getNode(unsigned long index_high, unsigned long index_low, long size) {			//得到一个空闲缓冲区数据块
	size += sizeof(item_info);
	unsigned long align = size % MINIMIZE_BLOCK_SIZE;
	if(align) size += MINIMIZE_BLOCK_SIZE - align;
	
	if(size > cache_size) return 0;
	m_Mutex.Lock();
	item_info *next;
	while(free_items->size < size) {										//删除后面的项目直到有足够的空间为止
		next = (item_info *)((char *)free_items + free_items->size);		//下一个节点
		if((char *)next - buffer >= cache_size) {							//已经到了结束的位置，回第一个
			free_items = (item_info *)buffer;
			free_items->index_high = 0;
		}
		else free_items->size += next->size;
	}

	free_items->index_high = index_high;
	free_items->index_low = index_low;
	next = (item_info *)((char *)free_items + size);
	if((char *)next - buffer == cache_size) next = (item_info *)buffer;
	else if(free_items->size > size) {
		next->index_high = 0;
		next->size = free_items->size - size;
		next->offset = free_items->offset + size;
	}

	free_items->size = size;
	last_item[last].index_high = index_high;
	last_item[last].index_low = index_low;
	last_item[last].size = size;
	last_item[last].offset = free_items->offset;
	if(++last == LAST_ITEM) last = 0;
	char *result = buffer + free_items->offset + sizeof(item_info);
	free_items = next;
	m_Mutex.Unlock();
	return result;
}

char *ZCache::searchNode(unsigned long index_high, unsigned long index_low) {
	m_Mutex.Lock();
	for(int index = 0; index < LAST_ITEM; index++) {
		if(last_item[index].index_high == index_high && last_item[index].index_low == index_low) {
			m_Mutex.Unlock();
			return buffer + last_item[index].offset + sizeof(item_info);
		}
	}
	item_info *current = (item_info *)buffer;
	while((char *)current - buffer < cache_size) {
		if(current->index_high == index_high && current->index_low == index_low) {
			last_item[last].index_high = index_high;
			last_item[last].index_low = index_low;
			last_item[last].size = current->size;
			last_item[last].offset = current->offset;
			if(++last == LAST_ITEM) last = 0;
			m_Mutex.Unlock();
			return buffer + current->offset + sizeof(item_info);
		}
		current = (item_info *)((char *)current + current->size);
	}
	m_Mutex.Unlock();
	return 0;
}

int ZPackFile::getNodeIndex(unsigned long id) {								//二分法找到指定的索引
	int nBegin, nEnd, nMid;
	nBegin = 0;
	nEnd = header.count;
	while (nBegin <= nEnd) {
		nMid = (nBegin + nEnd) / 2;
		if (id == index_list[nMid].id) break;
		if (id < index_list[nMid].id) nEnd = nMid - 1;
		else nBegin = nMid + 1;
	}
	if(id != index_list[nMid].id) return -1;								//数据文件里面也没有
	return nMid;
}

unsigned long ZPackFile::getSize(unsigned long index) {
	return index_list[index].size;
}

bool ZPackFile::_readData(int node_index, char *node) {
	// two different fields of the same record are in play here: this line sized the
	// window by `size' while the decompression below reads `compress_size' bytes out of
	// it, so a pack that names a larger block than the node it stores reads past the end
	// of the view.  The window is sized by whichever the read consumes.
	unsigned long ulWindow = (unsigned long)index_list[node_index].size;
	unsigned long ulPacked = (unsigned long)index_list[node_index].compress_size & 0x00FFFFFF;
	if(ulPacked > ulWindow) ulWindow = ulPacked;
	char *source = data_map->map(index_list[node_index].offset, ulWindow);
	if(!source) {
		return false;
	}
	unsigned int dest_length = index_list[node_index].size;  //ucl safe entry reads *dst_len as the destination room, writes only up to it
	unsigned long compress_type = index_list[node_index].compress_size >> 24;
	int r = 1;
	if((compress_type == TYPE_UCL) | (compress_type == TYPE_BZIP2)) {									//整体压缩
		if(compress_type == TYPE_UCL) {
			r = ucl_nrv2b_decompress_safe_8((BYTE *)source, index_list[node_index].compress_size & 0x00FFFFFF, (BYTE *)node, &dest_length, NULL);
		}
		else if(compress_type == TYPE_BZIP2) {
//			dest_length = index_list[node_index].size;
//			r = BZ2_bzBuffToBuffDecompress(node + sizeof(id), &dest_length, source, index_list[node_index].compress_size & 0x00FFFFFF, 0, 0);
		}
	}
	if(!r) return true;
	return false;
}

unsigned long hash(const char *file_name) {
	unsigned long id = 0;
	const char *ptr = file_name;
	int index = 0;
	while(*ptr) {
		if(*ptr >= 'A' && *ptr <= 'Z') id = (id + (++index) * (*ptr + 'a' - 'A')) % 0x8000000b * 0xffffffef;
		else id = (id + (++index) * (*ptr)) % 0x8000000b * 0xffffffef;
		ptr++;
	}
	return (id ^ 0x12345678);
}

ZPackFile::ZPackFile(const char *file_name, ZCache *the_cache) {
	data_map = 0;
	index_list = 0;
	opened = false;
	cache = the_cache;
	data_map = new ZMapFile(file_name);
	char *header_ptr = data_map->map(0, sizeof(header));
	if(!header_ptr) return;
	memcpy(&header, header_ptr, sizeof(header));
	// `header.count' and `header.index_offset' are numbers the file wrote, and nothing
	// above asks the file whether it has them.  The bound is divided out of the room in a
	// 64-bit type on purpose: `header.count * sizeof(index_info)' measures 8 bytes at
	// x64 and 4 at x86, while map() takes an `unsigned long' at both, so a count large
	// enough to matter is a wrap in one build and a narrowing in the other - and either
	// way the table copied below is shorter than the count that walks it.
	if(header.count == 0 || header.index_offset >= data_map->m_Size) return;
	unsigned long long ulSpan =
		(unsigned long long)header.count * sizeof(index_info);
	if(ulSpan > (unsigned long long)(data_map->m_Size - header.index_offset)) return;
	if(ulSpan > (unsigned long long)0xFFFFFFFF) return;
	index_list = new index_info[header.count];
	char *index_ptr = data_map->map(header.index_offset, (unsigned long)ulSpan);
	// the answer of the map is the window itself, so a refusal is read here instead of
	// being copied through as a pointer.
	if(!index_ptr) return;
	memcpy(&index_list[0], index_ptr, (size_t)ulSpan);
	opened = true;
}

ZPackFile::~ZPackFile() {
	if(index_list)
	{
		delete[] index_list;
		index_list=NULL;
	}
	if(data_map) 
	{
		delete data_map;
		data_map=NULL;
	}
}

char *ZPackFile::getData(unsigned long id) {
	char * node = cache->searchNode(id);
	if(node) return node;
	int node_index = getNodeIndex(id);
	if(node_index == -1) return 0;
	node = cache->getNode(id, -1, index_list[node_index].size);
	if(_readData(node_index, node)) return node;
	else return 0;
}

char *ZPackFile::getData(const char *name) {											//获取指定的文件数据
	unsigned long id = hash(name);
	return getData(id);
}
