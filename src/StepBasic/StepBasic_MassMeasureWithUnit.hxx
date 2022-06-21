// Created on: 2004-02-11
// Created by: Sergey KUUL
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

#ifndef _StepBasic_MassMeasureWithUnit_HeaderFile
#define _StepBasic_MassMeasureWithUnit_HeaderFile

#include <Standard.hxx>

#include <StepBasic_MeasureWithUnit.hxx>


class StepBasic_MassMeasureWithUnit;
DEFINE_STANDARD_HANDLE(StepBasic_MassMeasureWithUnit, StepBasic_MeasureWithUnit)


class StepBasic_MassMeasureWithUnit : public StepBasic_MeasureWithUnit
{

public:

  
  //! Returns a MassMeasureWithUnit
  Standard_EXPORT StepBasic_MassMeasureWithUnit();




  DEFINE_STANDARD_RTTIEXT(StepBasic_MassMeasureWithUnit,StepBasic_MeasureWithUnit)

protected:




private:




};







#endif // _StepBasic_MassMeasureWithUnit_HeaderFile
