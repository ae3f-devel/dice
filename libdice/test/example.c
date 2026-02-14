#include <libdice.h>
#include <libdice/opcode.h>
#include <stdio.h>

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

int main(void) {
	libdice_ctx	CONTEXT;
	libdice_word_t PROGRAMME[] = {
		/* NOP */
		LIBDICE_OPCODE_NOP,

		/* Unary operations */
		LIBDICE_OPCODE_SET, 0, 5,
		LIBDICE_OPCODE_BNOT, 1, 1, 0,
		LIBDICE_OPCODE_PUTI, 1, 1,
		LIBDICE_OPCODE_PUTC, 0, '\n',

		LIBDICE_OPCODE_LNOT, 2, 1, 0,
		LIBDICE_OPCODE_PUTI, 1, 2,
		LIBDICE_OPCODE_PUTC, 0, '\n',

		LIBDICE_OPCODE_INEG, 3, 1, 0,
		LIBDICE_OPCODE_PUTI, 1, 3,
		LIBDICE_OPCODE_PUTC, 0, '\n',

		LIBDICE_OPCODE_ITOF, 4, 1, 0,
		LIBDICE_OPCODE_PUTF, 1, 4,
		LIBDICE_OPCODE_PUTC, 0, '\n',

		LIBDICE_OPCODE_FTOI, 5, 1, 4,
		LIBDICE_OPCODE_PUTI, 1, 5,
		LIBDICE_OPCODE_PUTC, 0, '\n',

		LIBDICE_OPCODE_FNEG, 6, 1, 4,
		LIBDICE_OPCODE_PUTF, 1, 6,
		LIBDICE_OPCODE_PUTC, 0, '\n',

		/* Binary operations */
		LIBDICE_OPCODE_SET, 1, 10,
		LIBDICE_OPCODE_EQ, 7, 1, 0, 1, 1,
		LIBDICE_OPCODE_PUTI, 1, 7,
		LIBDICE_OPCODE_PUTC, 0, '\n',

		LIBDICE_OPCODE_NEQ, 8, 1, 0, 1, 1,
		LIBDICE_OPCODE_PUTI, 1, 8,
		LIBDICE_OPCODE_PUTC, 0, '\n',

		LIBDICE_OPCODE_BAND, 9, 1, 0, 1, 1,
		LIBDICE_OPCODE_PUTI, 1, 9,
		LIBDICE_OPCODE_PUTC, 0, '\n',

		LIBDICE_OPCODE_BOR, 10, 1, 0, 1, 1,
		LIBDICE_OPCODE_PUTI, 1, 10,
		LIBDICE_OPCODE_PUTC, 0, '\n',

		LIBDICE_OPCODE_FADD, 11, 1, 4, 1, 4,
		LIBDICE_OPCODE_PUTF, 1, 11,
		LIBDICE_OPCODE_PUTC, 0, '\n',

		LIBDICE_OPCODE_FSUB, 12, 1, 4, 1, 4,
		LIBDICE_OPCODE_PUTF, 1, 12,
		LIBDICE_OPCODE_PUTC, 0, '\n',

		LIBDICE_OPCODE_FMUL, 13, 1, 4, 1, 4,
		LIBDICE_OPCODE_PUTF, 1, 13,
		LIBDICE_OPCODE_PUTC, 0, '\n',

		LIBDICE_OPCODE_FDIV, 14, 1, 4, 1, 4,
		LIBDICE_OPCODE_PUTF, 1, 14,
		LIBDICE_OPCODE_PUTC, 0, '\n',

		/* Print "Hello world" */
		LIBDICE_OPCODE_PUTC, 0, 'H',
		LIBDICE_OPCODE_PUTC, 0, 'e',
		LIBDICE_OPCODE_PUTC, 0, 'l',
		LIBDICE_OPCODE_PUTC, 0, 'l',
		LIBDICE_OPCODE_PUTC, 0, 'o',
		LIBDICE_OPCODE_PUTC, 0, ' ',
		LIBDICE_OPCODE_PUTC, 0, 'w',
		LIBDICE_OPCODE_PUTC, 0, 'o',
		LIBDICE_OPCODE_PUTC, 0, 'r',
		LIBDICE_OPCODE_PUTC, 0, 'l',
		LIBDICE_OPCODE_PUTC, 0, 'd',
		LIBDICE_OPCODE_PUTC, 0, '\n',

		/* DEF and UNDEF operation */
		LIBDICE_OPCODE_DEF, 0, 0,
		LIBDICE_OPCODE_UNDEF, 0, 0,

		/* TIME and RAND operation */
		LIBDICE_OPCODE_TIME, 16,
		LIBDICE_OPCODE_RAND, 0, 16,
		LIBDICE_OPCODE_PUTI, 1, 16,
		LIBDICE_OPCODE_PUTC, 0, '\n',

		/* End of program */
		LIBDICE_OPCODE_EOP,
	};
	libdice_put_interface	INTERFACE;

	libdice_word_t	RAM[100];
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
			, PROGRAMME
			, sizeof(PROGRAMME) / sizeof(PROGRAMME[0])
			, RAM, sizeof(RAM) / sizeof(RAM[0])
			, LOOKUP, sizeof(LOOKUP) / sizeof(LOOKUP[0])
			, &LCK);

	printf("Final State: %u\n", CONTEXT.m_state);
	printf("Programme counter: %u\n", CONTEXT.m_pc);

	return (int)CONTEXT.m_state;
}
