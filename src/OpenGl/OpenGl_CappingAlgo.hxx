// Created on: 2013-09-05
// Created by: Anton POLETAEV
// Copyright (c) 2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#ifndef _OpenGl_CappingAlgo_H__
#define _OpenGl_CappingAlgo_H__

#include <OpenGl_RenderFilter.hxx>
#include <OpenGl_Group.hxx>

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
  Standard_EXPORT static void RenderCapping (const Handle(OpenGl_Workspace)& theWorkspace,
                                             const OpenGl_ListOfGroup& theGroups);

  //! Render infinite capping plane.
  //! @param theWorkspace [in] the GL workspace, context state.
  //! @param thePlane [in] the graphical plane, for which the capping surface is rendered.
  Standard_EXPORT static void RenderPlane (const Handle(OpenGl_Workspace)& theWorkspace,
                                           const Handle(Graphic3d_ClipPlane)& thePlane);

private:

  //! Init algorithm.
  static void Init();

  //! @return capping algorithm rendering filter.
  static const Handle(OpenGl_RenderFilter)& CappingFilter() { return myRenderFilter; }

  //! @return face aspect for front face culling mode.
  static const OpenGl_AspectFace* FrontCulling() { return &myFrontCulling; }

  //! @return face aspect for none culling mode.
  static const OpenGl_AspectFace* NoneCulling() { return &myNoneCulling; }

private:

  static Handle(OpenGl_RenderFilter) myRenderFilter;
  static OpenGl_AspectFace myFrontCulling;
  static OpenGl_AspectFace myNoneCulling;
  static Standard_Boolean myIsInit;
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
