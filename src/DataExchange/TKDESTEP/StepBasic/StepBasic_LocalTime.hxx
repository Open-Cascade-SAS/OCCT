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

class StepBasic_LocalTime;
DEFINE_STANDARD_HANDLE(StepBasic_LocalTime, Standard_Transient)

class StepBasic_LocalTime : public Standard_Transient
{

public:
  //! Returns a LocalTime
  Standard_EXPORT StepBasic_LocalTime();

  Standard_EXPORT void Init(const Standard_Integer aHourComponent,
                            const Standard_Boolean hasAminuteComponent,
                            const Standard_Integer aMinuteComponent,
                            const Standard_Boolean hasAsecondComponent,
                            const Standard_Real    aSecondComponent,
                            const Handle(StepBasic_CoordinatedUniversalTimeOffset)& aZone);

  Standard_EXPORT void SetHourComponent(const Standard_Integer aHourComponent);

  Standard_EXPORT Standard_Integer HourComponent() const;

  Standard_EXPORT void SetMinuteComponent(const Standard_Integer aMinuteComponent);

  Standard_EXPORT void UnSetMinuteComponent();

  Standard_EXPORT Standard_Integer MinuteComponent() const;

  Standard_EXPORT Standard_Boolean HasMinuteComponent() const;

  Standard_EXPORT void SetSecondComponent(const Standard_Real aSecondComponent);

  Standard_EXPORT void UnSetSecondComponent();

  Standard_EXPORT Standard_Real SecondComponent() const;

  Standard_EXPORT Standard_Boolean HasSecondComponent() const;

  Standard_EXPORT void SetZone(const Handle(StepBasic_CoordinatedUniversalTimeOffset)& aZone);

  Standard_EXPORT Handle(StepBasic_CoordinatedUniversalTimeOffset) Zone() const;

  DEFINE_STANDARD_RTTIEXT(StepBasic_LocalTime, Standard_Transient)

protected:
private:
  Standard_Integer                                 hourComponent;
  Standard_Integer                                 minuteComponent;
  Standard_Real                                    secondComponent;
  Handle(StepBasic_CoordinatedUniversalTimeOffset) zone;
  Standard_Boolean                                 hasMinuteComponent;
  Standard_Boolean                                 hasSecondComponent;
};

#endif // _StepBasic_LocalTime_HeaderFile
