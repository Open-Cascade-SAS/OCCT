// A Bison parser, made by GNU Bison 3.7.4.

// Skeleton interface for Bison LALR(1) parsers in C++

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

/**
 ** \file ExpToCasExe/exptocas.tab.hxx
 ** Define the exptocas::parser class.
 */

// C++ LALR(1) parser skeleton written by Akim Demaille.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.

#ifndef YY_EXPTOCAS_EXPTOCASEXE_EXPTOCAS_TAB_HXX_INCLUDED
#define YY_EXPTOCAS_EXPTOCASEXE_EXPTOCAS_TAB_HXX_INCLUDED
// "%code requires" blocks.

#include <OSD_OpenFile.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <Express_HSequenceOfField.hxx>
#include <Express_HSequenceOfItem.hxx>
#include <Express_Field.hxx>
#include <Express_Item.hxx>
#include <Express_Enum.hxx>
#include <Express_Alias.hxx>
#include <Express_Select.hxx>
#include <Express_Entity.hxx>
#include <Express_Type.hxx>
#include <Express_NamedType.hxx>
#include <Express_PredefinedType.hxx>
#include <Express_Number.hxx>
#include <Express_Integer.hxx>
#include <Express_Boolean.hxx>
#include <Express_Logical.hxx>
#include <Express_Real.hxx>
#include <Express_String.hxx>
#include <Express_ComplexType.hxx>
#include <Express_Array.hxx>
#include <Express_List.hxx>
#include <Express_Set.hxx>
#include <Express_Bag.hxx>
#include <Express_Schema.hxx>
#include <Express_Reference.hxx>
#include <Express.hxx>

namespace exptocas
{
class scanner;
};
#ifdef _MSC_VER
  // disable MSVC warning C4522: 'exptocas::parser::stack_symbol_type': multiple assignment
  // operators
  #pragma warning(disable : 4522)
#endif

#include <cstdlib> // std::abort
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#if defined __cplusplus
  #define YY_CPLUSPLUS __cplusplus
#else
  #define YY_CPLUSPLUS 199711L
#endif

// Support move semantics when possible.
#if 201103L <= YY_CPLUSPLUS
  #define YY_MOVE std::move
  #define YY_MOVE_OR_COPY move
  #define YY_MOVE_REF(Type) Type&&
  #define YY_RVREF(Type) Type&&
  #define YY_COPY(Type) Type
#else
  #define YY_MOVE
  #define YY_MOVE_OR_COPY copy
  #define YY_MOVE_REF(Type) Type&
  #define YY_RVREF(Type) const Type&
  #define YY_COPY(Type) const Type&
#endif

// Support noexcept when possible.
#if 201103L <= YY_CPLUSPLUS
  #define YY_NOEXCEPT noexcept
  #define YY_NOTHROW
#else
  #define YY_NOEXCEPT
  #define YY_NOTHROW throw()
#endif

// Support constexpr when possible.
#if 201703 <= YY_CPLUSPLUS
  #define YY_CONSTEXPR constexpr
#else
  #define YY_CONSTEXPR
#endif

#ifndef YY_ATTRIBUTE_PURE
  #if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
    #define YY_ATTRIBUTE_PURE __attribute__((__pure__))
  #else
    #define YY_ATTRIBUTE_PURE
  #endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
  #if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
    #define YY_ATTRIBUTE_UNUSED __attribute__((__unused__))
  #else
    #define YY_ATTRIBUTE_UNUSED
  #endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if !defined lint || defined __GNUC__
  #define YYUSE(E) ((void)(E))
#else
  #define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && !defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
  /* Suppress an incorrect diagnostic about yylval being uninitialized.  */
  #define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                                                      \
    _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wuninitialized\"")           \
      _Pragma("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
  #define YY_IGNORE_MAYBE_UNINITIALIZED_END _Pragma("GCC diagnostic pop")
#else
  #define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  #define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  #define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
  #define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && !defined __ICC && 6 <= __GNUC__
  #define YY_IGNORE_USELESS_CAST_BEGIN                                                             \
    _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wuseless-cast\"")
  #define YY_IGNORE_USELESS_CAST_END _Pragma("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
  #define YY_IGNORE_USELESS_CAST_BEGIN
  #define YY_IGNORE_USELESS_CAST_END
#endif

#ifndef YY_CAST
  #ifdef __cplusplus
    #define YY_CAST(Type, Val) static_cast<Type>(Val)
    #define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type>(Val)
  #else
    #define YY_CAST(Type, Val) ((Type)(Val))
    #define YY_REINTERPRET_CAST(Type, Val) ((Type)(Val))
  #endif
#endif
#ifndef YY_NULLPTR
  #if defined __cplusplus
    #if 201103L <= __cplusplus
      #define YY_NULLPTR nullptr
    #else
      #define YY_NULLPTR 0
    #endif
  #else
    #define YY_NULLPTR ((void*)0)
  #endif
#endif

/* Debug traces.  */
#ifndef YYDEBUG
  #define YYDEBUG 0
#endif

namespace exptocas
{

/// A Bison parser.
class parser
{
public:
#ifndef YYSTYPE
  /// Symbol semantic values.
  union semantic_type {

    int                              num;
    char*                            str;
    TColStd_HSequenceOfHAsciiString* tlist;
    Express_HSequenceOfField*        flist;
    Express_HSequenceOfItem*         ilist;
    Express_Field*                   field;
    Express_Item*                    item;
    Express_Type*                    type;
    Express_Schema*                  schema;
    Express_Reference*               ref;
  };
#else
  typedef YYSTYPE semantic_type;
#endif

  /// Syntax errors thrown from user actions.
  struct syntax_error : std::runtime_error
  {
    syntax_error(const std::string& m)
        : std::runtime_error(m)
    {
    }

    syntax_error(const syntax_error& s)
        : std::runtime_error(s.what())
    {
    }

    ~syntax_error() YY_NOEXCEPT YY_NOTHROW;
  };

  /// Token kinds.
  struct token
  {
    enum token_kind_type
    {
      YYEMPTY = -2,
      YYEOF   = 0,   // "end of file"
      YYerror = 256, // error
      YYUNDEF = 257, // "invalid token"
      KSCHEM  = 258, // KSCHEM
      KENDS   = 259, // KENDS
      KTYP    = 260, // KTYP
      KENDT   = 261, // KENDT
      KENT    = 262, // KENT
      KENDE   = 263, // KENDE
      KREF    = 264, // KREF
      KFROM   = 265, // KFROM
      KSEL    = 266, // KSEL
      KENUM   = 267, // KENUM
      KLIST   = 268, // KLIST
      KARR    = 269, // KARR
      KBAG    = 270, // KBAG
      KSET    = 271, // KSET
      KOF     = 272, // KOF
      KNUM    = 273, // KNUM
      KINT    = 274, // KINT
      KDBL    = 275, // KDBL
      KSTR    = 276, // KSTR
      KLOG    = 277, // KLOG
      KBOOL   = 278, // KBOOL
      KOPT    = 279, // KOPT
      KUNIQ   = 280, // KUNIQ
      KSELF   = 281, // KSELF
      KABSTR  = 282, // KABSTR
      KSUBT   = 283, // KSUBT
      KSPRT   = 284, // KSPRT
      KANDOR  = 285, // KANDOR
      K1OF    = 286, // K1OF
      KAND    = 287, // KAND
      NUMBER  = 288, // NUMBER
      NAME    = 289  // NAME
    };

    /// Backward compatibility alias (Bison 3.6).
    typedef token_kind_type yytokentype;
  };

  /// Token kind, as returned by yylex.
  typedef token::yytokentype token_kind_type;

  /// Backward compatibility alias (Bison 3.6).
  typedef token_kind_type token_type;

  /// Symbol kinds.
  struct symbol_kind
  {
    enum symbol_kind_type
    {
      YYNTOKENS   = 46, ///< Number of tokens.
      S_YYEMPTY   = -2,
      S_YYEOF     = 0,  // "end of file"
      S_YYerror   = 1,  // error
      S_YYUNDEF   = 2,  // "invalid token"
      S_KSCHEM    = 3,  // KSCHEM
      S_KENDS     = 4,  // KENDS
      S_KTYP      = 5,  // KTYP
      S_KENDT     = 6,  // KENDT
      S_KENT      = 7,  // KENT
      S_KENDE     = 8,  // KENDE
      S_KREF      = 9,  // KREF
      S_KFROM     = 10, // KFROM
      S_KSEL      = 11, // KSEL
      S_KENUM     = 12, // KENUM
      S_KLIST     = 13, // KLIST
      S_KARR      = 14, // KARR
      S_KBAG      = 15, // KBAG
      S_KSET      = 16, // KSET
      S_KOF       = 17, // KOF
      S_KNUM      = 18, // KNUM
      S_KINT      = 19, // KINT
      S_KDBL      = 20, // KDBL
      S_KSTR      = 21, // KSTR
      S_KLOG      = 22, // KLOG
      S_KBOOL     = 23, // KBOOL
      S_KOPT      = 24, // KOPT
      S_KUNIQ     = 25, // KUNIQ
      S_KSELF     = 26, // KSELF
      S_KABSTR    = 27, // KABSTR
      S_KSUBT     = 28, // KSUBT
      S_KSPRT     = 29, // KSPRT
      S_KANDOR    = 30, // KANDOR
      S_K1OF      = 31, // K1OF
      S_KAND      = 32, // KAND
      S_NUMBER    = 33, // NUMBER
      S_NAME      = 34, // NAME
      S_35_       = 35, // ','
      S_36_       = 36, // ';'
      S_37_       = 37, // '='
      S_38_       = 38, // '('
      S_39_       = 39, // ')'
      S_40_       = 40, // '['
      S_41_       = 41, // ':'
      S_42_       = 42, // ']'
      S_43_       = 43, // '?'
      S_44_       = 44, // '\\'
      S_45_       = 45, // '.'
      S_YYACCEPT  = 46, // $accept
      S_SCHEMA    = 47, // SCHEMA
      S_ILIST     = 48, // ILIST
      S_ITEM      = 49, // ITEM
      S_ENUM      = 50, // ENUM
      S_SELECT    = 51, // SELECT
      S_ALIAS     = 52, // ALIAS
      S_ENTITY    = 53, // ENTITY
      S_REFERENCE = 54, // REFERENCE
      S_TLIST     = 55, // TLIST
      S_TLIST1    = 56, // TLIST1
      S_TYPE      = 57, // TYPE
      S_TSTD      = 58, // TSTD
      S_TNAME     = 59, // TNAME
      S_TSET      = 60, // TSET
      S_INDEX     = 61, // INDEX
      S_OPTUNI    = 62, // OPTUNI
      S_SUBT      = 63, // SUBT
      S_SUPERT    = 64, // SUPERT
      S_SUPLST    = 65, // SUPLST
      S_FLIST     = 66, // FLIST
      S_FLIST1    = 67, // FLIST1
      S_FIELD     = 68, // FIELD
      S_REDEF     = 69, // REDEF
      S_SPECIF    = 70, // SPECIF
      S_OPTNL     = 71, // OPTNL
      S_UNIQIT    = 72, // UNIQIT
      S_UNIQLS    = 73, // UNIQLS
      S_UNIQUE    = 74, // UNIQUE
      S_SPCLST    = 75  // SPCLST
    };
  };

  /// (Internal) symbol kind.
  typedef symbol_kind::symbol_kind_type symbol_kind_type;

  /// The number of tokens.
  static const symbol_kind_type YYNTOKENS = symbol_kind::YYNTOKENS;

  /// A complete symbol.
  ///
  /// Expects its Base type to provide access to the symbol kind
  /// via kind ().
  ///
  /// Provide access to semantic value.
  template <typename Base>
  struct basic_symbol : Base
  {
    /// Alias to Base.
    typedef Base super_type;

    /// Default constructor.
    basic_symbol()
        : value()
    {
    }

#if 201103L <= YY_CPLUSPLUS
    /// Move constructor.
    basic_symbol(basic_symbol&& that)
        : Base(std::move(that)),
          value(std::move(that.value))
    {
    }
#endif

    /// Copy constructor.
    basic_symbol(const basic_symbol& that);
    /// Constructor for valueless symbols.
    basic_symbol(typename Base::kind_type t);

    /// Constructor for symbols with semantic value.
    basic_symbol(typename Base::kind_type t, YY_RVREF(semantic_type) v);

    /// Destroy the symbol.
    ~basic_symbol() { clear(); }

    /// Destroy contents, and record that is empty.
    void clear() { Base::clear(); }

#if YYDEBUG || 0
    /// The user-facing name of this symbol.
    const char* name() const YY_NOEXCEPT { return parser::symbol_name(this->kind()); }
#endif // #if YYDEBUG || 0

    /// Backward compatibility (Bison 3.6).
    symbol_kind_type type_get() const YY_NOEXCEPT;

    /// Whether empty.
    bool empty() const YY_NOEXCEPT;

    /// Destructive move, \a s is emptied into this.
    void move(basic_symbol& s);

    /// The semantic value.
    semantic_type value;

  private:
#if YY_CPLUSPLUS < 201103L
    /// Assignment operator.
    basic_symbol& operator=(const basic_symbol& that);
#endif
  };

  /// Type access provider for token (enum) based symbols.
  struct by_kind
  {
    /// Default constructor.
    by_kind();

#if 201103L <= YY_CPLUSPLUS
    /// Move constructor.
    by_kind(by_kind&& that);
#endif

    /// Copy constructor.
    by_kind(const by_kind& that);

    /// The symbol kind as needed by the constructor.
    typedef token_kind_type kind_type;

    /// Constructor from (external) token numbers.
    by_kind(kind_type t);

    /// Record that this symbol is empty.
    void clear();

    /// Steal the symbol kind from \a that.
    void move(by_kind& that);

    /// The (internal) type number (corresponding to \a type).
    /// \a empty when empty.
    symbol_kind_type kind() const YY_NOEXCEPT;

    /// Backward compatibility (Bison 3.6).
    symbol_kind_type type_get() const YY_NOEXCEPT;

    /// The symbol kind.
    /// \a S_YYEMPTY when empty.
    symbol_kind_type kind_;
  };

  /// Backward compatibility for a private implementation detail (Bison 3.6).
  typedef by_kind by_type;

  /// "External" symbols: returned by the scanner.
  struct symbol_type : basic_symbol<by_kind>
  {
  };

  /// Build a parser object.
  parser(exptocas::scanner* scanner_yyarg);
  virtual ~parser();

#if 201103L <= YY_CPLUSPLUS
  /// Non copyable.
  parser(const parser&) = delete;
  /// Non copyable.
  parser& operator=(const parser&) = delete;
#endif

  /// Parse.  An alias for parse ().
  /// \returns  0 iff parsing succeeded.
  int operator()();

  /// Parse.
  /// \returns  0 iff parsing succeeded.
  virtual int parse();

#if YYDEBUG
  /// The current debugging stream.
  std::ostream& debug_stream() const YY_ATTRIBUTE_PURE;
  /// Set the current debugging stream.
  void set_debug_stream(std::ostream&);

  /// Type for debugging levels.
  typedef int debug_level_type;
  /// The current debugging level.
  debug_level_type debug_level() const YY_ATTRIBUTE_PURE;
  /// Set the current debugging level.
  void set_debug_level(debug_level_type l);
#endif

  /// Report a syntax error.
  /// \param msg    a description of the syntax error.
  virtual void error(const std::string& msg);

  /// Report a syntax error.
  void error(const syntax_error& err);

#if YYDEBUG || 0
  /// The user-facing name of the symbol whose (internal) number is
  /// YYSYMBOL.  No bounds checking.
  static const char* symbol_name(symbol_kind_type yysymbol);
#endif // #if YYDEBUG || 0

private:
#if YY_CPLUSPLUS < 201103L
  /// Non copyable.
  parser(const parser&);
  /// Non copyable.
  parser& operator=(const parser&);
#endif

  /// Stored state numbers (used for stacks).
  typedef unsigned char state_type;

  /// Compute post-reduction state.
  /// \param yystate   the current state
  /// \param yysym     the nonterminal to push on the stack
  static state_type yy_lr_goto_state_(state_type yystate, int yysym);

  /// Whether the given \c yypact_ value indicates a defaulted state.
  /// \param yyvalue   the value to check
  static bool yy_pact_value_is_default_(int yyvalue);

  /// Whether the given \c yytable_ value indicates a syntax error.
  /// \param yyvalue   the value to check
  static bool yy_table_value_is_error_(int yyvalue);

  static const short       yypact_ninf_;
  static const signed char yytable_ninf_;

  /// Convert a scanner token kind \a t to a symbol kind.
  /// In theory \a t should be a token_kind_type, but character literals
  /// are valid, yet not members of the token_type enum.
  static symbol_kind_type yytranslate_(int t);

#if YYDEBUG || 0
  /// For a symbol, its name in clear.
  static const char* const yytname_[];
#endif // #if YYDEBUG || 0

  // Tables.
  // YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
  // STATE-NUM.
  static const short yypact_[];

  // YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
  // Performed when YYTABLE does not specify something else to do.  Zero
  // means the default is an error.
  static const signed char yydefact_[];

  // YYPGOTO[NTERM-NUM].
  static const short yypgoto_[];

  // YYDEFGOTO[NTERM-NUM].
  static const short yydefgoto_[];

  // YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
  // positive, shift that token.  If negative, reduce the rule whose
  // number is the opposite.  If YYTABLE_NINF, syntax error.
  static const unsigned char yytable_[];

  static const short yycheck_[];

  // YYSTOS[STATE-NUM] -- The (internal number of the) accessing
  // symbol of state STATE-NUM.
  static const signed char yystos_[];

  // YYR1[YYN] -- Symbol number of symbol that rule YYN derives.
  static const signed char yyr1_[];

  // YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.
  static const signed char yyr2_[];

#if YYDEBUG
  // YYRLINE[YYN] -- Source line where rule number YYN was defined.
  static const short yyrline_[];
  /// Report on the debug stream that the rule \a r is going to be reduced.
  virtual void yy_reduce_print_(int r) const;
  /// Print the state stack on the debug stream.
  virtual void yy_stack_print_() const;

  /// Debugging level.
  int yydebug_;
  /// Debug stream.
  std::ostream* yycdebug_;

  /// \brief Display a symbol kind, value and location.
  /// \param yyo    The output stream.
  /// \param yysym  The symbol.
  template <typename Base>
  void yy_print_(std::ostream& yyo, const basic_symbol<Base>& yysym) const;
#endif

  /// \brief Reclaim the memory associated to a symbol.
  /// \param yymsg     Why this token is reclaimed.
  ///                  If null, print nothing.
  /// \param yysym     The symbol.
  template <typename Base>
  void yy_destroy_(const char* yymsg, basic_symbol<Base>& yysym) const;

private:
  /// Type access provider for state based symbols.
  struct by_state
  {
    /// Default constructor.
    by_state() YY_NOEXCEPT;

    /// The symbol kind as needed by the constructor.
    typedef state_type kind_type;

    /// Constructor.
    by_state(kind_type s) YY_NOEXCEPT;

    /// Copy constructor.
    by_state(const by_state& that) YY_NOEXCEPT;

    /// Record that this symbol is empty.
    void clear() YY_NOEXCEPT;

    /// Steal the symbol kind from \a that.
    void move(by_state& that);

    /// The symbol kind (corresponding to \a state).
    /// \a symbol_kind::S_YYEMPTY when empty.
    symbol_kind_type kind() const YY_NOEXCEPT;

    /// The state number used to denote an empty symbol.
    /// We use the initial state, as it does not have a value.
    enum
    {
      empty_state = 0
    };

    /// The state.
    /// \a empty when empty.
    state_type state;
  };

  /// "Internal" symbol: element of the stack.
  struct stack_symbol_type : basic_symbol<by_state>
  {
    /// Superclass.
    typedef basic_symbol<by_state> super_type;
    /// Construct an empty symbol.
    stack_symbol_type();
    /// Move or copy construction.
    stack_symbol_type(YY_RVREF(stack_symbol_type) that);
    /// Steal the contents from \a sym to build this.
    stack_symbol_type(state_type s, YY_MOVE_REF(symbol_type) sym);
#if YY_CPLUSPLUS < 201103L
    /// Assignment, needed by push_back by some old implementations.
    /// Moves the contents of that.
    stack_symbol_type& operator=(stack_symbol_type& that);

    /// Assignment, needed by push_back by other implementations.
    /// Needed by some other old implementations.
    stack_symbol_type& operator=(const stack_symbol_type& that);
#endif
  };

  /// A stack with random access from its top.
  template <typename T, typename S = std::vector<T>>
  class stack
  {
  public:
    // Hide our reversed order.
    typedef typename S::iterator       iterator;
    typedef typename S::const_iterator const_iterator;
    typedef typename S::size_type      size_type;
    typedef typename std::ptrdiff_t    index_type;

    stack(size_type n = 200)
        : seq_(n)
    {
    }

#if 201103L <= YY_CPLUSPLUS
    /// Non copyable.
    stack(const stack&) = delete;
    /// Non copyable.
    stack& operator=(const stack&) = delete;
#endif

    /// Random access.
    ///
    /// Index 0 returns the topmost element.
    const T& operator[](index_type i) const { return seq_[size_type(size() - 1 - i)]; }

    /// Random access.
    ///
    /// Index 0 returns the topmost element.
    T& operator[](index_type i) { return seq_[size_type(size() - 1 - i)]; }

    /// Steal the contents of \a t.
    ///
    /// Close to move-semantics.
    void push(YY_MOVE_REF(T) t)
    {
      seq_.push_back(T());
      operator[](0).move(t);
    }

    /// Pop elements from the stack.
    void pop(std::ptrdiff_t n = 1) YY_NOEXCEPT
    {
      for (; 0 < n; --n)
        seq_.pop_back();
    }

    /// Pop all elements from the stack.
    void clear() YY_NOEXCEPT { seq_.clear(); }

    /// Number of elements on the stack.
    index_type size() const YY_NOEXCEPT { return index_type(seq_.size()); }

    /// Iterator on top of the stack (going downwards).
    const_iterator begin() const YY_NOEXCEPT { return seq_.begin(); }

    /// Bottom of the stack.
    const_iterator end() const YY_NOEXCEPT { return seq_.end(); }

    /// Present a slice of the top of a stack.
    class slice
    {
    public:
      slice(const stack& stack, index_type range)
          : stack_(stack),
            range_(range)
      {
      }

      const T& operator[](index_type i) const { return stack_[range_ - i]; }

    private:
      const stack& stack_;
      index_type   range_;
    };

  private:
#if YY_CPLUSPLUS < 201103L
    /// Non copyable.
    stack(const stack&);
    /// Non copyable.
    stack& operator=(const stack&);
#endif
    /// The wrapped container.
    S seq_;
  };

  /// Stack type.
  typedef stack<stack_symbol_type> stack_type;

  /// The stack.
  stack_type yystack_;

  /// Push a new state on the stack.
  /// \param m    a debug message to display
  ///             if null, no trace is output.
  /// \param sym  the symbol
  /// \warning the contents of \a s.value is stolen.
  void yypush_(const char* m, YY_MOVE_REF(stack_symbol_type) sym);

  /// Push a new look ahead token on the state on the stack.
  /// \param m    a debug message to display
  ///             if null, no trace is output.
  /// \param s    the state
  /// \param sym  the symbol (for its value and location).
  /// \warning the contents of \a sym.value is stolen.
  void yypush_(const char* m, state_type s, YY_MOVE_REF(symbol_type) sym);

  /// Pop \a n symbols from the stack.
  void yypop_(int n = 1);

  /// Constants.
  enum
  {
    yylast_  = 164, ///< Last index in yytable_.
    yynnts_  = 30,  ///< Number of nonterminal symbols.
    yyfinal_ = 4    ///< Termination state number.
  };

  // User arguments.
  exptocas::scanner* scanner;
};

} // namespace exptocas

// "%code provides" blocks.

#if !defined(yyFlexLexer) && !defined(FlexLexerOnce)
  #define yyFlexLexer exptocasFlexLexer
  #include "FlexLexer.h"
#endif
namespace exptocas
{
// To feed data back to bison, the yylex method needs yylval and
// yylloc parameters. Since the exptocasFlexLexer class is defined in the
// system header <FlexLexer.h> the signature of its yylex() method
// can not be changed anymore. This makes it necessary to derive a
// scanner class that provides a method with the desired signature:
class scanner : public exptocasFlexLexer
{
public:
  explicit scanner(std::istream* in = 0, std::ostream* out = 0);

  int lex(exptocas::parser::semantic_type* yylval);
};
}; // namespace exptocas

#endif // !YY_EXPTOCAS_EXPTOCASEXE_EXPTOCAS_TAB_HXX_INCLUDED
