#include <stdio.h>
#include <stdlib.h>
#define FILE_NAME "processes.txt"
typedef enum
{
    enum_pNew       ,
    enum_pReady     ,
    enum_pRunning   ,
    enum_pBlocked   ,
    enum_pterminated ,

}enum_pState;


typedef struct struct_PCB  //used for process info to be saved
{
    char process_ID;
    char CPU_time;
    char temp_CPU_time ;
    char IO_time;
    char arrival_time;
    char enum_pState ;
    char turnaround_time ;
	struct struct_PCB* next_PCB ;

}struct_PCB ;

typedef struct
{
    struct_PCB* firstElement ;
    struct_PCB* lastElement ;
	int queue_elementsNumber ;

}struct_queue ;



void initialization(void) ;
struct_PCB* create_PCB(void) ;
struct_PCB* remove_PCB(char PCB_ID,struct_queue* queue) ;
void insert_PCB(struct_PCB* PCB , struct_queue* queue) ;
void ready_search(void);
void blocked_search(void);
void swap(struct_PCB** PCB_array,int element_1, int element_2);
void sort_bubble (struct_PCB** PCB_array, int PCB_array_size);
void FCFS_scheduling(void);
void FCFS_dispatcher(void);
struct_PCB* RR_dispatcher(void);
void process_admit (void);
void IO_request (void);


struct_queue queue_newProcess, queue_readyProcess, queue_blocedProcess, queue_terminatedProcess  ;
int systemTime = 0 ;
char quantum_time = 4 ;
char quantum_flag = 0 ;

void main(void)
{
    char schedulingChoice ;

    while(1)
    {
		initialization();
		printf("please enter '0' for FCFS and '1' for RR scheduling '2' to exit: ");
        scanf("%d",&schedulingChoice);
        if(schedulingChoice!=0 && schedulingChoice!=1 && schedulingChoice!=2)
        {
            printf("wrong input\n");
            continue ;
        }
        else if (schedulingChoice == 0)
        {
            FCFS_scheduling();
        }
        else if (schedulingChoice == 2)
        {
			break ;
        }
        else
        {
            printf("please enter quantum time (positive integer) : ");
            scanf("%d",&quantum_time);
            if (quantum_time<= 0)
            {
                printf("wrong input\n");
                continue ;
			}
            RR_scheduling();
        }

    }
    return ;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void initialization(void)  //this function save data to struct  to be handled
{
// all coming code just an intialization
	char fileName[20] ;
	int numberOf_pInfo = 0 ;
	int array_Pinfo_index =0;
	int numberOf_processes=0 ;
	int temp[1] ;
	struct_PCB * tempPtr_PCB ;
	char* array_Pinfo ;

	queue_newProcess.firstElement=NULL;
	queue_newProcess.lastElement=NULL;
	queue_newProcess.queue_elementsNumber=0;

	queue_readyProcess.firstElement=NULL;
	queue_readyProcess.lastElement=NULL;
	queue_readyProcess.queue_elementsNumber=0;

	queue_blocedProcess.firstElement=NULL;
	queue_blocedProcess.lastElement=NULL;
	queue_blocedProcess.queue_elementsNumber=0;

	queue_terminatedProcess.firstElement=NULL;
	queue_terminatedProcess.lastElement=NULL;
	queue_terminatedProcess.queue_elementsNumber=0;
    printf ("enter file name: -\n");
	scanf("%s",fileName);//get file name from user

	FILE *file_1 = fopen( fileName, "r" );//open file which is 2nd arg
	FILE *file_2 = fopen( fileName, "r" );//open file which is 2nd arg


	if ( file_1 == NULL )// fopen returns 0, the NULL pointer, on failure
	{
		printf( "Could not open file\n" );
	}
	else
	{
		do
		{
			fscanf(file_1,"%d",&temp[0]); //Storing the number into the array
			numberOf_pInfo++;
		}
		while((getc(file_1))!=EOF);



		if(numberOf_pInfo%4!=0)//case miss of data
		{
			printf("there's one process or more with missing elements please check processes' data\n");
			return ;
		}

		numberOf_processes=numberOf_pInfo/4 ;

		array_Pinfo = (char *) malloc(sizeof(char)*numberOf_pInfo);

		do
		{
			fscanf(file_2,"%d",&temp[0]); //Storing the number into the array
			array_Pinfo[array_Pinfo_index]=temp[0] ;
			array_Pinfo_index ++;
		}
		while((getc(file_2))!=EOF);
	}
	fclose( file_1 );//done using the file
	fclose( file_2 );//done using the file

	array_Pinfo_index=0;

	for(;numberOf_processes>0;numberOf_processes--)//save data  to temporary structure
	{
		tempPtr_PCB=create_PCB();
		tempPtr_PCB->process_ID = array_Pinfo[array_Pinfo_index++] ;
		tempPtr_PCB->CPU_time = array_Pinfo[array_Pinfo_index++] ;
		tempPtr_PCB->temp_CPU_time = tempPtr_PCB->CPU_time ;
		tempPtr_PCB->IO_time = array_Pinfo[array_Pinfo_index++] ;
		tempPtr_PCB->arrival_time = array_Pinfo[array_Pinfo_index++] ;
		tempPtr_PCB->enum_pState = enum_pNew ;
		tempPtr_PCB->turnaround_time=0;
		insert_PCB( tempPtr_PCB , &queue_newProcess ) ;//insert each process to queue of new processes



	}

	free(array_Pinfo);
	return ;

}

////////////////////////////////////////////////

struct_PCB* create_PCB(void)
{

	 return (struct_PCB*)malloc(sizeof(struct_PCB)) ;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void insert_PCB(struct_PCB* PCB , struct_queue* queue)//insert process to queue
{
    if(0 == queue->queue_elementsNumber)//if it is an empty queue
    {
        queue->firstElement = PCB;
		queue->lastElement = PCB;

    }
    else //normal case
    {
        queue->lastElement->next_PCB = PCB;// let last pointer pointer to new process
        queue->lastElement = PCB;//let new  last elemnt  =new prpocess
        PCB->next_PCB = NULL ;//let last elemnt point to null
    }

	queue->queue_elementsNumber ++ ;
    return ;
}

//////////////////////////////////////////////////////////////////////////////////////////

struct_PCB* remove_PCB(char PCB_ID, struct_queue* queue) // return NULL if no processes in queue  or can't find id
{
	struct_PCB* temp_PCB = queue->firstElement ;
	struct_PCB* temp_2_PCB ;
    if(0 == queue->queue_elementsNumber)
    {

    }
    else
    {
        if(1 == queue->queue_elementsNumber && PCB_ID == temp_PCB->process_ID)//if queue has only one process
        {

            queue->firstElement = NULL ;
            queue->lastElement = NULL ;
            queue->queue_elementsNumber -- ;
			return temp_PCB ;

        }
        else if(PCB_ID == temp_PCB->process_ID) // if i want to remove first process in queue
        {

            queue->firstElement = temp_PCB->next_PCB ;
            temp_PCB->next_PCB =NULL ;
            queue->queue_elementsNumber -- ;
            return temp_PCB ;
		}
        else //normal case
        {

            temp_2_PCB= temp_PCB ;
            temp_PCB = temp_PCB->next_PCB ;
            while(NULL != temp_PCB->next_PCB)
            {


                if(PCB_ID == temp_PCB->process_ID)//if i found processes
                {
                    temp_2_PCB->next_PCB = temp_PCB->next_PCB ;
                    temp_PCB->next_PCB = NULL ;
                    queue->queue_elementsNumber -- ;
                    return temp_PCB ;
                }
                else// complete in search for process
                {
                    temp_2_PCB= temp_PCB ;
                    temp_PCB = temp_PCB->next_PCB ;//next processes in queue
                }
			}

        }

        if (PCB_ID == temp_PCB->process_ID)//after reach pointer to process i want is  last  elemnt
        {

            temp_2_PCB->next_PCB = NULL ; //make it point to
            queue->lastElement = temp_2_PCB ;
            queue->queue_elementsNumber -- ;
			return temp_PCB ;

        }
	}
    return NULL ;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ready_search(void)
{
    struct_PCB* temp_blockedPCB = queue_blocedProcess.firstElement ;
    struct_PCB* temp_newPCB = queue_newProcess.firstElement ;
    struct_PCB* temp_PCB ;
    struct_PCB** temp_PCB_array ;

    int temp_PCB_array_index =0 ;
    int temp_PCB_array_size = queue_blocedProcess.queue_elementsNumber + queue_newProcess.queue_elementsNumber ;//all number of possible new processes go ready state
    int readyProcesses_number =0 ;



    temp_PCB_array = (struct_PCB**)malloc(sizeof(struct_PCB*)*temp_PCB_array_size);//intiate array in heap

    while(temp_PCB_array_index<temp_PCB_array_size )//intiate array of pointer to structure
    {
        temp_PCB_array[temp_PCB_array_index]=NULL;//intiate all elemnt with null
        temp_PCB_array_index++;
    }
    temp_PCB_array_size=temp_PCB_array_index-1;
    temp_PCB_array_index = 0 ;

    if(NULL == temp_blockedPCB && NULL == temp_newPCB)//if new and blocked queues empty return nothing to do here
	{
        return ;
    }
	else
    {

        if(NULL != temp_newPCB)//there is process in newqueue
        {
			do
			{
                temp_PCB =temp_newPCB->next_PCB;//next process in new queue
                if(0==temp_newPCB->arrival_time)//arrival time equal zero has just arrived
                {
                    temp_PCB_array[readyProcesses_number]=remove_PCB(temp_newPCB->process_ID, &queue_newProcess);//insert it to ready queue
                    readyProcesses_number ++ ;

                }
                temp_newPCB = temp_PCB ;
            }
            while(NULL != temp_newPCB) ;//while queue not ended
        }

		if(NULL != temp_blockedPCB) //if there is processes in bocked queue
        {
            do
            {

                temp_PCB =temp_blockedPCB->next_PCB;

                if(0==temp_blockedPCB->IO_time)//if i/o time finish
                {
                    temp_blockedPCB->CPU_time= temp_blockedPCB->temp_CPU_time;//give it cpu time again
					temp_PCB_array[readyProcesses_number]=remove_PCB(temp_blockedPCB->process_ID, &queue_blocedProcess);//add it to ready processes
                    readyProcesses_number ++ ;

				}

                temp_blockedPCB = temp_PCB ;

            }
			while(NULL != temp_blockedPCB);//while still there is processes in queue

        }
    }
    if(readyProcesses_number>1)
    {
        sort_bubble(temp_PCB_array , temp_PCB_array_size );//sort array of ready processes
    }


    while(readyProcesses_number > 0)//this loop to add all processes i get into ready queue
    {
        if(NULL != temp_PCB_array[temp_PCB_array_index])
        {
			insert_PCB(temp_PCB_array[temp_PCB_array_index] , &queue_readyProcess);//add ready processes to ready queue
            temp_PCB_array[temp_PCB_array_index]->enum_pState = enum_pReady ;//change their state to ready state
            readyProcesses_number--;
        }
        temp_PCB_array_index++;
    }

    free(temp_PCB_array);//delet unsueful array now
    return ;

}


////////////////////////////////////////////////////////////////


void blocked_search(void)// search for blocked processes
{
	struct_PCB* temp_readyPCB = queue_readyProcess.firstElement ;
	struct_PCB* temp_PCB ;

    if(NULL == temp_readyPCB )
    {
        return ;
    }

     else
    {


        do
        {
			temp_PCB =temp_readyPCB->next_PCB; //next processes
            if(0==temp_readyPCB->CPU_time) //process finished cpu time
            {
                quantum_flag = 1 ;
                if(0==temp_readyPCB->IO_time)//this case function finished cpu and i/o time
                {
                    insert_PCB(remove_PCB(temp_readyPCB->process_ID, &queue_readyProcess), &queue_terminatedProcess);
                    temp_readyPCB->enum_pState = enum_pterminated ;//change its state
                }
                else//case function go to be blocked
				{
                    insert_PCB(remove_PCB(temp_readyPCB->process_ID, &queue_readyProcess), &queue_blocedProcess);
                    temp_readyPCB->enum_pState = enum_pBlocked ;//change state to blocked
				}

            }
            temp_readyPCB = temp_PCB ;
        }
		while(NULL != temp_readyPCB) ;
	}


    return ;
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void swap(struct_PCB** PCB_array,int element_1, int element_2)//swap between two elemnt in array of pointer to structure (queue)
{
    struct_PCB * temp ;
    temp = PCB_array[element_1];
    PCB_array[element_1] = PCB_array[element_2];
    PCB_array[element_2] = temp ;

}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void sort_bubble (struct_PCB** PCB_array, int PCB_array_size)//sort queue
{

    int outerIndex=0;
    int innerIndex=0;


	for(outerIndex=0;outerIndex<PCB_array_size-1;outerIndex++)
    {
        for(innerIndex=PCB_array_size-1;innerIndex>outerIndex;innerIndex--)
        {

			if( PCB_array[innerIndex]->process_ID < PCB_array[innerIndex-1]->process_ID)
			{
				swap(PCB_array,innerIndex,innerIndex-1);
			}
		}
	}

}

void FCFS_scheduling(void)
{
    int localFlag=0 ;
    float totalBurst=0 ;
    float cpuUtilization=0 ;
    struct_PCB* temp ;
    FILE *file_out = fopen("file_out.txt","w"); //open file to write


	ready_search(); //search for ready processes
	blocked_search();//search for blocked processes
    temp= queue_readyProcess.firstElement;

    fprintf(file_out,"0  %d:running\n",temp->process_ID);//first running process

	while( queue_newProcess.queue_elementsNumber>0 || queue_readyProcess.queue_elementsNumber>0 || queue_blocedProcess.queue_elementsNumber>0 )
	{
        FCFS_dispatcher();//choose process to be running
        IO_request(); //concern in blocked queue
        process_admit();// concern in new queue
        ready_search(); //search for ready processes to add them to queue
        blocked_search();// search for blocked processes to add them to blocked queue
        systemTime++;
        if(queue_newProcess.queue_elementsNumber>0 || queue_readyProcess.queue_elementsNumber>0 || queue_blocedProcess.queue_elementsNumber>0)
        {
			fprintf(file_out,"%d  ",systemTime);//simulation cycle
			//print id of running processes
        }
        temp= queue_readyProcess.firstElement;
		while(temp!=NULL)//till last process in queue
		{

		    if (0 == localFlag)
			{
			    fprintf(file_out,"%d:running ",temp->process_ID);
			    localFlag = 1 ;
            }
			else
            {
                fprintf(file_out,"%d:ready ",temp->process_ID);
            }
			temp= temp->next_PCB;//next process in queue
		}
		localFlag=0 ;
//		  printf id of blocked process
		temp= queue_blocedProcess.firstElement;
		while(temp!=NULL)//till last process in queue
		{
			fprintf(file_out,"%d:blocked ",temp->process_ID);//print id of each blocked process
			temp= temp->next_PCB;//next process in queue
		}

        fprintf(file_out,"\n");


	}

        systemTime--;

        //calculate total burst time
		temp= queue_terminatedProcess.firstElement;

		while(temp!=NULL)   //till last process in queue
		{
			totalBurst+=temp->temp_CPU_time ;   //variable store total burst time
			temp= temp->next_PCB;   //nexr process in queue
		}

        cpuUtilization =((totalBurst*2)/(systemTime+1)) ;   //cpu utilization
        fprintf(file_out,"\nFinish time: %d\n",(systemTime));
		fprintf(file_out,"Cpu Utilization: %f\n",cpuUtilization); //printf data of each process (turnaround time )
		temp= queue_terminatedProcess.firstElement;
		while(temp!=NULL)
		{
			fprintf(file_out,"Turnaround time of Process %d: ",temp->process_ID);
			fprintf(file_out,"%d\n",temp->turnaround_time);
			temp= temp->next_PCB;
		}


        fclose(file_out); //close output file

		temp= queue_terminatedProcess.firstElement;
		while(temp!=NULL) //delet all processes after finish to free space in heap
		{
			free(temp) ;
			temp= temp->next_PCB;//next process in queue
		}


	systemTime=0;
    printf("\n------------------------------------\n");
	return ;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RR_scheduling (void) //this function to do RR scheduling and print the output
{
    char localFlag=0 ;
    float totalBurst=0 ;
    float cpuUtilization=0;
    FILE *file_out=fopen("file_out.txt","w"); //output file

	struct_PCB* temp;

    ready_search(); //search for ready process and add them to ready queue
    blocked_search();//search for bloacked process and add them to blocked queue

    temp=queue_readyProcess.firstElement;

    fprintf(file_out,"0  %d:running\n",temp->process_ID);//first running process

    while( queue_newProcess.queue_elementsNumber>0 || queue_readyProcess.queue_elementsNumber>0 || queue_blocedProcess.queue_elementsNumber>0 )
    {
        temp = RR_dispatcher();// return process when quantum finished to insert it at last queue
        IO_request();
        process_admit();//get new process
        ready_search(); //search for ready processes
        if(temp != NULL )
        {
            insert_PCB( temp , &queue_readyProcess) ;//insert process in ready queue
        }
        blocked_search();//search for blocked process to join blocked queue
        systemTime++;  //simulation time

        if(queue_newProcess.queue_elementsNumber>0 || queue_readyProcess.queue_elementsNumber>0 || queue_blocedProcess.queue_elementsNumber>0)
        {
			fprintf(file_out,"%d  ",systemTime);//simulation cycle
        }
        //print id of running processes

        temp= queue_readyProcess.firstElement;

		while(temp!=NULL) //print ID of all ready processes
		{
            if (0==localFlag)
			{
			    fprintf(file_out,"%d:running ",temp->process_ID);
                localFlag=1 ;
            }
			else
            {
                fprintf(file_out,"%d:ready ",temp->process_ID);
            }
			temp= temp->next_PCB; //next elemnt in queue
		}
        // printf ID of blocked process
		temp= queue_blocedProcess.firstElement;
        localFlag=0 ;
		while(temp!=NULL)//until last elemnt in queue
		{
			fprintf(file_out,"%d:blocked ",temp->process_ID);
			temp= temp->next_PCB;//next process in queue
		}

        fprintf(file_out,"\n");

    }

    systemTime--;
    temp= queue_terminatedProcess.firstElement;

    while(temp!=NULL)  //calculate total burst time
    {
        totalBurst+=temp->temp_CPU_time ; //variable store total burst time
        temp= temp->next_PCB;
    }


    cpuUtilization =((totalBurst*2)/(systemTime+1)) ;   //cpu utilization
    fprintf(file_out,"\nFinish time: %d\n",(systemTime));
    fprintf(file_out,"Cpu Utilization: %f\n",cpuUtilization);

    //printf turnaround time of each process
    temp= queue_terminatedProcess.firstElement;
    while(temp!=NULL) //untill last elemnt in queue
    {
        fprintf(file_out,"Turnaround time of Process %d: ",temp->process_ID);//print ID of process
        fprintf(file_out,"%d\n",temp->turnaround_time);   //print Turnaround time of process
        temp= temp->next_PCB; //go to next process in queue
    }

        //delete all processes after finish
    temp= queue_terminatedProcess.firstElement;
    while(temp!=NULL)//until last elemnt in queue
    {
        free(temp) ;
        temp= temp->next_PCB;
    }
    systemTime=0;
    fclose(file_out); //close the file
    printf("\n------------------------------------\n");
    return ;


}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void FCFS_dispatcher(void)
{
    struct_PCB* temp= queue_readyProcess.firstElement;
    if(temp!=NULL)
    {
        temp->enum_pState = enum_pRunning ;
        temp->CPU_time -- ;
        while(temp!=NULL) //till last process in queue
        {
            temp->turnaround_time++; //increase turnaround time of all processes in ready queue
            temp= temp->next_PCB; //next elemnt in queue
        }
    }

    return ;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct_PCB* RR_dispatcher(void)
{
    static char temp_quantum_time = 1 ;
    struct_PCB* temp ;
    temp= queue_readyProcess.firstElement;
    if (quantum_flag)
    {
        temp_quantum_time = 1 ;
        quantum_flag =0 ;
    }
    if(temp!=NULL) //if first process in ready queue exist
    {

        temp->enum_pState = enum_pRunning ; //let its state running
        temp->CPU_time -- ;                    //decrease cpu time
        while(temp!=NULL)             //increase tunraround time of all processes
        {
            temp->turnaround_time++;
            temp= temp->next_PCB;
        }
        if(temp_quantum_time<quantum_time)//normal case
        {
            temp_quantum_time++;
        }
        else//if quantum finish
        {
            temp_quantum_time = 1 ; //intiate it with 1 again
            queue_readyProcess.firstElement->enum_pState = enum_pReady ; //make this process ready again
            return remove_PCB(queue_readyProcess.firstElement->process_ID , &queue_readyProcess);//remove it from queue to add at last of queue
        }
    }
    else
    {
        temp_quantum_time = 1 ;
    }

    return NULL;
}

////////////////////////////////////////

void process_admit (void) //this function concerned with queue of new processes
{
    struct_PCB* temp ;
    temp= queue_newProcess.firstElement;
    while(temp!=NULL)
    {
        temp->arrival_time -- ;
        temp= temp->next_PCB;

    }
    return ;

}

///////////////////////////////////////

void IO_request (void) //this function concerned to handel queue of blocked processes
{
    struct_PCB* temp ;
    temp= queue_blocedProcess.firstElement;
    while(temp!=NULL)
    {
        temp->IO_time -- ;
        temp->turnaround_time ++ ;
        temp= temp->next_PCB;
    }
    return ;
}
