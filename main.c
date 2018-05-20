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

// ****************************** SIGN EXTEND ******************************
typedef struct SIGN_EXTEND{
	pthread_t * thread;
	sem_t * begin;
	sem_t * done;
	sem_t * dependency;
	int * input;
	int * output;
} SIGN_EXTEND;

void* SIGN_EXTEND_main(void * args){
	SIGN_EXTEND * signals = (SIGN_EXTEND*)args;
	while(1){
		sem_wait(signals->begin);

		sem_guarantee(signals->dependency);

		*(signals->output) = (int)(short)*(signals->input);

		sem_post(signals->done);
	}
}

SIGN_EXTEND * SIGN_EXTEND_init(int * input, int * output, sem_t * dependency){
	SIGN_EXTEND * sign = (SIGN_EXTEND*)malloc(sizeof(SIGN_EXTEND));

    sign->thread = (pthread_t*)malloc(sizeof(pthread_t));

	sign->begin = (sem_t*)malloc(sizeof(sem_t));
	sign->done = (sem_t*)malloc(sizeof(sem_t));

	sign->dependency = dependency;

    sem_init(sign->begin, 0, 0);
    sem_init(sign->done, 0, 0);

	sign->input = input;
	sign->output = output;

    pthread_create(sign->thread,NULL,SIGN_EXTEND_main,(void*)sign);

	return sign;
}

// ****************************** SHIFT LEFT ******************************
typedef struct SHIFT_LEFT{
	pthread_t * thread;
	sem_t * begin;
	sem_t * done;
	sem_t * dependency;
	int * input;
	int * output;
	int amount;
} SHIFT_LEFT;

void* SHIFT_LEFT_main(void * args){
	SHIFT_LEFT * signals = (SHIFT_LEFT*)args;
	while(1){
		sem_wait(signals->begin);

		sem_guarantee(signals->dependency);

		*(signals->output) = *(signals->input) << signals->amount;

		sem_post(signals->done);
	}
}

SHIFT_LEFT * SHIFT_LEFT_init(int * input, int * output, int amount, sem_t * dependency){
	SHIFT_LEFT * sign = (SHIFT_LEFT*)malloc(sizeof(SHIFT_LEFT));

    sign->thread = (pthread_t*)malloc(sizeof(pthread_t));

	sign->begin = (sem_t*)malloc(sizeof(sem_t));
	sign->done = (sem_t*)malloc(sizeof(sem_t));

	sign->dependency = dependency;

    sem_init(sign->begin, 0, 0);
    sem_init(sign->done, 0, 0);

	sign->input = input;
	sign->output = output;

	sign->amount = amount;

    pthread_create(sign->thread,NULL,SHIFT_LEFT_main,(void*)sign);

	return sign;
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

		int op = *(signals->option);

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

// ****************************** REGISTERS *******************************
typedef struct REGISTERS{
	pthread_t * thread;
	sem_t * begin;
	sem_t * done;
	sem_t * dependency0;
	sem_t * dependency1;
	sem_t * dependency2;

	char * regWrite;

	int * readRegister1;
	int * readRegister2;
	int * writeRegister;
	int * writeData;

	int * readData1;
	int * readData2;

	int * registers;
} REGISTERS;

void * REGISTERS_main(void * args){
	REGISTERS * registers = (REGISTERS*)args;
	while(1){
		sem_wait(registers->begin);

		sem_guarantee(registers->dependency0);
		sem_guarantee(registers->dependency1);
		sem_guarantee(registers->dependency2);

		if(*(registers->regWrite)){
			registers->registers[*(registers->writeRegister)] = *(registers->writeData);
		}
		else{
			*(registers->readData1) = registers->registers[*(registers->readRegister1)];
			*(registers->readData2) = registers->registers[*(registers->readRegister2)];
		}

		sem_post(registers->done);
	}
}

REGISTERS * REGISTERS_init(int * readRegister1, int * readRegister2, int * writeRegister,
						   int * writeData, int * readData1, int * readData2, char * regWrite,
						   sem_t * dependency0, sem_t * dependency1, sem_t * dependency2){
	REGISTERS * registers = (REGISTERS*)malloc(sizeof(REGISTERS));
	registers->readRegister1 = readRegister1;
	registers->readRegister2 = readRegister2;
	registers->writeRegister = writeRegister;
	registers->writeData = writeData;
	registers->readData1 = readData1;
	registers->readData2 = readData2;
	registers->regWrite = regWrite;


	registers->registers = (int*)malloc(sizeof(int) * 32);

    sem_init(registers->begin, 0, 0);
    sem_init(registers->done, 0, 0);

    pthread_create(registers->thread,NULL,REGISTERS_main,(void*)registers);

	return registers;
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

void * ALU_main(void * args){
	ALU * signals = (ALU*)args;
	while(1){
		sem_wait(signals->begin);

		sem_guarantee(signals->dependency0);
		sem_guarantee(signals->dependency1);

		switch(*(signals->option)){
			case 0:
				*(signals->result) = *(signals->input0) & *(signals->input1);
			break;

			case 1:
				*(signals->result) = *(signals->input0) | *(signals->input1);
			break;

			case 2:
				*(signals->result) = *(signals->input0) + *(signals->input1);
			break;

			case 6:
				*(signals->result) = *(signals->input0) - *(signals->input1);
			break;

			case 7:
				*(signals->result) = ( *(signals->input0) < *(signals->input1) );
			break;

			case 12:
				*(signals->result) = !(*(signals->input0) | *(signals->input1));
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

    pthread_create(alu->thread,NULL,ALU_main,(void*)alu);

	return alu;
}

// ******************************* MEMORY *********************************
typedef struct MEMORY{
	pthread_t * thread;
	sem_t * begin;
	sem_t * done;
	sem_t * dependency0;
	sem_t * dependency1;

	char * memRead;
	char * memWrite;
	int * address;
	int * writeData;
	int * output;
	char * memory;
} MEMORY;

void * MEMORY_main(void * args){
	MEMORY * memory = (MEMORY*)args;
	while(1){
		sem_wait(memory->begin);

		sem_guarantee(memory->dependency0);
		sem_guarantee(memory->dependency1);

		if(*(memory->memRead))
			*(memory->output) = memory->memory[*(memory->address)];
		else if(*(memory->memWrite))
			memory->memory[*(memory->address)] = *(memory->writeData);

		sem_post(memory->done);
	}
}

MEMORY * MEMORY_init(char * memRead, char * memWrite, int * address, int * writeData,
					 int * output, sem_t * begin, sem_t * done, sem_t * dependency0,
					 sem_t * dependency1, int byteCapacity){
	MEMORY * memory = (MEMORY*)malloc(sizeof(MEMORY));
	memory->memRead = memRead;
	memory->memWrite = memWrite;
	memory->address = address;
	memory->writeData = writeData;
	memory->output = output;

	memory->dependency0 = dependency0;
	memory->dependency1 = dependency1;

	memory->memory = (char*)malloc(sizeof(char) * byteCapacity);

    sem_init(memory->begin, 0, 0);
    sem_init(memory->done, 0, 0);

    pthread_create(memory->thread,NULL,MEMORY_main,(void*)memory);

	return memory;
}

// ***************************** ALU CONTROL *******************************
typedef struct ALU_CONTROL{
	pthread_t * thread;
	sem_t * begin;
	sem_t * done;
	sem_t * dependency;
	int * input;
	int * output;
	char * option;
} ALU_CONTROL;

void* ALU_CONTROL_main(void * args){
	ALU_CONTROL * signals = (ALU_CONTROL*)args;
	while(1){
		sem_wait(signals->begin);

		sem_guarantee(signals->dependency);

		if(*(signals->option) == 0) {
			//add
			*(signals->option) = 2;
		} else if(*(signals->option) == 1) {
			//subtract
			*(signals->option) = 6;
		} else {
			//Use funct field
			switch (*(signals->input)) {
				case 32:	//add
					*(signals->option) = 2;
				break;

				case 34:	//subtract
					*(signals->option) = 6;
				break;

				case 36:	//AND
					*(signals->option) = 0;
				break;

				case 37:	//OR
					*(signals->option) = 1;
				break;

				case 42:	//set on less than
					*(signals->option) = 7;
				break;
			}
		}

		sem_post(signals->done);
	}
}

ALU_CONTROL * ALU_CONTROL_init(int * input, int * output, char * option, sem_t * dependency){
	ALU_CONTROL * sign = (ALU_CONTROL*)malloc(sizeof(ALU_CONTROL));

    sign->thread = (pthread_t*)malloc(sizeof(pthread_t));

	sign->begin = (sem_t*)malloc(sizeof(sem_t));
	sign->done = (sem_t*)malloc(sizeof(sem_t));

    sem_init(sign->begin, 0, 0);
    sem_init(sign->done, 0, 0);

	sign->input = input;
	sign->output = output;

	sign->option = option;

	sign->dependency = dependency;

    pthread_create(sign->thread,NULL,ALU_CONTROL_main,(void*)sign);

	return sign;
}

// ******************************* CONTROL *********************************
typedef struct CONTROL{
	pthread_t * thread;
	sem_t * begin;
	sem_t * done;
	char currentState;
	char * option;
	int * output;
} CONTROL;

void* CONTROL_main(void * args){
	CONTROL * signals = (CONTROL*)args;
	while(1){
		sem_wait(signals->begin);

		//"s" e "op" são variáveis utilizadas com o único propósito de facilitar a visualização e, consequentemente, a correção do trabalho
		char s = signals->currentState;
		char op = *(signals->option);
		signals->currentState = 0;

		#define	JUMP 0b000010
		#define JR 0b010100
		#define JAL 0b000011
		#define JALR 0b010101
		#define LW 0b100011
		#define SW 0b101011
		#define ADDI 0b001000
		#define RTYPE 0b000000
		#define ANDI 0b001100
		#define BEQ 0b000100
		#define BNE 0b000101

		printf("op = %d\n", (int)op);

		signals->currentState |= ( (s==0) | ((s==1) & ( (op == JUMP)|(op == JR)|(op==JALR) )) | (s==2) | (s==6) | (s==10) );

		signals->currentState |= ( ( (s==1) & ( (op==LW)|(op==SW)|(op==ADDI)|(op==RTYPE)|(op==ANDI)|(op==JAL)|(op==JALR) ) ) |
															 ( (s==2) & (op!=SW) ) | (s==6) | (s==10)	) << 1;

		signals->currentState |= ( ( (s==1) & ( (op==RTYPE)|(op==BNE)|(op==JR)|(op==JAL)|(op==JALR) ) ) |
			 													 (s==3) | ( (s==2) & (op==SW) ) | (s==6) ) << 2;

		signals->currentState |= ( ( (s==1) & ( (op==BEQ)|(op==JUMP)|(op==ANDI)|(op==BNE)|(op==JR)|(op==JAL)|(op==JALR) ) ) |
	 															((s==2) & (op==ADDI)) | (s==10) ) << 3;

		printf("estado anterior= %d\n", (int)s);
		s = signals->currentState;

		printf("estado atual= %d\n", (int)s);

		*(signals->output) = 0;
		*(signals->output) |= (s==7);	//regDST0
		*(signals->output) |= ((s==14) | (s==15)) << 1; //regDST1
		*(signals->output) |= ((s==4) | (s==7) | (s==11) | (s==14) | (s==15)) << 2;	//regWrite
		*(signals->output) |= ((s==2) | (s==6) | (s==8) | (s==10) | (s==12)) << 3;	//ALUSrcA
		*(signals->output) |= ((s==0) | (s==1)) << 4; //ALUSrcB0
		*(signals->output) |= ((s==1) | (s==2) | (s==10)) << 5;	//ALUSrcB1
		*(signals->output) |= ((s==8) | (s==12)) << 6;	//ALUOp0
		*(signals->output) |= ((s==6) | (s==10)) << 7;	//ALUOp1
		*(signals->output) |= ((s==8) | (s==12) | (s==13) | (s==15)) << 8;	//PCSource0
		*(signals->output) |= ((s==9) | (s==13) | (s==14) | (s==15)) << 9;	//PCSource1
		*(signals->output) |= ((s==8) | (s==12)) << 10;	//PCWriteCond
		*(signals->output) |= ((s==0) | (s==9) | (s==13) | (s==14) | (s==15)) << 11;	//PCWrite
		*(signals->output) |= ((s==3) | (s==5)) << 12;	//IorD
		*(signals->output) |= ((s==0) | (s==3)) << 13;	//MemRead
		*(signals->output) |= (s==5) << 14;	//MemWrite
		*(signals->output) |= (s==12) << 15;	//BNE
		*(signals->output) |= (s==0) << 16;	//IRWrite
		*(signals->output) |= (s==4) << 17;	//MemtoReg0
		*(signals->output) |= ((s==14) | (s==15)) << 18;	//MemtoReg1

		sem_post(signals->done);
	}
}

CONTROL * CONTROL_init(int * output, char * option){
	CONTROL * sign = (CONTROL*)malloc(sizeof(CONTROL));

    sign->thread = (pthread_t*)malloc(sizeof(pthread_t));

	sign->begin = (sem_t*)malloc(sizeof(sem_t));
	sign->done = (sem_t*)malloc(sizeof(sem_t));

    sem_init(sign->begin, 0, 0);
    sem_init(sign->done, 0, 0);

	sign->output = output;

	sign->option = option;

	sign->currentState = 0;

    pthread_create(sign->thread,NULL,CONTROL_main,(void*)sign);

	return sign;
}
// ******************************** MAIN **********************************
int main()
{
	char option = 0;
	int output = 0;

	CONTROL * c = CONTROL_init(&output, &option);

	while(1){
		option=0;
		for(int i = 0; i < 6; i++){
			int x;
			scanf("%d", &x);
			option <<= 1;
			option |= x;

		}
			sem_post(c->begin);

		sem_wait(c->done);
	  for(int i=0; i < 32; i++){
			printf("%d", output%2);
			output >>= 1;
		}
	}

    return 0;
}
