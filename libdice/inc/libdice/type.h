#ifndef libdice_type_h
#define libdice_type_h

#include <ae2f/c90/StdInt.h>

typedef u32_least libdice_word_t;

typedef union
{
	libdice_word_t m_word;
	i32_least m_i32;
	u32_least m_u32;

	float m_f32;
} libdice_type_literal;

#include <ae2f/cc.h>

/** @see enum LIBDICE_CTX_ */
typedef libdice_word_t E_LIBDICE_CTX_t;

typedef struct a_libdice_data *ae2f_restrict h_libdice_data;

typedef E_LIBDICE_CTX_t fn_libdice_puts_t(
    const libdice_word_t *ae2f_restrict rd_words,
    h_libdice_data h_data);

typedef E_LIBDICE_CTX_t fn_libdice_putc_t(
    int c_char,
    h_libdice_data h_data);

typedef E_LIBDICE_CTX_t fn_libdice_puti_t(
    i32_least c_i32,
    h_libdice_data h_data);

typedef E_LIBDICE_CTX_t fn_libdice_putu_t(
    libdice_word_t c_u32,
    h_libdice_data h_data);

typedef E_LIBDICE_CTX_t fn_libdice_putf_t(
    float c_f32,
    h_libdice_data h_data);

typedef struct
{
	h_libdice_data m_data;
	fn_libdice_putc_t *m_pfn_putc;
	fn_libdice_puts_t *m_pfn_puts;
	fn_libdice_putf_t *m_pfn_putf;
	fn_libdice_puti_t *m_pfn_puti;
	fn_libdice_putu_t *m_pfn_putu;
} libdice_put_interface;

#endif
