#include <libdice.h>
#include <libdice/opcode.h>
#include <../inc/libdice/lookup.h>
#include <assert.h>
#include <stdlib.h>
#include "../inc/libdice/type.h"

typedef struct
{
	libdice_word_t m_r0;
	libdice_ctx m_ctx;
} __result;

#define __strcount32_0(c_str) (!!(0x000000FF & c_str))
#define __strcount32_1(c_str) ((!!(0x0000FF00 & c_str)) & __strcount32_0(c_str))
#define __strcount32_2(c_str) ((!!(0x00FF0000 & c_str)) & __strcount32_1(c_str))
#define __strcount32_3(c_str) ((!!(0xFF000000 & c_str)) & __strcount32_2(c_str))
#define __strcount32(c_str) ((libdice_word_t)(__strcount32_0(c_str) + __strcount32_1(c_str) + __strcount32_2(c_str) + __strcount32_3(c_str)))

static ae2f_inline ae2f_ccpure libdice_word_t __strcount(
    const libdice_word_t *ae2f_restrict rd_mem,
    const libdice_word_t c_num_mem,
    const libdice_word_t c_pad_str)
{
	libdice_word_t COUNT = c_pad_str;
	while (
	    (COUNT < c_num_mem || ae2f_unexpected((COUNT = 0xFFFFFFFF) & 0)) && __strcount32(rd_mem[COUNT]) & 4 && ae2f_expected(1 | ++COUNT))
	{
	}

	return COUNT == 0xFFFFFFFF
		   ? COUNT
		   : (((COUNT - c_pad_str) << 2) + __strcount32(rd_mem[COUNT]));
}

static ae2f_inline ae2f_ccpure libdice_word_t __strequal2(
    const libdice_word_t *ae2f_restrict rd_mem,
    const libdice_word_t c_num_mem,
    const libdice_word_t *ae2f_restrict rd_mem2,
    const libdice_word_t c_num_mem2,
    libdice_word_t c_str,
    libdice_word_t c_str2)
{
	while (
	    ((c_str < c_num_mem) & (c_str2 < c_num_mem2)) && c_str[rd_mem] == c_str2[rd_mem2] && ((__strcount32(c_str[rd_mem]) & 4) && ae2f_expected(++c_str | ++c_str2 | 1)))
	{
	}

	ae2f_expected_but_else(c_str < c_num_mem) return 0xFFFFFFFF;

	ae2f_expected_but_else(c_str2 < c_num_mem2) return 0xFFFFFFFF;

	return (c_str2[rd_mem2] == c_str[rd_mem]) & (__strcount32(c_str[rd_mem]) < 4);
}

static ae2f_inline ae2f_ccpure libdice_word_t __strequal(
    const libdice_word_t *ae2f_restrict rd_mem,
    const libdice_word_t c_num_mem,
    libdice_word_t c_str_0,
    libdice_word_t c_str_1)
{
	while (
	    ((c_str_0 < c_num_mem) & (c_str_1 < c_num_mem)) && c_str_0[rd_mem] == c_str_1[rd_mem] && ((__strcount32(c_str_0[rd_mem]) & 4) && ae2f_expected(++c_str_0 | ++c_str_1 | 1)))
	{
	}

	ae2f_expected_but_else(c_str_0 < c_num_mem) return 0xFFFFFFFF;

	ae2f_expected_but_else(c_str_1 < c_num_mem) return 0xFFFFFFFF;

	return (c_str_0[rd_mem] == c_str_1[rd_mem]) & (__strcount32(c_str_0[rd_mem]) < 4);
}

static ae2f_inline ae2f_ccconst __result __one_const(
    libdice_ctx c_ctx,
    const libdice_word_t c_opcode,
    const libdice_word_t c_operand)
{
	union
	{
		float m_f32;
		int_least32_t m_i32;
		libdice_word_t m_u32;
	} CVRT;

	__result RET;

	RET.m_ctx = c_ctx;
	RET.m_ctx.m_pc += 4;
	RET.m_r0 = 0;

	switch (c_opcode)
	{
	case LIBDICE_OPCODE_BNOT:
		RET.m_r0 = ~c_operand;
		return RET;

	case LIBDICE_OPCODE_LNOT:
		RET.m_r0 = !c_operand;
		return RET;

	case LIBDICE_OPCODE_TOBIT:
		RET.m_r0 = !!c_operand;
		return RET;

	case LIBDICE_OPCODE_INEG:
		RET.m_r0 = -c_operand;
		return RET;

	case LIBDICE_OPCODE_JMP:
		RET.m_ctx.m_pc = c_operand;
		return RET;

	case LIBDICE_OPCODE_JMPA:
		RET.m_ctx.m_pc += c_operand - 4;
		return RET;

	case LIBDICE_OPCODE_JMPN:
		RET.m_ctx.m_pc -= c_operand + 4;
		return RET;

	case LIBDICE_OPCODE_ITOF:
		CVRT.m_f32 = (float)(int_least32_t)c_operand;
		RET.m_r0 = CVRT.m_u32;
		return RET;
	case LIBDICE_OPCODE_FTOI:
		CVRT.m_u32 = c_operand;
		RET.m_r0 = (libdice_word_t)(int_least32_t)CVRT.m_f32;
		return RET;
	case LIBDICE_OPCODE_FNEG:
		CVRT.m_u32 = c_operand;
		CVRT.m_f32 = -CVRT.m_f32;
		RET.m_r0 = CVRT.m_u32;
		return RET;

	default:
		assert(0);
		ae2f_unreachable();
	}
}

/* TODO : Can't understand variable naming. c_operand, c_op1*/
static ae2f_inline ae2f_ccconst __result __two_const(
    libdice_ctx c_ctx,
    const libdice_word_t c_opcode,
    const libdice_word_t c_operand,
    const libdice_word_t c_op1)
{

	__result RET;

	RET.m_ctx = c_ctx;
	/** opcode dst nref val nref val */
	RET.m_ctx.m_pc += 6;
	RET.m_r0 = 0;

	union
	{
		float m_f32;
		int_least32_t m_i32;
		libdice_word_t m_u32;
	} VAL0, VAL1;

	switch (c_opcode)
	{
	case LIBDICE_OPCODE_FADD:
	case LIBDICE_OPCODE_FSUB:
	case LIBDICE_OPCODE_FMUL:
	case LIBDICE_OPCODE_FDIV:
	case LIBDICE_OPCODE_FLT:
	case LIBDICE_OPCODE_FGT:
		VAL0.m_u32 = c_operand;
		VAL1.m_u32 = c_op1;
		ae2f_fallthrough;
	default:
		break;
	}

	VAL0.m_u32 = c_operand;
	VAL1.m_u32 = c_op1;

	switch (c_opcode)
	{
	case LIBDICE_OPCODE_EQ:
		RET.m_r0 = c_operand == c_op1;
		return RET;

	case LIBDICE_OPCODE_NEQ:
		RET.m_r0 = c_operand != c_op1;
		return RET;

	case LIBDICE_OPCODE_BAND:
		RET.m_r0 = c_operand & c_op1;
		return RET;

	case LIBDICE_OPCODE_BOR:
		RET.m_r0 = c_operand | c_op1;
		return RET;

	case LIBDICE_OPCODE_BXOR:
		RET.m_r0 = c_operand ^ c_op1;
		return RET;

	case LIBDICE_OPCODE_BLSHIFT:
		RET.m_r0 = c_operand << c_op1;
		return RET;

	case LIBDICE_OPCODE_BRSHIFT:
		RET.m_r0 = (uint_least32_t)((int_least32_t)c_operand >> c_op1);
		return RET;

	case LIBDICE_OPCODE_LRSHIFT:
		RET.m_r0 = (c_operand >> c_op1);
		return RET;

	case LIBDICE_OPCODE_LAND:
		RET.m_r0 = c_operand && c_op1;
		return RET;

	case LIBDICE_OPCODE_LOR:
		RET.m_r0 = c_operand || c_op1;
		return RET;

#define __operate_signed(L_oper, a, b) \
	((libdice_word_t)((i32_least)(a)L_oper(i32_least)(b)))

#define __operate_usigned(L_oper, a, b) \
	((libdice_word_t)((a)L_oper(b)))
	case LIBDICE_OPCODE_IADD:
		RET.m_r0 = c_operand + c_op1;
		return RET;

	case LIBDICE_OPCODE_ISUB:
		RET.m_r0 = c_operand - c_op1;
		return RET;

	case LIBDICE_OPCODE_IMUL:
		RET.m_r0 = __operate_signed(*, c_operand, c_op1);
		return RET;

	case LIBDICE_OPCODE_IDIV:
		RET.m_r0 = __operate_signed(/, c_operand, c_op1);
		return RET;

	case LIBDICE_OPCODE_IREM:
		RET.m_r0 = __operate_signed(%, c_operand, c_op1);
		return RET;

	case LIBDICE_OPCODE_UMUL:
		RET.m_r0 = __operate_usigned(*, c_operand, c_op1);
		return RET;

	case LIBDICE_OPCODE_UDIV:
		RET.m_r0 = __operate_usigned(/, c_operand, c_op1);
		return RET;

	case LIBDICE_OPCODE_UREM:
		RET.m_r0 = __operate_usigned(%, c_operand, c_op1);
		return RET;

	case LIBDICE_OPCODE_FADD:
		VAL0.m_f32 += VAL1.m_f32;
		RET.m_r0 = VAL0.m_u32;
		return RET;

	case LIBDICE_OPCODE_FSUB:
		VAL0.m_f32 -= VAL1.m_f32;
		RET.m_r0 = VAL0.m_u32;
		return RET;

	case LIBDICE_OPCODE_FMUL:
		VAL0.m_f32 *= VAL1.m_f32;
		RET.m_r0 = VAL0.m_u32;
		return RET;

	case LIBDICE_OPCODE_FDIV:
		VAL0.m_f32 /= VAL1.m_f32;
		RET.m_r0 = VAL0.m_u32;
		return RET;

	case LIBDICE_OPCODE_JMPZ:
		RET.m_ctx.m_pc = c_operand
				     ? RET.m_ctx.m_pc
				     : c_op1;

		return RET;

	case LIBDICE_OPCODE_JMPZA:
		RET.m_ctx.m_pc += c_operand
				      ? RET.m_ctx.m_pc - 6
				      : 0;

		return RET;

	case LIBDICE_OPCODE_JMPZN:
		RET.m_ctx.m_pc -= c_operand
				      ? RET.m_ctx.m_pc + 6
				      : 0;
		return RET;

	case LIBDICE_OPCODE_FGT:
		RET.m_r0 = VAL0.m_f32 > VAL1.m_f32;
		return RET;

	case LIBDICE_OPCODE_FLT:
		RET.m_r0 = VAL0.m_f32 < VAL1.m_f32;
		return RET;

	case LIBDICE_OPCODE_IGT:
		RET.m_r0 = c_operand > c_op1;
		return RET;
	case LIBDICE_OPCODE_ILT:
		RET.m_r0 = c_operand < c_op1;
		return RET;
	default:
		assert(0);
		ae2f_unreachable();
	}
#undef __operate_signed
#undef __operate_usigned
}

DICEIMPL libdice_ctx libdice_run_one(
    libdice_ctx c_ctx,
    const libdice_put_interface *ae2f_restrict rd_interface_put,
    const libdice_word_t *ae2f_restrict const rd_programme,
    const libdice_word_t c_num_programme,
    libdice_word_t *ae2f_restrict const rdwr_ram,
    const libdice_word_t c_num_ram,
    libdice_word_t *ae2f_restrict const rdwr_lookup,
    const libdice_word_t c_num_lookup)
{
	assert(rd_programme);
	assert(rdwr_ram);
	assert(rdwr_lookup);
	assert(rd_interface_put);
	assert(rd_interface_put->m_pfn_putc);
	assert(rd_interface_put->m_pfn_puts);
	assert(rd_interface_put->m_pfn_putf);
	assert(rd_interface_put->m_pfn_putu);
	assert(rd_interface_put->m_pfn_puti);
	assert(c_num_lookup >= LIBDICE_LOOKUP_SECTION_WORD_LEN); /* TODO : assert가 아닌, if문 사용이 맞는듯*/

	ae2f_unexpected_but_if(c_ctx.m_state != LIBDICE_CTX_GOOD) { return c_ctx; }
	ae2f_expected_but_else(c_ctx.m_pc < c_num_programme)
	{
		c_ctx.m_state = LIBDICE_CTX_PC_AFTER_PROGRAMME;
		return c_ctx;
	}

	ae2f_expected_but_else(c_ctx.m_lookup_used < c_num_lookup)
	{
		c_ctx.m_state = LIBDICE_CTX_LOOKUP_LEAK;
		return c_ctx;
	}

	switch (rd_programme[c_ctx.m_pc])
	{
		__result RESULT;
		libdice_word_t COUNT;
		libdice_word_t O0;
		libdice_word_t O1;

	default:
		break;
#define __deref(O0, c_pad)                                             \
	ae2f_expected_but_else(c_ctx.m_pc + (c_pad) < c_num_programme) \
	{                                                              \
		c_ctx.m_state = LIBDICE_CTX_PC_AFTER_PROGRAMME;        \
		return c_ctx;                                          \
	}                                                              \
	COUNT = rd_programme[c_ctx.m_pc + (c_pad)];                    \
	(O0) = rd_programme[c_ctx.m_pc + (c_pad) + 1];                 \
	while (COUNT-- && (O0) < c_num_ram)                            \
	{                                                              \
		(O0) = (O0)[rdwr_ram];                                 \
	}                                                              \
	ae2f_unexpected_but_if(COUNT + 1)                              \
	{                                                              \
		c_ctx.m_state = LIBDICE_CTX_DEREFINVAL;                \
		return c_ctx;                                          \
	}

	case LIBDICE_OPCODE_SET:
		ae2f_expected_but_else(c_ctx.m_pc + 2 < c_num_programme)
		{
			c_ctx.m_state = LIBDICE_CTX_PC_AFTER_PROGRAMME;
			return c_ctx;
		}
		
		ae2f_expected_but_else(rd_programme[c_ctx.m_pc + 1] < c_num_ram)
		{
			c_ctx.m_state = LIBDICE_CTX_PC_AFTER_PROGRAMME;
			return c_ctx;
		}

		rdwr_ram[rd_programme[c_ctx.m_pc + 1]] = rd_programme[c_ctx.m_pc + 2];
		c_ctx.m_pc += 3;
		return c_ctx;
	
	case LIBDICE_OPCODE_MSET:
	{
		__deref(O0, 1);
		__deref(O1, 3);

		ae2f_expected_but_else(O0 < c_num_ram)
		{
			c_ctx.m_state = LIBDICE_CTX_DEREFINVAL;
			return c_ctx;
		}

		rdwr_ram[O0] =  O1;

		c_ctx.m_pc += 5;
		return c_ctx;
	}
	case LIBDICE_OPCODE_MOV:
	{
		__deref(O0, 1);
		__deref(O1, 3);
		
		ae2f_expected_but_else(O0 < c_num_ram && O1 < c_num_ram)
		{
			c_ctx.m_state = LIBDICE_CTX_DEREFINVAL;
			return c_ctx;
		}

		rdwr_ram[O0] = rdwr_ram[O1];

		c_ctx.m_pc += 5;
		return c_ctx;

	}
	case LIBDICE_OPCODE_NOP:
		c_ctx.m_pc++;
		return c_ctx;
	case LIBDICE_OPCODE_EOP:
		c_ctx.m_state = LIBDICE_CTX_EOP;
		return c_ctx;

	case LIBDICE_OPCODE_INEG:
	case LIBDICE_OPCODE_FNEG:
	case LIBDICE_OPCODE_BNOT:
	case LIBDICE_OPCODE_LNOT:
	case LIBDICE_OPCODE_ITOF:
	case LIBDICE_OPCODE_FTOI:
		__deref(O0, 2);
		RESULT = __one_const(
		    c_ctx, rd_programme[c_ctx.m_pc], O0);

		ae2f_expected_but_else(rd_programme[c_ctx.m_pc + 1] < c_num_ram)
		{
			c_ctx.m_state = LIBDICE_CTX_DEREFINVAL;
			return c_ctx;
		}

		rdwr_ram[c_ctx.m_pc[rd_programme + 1]] = RESULT.m_r0;
		return RESULT.m_ctx;

	case LIBDICE_OPCODE_JMP:
		__deref(O0, 1);
		RESULT = __one_const(
		    c_ctx, rd_programme[c_ctx.m_pc], O0); /* Need boundary check*/

		return RESULT.m_ctx;

	case LIBDICE_OPCODE_FADD:
	case LIBDICE_OPCODE_FDIV:
	case LIBDICE_OPCODE_FMUL:
	case LIBDICE_OPCODE_FSUB:
	case LIBDICE_OPCODE_IADD:
	case LIBDICE_OPCODE_IDIV:
	case LIBDICE_OPCODE_IMUL:
	case LIBDICE_OPCODE_IREM:
	case LIBDICE_OPCODE_UDIV:
	case LIBDICE_OPCODE_UMUL:
	case LIBDICE_OPCODE_UREM:
	case LIBDICE_OPCODE_ISUB:
	case LIBDICE_OPCODE_EQ:
	case LIBDICE_OPCODE_NEQ:
	case LIBDICE_OPCODE_LAND:
	case LIBDICE_OPCODE_LOR:
	case LIBDICE_OPCODE_LRSHIFT:
	case LIBDICE_OPCODE_BLSHIFT:
	case LIBDICE_OPCODE_BRSHIFT:
	case LIBDICE_OPCODE_BAND:
	case LIBDICE_OPCODE_BOR:
	case LIBDICE_OPCODE_BXOR:
	case LIBDICE_OPCODE_IGT:
	case LIBDICE_OPCODE_FGT:
	case LIBDICE_OPCODE_ILT:
	case LIBDICE_OPCODE_FLT:
		__deref(O1, 4);
		__deref(O0, 2);

		RESULT = __two_const(
		    c_ctx, rd_programme[c_ctx.m_pc], O0, O1);

		ae2f_expected_but_else(rd_programme[c_ctx.m_pc + 1] < c_num_ram)
		{
			c_ctx.m_state = LIBDICE_CTX_DEREFINVAL;
			return c_ctx;
		}

		rdwr_ram[c_ctx.m_pc[rd_programme + 1]] = RESULT.m_r0;
		return RESULT.m_ctx;

	case LIBDICE_OPCODE_JMPZ:
	case LIBDICE_OPCODE_JMPZA:
	case LIBDICE_OPCODE_JMPZN:
		__deref(O1, 3);
		__deref(O0, 1);

		RESULT = __two_const(
		    c_ctx, rd_programme[c_ctx.m_pc], O0, O1);

		return RESULT.m_ctx;

	case LIBDICE_OPCODE_PUTC:
		__deref(O0, 1);

		c_ctx.m_state = rd_interface_put->m_pfn_putc(
		    (int)O0, rd_interface_put->m_data);

		c_ctx.m_pc += 3;
		return c_ctx;

	case LIBDICE_OPCODE_PUTI:
		__deref(O0, 1);

		c_ctx.m_state = rd_interface_put->m_pfn_puti(
		    (int)O0, rd_interface_put->m_data);

		c_ctx.m_pc += 3;
		return c_ctx;

	case LIBDICE_OPCODE_PUTS:
		__deref(O0, 1);

		ae2f_unexpected_but_if(0xFFFFFFFF == __strcount(
							 rdwr_ram, c_num_ram, O0))
		{
			c_ctx.m_state = LIBDICE_CTX_STRINVAL;
			return c_ctx;
		}

		c_ctx.m_state = rd_interface_put->m_pfn_puts(
		    rdwr_ram + O0, rd_interface_put->m_data);

		c_ctx.m_pc += 3;
		return c_ctx;

	case LIBDICE_OPCODE_PUTF:
	{
		union
		{
			libdice_word_t m_u;
			float m_f;
		} UF32;
		__deref(UF32.m_u, 1);

		c_ctx.m_state = rd_interface_put->m_pfn_putf(
		    UF32.m_f, rd_interface_put->m_data);

		c_ctx.m_pc += 3;
		return c_ctx;
	}

	case LIBDICE_OPCODE_DEF:
	{
		/*
		 * The key is nul-terminated
		 */
		libdice_word_t key_byte_len = 0;
		libdice_word_t key_word_len = 0;
		libdice_word_t i = 0;
		libdice_word_t tmp_key_byte_len = 0;

		ae2f_unexpected_but_if(c_ctx.m_lookup_used + LIBDICE_LOOKUP_SECTION_WORD_LEN > c_num_lookup)
		{
			c_ctx.m_state = LIBDICE_CTX_LOOKUP_LEAK;
			return c_ctx;
		}

		__deref(O0, 1); /* pointer to key */

		key_byte_len = __strcount(rdwr_ram, c_num_ram, O0);
		if (key_byte_len == 0xFFFFFFFF || key_byte_len > LIBDICE_LOOKUP_KEY_MAX_BYTE_LEN)
		{
			c_ctx.m_state = LIBDICE_CTX_STRINVAL;
			return c_ctx;
		}

		key_word_len = key_byte_len / 4 + !!(key_byte_len % 4);

		/* ram boundary check */
		if (c_num_ram < O0 + key_word_len)
		{
			c_ctx.m_state = LIBDICE_CTX_STRINVAL;
			return c_ctx;
		}

		/* find same key */
		for (i = 0; i < c_ctx.m_lookup_used; i += LIBDICE_LOOKUP_SECTION_WORD_LEN)
		{

			tmp_key_byte_len = rdwr_lookup[i];

			ae2f_expected_if(tmp_key_byte_len != key_byte_len)
			{
				continue;
			}

			ae2f_expected_if(!__strequal2(rdwr_ram, c_num_ram,
						      rdwr_lookup, c_num_lookup,
						      O0, i + LIBDICE_LOOKUP_METADATA_WORD_LEN))
			{
				continue;
			}
		}

		if (i < c_ctx.m_lookup_used)
		{
			/* found same key */
			c_ctx.m_pc += 3;
			return c_ctx;
		}

		rdwr_lookup[c_ctx.m_lookup_used] = key_byte_len;

		/* copy key */
		for (i = 0; i < key_word_len; i++)
		{
			rdwr_lookup[c_ctx.m_lookup_used + i + 1] = rdwr_ram[O0 + i];
		}
		c_ctx.m_lookup_used += LIBDICE_LOOKUP_SECTION_WORD_LEN;

		c_ctx.m_pc += 3;
		return c_ctx;
	}

	case LIBDICE_OPCODE_UNDEF:
	{
		libdice_word_t key_byte_len = 0;
		libdice_word_t i = 0;
		libdice_word_t tmp_key_byte_len = 0;
		libdice_word_t j = 0;

		__deref(O0, 1); /*pointer to key*/

		key_byte_len = __strcount(rdwr_ram, c_num_ram, O0);
		if (key_byte_len == 0xFFFFFFFF || key_byte_len > LIBDICE_LOOKUP_KEY_MAX_BYTE_LEN)
		{
			c_ctx.m_state = LIBDICE_CTX_STRINVAL;
			return c_ctx;
		}

		/* find same key */
		for (i = 0; i < c_ctx.m_lookup_used; i += LIBDICE_LOOKUP_SECTION_WORD_LEN)
		{

			tmp_key_byte_len = rdwr_lookup[i];

			ae2f_expected_if(tmp_key_byte_len != key_byte_len)
			{
				continue;
			}

			ae2f_expected_if(!__strequal2(rdwr_ram, c_num_ram,
						      rdwr_lookup, c_num_lookup,
						      O0, i + LIBDICE_LOOKUP_METADATA_WORD_LEN))
			{
				continue;
			}

			/* found same key */
			break;
		}

		if (i == c_ctx.m_lookup_used)
		{
			/* Couldn't find the same key */
			c_ctx.m_pc += 3;
			return c_ctx;
		}

		for (j = 0; j < LIBDICE_LOOKUP_SECTION_WORD_LEN; j++)
		{
			rdwr_lookup[i + j] = rdwr_lookup[c_ctx.m_lookup_used - LIBDICE_LOOKUP_SECTION_WORD_LEN + j];
		}

		c_ctx.m_lookup_used -= LIBDICE_LOOKUP_SECTION_WORD_LEN;
		c_ctx.m_pc += 3;
		return c_ctx;
	}
	case LIBDICE_OPCODE_SETRANDSEED:
	{
		__deref(O0, 1);

		srand(O0);

		c_ctx.m_pc += 3;
		return c_ctx;
	}
	case LIBDICE_OPCODE_IRAND:
	{

		ae2f_expected_but_else(c_ctx.m_pc + 1 < c_num_programme)
		{
			c_ctx.m_state = LIBDICE_CTX_PC_AFTER_PROGRAMME;
			return c_ctx;
		}

		ae2f_expected_but_else(rd_programme[c_ctx.m_pc + 1] < c_num_ram)
		{
			c_ctx.m_state = LIBDICE_CTX_DEREFINVAL;
			return c_ctx;
		}

		rdwr_ram[rd_programme[c_ctx.m_pc + 1]] = (libdice_word_t)rand();

		c_ctx.m_pc += 2;
		return c_ctx;
	}
	case LIBDICE_OPCODE_FRAND:
	{
		libdice_type_literal tmp;

		ae2f_expected_but_else(c_ctx.m_pc + 1 < c_num_programme)
		{
			c_ctx.m_state = LIBDICE_CTX_PC_AFTER_PROGRAMME;
			return c_ctx;
		}

		ae2f_expected_but_else(rd_programme[c_ctx.m_pc + 1] < c_num_ram)
		{
			c_ctx.m_state = LIBDICE_CTX_DEREFINVAL;
			return c_ctx;
		}

		tmp.m_f32 = (float)rand() /  (float)INT32_MAX;
				
		rdwr_ram[rd_programme[c_ctx.m_pc + 1]] = tmp.m_u32;

		c_ctx.m_pc += 2;
		return c_ctx;
	}
	}

	c_ctx.m_state = LIBDICE_CTX_OPINVAL;
	return c_ctx;
}

#undef __deref
