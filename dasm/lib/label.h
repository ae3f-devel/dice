#ifndef dasm_label_h
#define dasm_label_h

#include <libdice/type.h>

#define DASM_LABEL_MAX_LEN 64

struct dasm_label {
	char m_text[DASM_LABEL_MAX_LEN];
	libdice_word_t m_address;
};

struct dasm_label_table {
	struct dasm_label *m_labels;
	libdice_word_t m_labels_len;
	libdice_word_t m_label_cnt;
};



#endif /* dasm_label_h */