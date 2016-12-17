#include<unistd.h>
#include "dir_d.h"
#include<time.h>
typedef struct{
	char B[2*1024];
}PhysicalBlock;

typedef struct{
	PhysicalBlock pb[1024*8];
	int located;
}PhysicalBlocks;


void* getPhysicalBlock();
void Initial();

PhysicalBlocks* vf;
critical_dir* my_dir;
//获得物理块
void* getPhysicalBlock(){
	return &vf->pb[vf->located++];
}

FCB* findFCB(short inode);
     readfile(BUF* buf,FCB* fcb);

void my_ls(){
	Queue_d_address* queue = _ls_d();
	DoubleLinked double_list = createDLList();
	d_address Addr;
	while(!IsQueueEmpty(queue)){
		Dequeue(queue,&Addr);
		list_add(double_list,parse(Addr)->d_filename);
	}
}


//初始化操作
void Initial(){
	int i;
	for(i=0;i<6;i++){
		normal_cd[i] = (critical_dir*)getPhysicalBlock();
		InitCDBlocks(normal_cd[i]);
	}
//////////////////////////////////
	FCB* root_fcb = findFCB(0);
	char* initBlock = malloc(root_fcb->len);
	readfile(initBlock,root_fcb);
	normal_cd =(critical_dir*)initBlock;
	cd_number = root_fcb->len/2048;


	printf("cd:%d\n",sizeof(critical_dir));
	d_address root = _applyDentry_d();
	Pdentry pf = parse(root),ps;
	strcpy(pf->d_filename,"/");
	_mkfile_d("/usr");
	_mkfile_d("/src");
	_mkfile_d("/local");
	current_address = 3;
	_mkfile_d("/local/tony");
	_mkfile_d("/local/tony/yes");
	_rmfile_d("/src");
	_mkfile_d("/src");
	Queue_d_address* queue = _ls_d();
	d_address a;
	while(!IsQueueEmpty(queue)){
		Dequeue(queue,&a);
		printf("%s\n",parse(a)->d_filename);
	}
	printf("Here:%s\n",parse(_scan_d(".../......."))->d_filename);
	_printSubdir(root,0);
	printf("%s\n",_scan_d("local/tony.c") == FILEPATH_ERROR?"yes":"no");
}

int main(){
	vf = (PhysicalBlocks*)malloc(sizeof(PhysicalBlocks));
	vf->located = 0;
//	my_dir = malloc(sizeof(critical_dir));
	Initial();
//	InitCDBlocks(my_dir);
	printf("%.6f\n",(double)clock()/CLOCKS_PER_SEC);
}
