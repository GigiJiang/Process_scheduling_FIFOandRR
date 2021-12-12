//
// Created by Ziyi Jiang on 2021/11/27.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define N 20;

int quantum = -1;
int count;
int timer = 0;

typedef struct pcb{
    int id;
    int arrival_time;
    int runtime;
    int needtime;
    int remaining_quantum;
    char state[20];
    struct pcb *next;
}PCB;

PCB *ready_head;
PCB *ready_tail;
PCB *block_head;
PCB *block_tail;


void initiateProcesses(char **argv){
    PCB *p;
    int i;
    ready_head= ready_tail = NULL;

    printf("there are %d processes\n",count);
    printf("-----------Simulation Starts-------------\n");
    printf("Cycles\tRunning\tReady\tblock\n");

    for (i=1;i<=count;i++){
        p = (PCB*)malloc(sizeof(PCB));
        p->id = i;
        p->arrival_time = 0;
        p->runtime = 0;
        p->needtime = atoi(argv[4*(i-1)]);
        strcpy(p->state, "R1");
        p->next = NULL;
        if(quantum>0){
            p->remaining_quantum = quantum;
        }
        if (i==1){
            ready_head = p;
            ready_tail = p;
        }else{
            ready_tail->next= p;
            ready_tail = p;
        }
    }
    block_tail = block_head = NULL;
}

void freeReadyList(){
    PCB *p;
    while (ready_head != NULL){
        p = ready_head;
        ready_head = ready_head->next;
        free(p);
    }
    ready_tail = NULL;
}

void freeBlockList(){
    PCB *p;
    while (block_head != NULL){
        p = block_head;
        block_head = block_head->next;
        free(p);
    }
    block_tail = NULL;
}

void printReadyList(){
    PCB *p;
    p = ready_head;
    printf("Ready List: ");
    while (p != NULL){
        printf("%d, %d, %d, %d, %d, %s\t",p->id,p->arrival_time,p->runtime,p->needtime,p->remaining_quantum,p->state);
        p = p->next;
    }
    printf("\n");
}

void printBlockList(){
    PCB *p;
    p = block_head;
    printf("Block List: ");
    while (p != NULL){
        printf("%d, %d, %d, %d, %d, %s\t",p->id,p->arrival_time,p->runtime,p->needtime,p->remaining_quantum,p->state);
        p = p->next;
    }
    printf("\n");
}

void printCycle(){
    PCB *p;

    printf("%d\t",timer);
    p = ready_head;
    if(p!=NULL){
        printf("%d\t",p->id);
        p = ready_head->next;
        while (p != NULL){
            printf("%d ",p->id);
            p = p->next;
        }
    }else{
        printf("\t");
    }
    printf("\t");

    p = block_head;
    while (p != NULL){
        printf("%d ",p->id);
        p = p->next;
    }
    printf("\n");
}

void block_time(char **argv){
    PCB *p,*temp,*temp2,*insert_point,*previous_point;
    p = block_head;
    temp2 = NULL;
    if(timer == 8){
        printf("------------------\n");
        printBlockList();
    }

    while (p){
        if (p->needtime >= 1){
            p->needtime -= 1;
            p->runtime += 1;
            p = p->next;
        }else{
            temp = p;
            if(temp2!=NULL){
                temp2->next = p->next;
                if(block_tail==p){
                    block_tail = temp2;
                }
            }else{
                block_head = p->next;
                if(block_tail==p){
                    block_tail = NULL;
                }
            }

            if (!strcmp(p->state,"B1")){
                p->needtime = atoi(argv[4*(p->id-1)+2]);
                p->runtime = 0;
                p->arrival_time = timer;
                p->remaining_quantum = quantum;
                strcpy(p->state, "R2");
                p = p->next;
                previous_point = NULL;

                for(insert_point =ready_head;insert_point!=NULL;insert_point=insert_point->next) {
                    if(insert_point->arrival_time == timer && insert_point->id >temp->id) {
                        break;
                    }
                    previous_point = insert_point;
                }

                if(insert_point!=NULL) {
                    if(previous_point==NULL) {
                        temp->next = insert_point;
                        ready_head = temp;
                    }else{
                        previous_point->next = temp;
                        temp->next = insert_point;
                    }
                }else{
                    if(ready_head==NULL){
                        ready_head = temp;
                    }
                    if(ready_tail==NULL){
                        ready_tail = temp;
                        ready_tail->next = NULL;

                    }else{
                        ready_tail->next = temp;
                        ready_tail = ready_tail->next;
                        ready_tail->next = NULL;
                    }
                }

            }else if(!strcmp(p->state,"B2")){
                    p->needtime = 1;
                    p->runtime = 0;
                    p->arrival_time = timer;
                    p->remaining_quantum = quantum;
                    strcpy(p->state, "terminate");
                    p = p->next;
                    previous_point = NULL;

                    for(insert_point =ready_head;insert_point!=NULL;insert_point=insert_point->next) {
                        if(insert_point->arrival_time == timer && insert_point->id >temp->id) {
                            break;
                        }
                        previous_point = insert_point;
                    }
                    if(insert_point!=NULL) {
                        if(previous_point==NULL) {
                            temp->next = insert_point;
                            ready_head = temp;
                        }else{
                            previous_point->next = temp;
                            temp->next = insert_point;
                        }
                    }else{
                        if(ready_head==NULL){
                            ready_head = temp;
                        }if(ready_tail==NULL){
                            ready_tail = temp;
                            ready_tail->next = NULL;

                        }else{
                            ready_tail->next = temp;
                            ready_tail = ready_tail->next;
                            ready_tail->next = NULL;
                        }
                    }
//                printReadyList();
                }
            }
    }
}


void SA_RR(char **argv){
    // using Round Robin Scheduling Algo
    PCB *p,*temp;
    p = ready_head->next;
    int quantum_remaining = quantum;


    while(1){
        if (ready_head->next==NULL){
            printf("there is no process in ready queue!\n");
                return;
        }else{
            while(p){
                if(quantum_remaining == 0){
                    temp = p;
                    if(temp->needtime>0){
                        if(count>1){
                            ready_head->next = temp->next;
                            ready_tail->next = temp;
                            ready_tail = ready_tail->next;
                            ready_tail->next = NULL;
                        }else if (count==1){
                            ready_head->next = temp;
                            ready_tail = temp;
                            ready_tail->next = NULL;
                        }
                        quantum_remaining=quantum;
                    }
                }else{
                    if (!strcmp(p->state,"R1")){
                        p->runtime += 1;
                        p->needtime -= 1;
                        timer += 1;
                        if (p->needtime == 0){
                            strcpy(p->state, "B1");
                            p->runtime = 0;
                            p->needtime = atoi(argv[4*p->id +1]);
                            ready_head->next = p->next;
                            p->next = block_head->next;
                            block_head->next = p;
                        }
                        block_time(argv);
                        quantum_remaining -= 1;
                    }else if(!strcmp(p->state,"R2")) {
                        p->runtime += 1;
                        p->needtime -= 1;
                        timer += 1;
                        if (p->needtime == 0) {
                            strcpy(p->state, "B2");
                            p->runtime = 0;
                            p->needtime = atoi(argv[4 * p->id + 3]);
                            ready_head->next = p->next;
                            p->next = block_head->next;
                            block_head->next = p;
                        }
                        block_time(argv);
                        quantum_remaining -= 1;
                    }
                    printf("%-10d %-10d",timer,p->id);

                }
            }
            p = ready_head->next;
        }
    }
}



void SA_FIFO(char **argv) {
    //    using First Come First Serve
    PCB *p;
    p = ready_head;

    while (1) {
        if (count == 0) {
            printf("There is no process!\n");
            return;
        }else {
            if(p==NULL) {
                block_time(argv);
                timer++;
                p = ready_head;
                if(p==NULL){
                printCycle();
                }else{
                    timer--;
                }
                continue;
            }

            if (!strcmp(p->state, "R1")) {
                if (p->needtime == 0) {
                    strcpy(p->state, "B1");
                    p->runtime = 0;
                    p->needtime = atoi(argv[4 * (p->id - 1) + 1]);
                    ready_head = p->next;
                    p->next = NULL;
                    if (block_tail == NULL) {
                        block_tail = block_head = p;
                    } else {
                        block_tail->next = p;
                        block_tail = p;
                    }
                    p = ready_head;
                    continue;
                }else {
                    p->runtime += 1;
                    p->needtime -= 1;
                    block_time(argv);
                    timer++;
                }


            } else if (!strcmp(p->state, "R2")) {
                if (p->needtime == 0) {
                    strcpy(p->state, "B2");
                    p->runtime = 0;
                    p->needtime = atoi(argv[4 * (p->id - 1) + 3]);
                    ready_head = p->next;
                    p->next = NULL;
                    if (block_tail == NULL) {
                        block_tail = block_head = p;
                    } else {
                        block_tail->next = p;
                        block_tail = p;
                    }
                    p = ready_head;
                    continue;
                }else{
                p->runtime += 1;
                p->needtime -= 1;
                block_time(argv);
                timer++;
                }

            } else if (!strcmp(p->state, "terminate")) {
                if (p->needtime == 0) {
                    count --;
                    ready_head = p->next;
                    if(ready_tail==p){
                        ready_tail = NULL;
                    }
                    free(p);
                    p = ready_head;
                    continue;
                }else{
                    p->runtime += 1;
                    p->needtime -= 1;
                    block_time(argv);
                    timer++;
                }
            }
            printCycle();
        }
        p = ready_head;
    }
}


void SA_RR(char **argv) {
    //    using First Come First Serve
    PCB *p;
    p = ready_head;

    while (1) {
        if (count == 0) {
            printf("There is no process!\n");
            return;
        }else {
            if(p==NULL) {
                block_time(argv);
                timer++;
                p = ready_head;
                if(p==NULL){
                    printf("here is the situation that p==NULL!\n");
                    printCycle();
                    printReadyList();
                    printBlockList();
                }else{
                    timer--;
                }
                continue;
            }

            if(p->remaining_quantum<=0 && p->needtime > 0){
                p->remaining_quantum = quantum;
                ready_head = p->next;
                ready_tail->next = p;
                ready_tail = p;
                ready_tail->next = NULL;
                p = ready_head;
            }

            if (!strcmp(p->state, "R1")) {
                if (p->needtime == 0) {
                    strcpy(p->state, "B1");
                    p->runtime = 0;
                    p->needtime = atoi(argv[4 * (p->id - 1) + 1]);
                    ready_head = p->next;
                    p->next = NULL;
                    if (block_tail == NULL) {
                        block_tail = block_head = p;
                    } else {
                        block_tail->next = p;
                        block_tail = p;
                    }
                    p = ready_head;
                    continue;
                }else {
                    p->runtime += 1;
                    p->needtime -= 1;
                    p->remaining_quantum -= 1;
                    block_time(argv);
                    timer++;
                }


            } else if (!strcmp(p->state, "R2")) {
                if (p->needtime == 0) {
                    strcpy(p->state, "B2");
                    p->runtime = 0;
                    p->needtime = atoi(argv[4 * (p->id - 1) + 3]);
                    ready_head = p->next;
                    p->next = NULL;
                    if (block_tail == NULL) {
                        block_tail = block_head = p;
                    } else {
                        block_tail->next = p;
                        block_tail = p;
                    }
                    p = ready_head;
                    continue;
                }else{
                    p->runtime += 1;
                    p->needtime -= 1;
                    p->remaining_quantum -= 1;
                    block_time(argv);
                    timer++;
                }

            } else if (!strcmp(p->state, "terminate")) {
                if (p->needtime == 0) {
                    count --;
                    ready_head = p->next;
                    if(ready_tail==p){
                        ready_tail = NULL;
                    }
                    free(p);
                    p = ready_head;
                    continue;
                }else{
                    p->runtime += 1;
                    p->needtime -= 1;
                    p->remaining_quantum -= 1;
                    block_time(argv);
                    timer++;
                }
            }
            printCycle();
            printReadyList();
            printBlockList();
        }
        p = ready_head;
    }
}

int main(int argc, char **argv){
    char **args;
    if (argv[1] > 0){
        count = atoi(argv[1]);
        if (argv[2][0] == '1'){
            args = &argv[3];
            initiateProcesses(args);
            SA_FIFO(args);
        }else if(argv[2][0] == '2'){
            args = &argv[4];
            quantum = atoi(argv[3]);
            initiateProcesses(args);
            SA_RR(args);
        }else{
            printf("the number of processes should be larger than 0!\n");
            exit(0);
        }
    }
    printReadyList();
    freeReadyList();
    freeBlockList();
}
