#ifndef DIR_D
#define DIR_D
#include<stdio.h>
#include<string.h>

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

void InitCDBlocks(critical_dir* cdir);			//1.初始化新得到的cd块
Pdentry inline parseDArress(d_address dAddr);		//2.解析dAddr偏移量所在的dentry,如果偏移量不存在或者超出磁盘空间，则返回NULL			
d_address applyDentry();				//3.申请一项目录项
void _printSubdir(d_address dAddr,int depth);		//4.递归打印所有子目录（测试用）
Pdentry _mkfile_d(char* filepath);			//5.创建文件/文件夹
void _recurse_rm_d(d_address curAddr);			//6.递归删除curAddr下所有的文件/文件夹
Pdentry _rmfile_d(char* filepath);			//7.删除某一目录项,主要调用了上面的_mkfile_d()
d_address _scanParentDir_d(char* filepath,char* filename);//8.扫描获得父文件夹
int _checkFilepath(char* filepath);			//9.检查文件路径中是否包含非法字符
d_address _scan_d(char* filepath);			//10.扫描路径，如果正确则返回对应d_address,否则返回不同类型错误
Queue_d_address* _ls_d();				//11.展示当前目录下所有文件/文件夹的d_address,并存储在返回队列中

/*1.初始化新得到的cd块
 *@param cdir:  返回待初始化的cd块
 */
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

/*2.解析dAddr偏移量所在的dentry,如果偏移量不存在或者超出磁盘空间，则返回NULL
 *@param dAddr:待解析的目录项偏移量
 *return 如果正确解析则获得对应dAddr所在的目录项的指针，否则dAddr有错误或者未找到则返回NULL
 */
Pdentry inline parseDArress(d_address dAddr){
	short bid =dAddr/MAX_AVAILABLE;			//获得dAddr所在CD块的块号
	int i;
	if(dAddr == FILEPATH_ERROR || dAddr == FILEPATH_INVALID)
		return NULL;
	for(i=0;i<6;i++){/*此处在吴大师写好之后需要修改，因为有可能不在系统内存中*/
		if(normal_cd[i]->id==bid)
			return &normal_cd[i]->dentry_arr[dAddr%MAX_AVAILABLE];
	}
	return NULL;
}

/*3.申请一项目录项
 *return 目录项为空的目录项偏移量
 */
d_address applyDentry(){
	int i;
	d_address location;
	for(i=0;i<6;i++){/*此处在吴大师写好之后需要修改，因为有可能不在系统内存中*/
		if(_addDentry(normal_cd[i]->freeSpace,&location,i)!=CD_BLOCK_FULL){
			Pdentry pf = parse(location);
			printf("location:%d\t\n",location);
			return location;
		}
		printf("a block is full!\n");
	}
}

/*4.递归打印所有子目录
 *@param dAddr:
 *@param depth:dAddr所在层数，主要用于输出TAB
 */
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

/*5.创建文件/文件夹
 *@param filepath:待创建的文件路径
 *return 如果成功则返回待创建文件/的目录项
 */
Pdentry _mkfile_d(char* filepath){	
	char* filename =(char*) malloc(MAX_FILENAME*sizeof(char));
	d_address parentAddr = _scanParentDir_d(filepath,filename);/*此处得获得吴大师的文件属性字段*/
	Pdentry ps,pls;
	if(parentAddr == FILEPATH_ERROR || parentAddr == FILEPATH_INVALID){
		printf("There is something matter with file name !\n Plese check again!\n");
		return NULL;
	}
	dentry_each_child(parentAddr,ps,pls){	//检查是否有重名的情况
		if(!strcmp(ps->d_filename,filename)){
			printf("无法创建目录：%s; 文件已存在\n",filename);
			return NULL;
		}
	}
	Pdentry pf = parse(parentAddr);
	d_address new_dir = applyDentry();
	Pdentry psubdir = parse(new_dir);
	strcpy(psubdir->d_filename,filename);
	psubdir->d_parent = parentAddr;
	psubdir->d_rsibling = pf->d_subdir;
	psubdir->d_subdir = EOF;
	pf->d_subdir = new_dir;  
	return psubdir;
}

/*6.递归删除curAddr下所有的文件/文件夹
 *@param curAddr:待删除的文件/文件夹的目录项偏移量
 */
void _recurse_rm_d(d_address curAddr){
	d_address sAddr;
	Pdentry ps,pls;
	Queue_d_address* queue = (Queue_d_address*)malloc(sizeof(Queue_d_address));
	InitQueue(queue);
	int id,i;
	//将子目录压入队列
	dentry_each_child(curAddr,ps,pls){
		sAddr = pls->d_rsibling;
		if(pls == parse(curAddr))	
			Enqueue(queue,&(pls->d_subdir));
		else
			Enqueue(queue,&(pls->d_rsibling));
	}
	
	while(IsQueueEmpty(queue)==0){
		Dequeue(queue,&sAddr);
		_recurse_rm_d(sAddr);
		id = sAddr/MAX_AVAILABLE;
		_delDentry(normal_cd[id]->freeSpace,sAddr);
		memset(normal_cd[id]->dentry_arr[sAddr%MAX_AVAILABLE].d_filename,0,MAX_FILENAME*sizeof(char));
       		memset(&(normal_cd[id]->dentry_arr[sAddr%MAX_AVAILABLE].d_subdir),EOF,4*sizeof(d_address));
	}	
	id = curAddr/MAX_AVAILABLE;
	_delDentry(normal_cd[id]->freeSpace,curAddr);
	memset(normal_cd[id]->dentry_arr[curAddr%MAX_AVAILABLE].d_filename,0,MAX_FILENAME*sizeof(char));
        memset(&(normal_cd[id]->dentry_arr[curAddr%MAX_AVAILABLE].d_subdir),EOF,4*sizeof(d_address));
}

/*7.删除某一目录项,主要调用了上面的_mkfile_d()
 *@param filepath:待删除文件路径
 */
Pdentry _rmfile_d(char* filepath){
	d_address curAddr = _scan_d(filepath);	
	if(curAddr == FILEPATH_ERROR || curAddr == FILEPATH_INVALID){
		printf("There is something matter with file name !\n Plese check again!\n");
		return NULL;
	}
	Pdentry pf = parse(curAddr);
	Pdentry ps,pls;
	
	d_address parentAddr;
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
	_recurse_rm_d(curAddr);
}

/*8.扫描获得父文件夹
 *param:char* filepath	待扫描路径
 *param:char* filename	返回扫描得到的文件名
 *return :父文件夹目录项偏移量，如果扫描不正确则返回FILEPATH_INVALID或者FILEPATH_ERROR
 */
d_address _scanParentDir_d(char* filepath,char* filename){
	char* _filename,*_parentPath;
	size_t parentLength;
	_parentPath = (char*)malloc(strlen(filepath)*sizeof(char));
	if(_checkFilepath(filepath) == FILEPATH_INVALID){	//文件名非法
		return FILEPATH_INVALID;
	}
	if((strrchr(filepath,'/'))!=NULL){			
		_filename = strrchr(filepath,'/');
		_filename++;
		strcpy(filename,_filename);
//		printf("filename:%s\t",_filename);
//		printf("filepath:%s\t",filepath);
		parentLength = (_filename-filepath)-1;
		strncpy(_parentPath,filepath,(parentLength != 0?parentLength:1));	//判断是否在根目录下
//		printf("parentPath:%s\n",_parentPath);
		return _scan_d(_parentPath);
	}else{
		strcpy(filename,filepath);
		return current_address;				//如果是相对路径，且父目录为当前路径，则返回当前路径
	}
}

/*9.检查文件路径中是否包含非法字符
 *@param filepath:待扫描的文件路径
 *return : 如果路径中不包含非法字符则返回0,否则返回FILEPATH_INVALID
 */
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

/*10.扫描路径，如果正确则返回对应d_address,否则返回不同类型错误
 *@param filepath:待扫描的路径
 *return :如果文件找到则返回对应的目录项偏移量，如果未找到或者路径不合法则返回FILEPATH_INVALID或者FILEPATH_ERROR
 */
d_address _scan_d(char* filepath){
	char* _filepath = malloc((strlen(filepath)+1)*sizeof(char));
	char* _filename;
	int catch_flag = 0;
	d_address dAddr;
	Pdentry ps,pls;
	strcpy(_filepath,filepath);
	if(_checkFilepath(_filepath)==FILEPATH_INVALID){
		return FILEPATH_INVALID;
	}
	if(_filepath[0] == '/')					//绝对路径
		dAddr = 0;
	else							//相对路径
		dAddr = current_address;
	_filename = strtok(_filepath,"/");
	while(_filename!=NULL){
		catch_flag = 0;					//命中标记初始化
		if(!strcmp(_filename,"..")){			//".."返回上一级
			if(dAddr != 0){
				dAddr = parse(dAddr)->d_parent;
				printf("..\n");
			}
			catch_flag = 1;				//文件名命中
		}else if(!strcmp(_filename,".")){		//"."指向指向当前路径
			dAddr = current_address;
			catch_flag = 1;				//文件名命中
			printf(".\n");
		}
		dentry_each_child(dAddr,ps,pls){
			if(!strcmp(ps->d_filename,_filename)){	//如果跟其目录内的某个文件名相同
				if(pls == parse(dAddr))
					dAddr = pls->d_subdir;	
				else{
					dAddr = pls->d_rsibling;
				}
				catch_flag = 1;			//文件名命中
				break;
			}
		}
		if(!catch_flag)					//如果在一次遍历中没有找到对应的文件/文件夹则返回路径输入有误
			return 	FILEPATH_ERROR;			
		_filename = strtok(NULL,"/");		
	}
		return dAddr;
}

/*11.展示当前目录下所有文件/文件夹的d_address,并存储在返回队列中
 *return :返回由当前文件夹下所有文件/文件夹组成的队列
 */
Queue_d_address* _ls_d(){
    Pdentry ps,pls;
    Queue_d_address* queue =(Queue_d_address*) malloc(sizeof(Queue_d_address));
    InitQueue(queue);
    dentry_each_child(current_address,ps,pls){
        if(pls == parse(current_address)){
            Enqueue(queue,&pls->d_subdir);
        }else{
            Enqueue(queue,&pls->d_rsibling);
        }        
    }
    return queue;
}

#endif
