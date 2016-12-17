#ifndef DENTRY
#define DENTRY
#define MAX_AVAILABLE 127			//最大偏移量
#define MIN_AVAILABLE 0				//最小偏移量
#define MAX_FILENAME 8				//最大文件名
#define DENTRY_EXIST 1				//目录项存在标记
#define DENTRY_NOT_EXIST 2			//目录项不存在标记
#define DENTRY_AVAILABLE 0			//目录项为空
#define OUT_OF_SPACE -1				//超出CD的范围
#define CD_BLOCK_FULL -2			//CD已满标记

typedef short d_address;
typedef unsigned int d_bitmap;
typedef struct {
	char d_filename[MAX_FILENAME];		//文件名
	d_address d_subdir;			//子目录偏移量
	d_address d_rsibling;			//兄弟文件/文件夹偏移量
	d_address d_parent;			//父目录偏移量
	short d_inode;			//i节点偏移量
}dentry,*Pdentry;				//目录项

typedef struct{
	dentry dentry_arr[MAX_AVAILABLE];	//一个块中所有目录项
	d_bitmap freeSpace;			//空闲块的位示图
	char padding[8];				//下一Critical Dir位置
	int id;				//CD块id
}critical_dir;

int _lookup(d_bitmap* map,int dentry_number);			//查看map中的dentry_number项是否可用
int _lookNextFreeSpace(d_bitmap *map);				//查找下一个空闲块
int _addDentry(d_bitmap* map,d_address* dAddr,short id);	//添加一个目录项
int _delDentry(d_bitmap* map,d_address dAddr);			//删除一个目录项


/*查看map中的dentry_number项是否可用
 *@param map: 此CD块的空闲块的位示图 
 *@param dentry_number: 此目录项在目录内的偏移量
 *return :返回当前偏移量所在的目录项是否为空，0为空，1为非空
 */
int _lookup(d_bitmap* map,int dentry_number){
	if(dentry_number>MAX_AVAILABLE || dentry_number<MIN_AVAILABLE){
		return OUT_OF_SPACE;
	}
	d_bitmap _map = *map;		
	_map = (_map>>dentry_number) & 0x1;
	return _map;
}

/*查找下一个空闲块
 *@param map: 此CD块的空闲块的位示图 
 *return : 空目录项在目录内的偏移量（从低地址开始），如果CD块空间满则返回CD_BLOCK_FULL
 */
int _lookNextFreeSpace(d_bitmap *map){
	d_bitmap _map = *map,_map_temp;
	int i;
	for(i=0;i<MAX_AVAILABLE;i++){
		_map_temp = (_map>>i)& 0x1;
		if(_map_temp==DENTRY_AVAILABLE){
			return i;
		}
	}
	return CD_BLOCK_FULL;
}

/*添加一个目录项
 *@param map: 此CD块的空闲块的位示图 
 *@param dAddr: 返回空的目录项指针
 *@param id: CD块的块号
 *return : 如果分配成功则返回0,否则返回CD_BLOCK_FULL
 */
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

/*删除一个目录项
 *@param map: 此CD块的空闲块的位示图 
 *@param dAddr: 要删除的目录项偏移量
 *return : 如果目录项在该块中找到就返回0,否则返回DENTRY_NOT_EXIST
 */
int _delDentry(d_bitmap* map,d_address dAddr){
	int _dentry_number = dAddr%MAX_AVAILABLE,_temp = 0x1;
	if(_lookup(map,_dentry_number)==DENTRY_EXIST){
		_temp = _temp<<_dentry_number;
		_temp = ~_temp;
		*map = *map & _temp;
		return 0;
	}
	return DENTRY_NOT_EXIST;
}

#endif
