// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _StepVisual_TextOrCharacter_HeaderFile
#define _StepVisual_TextOrCharacter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepVisual_AnnotationText;
class StepVisual_CompositeText;
class StepVisual_TextLiteral;



class StepVisual_TextOrCharacter  : public StepData_SelectType
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns a TextOrCharacter SelectType
  Standard_EXPORT StepVisual_TextOrCharacter();
  
  //! Recognizes a TextOrCharacter Kind Entity that is :
  //! 1 -> AnnotationText
  //! 2 -> CompositeText
  //! 3 -> TextLiteral
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum (const Handle(Standard_Transient)& ent) const;
  
  //! returns Value as a AnnotationText (Null if another type)
  Standard_EXPORT Handle(StepVisual_AnnotationText) AnnotationText() const;
  
  //! returns Value as a CompositeText (Null if another type)
  Standard_EXPORT Handle(StepVisual_CompositeText) CompositeText() const;
  
  //! returns Value as a TextLiteral (Null if another type)
  Standard_EXPORT Handle(StepVisual_TextLiteral) TextLiteral() const;




protected:





private:





};







#endif // _StepVisual_TextOrCharacter_HeaderFile
