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

#ifndef _StepBasic_WeekOfYearAndDayDate_HeaderFile
#define _StepBasic_WeekOfYearAndDayDate_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <StepBasic_Date.hxx>

class StepBasic_WeekOfYearAndDayDate : public StepBasic_Date
{

public:
  //! Returns a WeekOfYearAndDayDate
  Standard_EXPORT StepBasic_WeekOfYearAndDayDate();

  Standard_EXPORT void Init(const int  aYearComponent,
                            const int  aWeekComponent,
                            const bool hasAdayComponent,
                            const int  aDayComponent);

  Standard_EXPORT void SetWeekComponent(const int aWeekComponent);

  Standard_EXPORT int WeekComponent() const;

  Standard_EXPORT void SetDayComponent(const int aDayComponent);

  Standard_EXPORT void UnSetDayComponent();

  Standard_EXPORT int DayComponent() const;

  Standard_EXPORT bool HasDayComponent() const;

  DEFINE_STANDARD_RTTIEXT(StepBasic_WeekOfYearAndDayDate, StepBasic_Date)

private:
  int  weekComponent;
  int  dayComponent;
  bool hasDayComponent;
};

#endif // _StepBasic_WeekOfYearAndDayDate_HeaderFile
