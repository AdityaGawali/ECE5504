#include "../include/dump.hpp"

Dump::Dump(char* filename) {
    this->filename = filename;
	this->sampled_values = 0;
    this->load();
}

void Dump::load() {
    FILE* dumpfile_p;
	dumpfile_p = fopen(this->filename, "rb");

	if(dumpfile_p) {
		Elf64_Ehdr elf_header;

		// read the ELF header
		fread(&elf_header, sizeof(elf_header), 1, dumpfile_p);

		// check so its really an elf file
		if (memcmp(elf_header.e_ident, ELFMAG, SELFMAG) == 0) {
		// this is a valid elf file
		}

		// std::cout << "e_type: " 	 << elf_header.e_type 	 	<< std::endl
		// 		  << "e_machine: " 	 << elf_header.e_machine 	<< std::endl
		// 		  //<< "e_version: " 	 << elf_header.e_version 	<< std::endl
		// 		  << "e_entry: " 	 << elf_header.e_entry 	 	<< std::endl
		// 		  << "e_phoff: " 	 << elf_header.e_phoff 	 	<< std::endl
		// 		  //<< "e_shoff: " 	 << elf_header.e_shoff	 	<< std::endl
		// 		  //<< "e_flags: " 	 << elf_header.e_flags	 	<< std::endl
		// 		  << "e_ehsize: "	 << elf_header.e_ehsize 	<< std::endl
		// 		  << "e_phentsize: " << elf_header.e_phentsize  << std::endl
		// 		  << "e_phnum: " 	 << elf_header.e_phnum 		<< std::endl
		// 		  //<< "e_shentsize: " << elf_header.e_shentsize 	<< std::endl
		// 		  //<< "e_shnum: " 	 << elf_header.e_shnum 		<< std::endl
		// 		  //<< "e_shstrndx: "  << elf_header.e_shstrndx 	<< std::endl
		// 		  << std::endl;

		std::vector<Elf64_Phdr> program_header_vec;
		
		fseek(dumpfile_p, elf_header.e_phoff, SEEK_SET);
		this->uncompressed_size = 0;
		for (int i = 0; i < elf_header.e_phnum; i++)
		{
			Elf64_Phdr program_header;
			fread(&program_header, sizeof(program_header), 1, dumpfile_p);

			// Only continue if segment contains memory data
			if (program_header.p_type != PT_LOAD) {
				continue;
			}

			program_header_vec.push_back(program_header);
			this->uncompressed_size += program_header.p_filesz;

			// std::cout << "p_type: " << program_header.p_type << std::endl
			// 		<< "p_offset: " << program_header.p_offset << std::endl
			// 		//<< "p_vaddr: " << program_header.p_vaddr << std::endl
			// 		//<< "p_paddr: " << program_header.p_paddr << std::endl
			// 		<< "p_filesz: " << program_header.p_filesz << std::endl
			// 		<< "p_memsz: " << program_header.p_memsz << std::endl
			// 		//<< "p_flags: " << program_header.p_flags << std::endl
			// 		//<< "p_align: " << program_header.p_align << std::endl
			// 		<< "number of pages from file size: " << program_header.p_filesz * 1.0 / PAGE_SIZE << std::endl
			// 		<< "number of pages from memory size: " << program_header.p_filesz * 1.0 / PAGE_SIZE << std::endl
			// 		<< std::endl;
		}

		const unsigned long SAMPLE_COUNTER = this->uncompressed_size / 10 * 9 / SAMPLE_SIZE;

		uint64_t zero_page_count = 0;

		for (Elf64_Phdr program_header : program_header_vec) {
			bool partial_page = program_header.p_filesz != program_header.p_memsz;
			uint16_t size_of_partial_page = program_header.p_filesz % PAGE_SIZE;
			unsigned int num_pages = program_header.p_memsz / PAGE_SIZE;
			int sample_counter = 0;

			fseek(dumpfile_p, program_header.p_offset, SEEK_SET);


			for (int i = 0; i < num_pages; i++) {
				Page page;
				if (i == num_pages-1 && partial_page) {
					page.set_to_zero();
					fread(page.blocks, sizeof(byte), size_of_partial_page, dumpfile_p);
				} else {
					fread(page.blocks, sizeof(byte), PAGE_SIZE, dumpfile_p);
				}
				if (page.is_nonzero()) {
					this->pages.push_back(page);
					if (this->sampled_values < SAMPLE_SIZE) {
						for (Block block : page.blocks) {
							for (Word word : block.words) {
								this->histogram[word_to_value(word)]++;
								if (! (++sample_counter %= SAMPLE_COUNTER)) {
									this->values.push_back(word_to_value(word));
									this->smallest_bins[this->get_bin_id(word_to_value(word), MAX_CONST_BITS)]++;
									if (++this->sampled_values >= SAMPLE_SIZE) {
										break;
									}
								}
							}
							if (this->sampled_values >= SAMPLE_SIZE) {
								break;
							}
						}
					}
					
				} else {
					zero_page_count++;
				}
			}
		}
		std::sort(this->values.begin(), this->values.end());
		this->values.erase(std::unique(this->values.begin(), this->values.end()), this->values.end());

		this->uncompressed_size = this->pages.size() * PAGE_SIZE;

		std::cout << "Number of pages loaded from file: " << this->pages.size() << std::endl
		 		  << "Zero-pages skipped: " << zero_page_count << std::endl
		 		  << "Sample Counter: " << SAMPLE_COUNTER << std::endl
		 		  << "Uncompressed size (bytes): " << this->uncompressed_size << std::endl;
		 		//   << "Contents of first page: " << std::endl;
		
		//this->pages.at(0).print();
		//std::cout << std::endl;

		// finally close the file
		fclose(dumpfile_p);
	}
}

void Dump::histogram_binning(unsigned int num_bases, unsigned int num_bits) {
	this->bases.clear();
	this->deltas.clear();

	const unsigned int NUM_CHANGE_BITS = WORD_SIZE_BITS-num_bits;
	const unsigned int MAX_BIN_ID = std::pow(2, num_bits)-1;
	const unsigned int MAX_CHANGE_BITS_VALUE = std::pow(2, NUM_CHANGE_BITS)-1;
	std::unordered_map<unsigned int, unsigned int> organized_bins;
	std::list<std::pair<unsigned int, unsigned int>> best_bins;
	std::vector<unsigned int> min_prox_vals;
	std::vector<unsigned int> max_prox_vals;
	bool zero_is_used = false;

	if (num_bits != MAX_CONST_BITS) {
		for (std::pair<unsigned int, unsigned int> bin : this->smallest_bins) {
			//std::cout << std::bitset<32>(bin.first) << " turns into " << std::bitset<32>(this->get_bin_id(bin.first, num_bits + (WORD_SIZE_BITS - MAX_CONST_BITS))) << std::endl;
			organized_bins[this->get_bin_id(bin.first, num_bits + (WORD_SIZE_BITS - MAX_CONST_BITS))] += bin.second;
		}
	} else {
		organized_bins = this->smallest_bins;
	}

	std::cout << "Number of bins created: " << organized_bins.size() << std::endl;

	// Find the best bins
	for (std::pair<unsigned int, unsigned int> bin : organized_bins) {
		if (best_bins.empty()) {
			best_bins.push_back(bin);
		} else if (best_bins.size() < num_bases || bin.second > best_bins.back().second) {
			bool found = false;
			for (auto iter = best_bins.begin(); iter != best_bins.end(); iter++) {
				if (bin.second > iter->second) {
					best_bins.insert(iter, bin);
					if (best_bins.size() > num_bases)
					{
						best_bins.pop_back();
					}
						
					found = true;
					break;
				}
			}
			if (!found && best_bins.size() < num_bases) {
				best_bins.push_back(bin);
			}
		}
	}

	// Find the base for each best bin
	for (auto iter = best_bins.begin(); iter != best_bins.end(); iter++) {
		if (iter->second) {
			unsigned int min_prox_val = -1;
			unsigned int max_prox_val = 0;
			unsigned int max_val = 0;
			unsigned int base = 0;
			unsigned int bit_delta = 0;
			unsigned int starting_value = iter->first << NUM_CHANGE_BITS;
			unsigned int ending_value = starting_value + MAX_CHANGE_BITS_VALUE;

			// Iterate through all elements in the bin and find the base
			auto values_iter = std::find_if(std::begin(this->values), std::end(this->values), [starting_value](int i) {return i > starting_value;});
			while (values_iter != std::end(this->values) && *values_iter < ending_value) {
				if (this->histogram[*values_iter] > max_val) {
					max_val = this->histogram[*values_iter];
					base = *values_iter;
				}
				values_iter++;
			}
			//std::cout << "Base selected: " << base << " from range " << starting_value << " to " << ending_value << std::endl;
			if (base == 0 && !zero_is_used)
			{
				zero_is_used = true;
				this->bases.push_back(base);
			} else if (base != 0) {
				this->bases.push_back(base);
			} else {
				continue;
			}

			// Iterate through all elements in the bin and find bit delta
			values_iter = std::find_if(std::begin(this->values), std::end(this->values), [starting_value](int i) {return i > starting_value;});
			while (values_iter != std::end(this->values) && *values_iter < ending_value) {
				if (this->histogram[*values_iter]) {
					//std::cout << complete_value << std::endl;
					bit_delta = std::max(bit_delta, this->bit_difference(*values_iter, base));
				}
				values_iter++;
			}
			this->deltas.push_back(std::min((unsigned int) MAX_DELTA, bit_delta));
		} else {
			break;
		}
	}
	std::cout << "Bases generated: " << this->bases.size() << std::endl;
			  //<< "Deltas generated: " << this->deltas.size() << std::endl;
			  //<< "Sample of bases and deltas:" << std::endl;

	/*
	for (int i = 0; i < this->bases.size(); i++) {
 		std::cout << this->bases[i] << ": " << this->deltas[i] << std::endl;
	}
	*/

}

unsigned int Dump::get_bin_id(unsigned int value, unsigned int num_bits) {
	unsigned int shift_bits = WORD_SIZE_BITS - num_bits;
	unsigned safety_mask = (1 << (num_bits)) - 1;
	unsigned int id_mask = safety_mask << shift_bits;
	return ((value & id_mask) >> shift_bits) & safety_mask;
}


unsigned int Dump::bit_difference(unsigned int value1, unsigned int value2) {
	unsigned int delta = 0;
	while (value1 != value2) {
		delta++;
		value1 /= 2;
		value2 /= 2;
	}
	return delta;
}

void Dump::calculate_huffman_codes()
{
	this->sorted_bp.clear();
	std::copy(this->bases.begin(),this->bases.end(),back_inserter(this->sorted_bp));
	std::sort(this->sorted_bp.begin(),this->sorted_bp.end());
	unsigned int closest_bp;
	unsigned int value;

	std::map<unsigned int,unsigned int>::iterator itr;

	unsigned int base_index;



	for(int i = 0 ; i< this->pages.size();i++)
	{
		for (Block block : pages.at(i).blocks) 
		{
			for (Word word : block.words) 
			{
				value = word_to_value(word);
				closest_bp = find_closest(this->sorted_bp,value);

				base_index = getIndexfromVec(this->bases, closest_bp);

				ptr = this->bp_freq.insert( std::pair<unsigned int, unsigned int>(base_index,1));
				if(!ptr.second)
				{

					itr = this->bp_freq.find(base_index);
					itr->second++;
				}
				

			}
		}
	}


	/* Convert map to vector here*/
	for( itr = this->bp_freq.begin(); itr != this->bp_freq.end(); ++itr)
	{
		//std::cout << "base pointer => " << itr->first << ", frequency => " << itr->second << std::endl;
		this->huff_bp.push_back(itr->first);
		this->huff_freq.push_back(itr->second);

	}

	
	this->codeMap = encode(huff_bp, huff_freq);
}

float Dump::pack() 
{

	unsigned int word_counter = 0;

	std::vector<bool> packed_deltas;
	std::vector<bool> packed_outliers;
	std::vector<bool> packed_base_pointers;
	std::vector<bool> packed_mask (16,0);

	std::vector<bool> compressed_block;
	std::vector<bool>compressed_page;

	unsigned long long comp_p_size = 0;

	std::vector<bool> huffman_code_bit_vec;



	for(int i = 0 ; i< this->pages.size();i++)
	{
		// compressed_block.clear();

		for (Block block : pages.at(i).blocks) 
		{
			packed_deltas.clear();
			packed_mask.clear();
			packed_base_pointers.clear();
			packed_outliers.clear();
			word_counter = 0;


			for (Word word : block.words) 
			{
				Packed_data packed_data;


				unsigned int value = word_to_value(word);
				unsigned int closest_bp;
				closest_bp = find_closest(this->sorted_bp,value);
				int index_delta = getIndexfromVec(this->bases, closest_bp);
				
				/*
					We have identified the closest base pointer
					store the base pointer in a vector, with it's index 
				*/

			/* Store closest base pointer in map with frequency 1 */


				int delta_tmp = closest_bp - value;
				packed_data.value = value;
				packed_data.base_pointer = closest_bp;
				packed_data.delta = std::abs(delta_tmp);
				//std::cout << "Test 1" << std::endl;
				//std::cout << index_delta << std::endl;
				if(this->deltas.at(index_delta) > std::abs(delta_tmp))
				{
					//Since mask is true, pack the Delta_k bits of calculated delta in a bit array
					packed_data.mask = true;
					unsigned int MAX_DELTA = this->deltas.at(index_delta);
					    for(int i = MAX_DELTA-1;i>=0;i--)
    					{
       					 packed_deltas.push_back(get_bit(packed_data.delta,i));
						}
				}
				else
				{
					//Since mask is false, pack the complete bits of value in a bit array

					packed_data.mask = false;
					for(int i = 31;i>=0;i--)
					{
						packed_outliers.push_back(get_bit(packed_data.value,i));
					}
					packed_mask[15 - word_counter].flip();
				}


				 huffman_code_bit_vec =  get_huffman_code (index_delta,this->codeMap);
				 for (int i = 0; i< huffman_code_bit_vec.size();i++)
				 {
					packed_base_pointers.push_back(huffman_code_bit_vec[i]);
				 }



				this->packed_data.push_back(packed_data);

				//std::cout<<"Value: "<<packed_data.value<<"\tBP: "<<packed_data.base_pointer<<"\tDelta: "<<packed_data.delta<<"\tmask: "<<packed_data.mask<<std::endl;
				//std::cout<<word_counter<<std::endl;
				word_counter++;
			}
			
			/*
				THIS TRANSFORMS A BLOCK into GBDI Compressed block
				Concatenate all the values in the GBDI format
				MASK-BP-DELTAS-OUTLIERS
			*/
			// for(int i=0;i<packed_mask.size();i++)
			// {
			// 	compressed_block.push_back(packed_mask[i]);
			// }
			// for(int i=0;i<packed_base_pointers.size();i++)
			// {
			// 	compressed_block.push_back(packed_base_pointers[i]);
			// }
			// for(int i=0;i<packed_deltas.size();i++)
			// {
			// 	compressed_block.push_back(packed_deltas[i]);
			// }
			// for(int i=0;i<packed_outliers.size();i++)
			// {
			// 	compressed_block.push_back(packed_outliers[i]);
			// }
			comp_p_size += packed_mask.size();
			comp_p_size += packed_base_pointers.size();
			comp_p_size += packed_deltas.size();
			comp_p_size += packed_outliers.size();
		}
		// for(int i=0;i<compressed_block.size();i++)
		// {
		// 	compressed_page.push_back(compressed_block[i]);
		// }
	}
	// unsigned long long size = compressed_page.size();
	// size = size /8;

	// std::cout<<"Size of the compressed dump is "<<size<<" Bytes"<<std::endl;
	// std::cout<<"Compression Ratio is "<<size/this->uncompressed_size<<std::endl;

	unsigned long long size = comp_p_size;
	size = size /8;

	std::cout<<"Size of the original dump is "<<this->uncompressed_size<<" Bytes"<<std::endl;
	std::cout<<"Size of the compressed dump is "<<size<<" Bytes"<<std::endl;

	float comp_ratio = (float)this->uncompressed_size/(float)size;
	std::cout<<"Compression Ratio is "<<comp_ratio<<std::endl;




	return comp_ratio;
}


unsigned int Dump::find_closest(std::vector<unsigned long> vec, unsigned int target) 
{
	unsigned int size = vec.size();

	
	if(target <= vec.at(0))
	{
		return vec.at(0);
	}
	if(target >= vec.at(size-1))
	{
		return vec.at(size-1);
	}
	int i=0,j=size,mid=0;
    while (i < j) {
        mid = (i + j) / 2;
        if (vec.at(mid) == target)
            return vec.at(mid);
 
        /* If target is less than array element,
            then search in left */
        if (target < vec.at(mid)) {
 
            // If target is greater than previous
            // to mid, return closest of two
            if (mid > 0 && target > vec.at(mid-1))
			{
                return getClosest(vec.at(mid-1),vec.at(mid), target);
			}
            /* Repeat for left half */
            j = mid;
        }
 
        // If target is greater than mid
        else {
            if (mid < size - 1 && target < vec.at(mid+1))
                return getClosest(vec.at(mid),vec.at(mid+1), target);
            // update i
            i = mid + 1;
        }
    }
 
    // Only single element left after search
    return vec.at(mid);
}

unsigned int Dump::getClosest(unsigned int val1,unsigned int val2, int target)
{
    if (target - val1 >= val2 - target)
        return val2;
    else
        return val1;
}

unsigned int Dump::getIndexfromVec(std::vector<unsigned int> vec, unsigned int target)
{
	for (int i=0;i<vec.size();i++)
	{
		if(vec.at(i) == target)
		{
			return i;
		}
	}
	return -1;
}
bool Dump::get_bit(unsigned int value, unsigned int bit_pos)
{
	return (1 & (value >> bit_pos));
}

// Function to build the Huffman tree
TreeNode* Dump::buildHuffmanTree(std::vector<unsigned int>& data, std::vector<unsigned int>& frequency) {
    std::priority_queue<TreeNode*, std::vector<TreeNode*>, Compare> pq;
    for (int i = 0; i < data.size(); i++) {
        TreeNode* node = new TreeNode(data[i], frequency[i]);
        pq.push(node);
    }
    while (pq.size() > 1) {
        TreeNode* left = pq.top(); pq.pop();
        TreeNode* right = pq.top(); pq.pop();
        TreeNode* parent = new TreeNode(0, left->frequency + right->frequency);
        parent->left = left;
        parent->right = right;
        pq.push(parent);
    }
    return pq.top();
}

// Function to perform pre-order traversal of the Huffman tree
void Dump::encodeHelper(TreeNode* node, std::string code, std::unordered_map<unsigned int, std::string>& codeMap) {
    if (node == nullptr) {
        return;
    }
    if (node->left == nullptr && node->right == nullptr) {
        codeMap[node->data] = code;
    }
    encodeHelper(node->left, code + "0", codeMap);
    encodeHelper(node->right, code + "1", codeMap);
}

// Function to encode the data using the Huffman tree
std::unordered_map<unsigned int, std::string> Dump::encode(std::vector<unsigned int>& data, std::vector<unsigned int>& frequency) {
    std::unordered_map<unsigned int, std::string> codeMap;
    TreeNode* root = buildHuffmanTree(data, frequency);
    encodeHelper(root, "", codeMap);
    return codeMap;
}

// Function to get the encoded data
std::vector<bool> Dump::get_huffman_code(unsigned int data, std::unordered_map<unsigned int, std::string>& codeMap) {
    // std::cout << "Encoded data:" << std::endl;
    
	std::string code_string= codeMap[data];
	std::vector<bool> code_bit_vec;

	code_bit_vec.reserve(code_string.size());

	for(auto a : code_string)
	{
    	code_bit_vec.push_back(a == '1');
	}
	return code_bit_vec;
}


