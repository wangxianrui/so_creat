/*
* Reed Solomon Encoder/Decoder
*
* Copyright Henry Minsky (hqm@alum.mit.edu) 1991-2009
*
* This software library is licensed under terms of the GNU GENERAL
* PUBLIC LICENSE
*
* RSCODE is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* RSCODE is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Rscode.  If not, see <http://www.gnu.org/licenses/>.

* Commercial licensing is available under a separate license, please
* contact author for details.
*
* Source code is available at http://rscode.sourceforge.net
*/

#include <stdio.h>
#include <ctype.h>
#include "ecc.h"
#include <math.h>
#include <cstring>

/* Encoder parity bytes */
int pBytes[MAXDEG];

/* Decoder syndrome bytes */
int synBytes[MAXDEG];

/* generator polynomial */
int genPoly[MAXDEG * 2];

int DEBUG = FALSE;

static void
compute_genpoly(int nbytes, int genpoly[]);

/* Initialize lookup tables, polynomials, etc. */
void
initialize_ecc()
{
	/* Initialize the galois field arithmetic tables */
	init_galois_tables();

	/* Compute the encoder generator polynomial */
	compute_genpoly(NPAR, genPoly);
}

void
zero_fill_from(unsigned char buf[], int from, int to)
{
	int i;
	for (i = from; i < to; i++) buf[i] = 0;
}

/* debugging routines */
void
print_parity(void)
{
	int i;
	printf("Parity Bytes: ");
	for (i = 0; i < NPAR; i++)
		printf("[%d]:%x, ", i, pBytes[i]);
	printf("\n");
}


void
print_syndrome(void)
{
	int i;
	printf("Syndrome Bytes: ");
	for (i = 0; i < NPAR; i++)
		printf("[%d]:%x, ", i, synBytes[i]);
	printf("\n");
}

/* Append the parity bytes onto the end of the message */
void
build_codeword(unsigned char msg[], int nbytes, unsigned char dst[], unsigned char dst1[])
{
	int i;

	for (i = 0; i < nbytes; i++)
	{
		dst[i] = msg[i];
		dst1[i] = msg[i];
	}
	for (i = 0; i < NPAR; i++) {
		dst[i + nbytes] = pBytes[NPAR - 1 - i];
		dst1[i + nbytes - 1] = pBytes[NPAR - 1 - i];
	}
	dst1[i + nbytes - 1] = '\0';
}

/**********************************************************
* Reed Solomon Decoder
*
* Computes the syndrome of a codeword. Puts the results
* into the synBytes[] array.
*/

void
decode_data(unsigned char b[], unsigned char decdatabin[])
{
	unsigned char *data;
	data = new unsigned char[256];
	data = binto2dec(b,16+NPAR);
	//insert(data);
	//for (int i = 0; i < 16+NPAR+1; i++)cout << (int)data[i] << " ";
	//cout << endl;
	//cout << data << endl;

	unsigned char decdata[256];
	int i, j, sum;
	int erasures[NPAR * 4];
	int nerasures = 0;
	int nbytes = 16 + NPAR + 1;
	for (j = 0; j < NPAR; j++) {
		sum = 0;
		for (i = 0; i < nbytes; i++) {
			sum = data[i] ^ gmult(gexp[j + 1], sum);
		}
		synBytes[j] = sum;
	}
	if (check_syndrome() != 0)
	{
		correct_errors_erasures(data, nbytes, nerasures, erasures);
	}
	for (i = 0; i < 128; i++)
	{
		decdata[i] = data[i];
	}
	decdata[i] = '\0';
	//for (int i = 0; i < 16 + NPAR + 1; i++)cout << (int)decdata[i] << " ";
	//cout << endl;
	get2string(decdatabin, decdata,16);
}


/* Check if the syndrome is zero */
int
check_syndrome(void)
{
	int i, nz = 0;
	for (i = 0; i < NPAR; i++) {
		if (synBytes[i] != 0) {
			nz = 1;
			break;
		}
	}
	return nz;
}


void
debug_check_syndrome(void)
{
	int i;

	for (i = 0; i < 3; i++) {
		printf(" inv log S[%d]/S[%d] = %d\n", i, i + 1,
			glog[gmult(synBytes[i], ginv(synBytes[i + 1]))]);
	}
}


/* Create a generator polynomial for an n byte RS code.
* The coefficients are returned in the genPoly arg.
* Make sure that the genPoly array which is passed in is
* at least n+1 bytes long.
*/

static void
compute_genpoly(int nbytes, int genpoly[])
{
	int i, tp[256], tp1[256];

	/* multiply (x + a^n) for n = 1 to nbytes */

	zero_poly(tp1);
	tp1[0] = 1;

	for (i = 1; i <= nbytes; i++) {
		zero_poly(tp);
		tp[0] = gexp[i];		/* set up x+a^n */
		tp[1] = 1;

		mult_polys(genpoly, tp, tp1);
		copy_poly(tp1, genpoly);
	}
}

/* Simulate a LFSR with generator polynomial for n byte RS code.
* Pass in a pointer to the data array, and amount of data.
*
* The parity bytes are deposited into pBytes[], and the whole message
* and parity are copied to dest to make a codeword.
*
*/

void
encode_data(unsigned char a[], unsigned char dst1[])
{
	unsigned char *msg;
	int len = strlen((char *)a) / 8;
	msg = new unsigned char[len];
	msg = binto2dec(a);
	//for (int i = 0; i < 17; i++)cout << (int)msg[i] << " ";
	//cout  << endl;
	unsigned char dst[256];
	int i, LFSR[NPAR + 1], dbyte, j;
	int nbytes = 16;
	unsigned char * dst2;
	dst2 = new unsigned char[nbytes + NPAR - 1];
	//cout << nbytes << endl;
	for (i = 0; i < NPAR + 1; i++) LFSR[i] = 0;

	for (i = 0; i < nbytes; i++) {
		dbyte = msg[i] ^ LFSR[NPAR - 1];
		for (j = NPAR - 1; j > 0; j--) {
			LFSR[j] = LFSR[j - 1] ^ gmult(genPoly[j], dbyte);
		}
		LFSR[0] = gmult(genPoly[0], dbyte);
	}

	for (i = 0; i < NPAR; i++)
		pBytes[i] = LFSR[i];

	build_codeword(msg, nbytes, dst, dst2);
	//cout << dst << endl;
	//for (int i = 0; i < 16+NPAR; i++)cout << (int)dst[i] << " ";
	//cout << endl;
	//for (int i = 0; i < 16 + NPAR; i++)cout << (int)dst2[i] << " ";
	//cout << endl;
	get2string(dst1, dst,16+NPAR);
	//cout << dst1 << endl;
}

void get2string(unsigned char *s, unsigned char *a,int len)
{
	int i, j, k = 0;
	unsigned char *b;
	b = new unsigned char[8];
	for (i = 0; i < len; i++)
	{
		b = dec2binstring((int)a[i]);
		//cout << dec2binstring((int)a[i]);
		for (j = 0; j < 8; j++)
		{
			s[k] = b[j];
			k++;
		}
	}
	s[k] = '\0';
}

void get2string(unsigned char *s, unsigned char *a)
{
	int i, j, k = 0;
	unsigned char *b;
	b = new unsigned char[8];
	for (i = 0; i < (int)strlen((char *)a); i++)
	{
		b = dec2binstring((int)a[i]);
		//cout << dec2binstring((int)a[i]);
		for (j = 0; j < 8; j++)
		{
			s[k] = b[j];
			k++;
		}
	}
	s[k] = '\0';
}


unsigned char * dec2binstring(int n)
{
	unsigned char b[8];
	static unsigned char a[8];
	int i, j = 0;
	while (1)
	{
		if (n)
		{
			i = n % 2;
			n /= 2;
			b[j] = i + '0';
			j++;
		}
		else if (j < 8)
		{
			b[j] = '0';
			j++;
		}
		else break;
	}
	for (i = 0; i < j; i++)
	{
		a[j - i - 1] = b[i];
	}
	return a;
}
unsigned char * binto2dec(unsigned char *s, int len)
{
	unsigned char *b;
	int *a;
	b = new unsigned char[len];
	//cout << len << endl;
	a = new int[len];
	int i, cnt, j, k, l = 0;
	for (i = 0; i < (len * 8) && l<len; i++)
	{
		if ((i + 1) % 8 == 0)
		{
			k = i;
			cnt = 0;
			for (j = 0; j < 8; k--, j++)
			{
				cnt = cnt + (s[k] - '0')* (int)pow(2, j);
				//cout << s[k] << " ";
				//cout << cnt << endl;
			}
			a[l] = cnt;
			//cout <<"a:"<< a[l] << endl;
			//cout << (unsigned char)a[l] << endl;
			//cout << endl << cnt << endl;
			l++;
		}
	}

	for (j = 0; j < len; j++)
	{
		b[j] = (unsigned char)a[j];
		//cout << "a:" << a[j] << " ";
		//cout << (unsigned char)a[j] << endl;
		//cout  << "b:" << (int)b[j] << " ";
		//cout  << b[j] << endl;
	}
	b[j] = '\0';
	//cout << b<<endl;
	return b;
}
unsigned char * binto2dec(unsigned char *s)
{
	unsigned char *b;
	int *a, len = strlen((char *)s) / 8;
	b = new unsigned char[len];
	//cout << len << endl;
	a = new int[len];
	int i, cnt, j, k, l = 0;
	for (i = 0; i < (int)strlen((char *)s) && l<len; i++)
	{
		if ((i + 1) % 8 == 0)
		{
			k = i;
			cnt = 0;
			for (j = 0; j < 8; k--, j++)
			{
				cnt = cnt + (s[k] - '0')* (int)pow(2, j);
				//cout << s[k] << " ";
				//cout << cnt << endl;
			}
			a[l] = cnt;
			//cout <<"a:"<< a[l] << endl;
			//cout << (unsigned char)a[l] << endl;
			//cout << endl << cnt << endl;
			l++;
		}
	}

	for (j = 0; j < len; j++)
	{
		b[j] = (unsigned char)a[j];
		//cout << "a:" << a[j] << " ";
		//cout << (unsigned char)a[j] << endl;
		//cout  << "b:" << (int)b[j] << " ";
		//cout  << b[j] << endl;
	}
	b[j] = '\0';
	//cout << b<<endl;
	return b;
}

void  insert(unsigned char a[])
{
	int i = 0;
	for (; i<16 + NPAR; i++)
	{
		if (i < 16)
			i++;
		else
		{
			for (int j = 16 + NPAR; j>i; j--)
			{
				a[j] = a[j - 1];
			}
			a[16] = '\0';
			break;
		}
	}
}