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

#ifndef _IGESControl_ToolContainer_HeaderFile
#define _IGESControl_ToolContainer_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESToBRep_ToolContainer.hxx>
class IGESToBRep_IGESBoundary;

class IGESControl_ToolContainer;
DEFINE_STANDARD_HANDLE(IGESControl_ToolContainer, IGESToBRep_ToolContainer)

class IGESControl_ToolContainer : public IGESToBRep_ToolContainer
{

public:
  //! Empty constructor
  Standard_EXPORT IGESControl_ToolContainer();

  //! Returns IGESControl_IGESBoundary
  Standard_EXPORT virtual Handle(IGESToBRep_IGESBoundary) IGESBoundary() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESControl_ToolContainer, IGESToBRep_ToolContainer)

protected:
private:
};

#endif // _IGESControl_ToolContainer_HeaderFile
