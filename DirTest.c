#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<time.h>
#include"dentry.h"
#include"Queue_d_address.h"
#define FILEPATH_INVALID -2			//路径包含非法字符
#define FILEPATH_ERROR -1			//路径出错，有文件/文件夹不存在的情况
#define OUT_OF_ADDRESS NULL			//超出了dentry的范围
#define parse(address) parseDArress(address)
#define dentry_each_child(parent_address,pdentry_child,pdentry_lsibling) Pdentry __pf=parse(parent_address);d_address __childAddr=__pf->d_subdir;\
for(pdentry_lsibling = parse(parent_address),pdentry_child = parse(__childAddr);__childAddr!=EOF;pdentry_lsibling = pdentry_child,__childAddr=pdentry_child->d_rsibling,pdentry_child = __childAddr!=EOF?parse(__childAddr):NULL)	//这个宏的意思是将父节点的d_address读入，还有两个指针用来接收指向孩子的遍历指针pdentry_child，以及在初始化时指向父节点pdentry_lsibling，后来这个指针用来标识当前正在读入节点的d_address
typedef struct{
	char B[2*1024];
}PhysicalBlock;

typedef struct{
	PhysicalBlock pb[1024*8];
	int located;
}PhysicalBlocks;


critical_dir* my_dir,* normal_cd[6];
PhysicalBlocks* vf;
d_address current_address;

void* getPhysicalBlock();
void InitCDBlocks(critical_dir* cdir);			//初始化新得到的cd块
Pdentry inline parseDArress(d_address dAddr);		//解析dAddr偏移量所在的dentry,如果偏移量不存在或者超出磁盘空间，则返回NULL			
d_address applyDentry();
void _printSubdir(d_address dAddr,int depth);
Pdentry _mkdir_d(d_address curAddr,char* filename);
Pdentry _rmdir_d(d_address curAddr);
void Initial();					
d_address _scan_d(char* filepath);


//初始化新得到的cd块
void InitCDBlocks(critical_dir* cdir){
	int j;
	for(j=0;j<MAX_AVAILABLE;j++){
               	memset(cdir->dentry_arr[j].d_filename,0,MAX_FILENAME*sizeof(char));
               	memset(&(cdir->dentry_arr[j].d_subdir),EOF,4*sizeof(d_address));	//把d_address设置成EOF，即对应没有相邻的节点。
        }
	cdir->freeSpace =(d_bitmap*) malloc(sizeof(d_bitmap));
	*(cdir->freeSpace) = 0x0;				//位示图初始化
        cdir->nextCD = EOF;
}

//解析dAddr偏移量所在的dentry,如果偏移量不存在或者超出磁盘空间，则返回NULL
Pdentry inline parseDArress(d_address dAddr){
	short bid =dAddr/MAX_AVAILABLE;			//获得dAddr所在CD块的块号
	int i=0;
	if(dAddr == FILEPATH_ERROR || dAddr == FILEPATH_INVALID)
		return NULL;
	for(;i<6;i++){/*此处在吴大师写好之后需要修改，因为有可能不在系统内存中*/
		if(normal_cd[i]->id==bid)
			return &normal_cd[i]->dentry_arr[dAddr%MAX_AVAILABLE];
	}
	return NULL;
}

//申请一项目录项
d_address applyDentry(){
	int i;
	d_address location;
	for(i=0;i<6;i++){/*此处在吴大师写好之后需要修改，因为有可能不在系统内存中*/
		if(_addDentry(normal_cd[i]->freeSpace,&location,i)!=CD_BLOCK_FULL){
			printf("location:%d\n",location);
			return location;
		}
		printf("a block is full!\n");
	}
}

//打印所有子目录
void _printSubdir(d_address dAddr,int depth){
	Pdentry pf = parse(dAddr);
	int i;
	d_address sAddr;
	Queue_d_address* queue = malloc(sizeof(Queue_d_address));	//队列用于打印树状结构
	InitQueue(queue);
	Pdentry ps,pls;
	for(i=0;i<depth;i++)printf("\t");
	printf("filename:%s parent:%d rsibling:%d subdir:%d\n",pf->d_filename,pf->d_parent,pf->d_rsibling,pf->d_subdir);
	dentry_each_child(dAddr,ps,pls){
		for(i=0;i<depth+1;i++)printf("\t");
		printf("filename:%s parent:%d rsibling:%d subdir:%d\n",ps->d_filename,ps->d_parent,ps->d_rsibling,ps->d_subdir);
		if(pls == parse(dAddr))					//如果是父节点就把子目录压入	
			Enqueue(queue,&(pls->d_subdir));
		else							//如果已经是子节点了，就把其右兄弟压入
			Enqueue(queue,&(pls->d_rsibling));
	}
	printf("%080d\n",0);
	while(IsQueueEmpty(queue)==0){					//递归解析
		Dequeue(queue,&sAddr);
		_printSubdir(sAddr,depth+1);
	}
}


//创建子目录
Pdentry _mkdir_d(d_address curAddr,char* filename){
	Pdentry pf = parse(curAddr);
	d_address new_dir = applyDentry();
	Pdentry ps = parse(new_dir);
	strcpy(ps->d_filename,filename);
	ps->d_parent = curAddr;
	ps->d_rsibling = pf->d_subdir;
	ps->d_subdir = EOF;
	pf->d_subdir = new_dir;  
	return ps;
}

//删除某一目录项
Pdentry _rmdir_d(d_address curAddr){
		
	Pdentry pf = parse(curAddr);
	Pdentry ps,pls;
	Queue_d_address* queue = (Queue_d_address*)malloc(sizeof(Queue_d_address));
	InitQueue(queue);
	int id,i;
	d_address sAddr,parentAddr;
	parentAddr = pf->d_parent;
	//将左边的兄弟或者父亲连接到右边的兄弟
	{
		dentry_each_child(parentAddr,ps,pls){
			if(pls == parse(parentAddr) && pls->d_subdir == curAddr){	
				pls->d_subdir = ps->d_rsibling;
				break;
			}
			else if(pls->d_rsibling == curAddr){
				pls->d_rsibling = ps->d_rsibling;
				break;
			}
		}
	}
	//将子目录压入队列
	{
		dentry_each_child(curAddr,ps,pls){
			sAddr = pls->d_rsibling;
			if(pls == parse(curAddr))	
				Enqueue(queue,&(pls->d_subdir));
			else
				Enqueue(queue,&(pls->d_rsibling));
		}
	}
	
	while(IsQueueEmpty(queue)==0){
		Dequeue(queue,&sAddr);
		id = sAddr/MAX_AVAILABLE;
		_delDentry(normal_cd[id]->freeSpace,sAddr);
		memset(normal_cd[i]->dentry_arr[sAddr%MAX_AVAILABLE].d_filename,0,MAX_FILENAME*sizeof(char));
        memset(&(normal_cd[i]->dentry_arr[sAddr%MAX_AVAILABLE].d_subdir),EOF,4*sizeof(d_address));
	}	
	id = curAddr/MAX_AVAILABLE;
	_delDentry(normal_cd[id]->freeSpace,curAddr);
	memset(normal_cd[i]->dentry_arr[curAddr%MAX_AVAILABLE].d_filename,0,MAX_FILENAME*sizeof(char));
        memset(&(normal_cd[i]->dentry_arr[curAddr%MAX_AVAILABLE].d_subdir),EOF,4*sizeof(d_address));
}


int _checkFilepath(char* filepath){
	char c;
	int i;
	for(i=0;i<strlen(filepath);i++){
		c = filepath[i];
		if((c>='-'&&c<=57) || (c>=65&&c<=90 || (c>=97&&c<=122)))
			continue;
		else
			return FILEPATH_INVALID;
	}
	return 0;
}

//扫描路径，如果正确则返回对应d_address,否则返回不同类型错误
d_address _scan_d(char* filepath){
	char* _filepath = malloc((strlen(filepath)+1)*sizeof(char));
	char* _filename;
	int catch_flag = 0;
	d_address dAddr;
	Pdentry ps,pls;
	strcpy(_filepath,filepath);
	if(_checkFilepath(_filepath)!=FILEPATH_INVALID){
		if(_filepath[0] == '/')
			dAddr = 0;
		else
			dAddr = current_address;
		_filename = strtok(_filepath,"/");
		while(_filename!=NULL){
			catch_flag = 0;
			//printf("%s\n",_filename);
			dentry_each_child(dAddr,ps,pls){
				if(!strcmp(ps->d_filename,_filename)){
					if(pls == parse(dAddr))
						dAddr = pls->d_subdir;	
					else{
						dAddr = pls->d_rsibling;
					}
					catch_flag = 1;
					break;
				}
			}
			if(!catch_flag)		
				return 	FILEPATH_ERROR;			
			_filename = strtok(NULL,"/");		
		}
		return dAddr;
	}else{
		return FILEPATH_INVALID;
	}
}



//获得物理块
void* getPhysicalBlock(){
	return &vf->pb[vf->located++];
}

//初始化操作
void Initial(){
	int i;
	for(i=0;i<6;i++){
		normal_cd[i] = (critical_dir*)getPhysicalBlock();
		InitCDBlocks(normal_cd[i]);
	}
	d_address root = applyDentry();
	Pdentry pf = parse(root),ps;
	strcpy(pf->d_filename,"/");
	_mkdir_d(root,"usr");
	_mkdir_d(root,"src");
	_mkdir_d(root,"local");
	_mkdir_d(pf->d_subdir,"tony");
	current_address = 0;
//	_rmdir_d(1);
	_printSubdir(root,0);
	printf("%s\n",_scan_d("local/tony.c") == FILEPATH_ERROR?"yes":"no");
}

int main(){
	vf = (PhysicalBlocks*)malloc(sizeof(PhysicalBlocks));
	vf->located = 0;
	my_dir = malloc(sizeof(critical_dir));
	Initial();
	InitCDBlocks(my_dir);
	printf("%.6f\n",(double)clock()/CLOCKS_PER_SEC);
}
