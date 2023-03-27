/**
 * Authors:
 * Aditya Rajendra Gawali
 * Maxwell Stelmack
 * 
 * To compile:
 * 1) Open a linux terminal.
 * 2) Set working directory to the directory of this file.
 * 3) Run "g++ -o final_project *.cpp".
 * 
 * To run:
 * 1) Open a linux terminal.
 * 2) Set working directory to the directory of this file.
 * 3) Run "./final_project".
 */
#include "../include/main.hpp"

int main() {
	Dump dump((char *) "memory_dumps/600.perlbench_s_5.dump");
	dump.histogram_binning(NUM_BASES, MAX_CONST_BITS);
	//dump.pack();
	return 0;
}