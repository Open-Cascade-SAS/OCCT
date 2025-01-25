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

#ifndef _StepAP214_Class_HeaderFile
#define _StepAP214_Class_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_Group.hxx>

class StepAP214_Class;
DEFINE_STANDARD_HANDLE(StepAP214_Class, StepBasic_Group)

//! Representation of STEP entity Class
class StepAP214_Class : public StepBasic_Group
{

public:
  //! Empty constructor
  Standard_EXPORT StepAP214_Class();

  DEFINE_STANDARD_RTTIEXT(StepAP214_Class, StepBasic_Group)

protected:
private:
};

#endif // _StepAP214_Class_HeaderFile
