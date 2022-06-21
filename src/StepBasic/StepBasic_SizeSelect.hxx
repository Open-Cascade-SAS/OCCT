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

#ifndef _StepBasic_SizeSelect_HeaderFile
#define _StepBasic_SizeSelect_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepData_SelectMember;



class StepBasic_SizeSelect  : public StepData_SelectType
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns a SizeSelect SelectType
  Standard_EXPORT StepBasic_SizeSelect();
  
  //! Recognizes a TrimmingSelect Kind Entity that is :
  //! 1 -> SizeMember
  //! 0 else (i.e. Real)
  Standard_EXPORT Standard_Integer CaseNum (const Handle(Standard_Transient)& ent) const Standard_OVERRIDE;
  
  //! Returns a SizeMember (POSITIVE_LENGTH_MEASURE) as preferred
  Standard_EXPORT virtual Handle(StepData_SelectMember) NewMember() const Standard_OVERRIDE;
  
  //! Recognizes a SelectMember as Real, named as PARAMETER_VALUE
  //! 1 -> PositiveLengthMeasure i.e. Real
  //! 0 else (i.e. Entity)
  Standard_EXPORT virtual Standard_Integer CaseMem (const Handle(StepData_SelectMember)& ent) const Standard_OVERRIDE;
  
  Standard_EXPORT void SetRealValue (const Standard_Real aReal);
  
  //! returns Value as a Real (Null if another type)
  Standard_EXPORT Standard_Real RealValue() const;




protected:





private:





};







#endif // _StepBasic_SizeSelect_HeaderFile
