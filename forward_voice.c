#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void print_buf(const uint8_t *data, int len)
{
    for(int i = 0; i < len; i++) {
        printf("%x", data[i]);
    }
    printf("\n");
}

static void mk_crc3(uint8_t *res, const uint8_t *input, int input_len)
{
    uint8_t inv;
    memset(res, 0, 3);

    for (int i = 0; i < input_len; ++i)
    {
        inv = input[i] ^ res[0];

        res[0] = res[1];
        res[1] = res[2] ^ inv;
        res[2] = inv;
    }
    res[0] = res[0] ^ 1;
    res[1] = res[1] ^ 1;
    res[2] = res[2] ^ 1;
}

int is_in_code(uint8_t j) {
    uint8_t pre_cod[] = {7, 10, 13, 16, 19, 22, 25, 28, 31, 34, 37, 40, 43, 46, 49, 52, 55, 58, 61, 64, 67, 70, 73, 76, 83, 86, 89, 92, 95, 98, 101, 104, 107, 110, 113, 116, 119, 122, 125, 128, 131, 134, 137, 140, 143, 146, 149};
    int i;
    for(i=0; i<sizeof(pre_cod); i++) {
            if(pre_cod[i]==j)
                    return 1;
    }
    return 0;
}

uint8_t encode_voice_frame(uint8_t *v, uint8_t carry)
{
	int i,j;
//	uint8_t v[120] = {1,0,0,1,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,1,0,1,1,0,0,0,1,0,0,0,1,1,1,0,0,1,0,1,1,0,0,1,1,0,0,0,1,1,1,1,0,0,1,0,0,0,1,0,0,1,1,1,0,0,0,0,1,0,1,1,0,1,1,0,0,0,0,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,1,0,0,0,1,0,1,1,0,0,0,1,0,0};

// 6.1
	uint8_t b[122];

	uint8_t asbx=1,asby=0;
	for(i=0;i<20;i++)
            b[i] = v[i];
        b[20] = asbx;
        b[21] = asby;
	for(i=22;i<122;i++)
	    b[i] = v[i-2];

	int SCR;

	SCR=4;
	
//	printf("b=");
//	print_buf(b,122);

// 6.1.1
	uint8_t bx[26];

	bx[0] = 0;
	for(j=0;j<22;j++)
	    bx[j+1] = b[j];

	mk_crc3(&bx[23], &bx[0], 23);

//	printf("bx=");
//	print_buf(bx,26);
	

	char bxx[26];

	for(j=0;j<23;j++)
	    bxx[j] = bx[j];

	for(j=23;j<26;j++)
//	    bxx[j] = 1 - bx[j];
	    bxx[j] =  bx[j];

//	printf("bxx=");
//	print_buf(bxx,26);

// 6.1.2

// bxx[-2] = bxx[24]
// bxx[-1] = bxx[25]

	char c[152];

	c[0] = bxx[0] ^ bxx[25] ^ bxx[24];	// j=0
	c[1] = bxx[0] ^ bxx[24];

	c[2] = bxx[1] ^ bxx[0] ^ bxx[25];	// j=1
	c[3] = bxx[1] ^ bxx[25];

	for(j=2; j<26; j++) {			//j=2..25
		c[2*j] = bxx[j] ^ bxx[j-1] ^ bxx[j-2];
		c[2*j+1] = bxx[j] ^ bxx[j-2];
	}

	for(j=52; j<= 152; j++)
		c[j] = b[j-30];

//	printf("c=");
//	print_buf(c,152);




// 6.1.4.1


	int K[] = {1, 77, 38, 114, 20, 96, 59, 135, 3, 79, 41, 117, 23, 99, 62, 138, 5, 81, 44, 120, 26, 102, 65, 141, 8, 84, 47, 123, 29, 105, 68, 144, 11, 87, 50, 126, 32, 108, 71, 147, 14, 90, 53, 129, 35, 111, 74, 150, 17, 93, 56, 132, 37, 113, 73, 4, 0, 76, 40, 119, 19, 95, 58, 137, 151, 80, 42, 115, 24, 100, 60, 133, 12, 88, 48, 121, 30, 106, 66, 139, 18, 91, 51, 124, 28, 104, 67, 146, 10, 89, 52, 131, 34, 110, 70, 149, 13, 97, 57, 130, 36, 112, 75, 148, 6, 82, 39, 116, 16, 92, 55, 134, 2, 78, 43, 122, 22, 98, 61, 140, 9, 85, 45, 118, 27, 103, 63, 136, 15, 83, 46, 125, 25, 101, 64, 143, 7, 86, 49, 128, 31, 107, 69, 142, 21, 94, 54, 127, 33, 109, 72, 145 };


	uint8_t e[152];
	uint8_t k;

	for(j=0; j<= 151; j++) {
		k=K[j];
		e[k] = c[j];
	}

//	printf("e=");
//	print_buf(e,152);


// 6.1.4.2

	uint8_t ex[152];

	ex[0] = e[0] ^ 0; 		//f7
	for(j=1; j<= 151; j++)
		if (is_in_code(j)) 
			ex[j] = e[j] ^ ex[j-2];
		else
			ex[j] = e[j] ^ ex[j-1];

//	printf("ex=");
//	print_buf(ex,152);

//// 6.2.5.1

	uint8_t s[127];

	s[0] = 1;
	s[1] = 1;
	s[2] = 1;
	s[3] = 1;
	s[4] = 1;
	s[5] = 1;
	s[6] = 1;
	for(j=7; j<= 127; j++)
		s[j] = s[j-1] ^ s[j-7];

//	printf("s=");
//	print_buf(s,127);

	
// 6.2.5.2

	uint8_t f[160];

	f[0] = 0;
	f[1] = 1;
	f[2] = 1;
	f[3] = 0;
	f[4] = 0;
	f[5] = 0;
	f[6] = 1;
	f[7] = 0;

	for(j=0; j<= 151; j++) {
		if (SCR > 0)
			f[j+8] = ex[j] ^ s[(j+SCR)%127];
		else
			f[j+8] = ex[j];
	}

//	printf("f=");
//	print_buf(f,160);

// 7.2
	uint8_t M[160];

	M[0] = f[k] ^ carry;
	for(k=1; k<= 160; k++)
		M[k] = f[k] ^ f[k-1];

	carry = f[159];

//	printf("M=");
	print_buf(M,160);
	return carry;
}

int main(int argc, char* argv[]) {
	char buf[122];
	uint8_t v[120];
	uint8_t carry=1;
	int f;
	int i;

	f=open("voice", 0);
	while (read(f,buf,122) == 122) {
		for (i=0; i<20;i++) {
			if (buf[i] == '0')
				v[i] = 0;
			else if (buf[i] == '1')
				v[i] = 1;
			else {
				printf("Data error 1!\n");
				exit(-1);
			}
		}
		for (i=20; i<120;i++) {
			if (buf[i+1] == '0')
				v[i] = 0;
			else if (buf[i+1] == '1')
				v[i] = 1;
			else {
				printf("Data error 2!\n");
				exit(-1);
			}
		}	
//		printf("v=");
//		print_buf(v,120);
		carry = encode_voice_frame(v, carry);
	}
		
	
}


