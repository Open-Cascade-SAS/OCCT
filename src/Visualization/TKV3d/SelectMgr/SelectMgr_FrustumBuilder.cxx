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

IMPLEMENT_STANDARD_RTTIEXT(SelectMgr_FrustumBuilder, Standard_Transient)

#define DOT(A, B) (A.x() * B.x() + A.y() * B.y() + A.z() * B.z())
#define LENGTH(A) (std::sqrt(A.x() * A.x() + A.y() * A.y() + A.z() * A.z()))

//=======================================================================
// function : SelectMgr_FrustumBuilder
// purpose  : Creates new frustum builder with empty matrices
//=======================================================================
SelectMgr_FrustumBuilder::SelectMgr_FrustumBuilder()
    : myWidth(INT_MAX),
      myHeight(INT_MAX),
      myIsViewportSet(false)
{
}

//=================================================================================================

void SelectMgr_FrustumBuilder::SetCamera(const occ::handle<Graphic3d_Camera>& theCamera)
{
  myCamera = theCamera;
}

//=======================================================================
// function : SetWindowSize
// purpose  : Stores current window width and height
//=======================================================================
void SelectMgr_FrustumBuilder::SetWindowSize(const int theWidth, const int theHeight)
{
  myWidth  = theWidth;
  myHeight = theHeight;
}

//=======================================================================
// function : SetViewport
// purpose  : Stores current viewport coordinates
//=======================================================================
void SelectMgr_FrustumBuilder::SetViewport(const double theX,
                                           const double theY,
                                           const double theWidth,
                                           const double theHeight)
{
  myViewport      = NCollection_Vec4<double>(theX, theY, theWidth, theHeight);
  myIsViewportSet = true;
}

//=================================================================================================

void SelectMgr_FrustumBuilder::WindowSize(int& theWidth, int& theHeight) const
{
  theWidth  = myWidth;
  theHeight = myHeight;
}

//=================================================================================================

void SelectMgr_FrustumBuilder::InvalidateViewport()
{
  myIsViewportSet = false;
}

//=======================================================================
// function : SignedPlanePntDist
// purpose  : Calculates signed distance between plane with equation
//            theEq and point thePnt
//=======================================================================
double SelectMgr_FrustumBuilder::SignedPlanePntDist(const NCollection_Vec3<double>& theEq,
                                                    const NCollection_Vec3<double>& thePnt) const
{
  const double aNormLength     = LENGTH(theEq);
  const double anInvNormLength = aNormLength < Precision::Confusion() ? 0.0 : 1.0 / aNormLength;
  const double anA             = theEq.x() * anInvNormLength;
  const double aB              = theEq.y() * anInvNormLength;
  const double aC              = theEq.z() * anInvNormLength;
  return anA * thePnt.x() + aB * thePnt.y() + aC * thePnt.z();
}

// =======================================================================
// function : ProjectPntOnViewPlane
// purpose  : Projects 2d screen point onto view frustum plane:
//            theZ = 0 - near plane,
//            theZ = 1 - far plane
// =======================================================================
gp_Pnt SelectMgr_FrustumBuilder::ProjectPntOnViewPlane(const double& theX,
                                                       const double& theY,
                                                       const double& theZ) const
{
  if (myCamera.IsNull())
  {
    return gp_Pnt();
  }
  // map coords to NDC
  gp_Pnt anXYZ;
  if (!myIsViewportSet)
  {
    anXYZ.SetCoord(2.0 * theX / myWidth - 1.0,
                   (myHeight - 1 - theY) / myHeight * 2.0 - 1.0,
                   myCamera->IsZeroToOneDepth() ? theZ : (2.0 * theZ - 1.0));
  }
  else
  {
    anXYZ.SetCoord(
      2.0 * (theX - myWidth * myViewport.x()) / (myWidth * (myViewport.z() - myViewport.x())) - 1.0,
      2.0 * (theY - myHeight * myViewport.y()) / (myHeight * (myViewport.w() - myViewport.y()))
        - 1.0,
      theZ);
  }
  return myCamera->UnProject(anXYZ);
}
