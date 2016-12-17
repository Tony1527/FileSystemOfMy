#ifndef DOUBLE_LINKED_FILENAME_H
#define DOUBLE_LINKED_FILENAME_H
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

typedef struct LIST_NODE{
	char filename[128];
	struct LIST_NODE *next;	
	struct LIST_NODE *pre;
}*Linked_List;				//链表节点

typedef struct DOUBLE_LINKED{
	int len;
	Linked_List head;
}*Double_Linked;			//双向链表


Linked_List _createList(){
	Linked_List head = (Linked_List)malloc(sizeof(struct LIST_NODE));
	head->next = head;
	head->pre = head;
	return head;
}

Double_Linked createDLList(){
	Double_Linked double_link = (Double_Linked)malloc(sizeof(struct DOUBLE_LINKED));;
	double_link->head = _createList();
	double_link->len = 0;
	return double_link;
}



void _print_list(Linked_List head){
	if(head==NULL){
		printf("List is Empty!\n");
		return;
	}
	Linked_List ptr=head->next;
	while(head!=ptr){
		printf("%s\n",ptr->filename);
		ptr = ptr->next;
	}
}



void print_list(Double_Linked head){
	if(head==NULL)
		printf("Error! list is NULL\n");
	if(head->len == 0){
		printf("Double List is Empty!\n");		
		return;
	}
	_print_list(head->head);
}




void _list_add(Linked_List head,char filename[]){
	if(head){
		Linked_List new = (Linked_List)malloc(sizeof(struct LIST_NODE));
		Linked_List ptr = head->next;
		strcpy(new->filename,filename);
		do{
			if(strcmp(filename,ptr->filename)<0){
					new->next = ptr;
					new->pre = ptr->pre;
					ptr->pre->next = new;
					return;
			}else if(ptr->next == head){
					head->pre->next = new;
					new->next = head;
					new->pre = head->pre;
					head->pre= new;
					return;
				}
			ptr = ptr->next;
		}while(ptr != head);
	}else{
		printf("List is Empty!\n");
	}
}

void list_add(Double_Linked head,char filename[]){
	if(head==NULL)
		printf("Error! list is NULL\n");
	_list_add(head->head,filename);
	head->len++;
}



void _list_del(Linked_List head,char filename[]){
	if(head == NULL)
		return;
	if(isEmpty(head))
		return;
	Linked_List ptr = head->next,old = head;
	while(ptr != head){
		if(!strcmp(ptr->filename,filename)){
			old->next = ptr->next;
			old->next->pre = old;
			free(ptr);
			break;
		}
		old = ptr ;
		ptr = ptr->next;
	}
}


void list_del(Double_Linked head,char filename[]){
	if(head==NULL)
		printf("Error! list is NULL\n");
	_list_del(head->head,filename);
	head->len--;
}


void _list_destroy(Linked_List head){
	if(head == NULL)
		return;
	Linked_List ptr = head->next,old = ptr;
	head->next = NULL;
	while(ptr!=NULL){
		ptr = ptr->next;
		free(old);
		old = ptr;
	}
}

void list_destroy(Double_Linked head){
	if(head==NULL)
		printf("Error! list is NULL\n");
	_list_destroy(head->head);
	free(head);
}

int _isEmpty(Linked_List head){
	if(head == head->next){
		return 1;
	}
	return 0;
}

int isEmpty(Double_Linked head){
	if(head==NULL)
		printf("Error! list is NULL\n");
	return head->len!=0?1:0;
}



#endif
