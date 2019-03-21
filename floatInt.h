#ifndef FLOATINT_H
#define FLOATINT_H
int float2Frac(float x); // returns the fractional part of x times 2^23
int float2Exp(float x); // returns the unbiased exponent of x
int float2Sign(float x); //returns the sign bit of x (1 if negative, 0 if positive)
int isNormal(float x); // Returns 1 if x is normal, 0 if x is subnormal
float makeFloat(int sign, int exp, int frac); // returns the floating point representation
#endif