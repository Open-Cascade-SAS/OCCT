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

#ifndef _StepBasic_LocalTime_HeaderFile
#define _StepBasic_LocalTime_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
class StepBasic_CoordinatedUniversalTimeOffset;

class StepBasic_LocalTime : public Standard_Transient
{

public:
  //! Returns a LocalTime
  Standard_EXPORT StepBasic_LocalTime();

  Standard_EXPORT void Init(const int aHourComponent,
                            const bool hasAminuteComponent,
                            const int aMinuteComponent,
                            const bool hasAsecondComponent,
                            const double    aSecondComponent,
                            const occ::handle<StepBasic_CoordinatedUniversalTimeOffset>& aZone);

  Standard_EXPORT void SetHourComponent(const int aHourComponent);

  Standard_EXPORT int HourComponent() const;

  Standard_EXPORT void SetMinuteComponent(const int aMinuteComponent);

  Standard_EXPORT void UnSetMinuteComponent();

  Standard_EXPORT int MinuteComponent() const;

  Standard_EXPORT bool HasMinuteComponent() const;

  Standard_EXPORT void SetSecondComponent(const double aSecondComponent);

  Standard_EXPORT void UnSetSecondComponent();

  Standard_EXPORT double SecondComponent() const;

  Standard_EXPORT bool HasSecondComponent() const;

  Standard_EXPORT void SetZone(const occ::handle<StepBasic_CoordinatedUniversalTimeOffset>& aZone);

  Standard_EXPORT occ::handle<StepBasic_CoordinatedUniversalTimeOffset> Zone() const;

  DEFINE_STANDARD_RTTIEXT(StepBasic_LocalTime, Standard_Transient)

private:
  int                                 hourComponent;
  int                                 minuteComponent;
  double                                    secondComponent;
  occ::handle<StepBasic_CoordinatedUniversalTimeOffset> zone;
  bool                                 hasMinuteComponent;
  bool                                 hasSecondComponent;
};

#endif // _StepBasic_LocalTime_HeaderFile
