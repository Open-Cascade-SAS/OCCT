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

#ifndef StepFile_recfile_HeaderFile
#define StepFile_recfile_HeaderFile

/*Types d'arguments (parametres) d'entites STEP (sans entrer dans le detail) */ 
#define rec_argSub     0
#define rec_argInteger 1
#define rec_argFloat   2
#define rec_argIdent   3
#define rec_argText    4
#define rec_argNondef  5
#define rec_argEnum    6
#define rec_argHexa    7
#define rec_argBinary  8
#define rec_argMisc    9

#ifdef __cplusplus

// Define stepFlexLexer class by inclusion of FlexLexer.h,
// but only if this has not been done yet, to avoid redefinition
#if !defined(yyFlexLexer) && !defined(FlexLexerOnce)
#define yyFlexLexer stepFlexLexer
#include <FlexLexer.h>
#endif

#include "step.tab.hxx"

namespace step
{
  // To feed data back to bison, the yylex method needs yylval and
  // yylloc parameters. Since the stepFlexLexer class is defined in the
  // system header <FlexLexer.h> the signature of its yylex() method
  // can not be changed anymore. This makes it necessary to derive a
  // scanner class that provides a method with the desired signature:

  class scanner : public stepFlexLexer
  {
  public:
    explicit scanner(std::istream* in = 0, std::ostream* out = 0);

    int lex(step::parser::semantic_type* /*yylval*/,
            step::parser::location_type* /*yylloc*/);
  };
}

#endif /* __cplusplus */

#endif // _StepFile_recfile_HeaderFile
