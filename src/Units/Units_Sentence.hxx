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

#include <Units_TokensSequence.hxx>
#include <Standard_CString.hxx>
#include <Standard_Boolean.hxx>
class Units_Lexicon;
class Units_Token;


//! This class describes all the methods to create and
//! compute an expression contained in a string.
class Units_Sentence 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates and  returns  a   Sentence, by  analyzing  the
  //! string <astring> with the lexicon <alexicon>.
  Standard_EXPORT Units_Sentence(const Handle(Units_Lexicon)& alexicon, const Standard_CString astring);
  
  //! For each constant encountered, sets the value.
  Standard_EXPORT void SetConstants();
  
  //! Returns <thesequenceoftokens>.
    Handle(Units_TokensSequence) Sequence() const;
  
  //! Sets the field <thesequenceoftokens> to <asequenceoftokens>.
    void Sequence (const Handle(Units_TokensSequence)& asequenceoftokens);
  
  //! Computes and  returns in a   token the result  of  the
  //! expression.
  Standard_EXPORT Handle(Units_Token) Evaluate();
  
  //! Return True if number of created tokens > 0
  //! (i.e creation of sentence is successful)
    Standard_Boolean IsDone() const;
  
  //! Useful for debugging.
    void Dump() const;




protected:





private:



  Handle(Units_TokensSequence) thesequenceoftokens;


};


#include <Units_Sentence.lxx>





#endif // _Units_Sentence_HeaderFile
