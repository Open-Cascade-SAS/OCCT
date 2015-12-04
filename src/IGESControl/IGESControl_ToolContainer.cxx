// Created on: 2000-02-08
// Created by: data exchange team
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


#include <IGESControl_IGESBoundary.hxx>
#include <IGESControl_ToolContainer.hxx>
#include <IGESToBRep_IGESBoundary.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESControl_ToolContainer,IGESToBRep_ToolContainer)

//=======================================================================
//function : IGESControl_ToolContainer
//purpose  : 
//=======================================================================
IGESControl_ToolContainer::IGESControl_ToolContainer():IGESToBRep_ToolContainer()
{
}

//=======================================================================
//function : IGESBoundary
//purpose  : 
//=======================================================================

Handle(IGESToBRep_IGESBoundary) IGESControl_ToolContainer::IGESBoundary() const
{
  return new IGESControl_IGESBoundary;
}
