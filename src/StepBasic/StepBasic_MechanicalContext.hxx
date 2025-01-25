// Created on: 1996-07-24
// Created by: Frederic MAUPAS
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _StepBasic_MechanicalContext_HeaderFile
#define _StepBasic_MechanicalContext_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_ProductContext.hxx>

class StepBasic_MechanicalContext;
DEFINE_STANDARD_HANDLE(StepBasic_MechanicalContext, StepBasic_ProductContext)

class StepBasic_MechanicalContext : public StepBasic_ProductContext
{

public:
  //! Returns a MechanicalContext
  Standard_EXPORT StepBasic_MechanicalContext();

  DEFINE_STANDARD_RTTIEXT(StepBasic_MechanicalContext, StepBasic_ProductContext)

protected:
private:
};

#endif // _StepBasic_MechanicalContext_HeaderFile
