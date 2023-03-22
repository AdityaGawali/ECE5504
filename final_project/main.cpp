/**
 * To compile:
 * 
 * To run:
 * 
 */
#include "main.hpp"

int main() {
	process_dump_file("memory_dumps/600.perlbench_s_5.dump");

	return 0;
}

void process_dump_file(char* filename) {
	FILE* dumpfile_p;
	dumpfile_p = fopen(filename, "rb");

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


		std::vector<Page> page_vec;
		uint64_t zero_page_count = 0;

		for (Elf64_Phdr program_header : program_header_vec) {
			bool partial_page = program_header.p_filesz != program_header.p_memsz;
			uint16_t size_of_partial_page = program_header.p_filesz % PAGE_SIZE;
			unsigned int num_pages = program_header.p_memsz / PAGE_SIZE;

			fseek(dumpfile_p, program_header.p_offset, SEEK_SET);


			for (int i = 0; i < num_pages; i++) {
				Page page;
				if (i == num_pages-1 && partial_page) {
					page.set_to_zero();
					fread(page.data, sizeof(byte), size_of_partial_page, dumpfile_p);
				} else {
					fread(page.data, sizeof(byte), PAGE_SIZE, dumpfile_p);
				}
				if (page.is_nonzero()) {
					page_vec.push_back(page);
				} else {
					zero_page_count++;
				}
			}
		}
		std::cout << "Number of pages loaded from file: " << page_vec.size() << std::endl
				  << "Zero pages skipped: " << zero_page_count << std::endl
				  << "contents of first page: " << std::endl;
		
		page_vec.at(10000).print();
		std::cout << std::endl;

		// finally close the file
		fclose(dumpfile_p);
	}
}