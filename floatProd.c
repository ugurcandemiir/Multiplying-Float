#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "floatInt.h"

float floatProd(float a,float b);
void floatPrint(char * pref,float a);

int main(int argc,char **argv) {
	if (argc!=3) {
		printf("Please invoke as %s <a> <b>\n",argv[0]);
		return 1;
	}
	float a=atof(argv[1]);
	floatPrint("a             :",a);
	float b=atof(argv[2]);
	floatPrint("b             :",b);
	floatPrint("floatProd(a,b):",floatProd(a,b));
	floatPrint("a x b         :",a*b);
	return 0;
}

float floatProd(float a,float b) {
	/* Your code to compute the product of a and b without doing a*b goes here */
	int sign_bit_value_a;
	int sign_bit_value_b;
	int exponent_product;
	long fractional_product;
	float result;
	if (float2Sign(a) != float2Sign(b)) {sign_bit_value = 1; }
	else{ sign_bit_value = 0; }
	exponent_product = float2Exp(a) + float2Exp(b);
	fractional_product = (long)float2Frac(a)*(long)float2Frac(b);
	fractional_product = fractional_product >> 23;
	if (fractional_product > pow(2,24)){
		fractional_product = fractional_product >> 1;
		exponent_product++;
	}
	if (exponent_product > 126){
		exponent_product = 127;
		fractional_product = 0.0;
	}
	if (exponent_product < -126){
		return 0.0;
	}
	result =  makeFloat(sign_bit_value,exponent_product, fractional_product);
	if (isnanf(a) || isnanf(b)){
		return NAN;
	}
	if (isinff(a) || isinff(b)){
		if (a != 0.0 || b!= 0.0){
			if  (float2Sign(a) != float2Sign(b)){
					return -INFINITY;
			}else{
					return INFINITY;
			}
		}
		if (a == 0.0 || b==0.0){
			return NAN;
		}
	}
	if  (result>0 && result<1){
		while (exponent_product > -126){
			exponent_product = exponent_product - 1;
			fractional_product = fractional_product << 1;
		}
		while (exponent_product < -126){
			exponent_product = exponent_product + 1;
			fractional_product = fractional_product >> 1;
		}

	}
	return result;

}

void floatPrint(char * pref,float a) {
	int norm=isNormal(a);
	int frac=float2Frac(a);
	int exp=float2Exp(a);
	if (isNormal(a)) {
		frac &= ~(1<<23); // Turn of 1. for print
	} else exp=-126;
	printf("%s %g = -1^%d x %d.%06x x 2^%d\n",pref,a,
		float2Sign(a),norm,frac,exp);
}
