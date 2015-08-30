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

#define DOT(A, B) (A.x() * B.x() + A.y() * B.y() + A.z() * B.z())
#define LENGTH(A) (std::sqrt (A.x() * A.x() + A.y() * A.y() + A.z() * A.z()))

IMPLEMENT_STANDARD_HANDLE (SelectMgr_FrustumBuilder, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(SelectMgr_FrustumBuilder, Standard_Transient)

//=======================================================================
// function : SelectMgr_FrustumBuilder
// purpose  : Creates new frustum builder with empty matrices
//=======================================================================
SelectMgr_FrustumBuilder::SelectMgr_FrustumBuilder()
: myOrientation(),
  myProjection(),
  myWidth (INT_MAX),
  myHeight (INT_MAX),
  myIsViewportSet (Standard_False) {}

//=======================================================================
// function : SetOrientation
// purpose  : Stores current orientation matrix
//=======================================================================
void SelectMgr_FrustumBuilder::SetOrientation (const Graphic3d_Mat4d& theOrientation)
{
  myOrientation = theOrientation;
}

//=======================================================================
// function : SetProjection
// purpose  : Stores current projection matrix
//=======================================================================
void SelectMgr_FrustumBuilder::SetProjection (const Graphic3d_Mat4d& theProjection)
{
  myProjection = theProjection;
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

//=======================================================================
// function : safePointCast
// purpose  :
//=======================================================================
static NCollection_Vec4<Standard_Real> safePointCast (const gp_Pnt& thePnt)
{
  Standard_Real aLim = 1e15f;

  // have to deal with values greater then max float
  gp_Pnt aSafePoint = thePnt;
  const Standard_Real aBigFloat = aLim * 0.1f;
  if (Abs (aSafePoint.X()) > aLim)
    aSafePoint.SetX (aSafePoint.X() >= 0 ? aBigFloat : -aBigFloat);
  if (Abs (aSafePoint.Y()) > aLim)
    aSafePoint.SetY (aSafePoint.Y() >= 0 ? aBigFloat : -aBigFloat);
  if (Abs (aSafePoint.Z()) > aLim)
    aSafePoint.SetZ (aSafePoint.Z() >= 0 ? aBigFloat : -aBigFloat);

  // convert point
  NCollection_Vec4<Standard_Real> aPnt (aSafePoint.X(), aSafePoint.Y(), aSafePoint.Z(), 1.0);

  return aPnt;
}

//=======================================================================
// function : unProject
// purpose  : Unprojects point from NDC coords to 3d world space
//=======================================================================
gp_Pnt SelectMgr_FrustumBuilder::unProject (const gp_Pnt& thePnt) const
{
  Graphic3d_Mat4d aInvView;
  Graphic3d_Mat4d aInvProj;

  // this case should never happen
  if (!myOrientation.Inverted (aInvView) || !myProjection.Inverted (aInvProj))
  {
    return gp_Pnt (0.0, 0.0, 0.0);
  }

  // use compatible type of point
  NCollection_Vec4<Standard_Real> aPnt = safePointCast (thePnt);

  aPnt = aInvProj * aPnt; // convert to view coordinate space
  aPnt = aInvView * aPnt; // convert to world coordinate space

  const Standard_Real aInvW = 1.0 / Standard_Real (aPnt.w());

  return gp_Pnt (aPnt.x() * aInvW, aPnt.y() * aInvW, aPnt.z() * aInvW);
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
  Standard_Real aX, anY, aZ;

  // map coords to NDC
  if (!myIsViewportSet)
  {
    aX = 2.0 * theX / myWidth - 1.0;
    anY = (myHeight - 1 - theY) / myHeight * 2.0 - 1.0;
    aZ = 2.0 * theZ - 1.0;
  }
  else
  {
    aX = 2.0 * (theX - myWidth * myViewport.x()) /
      (myWidth * (myViewport.z() - myViewport.x())) - 1.0;
    anY = 2.0 * (theY - myHeight * myViewport.y()) /
      (myHeight * (myViewport.w() - myViewport.y())) - 1.0;
    aZ = theZ;
  }

  return unProject (gp_Pnt (aX, anY, aZ));
}
