// Created on: 1992-06-24
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <Units_Lexicon.hxx>

#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Units_Token.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Units_Lexicon,Standard_Transient)

namespace
{

  //! Lexicon item
  struct LexiconItem
  {
    char   Prefix[10];   //!< prefix or symbol (e.g. "k" for kilo)
    char   Operation[2]; //!< operation
    double Value;        //!< numeric parameter (e.g. multiplier)
  };

  //! Lexicon table.
  //!
  //! Original table (UnitsAPI/Lexi_Expr.dat) used symbols from extended ASCII,
  //! which should not be used within UTF-8 text.
  //!
  //! This table preserves these codes for compatibility.
  //! UTF-8 items might be uncommented after updating UnitsAPI/Units.dat
  //! and analysis of further consequences.
  static const LexiconItem THE_LEXICON[] =
  {
    // scope
    {         "(", "S", 0.0 },
    {         ")", "S", 0.0 },
    // operators
    {         "+", "O", 0.0 },
    {         "-", "O", 0.0 },
    {         "*", "O", 0.0 },
    {         ".", "O", 0.0 },
    {         "/", "O", 0.0 },
    {        "**", "O", 0.0 },
    // ^2, power of two
    {      "\xB2", "P", 2.0 }, // ISO 8859-1/ISO Latin-1 (extended ASCII)
    //{  "\xC2\xB2", "P", 2.0 }, // UTF-8
    {        "p2", "P", 2.0 },
    {       "sq.", "P", 2.0 },
    // ^3, power of three
    {      "\xB3", "P", 3.0 }, // ISO 8859-1/ISO Latin-1 (extended ASCII)
    //{  "\xC2\xB3", "P", 3.0 }, // UTF-8
    {       "cu.", "P", 3.0 },
    // multipliers
    {         "y", "M", 1.E-24 }, // yocto
    {         "z", "M", 1.E-21 }, // zepto
    {         "a", "M", 1.E-18 }, // atto
    {         "f", "M", 1.E-15 }, // femto
    {         "p", "M", 1.E-12 }, // pico
    {         "n", "M", 1.E-09 }, // nano
    {      "\xB5", "M", 1.E-06 }, // micro, ISO 8859-1/ISO Latin-1 (extended ASCII)
    //{  "\xC2\xB5", "M", 1.E-06 }, // micro, UTF-8
    {         "m", "M", 1.E-03 }, // milli
    {         "c", "M", 1.E-02 }, // centi
    {         "d", "M", 1.E-01 }, // deci
    {        "da", "M", 1.E+01 }, // deca
    {         "h", "M", 1.E+02 }, // hecto
    {         "k", "M", 1.E+03 }, // kilo
    {         "M", "M", 1.E+06 }, // mega
    {         "G", "M", 1.E+09 }, // giga
    {         "T", "M", 1.E+12 }, // tera
    {         "P", "M", 1.E+15 }, // peta
    {         "E", "M", 1.E+18 }, // exa
    {         "Z", "M", 1.E+21 }, // zetta
    {         "Y", "M", 1.E+24 }, // yotta
    // Pi constant
    {       "\xB6", "",  M_PI }, // Pilcrow sign, ISO 8859-1/ISO Latin-1 (extended ASCII)
    //{   "\xCF\x80", "",  M_PI }, // UTF-8
    {         "Pi", "",  M_PI },
  };

}

//=======================================================================
//function : Units_Lexicon
//purpose  : 
//=======================================================================

Units_Lexicon::Units_Lexicon()
{
}


//=======================================================================
//function : Creates
//purpose  : 
//=======================================================================

void Units_Lexicon::Creates()
{
  thesequenceoftokens = new Units_TokensSequence();

  const Standard_Integer aNbLexiItems = sizeof(THE_LEXICON) / sizeof(LexiconItem);
  for (Standard_Integer anItemIter = 0; anItemIter < aNbLexiItems; ++anItemIter)
  {
    const LexiconItem& anItem = THE_LEXICON[anItemIter];
    if (thesequenceoftokens->IsEmpty())
    {
      Handle(Units_Token) aToken = new Units_Token (anItem.Prefix, anItem.Operation, anItem.Value);
      thesequenceoftokens->Prepend (aToken);
    }
    else
    {
      AddToken (anItem.Prefix, anItem.Operation, anItem.Value);
    }
  }
}

//=======================================================================
//function : AddToken
//purpose  : 
//=======================================================================

void Units_Lexicon::AddToken(const Standard_CString aword,
			     const Standard_CString amean,
			     const Standard_Real avalue)
{
  Handle(Units_Token) token;
  Handle(Units_Token) referencetoken;
  Standard_Boolean found = Standard_False;
  Standard_Integer index;

  for(index=1;index<=thesequenceoftokens->Length();index++) {
    referencetoken = thesequenceoftokens->Value(index);
    if( referencetoken->Word() == aword ) {
      referencetoken->Update(amean);
      found = Standard_True;
      break;
    }
    else if( !( referencetoken->Word()>aword ) ) {
      token = new Units_Token(aword,amean,avalue);
      thesequenceoftokens->InsertBefore(index,token);
      found = Standard_True;
      break;
    }
  }
  if(!found) {
    token = new Units_Token(aword,amean,avalue);
    thesequenceoftokens->Append(token);
  }
}
