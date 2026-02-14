
#include <dasm.h>
#include <stdio.h>
#include <libdice.h>

#define  PROGRAMME0 	"label0: nop		\n"	\
			"iadd 111 *111 #1	\n"	\
			"igt 112 *111 #8	\n"	\
			"puti *111 		\n"	\
			"jmpz *112 #label0	\n"	\
			"eop"


			
static E_LIBDICE_CTX_t	__putc(
		int a, 
		struct a_libdice_data* ae2f_restrict _) 
{
	(void)_;
	putc(a, stdout);

	return LIBDICE_CTX_GOOD;
}

static E_LIBDICE_CTX_t	__puti(
		i32_least a, 
		struct a_libdice_data* ae2f_restrict _) 
{
	(void)_;
	printf("%d", a);
	return LIBDICE_CTX_GOOD;
}

static E_LIBDICE_CTX_t	__putf(
		float a, 
		struct a_libdice_data* ae2f_restrict _) 
{
	(void)_;
	printf("%f", a);
	return LIBDICE_CTX_GOOD;
}

static E_LIBDICE_CTX_t	__putu(
		libdice_word_t a, 
		struct a_libdice_data* ae2f_restrict _) 
{
	(void)_;
	printf("%u", a);
	return LIBDICE_CTX_GOOD;
}

static E_LIBDICE_CTX_t	__puts(
		const libdice_word_t* ae2f_restrict a, 
		struct a_libdice_data* ae2f_restrict _)
{
	(void)_;
	puts((const char*)a);
	return LIBDICE_CTX_GOOD;
}

int main(void)
{	
	libdice_ctx	CONTEXT;
	
	libdice_word_t exe[DASM_PROGRAMME_MAX_LEN] = {0,};
	libdice_word_t exe_cnt = 0;
	volatile libdice_word_t i = 0;
	char asm_programme[DASM_PROGRAMME_MAX_LEN] = PROGRAMME0;

	exe_cnt = dasm_assemble(exe, DASM_PROGRAMME_MAX_LEN, asm_programme, DASM_PROGRAMME_MAX_LEN);
	
	if (exe_cnt == DASM_ERR_RET) {
		printf("Syntax error\n");
		return 0;
	}
	for (i=0; i<exe_cnt; ++i) {
		printf("%u, ", exe[i]);
	}

	printf("exe_cnt = %u\n", exe_cnt);


	printf("\n");

	libdice_put_interface	INTERFACE;

	libdice_word_t	RAM[200];
	libdice_word_t	LOOKUP[100];
	c89atomic_uint32	LCK;


	CONTEXT.m_lookup_used	= 0;
	CONTEXT.m_pc		= 0;
	CONTEXT.m_state		= LIBDICE_CTX_GOOD;

	INTERFACE.m_data = 0;
	INTERFACE.m_pfn_putc = __putc;
	INTERFACE.m_pfn_putf = __putf;
	INTERFACE.m_pfn_putu = __putu;
	INTERFACE.m_pfn_puti = __puti;
	INTERFACE.m_pfn_puts = __puts;

	CONTEXT = libdice_run(
			CONTEXT
			, &INTERFACE
			, exe
			, sizeof(exe) / sizeof(exe[0])
			, RAM, sizeof(RAM) / sizeof(RAM[0])
			, LOOKUP, sizeof(LOOKUP) / sizeof(LOOKUP[0])
			, &LCK);

	printf("\n ctx.m_pc = %u, ctx.m_state = %u\n", CONTEXT.m_pc, CONTEXT.m_state);

	return 0; 
} 	
