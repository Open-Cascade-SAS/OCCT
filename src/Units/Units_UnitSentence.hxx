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

#ifndef _Units_UnitSentence_HeaderFile
#define _Units_UnitSentence_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Units_Sentence.hxx>
#include <Standard_CString.hxx>
#include <Units_QuantitiesSequence.hxx>


//! This   class describes   all    the  facilities to
//! manipulate and compute units contained in a string
//! expression.
class Units_UnitSentence  : public Units_Sentence
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates   and   returns a   UnitSentence.   The string
  //! <astring> describes in natural  language the  unit  or
  //! the composed unit to be analysed.
  Standard_EXPORT Units_UnitSentence(const Standard_CString astring);
  
  //! Creates  and returns    a  UnitSentence.  The   string
  //! <astring> describes in natural language the unit to be
  //! analysed.   The    sequence     of physical quantities
  //! <asequenceofquantities>   describes    the   available
  //! dictionary of units you want to use.
  Standard_EXPORT Units_UnitSentence(const Standard_CString astring, const Handle(Units_QuantitiesSequence)& aquantitiessequence);
  
  //! Analyzes   the sequence  of   tokens  created  by  the
  //! constructor to  find  the true significance   of  each
  //! token.
  Standard_EXPORT void Analyse();
  
  //! For each token which  represents a unit, finds  in the
  //! sequence    of    physical   quantities      all   the
  //! characteristics of the unit found.
  Standard_EXPORT void SetUnits (const Handle(Units_QuantitiesSequence)& aquantitiessequence);




protected:





private:





};







#endif // _Units_UnitSentence_HeaderFile
