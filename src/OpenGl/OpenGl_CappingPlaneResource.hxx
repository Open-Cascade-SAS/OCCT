// Created on: 2013-08-15
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

#ifndef _OpenGl_CappingPlaneResource_H__
#define _OpenGl_CappingPlaneResource_H__

#include <OpenGl_Resource.hxx>
#include <OpenGl_AspectFace.hxx>
#include <OpenGl_Matrix.hxx>
#include <Graphic3d_ClipPlane.hxx>

class Handle(OpenGl_Context);

DEFINE_STANDARD_HANDLE (OpenGl_CappingPlaneResource, OpenGl_Resource)

//! Container of graphical resources for rendering capping plane
//! associated to graphical clipping plane.
//! This resource holds data necessary for OpenGl_CappingAlgo.
//! This object is implemented as OpenGl resource for the following reasons:
//! - one instance should be shared between contexts.
//! - instance associated to Graphic3d_ClipPlane data by id.
//! - should created and released within context (owns OpenGl elements and resources).
class OpenGl_CappingPlaneResource : public OpenGl_Resource
{
public:

  //! Constructor.
  //! Create capping plane presentation associated to clipping plane data.
  //! @param thePlane [in] the plane data.
  Standard_EXPORT OpenGl_CappingPlaneResource (const Handle(Graphic3d_ClipPlane)& thePlane);

  //! Destroy object.
  Standard_EXPORT virtual ~OpenGl_CappingPlaneResource();

  //! Update resource data in the passed context.
  //! @param theContext [in] the context.
  Standard_EXPORT void Update (const Handle(OpenGl_Context)& theContext);

  //! Release associated OpenGl resources.
  //! @param theContext [in] the resource context.
  Standard_EXPORT void Release (const OpenGl_Context* theContext);

  //! @return aspect face for rendering capping surface.
  inline const OpenGl_AspectFace* AspectFace() const { return myAspect; }

  //! @return evaluated orientation matrix to transform infinite plane.
  inline const OpenGl_Matrix* Orientation() const { return &myOrientation; }

private:

  //! Update precomputed plane orientation matrix.
  void UpdateTransform();

  //! Update resources.
  //! @param theContext [in] the context.
  void UpdateAspect (const Handle(OpenGl_Context)& theContext);

private:

  OpenGl_Matrix               myOrientation;   //!< plane transformation matrix.
  OpenGl_AspectFace*          myAspect;        //!< capping face aspect.
  Handle(Graphic3d_ClipPlane) myPlaneRoot;     //!< parent clipping plane structure.
  unsigned int                myEquationMod;   //!< modification counter for plane equation.
  unsigned int                myAspectMod;     //!< modification counter for aspect.

public:

  DEFINE_STANDARD_RTTI(OpenGl_CappingPlaneResource) // Type definition

};

#endif
