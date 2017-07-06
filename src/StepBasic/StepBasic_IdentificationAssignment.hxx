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

#ifndef _StepBasic_IdentificationAssignment_HeaderFile
#define _StepBasic_IdentificationAssignment_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class TCollection_HAsciiString;
class StepBasic_IdentificationRole;


class StepBasic_IdentificationAssignment;
DEFINE_STANDARD_HANDLE(StepBasic_IdentificationAssignment, Standard_Transient)

//! Representation of STEP entity IdentificationAssignment
class StepBasic_IdentificationAssignment : public Standard_Transient
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepBasic_IdentificationAssignment();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aAssignedId, const Handle(StepBasic_IdentificationRole)& aRole);
  
  //! Returns field AssignedId
  Standard_EXPORT Handle(TCollection_HAsciiString) AssignedId() const;
  
  //! Set field AssignedId
  Standard_EXPORT void SetAssignedId (const Handle(TCollection_HAsciiString)& AssignedId);
  
  //! Returns field Role
  Standard_EXPORT Handle(StepBasic_IdentificationRole) Role() const;
  
  //! Set field Role
  Standard_EXPORT void SetRole (const Handle(StepBasic_IdentificationRole)& Role);




  DEFINE_STANDARD_RTTIEXT(StepBasic_IdentificationAssignment,Standard_Transient)

protected:




private:


  Handle(TCollection_HAsciiString) theAssignedId;
  Handle(StepBasic_IdentificationRole) theRole;


};







#endif // _StepBasic_IdentificationAssignment_HeaderFile
