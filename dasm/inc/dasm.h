#ifndef	dasm_h
#define	dasm_h

#include <ae2f/Keys.h>
#include <ae2f/Guide.h>

#include <libdice/type.h>
#include <libdice/abi.h>

#define LIBDASM_PROGRAMME_MAX_LEN 2000
#define LIBDASM_ERR_RET ((libdice_word_t)(-1))


ae2f_extern DICECALL libdice_word_t libdasm_assemble(
		ae2f_LP(c_num_ret) libdice_word_t* ae2f_restrict	rdwr_ret_buf,
		const libdice_word_t					c_ret_buf_len,
		ae2f_LP(str_len) const char* ae2f_restrict		rd_programme,
		const libdice_word_t					c_programme_len
		);




#endif
