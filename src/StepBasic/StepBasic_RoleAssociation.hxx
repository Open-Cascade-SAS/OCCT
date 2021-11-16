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

#ifndef _StepBasic_RoleAssociation_HeaderFile
#define _StepBasic_RoleAssociation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_RoleSelect.hxx>
#include <Standard_Transient.hxx>
class StepBasic_ObjectRole;


class StepBasic_RoleAssociation;
DEFINE_STANDARD_HANDLE(StepBasic_RoleAssociation, Standard_Transient)

//! Representation of STEP entity RoleAssociation
class StepBasic_RoleAssociation : public Standard_Transient
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepBasic_RoleAssociation();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const Handle(StepBasic_ObjectRole)& aRole, const StepBasic_RoleSelect& aItemWithRole);
  
  //! Returns field Role
  Standard_EXPORT Handle(StepBasic_ObjectRole) Role() const;
  
  //! Set field Role
  Standard_EXPORT void SetRole (const Handle(StepBasic_ObjectRole)& Role);
  
  //! Returns field ItemWithRole
  Standard_EXPORT StepBasic_RoleSelect ItemWithRole() const;
  
  //! Set field ItemWithRole
  Standard_EXPORT void SetItemWithRole (const StepBasic_RoleSelect& ItemWithRole);




  DEFINE_STANDARD_RTTIEXT(StepBasic_RoleAssociation,Standard_Transient)

protected:




private:


  Handle(StepBasic_ObjectRole) theRole;
  StepBasic_RoleSelect theItemWithRole;


};







#endif // _StepBasic_RoleAssociation_HeaderFile
