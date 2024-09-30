// This file is part of Open CASCADE Technology software library.
// This file is generated, do not modify it directly; edit source file step.lex instead.

// Pre-include stdlib.h to avoid redefinition of integer type macros (INT8_MIN and similar in generated code)
#if !defined(_MSC_VER) || (_MSC_VER >= 1600) // Visual Studio 2010+
#include "stdint.h"
#endif


/* Target: C/C++ */
/* START of m4 controls */
/* M4_YY_TABLES_VERIFY = 0 */
/* M4_MODE_NO_DO_STDINIT */
/* M4_MODE_NO_YYTEXT_IS_ARRAY */
/* M4_MODE_YYMORE_USED */
/* M4_MODE_NO_REAL_FULLSPD */
/* M4_MODE_NO_REAL_FULLTBL */
/* M4_MODE_NO_CPP_USE_READ */
/* M4_MODE_VARIABLE_TRAILING_CONTEXT_RULES */
/* M4_MODE_FIND_ACTION_REJECT_REALLY_USED */
/* M4_MODE_USES_REJECT */
/* M4_MODE_USEMECS */
/* M4_MODE_FIND_ACTION_REJECT */
/* M4_MODE_NO_FULLSPD */
/* M4_MODE_NO_BOL_NEEDED */
/* M4_MODE_USEECS */
/* M4_MODE_GENTABLES */
/* M4_MODE_NO_INTERACTIVE */
/* M4_MODE_NO_FULLSPD_OR_FULLTBL */
/* M4_MODE_FIND_ACTION_REJECT_OR_INTERACTIVE */
/* M4_MODE_YYCLASS */
/* M4_MODE_NO_YYWRAP */
/* M4_MODE_CXX_ONLY */
/* M4_MODE_PREFIX = step */
/* M4_MODE_NO_YYINPUT */
/* M4_YY_NO_FLEX_ALLOC */
/* M4_YY_NO_FLEX_REALLOC */
/* M4_YY_NO_FLEX_FREE */
/* M4_YY_NO_UNISTD_H */
/* M4_YY_NEVER_INTERACTIVE */
/* M4_MODE_NO_REWRITE */
/* END of m4 controls */





#define FLEX_SCANNER
#define YY_FLEX_MAJOR_VERSION 2
#define YY_FLEX_MINOR_VERSION 6
#define YY_FLEX_SUBMINOR_VERSION 4
#if YY_FLEX_SUBMINOR_VERSION > 0
#define FLEX_BETA
#endif














    /* The c++ scanner is a mess. The FlexLexer.h header file relies on the
     * following macro. This is required in order to pass the c++-multiple-scanners
     * test in the regression suite. We get reports that it breaks inheritance.
     * We will address this in a future release of flex, or omit the C++ scanner
     * altogether.
     */
    #define yyFlexLexer stepFlexLexer









#ifdef yyalloc
#define stepalloc_ALREADY_DEFINED
#else
#define yyalloc stepalloc
#endif

    
#ifdef yyrealloc
#define steprealloc_ALREADY_DEFINED
#else
#define yyrealloc steprealloc
#endif

    
#ifdef yyfree
#define stepfree_ALREADY_DEFINED
#else
#define yyfree stepfree
#endif








/* First, we deal with  platform-specific or compiler-specific issues. */

/* begin standard C headers. */



/* end standard C headers. */

/* begin standard C++ headers. */

#include <iostream>
#include <errno.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
/* end standard C++ headers. */


/* flex integer type definitions */

#ifndef YYFLEX_INTTYPES_DEFINED
#define YYFLEX_INTTYPES_DEFINED

/* Prefer C99 integer types if available. */

# if defined(__cplusplus) && __cplusplus >= 201103L
#include <cstdint>
#  define YYFLEX_USE_STDINT
# endif
# if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
/* Include <inttypes.h> and not <stdint.h> because Solaris 2.6 has the former
 * and not the latter.
 */
#include <inttypes.h>
#  define YYFLEX_USE_STDINT
# else
#  if defined(_MSC_VER) && _MSC_VER >= 1600
/* Visual C++ 2010 does not define __STDC_VERSION__ and has <stdint.h> but not
 * <inttypes.h>.
 */
#include <stdint.h>
#   define YYFLEX_USE_STDINT
#  endif
# endif
# ifdef YYFLEX_USE_STDINT
typedef int8_t flex_int8_t;
typedef uint8_t flex_uint8_t;
typedef int16_t flex_int16_t;
typedef uint16_t flex_uint16_t;
typedef int32_t flex_int32_t;
typedef uint32_t flex_uint32_t;
# else
typedef unsigned char flex_uint8_t;
typedef short int flex_int16_t;
typedef unsigned short int flex_uint16_t;
#  ifdef __STDC__
typedef signed char flex_int8_t;
/* ISO C only requires at least 16 bits for int. */
#   ifdef __cplusplus
#include <climits>
#   else
#include <limits.h>
#   endif
#   if UINT_MAX >= 4294967295
#    define YYFLEX_INT32_DEFINED
typedef int flex_int32_t;
typedef unsigned int flex_uint32_t;
#   endif
#  else
typedef char flex_int8_t;
#  endif
#  ifndef YYFLEX_INT32_DEFINED
typedef long int flex_int32_t;
typedef unsigned long int flex_uint32_t;
#  endif
# endif
#endif /* YYFLEX_INTTYPES_DEFINED */


/* TODO: this is always defined, so inline it */
#define yyconst const

#if defined(__GNUC__) && __GNUC__ >= 3
#define yynoreturn __attribute__((__noreturn__))
#else
#define yynoreturn
#endif

/* Returned upon end-of-file. */
#define YY_NULL 0


/* Promotes a possibly negative, possibly signed char to an
 *   integer in range [0..255] for use as an array index.
 */
#define YY_SC_TO_UI(c) ((YY_CHAR) (c))

































/* Enter a start condition.  This macro really ought to take a parameter,
 * but we do it the disgusting crufty way forced on us by the ()-less
 * definition of BEGIN.
 */
#define yybegin(s) (yy_start) = 1 + 2 * (s)
/* Legacy interface */
#define BEGIN (yy_start) = 1 + 2 *
/* Translate the current start state into a value that can be later handed
 * to BEGIN to return to the state.  The YYSTATE alias is for lex
 * compatibility.
 */
#define yystart() (((yy_start) - 1) / 2)
/* Legacy interfaces */
#define YY_START (((yy_start) - 1) / 2)
#define YYSTATE YY_START
/* Action number for EOF rule of a given start state. */
#define YY_STATE_EOF(state) (YY_END_OF_BUFFER + state + 1)
/* Special action meaning "start processing a new file". */
#define YY_NEW_FILE yyrestart( yyin  )
#define YY_END_OF_BUFFER_CHAR 0



/* The state buf must be large enough to hold one state per character in the main buffer,
 * plus the start state, plus the two end-of-buffer byte states.
 */
#define YY_STATE_BUF_EXTRA_SPACE 3
#define YY_STATE_BUF_SIZE   (YY_BUF_SIZE + YY_STATE_BUF_EXTRA_SPACE)



#ifndef YY_TYPEDEF_YY_BUFFER_STATE
#define YY_TYPEDEF_YY_BUFFER_STATE
typedef struct yy_buffer_state *yybuffer;
/* Legacy interface */
typedef struct yy_buffer_state *YY_BUFFER_STATE;
#endif

#ifndef YY_TYPEDEF_YY_SIZE_T
#define YY_TYPEDEF_YY_SIZE_T
typedef size_t yy_size_t;
#endif


extern int yyleng;





#define EOB_ACT_CONTINUE_SCAN 0
#define EOB_ACT_END_OF_FILE 1
#define EOB_ACT_LAST_MATCH 2
    
    #define YY_LESS_LINENO(n)
    #define YY_LINENO_REWIND_TO(ptr)
    
/* Return all but the first "n" matched characters back to the input stream. */
#define yyless(n) \
	do \
		{ \
		/* Undo effects of setting up yytext. */ \
        int yyless_macro_arg = (n); \
        YY_LESS_LINENO(yyless_macro_arg);\
		*yy_cp = (yy_hold_char); \
		YY_RESTORE_YY_MORE_OFFSET \
		(yy_c_buf_p) = yy_cp = yy_bp + yyless_macro_arg - YY_MORE_ADJ; \
		YY_DO_BEFORE_ACTION; /* set up yytext again */ \
		} \
	while ( 0 )
#define yyunput(c) yyunput_r( c, (yytext_ptr)  )
/* Legacy interface */
#define unput(c) yyunput_r( c, (yytext_ptr)  )


#ifndef YY_STRUCT_YY_BUFFER_STATE
#define YY_STRUCT_YY_BUFFER_STATE
struct yy_buffer_state
	{


	std::streambuf* yy_input_file;

	char *yy_ch_buf;		/* input buffer */
	char *yy_buf_pos;		/* current position in input buffer */

	/* Size of input buffer in bytes, not including room for EOB
	 * characters.
	 */
	int yy_buf_size;

	/* Number of characters read into yy_ch_buf, not including EOB
	 * characters.
	 */
	int yy_n_chars;

	/* Whether we "own" the buffer - i.e., we know we created it,
	 * and can realloc() it to grow it, and should free() it to
	 * delete it.
	 */
	int yy_is_our_buffer;

	/* Whether this is an "interactive" input source; if so, and
	 * if we're using stdio for input, then we want to use getc()
	 * instead of fread(), to make sure we stop fetching input after
	 * each newline.
	 */
	int yy_is_interactive;

	/* Whether we're considered to be at the beginning of a line.
	 * If so, '^' rules will be active on the next match, otherwise
	 * not.
	 */
	int yyatbol;

	int yy_bs_lineno; /**< The line count. */
	int yy_bs_column; /**< The column count. */

	/* Whether to try to fill the input buffer when we reach the
	 * end of it.
	 */
	int yy_fill_buffer;

	int yy_buffer_status;

#define YY_BUFFER_NEW 0
#define YY_BUFFER_NORMAL 1
	/* When an EOF's been seen but there's still some text to process
	 * then we mark the buffer as YY_EOF_PENDING, to indicate that we
	 * shouldn't try reading from the input source any more.  We might
	 * still have a bunch of tokens to match, though, because of
	 * possible backing-up.
	 *
	 * When we actually see the EOF, we change the status to "new"
	 * (via yyrestart()), so that the user can continue scanning by
	 * just pointing yyin at a new input file.
	 */
#define YY_BUFFER_EOF_PENDING 2

	};
#endif /* !YY_STRUCT_YY_BUFFER_STATE */




/* We provide macros for accessing buffer states in case in the
 * future we want to put the buffer states in a more general
 * "scanner state".
 *
 * Returns the top of the stack, or NULL.
 */
#define yy_current_buffer() ( (yy_buffer_stack)			\
                          ? (yy_buffer_stack)[(yy_buffer_stack_top)] \
                          : NULL)
/* Legacy interface */
#define YY_CURRENT_BUFFER yy_current_buffer()
/* Same as previous macro, but useful when we know that the buffer stack is not
 * NULL or when we need an lvalue. For internal use only.
 */
#define YY_CURRENT_BUFFER_LVALUE (yy_buffer_stack)[(yy_buffer_stack_top)]




void *yyalloc ( yy_size_t  );
void *yyrealloc ( void *, yy_size_t  );
void yyfree ( void *  );


#define yy_new_buffer yy_create_buffer
#define yy_set_interactive(is_interactive) { \
	if ( yy_current_buffer() == NULL ) { \
		yyensure_buffer_stack ();	\
		YY_CURRENT_BUFFER_LVALUE =    \
			yy_create_buffer( yyin, YY_BUF_SIZE ); \
	} \
	YY_CURRENT_BUFFER_LVALUE->yy_is_interactive = is_interactive; \
}
#define yysetbol(at_bol) \
	{ \
	if ( yy_current_buffer() == NULL ) { \
		yyensure_buffer_stack ();	\
		YY_CURRENT_BUFFER_LVALUE =    \
            yy_create_buffer( yyin, YY_BUF_SIZE ); \
	} \
	YY_CURRENT_BUFFER_LVALUE->yyatbol = at_bol; \
}
#define yyatbol() (YY_CURRENT_BUFFER_LVALUE->yyatbol)
/* Legacy interface */
#define YY_AT_BOL() (YY_CURRENT_BUFFER_LVALUE->yyatbol)
#define yy_set_bol(at_bol) \
	{ \
	if ( yy_current_buffer() == NULL ) { \
		yyensure_buffer_stack ();	\
		YY_CURRENT_BUFFER_LVALUE =    \
            yy_create_buffer( yyin, YY_BUF_SIZE ); \
	} \
	YY_CURRENT_BUFFER_LVALUE->yyatbol = at_bol; \
}


/* Begin user sect3 */



#define YY_SKIP_YYWRAP


typedef flex_uint8_t YY_CHAR;


#define yytext_ptr yytext










#include <FlexLexer.h>

int yyFlexLexer::yywrap() { return 1;}


int yyFlexLexer::yylex()
	{
	LexerError( "yyFlexLexer::yylex invoked but %option yyclass used" );
	return 0;
	}

#define YY_DECL int step::scanner::yylex()





/* %% [1.5] DFA */
/* START of m4 controls */
/* M4_MODE_NO_NULTRANS */
/* M4_MODE_NO_NULTRANS_FULLTBL */
/* M4_MODE_NO_NULTRANS_FULLSPD */
/* END of m4 controls */

/* START of Flex-generated definitions */
#define YY_NUM_RULES 44
#define YY_END_OF_BUFFER 45
#define YY_JAMBASE 323
#define YY_JAMSTATE 123
#define YY_NUL_EC 49
#define YY_OFFSET_TYPE flex_int16_t
/* END of Flex-generated definitions */





struct yy_trans_info
	{
	/* We require that yy_verify and yy_nxt must be of the same size int. */


	/* We generate a bogus 'struct yy_trans_info' data type
	 * so we can guarantee that it is always declared in the skel.
	 * This is so we can compile "sizeof(struct yy_trans_info)"
	 * in any scanner.
	 */
	flex_int32_t yy_verify;
	flex_int32_t yy_nxt;

	};



/* Done after the current pattern has been matched and before the
 * corresponding action - sets up yytext.
 */
#define YY_DO_BEFORE_ACTION \
	do { \
	(yytext_ptr) = yy_bp; \
	(yytext_ptr) -= (yy_more_len); \
	yyleng = (int) (yy_cp - (yytext_ptr)); \
	 \
	 \
	(yy_hold_char) = *yy_cp; \
	*yy_cp = '\0'; \
 \
	(yy_c_buf_p) = yy_cp; \
	} while(0)





/* %% [2.0] data tables for the DFA are inserted here */









/* footprint: 21234 bytes */
/* tblend: 372 */
/* numecs: 49 */
/* num_rules: 44 */
/* lastdfa: 122 */

/* m4 controls begin */
/* M4_MODE_HAS_BACKING_UP */
/* m4 controls end */





static const flex_int16_t yy_accept[125] = { 0,
        1,    1,    1,    2,    3,    3,    3,    3,    3,    4,
        6,    9,   12,   15,   18,   20,   22,   24,   27,   29,
       32,   35,   37,   40,   43,   45,   48,   52,   55,   58,
       61,   64,   67,   70,   73,   76,   79,   81,   83,   85,
       87,   89,   92,   94,   95,   95,   97,   97,   98,  100,
      101,  102,  102,  103,  105,  108,  109,  110,  111,  112,
      113,  114,  115,  116,  117,  118,  119,  120,  121,  121,
      122,  123,  123,  125,  125,  125,  125,  126,  127,  128,
      129,  130,  131,  132,  132,  133,  134,  136,  137,  137,
      138,  139,  139,  140,  141,  142,  142,  143,  144,  146,

      147,  147,  148,  149,  150,  151,  152,  153,  153,  154,
      155,  156,  157,  157,  158,  159,  160,  161,  161,  162,
      164,  166,  167,  167
};




/* Character equivalence-class mapping */
static const YY_CHAR yy_ec[256] = { 0,
        1,    1,    1,    1,    1,    1,    1,    1,    2,    3,
        1,    1,    4,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    5,    6,    7,    8,    9,    1,   10,   11,   12,
       13,   14,   15,   16,   17,   18,   19,   20,   20,   20,
       20,   20,   20,   20,   20,   20,   20,    1,   21,    1,
       22,    1,    1,    1,   23,   24,   25,   26,   27,   24,
       28,   29,   30,   28,   28,   28,   28,   31,   32,   33,
       28,   34,   35,   36,   28,   28,   28,   28,   28,   28,
        1,    1,    1,    1,   28,    1,   37,   24,   38,   39,

       40,   24,   28,   41,   42,   28,   28,   28,   28,   43,
       44,   45,   28,   46,   47,   48,   28,   28,   28,   28,
       28,   28,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,

        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1
};




/* Character meta-equivalence-class mappings */
static const YY_CHAR yy_meta[50] = { 0,
        1,    1,    2,    1,    1,    1,    1,    1,    1,    1,
        3,    1,    1,    4,    1,    1,    1,    5,    1,    6,
        1,    1,    6,    6,    6,    6,    6,    7,    7,    7,
        7,    7,    7,    7,    7,    7,    6,    6,    6,    6,
        7,    7,    7,    7,    7,    7,    7,    7,    1
};








static const flex_int16_t yy_acclist[167] = { 0,
        2,    2,   45,   42,   44,   10,   42,   44,   12,   42,
       44,   13,   42,   44,   11,   42,   44,   42,   44,   42,
       44,   42,   44,   26,   42,   44,   42,   44,    5,   42,
       44,   23,   42,   44,   24,   44,   18,   42,   44,   25,
       42,   44,   42,   44,   37,   42,   44,   18,   40,   42,
       44,   28,   42,   44,   27,   42,   44,   40,   42,   44,
       40,   42,   44,   40,   42,   44,   40,   42,   44,   40,
       42,   44,   40,   42,   44,   14,   42,   44,    2,   44,
       12,   44,    3,   44,   43,   44,    8,   44,    6,   12,
       44,    7,   44,   41,   17,16400,   19,   18,   19,   19,

       19,    1,   19,   22,   18,   19,   40,   40,   22,   40,
       40,   40,   40,   40,   14,    2,    3,    3,    4,    8,
        9,   21,   15, 8208,   40,   40,   40,   40,   40,   40,
     8208,   20,   20,   20,   40,   40,   40,   40,   40,   36,
       40,   20,   20,   20,   40,   32,   40,   40,   40,   40,
       29,   38,   40,   40,   40,   40,   40,   31,   40,   30,
       35,   39,   40,   33,   34,   34
};




static const flex_int16_t yy_base[135] = { 0,
        0,    0,   47,   48,  255,  252,   49,   52,  253,  323,
      323,  323,  323,  323,    0,    0,  221,  323,   18,  323,
      323,  323,   36,  323,   39,  223,   46,  323,  323,  211,
       49,   40,   50,   52,   55,  148,    0,  323,   55,  323,
        0,  323,   89,    0,  183,   93,   50,   89,   90,   94,
      100,  139,  323,  104,  105,  138,  323,  101,   58,  110,
      118,  108,   90,    0,   66,   87,  323,    0,  148,  323,
      323,  136,  323,  110,  148,  152,  156,  157,  142,  160,
       61,  148,  323,  146,   91,  164,  165,  125,  158,  177,
      169,  186,  201,  323,  180,  165,   84,  190,  194,  323,

      176,  198,  202,  206,  210,  323,  323,  194,  214,  213,
      217,  218,  228,  225,  323,  233,  323,  240,  323,   82,
        0,    0,  323,  265,  272,  279,  281,   83,  284,  287,
      294,  301,  308,  315
};




static const flex_int16_t yy_def[135] = { 0,
      123,    1,  124,  124,  125,  125,  126,  126,  123,  123,
      123,  123,  123,  123,  127,  128,  123,  123,  123,  123,
      123,  123,  123,  123,  129,  123,  130,  123,  123,  130,
      130,  130,  130,  130,  130,  123,  131,  123,  132,  123,
      133,  123,  123,  127,  128,  123,  123,  123,  123,  129,
      129,  129,  123,  123,  130,  130,  123,  130,  130,  130,
      130,  130,  123,  131,  132,  132,  123,  133,  123,  123,
      123,  123,  123,  123,  123,  129,  130,  130,  130,  130,
      130,  130,  123,  123,  123,  129,  130,  130,  123,  130,
      130,  123,  130,  123,  130,  123,  123,  129,  130,  123,

      123,  130,  130,  130,  130,  123,  123,  123,  130,  130,
      130,  130,  123,  130,  123,  130,  123,  123,  123,  130,
      134,  134,    0,  123,  123,  123,  123,  123,  123,  123,
      123,  123,  123,  123
};




static const flex_int16_t yy_nxt[373] = { 0,
       10,   11,   12,   13,   14,   15,   16,   17,   18,   19,
       20,   21,   22,   10,   23,   24,   23,   25,   26,   27,
       28,   29,   30,   30,   30,   31,   32,   30,   33,   34,
       30,   30,   30,   30,   35,   30,   30,   30,   31,   32,
       33,   34,   30,   30,   30,   30,   35,   30,   36,   38,
       38,   42,   47,   48,   42,   49,   50,   57,   51,   43,
       39,   39,   43,   54,   47,   55,   57,   57,   66,   57,
       59,   58,   57,   67,   74,   57,   60,   92,   57,  123,
       93,   94,   59,   79,  123,   58,   61,   74,   45,   60,
       62,   69,   69,   69,   72,   69,   79,   72,   61,   57,

       66,   70,   62,   97,   70,   67,   48,   48,   48,   49,
       97,   50,   46,   51,   73,   75,   75,   54,   57,   51,
       76,   48,   54,   48,   55,   57,   76,   57,   75,   75,
       75,   77,   80,   76,   82,   57,   78,   72,   63,   76,
       72,   84,   57,   75,   77,  100,   80,   82,   78,   81,
       69,   69,   69,   84,   69,   57,   57,   83,   89,   57,
       70,   81,   85,   70,   85,   57,   85,   85,   85,   57,
       85,   86,   85,   57,   57,   87,   90,   57,   96,   88,
       95,   57,   57,   98,   99,   91,   57,  101,   90,   71,
       96,  107,   95,   88,   57,  105,   63,   57,   91,  101,

      106,  102,   92,  103,  107,   92,   94,   57,  105,   98,
      108,   57,  104,   99,  102,   57,  103,   92,   57,   57,
       93,   94,  108,   57,  104,  113,  110,   57,   57,  109,
       57,   57,  111,  115,   57,   57,   53,  113,  117,  110,
       46,  109,   57,  112,  118,  111,  114,  118,  119,  116,
       57,  120,  123,  121,   38,  112,  118,   38,  114,  118,
      119,  116,  123,  123,  120,   37,   37,   37,   37,   37,
       37,   37,   40,   40,   40,   40,   40,   40,   40,   41,
       41,   41,   41,   41,   41,   41,   44,   44,   52,   52,
       52,   56,   56,   56,   64,  123,   64,  123,   64,   64,

       64,   65,  123,   65,   65,   65,   65,   65,   68,  123,
      123,   68,   68,   68,   68,  122,  123,  122,  122,  122,
      122,  122,    9,  123,  123,  123,  123,  123,  123,  123,
      123,  123,  123,  123,  123,  123,  123,  123,  123,  123,
      123,  123,  123,  123,  123,  123,  123,  123,  123,  123,
      123,  123,  123,  123,  123,  123,  123,  123,  123,  123,
      123,  123,  123,  123,  123,  123,  123,  123,  123,  123,
      123,  123
};




static const flex_int16_t yy_chk[373] = { 0,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    3,
        4,    7,   19,   23,    8,   23,   25,   32,   25,    7,
        3,    4,    8,   27,   19,   27,   31,   33,   39,   34,
       32,   31,   35,   39,   47,   59,   33,   81,   81,   65,
       81,   81,   32,   59,   65,   31,   34,   47,  128,   33,
       35,   43,   43,   43,   46,   43,   59,   46,   34,  120,

       66,   43,   35,   97,   43,   66,   48,   49,   48,   49,
       85,   50,   46,   50,   46,   48,   49,   51,   58,   51,
       50,   54,   55,   54,   55,   62,   51,   60,   48,   49,
       54,   55,   60,   50,   62,   61,   58,   72,   63,   51,
       72,   74,   88,   54,   55,   88,   60,   62,   58,   61,
       69,   69,   69,   74,   69,   56,   52,   72,   79,   79,
       69,   61,   75,   69,   75,   82,   76,   75,   76,   76,
       77,   76,   77,   77,   78,   77,   79,   80,   84,   78,
       82,   86,   87,   86,   87,   80,   91,   89,   79,   45,
       84,   96,   82,   78,   90,   91,   36,   95,   80,   89,

       95,   90,   92,   90,   96,   92,   92,   98,   91,   98,
      101,   99,   90,   99,   90,  102,   90,   93,   93,  103,
       93,   93,  101,  104,   90,  108,  103,  105,   30,  102,
      110,  109,  104,  110,  111,  112,   26,  108,  112,  103,
       17,  102,  114,  105,  113,  104,  109,  113,  113,  111,
      116,  114,    9,  116,    6,  105,  118,    5,  109,  118,
      118,  111,    0,    0,  114,  124,  124,  124,  124,  124,
      124,  124,  125,  125,  125,  125,  125,  125,  125,  126,
      126,  126,  126,  126,  126,  126,  127,  127,  129,  129,
      129,  130,  130,  130,  131,    0,  131,    0,  131,  131,

      131,  132,    0,  132,  132,  132,  132,  132,  133,    0,
        0,  133,  133,  133,  133,  134,    0,  134,  134,  134,
      134,  134,  123,  123,  123,  123,  123,  123,  123,  123,
      123,  123,  123,  123,  123,  123,  123,  123,  123,  123,
      123,  123,  123,  123,  123,  123,  123,  123,  123,  123,
      123,  123,  123,  123,  123,  123,  123,  123,  123,  123,
      123,  123,  123,  123,  123,  123,  123,  123,  123,  123,
      123,  123
};













#define YY_TRAILING_MASK 0x2000
#define YY_TRAILING_HEAD_MASK 0x4000





#define yyreject()				\
{ \
*yy_cp = (yy_hold_char); /* undo effects of setting up yytext */ \
yy_cp = (yy_full_match); /* restore poss. backed-over text */ \
 \
(yy_lp) = (yy_full_lp); /* restore orig. accepting pos. */ \
(yy_state_ptr) = (yy_full_state); /* restore orig. state */ \
yy_current_state = *(yy_state_ptr); /* restore curr. state */ \
 \
++(yy_lp); \
goto find_rule; \
}
#define REJECT	yyreject()







#define yymore() ((yy_more_flag) = 1)
#define YY_MORE_ADJ (yy_more_len)
#define YY_RESTORE_YY_MORE_OFFSET







/* %% [3.0] static declarations conditional on mode switches go here */
/* 
 Copyright (c) 1999-2014 OPEN CASCADE SAS

 This file is part of Open CASCADE Technology software library.

 This library is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License version 2.1 as published
 by the Free Software Foundation, with special exception defined in the file
 OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
 distribution for complete text of the license and disclaimer of any warranty.

 Alternatively, this file may be used under the terms of Open CASCADE
 commercial license or contractual agreement.
*/ 
/*
    c++                 generate C++ parser class
    8bit                don't fail on 8-bit input characters
    warn                warn about inconsistencies
    nodefault           don't create default echo-all rule
    noinput             disables the generation of code for reading input from standard input
    noyywrap            don't use yywrap() function
    yyclass             define name of the scanner class
    noyyalloc           disables default allocation function
    noyyfree            disables default deallocation function
    noyyrealloc         disables default reallocation function
    case-insensitive    enable case insensitive parsing(any ?i: and other case setting will be ignored)
*/

#include <step.tab.hxx>
#include "stdio.h"

// Tell flex which function to define
#ifdef  YY_DECL
# undef YY_DECL
#endif
#define YY_DECL int step::scanner::lex (step::parser::semantic_type* /*yylval*/)

// Disable checking for eof
#ifdef  YY_INTERACTIVE
#undef YY_INTERACTIVE
#endif

typedef step::parser::token token;

/* skl 31.01.2002 for OCC133(OCC96,97) - incorrect
long string in files Henri.stp and 401.stp*/
#include <Standard_Failure.hxx>
#define YY_FATAL_ERROR(msg) Standard_Failure::Raise(msg);

/* abv 07.06.02: force inclusion of stdlib.h on WNT to avoid warnings */
#ifdef _MSC_VER
// add includes for flex 2.91 (Linux version)
#include <stdlib.h>
#include <io.h>

// Avoid includion of unistd.h if parser is generated on Linux (flex 2.5.35)
#ifndef YY_NO_UNISTD_H
#define YY_NO_UNISTD_H
#endif

// disable MSVC warnings in flex 2.89 and 2.5.35 code
// Note that Intel compiler also defines _MSC_VER but has different warning ids
#if defined(__INTEL_COMPILER)
#pragma warning(disable:177 1786 1736)
#elif defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Winconsistent-dllimport"
#pragma GCC diagnostic ignored "-Wunneeded-internal-declaration"
#else
#pragma warning(disable:4131 4244 4273 4127 4267)
#endif

#endif /* MSC_VER */

#define CreateNewText myDataModel->CreateNewText
#define SetTypeArg myDataModel->SetTypeArg

// disable GCC warnings in flex code
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wunused-function"
#endif




#define INITIAL 0
#define Com 1
#define End 2
#define Text 3






#ifndef YY_EXTRA_TYPE
#define YY_EXTRA_TYPE void *
#endif





#ifndef yytext_ptr
static void yy_flex_strncpy ( char *, const char *, int );
#endif

#ifdef YY_NEED_STRLEN
static int yy_flex_strlen ( const char * );
#endif

#define YY_NO_YYINPUT 1

#ifndef YY_NO_YYINPUT

#endif




/*
 * Amount of stuff to slurp up with each read.
 * We assume the stdio library has already
 * chosen a fit size foe whatever platform
 * we're running on.
 */
#define YY_READ_BUF_SIZE BUFSIZ

/* Size of default input buffer. We want to be able to fit two
 * OS-level reads, but efficiency gains as the buffer size
 * increases fall off after that
 */
#ifndef YY_BUF_SIZE
#define YY_BUF_SIZE	(2 * YY_READ_BUF_SIZE)
#endif


/* Copy whatever the last rule matched to the standard output. */
#ifndef yyecho


#define yyecho() LexerOutput( yytext, yyleng )

#endif
/* Legacy interface */
#define ECHO yyecho()



#ifndef YY_EXIT_FAILURE
#define YY_EXIT_FAILURE 2
#endif




void yyFlexLexer::LexerError( const char* msg ) {
		std::cerr << msg << std::endl;
	exit( YY_EXIT_FAILURE );
}



/* Report a fatal error. Legacy interface. */
#ifndef YY_FATAL_ERROR


#define YY_FATAL_ERROR(msg) LexerError( msg )

#endif

/* Legacy interface */
#ifndef YY_INPUT
#define YY_INPUT(buf,result,max_size) do {result = yyread(buf, max_size );} while (0)


/* Gets input and stuffs it into "buf".  number of characters read, or YY_NULL,
 * is returned in "result".
 */

int yyFlexLexer::yyread(char *buf, size_t max_size) {


	int result;
	


	if ( (int)(result = LexerInput( (char *) buf, max_size )) < 0 ) {
		YY_FATAL_ERROR( "input in flex scanner failed" );
	}

	 return result;
}
#endif





/* No semi-colon after return; correct usage is to write "yyterminate();" -
 * we don't want an extra ';' after the "return" because that will cause
 * some compilers to complain about unreachable statements.
 */
#ifndef yyterminate
#define yyterminate() return YY_NULL
#endif



/* Number of entries by which start-condition stack grows. */
#ifndef YY_START_STACK_INCR
#define YY_START_STACK_INCR 25
#endif



/* Default declaration of generated scanner - a define so the user can
 * easily add parameters.
 */
#ifndef YY_DECL
#define YY_DECL_IS_OURS 1


#define YY_DECL int yyFlexLexer::yylex()

#endif /* !YY_DECL */


/* Code executed at the beginning of each rule, after yytext and yyleng
 * have been set up.
 */
#ifndef YY_USER_ACTION
#define YY_USER_ACTION
#endif



#define YY_RULE_SETUP \
		YY_USER_ACTION











/** The main scanner function which does all the work.
 */
YY_DECL {
	yy_state_type yy_current_state;
	char *yy_cp, *yy_bp;
	int yy_act;
    

    
    






	if ( !(yy_init) ) {
		(yy_init) = 1;

#ifdef YY_USER_INIT
		YY_USER_INIT;
#endif

		if ( ! (yy_start) ) {
			(yy_start) = 1;	/* first start state */
		}
		if ( ! yyin ) {


			yyin.rdbuf(std::cin.rdbuf());

		}
		if ( ! yyout ) {


			yyout.rdbuf(std::cout.rdbuf());

		}
		if ( yy_current_buffer() == NULL ) {
			yyensure_buffer_stack ();
			YY_CURRENT_BUFFER_LVALUE =
				yy_create_buffer( yyin, YY_BUF_SIZE );
		}


		/* Create the reject buffer large enough to save one state per allowed character.
		 * If the reject buffer already exists, keep using it.
		 */
		if ( ! (yy_state_buf) ) {
			(yy_state_buf) = (yy_state_type *)yyalloc( ((YY_CURRENT_BUFFER_LVALUE->yy_buf_size + YY_STATE_BUF_EXTRA_SPACE) * sizeof(yy_state_type))  );
			if ( ! (yy_state_buf) ) {
				YY_FATAL_ERROR( "out of dynamic memory in yylex()" );
			}
			(yy_state_buf_max) = (YY_CURRENT_BUFFER_LVALUE->yy_buf_size + YY_STATE_BUF_EXTRA_SPACE);
		}


		yy_load_buffer_state(  );
	}

	/* open scope of user declarationns */
	{
/* %% [4.0] user's declarations go here */


		while ( /*CONSTCOND*/1 ) {		/* loops until end-of-file is reached */


			(yy_more_len) = 0;
			if ( (yy_more_flag) ) {
				(yy_more_len) = (int) ((yy_c_buf_p) - (yytext_ptr));
				(yy_more_flag) = 0;
			}


			yy_cp = (yy_c_buf_p);

			/* Support of yytext. */
			*yy_cp = (yy_hold_char);

			/* yy_bp points to the position in yy_ch_buf of the start of
			 * the current run.
			 */
			yy_bp = yy_cp;


	/* Generate the code to find the start state. */


			yy_current_state = (yy_start);

			/* Set up for storing up states. */
			
			(yy_state_ptr) = (yy_state_buf);
			*(yy_state_ptr)++ = yy_current_state;




	yy_match:
			/* Generate the code to find the next match. */






			do {
				
	int yy_c = *(yy_ec+YY_SC_TO_UI(*yy_cp));
	/* Save the backing-up info \before/ computing the next state
	 * because we always compute one more state than needed - we
	 * always proceed until we reach a jam state
	 */
	



	while ( yy_chk[yy_base[yy_current_state] + yy_c] != yy_current_state ) {
		yy_current_state = (int) yy_def[yy_current_state];


		/* We've arranged it so that templates are never chained
		 * to one another.  This means we can afford to make a
		 * very simple test to see if we need to convert to
		 * yy_c's meta-equivalence class without worrying
		 * about erroneously looking up the meta-equivalence
		 * class twice
		 */

		/* lastdfa + 2 == YY_JAMSTATE + 1 is the beginning of the templates */
		if (yy_current_state >= YY_JAMSTATE + 1) {
			yy_c = yy_meta[yy_c];
		}

	}
	yy_current_state = yy_nxt[yy_base[yy_current_state] + yy_c];


				*(yy_state_ptr)++ = yy_current_state;
				++yy_cp;

			}
			
			while ( yy_current_state != YY_JAMSTATE );




	yy_find_action:
			/* code to find the action number goes here */
			
			

			yy_current_state = *--(yy_state_ptr);
			(yy_lp) = yy_accept[yy_current_state];
find_rule: /* we branch to this label when backing up */
			for ( ; ; ) {	/* loop until we find out what rule we matched */
				if ((yy_lp) && (yy_lp) < yy_accept[yy_current_state + 1]) {
					yy_act = yy_acclist[(yy_lp)];

					if ((yy_act & YY_TRAILING_HEAD_MASK) != 0 || (yy_looking_for_trail_begin)) {
						if (yy_act == (yy_looking_for_trail_begin)) {
							(yy_looking_for_trail_begin) = 0;
							yy_act &= ~YY_TRAILING_HEAD_MASK;
							break;
						}
					} else if (( yy_act & YY_TRAILING_MASK) != 0) {
						(yy_looking_for_trail_begin) = yy_act & ~YY_TRAILING_MASK;
						(yy_looking_for_trail_begin) |= YY_TRAILING_HEAD_MASK;

					} else {
						(yy_full_match) = yy_cp;
						(yy_full_state) = (yy_state_ptr);
						(yy_full_lp) = (yy_lp);
						break;
					}
					++(yy_lp);
				goto find_rule;


				}

				--yy_cp;

				/* We could consolidate the following two lines with those at
				 * the beginning, but at the cost of complaints that we're
				 * branching inside a loop.
				 */
				yy_current_state = *--(yy_state_ptr);
				(yy_lp) = yy_accept[yy_current_state];
			} /* close for */



			YY_DO_BEFORE_ACTION;



		do_action:	/* This label is used only to access EOF actions. */



			switch ( yy_act ) { /* beginning of action switch */

/* %% [5.0] user actions get inserted here */
	case 1:
YY_RULE_SETUP

{ BEGIN(Com); }     /* start of comment - put the scanner in the "Com" state */
	/*LINTED*/break;
	case 2:
YY_RULE_SETUP

{;}                 /* in comment, skip any characters except asterisk (and newline, handled by its own rule) */
	/*LINTED*/break;
	case 3:
YY_RULE_SETUP

{;}                 /* in comment, skip any sequence of asterisks followed by other symbols (except slash or newline) */
	/*LINTED*/break;
	case 4:
YY_RULE_SETUP

{ BEGIN(INITIAL); } /* end of comment - reset the scanner to initial state */
	/*LINTED*/break;
	case 5:
YY_RULE_SETUP

{ BEGIN(Text); yymore(); }   /* start of quoted text string - put the scanner in the "Text" state, but keep ' as part of yytext */
	/*LINTED*/break;
	case 6:
/* rule 6 can match eol */
YY_RULE_SETUP

{ yymore(); yylineno ++; }   /* newline in text string - increment line counter and keep collecting yytext */
	/*LINTED*/break;
	case 7:
YY_RULE_SETUP

{ yymore(); }                /* single ' inside text string - keep collecting yytext*/
	/*LINTED*/break;
	case 8:
YY_RULE_SETUP

{ yymore(); }                /* a sequence of any characters except ' and \n - keep collecting yytext */
	/*LINTED*/break;
	case 9:
/* rule 9 can match eol */
*yy_cp = (yy_hold_char); /* undo effects of setting up yytext */
YY_LINENO_REWIND_TO(yy_bp + 1);
(yy_c_buf_p) = yy_cp = yy_bp + 1;
YY_DO_BEFORE_ACTION; /* set up yytext */
YY_RULE_SETUP

{ BEGIN(INITIAL); CreateNewText(YYText(),YYLeng()); SetTypeArg(Interface_ParamText); return(token::QUID); } /* end of string (apostrophe followed by comma or closing parenthesis) - reset the scanner to initial state, record the value of all yytext collected */
	/*LINTED*/break;
	case 10:
YY_RULE_SETUP

{;}
	/*LINTED*/break;
	case 11:
YY_RULE_SETUP

{;}
	/*LINTED*/break;
	case 12:
/* rule 12 can match eol */
YY_RULE_SETUP

{ yylineno ++; } /* count lines (one rule for all start conditions) */
	/*LINTED*/break;
	case 13:
YY_RULE_SETUP

{;} /* abv 30.06.00: for reading DOS files */
	/*LINTED*/break;
	case 14:
YY_RULE_SETUP

{;} /* fix from C21. for test load e3i file with line 15 with null symbols */
	/*LINTED*/break;
	case 15:
*yy_cp = (yy_hold_char); /* undo effects of setting up yytext */
(yy_c_buf_p) = yy_cp -= 1;
YY_DO_BEFORE_ACTION; /* set up yytext */
YY_RULE_SETUP

{ CreateNewText(YYText(),YYLeng()); return(token::ENTITY); }
	/*LINTED*/break;
	case 16:
YY_RULE_SETUP

{ CreateNewText(YYText(),YYLeng()); return(token::ENTITY); }
	/*LINTED*/break;
	case 17:
YY_RULE_SETUP

{ CreateNewText(YYText(),YYLeng()); return(token::IDENT); }
	/*LINTED*/break;
	case 18:
YY_RULE_SETUP

{ CreateNewText(YYText(),YYLeng()); SetTypeArg(Interface_ParamInteger); return(token::QUID); }
	/*LINTED*/break;
	case 19:
YY_RULE_SETUP

{ CreateNewText(YYText(),YYLeng()); SetTypeArg(Interface_ParamReal); return(token::QUID); }
	/*LINTED*/break;
	case 20:
YY_RULE_SETUP

{ CreateNewText(YYText(),YYLeng()); SetTypeArg(Interface_ParamReal); return(token::QUID); }
	/*LINTED*/break;
	case 21:
YY_RULE_SETUP

{ CreateNewText(YYText(),YYLeng()); SetTypeArg(Interface_ParamHexa); return(token::QUID); }
	/*LINTED*/break;
	case 22:
YY_RULE_SETUP

{ CreateNewText(YYText(),YYLeng()); SetTypeArg(Interface_ParamEnum); return(token::QUID); }
	/*LINTED*/break;
	case 23:
YY_RULE_SETUP

{ return ('('); }
	/*LINTED*/break;
	case 24:
YY_RULE_SETUP

{ return (')'); }
	/*LINTED*/break;
	case 25:
YY_RULE_SETUP

{ myDataModel->PrepareNewArg(); return (','); }
	/*LINTED*/break;
	case 26:
YY_RULE_SETUP

{ CreateNewText(YYText(),YYLeng()); SetTypeArg(Interface_ParamVoid); return(token::QUID); }
	/*LINTED*/break;
	case 27:
YY_RULE_SETUP

{ return ('='); }
	/*LINTED*/break;
	case 28:
YY_RULE_SETUP

{ return (';'); }
	/*LINTED*/break;
	case 29:
YY_RULE_SETUP

{ return(token::STEP); }
	/*LINTED*/break;
	case 30:
YY_RULE_SETUP

{ return(token::HEADER); }
	/*LINTED*/break;
	case 31:
YY_RULE_SETUP

{ return(token::ENDSEC); }
	/*LINTED*/break;
	case 32:
YY_RULE_SETUP

{ return(token::DATA); }
	/*LINTED*/break;
	case 33:
YY_RULE_SETUP

{ return(token::ENDSTEP);}
	/*LINTED*/break;
	case 34:
YY_RULE_SETUP

{ return(token::ENDSTEP);}
	/*LINTED*/break;
	case 35:
YY_RULE_SETUP

{ BEGIN(End); return(token::ENDSTEP); } /* at the end of the STEP data, enter dedicated start condition "End" to skip everything that follows */
	/*LINTED*/break;
	case 36:
YY_RULE_SETUP

{ return(token::STEP); }
	/*LINTED*/break;
	case 37:
YY_RULE_SETUP

{ return ('/'); }
	/*LINTED*/break;
	case 38:
YY_RULE_SETUP

{ return(token::SCOPE); }
	/*LINTED*/break;
	case 39:
YY_RULE_SETUP

{ return(token::ENDSCOPE); }
	/*LINTED*/break;
	case 40:
YY_RULE_SETUP

{ CreateNewText(YYText(),YYLeng()); return(token::TYPE); }
	/*LINTED*/break;
	case 41:
YY_RULE_SETUP

{ CreateNewText(YYText(),YYLeng()); return(token::TYPE); }
	/*LINTED*/break;
	case 42:
/* rule 42 can match eol */
YY_RULE_SETUP

{ CreateNewText(YYText(),YYLeng()); SetTypeArg(Interface_ParamMisc); return(token::QUID); }
	/*LINTED*/break;
	case 43:
YY_RULE_SETUP

{;} /* skip any characters (except newlines) */
	/*LINTED*/break;
	case 44:
YY_RULE_SETUP

yyecho();
	/*LINTED*/break;
	case YY_STATE_EOF(INITIAL):
		/* FALLTHROUGH */
	case YY_STATE_EOF(Com):
		/* FALLTHROUGH */
	case YY_STATE_EOF(End):
		/* FALLTHROUGH */
	case YY_STATE_EOF(Text):
		/* FALLTHROUGH */
		yyterminate();

			case YY_END_OF_BUFFER:
			{
				/* Amount of text matched not including the EOB char. */
				int yy_amount_of_matched_text = (int) (yy_cp - (yytext_ptr)) - 1;

				/* Undo the effects of YY_DO_BEFORE_ACTION. */
				*yy_cp = (yy_hold_char);
				YY_RESTORE_YY_MORE_OFFSET

				if ( YY_CURRENT_BUFFER_LVALUE->yy_buffer_status == YY_BUFFER_NEW ) {
					/* We're scanning a new file or input source.  It's
					 * possible that this happened because the user
					 * just pointed yyin at a new source and called
					 * yylex().  If so, then we have to assure
					 * consistency between yy_current_buffer() and our
					 * globals.  Here is the right place to do so, because
					 * this is the first action (other than possibly a
					 * back-up) that will match for the new input source.
					 */
					(yy_n_chars) = YY_CURRENT_BUFFER_LVALUE->yy_n_chars;


					YY_CURRENT_BUFFER_LVALUE->yy_input_file = yyin.rdbuf();

					YY_CURRENT_BUFFER_LVALUE->yy_buffer_status = YY_BUFFER_NORMAL;
				}

				/* Note that here we test for yy_c_buf_p "<=" to the position
				 * of the first EOB in the buffer, since yy_c_buf_p will
				 * already have been incremented past the NUL character
				 * (since all states make transitions on EOB to the
				 * end-of-buffer state).  Contrast this with the test
				 * in input().
				 */
				if ( (yy_c_buf_p) <= &YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[(yy_n_chars)] ) { /* This was really a NUL. */
					yy_state_type yy_next_state;

					(yy_c_buf_p) = (yytext_ptr) + yy_amount_of_matched_text;

					yy_current_state = yy_get_previous_state(  );

					/* Okay, we're now positioned to make the NUL
					 * transition.  We couldn't have
					 * yy_get_previous_state() go ahead and do it
					 * for us because it doesn't know how to deal
					 * with the possibility of jamming (and we don't
					 * want to build jamming into it because then it
					 * will run more slowly).
					 */

					yy_next_state = yy_try_NUL_trans( yy_current_state );

					yy_bp = (yytext_ptr) + YY_MORE_ADJ;

					if ( yy_next_state ) {
						/* Consume the NUL. */
						yy_cp = ++(yy_c_buf_p);
						yy_current_state = yy_next_state;
						goto yy_match;
					} else {
						
						



						/* Still need to initialize yy_cp, though
						 * yy_current_state was set up by
						 * yy_get_previous_state().
						 */
						yy_cp = (yy_c_buf_p);


						goto yy_find_action;
					}
				} else {	/* not a NUL */
					switch ( yy_get_next_buffer(  ) ) {
					case EOB_ACT_END_OF_FILE:
						(yy_did_buffer_switch_on_eof) = 0;

						if ( yywrap(  ) ) {
							/* Note: because we've taken care in
							 * yy_get_next_buffer() to have set up
							 * yytext, we can now set up
							 * yy_c_buf_p so that if some total
							 * hoser (like flex itself) wants to
							 * call the scanner after we return the
							 * YY_NULL, it'll still work - another
							 * YY_NULL will get returned.
							 */
							(yy_c_buf_p) = (yytext_ptr) + YY_MORE_ADJ;

							yy_act = YY_STATE_EOF(yystart());
							goto do_action;
						} else {
							if ( ! (yy_did_buffer_switch_on_eof) ) {
								YY_NEW_FILE;
							}
						}
						break;
					case EOB_ACT_CONTINUE_SCAN:
						(yy_c_buf_p) =
							(yytext_ptr) + yy_amount_of_matched_text;

						yy_current_state = yy_get_previous_state(  );

						yy_cp = (yy_c_buf_p);
						yy_bp = (yytext_ptr) + YY_MORE_ADJ;
						goto yy_match;

					case EOB_ACT_LAST_MATCH:
						(yy_c_buf_p) =
						&YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[(yy_n_chars)];

						yy_current_state = yy_get_previous_state(  );

						yy_cp = (yy_c_buf_p);
						yy_bp = (yytext_ptr) + YY_MORE_ADJ;
						goto yy_find_action;
					default:
						YY_FATAL_ERROR("unexpected return value from yy_get_next_buffer()");
					} /* end EOB inner switch */
				} /* end if */
				break;
			} /* case YY_END_OF_BUFFER */
			default:
				YY_FATAL_ERROR("fatal flex scanner internal error--no action found" );
			} /* end of action switch */
		} /* end of scanning one token */
	} /* end of user's declarations */
} /* end of yylex */



/* The contents of this function are C++ specific, so the () macro is not used.
 * This constructor simply maintains backward compatibility.
 * DEPRECATED
 */
yyFlexLexer::yyFlexLexer( std::istream* arg_yyin, std::ostream* arg_yyout ):
	yyin(arg_yyin ? arg_yyin->rdbuf() : std::cin.rdbuf()),
	yyout(arg_yyout ? arg_yyout->rdbuf() : std::cout.rdbuf())
{
	ctor_common();
}

/* The contents of this function are C++ specific, so the () macro is not used.
 */
yyFlexLexer::yyFlexLexer( std::istream& arg_yyin, std::ostream& arg_yyout ):
	yyin(arg_yyin.rdbuf()),
	yyout(arg_yyout.rdbuf())
{
	ctor_common();
}

/* The contents of this function are C++ specific, so the () macro is not used.
 */
void yyFlexLexer::ctor_common() {
	yy_c_buf_p = 0;
	yy_init = 0;
	yy_start = 0;
	yyflexdebug = 0;
	yylineno = 1;	// this will only get updated if %option yylineno

	yy_did_buffer_switch_on_eof = 0;

	yy_looking_for_trail_begin = 0;
	yy_more_flag = 0;
	yy_more_len = 0;
	yy_more_offset = yy_prev_more_offset = 0;

	yy_start_stack_ptr = yy_start_stack_depth = 0;
	yy_start_stack = NULL;

	yy_buffer_stack = NULL;
	yy_buffer_stack_top = 0;
	yy_buffer_stack_max = 0;



	yy_state_buf = new yy_state_type[YY_STATE_BUF_SIZE];
	yy_state_buf_max = YY_STATE_BUF_SIZE;

}

/* The contents of this function are C++ specific, so the () macro is not used.
 */
yyFlexLexer::~yyFlexLexer() {
	delete [] yy_state_buf;
	yyfree( yy_start_stack  );
	yy_delete_buffer( yy_current_buffer() );
	yyfree( yy_buffer_stack  );
}

/* The contents of this function are C++ specific, so the () macro is not used.
 */
void yyFlexLexer::switch_streams( std::istream& new_in, std::ostream& new_out ) {
	// was if( new_in )
	yy_delete_buffer( YY_CURRENT_BUFFER );
	yy_switch_to_buffer( yy_create_buffer( new_in, YY_BUF_SIZE  ) );

	// was if( new_out )
	yyout.rdbuf(new_out.rdbuf());
}

/* The contents of this function are C++ specific, so the () macro is not used.
 */
void yyFlexLexer::switch_streams( std::istream* new_in, std::ostream* new_out )
{
	if( ! new_in ) {
		new_in = &yyin;
	}

	if ( ! new_out ) {
		new_out = &yyout;
	}

	switch_streams(*new_in, *new_out);
}

#ifdef YY_INTERACTIVE
int yyFlexLexer::LexerInput( char* buf, int /* max_size */ )
#else
int yyFlexLexer::LexerInput( char* buf, int max_size )
#endif
{
	if ( yyin.eof() || yyin.fail() ) {
		return 0;
	}
#ifdef YY_INTERACTIVE
	yyin.get( buf[0] );

	if ( yyin.eof() ) {
		return 0;
	}
	if ( yyin.bad() ) {
		return -1;
	}
	return 1;

#else
	(void) yyin.read( buf, max_size );

	if ( yyin.bad() ) {
		return -1;
	} else {
		return (int)yyin.gcount();
	}
#endif
}

void yyFlexLexer::LexerOutput( const char* buf, int size ) {
	(void) yyout.write( buf, size );
}




/* yy_get_next_buffer - try to read in a new buffer
 *
 * Returns a code representing an action:
 *	EOB_ACT_LAST_MATCH -
 *	EOB_ACT_CONTINUE_SCAN - continue scanning from current position
 *	EOB_ACT_END_OF_FILE - end of file
 */


int yyFlexLexer::yy_get_next_buffer()

{
		char *dest = YY_CURRENT_BUFFER_LVALUE->yy_ch_buf;
	char *source = (yytext_ptr);
	int number_to_move, i;
	int ret_val;

	if ( (yy_c_buf_p) > &YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[(yy_n_chars) + 1] ) {
		YY_FATAL_ERROR( "fatal flex scanner internal error--end of buffer missed" );
	}
	if ( YY_CURRENT_BUFFER_LVALUE->yy_fill_buffer == 0 ) {
		/* Don't try to fill the buffer, so this is an EOF. */
		if ( (yy_c_buf_p) - (yytext_ptr) - YY_MORE_ADJ == 1 ) {
			/* We matched a single character, the EOB, so
			 * treat this as a final EOF.
			 */
			return EOB_ACT_END_OF_FILE;
		} else {
			/* We matched some text prior to the EOB, first
			 * process it.
			 */
			return EOB_ACT_LAST_MATCH;
		}
	}

	/* Try to read more data. */

	/* First move last chars to start of buffer. */
	number_to_move = (int) ((yy_c_buf_p) - (yytext_ptr) - 1);

	for ( i = 0; i < number_to_move; ++i ) {
		*(dest++) = *(source++);
	}
	if ( YY_CURRENT_BUFFER_LVALUE->yy_buffer_status == YY_BUFFER_EOF_PENDING ) {
		/* don't do the read, it's not guaranteed to return an EOF,
		 * just force an EOF
		 */
		YY_CURRENT_BUFFER_LVALUE->yy_n_chars = (yy_n_chars) = 0;
	} else {
		int num_to_read =
			YY_CURRENT_BUFFER_LVALUE->yy_buf_size - number_to_move - 1;

		while ( num_to_read <= 0 ) { /* Not enough room in the buffer - grow it. */

			YY_FATAL_ERROR(
"input buffer overflow, can't enlarge buffer because scanner uses yyreject()" );

		}

		if ( num_to_read > YY_READ_BUF_SIZE ) {
			num_to_read = YY_READ_BUF_SIZE;
		}
		/* Read in more data. */
		YY_INPUT( (&YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[number_to_move]),
			(yy_n_chars), num_to_read );

		YY_CURRENT_BUFFER_LVALUE->yy_n_chars = (yy_n_chars);
	}

	if ( (yy_n_chars) == 0 ) {
		if ( number_to_move == YY_MORE_ADJ ) {
			ret_val = EOB_ACT_END_OF_FILE;
			yyrestart( yyin  );
		} else {
			ret_val = EOB_ACT_LAST_MATCH;
			YY_CURRENT_BUFFER_LVALUE->yy_buffer_status =
				YY_BUFFER_EOF_PENDING;
		}
	} else {
		ret_val = EOB_ACT_CONTINUE_SCAN;
	}
	if (((yy_n_chars) + number_to_move) > YY_CURRENT_BUFFER_LVALUE->yy_buf_size) {
		/* Extend the array by 50%, plus the number we really need. */
		int new_size = (yy_n_chars) + number_to_move + ((yy_n_chars) >> 1) + 2;
		YY_CURRENT_BUFFER_LVALUE->yy_ch_buf = (char *) yyrealloc(
			(void *) YY_CURRENT_BUFFER_LVALUE->yy_ch_buf, (yy_size_t) new_size  );
		if ( ! YY_CURRENT_BUFFER_LVALUE->yy_ch_buf ) {
			YY_FATAL_ERROR( "out of dynamic memory in yy_get_next_buffer()" );
		}
		/* "- 2" to take care of EOB's */
		YY_CURRENT_BUFFER_LVALUE->yy_buf_size = (int) (new_size - 2);
	}

	(yy_n_chars) += number_to_move;
	YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[(yy_n_chars)] = YY_END_OF_BUFFER_CHAR;
	YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[(yy_n_chars) + 1] = YY_END_OF_BUFFER_CHAR;

	(yytext_ptr) = &YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[0];

	return ret_val;
}


/* yy_get_previous_state - get the state just before the EOB char was reached */



yy_state_type yyFlexLexer::yy_get_previous_state()

{
	yy_state_type yy_current_state;
	char *yy_cp;
	
	
	/* Generate the code to find the start state. */


			yy_current_state = (yy_start);

			/* Set up for storing up states. */
			
			(yy_state_ptr) = (yy_state_buf);
			*(yy_state_ptr)++ = yy_current_state;



	for ( yy_cp = (yytext_ptr) + YY_MORE_ADJ; yy_cp < (yy_c_buf_p); ++yy_cp ) {
		/* Generate the code to find the next state. */
		
		

		

		

		
		
	int yy_c = (*yy_cp ? *(yy_ec+YY_SC_TO_UI(*yy_cp)) : YY_NUL_EC);
	/* Save the backing-up info \before/ computing the next state
	 * because we always compute one more state than needed - we
	 * always proceed until we reach a jam state
	 */
	



	while ( yy_chk[yy_base[yy_current_state] + yy_c] != yy_current_state ) {
		yy_current_state = (int) yy_def[yy_current_state];


		/* We've arranged it so that templates are never chained
		 * to one another.  This means we can afford to make a
		 * very simple test to see if we need to convert to
		 * yy_c's meta-equivalence class without worrying
		 * about erroneously looking up the meta-equivalence
		 * class twice
		 */

		/* lastdfa + 2 == YY_JAMSTATE + 1 is the beginning of the templates */
		if (yy_current_state >= YY_JAMSTATE + 1) {
			yy_c = yy_meta[yy_c];
		}

	}
	yy_current_state = yy_nxt[yy_base[yy_current_state] + yy_c];




		
		
		*(yy_state_ptr)++ = yy_current_state;
	}

	return yy_current_state;
}


/* yy_try_NUL_trans - try to make a transition on the NUL character
 *
 * synopsis
 *	next_state = yy_try_NUL_trans( current_state );
 */


yy_state_type yyFlexLexer::yy_try_NUL_trans( yy_state_type yy_current_state )

{
	int yy_is_jam;
		/* Generate code for handling NUL's, if needed. */

	/* First, deal with backing up and setting up yy_cp if the scanner
	 * finds that it should JAM on the NUL.
	 *
	 * Only generate a definition for "yy_cp" if we'll generate code
	 * that uses it.  Otherwise lint and the like complain.
	 */
	











	int yy_c = YY_NUL_EC;
	/* Save the backing-up info \before/ computing the next state
	 * because we always compute one more state than needed - we
	 * always proceed until we reach a jam state
	 */
	



	while ( yy_chk[yy_base[yy_current_state] + yy_c] != yy_current_state ) {
		yy_current_state = (int) yy_def[yy_current_state];


		/* We've arranged it so that templates are never chained
		 * to one another.  This means we can afford to make a
		 * very simple test to see if we need to convert to
		 * yy_c's meta-equivalence class without worrying
		 * about erroneously looking up the meta-equivalence
		 * class twice
		 */

		/* lastdfa + 2 == YY_JAMSTATE + 1 is the beginning of the templates */
		if (yy_current_state >= YY_JAMSTATE + 1) {
			yy_c = yy_meta[yy_c];
		}

	}
	yy_current_state = yy_nxt[yy_base[yy_current_state] + yy_c];

yy_is_jam = (yy_current_state == YY_JAMSTATE);

	/* Only stack this state if it's a transition we
	 * actually make.  If we stack it on a jam, then
	 * the state stack and yy_c_buf_p get out of sync.
	 */
	if ( ! yy_is_jam ) {
		*(yy_state_ptr)++ = yy_current_state;
	}
	






		return yy_is_jam ? 0 : yy_current_state;
}





void yyFlexLexer::yyunput_r( int c, char* yy_bp)

{
	char *yy_cp;
	
	yy_cp = (yy_c_buf_p);

	/* undo effects of setting up yytext */
	*yy_cp = (yy_hold_char);

	if ( yy_cp < YY_CURRENT_BUFFER_LVALUE->yy_ch_buf + 2 ) {
		/* need to shift things up to make room */
		/* +2 for EOB chars. */
		int number_to_move = (yy_n_chars) + 2;
		char *dest = &YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[
					YY_CURRENT_BUFFER_LVALUE->yy_buf_size + 2];
		char *source =
				&YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[number_to_move];

		while ( source > YY_CURRENT_BUFFER_LVALUE->yy_ch_buf ) {
			*--dest = *--source;
		}
		yy_cp += (int) (dest - source);
		yy_bp += (int) (dest - source);
		YY_CURRENT_BUFFER_LVALUE->yy_n_chars =
			(yy_n_chars) = (int) YY_CURRENT_BUFFER_LVALUE->yy_buf_size;

		if ( yy_cp < YY_CURRENT_BUFFER_LVALUE->yy_ch_buf + 2 ) {
			YY_FATAL_ERROR( "flex scanner push-back overflow" );
		}
	}

	*--yy_cp = (char) c;



	(yytext_ptr) = yy_bp;
	(yy_hold_char) = *yy_cp;
	(yy_c_buf_p) = yy_cp;
}





int yyFlexLexer::yyinput()

{
	int c;
	
	*(yy_c_buf_p) = (yy_hold_char);

	if ( *(yy_c_buf_p) == YY_END_OF_BUFFER_CHAR ) {
		/* yy_c_buf_p now points to the character we want to return.
		 * If this occurs *before* the EOB characters, then it's a
		 * valid NUL; if not, then we've hit the end of the buffer.
		 */
		if ( (yy_c_buf_p) < &YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[(yy_n_chars)] ) {
			/* This was really a NUL. */
			*(yy_c_buf_p) = '\0';
		} else {
			/* need more input */
			int offset = (int) ((yy_c_buf_p) - (yytext_ptr));
			++(yy_c_buf_p);

			switch ( yy_get_next_buffer(  ) ) {
			case EOB_ACT_LAST_MATCH:
				/* This happens because yy_g_n_b()
				 * sees that we've accumulated a
				 * token and flags that we need to
				 * try matching the token before
				 * proceeding.  But for input(),
				 * there's no matching to consider.
				 * So convert the EOB_ACT_LAST_MATCH
				 * to EOB_ACT_END_OF_FILE.
				 */

				/* Reset buffer status. */
				yyrestart( yyin );

				/*FALLTHROUGH*/

			case EOB_ACT_END_OF_FILE:
				if ( yywrap(  ) ) {
					return 0;
				}
				if ( ! (yy_did_buffer_switch_on_eof) ) {
					YY_NEW_FILE;
				}
				return yyinput();

			case EOB_ACT_CONTINUE_SCAN:
				(yy_c_buf_p) = (yytext_ptr) + offset;
				break;
			default:
				YY_FATAL_ERROR("unexpected return value from yy_get_next_buffer()");
			}
		}
	}

	c = *(unsigned char *) (yy_c_buf_p);	/* cast for 8-bit char's */
	*(yy_c_buf_p) = '\0';	/* preserve yytext */
	(yy_hold_char) = *++(yy_c_buf_p);






	return c;
}


/** Immediately switch to a different input stream.
 * @param input_file A readable stream.
 * 
 * @note This function does not reset the start condition to @c INITIAL .
 */


void yyFlexLexer::yyrestart( std::istream& input_file )

{
	
	size_t new_size = 0;
	yy_state_type *new_state_buf = 0;
 

	if ( yy_current_buffer() == NULL ) {
		yyensure_buffer_stack ();
		YY_CURRENT_BUFFER_LVALUE =
	        	yy_create_buffer( yyin, YY_BUF_SIZE );
	}

	yy_init_buffer( YY_CURRENT_BUFFER_LVALUE, input_file );
	yy_load_buffer_state(  );


	/* Ensure the reject state buffer is large enough.
	 */
	if ( (yy_state_buf_max) < (yy_size_t) (yy_current_buffer()->yy_buf_size + YY_STATE_BUF_EXTRA_SPACE) ) {
		new_size = yy_current_buffer()->yy_buf_size + YY_STATE_BUF_EXTRA_SPACE;
		new_state_buf = (yy_state_type *)yyrealloc( (yy_state_buf), (new_size * sizeof(yy_state_type))  );

		if ( new_state_buf == NULL ) {
			YY_FATAL_ERROR( "out of dynamic memory in yylex()" );
		}
		else {
			(yy_state_buf) = new_state_buf;
			(yy_state_buf_max) = new_size;
		}
	}
 

	/* We don't actually know whether we did this switch during
	 * EOF (yywrap()) processing, but the only time this flag
	 * is looked at is after yywrap() is called, so it's safe
	 * to go ahead and always set it.
	 */
	(yy_did_buffer_switch_on_eof) = 1;
}


/** Delegate to the new version that takes an istream reference.
 * @param input_file A readable stream.
 * 
 * @note This function does not reset the start condition to @c INITIAL .
 */
void yyFlexLexer::yyrestart( std::istream* input_file )
{
	if( ! input_file ) {
		input_file = &yyin;
	}
	yyrestart( *input_file );
}


/** Switch to a different input buffer.
 * @param new_buffer The new input buffer.
 * 
 */


void yyFlexLexer::yy_switch_to_buffer( yybuffer new_buffer )

{
	
	size_t new_size = 0;
	yy_state_type *new_state_buf = 0;
 

	/* TODO. We should be able to replace this entire function body
	 * with
	 *		yypop_buffer_state();
	 *		yypush_buffer_state(new_buffer);
	 */
	yyensure_buffer_stack ();
	if ( yy_current_buffer() == new_buffer ) {
		return;
	}
	if ( yy_current_buffer() ) {
		/* Flush out information for old buffer. */
		*(yy_c_buf_p) = (yy_hold_char);
		YY_CURRENT_BUFFER_LVALUE->yy_buf_pos = (yy_c_buf_p);
		YY_CURRENT_BUFFER_LVALUE->yy_n_chars = (yy_n_chars);
	}

	YY_CURRENT_BUFFER_LVALUE = new_buffer;
	yy_load_buffer_state(  );


	/* Ensure the reject state buffer is large enough.
	 */
	if ( (yy_state_buf_max) < (yy_size_t) (YY_CURRENT_BUFFER_LVALUE->yy_buf_size + YY_STATE_BUF_EXTRA_SPACE) ) {
		new_size = YY_CURRENT_BUFFER_LVALUE->yy_buf_size + YY_STATE_BUF_EXTRA_SPACE;
		new_state_buf = (yy_state_type *)yyrealloc( (yy_state_buf), (new_size * sizeof(yy_state_type))  );

		if ( new_state_buf == NULL ) {
			YY_FATAL_ERROR( "out of dynamic memory in yylex()" );
		}
		else {
			(yy_state_buf) = new_state_buf;
			(yy_state_buf_max) = new_size;
		}
	}
 

	/* We don't actually know whether we did this switch during
	 * EOF (yywrap()) processing, but the only time this flag
	 * is looked at is after yywrap() is called, so it's safe
	 * to go ahead and always set it.
	 */
	(yy_did_buffer_switch_on_eof) = 1;
}




void yyFlexLexer::yy_load_buffer_state()

{
		(yy_n_chars) = YY_CURRENT_BUFFER_LVALUE->yy_n_chars;
	(yytext_ptr) = (yy_c_buf_p) = YY_CURRENT_BUFFER_LVALUE->yy_buf_pos;


	yyin.rdbuf(YY_CURRENT_BUFFER_LVALUE->yy_input_file);

	(yy_hold_char) = *(yy_c_buf_p);
}

/** Allocate and initialize an input buffer state.
 * @param file A readable stream.
 * @param size The character buffer size in bytes. When in doubt, use @c YY_BUF_SIZE.
 * 
 * @return the allocated buffer state.
 */


yybuffer yyFlexLexer::yy_create_buffer( std::istream& file, int size )

{
	yybuffer b;

	b = (yybuffer) yyalloc( sizeof( struct yy_buffer_state )  );
	if (  b == NULL ) {
		YY_FATAL_ERROR( "out of dynamic memory in yy_create_buffer()" );
	}
	b->yy_buf_size = size;

	/* yy_ch_buf has to be 2 characters longer than the size given because
	 * we need to put in 2 end-of-buffer characters.
	 */
	b->yy_ch_buf = (char *) yyalloc( (yy_size_t) (b->yy_buf_size + 2)  );
	if ( b->yy_ch_buf == NULL ) {
		YY_FATAL_ERROR( "out of dynamic memory in yy_create_buffer()" );
	}
	b->yy_is_our_buffer = 1;

	yy_init_buffer( b, file );

	return b;
}


/** Delegate creation of buffers to the new version that takes an istream reference.
 * @param file A readable stream.
 * @param size The character buffer size in bytes. When in doubt, use @c YY_BUF_SIZE.
 * 
 * @return the allocated buffer state.
 */
yybuffer yyFlexLexer::yy_create_buffer( std::istream* file, int size )
{
	return yy_create_buffer( *file, size );
}


/** Destroy the buffer.
 * @param b a buffer created with yy_create_buffer()
 * 
 */


void yyFlexLexer::yy_delete_buffer( yybuffer b )

{
	
	if ( b == NULL ) {
		return;
	}
	if ( b == yy_current_buffer() ) {	/* Not sure if we should pop here. */
		YY_CURRENT_BUFFER_LVALUE = (yybuffer) 0;
	}
	if ( b->yy_is_our_buffer ) {
		yyfree( (void *) b->yy_ch_buf  );
	}
	yyfree( (void *) b  );
}


/* Initializes or reinitializes a buffer.
 * This function is sometimes called more than once on the same buffer,
 * such as during a yyrestart() or at EOF.
 */


void yyFlexLexer::yy_init_buffer( yybuffer b, std::istream& file )

{
	int oerrno = errno;
	
	yy_flush_buffer( b );



	b->yy_input_file = file.rdbuf();


    /* b->yy_input_file should never by NULL but we'll handle it cleanly
	 * on the off chance.
	 */
	if (b->yy_input_file == NULL){
	  b->yy_fill_buffer = 0;
	} else {
	  b->yy_fill_buffer = 1;
	}



	/* If b is the current buffer, then yy_init_buffer was _probably_
	 * called from yyrestart() or through yy_get_next_buffer.
	 * In that case, we don't want to reset the lineno or column.
	 */
	if (b != yy_current_buffer()) {
		b->yy_bs_lineno = 1;
		b->yy_bs_column = 0;
	}



	b->yy_is_interactive = 0;

	errno = oerrno;
}

/** Discard all buffered characters. On the next scan, YY_INPUT will be called.
 * @param b the buffer state to be flushed, usually @c yy_current_buffer().
 * 
 */


void yyFlexLexer::yy_flush_buffer( yybuffer b )

{
		if ( b == NULL ) {
		return;
	}
	b->yy_n_chars = 0;

	/* We always need two end-of-buffer characters.  The first causes
	 * a transition to the end-of-buffer state.  The second causes
	 * a jam in that state.
	 */
	b->yy_ch_buf[0] = YY_END_OF_BUFFER_CHAR;
	b->yy_ch_buf[1] = YY_END_OF_BUFFER_CHAR;

	b->yy_buf_pos = &b->yy_ch_buf[0];

	b->yyatbol = 1;
	b->yy_buffer_status = YY_BUFFER_NEW;

	if ( b == yy_current_buffer() ) {
		yy_load_buffer_state(  );
	}
}

/** Pushes the new state onto the stack. The new state becomes
 *  the current state. This function will allocate the stack
 *  if necessary.
 *  @param new_buffer The new state.
 *  
 */


void yyFlexLexer::yypush_buffer_state (yybuffer new_buffer)

{
		if (new_buffer == NULL) {
		return;
	}
	yyensure_buffer_stack();

	/* This block is copied from yy_switch_to_buffer. */
	if ( yy_current_buffer() != NULL ) {
		/* Flush out information for old buffer. */
		*(yy_c_buf_p) = (yy_hold_char);
		YY_CURRENT_BUFFER_LVALUE->yy_buf_pos = (yy_c_buf_p);
		YY_CURRENT_BUFFER_LVALUE->yy_n_chars = (yy_n_chars);
	}

	/* Only push if top exists. Otherwise, replace top. */
	if (yy_current_buffer()) {
		(yy_buffer_stack_top)++;
	}
	YY_CURRENT_BUFFER_LVALUE = new_buffer;

	/* copied from yy_switch_to_buffer. */
	yy_load_buffer_state(  );
	(yy_did_buffer_switch_on_eof) = 1;
}


/** Removes and deletes the top of the stack, if present.
 *  The next element becomes the new top.
 *  
 */


void yyFlexLexer::yypop_buffer_state (void)

{
		if (yy_current_buffer() == NULL) {
		return;
	}
	yy_delete_buffer(yy_current_buffer() );
	YY_CURRENT_BUFFER_LVALUE = NULL;
	if ((yy_buffer_stack_top) > 0) {
		--(yy_buffer_stack_top);
	}
	if (yy_current_buffer() != NULL) {
		yy_load_buffer_state(  );
		(yy_did_buffer_switch_on_eof) = 1;
	}
}


/* Allocates the stack if it does not exist.
 *  Guarantees space for at least one push.
 */


void yyFlexLexer::yyensure_buffer_stack(void)

{
	yy_size_t num_to_alloc;
	
	if ((yy_buffer_stack) == NULL) {
		/* First allocation is just for 2 elements, since we don't know if this
		 * scanner will even need a stack. We use 2 instead of 1 to avoid an
		 * immediate realloc on the next call.
		 */
		num_to_alloc = 1; /* After all that talk, this was set to 1 anyways... */
		(yy_buffer_stack) = (struct yy_buffer_state**)yyalloc
								(num_to_alloc * sizeof(struct yy_buffer_state*)
								);
		if ( (yy_buffer_stack == NULL) ) {
			YY_FATAL_ERROR( "out of dynamic memory in yyensure_buffer_stack()" );
		}

		memset((yy_buffer_stack), 0, num_to_alloc * sizeof(struct yy_buffer_state*));

		(yy_buffer_stack_max) = num_to_alloc;
		(yy_buffer_stack_top) = 0;
		return;
	}

	if ((yy_buffer_stack_top) >= ((yy_buffer_stack_max)) - 1) {
		/* Increase the buffer to prepare for a possible push. */
		yy_size_t grow_size = 8 /* arbitrary grow size */;

		num_to_alloc = (yy_buffer_stack_max) + grow_size;
		(yy_buffer_stack) = (struct yy_buffer_state**)yyrealloc
								((yy_buffer_stack),
								num_to_alloc * sizeof(struct yy_buffer_state*)
								);
		if ((yy_buffer_stack) == NULL) {
			YY_FATAL_ERROR( "out of dynamic memory in yyensure_buffer_stack()" );
		}
		/* zero only the new slots.*/
		memset((yy_buffer_stack) + (yy_buffer_stack_max), 0, grow_size * sizeof(struct yy_buffer_state*));
		(yy_buffer_stack_max) = num_to_alloc;
	}
}






















void yyFlexLexer::yy_push_state( int _new_state )

{
		if ( (yy_start_stack_ptr) >= (yy_start_stack_depth) ) {
		yy_size_t new_size;

		(yy_start_stack_depth) += YY_START_STACK_INCR;
		new_size = (yy_size_t) (yy_start_stack_depth) * sizeof( int );

		if ( ! (yy_start_stack) ) {
			(yy_start_stack) = (int *) yyalloc( new_size  );

		} else {
			(yy_start_stack) = (int *) yyrealloc(
					(void *) (yy_start_stack), new_size  );
		}
		if ( ! (yy_start_stack) ) {
			YY_FATAL_ERROR( "out of memory expanding start-condition stack" );
		}
	}
	(yy_start_stack)[(yy_start_stack_ptr)++] = yystart();

	yybegin(_new_state);
}






void yyFlexLexer::yy_pop_state()

{
		if ( --(yy_start_stack_ptr) < 0 ) {
		YY_FATAL_ERROR( "start-condition stack underflow" );
	}
	yybegin((yy_start_stack)[(yy_start_stack_ptr)]);
}






int yyFlexLexer::yy_top_state()

{
		return (yy_start_stack_ptr) > 0 ? (yy_start_stack)[(yy_start_stack_ptr) - 1] : yystart();
}


/* Redefine yyless() so it works in section 3 code. */

#undef yyless
#define yyless(n) \
	do { \
		/* Undo effects of setting up yytext. */ \
		int yyless_macro_arg = (n); \
		YY_LESS_LINENO(yyless_macro_arg);\
			yytext[yyleng] = (yy_hold_char); \
			(yy_c_buf_p) = yytext + yyless_macro_arg; \
			(yy_hold_char) = *(yy_c_buf_p); \
			*(yy_c_buf_p) = '\0'; \
			yyleng = yyless_macro_arg; \
	} while ( 0 )



/* Accessor  methods (get/set functions) to struct members. */












/*
 * Internal utility routines.
 */



#ifndef yytext_ptr
static void yy_flex_strncpy (char* s1, const char * s2, int n ) {
		
	int i;
	for ( i = 0; i < n; ++i ) {
		s1[i] = s2[i];
	}
}
#endif



#ifdef YY_NEED_STRLEN
static int yy_flex_strlen (const char * s )
{
	int n;
	for ( n = 0; s[n]; ++n )
		;

	return n;
}
#endif


















#include <Standard.hxx>

void* yyalloc (size_t theNbBytes)
{
  return Standard::AllocateOptimal (theNbBytes);
}

void* yyrealloc (void * thePnt, size_t theNbBytes)
{
   return Standard::Reallocate (thePnt, theNbBytes);
}

void yyfree (void* thePnt)
{      
  Standard::Free (thePnt);
}

step::scanner::scanner(StepFile_ReadData* theDataModel, std::istream* in, std::ostream* out)
    : stepFlexLexer(in, out), myDataModel(theDataModel)
{
}

