#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

/*
 * ELF parser with no libraries. Only parses for symbol table, names and
 * addresses. Written to parse a 64-bit LSB ELF.
 */



/*
 * ELF Header:
 * Offset	Size	Field 		Purpose
 * 0x00		4		e_ident		magic number
 * 0x04		1		e_ident		32 or 64-bit file
 * 0x05		1		e_ident		little or big endian
 * 0x06		1		e_ident		original or current version of elf
 * 0x07		1		e_ident		target operating system (ABI)
 * 0x08		1		e_ident		ABI version
 * 0x09		7		e_ident		unused bytes
 * 0x10		2		e_type		object file type
 * 0x12		2		e_machine	target set architecture
 * 0x14		4		e_version	original version of elf byte
 * 0x18		8		e_entry		entry point for execution, or 0 for no entry
 * 0x20		8		e_phoff		start of program header table
 * 0x28		8		e_shoff		start of section header table
 * 0x30		4		e_flags		target architecture specific byte
 * 0x34		2		e_ehsize	size of this header
 * 0x36		2		e_phentsize	size of program header table
 * 0x38		2		e_phnum		number of entries in program header table
 * 0x3A		2		e_shentsize	size of section header table
 * 0x3C		2		e_shnum		number of entries in section header table
 * 0x3E		2		e_shstrndx	end
 * 0x40
 */
typedef struct {
	unsigned char e_ident[16];
	uint16_t	  e_type;
	uint16_t	  e_machine;
	uint32_t	  e_version;
	uint64_t	  e_entry;
	uint64_t	  e_phoff;
	uint64_t	  e_shoff;
	uint32_t	  e_flags;
	uint16_t	  e_ehsize;
	uint16_t	  e_phentsize;
	uint16_t	  e_phnum;
	uint16_t	  e_shentsize;
	uint16_t	  e_shnum;
	uint16_t	  e_shstrndx;
} ELF64_header;

/*
 * Section header
 *
 * Offset	Field			Purpose
 * 0x00		sh_name			name of this section
 * 0x04		sh_type			type of header
 * 0x08		sh_flags		attributes of the section
 * 0x10		sh_addr			virtual address of loaded section in memory
 * 0x18		sh_offset		offset of section in file image
 * 0x20		sh_size			size of section
 * 0x28		sh_link			??
 * 0x2C		sh_info			??
 * 0x30		sh_addralign	alignment of section
 * 0x38		sh_entsize		size, in bytes, of each entry
 * 0x40		End
 */
typedef struct {
	uint32_t	sh_name;
	uint32_t	sh_type;
	uint64_t	sh_flags;
	uint64_t	sh_addr;
	uint64_t	sh_offset;
	uint64_t	sh_size;
	uint32_t	sh_link;
	uint32_t	sh_info;
	uint64_t	sh_addralign;
	uint64_t	sh_entsize;
} ELF64_section;

typedef struct {
	FILE *file;
	ELF64_header *elf;
	ELF64_section *sh_table;
	char *sh_strtab;
} ELF64_file;

typedef struct {
	uint32_t		st_name;
	unsigned char 	st_info;
	unsigned char 	st_other;
	uint16_t		st_shndx;
	uint64_t		st_value;
	uint64_t		st_size;
} ELF64_sym;

#define SH_SYMTAB 2
#define SH_STRTAB 3

/*
 * Read ELF as binary. If file magic bytes is not 0x7F, then its not an ELF.
 */
int check_elf(const char *filename, ELF64_header *elf) {
	FILE *file = fopen(filename, "rb");
	if (!file) {
		fprintf(stderr, "Error opening file\n");
		return -1;
	}

	size_t bytes = fread(elf, sizeof(ELF64_header), 1, file);
	if (!bytes) {
		fprintf(stderr, "Error reading bytes\n");
		return -1;
	}
	
	if (elf->e_ident[0] != 0x7F) {
		fprintf(stderr, "File is not an ELF file\n");
		return -1;
	}

	fclose(file);
	return 0;
}

int read_sh_table(ELF64_file *elf) {
	// Allocate memory for section headers
	// The size is number of sh entries * size of a header
	size_t sh_table_size = elf->elf->e_shnum * sizeof(ELF64_section);
	elf->sh_table = malloc(sh_table_size);
	if (!elf->sh_table) {
		fprintf(stderr, "Failed to allocate section header\n");
		return -1;
	}

	// Seek to the start of sh table
	if (fseek(elf->file, elf->elf->e_shoff, SEEK_SET) != 0) {
		fprintf(stderr, "Could not find section header table entry\n");
		return -1;
	}

	// Read section header table entries
	if (fread(elf->sh_table, sizeof(ELF64_section), elf->elf->e_shnum, 
				elf->file) != elf->elf->e_shnum) {
		fprintf(stderr, "Failed to read section header table\n");
		return -1;
	}

	// Find sh string table
	ELF64_section *shstrtab_header = &elf->sh_table[elf->elf->e_shstrndx];
	elf->sh_strtab = malloc(shstrtab_header->sh_size);
	if (!elf->sh_strtab) {
		fprintf(stderr, "Failed to allocate memory for sh string table");
		return -1;
	}

	// Seek to sh string table and read
	if (fseek(elf->file, shstrtab_header->sh_offset, SEEK_SET) != 0 ||
			fread(elf->sh_strtab, shstrtab_header->sh_size, 1, elf->file) != 1) {
		fprintf(stderr, "Failed to read sh string table");
		return -1;
	}

	return 0;
}

const char* get_section_name(ELF64_file *elf, ELF64_section *section) {
	return elf->sh_strtab + section->sh_name;
}

ELF64_section* find_section(ELF64_file *elf, const char *section_name) {
	for (int i = 0; i < elf->elf->e_shnum; i++) {
		const char *name = get_section_name(elf, &elf->sh_table[i]);
		if (strcmp(name, section_name) == 0) {
			return &elf->sh_table[i];
		}
	}
	return NULL;
}

int read_symbol_table(ELF64_file *elf) {
	// Find symbol table section
	ELF64_section *symtab_header = find_section(elf, ".symtab");
	if (!symtab_header) {
		fprintf(stderr, "Could not find symbol table\n");
		return -1;
	}

	// Find string table for symbol names
	ELF64_section *strtab_header = find_section(elf, ".strtab");
	if (!strtab_header) {
		fprintf(stderr, "Could not find string table\n");
		return -1;
	}

	// Read strin table for symbol names
	char *strtab = malloc(strtab_header->sh_size);
	if (!strtab) {
		fprintf(stderr, "Could not allocate string table\n");
		return -1;
	}

	// Seek to string table and read 
	if (fseek(elf->file, strtab_header->sh_offset, SEEK_SET) != 0 ||
			fread(strtab, strtab_header->sh_size, 1, elf->file)  != 1) {
		fprintf(stderr, "Failed to read string table\n");
		free(strtab);
		return -1;
	}

	// Number of symbols
	int num_symbols = symtab_header->sh_size / sizeof(ELF64_sym);

	ELF64_sym sym;
	printf("\nSymbol Table '.symtab' contains %d entries:\n", num_symbols);
	printf("   Num:    Value          Size Type    Name\n");

	// Seek to symbol table
	if (fseek(elf->file, symtab_header->sh_offset, SEEK_SET) != 0) {
		fprintf(stderr, "Failed to seek to symbol table\n");
		free(strtab);
		return -1;
	}

	// Read and print symbols
	for (int i = 0; i < num_symbols; i++) {
		if (fread(&sym, sizeof(ELF64_sym), 1, elf->file) != 1) {
			fprintf(stderr, "Failed to read symbols\n");
			free(strtab);
			return -1;
		}

		// Print symbols with names only
		if (sym.st_name != 0) {
			const char *name = strtab + sym.st_name;
			printf("[%4d] 0x%-12lx %5ld %s\n",
                   i,
                   sym.st_value,
                   sym.st_size,
                   name);
		}
	}

	free(strtab);
	return 0;
}

void init_elf(ELF64_file *elf) {
	elf->file = NULL;
	elf->elf = NULL;
	elf->sh_table = NULL;
	elf->sh_strtab = NULL;
}

void clean_elf(ELF64_file *elf) {
	if (elf->file) { fclose(elf->file); }
	free(elf->elf);
	free(elf->sh_table);
	free(elf->sh_strtab);
}

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <elf_64_bit>\n", argv[0]);
		return 1;
	}

	// Init elf file
	ELF64_file elf;
	init_elf(&elf);

	elf.file = fopen(argv[1], "rb");
	if (!elf.file) {
		fprintf(stderr, "Error opening file");
		return 1;
	}

	elf.elf = malloc(sizeof(ELF64_header));
	if (!elf.elf) {
		fprintf(stderr, "Could not allocate ELF header");
		clean_elf(&elf);
		return 1;
	}

	if (fread(elf.elf, sizeof(ELF64_header), 1, elf.file) != 1) {
		fprintf(stderr, "Failed to read ELF header\n");
		clean_elf(&elf);
		return 1;
	}

	printf("ELF file analysis for: %s\n", argv[1]);
	printf("Number of section headers: %d\n", elf.elf->e_shnum);
	printf("Section header string table index %d\n", elf.elf->e_shstrndx);

	// Read all section headers
	if (read_sh_table(&elf) < 0) {
		clean_elf(&elf);
		return 1;
	}

	// Read symbol table
	if (read_symbol_table(&elf) < 0) {
		clean_elf(&elf);
		return 1;
	}

	// Clean
	clean_elf(&elf);
	return 0;
}
