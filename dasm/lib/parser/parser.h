#ifndef dasm_parser_h
#define dasm_parser_h


#include <ae2f/c90/StdBool.h>
#include <dasm/err.h>
#include <libdice/abi.h>
#include <libdice/type.h>
#include "../lexer/toks.h"

struct dasm_parser {
	struct dasm_tok_stream *m_tok_stream;
	
};

#endif /* dasm_parser_h */