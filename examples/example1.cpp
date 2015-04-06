#include <iostream>
#include "../src/tiarg.hpp"


int main(int argc, char** argv){
	TiObj param;
	getArgs(param, argc, argv);
	cout << param;
	return 0;
}
