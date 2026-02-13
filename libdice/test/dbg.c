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
	libdice_word_t	PROGRAMME[] = {
		0, 
		1, 111, 1, 111, 0, 1, 
		39, 112, 1, 111, 0, 8, 
		44, 1, 111, 
		18, 1, 112, 0, 0,
		LIBDICE_OPCODE_EOP 
	};
	libdice_put_interface	INTERFACE;

	libdice_word_t	RAM[120];
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
