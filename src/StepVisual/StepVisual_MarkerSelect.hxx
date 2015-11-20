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

#ifndef _StepVisual_MarkerSelect_HeaderFile
#define _StepVisual_MarkerSelect_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepData_SelectMember;
class StepVisual_MarkerMember;



class StepVisual_MarkerSelect  : public StepData_SelectType
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns a MarkerSelect SelectType
  Standard_EXPORT StepVisual_MarkerSelect();
  
  //! Recognizes a MarkerSelect Kind Entity that is :
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum (const Handle(Standard_Transient)& ent) const Standard_OVERRIDE;
  
  //! Returns a new MarkerMember
  Standard_EXPORT virtual Handle(StepData_SelectMember) NewMember() const Standard_OVERRIDE;
  
  //! Returns 1 for a SelectMember enum, named MARKER_TYPE
  Standard_EXPORT virtual Standard_Integer CaseMem (const Handle(StepData_SelectMember)& sm) const Standard_OVERRIDE;
  
  //! Gives access to the MarkerMember in order to get/set its value
  Standard_EXPORT Handle(StepVisual_MarkerMember) MarkerMember() const;




protected:





private:





};







#endif // _StepVisual_MarkerSelect_HeaderFile
