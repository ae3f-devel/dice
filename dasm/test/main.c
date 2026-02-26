#include <string.h>
#include <stdio.h>
#include "../lib/pp/pp.h"
#include "../lib/lexer/lexer.h"
#include "libdice/type.h"

#include <assert.h>

#define PROGRAMME	"iadd 1000 10, 10 	   \r\n"	\
			"     \"	dsdsf 	 \"   \n"	\
			"      		\'aa\'\'   		\n"		


int main(void)
{
	libdice_word_t i;

	struct dasm_pp pp;
	struct dasm_lexer lexer;
	struct dasm_tok *prev_tok = NULL;
	struct dasm_tok *tok = NULL;

	const char src[100] = PROGRAMME; 
	char dst[100] = {0,};
	struct dasm_tok toks[100] = {0,};
	
	dasm_pp_init(&pp, dst, 3, src, strlen(src)+1);
	printf("err = %u\n", dasm_pp_execute(&pp));
	dasm_pp_reset_dst(&pp, NULL, 100);
	printf("err = %u\n", dasm_pp_execute(&pp));
	
	printf("-----------------\n");
	fwrite(dst, 1, pp.m_dst_cnt, stdout);
	printf("------------------\n");

	dasm_lexer_init(&lexer, toks, 100, pp.m_dst, pp.m_dst_cnt);
	printf("err = %u\n", dasm_lexer_execute(&lexer));

	printf("tok_cnt = %u\n", lexer.m_tok_stream.m_tok_cnt);
	
	dasm_tok_stream_seek(&lexer.m_tok_stream, 0, DASM_TOK_STREAM_WHENCE_SET);

	while ((tok = dasm_tok_stream_peek(&lexer.m_tok_stream))) {
		if (prev_tok == tok) {
			break;
		}
		prev_tok = tok;
		
		for (i=0; i<tok->m_lexeme_len; ++i) {
			printf("%c", tok->m_lexeme[i]);
		}
		
		dasm_tok_stream_advance(&lexer.m_tok_stream);
		printf("\t\t\t\ttok_type = %u\n", tok->m_tok_type);
	}

	printf("------------------\n");

	dasm_pp_deinit(&pp);
	dasm_lexer_deinit(&lexer);

	return 0;
}