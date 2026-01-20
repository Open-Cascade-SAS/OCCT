// Created on: 1992-06-22
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

#ifndef _Units_Sentence_HeaderFile
#define _Units_Sentence_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Units_Token.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <Standard_CString.hxx>
class Units_Lexicon;
class Units_Token;

//! This class describes all the methods to create and
//! compute an expression contained in a string.
class Units_Sentence
{
public:
  DEFINE_STANDARD_ALLOC

  //! Createsand returns a Sentence, by analyzing the
  //! string <astring> with the lexicon <alexicon>.
  Standard_EXPORT Units_Sentence(const occ::handle<Units_Lexicon>& alexicon,
                                 const char* const       astring);

  //! For each constant encountered, sets the value.
  Standard_EXPORT void SetConstants();

  //! Returns <thesequenceoftokens>.
  occ::handle<NCollection_HSequence<occ::handle<Units_Token>>> Sequence() const;

  //! Sets the field <thesequenceoftokens> to <asequenceoftokens>.
  void Sequence(const occ::handle<NCollection_HSequence<occ::handle<Units_Token>>>& asequenceoftokens);

  //! Computes and returns in a token the result of the
  //! expression.
  Standard_EXPORT occ::handle<Units_Token> Evaluate();

  //! Return True if number of created tokens > 0
  //! (i.e creation of sentence is successful)
  bool IsDone() const;

  //! Useful for debugging.
  void Dump() const;

private:
  occ::handle<NCollection_HSequence<occ::handle<Units_Token>>> thesequenceoftokens;
};

#include <Units_Sentence.lxx>

#endif // _Units_Sentence_HeaderFile
