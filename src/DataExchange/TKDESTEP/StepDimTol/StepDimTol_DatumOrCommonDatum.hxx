// Created on: 2015-07-16
// Created by: Irina KRYLOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _StepDimTol_DatumOrCommonDatum_HeaderFile
#define _StepDimTol_DatumOrCommonDatum_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <StepData_SelectType.hxx>

class Standard_Transient;
class StepDimTol_Datum;
class StepDimTol_HArray1OfDatumReferenceElement;

class StepDimTol_DatumOrCommonDatum : public StepData_SelectType
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns a DatumOrCommonDatum select type
  Standard_EXPORT StepDimTol_DatumOrCommonDatum();

  //! Recognizes a DatumOrCommonDatum Kind Entity that is :
  //! 1 -> Datum
  //! 2 -> CommonDatumList
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum(const Handle(Standard_Transient)& ent) const;

  //! returns Value as a Datum (Null if another type)
  Standard_EXPORT Handle(StepDimTol_Datum) Datum() const;

  //! returns Value as a CommonDatumList  (Null if another type)
  Standard_EXPORT Handle(StepDimTol_HArray1OfDatumReferenceElement) CommonDatumList() const;
};
#endif // _StepDimTol_DatumOrCommonDatum_HeaderFile
