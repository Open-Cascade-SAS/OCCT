// Created on: 2001-04-24
// Created by: Atelier IED
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef _StepShape_LimitsAndFits_HeaderFile
#define _StepShape_LimitsAndFits_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class TCollection_HAsciiString;


class StepShape_LimitsAndFits;
DEFINE_STANDARD_HANDLE(StepShape_LimitsAndFits, Standard_Transient)

//! Added for Dimensional Tolerances
class StepShape_LimitsAndFits : public Standard_Transient
{

public:

  
  Standard_EXPORT StepShape_LimitsAndFits();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& form_variance, const Handle(TCollection_HAsciiString)& zone_variance, const Handle(TCollection_HAsciiString)& grade, const Handle(TCollection_HAsciiString)& source);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) FormVariance() const;
  
  Standard_EXPORT void SetFormVariance (const Handle(TCollection_HAsciiString)& form_variance);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) ZoneVariance() const;
  
  Standard_EXPORT void SetZoneVariance (const Handle(TCollection_HAsciiString)& zone_variance);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) Grade() const;
  
  Standard_EXPORT void SetGrade (const Handle(TCollection_HAsciiString)& grade);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) Source() const;
  
  Standard_EXPORT void SetSource (const Handle(TCollection_HAsciiString)& source);




  DEFINE_STANDARD_RTTIEXT(StepShape_LimitsAndFits,Standard_Transient)

protected:




private:


  Handle(TCollection_HAsciiString) theFormVariance;
  Handle(TCollection_HAsciiString) theZoneVariance;
  Handle(TCollection_HAsciiString) theGrade;
  Handle(TCollection_HAsciiString) theSource;


};







#endif // _StepShape_LimitsAndFits_HeaderFile
