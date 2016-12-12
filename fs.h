#ifndef FS_H_INCLUDED
#define FS_H_INCLUDED


typedef unsigned char BYTE;
typedef unsigned short U16;
typedef struct
{
    BYTE data[2048];
}BLOCK;

typedef struct
{
    int blocklen;
    int fat1size;
    int fat1ptr;
    int fat2size;
    int fat2ptr;
    int rootptr;
    int datasize;
}BTBLK;

typedef struct
{
    char name[16];
    BYTE reserve[5];
    BYTE attr;
    U16 time;
    U16 date;
    U16 fsb;
    int len;
}FCB;

typedef struct
{
    U16 bid;
}FAT;

typedef struct
{
    char path[80];
    BYTE fid;

}USEROPEN;
void my_format(int flag, char *path);
void my_exitsys();
#endif // FS_H_INCLUDED
