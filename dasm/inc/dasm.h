#ifndef	dasm_h
#define	dasm_h

#include <ae2f/Keys.h>
#include <ae2f/Guide.h>

#include <libdice/type.h>
#include <libdice/abi.h>

enum DASM_ {
	DASM_GOOD,
	DASM_RET_INSUFFICIENT
};

/** 
 * @fn dasm
 * compile one assembly as `rd_str` and store to `ret`
 * @returns the word count which is actually required to store the output.
 *
 * @param r_num_ret_opt
 * 	is `ret_opt`'s required word count.
 *
 * @param c_num_str
 * 	is `rd_str`'s available count as byte
 *
 * @param c_num_ret
 * 	is `ret_opt`'s available count as word
 * */
ae2f_extern DICECALL libdice_word_t dasm(
		ae2f_LP(c_num_ret) libdice_word_t* ae2f_restrict	ret_opt,
		const libdice_word_t					c_num_ret,
		ae2f_LP(c_num_str) const char* ae2f_restrict		rd_str,
		const libdice_word_t					c_num_str
		);

#endif
