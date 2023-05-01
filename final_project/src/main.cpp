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

void displayUsage();
bool isNumber(const std::string& str);
void fileAnalysis(Dump* dump_ptr, int bin_size);

int main(int argc, char* argv[]) {
	if (argc < 3 || argv[1] == "-h" || argv[2] == "-h") {
		std::cout << "\nImproper arguments!\n\n";
		displayUsage();
		return 1;
	} else if (!isNumber(argv[1])) {
		std::cout << "\nFile ID must be an integer!\n\n";
		displayUsage();
		return 1;
	}

	int fileID = std::atoi(argv[1]);

	std::unordered_map<int, std::string> fileNames;
	/* C++ Workloads */
	fileNames[0] = "/home/memory_dumps/C_Workloads/600.perlbench_s_5.dump";
	fileNames[1] = "/home/memory_dumps/C_Workloads/620.omnetpp_s_5.dump";
	fileNames[2] = "/home/memory_dumps/C_Workloads/parsec_fluidanimate5dump";
	fileNames[3] = "/home/memory_dumps/C_Workloads/parsec_freqmine5dump";

	/* Java Workloads */
	fileNames[4] = "/home/memory_dumps/Java_Workloads/ConnectedComponent_3.dump";
	fileNames[5] = "/home/memory_dumps/Java_Workloads/DecisionTree_4.dump";
	fileNames[6] = "/home/memory_dumps/Java_Workloads/KMeans_2.dump";
	fileNames[7] = "/home/memory_dumps/Java_Workloads/LogisticRegression_5.dump";
	fileNames[8] = "/home/memory_dumps/Java_Workloads/MatrixFactorization_4.dump";
	fileNames[9] = "/home/memory_dumps/Java_Workloads/SVM_3.dump";
	fileNames[10] = "/home/memory_dumps/Java_Workloads/TriangleCount_3.dump";

	std::string fileName = fileNames[fileID];

	if (fileName == "") {
		std::cout << "\nInvalid File ID!\n\n";
		displayUsage();
		return 1;
	} else if (strcmp(argv[2], "sweep") && !isNumber(argv[2])) {
		std::cout << "\nBin size must be an integer!\n\n";
		displayUsage();
		return 1;
	}

	int bin_size = std::atoi(argv[2]);

	if (bin_size%2) {
		std::cout << "\nBin size must be an even integer!\n\n";
		displayUsage();
		return 1;
	}

	std::cout << std::endl << "Compression analysis for " << fileName << ":" << std::endl;

	Dump dump((char *) fileName.c_str());

	if (!strcmp(argv[2], "sweep")) {
		for (int i = 30; i >= 2; i -= 2) {
			fileAnalysis(&dump, 32-i);
		}
	} else {
		fileAnalysis(&dump, bin_size);
	}

	return 0;
}

void displayUsage() {
	std::cout << "Call this executable in the following format:\n"
			  << "./final_project <file ID> <bin size>\n\n"

			  << "<file ID>: Here is the list of file IDs:\n"
			  << "  C++ Loads:\n"
			  << "    0:  perlbench\n"
			  << "    1:  omnetpp\n"
			  << "    2:  fluidanimate\n"
			  << "    3:  freqmine\n"
			  << "  Java Loads:\n"
			  << "    4:  ConnectedComponent\n"
			  << "    5:  DecisionTree\n"
			  << "    6:  KMeans\n"
			  << "    7:  LogisticRegression\n"
			  << "    8:  MatrixFactorization\n"
			  << "    9:  SVM\n"
			  << "    10: TriangleCount\n\n"

			  << "<bin size>: Even integer between 2 and 30, inclusive.\n\n"

			  << "Type -h for any of these arguments to show this help message.\n";
}

bool isNumber(const std::string& str)
{
    return !str.empty() && std::find_if(str.begin(), str.end(), [](unsigned char chr) { return !std::isdigit(chr); }) == str.end();
}

void fileAnalysis(Dump* dump_ptr, int bin_size) {
	std::cout << "Testing for 2^" << 32 - bin_size << " bins (bin size is " << bin_size << "):" << std::endl;
	dump_ptr->histogram_binning(NUM_BASES, 32 - bin_size);
	dump_ptr->calculate_huffman_codes();
	dump_ptr->pack();
}