all : lambdagen

lambdagen: lambdagen.cpp
	clang++ -ggdb -march=native -O3 -flto -std=gnu++11 -o lambdagen lambdagen.cpp 
