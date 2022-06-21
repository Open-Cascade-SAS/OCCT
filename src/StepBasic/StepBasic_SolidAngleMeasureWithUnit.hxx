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

#ifndef _StepBasic_SolidAngleMeasureWithUnit_HeaderFile
#define _StepBasic_SolidAngleMeasureWithUnit_HeaderFile

#include <Standard.hxx>

#include <StepBasic_MeasureWithUnit.hxx>


class StepBasic_SolidAngleMeasureWithUnit;
DEFINE_STANDARD_HANDLE(StepBasic_SolidAngleMeasureWithUnit, StepBasic_MeasureWithUnit)


class StepBasic_SolidAngleMeasureWithUnit : public StepBasic_MeasureWithUnit
{

public:

  
  //! Returns a SolidAngleMeasureWithUnit
  Standard_EXPORT StepBasic_SolidAngleMeasureWithUnit();




  DEFINE_STANDARD_RTTIEXT(StepBasic_SolidAngleMeasureWithUnit,StepBasic_MeasureWithUnit)

protected:




private:




};







#endif // _StepBasic_SolidAngleMeasureWithUnit_HeaderFile
