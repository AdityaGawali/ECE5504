#include "dump.hpp"

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

		std::cout << "e_type: " 	 << elf_header.e_type 	 	<< std::endl
				  << "e_machine: " 	 << elf_header.e_machine 	<< std::endl
				  //<< "e_version: " 	 << elf_header.e_version 	<< std::endl
				  << "e_entry: " 	 << elf_header.e_entry 	 	<< std::endl
				  << "e_phoff: " 	 << elf_header.e_phoff 	 	<< std::endl
				  //<< "e_shoff: " 	 << elf_header.e_shoff	 	<< std::endl
				  //<< "e_flags: " 	 << elf_header.e_flags	 	<< std::endl
				  << "e_ehsize: "	 << elf_header.e_ehsize 	<< std::endl
				  << "e_phentsize: " << elf_header.e_phentsize  << std::endl
				  << "e_phnum: " 	 << elf_header.e_phnum 		<< std::endl
				  //<< "e_shentsize: " << elf_header.e_shentsize 	<< std::endl
				  //<< "e_shnum: " 	 << elf_header.e_shnum 		<< std::endl
				  //<< "e_shstrndx: "  << elf_header.e_shstrndx 	<< std::endl
				  << std::endl;

		std::vector<Elf64_Phdr> program_header_vec;
		
		fseek(dumpfile_p, elf_header.e_phoff, SEEK_SET);
		for (int i = 0; i < elf_header.e_phnum; i++)
		{
			Elf64_Phdr program_header;
			fread(&program_header, sizeof(program_header), 1, dumpfile_p);

			// Only continue if segment contains memory data
			if (program_header.p_type != PT_LOAD) {
				continue;
			}

			program_header_vec.push_back(program_header);

			std::cout << "p_type: " << program_header.p_type << std::endl
					<< "p_offset: " << program_header.p_offset << std::endl
					//<< "p_vaddr: " << program_header.p_vaddr << std::endl
					//<< "p_paddr: " << program_header.p_paddr << std::endl
					<< "p_filesz: " << program_header.p_filesz << std::endl
					<< "p_memsz: " << program_header.p_memsz << std::endl
					//<< "p_flags: " << program_header.p_flags << std::endl
					//<< "p_align: " << program_header.p_align << std::endl
					<< "number of pages from file size: " << program_header.p_filesz * 1.0 / PAGE_SIZE << std::endl
					<< "number of pages from memory size: " << program_header.p_filesz * 1.0 / PAGE_SIZE << std::endl
					<< std::endl;
		}

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
								if (! (++sample_counter % 57)) {
									sample_counter = 0;
									this->smallest_bins[word_to_value(word)/this->smallest_bin_size]++;
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

		this->uncompressed_size = this->pages.size() * PAGE_SIZE;

		std::cout << "Number of pages loaded from file: " << this->pages.size() << std::endl
				  << "Zero-pages skipped: " << zero_page_count << std::endl
				  << "Uncompressed size (bytes): " << this->uncompressed_size << std::endl
				  << "Contents of first page: " << std::endl;
		
		this->pages.at(0).print();
		std::cout << std::endl;

		// finally close the file
		fclose(dumpfile_p);
	}
}

void Dump::histogram_binning(unsigned long num_bases, unsigned long num_bins) {
	unsigned long bin_size = this->num_values / num_bins;
	std::list<std::pair<unsigned long, unsigned long>> best_bins;
	std::vector<unsigned long> min_prox_vals;
	std::vector<unsigned long> max_prox_vals;

	for (unsigned long i = 0; i < num_bins; i++) {
		if (best_bins.empty()) {
			best_bins.push_back(std::make_pair(i, this->smallest_bins[i]));
		} else if (best_bins.size() < num_bases || this->smallest_bins[i] > best_bins.end()->second) {
			bool found = false;
			for (auto iter = best_bins.begin(); iter != best_bins.end(); iter++) {
				if (this->smallest_bins[i] > iter->second) {
					best_bins.insert(iter, std::make_pair(i, this->smallest_bins[i]));
					if (best_bins.size() > num_bases)
						best_bins.pop_back();
					found = true;
					break;
				}
			}
			if (!found && best_bins.size() < num_bases) {
				best_bins.push_back(std::make_pair(i, this->smallest_bins[i]));
			}
		}
	}

	for (auto iter = best_bins.begin(); iter != best_bins.end(); iter++) {
		if (iter->second) {
			unsigned long min_prox_val = -1;
			unsigned long max_prox_val = 0;
			unsigned long max_val = 0;
			unsigned long base = 0;
			for (unsigned long i = iter->first * bin_size; i < (iter->first + 1) * bin_size; i++) {
				if (this->histogram[i]) {
					if (i < min_prox_val) {
						min_prox_val = i;
					} else if (i > max_prox_val) {
						max_prox_val = i;
					}
					if (this->histogram[i] > max_val) {
						max_val = this->histogram[i];
						base = i;
					}
				}
			}
			min_prox_vals.push_back(min_prox_val);
			max_prox_vals.push_back(max_prox_val);
			this->bases.push_back(base);
		} else {
			break;
		}
	}

	int count = 0;
	for (int i = 0; i < this->bases.size(); i++) {
		unsigned int bit_delta = 0;

		for (unsigned long delta = std::max(this->bases[i] - min_prox_vals[i], max_prox_vals[i] - this->bases[i]); delta > 0; delta /= 2) {
			++bit_delta;
		}

		std::cout << min_prox_vals[i] << " - " << this->bases[i] << " - " << max_prox_vals[i] << ": " << bit_delta << std::endl;
		deltas.push_back(std::min((unsigned int) MAX_DELTA, bit_delta));

		if (++count == 10) {
			break;
		}
	}
}