
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
/*
Author: Conner Wulf

Approach: 
	FIFO (Starts Line:)
		For the FIFO Alogrithm, I used a normal queue since a queue operates in a FIFO fashion.
		The Queue is built up of nodes that contain a couple pointers, page number, and what frame
		that page number is in. The FIFO Alogrithm then loops through the input file and adds N Nodes with unique
		page numbers, to the queue. Where N is the memory size parameter inputed at runtime. When the Framelimit has been 
		reached, the algorithm will search the queue to see if the page number is already loaded into a frame, if not then 
		it will simply dequeue then enqueue the next value, Keeping the FIFO state. Frame number was added to the Node struct
		to aid the print_queue function when printing the queue in order by Frames

	LRU (Starts Line:)
		This implementation of LRU works very similar to the FIFO alogrithm with one major change. The initial N Frames are filled
		in the same way as the FIFO Algorithm, However whenever the LRU alogrithm needs to search the queue, it will use the function
		recentlyUsed() instead of search_queue(). The main difference between these two aglorithms when the recentlyUsed() function finds a match
		in the queue, it will remove it from its spot in the queue and repoint it to the back of the queue. This ensures that the value at the front of the queue is 
		always the Least Recently Used Value.
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
//Start and explanation at Line:
int search_queue(Node *head, int pNum);

//Start and explanation at Line:
void print_queue(Node *head, int length);

//Start and explanation at Line:
int dequeue(Node **head);

//Start and explanation at Line:
void enqueue(Node **head, int pNum, int frameNum);

//Start and explanation at Line:
void push(Node **head, Node **tail, int pNum, int frameNum);

//Start and explanation at Line:
int recentlyUsed(Node **head, int value);


/****************************************************************
*                  Main Body                                    *
****************************************************************/
int main(int argc, char *argv[])
{

	//REading in max frames and checking which alogrithm to run
	int isFIFO = strcmp(argv[1], "FIFO");
	int isLRU = strcmp(argv[1], "LRU");
	int maxFrames = atoi(argv[3]);

	//Checking to see if frame size is in bounds provided from assignment
	if(maxFrames < 1 || maxFrames > 50)
	{
		printf("Error: memory size must be between 1 and 50, inclusive\n");
		exit(1);
	}

	//printing inputed Memory Frame Size
	printf("\nMemory Size = %s\n", argv[3]);


	/**** START OF FIFO ALGORITHM ****/
	if(isFIFO == 0)
	{
		//Intialize Queue, FILE and variables
		Node *head = NULL;
		FILE *file;
		int pageNumber;
		int numOfPFs = 0;
		int usedFrames = 0;
		

		
		//Open file, throwing error if it cannot open
		if(!(file = fopen(argv[2], "rt")))
		{
			perror("Problem reading in file\n");
			exit(1);
		}

		//Scan in first number into variable pageNumber
		fscanf(file, "%d", &pageNumber);

		//Start looping through the input file
		while(!feof(file))
		{

			//Makes sure page number is a non-negative value
			//Skips if not
			if(pageNumber >= 0)
			{

				//Case Where all frames are empty
				if(head == NULL)
				{
					//Add First page number to Frame 0
					enqueue(&head, pageNumber, usedFrames);
					//printf("enqueue page %d\n", pageNumber);
					usedFrames++;
					numOfPFs++;
				}

				//Case where some frames are filled but some are still empty
				//While usedFrames < maxFrames empty frames 1 to frame(argv[3] -1) will be filled
				else if(usedFrames < maxFrames)
				{
					//checks if page exists in another Frame
					int check = search_queue(head, pageNumber);
					if( check != 1)
					{
						//adds value to blank frame if it is not in another frame
						enqueue(&head, pageNumber, usedFrames);
						//printf("enqueue page %d\n", pageNumber);
						usedFrames++;
						numOfPFs++;
					}
				}
				//Case where a page replacement must occur
				else
				{

					//checks if page exists in another Frame
					int noFault = search_queue(head, pageNumber);

					if(noFault == 1)
					{
						//No page replacement needed
						//Skip to next number
					}
					else
					{
						//Dequeue from front of the queue insures FIFO
						int temp = dequeue(&head);
						//printf("denqueue frame %d\n", temp);

						//Enqueue new page number to keep FIFO order
						enqueue(&head, pageNumber, temp);
						//printf("enqueue page %d\n", pageNumber);
						

						numOfPFs++;
					}
				}

			}
			//Read in the next page number
			fscanf(file, "%d", &pageNumber);
		}
		//close file
		fclose(file);

		//Print out results from FIFO Alogrithm
		printf("\nThe number of page faults was %d\n", numOfPFs);
		printf("\nThe final set of frames is\n");
		print_queue(head, maxFrames);
	}


/*** 	START OF LRU ALGORITHM 	***/

	else if(isLRU == 0)
	{
		//Intialize modified Queue, FILE and variables
		Node *head = NULL;
		Node *tail = NULL;
		FILE *file;
		int pageNumber;
		int numOfPFs = 0;
		int usedFrames = 0;



		//Open file, throwing error if it cannot open
		if(!(file = fopen(argv[2], "rt")))
		{
			perror("Problem reading in file\n");
			exit(1);
		}

		//Scan in first number into variable pageNumber
		fscanf(file, "%d", &pageNumber);

		//Start looping through the input file
		while(!feof(file))
		{
			//Makes sure page number is a non-negative value
			//Skips if not
			if(pageNumber >= 0)
			{

				//Case Where all frames are empty
				if(head == NULL)
				{
					//Add First page number to Frame 0
					push(&head, &tail, pageNumber, usedFrames);
					//printf("enqueue page %d\n", pageNumber);
					usedFrames++;
					numOfPFs++;
				}
				//Case where some frames are filled but some are still empty
				//While usedFrames < maxFrames empty frames 1 to frame(argv[3] -1) will be filled
				else if(usedFrames < maxFrames)
				{
					//checks if page exists in another Frame
					//If it does it moves the page reference to the end of the queue
					int check = recentlyUsed(&head, pageNumber);
					if( check != 1)
					{
						//adds value to blank frame if it is not in another frame
						push(&head, &tail, pageNumber, usedFrames);
						//printf("enqueue page %d\n", pageNumber);
						usedFrames++;
						numOfPFs++;
					}
					
				}
				else
				{
					
					//checks if page exists in another Frame
					//If it does it moves the page reference to the end of the queue
					int noFault = recentlyUsed(&head, pageNumber);

					if(noFault == 1)
					{
						//No page replacement needed
						//Skip to next number

						//printf("no fault\n");
					}
					else
					{
						//Dequeue value at the front of the queue to get rid of the Least recently used value
						int temp = dequeue(&head);
						//printf("dequeue frame %d\n", temp);
						//add to back of queue
						push(&head, &tail, pageNumber, temp);
						//printf("enqueue page %d\n", pageNumber);
						numOfPFs++;
					}
				}

			}
			//Read in next page reference
			fscanf(file, "%d", &pageNumber);
			
		}
		//close file
		fclose(file);

		//Print Results
		printf("\nThe number of page faults was %d\n", numOfPFs);
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


//Print N Frames in order from Frame 0 to frame N-1
void print_queue(Node *head, int length)
{
	Node *temp = head;
	int num = 0;
	int pagenumTemp = -1;

	//debug to print in order of the queue, not frame
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

//Basic queue search, Loops through queue until a match is found or 
//the end of the queue has been reached
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

//Queue dequeue function that returns frame number that was dequeued
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

//Queue Enqueue function that enqueues a page number and sets the
//frame number based on parameters
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


//Modified version of enqueue to work with a Tail pointer.
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

//Modified from search_queue function. Once match is found on Queue, function
//reassigns pointers of Queue Node to be at the end of the queue.
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
















