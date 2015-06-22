// Created on: 2013-09-05
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

#ifndef _OpenGl_CappingAlgo_H__
#define _OpenGl_CappingAlgo_H__

#include <OpenGl_RenderFilter.hxx>
#include <OpenGl_Group.hxx>

#include <Graphic3d_SequenceOfGroup.hxx>

// Forward declaration
class Handle(OpenGl_Workspace);
class Handle(Graphic3d_ClipPlane);

DEFINE_STANDARD_HANDLE (OpenGl_CappingAlgoFilter, OpenGl_RenderFilter)

//! Capping surface rendering algorithm.
class OpenGl_CappingAlgo
{
public:

  //! Draw capping surfaces by OpenGl for the clipping planes
  //! enabled in current context state. Depth buffer must be generated
  //! for the passed groups.
  //! @param theWorkspace [in] the GL workspace, context state.
  //! @param theGroups [in] the group of primitives to be capped.
  Standard_EXPORT static void RenderCapping (const Handle(OpenGl_Workspace)&  theWorkspace,
                                             const Graphic3d_SequenceOfGroup& theGroups);

  //! Render infinite capping plane.
  //! @param theWorkspace [in] the GL workspace, context state.
  //! @param thePlane [in] the graphical plane, for which the capping surface is rendered.
  Standard_EXPORT static void RenderPlane (const Handle(OpenGl_Workspace)& theWorkspace,
                                           const Handle(Graphic3d_ClipPlane)& thePlane);
};

//! Graphical capping rendering algorithm filter.
//! Filters out everything excepth shaded primitives.
class OpenGl_CappingAlgoFilter : public OpenGl_RenderFilter
{
public:

  //! Default constructor.
  OpenGl_CappingAlgoFilter() {}

  //! Checks whether the element can be rendered or not.
  //! @param theElement [in] the element to check.
  //! @return True if element can be rendered.
  virtual Standard_Boolean CanRender (const OpenGl_Element* theElement);

public:

  DEFINE_STANDARD_RTTI(OpenGl_CappingAlgoFilter)
};

#endif
