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

#ifndef _StepBasic_SecurityClassification_HeaderFile
#define _StepBasic_SecurityClassification_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class TCollection_HAsciiString;
class StepBasic_SecurityClassificationLevel;


class StepBasic_SecurityClassification;
DEFINE_STANDARD_HANDLE(StepBasic_SecurityClassification, Standard_Transient)


class StepBasic_SecurityClassification : public Standard_Transient
{

public:

  
  //! Returns a SecurityClassification
  Standard_EXPORT StepBasic_SecurityClassification();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(TCollection_HAsciiString)& aPurpose, const Handle(StepBasic_SecurityClassificationLevel)& aSecurityLevel);
  
  Standard_EXPORT void SetName (const Handle(TCollection_HAsciiString)& aName);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) Name() const;
  
  Standard_EXPORT void SetPurpose (const Handle(TCollection_HAsciiString)& aPurpose);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) Purpose() const;
  
  Standard_EXPORT void SetSecurityLevel (const Handle(StepBasic_SecurityClassificationLevel)& aSecurityLevel);
  
  Standard_EXPORT Handle(StepBasic_SecurityClassificationLevel) SecurityLevel() const;




  DEFINE_STANDARD_RTTIEXT(StepBasic_SecurityClassification,Standard_Transient)

protected:




private:


  Handle(TCollection_HAsciiString) name;
  Handle(TCollection_HAsciiString) purpose;
  Handle(StepBasic_SecurityClassificationLevel) securityLevel;


};







#endif // _StepBasic_SecurityClassification_HeaderFile
