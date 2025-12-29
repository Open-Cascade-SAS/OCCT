// Created on: 1997-03-28
// Created by: Christian CAILLET
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _StepBasic_MeasureValueMember_HeaderFile
#define _StepBasic_MeasureValueMember_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <StepData_SelectReal.hxx>

//! for Select MeasureValue, i.e. :
//! length_measure,time_measure,plane_angle_measure,
//! solid_angle_measure,ratio_measure,parameter_value,
//! context_dependent_measure,positive_length_measure,
//! positive_plane_angle_measure,positive_ratio_measure,
//! area_measure,volume_measure, count_measure
class StepBasic_MeasureValueMember : public StepData_SelectReal
{

public:
  Standard_EXPORT StepBasic_MeasureValueMember();

  Standard_EXPORT bool HasName() const override;

  Standard_EXPORT const char* Name() const override;

  Standard_EXPORT bool SetName(const char* name) override;

  DEFINE_STANDARD_RTTIEXT(StepBasic_MeasureValueMember, StepData_SelectReal)

private:
  int thecase;
};

#endif // _StepBasic_MeasureValueMember_HeaderFile
