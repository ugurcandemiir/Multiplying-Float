# Lab-02 Exploring Floating Point Arithmetic

## Problem Statement

For this lab, we are investigating some basic properties of floating point numbers. We will see some of these properties by working on the internals of how floating point numbers are multiplied. In order to do this, I have provided some C code that allows access to the internal values of a floating point number. If you remember from the floating point lecture, all floating point numbers (except for the special cases) are represented as:

*value* = -1<sup>*S*</sup> x *FRAC* x 2<sup>*EXP*</sup>

where *S* is the sign bit (0 for positive, 1 for negative), *FRAC* is a number between 1 and 2, expressed as 1.*fffff*, and *EXP* is an exponent between -126 and 126.

Normally, you would multiply two floating point nubmers just by using the C '\*' operator, as in:

```
float a=2.0; float b=5.3;
printf("a*b=%f\n",a*b); // prints a*b=10.6
```

However, for this lab, I'm going to ask you to find the product of two floating point values without using the '\*' operator to multiply two floating point values. Instead, you need to understand and mimic how the hardware actually performs that multiplication. In order to understand how this works, think of the ```a*b``` scenario.  Internally, a and b are represented as:

*value<sub>a</sub>* = -1<sup>*S<sub>a</sub>*</sup> x *FRAC<sub>a</sub>* x 2<sup>*EXP<sub>a</sub>*</sup>

*value<sub>b</sub>* = -1<sup>*S<sub>b</sub>*</sup> x *FRAC<sub>b</sub>* x 2<sup>*EXP<sub>b</sub>*</sup>

... and the product of ```a*b``` is calculated as:

*value<sub>a\*b</sub>* = normalized( -1<sup>*S<sub>a</sub>!=S<sub>b</sub>*</sup> x (*FRAC<sub>a</sub>* \* *FRAC<sub>b</sub>*) x 2<sup>*EXP<sub>a</sub>+EXP<sub>b</sub>*</sup> )

Note that when the fractional parts of a and b are multiplied together, it is possible that the result is no longer a number between 1 and 2. Therefore, the result of the normalization must be re-normalized.

## Files provided to you

I have provided several files for this lab. The top level C code is in a file called "floatProd.c". This contains a "main" function that reads two floating point numbers from the command line, prints out the internal representation of these numbers, invokes a "floatProd" function on your two numbers, and then compares the result against the C floating point multiplication of the two numbers. The floatProd.c file also contains an empty "floatProd" function - your job is to write that function.

I have also provided code to extract internal information from a floating point number in file "floatInt.c". Feel free to look at this code to see how it works, but you should not need to modify this code. A more complete description of the functions in this file is in the next section.

I have provided "floatInt.h", which contains everything you need to use the floatInt functions.

Finally, I have provided a "Makefile" that enables building and testing of a C executable called floatProd. The Makefile contains three test targets - a normal "test" target which tests for standard floating point numbers, and a "testSpecial" target which tests special case floating point numbers, and a "testSubNorm" target that tests multiplication of sub-normal floating point numbers.

## Functions for Extracting the Internal Values from Floating Point Nubmers

I have provided several functions to extract internal information from floating point numbers in the file "floatInt.c". These functions are designed to return integer representations of the internal information so that you can manipulate the internal values without using floating point operators. I also provide a function which takes the internal integer representations and converts them back to a floating point number. These functions are as follows:

- ```int float2Frac(float x) ``` - a function which extracts the fractional part of the normalized representation of ```x```, multiplied by 2<sup>23</sup> so that no precision is lost. Note that if x is a normal floating point number, then the "1." that is implicit in the binary representation of the fraction **IS** included in the return value from float2Frac.  If x is sub-normal, then the "0." that is implicit in the binary representation of the fraction is also "included". For instance, ```float2Frac(1.0)``` will return 2<sup>24</sup> = 16,777,216 even though the frac bits in the binary representation of 1.0 are all zeroes. For special cases, if x is zero or +/- infinity, returns 0. If x is "Not a Number", returns 1.

- ```int float2Exp(float x)``` - a function which extracts the exponent (power of two) of the normalized representation of ```x```. Note that the function returns the **unbiased** value of the exponent, even though a biased value is stored in the binary representation of x. For instance, ```float2Exp(1.0)``` will return the value 0. For special cases, if x is zero, returns -127. If x is infinite or "Not a Number", returns 128. If x is sub-normal, returns -126.

- ```int float2Sign(float x)``` - a function which extracts the value of the sign bit of the normalized representation of ```x```. This function returns 0 if the x is positive or zero, and 1 if x is negative.

   :warning: The GCC library code allows the sign bit to be turned on for "Not a Number", even though, according to the rules "Not a Number" cannot have a sign. Thus printf and scanf will write and read "-nan" as well as "nan" as a valid value. However, sign bits are not always managed correctly for "Not a Number" values by the GCC library (for instance ```-nan * -nan``` results in ```-nan```, but should be ```+nan```.) For the purposes of this lab, differences in sign bits for "Not a Number" values will be ignored.
 
 - ```int isNormal(float x)``` - a function which determines if the binary representation of ```x``` is normal or sub-normal, and returns a 1 if x is normal, and a zero if x is subnormal. For special cases, 0.0 is treated as sub-normal, but infinity and "Not a Number" are treated as normal.
 
 - ```float makeFloat(int sign, int exp, int frac)``` - a function which creates a floating point value from the integer parameters for sign, exp, and frac. The values for sign, exp, and frac are the values returned by float2Sign, float2Exp, and float2Frac for a floating point value. In other words, the sign value should be zero or 1, the exp value should be between -127 and 128, and the frac value should be between 1x2<sup>24</sup> and 1x2<sup>25</sup> for normal numbers, and 1 and 1x2<sup>24</sup> for sub-normal numbers. Special cases follow the same special case rules defined for the extraction functions.
 
 ## Multiplying Floats
 
With this infrastructure in place, you can now write the floatProd function in "floatProd.c". To start with, handle normal floating point numbers, disregarding special cases and sub-normal numbers.
 
To handle these numbers, you will need to first compute the sign of the result. That's pretty easy... just compare ```float2Sign(a)``` with ```float2Sign(b)```. If they are different the sign of the product is negative (sign bit set to 1).

Next, we can calculate the exponent. To start with, that's pretty easy. Just add ```float2Exp(a)``` to ```float2Exp(b)```. But hang on to the result. We may need to modify the exponent value and check it later.
 
Next, you need to compute the fractional product. To do this, multiply ```float2Frac(a)``` with ```float2Frac(b)```. There are a couple of things you need to keep in mind when you do this. First, the result of the float2Frac function is multiplied by 2<sup>23</sup>, and two numbers between 1 and 2 times 2<sup>23</sup> will result in a number between 1 and 4 times 2<sup>46</sup>... which doesn't fit in a 32 bit integer!  Therefore, you will need to perform the multiplication on *long* integers instead of *int* integers. Secondly, the result will be too big... we need to calculate a fraction that has a factor of 2<sup>23</sup>, not 2<sup>46</sup>. To fix this problem, we will need to shift the result to the right by 23 bit positions - a leaky abstraction way to divide by 2<sup>23</sup>.  Finally, the result may be between 1 and 4 times 2<sup>23</sup>, but we need a result between 1 and 2 times 2<sup>23</sup>. If the result is greater than 2<sup>24</sup>, then we need to shift the result to the right by 1 more bit position (divide by two), and increase our exponent by 1.

Next, we need to check the exponent to make sure it is still valid. If the exponent is greater than 126, the product is bigger than what fits in a float, so we should return infinity. This can be done by setting the exponent to 127, and the frac to zero. If the exponent is less than -126, then we have two choices... either we can try to express the result as a sub-normal number, or we can return a zero value, indicating that the number "underflowed" - was too little to fit in a float.  For now, assume the number underflowed, and return a zero. (We will work on sub-normal numbers for extra credit if you still have time.)

The last step is to return the result of ```makeFloat(psign,pexp,pfrac)```, where `psign` is the value of the sign bit of the product, `pexp` is the exponent of the product, and `pfrac` is the fractional part (times 2<sup>23</sup>) of the product.

Compile and test your result. The floadProd main function will print out the internal representation of your result as well as the internal representation of the floating point product calculated by C. If you have written your code correctly, these should match - at least for all cases where the arguments are normal numbers.

## Handling Special Cases

Floating point arithmetic requires handling of many special cases! The table below defines how each of these special cases should be handled, assuming we are computing ```(float)a * (float)b```, and the value of `a` is defined by the first column, and the value of 'b' is defined by the top row, and the value of the result of a x b is in the cell.

|     | +0 | -0 |+inf|-inf|NAN |nrml| 
|-----|----|----|----|----|----|----|
| **+0**  | +0 | -0 |NAN |NAN |NAN |+/-0|
| **-0**  | -0 | +0 |NAN |NAN |NAN |-/+0|
|**+inf** |NAN |NAN |+inf|-inf|NAN |+/-inf|
|**-inf** |NAN |NAN |-inf|+inf|NAN |-/+inf|
| **NAN** |NAN |NAN |NAN |NAN |NAN |NAN |
|**nrml** |+/-0|-/+0|+/-inf|-/+inf|NAN|a x b |

Note that most of the time, the bottom right cell, labeled *a x b* results in a normal floating point number as a result, but as we have seen above, it is possible for the result to be +/- infinity, or +/-0.

Add code to your ```floatProd``` function to check to see if either `a` or `b` is a special case. You can use the ```isnanf(a)``` library function in "math.h" to determine if a is "Not a Number", or the ```isinff(a)``` function in the same library to determine is a is infinite. Note that the ```float2Sign(a)``` function works for zero, and infinite numbers. (It also works for "Not a Number", even though the sign for "Not a Number" doesn't really make sense. See the warning above.)

Change the code, and run make with the "testSpecial" taget to see if your results are correct. (Note... the C library may incorrectly report -nan in some cases... you may ignore signs in front of "Not a Number" values, even if they don't match what you calculated.)

## Handling Sub-Normal Numbers

If you still have time, for extra credit, see if you can modify your code to handle sub-normal numbers as well as normal numbers. Remember, the binary representation of sub-normal numbers has a biased exponent of 0 (which is -127 unbiased), and there is no implicit "1." in front of the binary representation of the fraction... instead we assume "0." in front of the fraction.

The code provided to return the internal integer representation of fractions handle sub-normal numbers correctly. The ```float2Exp``` routine returns -126 as the exponent for a sub-normal number, and ```float2Frac``` will return a number less than 2<sup>24</sup> to indicate that the "0." has been assumed. 

Handling sub-normal numbers starts off just like handling normal numbers. However, when you multiply fractions, if one of the arguments is sub-normal, you can end up with a value that is between 0 and 4 instead of 1 and 4 times 2<sup>23</sup>. If the result is between 0 and 1, and the exponent is greater than -126, then you need to shift the fraction to the left and decrease exponent until either the exponent reaches -126, or there is a 1 in the 24'th bit position of the fraction. If the exponent is less than -126, then you need to shift the fraction to the right and increase the exponent until either the fraction is entirely shifted out (equal to zero), or the exponent reaches -126.

The ```makeFloat``` function is smart enough to know that if the exponent is -126 and the integer representation of the fraction is less than 2<sup>24</sup>, the result is a sub-normal float value, and it will make a sub-normal floating point result.

Change the code, and test it using the "testSubNorm" target provided in the Makefile. Note that there may be cases where the C library uses a slightly different rounding scheme than the one you implement in your code, so if the last binary digit of your result is different than the C library, that's OK.

## Submitting your Lab

When you are finished editing and testing your code, make sure to commit and push your repository using git.  Then, do a `git rev-parse HEAD` from the project directory to get the hash code associated with the commit. Cut and paste that hashCode into the Lab 02 submission area in the Lab Submissions folder under Content for CS-220 in myCourses. You may submit as many times as you wish up to the deadline of midnight tonight (Feb. 5), only the last submission will be graded.

## Grading your Submission

If you turn in your code late, 4 points will be deducted, unless you have received an extension from the professor. Any submissions after Thursday will receive a zero grade.

Missing or lack of attempt to solve the problem will result in a zero grade.

If your code does not compile, or has an obvious bug, you will get a 3 point deduction for each bug. Bugs include things like floating point multiplication producing a wrong value, or using floating point multiplication in the ```floatProd``` function.

Your code will be run with several normal and special case values (probably different values than those in the Makefile test cases), and a point will be deducted for each incorrect response (a response that does not match the C floating point product.)

Your code will then be run with four sub-normal test case values. Each sub-normal case for which your code produces a correct response will get a 1 point *addition* to your score up to the maximum score of 10 points.

There will be a 1 point deduction for each class of compiler warning generated by your code. 


