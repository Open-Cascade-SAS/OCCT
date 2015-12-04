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

#ifndef _StepBasic_ExternalIdentificationAssignment_HeaderFile
#define _StepBasic_ExternalIdentificationAssignment_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_IdentificationAssignment.hxx>
class StepBasic_ExternalSource;
class TCollection_HAsciiString;
class StepBasic_IdentificationRole;


class StepBasic_ExternalIdentificationAssignment;
DEFINE_STANDARD_HANDLE(StepBasic_ExternalIdentificationAssignment, StepBasic_IdentificationAssignment)

//! Representation of STEP entity ExternalIdentificationAssignment
class StepBasic_ExternalIdentificationAssignment : public StepBasic_IdentificationAssignment
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepBasic_ExternalIdentificationAssignment();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aIdentificationAssignment_AssignedId, const Handle(StepBasic_IdentificationRole)& aIdentificationAssignment_Role, const Handle(StepBasic_ExternalSource)& aSource);
  
  //! Returns field Source
  Standard_EXPORT Handle(StepBasic_ExternalSource) Source() const;
  
  //! Set field Source
  Standard_EXPORT void SetSource (const Handle(StepBasic_ExternalSource)& Source);




  DEFINE_STANDARD_RTTIEXT(StepBasic_ExternalIdentificationAssignment,StepBasic_IdentificationAssignment)

protected:




private:


  Handle(StepBasic_ExternalSource) theSource;


};







#endif // _StepBasic_ExternalIdentificationAssignment_HeaderFile
