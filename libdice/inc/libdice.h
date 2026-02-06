#ifndef libdice_h
#define libdice_h

#include <ae2f/Keys.h>
#include <ae2f/c90/StdInt.h>
#include <ae2f/Keys.h>

#include "./libdice/type.h"
#include "./libdice/abi.h"

enum LIBDICE_CTX_
{
	/** @brief end of programme */
	LIBDICE_CTX_EOP,
	LIBDICE_CTX_GOOD,
	LIBDICE_CTX_PC_AFTER_PROGRAMME,
	LIBDICE_CTX_LOOKUP_LEAK,
	/** @brief dereference is invalid */
	LIBDICE_CTX_DEREFINVAL,
	/** @brief operation is invalid */
	LIBDICE_CTX_OPINVAL,
	LIBDICE_CTX_STRINVAL
};

typedef struct
{
	E_LIBDICE_CTX_t m_state;

	/** @brief programme counter */
	libdice_word_t m_pc;
	libdice_word_t m_lookup_used;
} libdice_ctx;

ae2f_extern DICECALL libdice_ctx libdice_run_one(
    libdice_ctx c_ctx,
    const libdice_put_interface *ae2f_restrict rd_interface_put,
    const libdice_word_t *ae2f_restrict const rd_programme,
    const libdice_word_t c_num_programme,
    libdice_word_t *ae2f_restrict const rdwr_ram,
    const libdice_word_t c_num_ram,
    libdice_word_t *ae2f_restrict const rdwr_lookup,
    const libdice_word_t c_num_lookup);

ae2f_extern DICECALL libdice_ctx libdice_run(
    libdice_ctx c_ctx,
    const libdice_put_interface *ae2f_restrict rd_interface_put,
    const libdice_word_t *ae2f_restrict const rd_programme,
    const libdice_word_t c_num_programme,
    libdice_word_t *ae2f_restrict const rdwr_ram,
    const libdice_word_t c_num_ram,
    libdice_word_t *ae2f_restrict const rdwr_lookup,
    const libdice_word_t c_num_lookup);

#endif
