#include <assert.h>
#include <math.h>
#include "floatInt.h"

unsigned int bitRep(float x);
int float2BiasExp(float x);

int float2Frac(float x) { // returns the fractional part of x times 2^23
	// Special cases
	if (isinff(x)) return 0;
	if (isnan(x)) return 1;
	// return the rightmost 23 bits of the bit representation of x
	unsigned int xi=bitRep(x);
	int frac= xi&((1<<23)-1); // May be denormal!
	if (isNormal(x)==0) return frac; // denormal... no 1.
	return frac|(1<<23); // Or in the 1. in front of the decimal!
}

int float2Exp(float x) { // returns the unbiased exponent of x
	if (isNormal(x)) return float2BiasExp(x)-127;
	return -126;
}

int float2Sign(float x) { //returns the sign bit of x (1 if negative, 0 if positive)
	return (0!=(bitRep(x)&(1<<31)));
}

float makeFloat(int sign, int exp, int frac) { // returns the floating point representation
	union {
		float fl;
		unsigned int i;
	} flint;
	// Assumes exp=-127 and frac already shifted for denormals
	flint.i=0;
	assert(sign==0 || sign==1);
	assert(exp>-128 && exp<129);
	assert(frac<(1<<24)); // frac fits in 24 bits?
	flint.i |= (sign<<31); // Or in sign bit
	if (frac < (1<<23)) { // Is it a normal number?
			exp=-127; // sub-normal special case for exponent
	} else { // Remove the 1. from the fraction!
		frac &= ~(1<<23); // Turn off 1.
	}
	flint.i |= (exp+127)<<23; // Or in exponent
	flint.i |= frac; // Or in fraction
	return flint.fl;
}

int isNormal(float x) { // Returns 1 if x is normal, 0 if x is denormal
	return 0!=float2BiasExp(x);
}

int float2BiasExp(float x) { // returns the unbiased exponent of x
	// Biased exponent is bits 30-23 of the bit rep of x
	int mask=((1<<8)-1)<<23;
	return (bitRep(x) & mask)>>23; // *Biased exponent
}

unsigned int bitRep(float x) {
	union {
		float fl;
		unsigned int i;
	} flint;
	flint.fl=x;
	return flint.i;
}