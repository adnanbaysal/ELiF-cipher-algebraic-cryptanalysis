#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define u64 unsigned long long

clock_t clk;

u64 rconsts[63] = {
0,0,0,0,0,0xffffffffffffffffL,0,0,0,0,0xffffffffffffffffL,0xffffffffffffffffL,0,0,0,
0xffffffffffffffffL,0,0xffffffffffffffffL,0,0,0xffffffffffffffffL,0xffffffffffffffffL,
0xffffffffffffffffL,0xffffffffffffffffL,0,0xffffffffffffffffL,0,0,0,0xffffffffffffffffL,
0xffffffffffffffffL,0xffffffffffffffffL,0,0,0xffffffffffffffffL,0,0,0xffffffffffffffffL,
0,0xffffffffffffffffL,0xffffffffffffffffL,0,0xffffffffffffffffL,0xffffffffffffffffL,
0xffffffffffffffffL,0,0xffffffffffffffffL,0xffffffffffffffffL,0,0,0xffffffffffffffffL,
0xffffffffffffffffL,0,0xffffffffffffffffL,0,0xffffffffffffffffL,0,0xffffffffffffffffL,
0xffffffffffffffffL,0xffffffffffffffffL,0xffffffffffffffffL,0xffffffffffffffffL,0xffffffffffffffffL};

typedef struct multi_block{
	u64 data[64];
} MB;

void elf64round(MB *states,u64 *key,unsigned *index){
	states->data[(index[0]+2)&63] ^= ((states->data[(index[0]+1)&63])&(states->data[index[0]]))^key[0];
	index[0] = (index[0]+1)&63;
}

void elf64enc(MB *states,u64 *keys,unsigned rounds){
	unsigned i, index = 0;
	for(i=0;i<rounds;i++){
		//printf("3.0.1.%d, index = %d\n",i,index);
		elf64round(states,keys+i,&index);
	}
}

void key_schedule(u64 key,u64 *rkeys,unsigned rounds){
	unsigned i;
	for(i=0;i<62;i++){
		if((key>>i)&1) rkeys[i] = 0xffffffffffffffffL;
		//else rkeys[i] = 0;
	}
	for(i=62;i<rounds;i++) rkeys[i] = rkeys[i-62];
	for(i=0;i<rounds;i++) rkeys[i] ^= rconsts[i%63];
}

#define ROUNDS	7936

int main(int argc,char **argv){
	srand(atoi(argv[1]));
	MB *states;
	states = (MB*)calloc(1,sizeof(MB));
	u64 i, j, key = 0, rkeys[ROUNDS] = {0}, sum = 0;
	for(i=0;i<4;i++) key = (key<<16)^rand();
	printf("1\n");
	key_schedule(key,rkeys,ROUNDS);
	printf("2\n");
	clk = clock();
	printf("3\n");
	for(i=0;i<1000000;i++){
		if((i%1000)==0) printf("3.%d\n",i);
		for(j=0;j<64;j++) states->data[j] = i^j;
		//printf("3.%d.1\n",i);
		elf64enc(states,rkeys,ROUNDS);
		//printf("3.%d.2\n",i);
		for(j=0;j<64;j+=8) sum ^= states->data[j];
	}
	printf("4\n");
	float elapsed = (((float)clock())-((float)clk))/((float)CLOCKS_PER_SEC);
	printf("sum = %08X%08X @ %f seconds\n",sum>>32,sum&0xffffffff,elapsed);
	return 0;
}
