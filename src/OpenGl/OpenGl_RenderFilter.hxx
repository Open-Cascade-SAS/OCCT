// Created on: 2013-07-25
// Created by: Anton POLETAEV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef _OpenGl_RenderFilter_H__
#define _OpenGl_RenderFilter_H__

#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>

class OpenGl_RenderFilter;
DEFINE_STANDARD_HANDLE (OpenGl_RenderFilter, Standard_Transient)

class OpenGl_Element;
class OpenGl_Workspace;

//! Base class for defining element rendering filters.
//! This class can be used in pair with advance rendering passes, and for 
//! disabling rendering (setting up) graphical aspects.
class OpenGl_RenderFilter : public Standard_Transient
{
public:

  //! Checks whether the element can be rendered or not.
  //! @param theWorkspace [in] the current workspace.
  //! @param theElement [in] the element to check.
  //! @return True if element can be rendered.
  virtual Standard_Boolean ShouldRender (const Handle(OpenGl_Workspace)& theWorkspace, const OpenGl_Element* theElement) = 0;

public:

  DEFINE_STANDARD_RTTIEXT(OpenGl_RenderFilter, Standard_Transient)
};

#endif
