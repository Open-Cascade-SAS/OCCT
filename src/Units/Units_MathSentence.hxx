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

#ifndef _Units_MathSentence_HeaderFile
#define _Units_MathSentence_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Units_Sentence.hxx>


//! This class  defines all the methods to  create and
//! compute an algebraic formula.
class Units_MathSentence  : public Units_Sentence
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates and returns a  MathSentence object. The string
  //! <astring>  describes  an algebraic  formula in natural
  //! language.
  Standard_EXPORT Units_MathSentence(const Standard_CString astring);




protected:





private:





};







#endif // _Units_MathSentence_HeaderFile
