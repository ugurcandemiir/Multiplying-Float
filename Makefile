test : floatProd
	./floatProd 1.0 -2.75
	./floatProd 3.2 -1.3
	./floatProd 6.22e23 0.005
	
testSpecial : floatProd
	./floatProd nan 0.0
	./floatProd inf 3.754
	./floatProd 7.35 0.0
	./floatProd -inf 3.34
	
testSubNorm : floatProd
	./floatProd 1.3e-42 1.0
	./floatProd 1.0e-20 3.2e-10
	./floatProd 7.993e-20 8.32e-20
	./floatProd 7.993e-24 8.32e-23
	
floatProd : floatProd.c floatInt.h floatInt.c
	gcc -Wall -g -o floatProd floatProd.c floatInt.c
	
clean : 
	-rm floatProd