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

#ifndef _StepRepr_ParametricRepresentationContext_HeaderFile
#define _StepRepr_ParametricRepresentationContext_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_RepresentationContext.hxx>


class StepRepr_ParametricRepresentationContext;
DEFINE_STANDARD_HANDLE(StepRepr_ParametricRepresentationContext, StepRepr_RepresentationContext)


class StepRepr_ParametricRepresentationContext : public StepRepr_RepresentationContext
{

public:

  
  //! Returns a ParametricRepresentationContext
  Standard_EXPORT StepRepr_ParametricRepresentationContext();




  DEFINE_STANDARD_RTTIEXT(StepRepr_ParametricRepresentationContext,StepRepr_RepresentationContext)

protected:




private:




};







#endif // _StepRepr_ParametricRepresentationContext_HeaderFile
