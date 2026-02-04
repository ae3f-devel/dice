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
		/** eq 1, 0, 0 */
		LIBDICE_OPCODE_EQ, 1, 0, 0, 0, 0,
		/** puti *1 ; expected 1 */
		LIBDICE_OPCODE_PUTI, 1, 1,
		/** putc '\n' */
		LIBDICE_OPCODE_PUTC, 0, '\n',
		/** set 1, 3 */
		LIBDICE_OPCODE_SET, 1, 3,
		/** set 2, 3 */
		LIBDICE_OPCODE_SET, 2, 3,

		/** bxor 3, *1, *2 */
		LIBDICE_OPCODE_BXOR, 3, 1, 1, 1, 2,
		/** puti *3 */
		LIBDICE_OPCODE_PUTI, 1, 3,

		/** band 3, *1, *2 */
		LIBDICE_OPCODE_BAND, 3, 1, 1, 1, 2,
		/** puti *3 */
		LIBDICE_OPCODE_PUTI, 1, 3,

		/** putc '\n' */
		LIBDICE_OPCODE_PUTC, 0, '\n',
		LIBDICE_OPCODE_JMPZA, 0, 0, 0, 6,

		/** now this is skipped */
		LIBDICE_OPCODE_EOP,
		/** ============================= */

		/** set 0, ???; "hi" as uint32 */
		LIBDICE_OPCODE_SET, 0, *(const libdice_word_t*)(const void*)"hi\0\0",
		/** puts 0 */
		LIBDICE_OPCODE_PUTS, 0, 0,
		/* dictionary test: set RAM[0]=42; define entry for key 0 with value 999, write entry id to RAM[1] */
		LIBDICE_OPCODE_SET, 0, 42,
		LIBDICE_OPCODE_DEF, 1, 0, 0, 999,
		/* puti *1 => should print entry id (0) */
		LIBDICE_OPCODE_PUTI, 1, 1,
		/* undef the key, write result to RAM[2] */
		LIBDICE_OPCODE_UNDEF, 2, 0, 0,
		/* puti *2 => should print 1 (success) */
		LIBDICE_OPCODE_PUTI, 1, 2,
		LIBDICE_OPCODE_EOP,
	};
	libdice_put_interface	INTERFACE;

	libdice_word_t	RAM[100];
	libdice_word_t	LOOKUP[100];


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
			);

	printf("Final State: %u\n", CONTEXT.m_state);
	printf("Programme counter: %u\n", CONTEXT.m_pc);

	return (int)CONTEXT.m_state;
}
