#include <iostream>
#include <random>
#include <chrono>
#include "DataStructure.h"

using namespace std;

int main(int argc, char **argv) {
	/*if (argc < 3) {
		cout << "Error, arguments missing!\n";
		cout << "usage: " << argv[0] << " <input file> <time> [outputDir]" << endl;
		return -1;
	}*/

	//string inputFile = argv[1];

	DataStructure *data = DataStructure::fromFile("C:/Users/Daniel/Desktop/Benchmarck2015/I_30_2_02_02_2.csv");

	return 0;
}