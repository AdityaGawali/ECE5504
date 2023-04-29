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
	std::vector<std::string> fileNames;
	/* C++ Workloads */
	//fileNames.push_back("memory_dumps/C_Workloads/600.perlbench_s_5.dump");
	//fileNames.push_back("memory_dumps/C_Workloads/620.omnetpp_s_5.dump");
	//fileNames.push_back("memory_dumps/C_Workloads/parsec_fluidanimate5dump");
	fileNames.push_back("memory_dumps/C_Workloads/parsec_freqmine5dump");

	/* Java Workloads */

	std::unordered_map<std::string, unsigned int> constBitsPerFile;
	constBitsPerFile["memory_dumps/C_Workloads/600.perlbench_s_5.dump"] = 24;

	for (std::string fileName : fileNames) {
		std::cout << std::endl << "Compression analysis for " << fileName << ":" << std::endl;

		Dump dump((char *) fileName.c_str());

		if (constBitsPerFile.count(fileName))
		{
			dump.histogram_binning(NUM_BASES, constBitsPerFile[fileName]);
			dump.calculate_huffman_codes();
			dump.pack();
		} else {
			for (int i = 30; i >= (signed int) MIN_CONST_BITS; i -= 2) {
				std::cout << "Testing for 2^" << i << " bins:" << std::endl;
				dump.histogram_binning(NUM_BASES, i);
				dump.calculate_huffman_codes();
				dump.pack();
			}
		}
	}

	
	return 0;
}