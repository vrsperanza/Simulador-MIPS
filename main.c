#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>

// ************************** HELPER FUNCTIONS ***************************
void sem_guarantee(sem_t * sem){
	sem_wait(sem);
	sem_post(sem);
}

// ****************************** MUX 1 BIT ******************************
typedef struct MUX_1bit{
	pthread_t * thread;
	sem_t * begin;
	sem_t * done;
	char * option;
	int * input0;
	int * input1;
	int * result;
} MUX_1bit;

void* MUX_1bit_main(void * args){
	MUX_1bit * signals = (MUX_1bit*)args;
	while(1){
		sem_wait(signals->begin);
		
		switch(*(signals->option)){
			case 0:
				*(signals->result) = *(signals->input0);
				break;
			case 1:
				*(signals->result) = *(signals->input1);
				break;
		}
		
		sem_post(signals->done);
	}
}

MUX_1bit * MUX_1bit_init(char * option, int * result, int * input0, int * input1){
	MUX_1bit * mux = (MUX_1bit*)malloc(sizeof(MUX_1bit));
	
    mux->thread = (pthread_t*)malloc(sizeof(pthread_t));
	
	mux->begin = (sem_t*)malloc(sizeof(sem_t));
	mux->done = (sem_t*)malloc(sizeof(sem_t));
	
    sem_init(mux->begin, 0, 0);
    sem_init(mux->done, 0, 0);
	
	mux->option = option;
	mux->input0 = input0;
	mux->input1 = input1;
	mux->result = result;
	
    pthread_create(mux->thread,NULL,MUX_1bit_main,(void*)mux);
	
	return mux;
}


// ****************************** MUX 2 BITS ******************************
typedef struct MUX_2bits{
	pthread_t * thread;
	sem_t * begin;
	sem_t * done;
	char * option;
	int * input0;
	int * input1;
	int * input2;
	int * input3;
	int * result;
} MUX_2bits;

void* MUX_2bits_main(void * args){
	MUX_2bits * signals = (MUX_2bits*)args;
	while(1){
		sem_wait(signals->begin);
		
		int op = *(signals->option)
		
		switch(*(signals->option)){
			case 0:
				*(signals->result) = *(signals->input0);
				break;
			case 1:
				*(signals->result) = *(signals->input1);
				break;
			case 2:
				*(signals->result) = *(signals->input2);
				break;
			case 3:
				*(signals->result) = *(signals->input3);
				break;
		}
		
		sem_post(signals->done);
	}
}

MUX_2bits * MUX_2bits_init(char * option, int * result, int * input0, int * input1, int * input2, int * input3){
	MUX_2bits * mux = (MUX_2bits*)malloc(sizeof(MUX_2bits));
	
    mux->thread = (pthread_t*)malloc(sizeof(pthread_t));
	
	mux->begin = (sem_t*)malloc(sizeof(sem_t));
	mux->done = (sem_t*)malloc(sizeof(sem_t));
	
    sem_init(mux->begin, 0, 0);
    sem_init(mux->done, 0, 0);
	
	mux->option = option;
	mux->input0 = input0;
	mux->input1 = input1;
	mux->input2 = input2;
	mux->input3 = input3;
	mux->result = result;
	
    pthread_create(mux->thread,NULL,MUX_2bits_main,(void*)mux);
	
	return mux;
}

// ******************************* REGISTER *******************************
typedef struct REGISTER{
	pthread_t * thread;
	sem_t * begin;
	sem_t * done;
	int * input;
	int * output;
} REGISTER;

void* REGISTER_main(void * args){
	REGISTER * signals = (REGISTER*)args;
	while(1){
		sem_wait(signals->begin);
		
		*(signals->output) = *(signals->input);
		
		sem_post(signals->done);
	}
}

REGISTER * REGISTER_init(int * input, int * output){
	REGISTER * reg = (REGISTER*)malloc(sizeof(REGISTER));
	
    reg->thread = (pthread_t*)malloc(sizeof(pthread_t));
	
	reg->begin = (sem_t*)malloc(sizeof(sem_t));
	reg->done = (sem_t*)malloc(sizeof(sem_t));
	
    sem_init(reg->begin, 0, 0);
    sem_init(reg->done, 0, 0);
	
	reg->input = input;
	reg->output = output;
	
    pthread_create(reg->thread,NULL,REGISTER_main,(void*)reg);
	
	return reg;
}

// ********************************* ALU **********************************
typedef struct ALU{
	pthread_t * thread;
	sem_t * begin;
	sem_t * done;
	sem_t * dependency0;
	sem_t * dependency1;
	
	char * option;
	int * result;
	char * zero;
	int * input0;
	int * input1;
} ALU;

void* ALU_main(void * args){
	ALU * signals = (ALU*)args;
	while(1){
		sem_wait(signals->begin);
		
		sem_guarantee(signals->dependency0);
		sem_guarantee(signals->dependency1);
		
		switch(*(signals->option)){
			case 0:
				*(signals->result) = *(signals->input0) + *(signals->input1);
				break;
			case 1:
				*(signals->result) = *(signals->input0) - *(signals->input1);
				break;
			case 2:
				*(signals->result) = *(signals->input0) * *(signals->input1);
				break;
			case 3:
				*(signals->result) = *(signals->input0) / *(signals->input1);
				break;
		}
		
		*(signals->zero) = (*(signals->result) == 0);
		
		sem_post(signals->done);
	}
}

ALU * ALU_init(char * option, int * result, char * zero, int * input0, int * input1, sem_t * dependency0, sem_t * dependency1){
	ALU * alu = (ALU*)malloc(sizeof(ALU));
	
    alu->thread = (pthread_t*)malloc(sizeof(pthread_t));
	
	alu->begin = (sem_t*)malloc(sizeof(sem_t));
	alu->done = (sem_t*)malloc(sizeof(sem_t));
	
    sem_init(alu->begin, 0, 0);
    sem_init(alu->done, 0, 0);
	
	alu->option = option;
	alu->result = result;
	alu->zero = zero;
	alu->input0 = input0;
	alu->input1 = input1;
	alu->dependency0 = dependency0;
	alu->dependency1 = dependency1;

		int intOp1, intOp2, intOp3;
		scanf("%d%d%d%d", &A, &intOp1, &intOp2, &intOp3);
		mux1Op = (char)intOp1;
		mux2Op = (char)intOp2;
		aluOp = (char)intOp3;
		
		// Run operations
		sem_post(mux1->begin);
		sem_post(mux2->begin);
		sem_post(alu->begin);
		
		sem_guarantee(mux1->done);
		sem_guarantee(mux2->done);
		sem_guarantee(alu->done);
		
		sem_wait(alu->done);
		sem_wait(mux1->done);
		sem_wait(mux2->done);
		
		printf("%d %d %d\n", aluResult, (int)aluZero, aluOut);
		
		// Update registers
		sem_post(aluOutReg->begin);
		
		
		sem_wait(aluOutReg->done);
		
		
		printf("%d %d %d\n", aluResult, (int)aluZero, aluOut);
	}
	
    return 0;
}