
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
	struct Node *ptr_fwd;
	int Page_Num;
	int FrameNum;
} Node;


//Function definitions
int search_queue(Node *head, int pNum);
void print_queue(Node *head, int length);
int dequeue(Node **head);
void enqueue(Node **head, int pNum, int frameNum);
void push(Node **head, Node **tail, int pNum, int frameNum);
int removeLRU(Node **tail);
int recentlyUsed(Node **head, int value);


/****************************************************************
*                  Main Body                                    *
****************************************************************/
int main(int argc, char *argv[])
{

	printf("\nMemory Size = %s\n", argv[3]);

	int isFIFO = strcmp(argv[1], "FIFO");
	int isLRU = strcmp(argv[1], "LRU");
	int maxFrames = atoi(argv[3]);
	if(maxFrames < 1 || maxFrames > 50)
	{
		printf("Error: memory size must be between 1 and 50, inclusive\n");
		exit(1);
	}
	if(isFIFO == 0)
	{
		Node *head = NULL;
		FILE *file;
		int pageNumber;
		int numOfPFs = 0;
		int usedFrames = 0;
		

		

		if(!(file = fopen(argv[2], "rt")))
		{
			perror("Problem reading in file\n");
			exit(1);
		}

		fscanf(file, "%d", &pageNumber);
		while(!feof(file))
		{
			if(pageNumber >= 0)
			{
				if(head == NULL)
				{
					enqueue(&head, pageNumber, usedFrames);
					//printf("enqueue page %d\n", pageNumber);
					usedFrames++;
					numOfPFs++;
				}
				else if(usedFrames < maxFrames)
				{
					int check = search_queue(head, pageNumber);
					if( check != 1)
					{

						enqueue(&head, pageNumber, usedFrames);
						//printf("enqueue page %d\n", pageNumber);
						usedFrames++;
						numOfPFs++;
					}
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
						//printf("denqueue frame %d\n", temp);
						enqueue(&head, pageNumber, temp);
						//printf("enqueue page %d\n", pageNumber);
						

						numOfPFs++;
					}
				}

			}
			fscanf(file, "%d", &pageNumber);
		}
		fclose(file);


		printf("\n The number of page faults was %d\n", numOfPFs);
		printf("\nThe final set of frames is\n");
		print_queue(head, maxFrames);
	}

	else if(isLRU == 0)
	{
		Node *head = NULL;
		Node *tail = NULL;
		
		FILE *file;
		int pageNumber;
		int numOfPFs = 0;
		int usedFrames = 0;



		if(!(file = fopen(argv[2], "rt")))
		{
			perror("Problem reading in file\n");
			exit(1);
		}

		fscanf(file, "%d", &pageNumber);
		while(!feof(file))
		{
			if(pageNumber >= 0)
			{
				if(head == NULL)
				{
					push(&head, &tail, pageNumber, usedFrames);
					//printf("enqueue page %d\n", pageNumber);
					usedFrames++;
					numOfPFs++;
				}
				else if(usedFrames < maxFrames)
				{
					int check = recentlyUsed(&head, pageNumber);
					if( check != 1)
					{

						push(&head, &tail, pageNumber, usedFrames);
						//printf("enqueue page %d\n", pageNumber);
						usedFrames++;
						numOfPFs++;
					}
					
				}
				else
				{
					//print_queue(head, maxFrames);
					//printf("abouttocheck\n");
					int noFault = recentlyUsed(&head, pageNumber);
					//printf("noFault = %d on %d\n", noFault, pageNumber);
					if(noFault == 1)
					{
						//printf("no fault\n");
					}
					else
					{

						int temp = dequeue(&head);
						//printf("dequeue frame %d\n", temp);
						push(&head, &tail, pageNumber, temp);
						//printf("enqueue page %d\n", pageNumber);
						numOfPFs++;
					}
				}

			}
			fscanf(file, "%d", &pageNumber);
			
		}
		fclose(file);


		printf("\n The number of page faults was %d\n", numOfPFs);
		printf("\nThe final set of frames is\n");
		print_queue(head, maxFrames);

	}
	else
	{
		printf("Error: Algorithm not supported Please enter Either LRU or FIFO for First parameter\n");
		exit(1);
	}
}

/****************************************************************
*                  Supporting Functions definitions                                    *
****************************************************************/


//Debug print for custom queue
void print_queue(Node *head, int length)
{
	Node *temp = head;
	int num = 0;
	int pagenumTemp = -1;
	// while(temp != NULL) 
	// {
	// 	pagenumTemp = temp->Page_Num;
	// 	printf("Frame %d has page %d\n",num, pagenumTemp );
	// 	 	temp = temp->ptr;
	// 	 	num++;
	// }

	while(num <= length) 
	{
		if(temp->FrameNum == num)
		{
			pagenumTemp = temp->Page_Num;
			printf("Frame %d has page %d\n",num, pagenumTemp );
			temp = head;
			num++;
		}
		else if(temp->ptr != NULL)
		{
			temp = temp->ptr;
		}
		else
		{
			num++;
		}
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
	temp = cur->FrameNum;
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

void enqueue(Node **head, int pNum, int frameNum)
{
	Node *new = malloc(sizeof(Node));

	if(!new)
	{
		exit(1);
	}
	new->Page_Num = pNum;
	new->ptr = *head;
	new->FrameNum = frameNum;
	*head = new;
}

void push(Node **head, Node **tail, int pNum, int frameNum)
{
	Node *new = malloc(sizeof(Node));

	if(!new)
	{
		exit(1);
	}

	new->Page_Num = pNum;
	new->FrameNum = frameNum;

	if(*tail == NULL && *head == NULL)
	{
		new->ptr = *tail;
		new->ptr_fwd = *head;
		*head = new;
		*tail = new;
	}
	else
	{
		Node *temp = *head;
		new->ptr = *head;
		temp->ptr_fwd = new;
		*head = new;

	}
}

int removeLRU(Node **tail)
{
	if(tail != NULL)
	{
		Node *temp = *tail;
		Node *next = temp->ptr_fwd;
		*tail = next;
		free(temp);
		return temp->FrameNum;

	}
	else
	{
		return 0;
	}
}
int recentlyUsed(Node **head, int value)
{
	Node *temp = *head;
	Node *hold = *head;
	Node *prev = NULL;
	Node *next = NULL;
	
	while (temp != NULL)
	{

		int PageTemp = temp->Page_Num;
		
		if(PageTemp == value)
		{
			if(temp == *head)
			{
				return 1;
			}
			else if(temp != *head && temp->ptr != NULL)
			{
				prev = temp->ptr;
				next = temp->ptr_fwd;
				prev->ptr_fwd = next;
				next->ptr = prev;
				temp->ptr = *head;
				hold->ptr_fwd = temp;
				temp->ptr_fwd = NULL;
				*head = temp;
				temp = temp->ptr;

				return 1;
			}
			
			else if(temp->ptr == NULL)
			{
				next = temp->ptr_fwd;
				next->ptr = NULL;
				temp->ptr = *head;
				hold->ptr_fwd = temp;
				temp->ptr_fwd = NULL;
				*head = temp;
				temp = temp->ptr;
				return 1;
			}
			else
			{
				temp = temp->ptr;
			}
		}
		else
		{
			temp = temp->ptr;
		}
		
	}
	return 0;
}
















