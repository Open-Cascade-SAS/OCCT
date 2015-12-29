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

#ifndef _StepBasic_CalendarDate_HeaderFile
#define _StepBasic_CalendarDate_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <StepBasic_Date.hxx>


class StepBasic_CalendarDate;
DEFINE_STANDARD_HANDLE(StepBasic_CalendarDate, StepBasic_Date)


class StepBasic_CalendarDate : public StepBasic_Date
{

public:

  
  //! Returns a CalendarDate
  Standard_EXPORT StepBasic_CalendarDate();
  
  Standard_EXPORT void Init (const Standard_Integer aYearComponent, const Standard_Integer aDayComponent, const Standard_Integer aMonthComponent);
  
  Standard_EXPORT void SetDayComponent (const Standard_Integer aDayComponent);
  
  Standard_EXPORT Standard_Integer DayComponent() const;
  
  Standard_EXPORT void SetMonthComponent (const Standard_Integer aMonthComponent);
  
  Standard_EXPORT Standard_Integer MonthComponent() const;




  DEFINE_STANDARD_RTTIEXT(StepBasic_CalendarDate,StepBasic_Date)

protected:




private:


  Standard_Integer dayComponent;
  Standard_Integer monthComponent;


};







#endif // _StepBasic_CalendarDate_HeaderFile
