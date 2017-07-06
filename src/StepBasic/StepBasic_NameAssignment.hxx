// Created on: 2000-05-10
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _StepBasic_NameAssignment_HeaderFile
#define _StepBasic_NameAssignment_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class TCollection_HAsciiString;


class StepBasic_NameAssignment;
DEFINE_STANDARD_HANDLE(StepBasic_NameAssignment, Standard_Transient)

//! Representation of STEP entity NameAssignment
class StepBasic_NameAssignment : public Standard_Transient
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepBasic_NameAssignment();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aAssignedName);
  
  //! Returns field AssignedName
  Standard_EXPORT Handle(TCollection_HAsciiString) AssignedName() const;
  
  //! Set field AssignedName
  Standard_EXPORT void SetAssignedName (const Handle(TCollection_HAsciiString)& AssignedName);




  DEFINE_STANDARD_RTTIEXT(StepBasic_NameAssignment,Standard_Transient)

protected:




private:


  Handle(TCollection_HAsciiString) theAssignedName;


};







#endif // _StepBasic_NameAssignment_HeaderFile
