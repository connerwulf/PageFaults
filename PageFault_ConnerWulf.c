
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
/*
Author: Conner Wulf
*/


//Base node for custom Queue
typedef struct Node
{
	struct Node *ptr;
	int Page_Num;
} Node;


//Function definitions
int search_queue(Node *head, int pNum);
void print_queue(Node *head);
int dequeue(Node **head);
void enqueue(Node **head, int pNum);



/****************************************************************
*                  Main Body                                    *
****************************************************************/
int main(int argc, char *argv[])
{
	Node *head = NULL;
	FILE *file;
	int pageNumber;
	int numOfPFs = 0;
	int usedFrames = 0;
	printf("%s %s %s", argv[1],  argv[2],  argv[3]);
	if(argv[1] == "FIFO")
	{
		if(!(file = fopen(argv[2], "rt")))
		{
			perror("Problem reading in file\n");
			exit(1);
		}

		while((pageNumber = fgetc(file)) != EOF)
		{
			if(pageNumber != ' ')
			{
				if(head == NULL)
				{
					enqueue(&head, pageNumber);
					usedFrames++;
					numOfPFs++;
				}
				else if(usedFrames < atoi(argv[3]))
				{
					enqueue(&head, pageNumber);
					usedFrames++;
					numOfPFs++;
				}
				else
				{
					int noFault = search_queue(head, pageNumber);
					if(noFault == 1)
					{
						// do nothing
					}
					else
					{
						int temp = dequeue(&head);
						enqueue(&head, pageNumber);

						numOfPFs++;
					}
				}

			}
		}


		printf("Page Faults: %d\n", numOfPFs);
		print_queue(head);
	}
}

/****************************************************************
*                  Supporting Functions definitions                                    *
****************************************************************/


//Debug print for custom queue
void print_queue(Node *head)
{
	Node *temp = head;
	int num = 0;
	while (temp != NULL)
	{
		int pagenumTemp = temp->Page_Num;

		printf("Frame %d has page %d\n",num, pagenumTemp );
		num++;
		temp = temp->ptr;
	}
}

//Debug print for custom queue
int search_queue(Node *head, int searchPage)
{
	Node *temp = head;
	while (temp != NULL)
	{
		int PageTemp = temp->Page_Num;
		if(PageTemp == searchPage)
		{
			temp = NULL;
			return 1;
		}
		else
		{
				temp = temp->ptr;
		}
	}
	return 0;
}


int dequeue(Node **head)
{
	Node *cur  = NULL;
	Node *prev = NULL;
	int temp;
	if(*head == NULL)
	{
		return -1;
	}

	cur = *head;
	while(cur->ptr != NULL)
	{
		prev = cur;
		cur = cur->ptr;
	}
	temp = cur->Page_Num;
	free(cur);

	if(prev)
	{
		prev->ptr = NULL;
	}
	else
	{
		*head = NULL;
	}
	return temp;
}

void enqueue(Node **head, int pNum)
{
	Node *new = malloc(sizeof(Node));

	if(!new)
	{
		exit(1);
	}
	new->Page_Num = pNum;
	new->ptr = *head;
	*head = new;
}
