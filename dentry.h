#ifndef DENTRY
#define DENTRY
#define d_address short
#define d_bitmap int
#define CD_NUMBER 6
#define MAX_AVAILABLE 127
#define MAX_FILENAME 8
#define ENTRY_EXIST 1
#define ENTRY_NOT_EXIST 2
#define ENTRY_AVAILABLE 0
#define OUT_OF_SPACE -1
#define CD_BLOCK_FULL -2
typedef struct {
	char d_filename[MAX_FILENAME];		//文件名
	d_address d_subdir;			//子目录偏移量
	d_address d_rsibling;			//兄弟文件/文件夹偏移量
	d_address d_parent;			//父目录偏移量
	d_address d_inode;			//i节点偏移量
}dentry,*Pdentry;				//目录项

typedef struct{
	dentry dentry_arr[MAX_AVAILABLE];	//一个块中所有目录项
	d_bitmap* freeSpace;			//位示图
	short nextCD;				//下一Critical Dir位置
	short id;				//CD块id
}critical_dir;

int _lookup(d_bitmap* map,int dentry_number);			//查看map中的dentry_number项是否可用
int _lookNextFreeSpace(d_bitmap *map);				//查找下一个空闲块
int _addDentry(d_bitmap* map,d_address* dAddr,short id);	//添加一个目录项
int _delDentry(d_bitmap* map,d_address dAddr);			//删除一个目录项


//查看map中的dentry_number项是否可用
int _lookup(d_bitmap* map,int dentry_number){
	if(dentry_number>MAX_AVAILABLE){
		return OUT_OF_SPACE;
	}
	d_bitmap _map = *map;		
	_map = (_map>>dentry_number) && 0x1;
	return _map;
}

//查找下一个空闲块
int _lookNextFreeSpace(d_bitmap *map){
	d_bitmap _map = *map,_map_temp;
	int i;
	for(i=0;i<MAX_AVAILABLE;i++){
		_map_temp = (_map>>i)&& 0x1;
		if(_map_temp==ENTRY_AVAILABLE){
			return i;
		}
	}
	return CD_BLOCK_FULL;
}

//添加一个目录项
int _addDentry(d_bitmap* map,d_address* dAddr,short id){
	int _dentry_number,_temp = 0x1;
	if((_dentry_number = _lookNextFreeSpace(map))!=CD_BLOCK_FULL){
		_temp = _temp<<_dentry_number;
		*map = *map|_temp;
		*dAddr = _dentry_number + id*MAX_AVAILABLE;
		return 0;
	}
	return CD_BLOCK_FULL;
}

//删除一个目录项
int _delDentry(d_bitmap* map,d_address dAddr){
	int _dentry_number = dAddr%MAX_AVAILABLE,_temp = 0x1;
	if(_lookup(map,_dentry_number)==ENTRY_EXIST){
		_temp = _temp<<_dentry_number;
		_temp |= _temp;
		*map = *map && _temp;
		return 0;
	}
	return ENTRY_NOT_EXIST;
}

#endif
