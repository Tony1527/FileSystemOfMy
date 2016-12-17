#ifndef IDLIST_H
#define IDLIST_H
#include<stdlib.h>
#include<stdio.h>
#define ASC 1
#define DSC 2

typedef struct LIST_NODE{
	int id;
	struct LIST_NODE *next;	
	struct LIST_NODE *pre;
}*Linked_List;

Linked_List createList(){
	Linked_List head = (Linked_List)malloc(sizeof(struct LIST_NODE));
	head->next = head;
	head->pre = head;
	return head;
}

void print_list(Linked_List head){
	if(head==NULL){
		printf("List is Empty!\n");
		return;
	}
	Linked_List ptr=head->next;
	printf("************\n");
	while(head!=ptr){
		printf("%d\n",ptr->id);
		ptr = ptr->next;
	}
}

void list_add(Linked_List head,int id,int policy){
	if(head){
		Linked_List new = (Linked_List)malloc(sizeof(struct LIST_NODE));
		Linked_List ptr = head->next;
		new->id = id;
		do{
			if(policy == ASC){
				if(id<ptr->id){
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
			}else if(policy == DSC){
				if(id>ptr->id){
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


			}else{
				printf("Wrong argument of policy!\n");
				return;
			}
			ptr = ptr->next;
		}while(ptr != head);
	}else{
		printf("List is Empty!\n");
	}
}

void list_del(Linked_List head,int id){
	if(head == NULL)
		return;
	if(isEmpty(head))
		return;
	Linked_List ptr = head->next,old = head;
	while(ptr != head){
		if(ptr->id == id){
			old->next = ptr->next;
			old->next->pre = old;
			free(ptr);
			break;
		}
		old = ptr ;
		ptr = ptr->next;
	}
}

void list_destroy(Linked_List head){
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

int isEmpty(Linked_List head){
	if(head == head->next){
		return 1;
	}
	return 0;
}

#endif
