// Created on: 2013-08-15
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

#ifndef OpenGl_CappingPlaneResource_HeaderFile
#define OpenGl_CappingPlaneResource_HeaderFile

#include <OpenGl_PrimitiveArray.hxx>
#include <OpenGl_Resource.hxx>
#include <OpenGl_Aspects.hxx>
#include <OpenGl_Vec.hxx>
#include <Graphic3d_ClipPlane.hxx>

class OpenGl_CappingPlaneResource;
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
  //! @param theContext   [in] the context
  //! @param theObjAspect [in] object aspect
  Standard_EXPORT void Update (const Handle(OpenGl_Context)& theContext,
                               const Handle(Graphic3d_Aspects)& theObjAspect);

  //! Release associated OpenGl resources.
  //! @param theContext [in] the resource context.
  Standard_EXPORT virtual void Release (OpenGl_Context* theContext) Standard_OVERRIDE;

  //! Returns estimated GPU memory usage - not implemented.
  virtual Standard_Size EstimatedDataSize() const Standard_OVERRIDE { return 0; }

  //! Return parent clipping plane structure.
  const Handle(Graphic3d_ClipPlane)& Plane() const { return myPlaneRoot; }

  //! @return aspect face for rendering capping surface.
  inline const OpenGl_Aspects* AspectFace() const { return myAspect; }

  //! @return evaluated orientation matrix to transform infinite plane.
  inline const OpenGl_Mat4& Orientation() const { return myOrientation; }

  //! @return primitive array of vertices to render infinite plane.
  inline const OpenGl_PrimitiveArray& Primitives() const { return myPrimitives; }

private:

  //! Update precomputed plane orientation matrix.
  void updateTransform (const Handle(OpenGl_Context)& theCtx);

  //! Update resources.
  void updateAspect (const Handle(Graphic3d_Aspects)& theObjAspect);

private:

  OpenGl_PrimitiveArray       myPrimitives;    //!< vertices and texture coordinates for rendering
  OpenGl_Mat4                 myOrientation;   //!< plane transformation matrix.
  OpenGl_Aspects*             myAspect;        //!< capping face aspect.
  Handle(Graphic3d_ClipPlane) myPlaneRoot;     //!< parent clipping plane structure.
  Handle(Graphic3d_Aspects)   myFillAreaAspect;//!< own capping aspect
  gp_XYZ                      myLocalOrigin;   //!< layer origin
  unsigned int                myEquationMod;   //!< modification counter for plane equation.
  unsigned int                myAspectMod;     //!< modification counter for aspect.

public:

  DEFINE_STANDARD_RTTIEXT(OpenGl_CappingPlaneResource,OpenGl_Resource) // Type definition

};

#endif
