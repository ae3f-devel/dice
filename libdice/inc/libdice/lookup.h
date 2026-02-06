#ifndef libdice_lookup_h
#define libdice_lookup_h

#include "./.lookup.h"

/**
 * @brief format of lookup
 * | key_length | key | value |
 * key_length are variable.
 * key is terminated by nul-character
 * key must be aligned 4-byte (Add padding if key_length is not aligned)
 */

#define LIBDICE_LOOKUP_SECTION_BYTE_LEN \
	(LIBDICE_LOOKUP_SECTION_WORD_LEN << 2)
#define LIBDICE_LOOKUP_METADATA_BYTE_LEN \
	(LIBDICE_LOOKUP_METADATA_WORD_LEN << 2)
#define LIBDICE_LOOKUP_KEY_MAX_BYTE_LEN \
	(LIBDICE_LOOKUP_KEY_MAX_WORD_LEN << 2)

#define LIBDICE_LOOKUP_METADATA_WORD_LEN	1
#define LIBDICE_LOOKUP_KEY_MAX_WORD_LEN		\
	((LIBDICE_LOOKUP_SECTION_WORD_LEN) - LIBDICE_LOOKUP_METADATA_WORD_LEN)

typedef	char SANCHECK_LOOKUP_SECTION_WRDLEN[LIBDICE_LOOKUP_KEY_MAX_WORD_LEN > 0 ? 1 : -1];

#endif
