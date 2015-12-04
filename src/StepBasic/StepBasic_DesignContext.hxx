// Created on: 1996-10-07
// Created by: Christian CAILLET
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

#ifndef _StepBasic_DesignContext_HeaderFile
#define _StepBasic_DesignContext_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_ProductDefinitionContext.hxx>


class StepBasic_DesignContext;
DEFINE_STANDARD_HANDLE(StepBasic_DesignContext, StepBasic_ProductDefinitionContext)

//! class added to Schema AP214 around April 1996
class StepBasic_DesignContext : public StepBasic_ProductDefinitionContext
{

public:

  
  Standard_EXPORT StepBasic_DesignContext();




  DEFINE_STANDARD_RTTIEXT(StepBasic_DesignContext,StepBasic_ProductDefinitionContext)

protected:




private:




};







#endif // _StepBasic_DesignContext_HeaderFile
