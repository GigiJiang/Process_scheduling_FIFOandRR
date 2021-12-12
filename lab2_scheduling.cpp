//
// Created by Ziyi Jiang on 2021/12/3.
//

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <deque>
using namespace std;

int quantum = -1;
int process_count = 0;
int timer = 0;


typedef enum STATE{
    R1 = 0, B1, R2, B2, TER
}STATE;

string state_str[] = {
    "R1", "B1", "R2", "B2", "ter"
};

struct PCB{
    int id;
    int arrival_time;
    int runtime;
    int needtime;
    int remaining_quantum;
    STATE state;
    PCB& operator = (const PCB & p){
        this->id = p.id;
        this->arrival_time = p.arrival_time;
        this->runtime = p.runtime;
        this->needtime = p.needtime;
        this->remaining_quantum = p.remaining_quantum;
        this->state = p.state;
        return *this;
    }
};

deque<PCB> ReadyQueue;
deque<PCB> BlockQueue;

void initiateProcesses(char **argv){
    PCB p;
    int i;

    printf("there are %d processes\n",process_count);
    printf("-----------Simulation Starts-------------\n");
    printf("Cycles\tRunning\tReady\tblock\n");

    for (i=1;i<=process_count;i++){
        p.id = i;
        p.arrival_time = 0;
        p.runtime = 0;
        p.needtime = atoi(argv[4*(i-1)]);
        p.state = R1;
        if(quantum>0){
            p.remaining_quantum = quantum;
        }
        ReadyQueue.push_back(p);
    }
}

void printReadyList(){
    PCB *p;
    printf("Ready List: ");
    for (int i = 0;i<ReadyQueue.size();i++){
        p = &ReadyQueue[i];
        printf("%d, %d, %d, %d, %d, %s\t",p->id,p->arrival_time,p->runtime,p->needtime,p->remaining_quantum,state_str[p->state].c_str());
    }
    printf("\n");
}

void printBlockList(){
    PCB *p;
    printf("Block List: ");
    for (int i = 0;i<BlockQueue.size();i++){
        p = &BlockQueue[i];
        printf("%d, %d, %d, %d, %d, %s\t",p->id,p->arrival_time,p->runtime,p->needtime,p->remaining_quantum,state_str[p->state].c_str());
    }
    printf("\n");
}

void printCycle(){
    PCB *p;

    printf("%d\t",timer);
    if(ReadyQueue.size() >0){
        p = &ReadyQueue[0];
        if (p->state == TER)
            printf("P%d(ter)\t",p->id);
        else printf("P%d\t",p->id);
        for(int i=1;i<ReadyQueue.size();i++){
            p = &ReadyQueue[i];
            printf("P%d ",p->id);
        }
    }else{
        printf("\t");
    }
    printf("\t");

    for(int i = 0;i<BlockQueue.size();i++){
        p = &BlockQueue[i];
        printf("P%d ",p->id);
    }
    printf("\n");
}

void block_time(char **argv){
    PCB *p,temp,temp2;
    int size = BlockQueue.size();

    for(int i = 0; i < size;i++){
        p = &BlockQueue[i];
        if (p->needtime >= 1){
            p->needtime -= 1;
            p->runtime += 1;
            continue;
        }else {
            temp = *p;
            BlockQueue.erase(BlockQueue.begin() + i);
            size = BlockQueue.size();
        }

        if (temp.state == B1){
            temp.needtime = atoi(argv[4*(temp.id-1)+2]);
            temp.runtime = 0;
            temp.arrival_time = timer;
            temp.remaining_quantum = quantum;
            temp.state = R2;

            int position;
            for(position = 0; position < ReadyQueue.size();position++){
                if(ReadyQueue[position].arrival_time == timer && ReadyQueue[position].id >temp.id) {
                    break;
                }
            }
            ReadyQueue.insert(ReadyQueue.begin()+position, temp);

        }else if (temp.state == B2){
            temp.needtime = 1;
            temp.runtime = 0;
            temp.arrival_time = timer;
            temp.remaining_quantum = quantum;
            temp.state = TER;

            int position;
            for(position = 0; position < ReadyQueue.size();position++){
                if(ReadyQueue[position].arrival_time == timer && ReadyQueue[position].id >temp.id) {
                    break;
                }
            }
            ReadyQueue.insert(ReadyQueue.begin()+position,temp);
        }
    }
}

void SA_FIFO(char **argv) {
    //    using First Come First Serve
    PCB *p;
    int rsize = ReadyQueue.size();
    int bsize = BlockQueue.size();

    while (1) {
        if (process_count == 0) {
            printf("-----------Simulation Ends--------------\n");
            return;
        }else {
            if (rsize == 0) {
                block_time(argv);
                timer++;
                rsize = ReadyQueue.size();
                if (rsize == 0) {
                    printCycle();
                } else {
                    timer--;
                }
                continue;
            }
        }

        p = &ReadyQueue[0];

        if (p->state == R1){
            if (p->needtime == 0) {
                p->state = B1;
                p->runtime = 0;
                p->needtime = atoi(argv[4 * (p->id - 1) + 1]);
                BlockQueue.push_back(*p);
                ReadyQueue.pop_front();
                rsize = ReadyQueue.size();
                continue;
            }else {
                p->runtime += 1;
                p->needtime -= 1;
                block_time(argv);
                timer++;
            }


        } else if (p->state==R2) {
            if (p->needtime == 0) {
                p->state = B2;
                p->runtime = 0;
                p->needtime = atoi(argv[4 * (p->id - 1) + 3]);
                BlockQueue.push_back(*p);
                ReadyQueue.pop_front();
                rsize = ReadyQueue.size();
                continue;
            }else{
                p->runtime += 1;
                p->needtime -= 1;
                block_time(argv);
                timer++;
            }

        } else if (p->state == TER){
            if (p->needtime == 0) {
                process_count--;
                ReadyQueue.pop_front();
                rsize = ReadyQueue.size();
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
    rsize = ReadyQueue.size();
}

void SA_RR(char **argv) {
    //    using Round Robin
    PCB *p;
    int rsize = ReadyQueue.size();
    int bsize = BlockQueue.size();

    while (1) {
        if(process_count == 0){
            printf("-----------Simulation Ends--------------\n");
            return;
        }else {
            if (rsize == 0) {
                block_time(argv);
                timer++;
                rsize = ReadyQueue.size();
                if (rsize == 0) {
                    printCycle();
                } else {
                    timer--;
                }
                continue;
            }
        }

        p = &ReadyQueue[0];

        if(p->remaining_quantum <=0 && p->needtime > 0){
            p->remaining_quantum = quantum;
            ReadyQueue.push_back(*p);
            ReadyQueue.pop_front();
            p = &ReadyQueue[0];
        }



        if (p->state == R1){
            if (p->needtime == 0) {
                p->state = B1;
                p->runtime = 0;
                p->needtime = atoi(argv[4 * (p->id - 1) + 1]);
                p->remaining_quantum = quantum;
                BlockQueue.push_back(*p);
                ReadyQueue.pop_front();
                rsize = ReadyQueue.size();
                continue;
            }else {
                p->runtime += 1;
                p->needtime -= 1;
                p->remaining_quantum -= 1;
                block_time(argv);
                timer++;
            }


        } else if (p->state==R2) {
            if (p->needtime == 0) {
                p->state = B2;
                p->runtime = 0;
                p->needtime = atoi(argv[4 * (p->id - 1) + 3]);
                p->remaining_quantum = quantum;
                BlockQueue.push_back(*p);
                ReadyQueue.pop_front();
                rsize = ReadyQueue.size();
                continue;
            }else{
                p->runtime += 1;
                p->needtime -= 1;
                p->remaining_quantum -= 1;
                block_time(argv);
                timer++;
            }

        } else if (p->state == TER){
            if (p->needtime == 0) {
                process_count--;
                ReadyQueue.pop_front();
                rsize = ReadyQueue.size();
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
    }
    rsize = ReadyQueue.size();
}


int main(int argc, char **argv){
    char **args;
    if (atoi(argv[1]) > 0){
        process_count = atoi(argv[1]);
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
}