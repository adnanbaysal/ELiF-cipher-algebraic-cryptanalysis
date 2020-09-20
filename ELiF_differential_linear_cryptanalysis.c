#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define u32 unsigned

/*constants : x^4+x+1 start at 0001, shift left, xor leftmost 2 bit*/
u32 and_table[4] = {0,0,0,4};
u32 BIT_SIZE = 8, MAX_VEC = 256, MSB = 128, ONES = 255;
int quanta[2] = {1,-1};
u32 rconsts[] = {
4,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,4,4,0,4,4,0,0,0,0,4,
0,4,0,0,0,4,0,4,4,4,4,0,4,4,0,4,4,4,4,4,4,4,0,0,0,0,4,4,0,0,0,4,
0,0,4,4,0,4,4,0,0,4,0,0,4,4,0,0,0,4,4,0,0,0,4,4,4,0,0,0,4,4,0,0,
0,0,0,0,0,0,0,4,4,0,4,0,0,0,0,0,4,0,4,0,4,4,4,4,0,4,4,4,0,0,4,4,
4,4,4,0,4,0,0,4,4,0,4,0,4,4,4,4,0,4,4,0,4,0,4,4,4,4,4,0,0,0,4,0,
4,4,4,0,4,0,0,0,4,4,4,4,0,4,4,4,4,0,0,0,4,4,4,0,4,4,4,0,0,0,0,0,
0,4,0,4,0,0,0,4,0,0,4,4,4,0,4,4,0,4,0,0,4,0,0,4,0,0,0,4,4,0,0,0,
0,0,0,4,0,0,4,4,0,4,0,0,4,4,0,0,4,4,0,4,4,0,4,4,4,4,4,4,0,0,0,0,
0,4,4,0,0,4,0,0,4,0,4,0,4,4,4,0,0,0,0,4,0,0,4,4,0,0,0,4,4,4,0,0,
4,4,4,0,0,0,0,0,0,4,4,0,0,0,0,4,0,0,4,0,4,4,0,4,4,4,0,0,0,4,4,0,
0,0,4,0,0,0,0,0,4,4,0,0,4,0,4,4,0,4,0,4,4,4,0,4,4,0,0,4,0,4,4,0,
4,0,0,4,4,0,4,4,0,0,4,4,0,4,4,0,0,0,4,4,4,4,0,0,0,4,0,0,0,0,4,4,
0,0,0,4,0,4,4,4,0,4,4,0,0,4,4,4,4,0,4,0,0,4,4,4,0,4,4,4,4,4,0,4,
0,0,4,0,4,4,0,4,4,4,4,0,0,4,4,0,0,0,4,4,4,0,4,4,4,4,0,0,0,0,0,4,
0,4,4,4,0,0,4,0,4,4,4,4,4,0,0,4,0,4,4,4,4,0,4,0,4,0,4,4,4,4,4,4,
4,0,0,0,4,4,4,0,0,0,4,0,0,0,0,0,0,0,0,0,4,0,4,4,0,0,0,0,0,4,4,4,
4,0,4,0,4,0,4,0,0,4,4,4,4,0,0,0,0,4,0,4,0,4,4,0,0,4,4,4,4,0,0,4,
0,4,4,4,0,4,4,0,4,0,4,4,4,0,4,0,0,0,4,0,4,4,0,4,4,4,4,0,4,4,4,0,
0,0,4,4,4,4,0,4,0,0,0,0,0,0,4,4,4,4,4,4,4,0,0,4,0,0,4,0,0,0,4,0,
4,0,0,0,0,0,4,0,4,4,0,4,4,4,0,4,4,4,4,0,0,0,4,0,4,0,4,4,4,0,0,0,
4,4,0,0,4,4,0,0,0,0,0,4,4,4,4,4,4,0,4,0,4,0,0,4,0,0,4,4,4,0,0,4,
0,0,0,0,4,0,0,0,4,0,0,4,4,4,4,0,4,0,4,0,0,4,0,4,4,4,4,0,0,4,0,0,
4,4,4,4,4,0,4,0,0,0,4,0,4,0,4,4,4,4,4,0,4,4,0,0,4,4,4,0,4,4,0,0,
4,4,4,0,0,4,0,0,4,4,4,0,0,0,4,0,0,0,4,0,0,0,0,0,4,0,4,0,4,0,4,4,
0,4,4,4,0,0,0,0,4,0,0,0,4,0,0,0,4,4,4,0,4,0,4,0,4,0,0,0,0,4,4,0,
0,0,0,4,4,4,4,0,4,4,0,4,4,0,0,4,4,4,0,0,0,0,0,4,4,4,0,0,0,0,4,0,
4,0,0,0,0,0,4,4,4,4,0,4,4,4,0,4,0,0,4,4,4,0,4,0,4,4,4,0,4,0,0,4,
4,0,4,4,0,4,4,4,0,4,4,0,0,0,0,0,4,0};

inline void l2r_round(u32 *data,u32 key){
	u32 temp = (data[0]&3);
	data[0] ^= and_table[temp]^key;
	temp = ((data[0]&MSB)?1:0);
	data[0] <<= 1;
	data[0] ^= temp;
	data[0] &= ONES;
}

inline void l2r_multi_round(u32 *data,u32 *key,u32 rounds){
	u32 i;
	for(i=0;i<rounds;i++) l2r_round(data,key[i]);
}

inline void gen_sbox_by_keyNround(u32 *sbox,u32 *key,u32 rounds){
	u32 i;
	for(i=0;i<MAX_VEC;i++){
		sbox[i] = (u32)i;
		l2r_multi_round(sbox+i,key,rounds);
	}
}

inline u32 find_max_DDT(u32 *sbox){
	u32 i, j, max = 0, *ddt;
	for(i=1;i<MAX_VEC;i++){
		ddt = (u32*)calloc(MAX_VEC,sizeof(u32));
		for(j=0;j<MAX_VEC;j++) ddt[sbox[j]^sbox[j^i]]++;
		for(j=1;j<MAX_VEC;j++) if(ddt[j]>max) max = ddt[j];
		free(ddt);
	}
	return max;
}

inline int parity(int inp){
   int sum = 0;
   while(inp!=0){
      sum ^= inp&1;
      inp >>= 1;
   }
   return sum;
}

void gen_parity_table(int *par_table,int bitsize){
   int i, maxvec = 1<<bitsize;
   for(i=0;i<maxvec;i++) par_table[i] = parity(i);
}

int find_lat_max_fwalsh(int *sbox,int *par_table,int bitsize){
   int *transform, omask, inp, step, temp, maxcorr = 0, maxvec = 1<<bitsize;
   for(omask=1;omask<maxvec;omask++){
      transform = (int*)calloc(maxvec,sizeof(int));
      for(inp=0;inp<maxvec;inp++) transform[inp] = quanta[par_table[omask&sbox[inp]]];
      for(step=1;step<maxvec;step<<=1){
         for(inp=0;inp<maxvec;inp++){
            if((step&inp)==0){
               temp = transform[inp];
               transform[inp] += transform[inp^step];
               transform[inp^step] = temp - transform[inp^step];
            }
         }
      }
      for(inp=0;inp<maxvec;inp++) if(abs(transform[inp])>maxcorr) maxcorr = abs(transform[inp]);
      free(transform);
   }
   return maxcorr;
}

int main(int argc,char **argv){//./a.exe MODE BIT_SIZE KEY_SIZE TEST_SIZE PRINT OFILE SEED
	u32 i, j, k, t, *sums, max_num_rounds, *KEY, *key, num_rounds, *sbox, max_DDT, max_LAT, **histograms;
	u32 MODE =  atoi(argv[1]), BIT_SIZE = atoi(argv[2]), KEY_SIZE = atoi(argv[3]);
	u32 TEST_SIZE =  atoi(argv[4]), PRINT = atoi(argv[5]), SEED = atoi(argv[7]);
	srand(SEED);
	int *parity;
	FILE *ofile;
	ofile = fopen(argv[6],"w");
	MAX_VEC = 1<<BIT_SIZE;
	ONES = MAX_VEC-1;
	MSB = MAX_VEC>>1;
	max_num_rounds = BIT_SIZE*BIT_SIZE*2;
	parity = (int*)calloc(MAX_VEC,sizeof(int));
	key = (u32*)calloc(max_num_rounds,sizeof(u32));
	KEY = (u32*)calloc(KEY_SIZE,sizeof(u32));
	sums = (u32*)calloc(4*BIT_SIZE,sizeof(u32));//[dif-lin],[dif-lin],...
	histograms = (u32**)calloc(TEST_SIZE,sizeof(u32*));
	for(i=0;i<TEST_SIZE;i++) histograms[i] = (u32*)calloc(4*BIT_SIZE,sizeof(u32));
	printf("MODE = %d, BIT_SIZE = %d, KEY_SIZE = %d, TEST_SIZE = %d, OFILE = %s\n",
		MODE,BIT_SIZE,KEY_SIZE,TEST_SIZE,argv[6]);
	gen_parity_table(parity,BIT_SIZE);
	for(k=0;k<TEST_SIZE;k++){
		for(i=0;i<KEY_SIZE;i++){
			if(MODE==0) KEY[i] = ((k>>i)&1)*4;
			else KEY[i] = rand()&4;
		}
		for(i=0;i<max_num_rounds;i++) key[i] = KEY[i%KEY_SIZE]^rconsts[i];
		for(t=0,num_rounds=BIT_SIZE*2;num_rounds<=max_num_rounds;num_rounds+=BIT_SIZE,t++){
			sbox = (u32*)calloc(MAX_VEC,sizeof(u32));
			gen_sbox_by_keyNround(sbox,key,num_rounds);
			max_DDT = find_max_DDT(sbox);
			max_LAT = find_lat_max_fwalsh((int*)sbox,parity,BIT_SIZE);
			histograms[k][2*t] = max_DDT;
			histograms[k][2*t+1] = max_LAT;
			sums[2*t] += max_DDT;
			sums[2*t+1] += max_LAT;
			free(sbox);
		}
		if((k%PRINT)==0){
			printf("test %d\n",k);
			for(i=0;i<(2*BIT_SIZE-1);i++)
				printf("%3d round max dif average : %f\n",(i+2)*BIT_SIZE,((float)sums[2*i])/((float)(k+1)));
         printf("\n");
	      for(i=0;i<(2*BIT_SIZE-1);i++)
            printf("%3d round max lin average : %f\n",(i+2)*BIT_SIZE,((float)sums[2*i+1])/((float)(k+1)));
			printf("--------------------------------------\n\n");
			fprintf(ofile,"test %d\n",k);
			for(i=0;i<(2*BIT_SIZE-1);i++)
				fprintf(ofile,"%3d round max dif average : %f\n",(i+2)*BIT_SIZE,((float)sums[2*i])/((float)(k+1)));
         fprintf(ofile,"\n");
	      for(i=0;i<(2*BIT_SIZE-1);i++)
            fprintf(ofile,"%3d round max lin average : %f\n",(i+2)*BIT_SIZE,((float)sums[2*i+1])/((float)(k+1)));
			fprintf(ofile,"--------------------------------------\n\n");
			fflush(stdout);
		}
	}
	fprintf(ofile,"Final results:\n");
	for(i=0;i<(2*BIT_SIZE-1);i++)
		fprintf(ofile,"%3d round max dif average : %f\n",(i+2)*BIT_SIZE,((float)sums[2*i])/((float)TEST_SIZE));
	fprintf(ofile,"--------------------------------------\n");
	for(i=0;i<(2*BIT_SIZE-1);i++)
		fprintf(ofile,"%3d round max lin average : %f\n",(i+2)*BIT_SIZE,((float)sums[2*i+1])/((float)TEST_SIZE));
	fprintf(ofile,"--------------------------------------\nPrinting histograms:\n");
	for(num_rounds=2*BIT_SIZE;num_rounds<=max_num_rounds;num_rounds+=BIT_SIZE){
	   j = num_rounds/BIT_SIZE-2;
	   fprintf(ofile,"%3d rounds dif :",num_rounds);
	   for(i=0;i<TEST_SIZE;i++) fprintf(ofile,"%d,",histograms[i][2*j]);
	   fprintf(ofile,"\n%3d rounds lin :",num_rounds);
	   for(i=0;i<TEST_SIZE;i++) fprintf(ofile,"%d,",histograms[i][2*j+1]);
	   fprintf(ofile,"\n\n");
   }
   fclose(ofile);
	return 0;
}

