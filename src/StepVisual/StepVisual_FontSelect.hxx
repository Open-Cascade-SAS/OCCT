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

#ifndef _StepVisual_FontSelect_HeaderFile
#define _StepVisual_FontSelect_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepVisual_PreDefinedTextFont;
class StepVisual_ExternallyDefinedTextFont;

class StepVisual_FontSelect : public StepData_SelectType
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns a FontSelect SelectType
  Standard_EXPORT StepVisual_FontSelect();

  //! Recognizes a FontSelect Kind Entity that is :
  //! 1 -> PreDefinedTextFont
  //! 2 -> ExternallyDefinedTextFont
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum(const Handle(Standard_Transient)& ent) const;

  //! returns Value as a PreDefinedTextFont (Null if another type)
  Standard_EXPORT Handle(StepVisual_PreDefinedTextFont) PreDefinedTextFont() const;

  //! returns Value as a ExternallyDefinedTextFont (Null if another type)
  Standard_EXPORT Handle(StepVisual_ExternallyDefinedTextFont) ExternallyDefinedTextFont() const;

protected:
private:
};

#endif // _StepVisual_FontSelect_HeaderFile
