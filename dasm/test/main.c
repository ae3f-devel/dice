
#include <dasm.h>
#include <stdio.h>

int main(void)
{	
	libdice_word_t exe[LIBDASM_PROGRAMME_MAX_LEN] = {0,};
	libdice_word_t exe_cnt = 0;
	libdice_word_t i = 0;

	char asm_programme[LIBDASM_PROGRAMME_MAX_LEN] = "	buf: nop\n	iadd buf *1111 *61111\n	puti *buf\n eop	";
	exe_cnt = libdasm_assemble(exe, LIBDASM_PROGRAMME_MAX_LEN, asm_programme, LIBDASM_PROGRAMME_MAX_LEN);
	
	for (i=0; i<exe_cnt; ++i) {
		printf("%u\n", exe[i]);
	}
	return 0; 
} 