// A Bison parser, made by GNU Bison 3.7.4.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2020 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.

// "%code top" blocks.

  // Created:	Thu Oct 28 12:21:16 1999
  // Author:	Andrey BETENEV
  // Copyright (c) 1999-2020 OPEN CASCADE SAS
  //
  // This file is part of Open CASCADE Technology software library.
  //
  // This library is free software; you can redistribute it and/or modify it under
  // the terms of the GNU Lesser General Public License version 2.1 as published
  // by the Free Software Foundation, with special exception defined in the file
  // OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
  // distribution for complete text of the license and disclaimer of any warranty.
  //
  // Alternatively, this file may be used under the terms of Open CASCADE
  // commercial license or contractual agreement.

  /*****************************************************************************
  
  This YACC parser implements parsing algorithm for EXPRESS -> CASCADE/XSTEP
  classes generator
  
  Input in the form of tokens is obtained from lexical analyser. Then, data 
  structure representing schema is created on the basis of grammar analysis.
  
  NOTE: The grammar currently implemented is not full. 
  FUNCTION, RULE and CONSTANT items, WHERE, INVERSE and DERIVE clauses 
  of TYPE and ENTITY items are not considered (ignored on the level of lexical 
  scanner). 
  SUPERTYPE and UNIQUE clauses of ENTITY item are recognized but ignored.
  Also, complex constructs such as using call to function in dimensions of 
  complex time or redefinition of inherited field are ignored.
  
  *****************************************************************************/


// Take the name prefix into account.
#define yylex   exptocaslex



#include "exptocas.tab.hxx"


// Unqualified %code blocks.

#undef yylex
#define yylex scanner->lex
// disable MSVC warnings in bison code
#ifdef _MSC_VER
#pragma warning(disable:4244 4800)
#endif

/************************************************/
/* ERROR MESSAGE FUNCTION                       */

/* external functions (from exptocas.l) */
int ec_error ( const std::string& s, const std::string& text );
int ec_curline ( void );

//int yyerror ( char *s )
//{
//  printf ( "\nParse error at line %d: %s\n", ec_curline(), s );
//  return 0;
//}

/************************************************/
/* FUNCTIONS FOR CREATING SCHEMA REPRESENTATION */

static Express_Schema *mkschema ( char *name, Express_HSequenceOfItem *ilist );
static Express_HSequenceOfItem *mkilist ( Express_Item *item, Express_HSequenceOfItem *seq );
static Express_Item *mkenum ( char *name, TColStd_HSequenceOfHAsciiString *tlist );
static Express_Item *mkselect ( char *name, TColStd_HSequenceOfHAsciiString *tlist );
static Express_Item *mkalias ( char *name, Express_Type *type );
static Express_Item *mkentity ( char *name, TColStd_HSequenceOfHAsciiString *inherit,
                                Express_HSequenceOfField *field, int isabstract );
static Express_Reference *mkrefs ( char *name, TColStd_HSequenceOfHAsciiString *items);
static TColStd_HSequenceOfHAsciiString *mktlist ( char *name, TColStd_HSequenceOfHAsciiString *tlist );
static TColStd_HSequenceOfHAsciiString *mktlists ( TColStd_HSequenceOfHAsciiString *tlist1, TColStd_HSequenceOfHAsciiString *tlist2 );
static Express_Type *mktstd ( int keyword );
static Express_Type *mktname ( char *name );
static Express_Type *mktset ( int keyword, int ilow, int ihigh, Express_Type *of );
static Express_Field *mkfield ( char *name, Express_Type *type, int optional );
static Express_HSequenceOfField *mkflist ( Express_Field *field, Express_HSequenceOfField *seq );



#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif



// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

namespace exptocas {

  /// Build a parser object.
  parser::parser (exptocas::scanner* scanner_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      scanner (scanner_yyarg)
  {}

  parser::~parser ()
  {}

  parser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------------.
  | symbol kinds.  |
  `---------------*/

  // basic_symbol.
  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value (that.value)
  {}


  /// Constructor for valueless symbols.
  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t)
    : Base (t)
    , value ()
  {}

  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, YY_RVREF (semantic_type) v)
    : Base (t)
    , value (YY_MOVE (v))
  {}

  template <typename Base>
  parser::symbol_kind_type
  parser::basic_symbol<Base>::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }

  template <typename Base>
  bool
  parser::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return this->kind () == symbol_kind::S_YYEMPTY;
  }

  template <typename Base>
  void
  parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    value = YY_MOVE (s.value);
  }

  // by_kind.
  parser::by_kind::by_kind ()
    : kind_ (symbol_kind::S_YYEMPTY)
  {}

#if 201103L <= YY_CPLUSPLUS
  parser::by_kind::by_kind (by_kind&& that)
    : kind_ (that.kind_)
  {
    that.clear ();
  }
#endif

  parser::by_kind::by_kind (const by_kind& that)
    : kind_ (that.kind_)
  {}

  parser::by_kind::by_kind (token_kind_type t)
    : kind_ (yytranslate_ (t))
  {}

  void
  parser::by_kind::clear ()
  {
    kind_ = symbol_kind::S_YYEMPTY;
  }

  void
  parser::by_kind::move (by_kind& that)
  {
    kind_ = that.kind_;
    that.clear ();
  }

  parser::symbol_kind_type
  parser::by_kind::kind () const YY_NOEXCEPT
  {
    return kind_;
  }

  parser::symbol_kind_type
  parser::by_kind::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


  // by_state.
  parser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  parser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  parser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  parser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  parser::symbol_kind_type
  parser::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  parser::stack_symbol_type::stack_symbol_type ()
  {}

  parser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.value))
  {
#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  parser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.value))
  {
    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    return *this;
  }

  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);

    // User destructor.
    YYUSE (yysym.kind ());
  }

#if YYDEBUG
  template <typename Base>
  void
  parser::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " (";
        YYUSE (yykind);
        yyo << ')';
      }
  }
#endif

  void
  parser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  parser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  parser::yypop_ (int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  parser::debug_level_type
  parser::debug_level () const
  {
    return yydebug_;
  }

  void
  parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  parser::state_type
  parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  parser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  parser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  parser::operator() ()
  {
    return parse ();
  }

  int
  parser::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            yyla.kind_ = yytranslate_ (yylex (&yyla.value));
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* If YYLEN is nonzero, implement the default value of the
         action: '$$ = $1'.  Otherwise, use the top of the stack.

         Otherwise, the following line sets YYLHS.VALUE to garbage.
         This behavior is undocumented and Bison users should not rely
         upon it.  */
      if (yylen)
        yylhs.value = yystack_[yylen - 1].value;
      else
        yylhs.value = yystack_[0].value;


      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2: // SCHEMA: KSCHEM NAME ';' ILIST KENDS ';'
                                        { (yylhs.value.schema) = mkschema ( (yystack_[4].value.str), (yystack_[2].value.ilist) ); /* Root: EXPRESS schema */ }
    break;

  case 3: // ILIST: ITEM
                         { (yylhs.value.ilist) = mkilist ( (yystack_[0].value.item), 0 ); /* list of items for schema */ }
    break;

  case 4: // ILIST: ITEM ILIST
                         { (yylhs.value.ilist) = mkilist ( (yystack_[1].value.item), (yystack_[0].value.ilist) ); }
    break;

  case 5: // ITEM: ENUM
        { (yylhs.value.item) = (yystack_[0].value.item); }
    break;

  case 6: // ITEM: SELECT
        { (yylhs.value.item) = (yystack_[0].value.item); }
    break;

  case 7: // ITEM: ALIAS
        { (yylhs.value.item) = (yystack_[0].value.item); }
    break;

  case 9: // ITEM: ENTITY
                         { (yylhs.value.item) = (yystack_[0].value.item); /* item of schema (type definition) */ }
    break;

  case 10: // ENUM: KTYP NAME '=' KENUM KOF TLIST1 ';' KENDT ';'
                                                     { (yylhs.value.item) = mkenum ( (yystack_[7].value.str), (yystack_[3].value.tlist) ); /* TYPE ENUMERATION definition */ }
    break;

  case 11: // SELECT: KTYP NAME '=' KSEL TLIST1 ';' KENDT ';'
                                                { (yylhs.value.item) = mkselect ( (yystack_[6].value.str), (yystack_[3].value.tlist) ); /* TYPE SELECT definition */ }
    break;

  case 12: // ALIAS: KTYP NAME '=' TYPE ';' KENDT ';'
                                         { (yylhs.value.item) = mkalias ( (yystack_[5].value.str), (yystack_[3].value.type) ); /* TYPE '=' definition (alias) */ }
    break;

  case 13: // ENTITY: KENT NAME SUPERT SUBT ';' FLIST1 UNIQUE KENDE ';'
                                                                 { (yylhs.value.item) = mkentity ( (yystack_[7].value.str), (yystack_[5].value.tlist), (yystack_[3].value.flist), 0 ); /* ENTITY definition */ }
    break;

  case 14: // ENTITY: KENT NAME KABSTR SUPERT SUBT ';' FLIST1 UNIQUE KENDE ';'
                                                                 { (yylhs.value.item) = mkentity ( (yystack_[8].value.str), (yystack_[5].value.tlist), (yystack_[3].value.flist), 1 ); /* ENTITY definition */ }
    break;

  case 15: // REFERENCE: KREF KFROM NAME TLIST1 ';'
                                      { (yylhs.value.ref) = mkrefs ( (yystack_[2].value.str), (yystack_[1].value.tlist) ); /* REFERENCE FROM definition */ }
    break;

  case 16: // TLIST: NAME
                         { (yylhs.value.tlist) = mktlist ( (yystack_[0].value.str), 0 ); /* list of (type) names */ }
    break;

  case 17: // TLIST: NAME ',' TLIST
                         { (yylhs.value.tlist) = mktlist ( (yystack_[2].value.str), (yystack_[0].value.tlist) ); }
    break;

  case 18: // TLIST1: '(' TLIST ')'
                         { (yylhs.value.tlist) = (yystack_[1].value.tlist); /* TLIST in brackets */ }
    break;

  case 19: // TYPE: TSTD
        { (yylhs.value.type) = (yystack_[0].value.type); }
    break;

  case 20: // TYPE: TNAME
        { (yylhs.value.type) = (yystack_[0].value.type); }
    break;

  case 21: // TYPE: TSET
                         { (yylhs.value.type) = (yystack_[0].value.type); /* type, simple or complex */ }
    break;

  case 22: // TSTD: KINT
                         { (yylhs.value.type) = mktstd ( (yystack_[0].value.num) ); /* predefined type: INTEGER */ }
    break;

  case 23: // TSTD: KNUM
                         { (yylhs.value.type) = mktstd ( (yystack_[0].value.num) ); /* predefined type: NUMBER */ }
    break;

  case 24: // TSTD: KDBL
                         { (yylhs.value.type) = mktstd ( (yystack_[0].value.num) ); /* predefined type: REAL */ }
    break;

  case 25: // TSTD: KSTR
                         { (yylhs.value.type) = mktstd ( (yystack_[0].value.num) ); /* predefined type: STRING */ }
    break;

  case 26: // TSTD: KLOG
                         { (yylhs.value.type) = mktstd ( (yystack_[0].value.num) ); /* predefined type: LOGICAL */ }
    break;

  case 27: // TSTD: KBOOL
                         { (yylhs.value.type) = mktstd ( (yystack_[0].value.num) ); /* predefined type: BOOLEAN */ }
    break;

  case 28: // TNAME: NAME
                         { (yylhs.value.type) = mktname ( (yystack_[0].value.str) ); /* named type */ }
    break;

  case 29: // TSET: KLIST '[' INDEX ':' INDEX ']' KOF OPTUNI TYPE
                                                      { (yylhs.value.type) = mktset ( (yystack_[8].value.num), (yystack_[6].value.num), (yystack_[4].value.num), (yystack_[0].value.type) ); /* complex type: LIST */ }
    break;

  case 30: // TSET: KARR '[' INDEX ':' INDEX ']' KOF OPTUNI TYPE
                                                      { (yylhs.value.type) = mktset ( (yystack_[8].value.num), (yystack_[6].value.num), (yystack_[4].value.num), (yystack_[0].value.type) ); /* complex type: ARRAY */ }
    break;

  case 31: // TSET: KSET '[' INDEX ':' INDEX ']' KOF OPTUNI TYPE
                                                      { (yylhs.value.type) = mktset ( (yystack_[8].value.num), (yystack_[6].value.num), (yystack_[4].value.num), (yystack_[0].value.type) ); /* complex type: SET */ }
    break;

  case 32: // TSET: KBAG '[' INDEX ':' INDEX ']' KOF OPTUNI TYPE
                                                      { (yylhs.value.type) = mktset ( (yystack_[8].value.num), (yystack_[6].value.num), (yystack_[4].value.num), (yystack_[0].value.type) ); /* complex type: BAG */ }
    break;

  case 33: // INDEX: NUMBER
                         { (yylhs.value.num) = (yystack_[0].value.num); /* index for array, set, bag, list range */ }
    break;

  case 34: // INDEX: '?'
                         { (yylhs.value.num) = -1; /* undefined */ }
    break;

  case 35: // INDEX: NAME '(' NAME ')'
                          {(yylhs.value.num) = -1; printf ( "Warning at line %d: index function %s(%s) ignored\n", ec_curline(), (yystack_[3].value.str), (yystack_[1].value.str) ); /* some function.. */ }
    break;

  case 36: // OPTUNI: %empty
                         { (yylhs.value.num) = 0; /* UNIQUE keyword for complex type definition */ }
    break;

  case 37: // OPTUNI: KUNIQ
                         { (yylhs.value.num) = 1; }
    break;

  case 38: // SUBT: %empty
                         { (yylhs.value.tlist) = NULL; /* no subtype clause */ }
    break;

  case 39: // SUBT: KSUBT KOF TLIST1
                         { (yylhs.value.tlist) = (yystack_[0].value.tlist);   /* subtype clause */ }
    break;

  case 40: // SUPERT: %empty
                         { (yylhs.value.num) = 0;  /* no supertype clause */ }
    break;

  case 41: // SUPERT: KSPRT
                         { (yylhs.value.num) = 0;  /* supertype clause (ignored) */ }
    break;

  case 42: // SUPERT: KSPRT KOF SUPLST
                         { (yylhs.value.num) = 0;  /* supertype clause (ignored) */ }
    break;

  case 43: // SUPLST: NAME
                         { (yylhs.value.num) = 0; /* simple list of supertypes */ }
    break;

  case 44: // SUPLST: '(' SUPLST ')'
                         { (yylhs.value.num) = 0; /* allow bracketing */ }
    break;

  case 45: // SUPLST: NAME ',' SUPLST
                         { (yylhs.value.num) = 0; /* ... */ }
    break;

  case 46: // SUPLST: K1OF SUPLST
                         { (yylhs.value.num) = 0; /* ONEOF construct */ }
    break;

  case 47: // SUPLST: SUPLST KANDOR SUPLST
                             { (yylhs.value.num) = 0; /* ANDOR construct */ }
    break;

  case 48: // FLIST: FIELD
                         { (yylhs.value.flist) = mkflist ( (yystack_[0].value.field), 0 ); /* list of fields of ENTITY item */ }
    break;

  case 49: // FLIST: FIELD FLIST
                         { (yylhs.value.flist) = mkflist ( (yystack_[1].value.field), (yystack_[0].value.flist) ); }
    break;

  case 50: // FLIST: REDEF
                         { (yylhs.value.flist) = 0;  /* redefinition of inherited field, just skip */ }
    break;

  case 51: // FLIST: REDEF FLIST
                         { (yylhs.value.flist) = (yystack_[0].value.flist); /* ignore redefinition of inherited field, take trailing list */ }
    break;

  case 52: // FLIST1: %empty
                         { (yylhs.value.flist) = NULL; /* empty list of fields */ }
    break;

  case 53: // FLIST1: FLIST
                         { (yylhs.value.flist) = (yystack_[0].value.flist);   /* or not empty.. just to fix reduce/reduce conflict */ }
    break;

  case 54: // FIELD: NAME ':' OPTNL TYPE ';'
                                { (yylhs.value.field) = mkfield ( (yystack_[4].value.str), (yystack_[1].value.type), (yystack_[2].value.num) ); }
    break;

  case 55: // REDEF: KSELF '\\' SPECIF ':' TYPE ';'
                                       { (yylhs.value.num) = 0; printf ( "Warning at line %d: field redefinition ignored\n", ec_curline() ); /* redefinition of inherited field */ }
    break;

  case 56: // SPECIF: NAME
                         { (yylhs.value.tlist) = mktlist ( (yystack_[0].value.str), 0 ); /* inherited field specification */ }
    break;

  case 57: // SPECIF: NAME '.' SPECIF
                         { (yylhs.value.tlist) = mktlist ( (yystack_[2].value.str), (yystack_[0].value.tlist) ); }
    break;

  case 58: // OPTNL: %empty
                         { (yylhs.value.num) = 0; }
    break;

  case 59: // OPTNL: KOPT
                         { (yylhs.value.num) = 1; }
    break;

  case 60: // UNIQIT: NAME ':' TLIST ';'
                            { (yylhs.value.tlist) = (yystack_[1].value.tlist);   /* UNIQUE statement */ }
    break;

  case 61: // UNIQIT: NAME ':' SPCLST ';'
                            { (yylhs.value.tlist) = NULL; /* UNIQUE statement */ }
    break;

  case 62: // UNIQLS: UNIQIT
                         { (yylhs.value.tlist) = NULL;    /* list of 1 UNIQUE statements */ }
    break;

  case 63: // UNIQLS: UNIQIT UNIQLS
                         { (yylhs.value.tlist) = mktlists ( (yystack_[1].value.tlist), (yystack_[0].value.tlist) );/* list of UNIQUE statements */ }
    break;

  case 64: // UNIQUE: %empty
                         { (yylhs.value.tlist) = NULL;    /* no UNIQUE clause in ENTITY */ }
    break;

  case 65: // UNIQUE: KUNIQ UNIQLS
                         { (yylhs.value.tlist) = (yystack_[0].value.tlist);      /* UNIQUE clause in ENTITY */ }
    break;

  case 66: // SPCLST: KSELF '\\' SPECIF
                               { (yylhs.value.num) = 0; /* list of specifications */ }
    break;

  case 67: // SPCLST: KSELF '\\' SPECIF ',' SPCLST
                                     { (yylhs.value.num) = 0; }
    break;

  case 68: // SPCLST: NAME ',' SPCLST
                               { (yylhs.value.num) = 0; }
    break;



            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        std::string msg = YY_("syntax error");
        error (YY_MOVE (msg));
      }


    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;


      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  parser::error (const syntax_error& yyexc)
  {
    error (yyexc.what ());
  }

#if YYDEBUG || 0
  const char *
  parser::symbol_name (symbol_kind_type yysymbol)
  {
    return yytname_[yysymbol];
  }
#endif // #if YYDEBUG || 0





  const short parser::yypact_ninf_ = -134;

  const signed char parser::yytable_ninf_ = -1;

  const short
  parser::yypact_[] =
  {
      24,     3,    51,     5,  -134,    73,    20,    34,    60,    70,
      73,  -134,  -134,  -134,  -134,  -134,    42,    44,    47,    48,
    -134,    -1,    54,    68,    58,    49,  -134,    49,    72,    50,
      52,    53,    55,  -134,  -134,  -134,  -134,  -134,  -134,  -134,
      61,  -134,  -134,  -134,    58,    38,    74,    62,    65,    64,
      66,    49,    -8,    -8,    -8,    -8,    88,    67,    38,    69,
      38,    71,    49,    12,    75,    57,  -134,    99,    76,  -134,
      77,  -134,    78,    79,    80,    81,    82,    12,  -134,    38,
       1,    38,  -134,    63,    83,  -134,    84,    12,    12,    65,
    -134,    87,   100,    91,    -8,    -8,    -8,    -8,  -134,    84,
    -134,  -134,  -134,    92,    89,    93,   103,  -134,  -134,  -134,
    -134,    94,    90,    86,    95,    96,    97,   106,    98,   101,
    -134,    43,   104,    93,  -134,   105,  -134,  -134,   114,   115,
     116,   117,   108,    92,    43,   110,    26,  -134,  -134,   111,
     111,   111,   111,  -134,  -134,   112,  -134,   107,   118,   113,
     119,  -134,    43,    43,    43,    43,  -134,    92,    26,  -134,
    -134,  -134,  -134,  -134,  -134,   121,  -134,    41,   122,  -134,
      41
  };

  const signed char
  parser::yydefact_[] =
  {
       0,     0,     0,     0,     1,     0,     0,     0,     0,     0,
       3,     5,     6,     7,     9,     8,     0,    40,     0,     0,
       4,     0,    40,    41,    38,     0,     2,     0,     0,     0,
       0,     0,     0,    23,    22,    24,    25,    26,    27,    28,
       0,    19,    20,    21,    38,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    43,
       0,    42,     0,    52,    16,     0,    15,     0,     0,    33,
       0,    34,     0,     0,     0,     0,     0,    52,    46,     0,
       0,     0,    39,     0,     0,    53,    64,    48,    50,     0,
      18,     0,     0,     0,     0,     0,     0,     0,    12,    64,
      45,    44,    47,     0,    58,     0,     0,    49,    51,    17,
      11,     0,     0,     0,     0,     0,     0,     0,    56,     0,
      59,     0,     0,    62,    65,     0,    10,    35,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    63,    13,    36,
      36,    36,    36,    14,    57,     0,    54,     0,    16,     0,
       0,    37,     0,     0,     0,     0,    55,     0,     0,    60,
      61,    29,    30,    32,    31,    66,    68,     0,     0,    67,
       0
  };

  const short
  parser::yypgoto_[] =
  {
    -134,  -134,   125,  -134,  -134,  -134,  -134,  -134,  -134,   -48,
     -23,  -105,  -134,  -134,  -134,   -52,  -133,   120,   128,   -26,
     -64,    31,  -134,  -134,  -127,  -134,  -134,    -7,    18,  -131
  };

  const short
  parser::yydefgoto_[] =
  {
      -1,     2,     9,    10,    11,    12,    13,    14,    15,   109,
      49,    40,    41,    42,    43,    72,   152,    47,    24,    61,
      85,    86,    87,    88,   119,   121,   123,   124,   106,   166
  };

  const unsigned char
  parser::yytable_[] =
  {
      65,    73,    74,    75,    50,   150,   144,   153,   154,   155,
      27,    28,    29,    30,    31,    32,   135,    33,    34,    35,
      36,    37,    38,   107,   108,    69,    70,     1,    68,   145,
     165,    81,    78,    39,    80,    71,   169,     3,    83,    82,
     101,     5,   113,   114,   115,   116,    84,   161,   162,   163,
     164,     4,   147,   100,    16,   102,    29,    30,    31,    32,
     148,    33,    34,    35,    36,    37,    38,   147,    17,    58,
      18,    22,    59,    23,    19,   168,    60,    39,     6,    21,
       7,    25,     8,    23,    26,    45,    46,    48,   149,    51,
      52,    62,    53,    54,    76,    55,    90,    56,    63,    64,
      66,    81,    67,    77,    79,    91,   111,   103,    99,   105,
      89,   125,    92,   120,   132,    93,   137,   117,    98,    94,
      95,    96,    97,   110,   104,   112,   118,   122,   128,   127,
     126,   139,   140,   141,   142,    20,   151,   129,   130,   131,
       0,   138,   134,   133,   143,   136,   146,     0,   156,   159,
      44,   157,     0,   158,     0,   160,   167,   170,     0,     0,
       0,     0,     0,     0,    57
  };

  const short
  parser::yycheck_[] =
  {
      48,    53,    54,    55,    27,   136,   133,   140,   141,   142,
      11,    12,    13,    14,    15,    16,   121,    18,    19,    20,
      21,    22,    23,    87,    88,    33,    34,     3,    51,   134,
     157,    30,    58,    34,    60,    43,   167,    34,    26,    62,
      39,    36,    94,    95,    96,    97,    34,   152,   153,   154,
     155,     0,    26,    79,    34,    81,    13,    14,    15,    16,
      34,    18,    19,    20,    21,    22,    23,    26,    34,    31,
      10,    27,    34,    29,     4,    34,    38,    34,     5,    37,
       7,    34,     9,    29,    36,    17,    28,    38,   136,    17,
      40,    17,    40,    40,     6,    40,    39,    36,    36,    34,
      36,    30,    36,    36,    35,     6,     6,    44,    77,    25,
      35,     8,    36,    24,     8,    38,   123,    99,    36,    41,
      41,    41,    41,    36,    41,    34,    34,    34,    42,    39,
      36,    17,    17,    17,    17,    10,    25,    42,    42,    42,
      -1,    36,    41,    45,    36,    41,    36,    -1,    36,    36,
      22,    44,    -1,    35,    -1,    36,    35,    35,    -1,    -1,
      -1,    -1,    -1,    -1,    44
  };

  const signed char
  parser::yystos_[] =
  {
       0,     3,    47,    34,     0,    36,     5,     7,     9,    48,
      49,    50,    51,    52,    53,    54,    34,    34,    10,     4,
      48,    37,    27,    29,    64,    34,    36,    11,    12,    13,
      14,    15,    16,    18,    19,    20,    21,    22,    23,    34,
      57,    58,    59,    60,    64,    17,    28,    63,    38,    56,
      56,    17,    40,    40,    40,    40,    36,    63,    31,    34,
      38,    65,    17,    36,    34,    55,    36,    36,    56,    33,
      34,    43,    61,    61,    61,    61,     6,    36,    65,    35,
      65,    30,    56,    26,    34,    66,    67,    68,    69,    35,
      39,     6,    36,    38,    41,    41,    41,    41,    36,    67,
      65,    39,    65,    44,    41,    25,    74,    66,    66,    55,
      36,     6,    34,    61,    61,    61,    61,    74,    34,    70,
      24,    71,    34,    72,    73,     8,    36,    39,    42,    42,
      42,    42,     8,    45,    41,    57,    41,    73,    36,    17,
      17,    17,    17,    36,    70,    57,    36,    26,    34,    55,
      75,    25,    62,    62,    62,    62,    36,    44,    35,    36,
      36,    57,    57,    57,    57,    70,    75,    35,    34,    75,
      35
  };

  const signed char
  parser::yyr1_[] =
  {
       0,    46,    47,    48,    48,    49,    49,    49,    49,    49,
      50,    51,    52,    53,    53,    54,    55,    55,    56,    57,
      57,    57,    58,    58,    58,    58,    58,    58,    59,    60,
      60,    60,    60,    61,    61,    61,    62,    62,    63,    63,
      64,    64,    64,    65,    65,    65,    65,    65,    66,    66,
      66,    66,    67,    67,    68,    69,    70,    70,    71,    71,
      72,    72,    73,    73,    74,    74,    75,    75,    75
  };

  const signed char
  parser::yyr2_[] =
  {
       0,     2,     6,     1,     2,     1,     1,     1,     1,     1,
       9,     8,     7,     9,    10,     5,     1,     3,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     9,
       9,     9,     9,     1,     1,     4,     0,     1,     0,     3,
       0,     1,     3,     1,     3,     3,     2,     3,     1,     2,
       1,     2,     0,     1,     5,     6,     1,     3,     0,     1,
       4,     4,     1,     2,     0,     2,     3,     5,     3
  };


#if YYDEBUG
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const parser::yytname_[] =
  {
  "\"end of file\"", "error", "\"invalid token\"", "KSCHEM", "KENDS",
  "KTYP", "KENDT", "KENT", "KENDE", "KREF", "KFROM", "KSEL", "KENUM",
  "KLIST", "KARR", "KBAG", "KSET", "KOF", "KNUM", "KINT", "KDBL", "KSTR",
  "KLOG", "KBOOL", "KOPT", "KUNIQ", "KSELF", "KABSTR", "KSUBT", "KSPRT",
  "KANDOR", "K1OF", "KAND", "NUMBER", "NAME", "','", "';'", "'='", "'('",
  "')'", "'['", "':'", "']'", "'?'", "'\\\\'", "'.'", "$accept", "SCHEMA",
  "ILIST", "ITEM", "ENUM", "SELECT", "ALIAS", "ENTITY", "REFERENCE",
  "TLIST", "TLIST1", "TYPE", "TSTD", "TNAME", "TSET", "INDEX", "OPTUNI",
  "SUBT", "SUPERT", "SUPLST", "FLIST", "FLIST1", "FIELD", "REDEF",
  "SPECIF", "OPTNL", "UNIQIT", "UNIQLS", "UNIQUE", "SPCLST", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const short
  parser::yyrline_[] =
  {
       0,   230,   230,   232,   233,   236,   237,   238,   239,   240,
     242,   244,   246,   248,   249,   252,   255,   256,   258,   261,
     262,   263,   265,   266,   267,   268,   269,   270,   272,   274,
     275,   276,   277,   279,   280,   281,   283,   284,   287,   288,
     290,   291,   292,   294,   295,   296,   297,   298,   301,   302,
     303,   304,   306,   307,   309,   311,   313,   314,   317,   318,
     321,   322,   324,   325,   327,   328,   330,   331,   332
  };

  void
  parser::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
  parser::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG

  parser::symbol_kind_type
  parser::yytranslate_ (int t)
  {
    // YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to
    // TOKEN-NUM as returned by yylex.
    static
    const signed char
    translate_table[] =
    {
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      38,    39,     2,     2,    35,     2,    45,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    41,    36,
       2,    37,     2,    43,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    40,    44,    42,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34
    };
    // Last valid token kind.
    const int code_max = 289;

    if (t <= 0)
      return symbol_kind::S_YYEOF;
    else if (t <= code_max)
      return YY_CAST (symbol_kind_type, translate_table[t]);
    else
      return symbol_kind::S_YYUNDEF;
  }

} // exptocas



/************************************************/
/* FUNCTIONS FOR CREATING SCHEMA REPRESENTATION */

static Express_Schema *mkschema ( char *name, Express_HSequenceOfItem *ilist )
{
  Express_Schema *sch = new Express_Schema ( name, ilist );
  Express::Schema() = sch;
  return sch;
}

static Express_HSequenceOfItem *mkilist ( Express_Item *item, Express_HSequenceOfItem *seq )
{
  if ( ! seq ) { 
    seq = new Express_HSequenceOfItem;
    seq->Append ( item );
  }
  else seq->Prepend ( item );
  return seq;
}

static Express_Item *mkenum ( char *name, TColStd_HSequenceOfHAsciiString *tlist )
{
  return new Express_Enum ( name, tlist );
}

static Express_Item *mkselect ( char *name, TColStd_HSequenceOfHAsciiString *tlist )
{
  return new Express_Select ( name, tlist );
}

static Express_Item *mkalias ( char *name, Express_Type *type )
{
  return new Express_Alias ( name, type );
}

static Express_Item *mkentity ( char *name, TColStd_HSequenceOfHAsciiString *inherit,
				 Express_HSequenceOfField *field, int isabstract )
{
  Express_Entity *ent = new Express_Entity ( name, inherit, field );
  if ( isabstract ) ent->SetAbstractFlag ( Standard_True );
  return ent;
}

static Express_Reference *mkrefs ( char *name, TColStd_HSequenceOfHAsciiString *items)
{
  return new Express_Reference ( name, items );
}

static TColStd_HSequenceOfHAsciiString *mktlist ( char *name, TColStd_HSequenceOfHAsciiString *tlist )
{
  Handle(TCollection_HAsciiString) str = new TCollection_HAsciiString ( name );
  if ( tlist ) tlist->Prepend ( str );
  else {
    tlist = new TColStd_HSequenceOfHAsciiString;
    tlist->Append ( str );
  }
  return tlist;
}

static TColStd_HSequenceOfHAsciiString *mktlists ( TColStd_HSequenceOfHAsciiString *tlist1, 
						   TColStd_HSequenceOfHAsciiString *tlist2 )
{
  if ( ! tlist1 ) return tlist2;
  if ( ! tlist2 ) return tlist1;
  for ( int i=1; i <= tlist2->Length(); i++ )
    tlist1->Append ( tlist2->Value(i) );
  return tlist1;
}

static Express_Type *mktstd ( int keyword )
{
  switch ( keyword ) {
  case exptocas::parser::token::KINT : return new Express_Integer;
  case exptocas::parser::token::KNUM : return new Express_Number;
  case exptocas::parser::token::KDBL : return new Express_Real;
  case exptocas::parser::token::KSTR : return new Express_String;
  case exptocas::parser::token::KBOOL: return new Express_Boolean;
  case exptocas::parser::token::KLOG : return new Express_Logical;
  default   : ec_error ( "Predefined type not treated!", "" );
    return NULL;
  }
}

static Express_Type *mktname ( char *name )
{
  return new Express_NamedType ( name );
}

static Express_Type *mktset ( int keyword, int ilow, int ihigh, Express_Type *of )
{
  switch ( keyword ) {
  case exptocas::parser::token::KLIST: return new Express_List  ( ilow, ihigh, of );
  case exptocas::parser::token::KARR : return new Express_Array ( ilow, ihigh, of );
  case exptocas::parser::token::KBAG : return new Express_Bag   ( ilow, ihigh, of );
  case exptocas::parser::token::KSET : return new Express_Set   ( ilow, ihigh, of );
  default   : ec_error ( "Complex type not treated!", "" );
    return NULL;
  }
}

static Express_Field *mkfield ( char *name, Express_Type *type, int optional )
{
  return new Express_Field ( name, type, optional );
}

static Express_HSequenceOfField *mkflist ( Express_Field *field, Express_HSequenceOfField *seq )
{
  if ( seq ) seq->Prepend ( field );
  else {
    seq = new Express_HSequenceOfField;
    seq->Append ( field );
  }
  return seq;
}

/*******************************************************************/
/* MAIN & co */

/*
void tlistfree ( struct tlist *list )
{
  if ( ! list ) return;
  tlistfree ( list->next );
  list->next = 0;
  free ( list->str );
  list->str = 0;
}
* /

int printtlist ( struct ec_tlist *tl )
{
  int num=0;
  while ( tl ) {
    num++;
    printf ( "%s\n", tl->name );
    tl = tl->next;
  }
  return num;
}

int main ( void )
{
  int num = 0;
  yyparse();

  printf ( "\nFinished\n" );
  if ( schema ) {
    struct ec_item *it;
    it = schema->items;
    printf ( "\nSchema %s", schema->name );
    printf ( "\nItems:" );
    while ( it ) {
      num++;
      printf ( "\n%s", it->name );
      it = it->next;
    }
//    num = printtlist ( res );
    printf ( "\nTotal %d", num );
  }

//  tlistfree ( yylval.tlist );
  return num;
}
*/

void exptocas::parser::error(const std::string& s)
{
  printf("\nParse error at line %d: %s\n", scanner->lineno(), s.c_str());
}

