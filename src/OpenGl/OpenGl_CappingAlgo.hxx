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

// Forward declaration
class OpenGl_CappingAlgoFilter;
class OpenGl_CappingPlaneResource;
class OpenGl_Structure;

DEFINE_STANDARD_HANDLE (OpenGl_CappingAlgoFilter, OpenGl_RenderFilter)

//! Capping surface rendering algorithm.
class OpenGl_CappingAlgo
{
public:

  //! Draw capping surfaces by OpenGl for the clipping planes enabled in current context state.
  //! Depth buffer must be generated  for the passed groups.
  //! @param theWorkspace [in] the GL workspace, context state
  //! @param theStructure [in] the structure to be capped
  Standard_EXPORT static void RenderCapping (const Handle(OpenGl_Workspace)& theWorkspace,
                                             const OpenGl_Structure&         theStructure);

};

//! Graphical capping rendering algorithm filter.
//! Filters out everything except shaded primitives.
class OpenGl_CappingAlgoFilter : public OpenGl_RenderFilter
{
public:

  //! Default constructor.
  OpenGl_CappingAlgoFilter() {}

  //! Sets the current active filter in workspace.
  //! @param thePrevFilter [in] the previously active filter that should have additive effect.
  void SetPreviousFilter (const Handle(OpenGl_RenderFilter)& thePrevFitler) { myFilter = thePrevFitler; }

  //! Checks whether the element can be rendered or not.
  //! @param theElement [in] the element to check.
  //! @return True if element can be rendered.
  virtual Standard_Boolean ShouldRender (const Handle(OpenGl_Workspace)& theWorkspace,
                                         const OpenGl_Element* theGlElement) Standard_OVERRIDE;

private:

  Handle(OpenGl_RenderFilter) myFilter; //!< Previous active filter that should be combined.

public:

  DEFINE_STANDARD_RTTIEXT(OpenGl_CappingAlgoFilter,OpenGl_RenderFilter)
};

#endif
