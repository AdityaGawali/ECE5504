/**
 * Authors:
 * Aditya Rajendra Gawali
 * Maxwell Stelmack
 * 
 * To compile:
 * 1) Open a linux terminal.
 * 2) Set working directory to final_project.
 * 3) Run "make".
 * 
 * To run:
 * 1) Open a linux terminal.
 * 2) Set working directory to final_project.
 * 3) Run "./final_project".
 */
#include "../include/main.hpp"

int main() {
	Dump dump((char *) "memory_dumps/620.omnetpp_s_5.dump");
	//Dump dump((char *) "memory_dumps/600.perlbench_s_5.dump");



	// for (int i = MAX_CONST_BITS; i > MIN_CONST_BITS; i -= 2) {
	// 	std::cout << "Testing for 2^" << i << " bins:" << std::endl;
	// 	dump.histogram_binning(NUM_BASES, i);
	// 	dump.calculate_huffman_codes();
	// 	dump.pack();
	// }

	dump.histogram_binning(NUM_BASES, 30);
	dump.calculate_huffman_codes();
	dump.pack();
	return 0;
}