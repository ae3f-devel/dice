#include <string.h>
#include <stdio.h>
#include "../lib/pp/pp.h"
#include "../lib/lexer/lexer.h"
#include "libdice/type.h"
#include <stdlib.h>

#include <assert.h>

#define PROGRAMME	"iadd 1000 10, 10 	   \r\n"	\
			"  label0:   \"	dsdsf 	 \"   \n"	\
			"      	\"\"	puti \'a\' label0		\n"		


int main(void)
{
	enum DASM_ERR_ err;
	struct dasm_pp pp;
	char dst[100] = {0,};
	const char src[100] = PROGRAMME;

	struct dasm_tok_stream tstream;
	struct dasm_tok toks[100];

	struct dasm_lexer lexer;

	if (!dasm_pp_init(&pp, dst, 100, src, (libdice_word_t)strlen(src)+1)) {
		printf("[ERROR0]\n");
		exit(1);
	}

	if ((err = dasm_pp_execute(&pp)) != DASM_ERR_OK) {
		printf("[ERROR1] DASM_ERR_ = %u\n", err);
		exit(1);
	}

	if (!dasm_tok_stream_init(&tstream, toks, 100)) {
		printf("[ERROR2]\n");
		exit(1);
	}

	if (!dasm_lexer_init(&lexer, &tstream, pp.m_dst, pp.m_dst_cnt)) {
		printf("[ERROR3]\n");
		exit(1);
	}

	if ((err = dasm_lexer_execute(&lexer)) != DASM_ERR_OK) {
		printf("[ERROR4] DASM_ERR_ = %u\n",  err);
		exit(1);
	}

	if (!dasm_tok_stream_seek(lexer.m_tok_stream, 0, DASM_TOK_STREAM_WHENCE_SET)) {
		printf("[ERROR5]\n");
		exit(1);
	}

	do {
		struct dasm_tok *tok;
		libdice_word_t i;

		tok = dasm_tok_stream_peek(lexer.m_tok_stream);
		if (!tok) {
			printf("[ERROR6]\n");
			break;
		}

		for (i=0; i<tok->m_lexeme_len; ++i) {
			printf("%c", tok->m_lexeme[i]);
		}
		printf("\t tok_type = %u\n", tok->m_tok_type);

	} while (dasm_tok_stream_advance(lexer.m_tok_stream));

	dasm_pp_deinit(&pp);
	dasm_lexer_deinit(&lexer);
	dasm_tok_stream_deinit(&tstream);

	return 0;
}