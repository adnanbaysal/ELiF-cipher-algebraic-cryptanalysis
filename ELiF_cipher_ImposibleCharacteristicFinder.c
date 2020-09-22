#include <stdio.h>
#include <stdlib.h>

#define u8  unsigned char
#define u32 unsigned

#define B	8

u32 rconsts[] = {
1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,1,
0,1,0,0,0,1,0,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,0,0,0,0,1,1,0,0,0,1,
0,0,1,1,0,1,1,0,0,1,0,0,1,1,0,0,0,1,1,0,0,0,1,1,1,0,0,0,1,1,0,0,
0,0,0,0,0,0,0,1,1,0,1,0,0,0,0,0,1,0,1,0,1,1,1,1,0,1,1,1,0,0,1,1,
1,1,1,0,1,0,0,1,1,0,1,0,1,1,1,1,0,1,1,0,1,0,1,1,1,1,1,0,0,0,1,0,
1,1,1,0,1,0,0,0,1,1,1,1,0,1,1,1,1,0,0,0,1,1,1,0,1,1,1,0,0,0,0,0,
0,1,0,1,0,0,0,1,0,0,1,1,1,0,1,1,0,1,0,0,1,0,0,1,0,0,0,1,1,0,0,0,
0,0,0,1,0,0,1,1,0,1,0,0,1,1,0,0,1,1,0,1,1,0,1,1,1,1,1,1,0,0,0,0,
0,1,1,0,0,1,0,0,1,0,1,0,1,1,1,0,0,0,0,1,0,0,1,1,0,0,0,1,1,1,0,0,
1,1,1,0,0,0,0,0,0,1,1,0,0,0,0,1,0,0,1,0,1,1,0,1,1,1,0,0,0,1,1,0,
0,0,1,0,0,0,0,0,1,1,0,0,1,0,1,1,0,1,0,1,1,1,0,1,1,0,0,1,0,1,1,0,
1,0,0,1,1,0,1,1,0,0,1,1,0,1,1,0,0,0,1,1,1,1,0,0,0,1,0,0,0,0,1,1,
0,0,0,1,0,1,1,1,0,1,1,0,0,1,1,1,1,0,1,0,0,1,1,1,0,1,1,1,1,1,0,1,
0,0,1,0,1,1,0,1,1,1,1,0,0,1,1,0,0,0,1,1,1,0,1,1,1,1,0,0,0,0,0,1,
0,1,1,1,0,0,1,0,1,1,1,1,1,0,0,1,0,1,1,1,1,0,1,0,1,0,1,1,1,1,1,1,
1,0,0,0,1,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,1,1,0,0,0,0,0,1,1,1,
1,0,1,0,1,0,1,0,0,1,1,1,1,0,0,0,0,1,0,1,0,1,1,0,0,1,1,1,1,0,0,1,
0,1,1,1,0,1,1,0,1,0,1,1,1,0,1,0,0,0,1,0,1,1,0,1,1,1,1,0,1,1,1,0,
0,0,1,1,1,1,0,1,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,1,0,0,1,0,0,0,1,0,
1,0,0,0,0,0,1,0,1,1,0,1,1,1,0,1,1,1,1,0,0,0,1,0,1,0,1,1,1,0,0,0,
1,1,0,0,1,1,0,0,0,0,0,1,1,1,1,1,1,0,1,0,1,0,0,1,0,0,1,1,1,0,0,1,
0,0,0,0,1,0,0,0,1,0,0,1,1,1,1,0,1,0,1,0,0,1,0,1,1,1,1,0,0,1,0,0,
1,1,1,1,1,0,1,0,0,0,1,0,1,0,1,1,1,1,1,0,1,1,0,0,1,1,1,0,1,1,0,0,
1,1,1,0,0,1,0,0,1,1,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,1,0,1,1,
0,1,1,1,0,0,0,0,1,0,0,0,1,0,0,0,1,1,1,0,1,0,1,0,1,0,0,0,0,1,1,0,
0,0,0,1,1,1,1,0,1,1,0,1,1,0,0,1,1,1,0,0,0,0,0,1,1,1,0,0,0,0,1,0,
1,0,0,0,0,0,1,1,1,1,0,1,1,1,0,1,0,0,1,1,1,0,1,0,1,1,1,0,1,0,0,1,
1,0,1,1,0,1,1,1,0,1,1,0,0,0,0,0,1,0};

inline void elf_enc_round_no_kc(u8 *state,u8 bitlen){
	u8 i, t = state[bitlen-1];
	state[2] ^= state[1]&state[0];
	for(i=bitlen-1;i!=0;i--) state[i] = state[i-1];
	state[0] = t;
}

inline void elf_dec_round_no_kc(u8 *state,u8 bitlen){
	u8 i, t = state[0];
	for(i=1;i<bitlen;i++) state[i-1] = state[i];
	state[2] ^= state[1]&state[0];
	state[bitlen-1] = t;
}

inline void elf_enc_no_kc(u8 *state,u32 rounds,u8 bitlen){
	u32 i;
	for(i=0;i<rounds;i++) elf_enc_round_no_kc(state,bitlen);
}

inline void elf_dec_no_kc(u8 *state,u32 rounds,u8 bitlen){
	u32 i;
	for(i=0;i<rounds;i++) elf_dec_round_no_kc(state,bitlen);
}

inline void elf_enc_round(u8 *state,u8 kc,u8 bitlen){
	u8 i, t = state[bitlen-1];
	state[2] ^= (state[1]&state[0])^kc;
	for(i=bitlen-1;i!=0;i--) state[i] = state[i-1];
	state[0] = t;
}

inline void elf_dec_round(u8 *state,u8 kc,u8 bitlen){
	u8 i, t = state[0];
	for(i=1;i<bitlen;i++) state[i-1] = state[i];
	state[2] ^= (state[1]&state[0])^kc;
	state[bitlen-1] = t;
}

inline void elf_enc(u8 *state,u8 *kc,u32 rounds,u8 bitlen){
	u32 i;
	for(i=0;i<rounds;i++) elf_enc_round(state,kc[i],bitlen);
}

inline void elf_dec(u8 *state,u8 *kc,u32 rounds,u8 bitlen){
	u32 i;
	for(i=0;i<rounds;i++) elf_dec_round(state,kc[rounds-1-i],bitlen);
}

#define TESTNUM	64

int main(int argc,char **argv){
	srand(atoi(argv[1]));
	u32 table_ff[B][B] = {0}, table_rw[B][B] = {0}, ff_max_r = B*B/2-B+2;
	u32 ff_rounds, rw_rounds, i_bit, o_bit, test, i, j, rndd, rw_max_r = 2*B-3;
	char z;
	for(ff_rounds=ff_max_r;ff_rounds>0;ff_rounds--){
	   for(rw_rounds=rw_max_r;rw_rounds>0;rw_rounds--){
	      printf("%d + %d\n",ff_rounds,rw_rounds);
	      for(i_bit=0;i_bit<B;i_bit++){
      		for(test=0;test<TESTNUM;test++){
      			u8 p1[B], p2[B], c1[B], c2[B], K[2*B], kc[B*B];
      			for(i=0;i<B;i++){
      				rndd = rand();
      				p1[i] = rndd&1;
      				p2[i] = p1[i];
      				c1[i] = (rndd>>1)&1;
      				c2[i] = c1[i];
      				K[2*i] = (rndd>>2)&1;
      				K[2*i+1] = (rndd>>3)&1;
      			}
      			for(i=0;i<B*B;i++) kc[i] = K[i%(2*B)]^rconsts[i];
      			p2[i_bit] ^= 1;
      			c2[i_bit] ^= 1;
      			elf_enc(p1,kc,ff_rounds,B);
      			elf_enc(p2,kc,ff_rounds,B);
      			elf_dec(c1,kc+ff_rounds,rw_rounds,B);
      			elf_dec(c2,kc+ff_rounds,rw_rounds,B);
      			for(i=0;i<B;i++) p1[i] ^= p2[i];
      			for(i=0;i<B;i++) c1[i] ^= c2[i];
      			for(o_bit=0;o_bit<B;o_bit++) table_ff[i_bit][o_bit] += p1[o_bit];
      			for(o_bit=0;o_bit<B;o_bit++) table_rw[i_bit][o_bit] += c1[o_bit];
      		}
      	}
      	for(i_bit=0;i_bit<B;i_bit++){
      		for(o_bit=0;o_bit<B;o_bit++){
      			for(j=0;j<B;j++){
      				if(((table_ff[i_bit][j]==0)&&(table_rw[o_bit][j]==TESTNUM))||
                     ((table_ff[i_bit][j]==TESTNUM)&&(table_rw[o_bit][j]==0)))
      						printf("%d -(%d)-> %d <-(%d)- %d impossible!\n",i_bit,ff_rounds,j,rw_rounds,o_bit);
      			}
      		}
      	}
      }
   }
	return 0;
}
