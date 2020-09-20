#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define u64 unsigned long long

u64 and_table[4] = {0L,0L,0L,4L}, rconsts[63] = {
0,0,0,0,0,1,0,0,0,0,1,1,0,0,0,1,
0,1,0,0,1,1,1,1,0,1,0,0,0,1,1,1,
0,0,1,0,0,1,0,1,1,0,1,1,1,0,1,1,
0,0,1,1,0,1,0,1,0,1,1,1,1,1,1};

inline void l2r_sl_round(u64 *data,u64 key){
	u64 temp = (data[0]&3);
	data[0] ^= and_table[temp]^key;
	temp = ((data[0]&0x8000000000000000L)?1:0);
	data[0] <<= 1;
	data[0] ^= temp;
}

inline void l2r_sl_enc(u64 *data,u64 *key,u64 rounds){
	int i;
	for(i=0;i<rounds;i++) l2r_sl_round(data,key[i]);
}

#define SQ_ROWS	16
#define SQ_COLS	8

int main(int argc,char **argv){//./a.exe TEST_SIZE FILE_NAME SEED
	u64 i, j, k, m, plain1, plain2, cipher1, cipher2, c_xor, key[64*SQ_ROWS*SQ_COLS], rounds, one = 1L;
	int TEST_SIZE = atoi(argv[1]), SEED = atoi(argv[3]), ***sac_int;//sac_int[SQ_ROWS*SQ_COLS][64][64] = {0};
	sac_int = (int***)calloc(SQ_ROWS*SQ_COLS,sizeof(int**));
	for(i=0;i<(SQ_ROWS*SQ_COLS);i++){
		sac_int[i] = (int**)calloc(64,sizeof(int*));
		for(j=0;j<64;j++) sac_int[i][j] = (int*)calloc(64,sizeof(int));
	}
	char sac_char[SQ_ROWS][SQ_COLS][64][64] = {0};
	srand(SEED);
	u64 KEY[62];
	for(i=0;i<62;i++) KEY[i] = rand()&4;
	for(i=0;i<(64*SQ_ROWS*SQ_COLS);i++) key[i] = KEY[i%62]^rconsts[i%63];
	for(i=0;i<TEST_SIZE;i++){
		if((i%256)==0) printf("i = %d\n",i/256);
		plain1 = 0;
		for(j=0;j<8;j++){
			plain1 <<= 8;
			plain1 ^= rand();
		}
		for(j=0;j<64;j++){//fark alan girdi biti
			plain2 = plain1^(one<<(63-j));
			cipher1 = plain1;
			cipher2 = plain2;
			for(rounds=0;rounds<(SQ_ROWS*SQ_COLS);rounds++){
				l2r_sl_enc(&cipher1,key+64*rounds,64);
				l2r_sl_enc(&cipher2,key+64*rounds,64);
				c_xor = cipher1^cipher2;
				for(k=0;k<64;k++) sac_int[rounds][j][k] += (c_xor>>(63-k))&one;
			}
		}
	}
	for(i=0;i<(SQ_ROWS*SQ_COLS);i++){
		for(j=0;j<64;j++){
			for(k=0;k<64;k++){
				sac_int[i][j][k] *= 255;
				sac_int[i][j][k] /= TEST_SIZE;				
			}
		}
	}
	for(i=0;i<SQ_ROWS;i++){//64x64 kare satýr
		for(j=0;j<SQ_COLS;j++){//64x64 kare sütun
			for(k=0;k<64;k++){
				for(m=0;m<64;m++){
					sac_char[i][j][k][m] = (char)sac_int[i*SQ_COLS+j][k][m];
				}
			}
		}
	}
	FILE *ofile;
	ofile = fopen(argv[2],"wb");
	fprintf(ofile,"P5\n%d %d\n255\n",64*SQ_COLS,64*(SQ_ROWS-8));
	for(i=8;i<SQ_ROWS;i++){
		for(j=0;j<64;j++){
			for(k=0;k<SQ_COLS;k++){
				for(m=0;m<64;m++){
					fprintf(ofile,"%c",sac_char[i][k][j][m]);
				}
			}
		}
	}
	fclose(ofile);
	return 0;
}

