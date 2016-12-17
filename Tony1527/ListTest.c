#include<stdio.h>
#include"DoubleLinked.h"



int main(void){
	Double_Linked list = createList();
	printf("%x\n",list);
	list_add(list,"helloc.c");
	list_add(list,"asb");
	list_add(list,"yese");
	list_add(list,"nope");
	list_add(list,"/hello");
	print_list(list);
	list_destroy(list);
}
