#include<stdio.h>
#include<stdlib.h>
#include<string.h>

void clock_r(int* r, int input_bit_r, int control_bit_r)
{
	int r_dash[100];

	int feedback_bit = r[99]^input_bit_r;

	for(int i=1;i<=99;i++)
	{
		r_dash[i] = r[i-1];
	}
	r_dash[0] = 0;

	int r_taps[50] = {0,1,3,4,5,6,9,12,13,16,19,20,21,22,25,28,37,38,41,42,45,46,50,52,
		54,56,58,60,61,63,64,65,66,67,71,72,79,80,81,82,87,88,89,90,91,92,94,95,96,97};


	for(int i=0;i<50;i++)
	{
		r_dash[r_taps[i]] = r_dash[r_taps[i]] ^ feedback_bit;
	}

	if( control_bit_r == 1 )
	{
		for(int i=0;i<=99;i++)
		{
			r_dash[i] = r_dash[i] ^ r[i];
		}
	}

	memcpy(r,&r_dash,100*sizeof(int));
}

void clock_s(int* s,int input_bit_s, int control_bit_s)
{
	int s_dash[100],s_hat[100];

	int feedback_bit = s[99] ^ input_bit_s ;

	int COMP0[99] =  {0,0,0,0,1,1,0,0,0,1,0,1,1,1,1,0,1,0,0,1,0,1,0,1,0,
		              1,0,1,0,1,1,0,1,0,0,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,
		              0,0,0,0,1,0,1,0,0,1,1,1,1,0,0,1,0,1,0,1,1,1,1,1,1,
		              1,1,1,0,1,0,1,1,1,1,1,1,0,1,0,1,0,0,0,0,0,0,1,1};
	int COMP1[99]   = {0,1,0,1,1,0,0,1,0,1,1,1,1,0,0,1,0,1,0,0,0,1,1,0,1,
	                  0,1,1,1,0,1,1,1,1,0,0,0,1,1,0,1,0,1,1,1,0,0,0,0,1,
	                  0,0,0,1,0,1,1,1,0,0,0,1,1,1,1,1,1,0,1,0,1,1,1,0,1,
	                  1,1,1,0,0,0,1,0,0,0,0,1,1,1,0,0,0,1,0,0,1,1,0,0};

	int FB0[100]    = {1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,0,0,1,0,1,1,1,1,1,1,
	                  1,1,1,1,0,0,1,1,0,0,0,0,0,0,1,1,1,0,0,1,0,0,1,0,1,
	                  0,1,0,0,1,0,1,1,1,1,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,
	                  1,1,0,1,0,0,0,1,1,0,1,1,1,0,0,1,1,1,0,0,1,1,0,0,0};
	int FB1[100]    = {1,1,1,0,1,1,1,0,0,0,0,1,1,1,0,1,0,0,1,1,0,0,0,1,0,
	                  0,1,1,0,0,1,0,1,1,0,0,0,1,1,0,0,0,0,0,1,1,0,1,1,0,
	                  0,0,1,0,0,0,1,0,0,1,0,0,1,0,1,1,0,1,0,1,0,0,1,0,1,
	                  0,0,0,1,1,1,1,0,1,1,1,1,1,0,0,0,0,0,0,1,0,0,0,0,1};	              

	for(int i=1;i<=98;i++)
	{
		s_hat[i] = s[i-1] ^ ( (s[i]^COMP0[i]) & (s[i+1]^COMP1[i]) ) ;
	}

	s_hat[0] = 0 ;
	s_hat[99] = s[98] ;

	if(control_bit_s==0)
	{
		for(int i=0;i<=99;i++)
		{
			s_dash[i] = s_hat[i] ^ ( FB0[i] & feedback_bit );
		}
	}
	else
	{
		for(int i=0;i<=99;i++)
		{
			s_dash[i] = s_hat[i] ^ ( FB1[i] & feedback_bit );
		}
	}

	memcpy(s,&s_dash,100*sizeof(int));
}

void clock_kg(int* r,int* s,int mixing,int input_bit)
{
	int control_bit_r = s[34] ^ r[67];

	int control_bit_s = s[67] ^ r[33];

	int input_bit_r;

	if(mixing == 1)
	{
		input_bit_r = input_bit ^ s[50];
	}
	else
	{
		input_bit_r = input_bit;
	}

	int input_bit_s = input_bit;

	clock_r(r,input_bit_r,control_bit_r);

	clock_s(s,input_bit_s,control_bit_s);

}

void encrypt(int* r,int* s,int* key_stream,int length)
{
	//generating keystream
	for(int i=0;i<length;i++)
	{
		key_stream[i]=0;

		for(int j=7;j>=0;j--)
		{
			key_stream[i] ^= ( (r[0]^s[0])<<j );

			clock_kg(r,s,0,0);
		}
	}
}
void setup(int* r,int* s,int* key,int* iv,int iv_length)
{
	memset(r,0,100*sizeof(int));
	memset(s,0,100*sizeof(int));

    /* Load new key and IV*/
    
    // LOAD IV
	for(int i=0;i<iv_length/8;i++)
	{
		for(int j=7;j>=0;j--)
		{
			int iv_bit = (iv[i]>>j) & 1 ;
			clock_kg(r,s,1,iv_bit);
		}
	}

	//LOAD KEY
	for(int i=0;i<10;i++)
	{
		for(int j=7;j>=0;j--)
		{
			int key_bit = (key[i]>>j) & 1;

			clock_kg(r,s,1,key_bit);
		}
	}

	//PRECLOCK
	for(int i=0;i<=99;i++)
	{
		clock_kg(r,s,1,0);
	}
}

void iterated_test(int* r,int* s,int* key)
{

	setup(r,s,key,NULL,0);

	int new_key[10];
	int new_iv[4];
	int iv_length = 32;

	for (int i=0; i<1000; i++)
    {
        /* Generate new key and iv from keystream */
        encrypt(r,s,new_key,10);
        encrypt(r,s,new_iv,4);

       	setup(r,s,new_key,new_iv,32);
    }

    int key_stream[16];

    encrypt(r,s,key_stream,16);

    printf("Generated keystream: ");
	for(int i=0;i<16;i++)
	{
		printf(" %02x",key_stream[i]);
	}
	printf("\n");
}

int main()
{
	int r[100],s[100];

	memset(r,0,100*sizeof(int));
	memset(s,0,100*sizeof(int));

	int key[10] = {0x82, 0xac, 0xb3, 0x8c, 0x5d, 0x7a, 0x3c, 0x78, 0xd9, 0x8f};
	int iv[10]  = {0x9c, 0x53, 0x2f, 0x8a, 0xc3, 0xea, 0x4b, 0x2e, 0xa0, 0xf5};

	int iv_length = 0; // length of IV in bits

	

	setup(r,s,key,iv,iv_length);

	int key_stream[16];

	encrypt(r,s,key_stream,16);

	printf("Key: ");

	for(int i=0;i<10;i++)
	{
		printf(" %02x",key[i]);
	}
	printf("\n");

	//KEY ONLY SETUP

	iterated_test(r,s,key);

	//KEY+IV SETUP

	// printf("IV: ");
	// for(int i=0;i<iv_length/8;i++)
	// {
	// 	printf(" %02x",iv[i]);
	// }
	// printf("\n");

	// printf("Generated keystream: ");
	// for(int i=0;i<16;i++)
	// {
	// 	printf(" %02x",key_stream[i]);
	// }
	// printf("\n");

	return 0;
}

