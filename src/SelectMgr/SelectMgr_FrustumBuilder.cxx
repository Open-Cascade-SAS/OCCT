// Created on: 2014-11-24
// Created by: Varvara POSKONINA
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#include <SelectMgr_FrustumBuilder.hxx>

IMPLEMENT_STANDARD_RTTIEXT(SelectMgr_FrustumBuilder,Standard_Transient)

#define DOT(A, B) (A.x() * B.x() + A.y() * B.y() + A.z() * B.z())
#define LENGTH(A) (std::sqrt (A.x() * A.x() + A.y() * A.y() + A.z() * A.z()))


//=======================================================================
// function : SelectMgr_FrustumBuilder
// purpose  : Creates new frustum builder with empty matrices
//=======================================================================
SelectMgr_FrustumBuilder::SelectMgr_FrustumBuilder()
: myWidth (INT_MAX),
  myHeight (INT_MAX),
  myIsViewportSet (Standard_False)
{
  //
}

//=======================================================================
// function : SetCamera
// purpose  :
//=======================================================================
void SelectMgr_FrustumBuilder::SetCamera (const Handle(Graphic3d_Camera)& theCamera)
{
  myCamera = theCamera;
}

//=======================================================================
// function : SetWindowSize
// purpose  : Stores current window width and height
//=======================================================================
void SelectMgr_FrustumBuilder::SetWindowSize (const Standard_Integer theWidth,
                                              const Standard_Integer theHeight)
{
  myWidth = theWidth;
  myHeight = theHeight;
}

//=======================================================================
// function : SetViewport
// purpose  : Stores current viewport coordinates
//=======================================================================
void SelectMgr_FrustumBuilder::SetViewport (const Standard_Real theX,
                                            const Standard_Real theY,
                                            const Standard_Real theWidth,
                                            const Standard_Real theHeight)
{
  myViewport = NCollection_Vec4<Standard_Real> (theX, theY, theWidth, theHeight);
  myIsViewportSet = Standard_True;
}

//=======================================================================
// function : WindowSize
// purpose  :
//=======================================================================
void SelectMgr_FrustumBuilder::WindowSize (Standard_Integer& theWidth,
                                           Standard_Integer& theHeight) const
{
  theWidth = myWidth;
  theHeight = myHeight;
}

//=======================================================================
// function : InvalidateViewport
// purpose  :
//=======================================================================
void SelectMgr_FrustumBuilder::InvalidateViewport()
{
  myIsViewportSet = Standard_False;
}

//=======================================================================
// function : SignedPlanePntDist
// purpose  : Calculates signed distance between plane with equation
//            theEq and point thePnt
//=======================================================================
Standard_Real SelectMgr_FrustumBuilder::SignedPlanePntDist (const SelectMgr_Vec3& theEq,
                                                            const SelectMgr_Vec3& thePnt) const
{
  const Standard_Real aNormLength = LENGTH (theEq);
  const Standard_Real anInvNormLength = aNormLength < Precision::Confusion() ? 0.0 : 1.0 / aNormLength;
  const Standard_Real anA = theEq.x() * anInvNormLength;
  const Standard_Real aB  = theEq.y() * anInvNormLength;
  const Standard_Real aC  = theEq.z() * anInvNormLength;
  return anA * thePnt.x() + aB * thePnt.y() + aC * thePnt.z();
}

// =======================================================================
// function : ProjectPntOnViewPlane
// purpose  : Projects 2d screen point onto view frustum plane:
//            theZ = 0 - near plane,
//            theZ = 1 - far plane
// =======================================================================
gp_Pnt SelectMgr_FrustumBuilder::ProjectPntOnViewPlane (const Standard_Real& theX,
                                                        const Standard_Real& theY,
                                                        const Standard_Real& theZ) const
{
  if (myCamera.IsNull())
  {
    return gp_Pnt();
  }
  // map coords to NDC
  gp_Pnt anXYZ;
  if (!myIsViewportSet)
  {
    anXYZ.SetCoord (2.0 * theX / myWidth - 1.0,
                    (myHeight - 1 - theY) / myHeight * 2.0 - 1.0,
                    myCamera->IsZeroToOneDepth() ? theZ : (2.0 * theZ - 1.0));
  }
  else
  {
    anXYZ.SetCoord (2.0 * (theX - myWidth  * myViewport.x()) / (myWidth  * (myViewport.z() - myViewport.x())) - 1.0,
                    2.0 * (theY - myHeight * myViewport.y()) / (myHeight * (myViewport.w() - myViewport.y())) - 1.0,
                    theZ);
  }
  return myCamera->UnProject (anXYZ);
}
