#include <stdio.h>
#include <stdlib.h>

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
