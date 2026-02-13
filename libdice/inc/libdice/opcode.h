/** @file opcode.h */

#ifndef libdice_opcode_h
#define libdice_opcode_h

#define LIBDICE_OPERAND_MAX_CNT 5
#define LIBDICE_OPCODE_CNT 56
/**
 *
 * */
enum LIBDICE_OPCODE_
{

	/**
	 * @brief do nothing.
	 * @details nop
	 * */
	LIBDICE_OPCODE_NOP,

	/**
	 * @brief adds two integers.
	 * @details iadd dst nref val nref2 val2
	 * */
	LIBDICE_OPCODE_IADD,

	/**
	 * @brief subtracts two integers.
	 * @details isub dst nref val nref2 val2
	 * */
	LIBDICE_OPCODE_ISUB,

	/**
	 * @brief multiplies two integers.
	 * @details imul dst nref val nref2 val2
	 * */
	LIBDICE_OPCODE_IMUL,

	/**
	 * @brief divides two integers.
	 * @details idiv dst nref val nref2 val2
	 * */
	LIBDICE_OPCODE_IDIV,

	/**
	 * @brief divides two integers and gets its remainder.
	 * @details irem dst nref val nref2 val2
	 * */
	LIBDICE_OPCODE_IREM,

	/**
	 * @brief multiplies two unsigned integers.
	 * @details umul dst nref val nref2 val2
	 * */
	LIBDICE_OPCODE_UMUL,

	/**
	 * @brief divides two unsigned integers.
	 * @details udiv dst nref val nref2 val2
	 * */
	LIBDICE_OPCODE_UDIV,

	/**
	 * @brief divides two unsigned integers and gets its remainder.
	 * @details urem dst nref val nref2 val2
	 * */
	LIBDICE_OPCODE_UREM,

	/**
	 * @brief add two floating points.
	 * @details fadd dst nref val nref2 val2
	 * */
	LIBDICE_OPCODE_FADD,

	/**
	 * @brief subtracts two floating points.
	 * @details fsub dst nref val nref2 val2
	 * */
	LIBDICE_OPCODE_FSUB,

	/**
	 * @brief multiplies two floating points.
	 * @details fmul dst nref val nref2 val2
	 * */
	LIBDICE_OPCODE_FMUL,

	/**
	 * @brief divides two floating points.
	 * @details fdiv dst nref val nref2 val2
	 * */
	LIBDICE_OPCODE_FDIV,

	/**
	 * @brief negates an integer
	 * @details ineg dst nref val
	 * */
	LIBDICE_OPCODE_INEG,

	/**
	 * @brief negates a floating point
	 * @details fneg dst nref val
	 * */
	LIBDICE_OPCODE_FNEG,

	/**
	 * @brief jumps to wanted location.
	 * @details jmp nref dst
	 * */
	LIBDICE_OPCODE_JMP,
	LIBDICE_OPCODE_JMPA,
	LIBDICE_OPCODE_JMPN,

	/**
	 * @brief jumps to wanted location when condition is 0.
	 * @details jmpz nref cnd nref dst
	 * */
	LIBDICE_OPCODE_JMPZ,
	LIBDICE_OPCODE_JMPZA,
	LIBDICE_OPCODE_JMPZN,

	/**
	 * @brief bitand
	 * @details band dst nref val nref2 val2
	 * */
	LIBDICE_OPCODE_BAND,

	/**
	 * @brief bitor
	 * @details bor dst nref val nref2 val2
	 * */
	LIBDICE_OPCODE_BOR,

	/**
	 * @brief bitxor
	 * @details bxor dst nref val nref2 val2
	 * */
	LIBDICE_OPCODE_BXOR,

	/**
	 * @brief bit left shift
	 * @details blshift dst nref val nref2 val2
	 * */
	LIBDICE_OPCODE_BLSHIFT,

	/**
	 * @brief bit right shift
	 * @details brshift dst nref val nref2 val2
	 * */
	LIBDICE_OPCODE_BRSHIFT,

	/**
	 * @brief logicwise right shift
	 * @details lrshift dst nref val nref2 val2
	 * */
	LIBDICE_OPCODE_LRSHIFT,

	/**
	 * @brief bitnot
	 * @details bnot dst nref val
	 * */
	LIBDICE_OPCODE_BNOT,

	/**
	 * @brief logical and
	 * @details land dst nref val nref2 val2
	 * */
	LIBDICE_OPCODE_LAND,

	/**
	 * @brief logical or
	 * @details lor dst nref val nref2 val2
	 * */
	LIBDICE_OPCODE_LOR,

	/**
	 * @brief logical not
	 * @details lnot dst nref val
	 * */
	LIBDICE_OPCODE_LNOT,

	/**
	 * @brief cast integer to bit
	 * @details tobit dst nref val
	 * */
	LIBDICE_OPCODE_TOBIT,

	/**
	 * @brief
	 * check if two values are equal.
	 * result will be normalised to bool.
	 * @details eq dst nref val nref2 val2
	 * */
	LIBDICE_OPCODE_EQ,

	/**
	 * @brief
	 * check if two values are not equal.
	 * result will be normalised to bool.
	 * @details eq dst nref val nref2 val2
	 * */
	LIBDICE_OPCODE_NEQ,
	/**
	 * @brief set a value on one word
	 * @details
	 * set dst val
	 * */
	LIBDICE_OPCODE_SET,

	/**
	 * @brief copy src_val to dst
	 * @details mset nref dst_ptr nref src_val
	 * */
	LIBDICE_OPCODE_MSET,

	/**
	 * @brief copy *src_ptr to *dst_ptr
	 * @details mov nref dst_ptr nref src_ptr
	 * */
	LIBDICE_OPCODE_MOV,

	/**
	 * @brief cast integer to float
	 * @details itof dst nref val
	 * */
	LIBDICE_OPCODE_ITOF,

	/**
	 * @brief cast float to int
	 * @details ftoi dst nref val
	 * */
	LIBDICE_OPCODE_FTOI,

	/**
	 * @brief	Integer Greater Than
	 * @details
	 * ```
	 * igt dst nref val nref2 val2
	 * ```
	 *
	 * would be val > val2 as int
	 * */
	LIBDICE_OPCODE_IGT,

	/**
	 * @brief	Floating point Greater Than
	 * @details
	 * ```
	 * fgt dst nref val nref2 val2
	 * ```
	 *
	 * would be val > val2 as float
	 * */
	LIBDICE_OPCODE_FGT,

	/**
	 * @brief	Floating point Lesser Than
	 * @details
	 * ```
	 * ilt dst nref val nref2 val2
	 * ```
	 *
	 * would be val < val2 as int
	 * */
	LIBDICE_OPCODE_ILT,

	/**
	 * @brief	Floating point Lesser Than
	 * @details
	 * ```
	 * flt dst nref val nref2 val2
	 * ```
	 *
	 * would be val < val2 as float
	 * */
	LIBDICE_OPCODE_FLT,

	/**
	 * @brief	puts string in terminal
	 * @details	puts nref ptr
	 * */
	LIBDICE_OPCODE_PUTS,

	/**
	 * puts		signed integer in terminal
	 * @details	puti nref val
	 * */
	LIBDICE_OPCODE_PUTI,

	/**
	 * puts		unsigned integer in terminal
	 * @details	putu nref val
	 * */
	LIBDICE_OPCODE_PUTU,

	/**
	 * puts		unsigned character in terminal
	 * @details	putc nref val
	 * */
	LIBDICE_OPCODE_PUTC,

	/**
	 * putf		unsigned character in terminal
	 * @details	putc nref val
	 * */
	LIBDICE_OPCODE_PUTF,

	/**
	 * @details def nref key_ptr
	 * */
	LIBDICE_OPCODE_DEF,

	/**
	 * @details undef nref key_ptr
	 * */
	LIBDICE_OPCODE_UNDEF,

	/**
	 * @brief	using `ptrdst` as seed, make new random word and store back in `ptrdst`
	 * @details	rand nref ptrdst(u32*)
	 * */
	LIBDICE_OPCODE_RAND,

	/**
	 * 	@brief	get time as u32
	 * 	@details dst
	 * */
	LIBDICE_OPCODE_TIME,


	/**
	 * @brief end of programme
	 * @details eop
	 * */
	LIBDICE_OPCODE_EOP = 0x7FFFFFFF
};

#endif
