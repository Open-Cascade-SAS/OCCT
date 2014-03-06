// Created on: 1992-03-13
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <Select3D_Projector.ixx>
#include <Precision.hxx>
#include <gp_Ax3.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Mat.hxx>
#include <Graphic3d_Vec4.hxx>

namespace
{
  //=======================================================================
  //function : TrsfType
  //purpose  :
  //=======================================================================
  static Standard_Integer TrsfType(const gp_GTrsf& theTrsf)
  {
    const gp_Mat& aMat = theTrsf.VectorialPart();
    if ((Abs (aMat.Value (1, 1) - 1.0) < 1e-15)
     && (Abs (aMat.Value (2, 2) - 1.0) < 1e-15)
     && (Abs (aMat.Value (3, 3) - 1.0) < 1e-15))
    {
      return 1; // top
    }
    else if ((Abs (aMat.Value (1, 1) - 0.7071067811865476) < 1e-15)
          && (Abs (aMat.Value (1, 2) + 0.5) < 1e-15)
          && (Abs (aMat.Value (1, 3) - 0.5) < 1e-15)
          && (Abs (aMat.Value (2, 1) - 0.7071067811865476) < 1e-15)
          && (Abs (aMat.Value (2, 2) - 0.5) < 1e-15)
          && (Abs (aMat.Value (2, 3) + 0.5) < 1e-15)
          && (Abs (aMat.Value (3, 1)) < 1e-15)
          && (Abs (aMat.Value (3, 2) - 0.7071067811865476) < 1e-15)
          && (Abs (aMat.Value (3, 3) - 0.7071067811865476) < 1e-15))
    {
      return 0; // inverse axo
    }
    else if ((Abs (aMat.Value (1, 1) - 1.0) < 1e-15)
          && (Abs (aMat.Value (2, 3) - 1.0) < 1e-15)
          && (Abs (aMat.Value (3, 2) + 1.0) < 1e-15))
    {
      return 2; // front
    }
    else if ((Abs (aMat.Value (1, 1) - 0.7071067811865476) < 1e-15)
          && (Abs (aMat.Value (1, 2) - 0.7071067811865476) < 1e-15)
          && (Abs (aMat.Value (1, 3)) < 1e-15)
          && (Abs (aMat.Value (2, 1) + 0.5) < 1e-15)
          && (Abs (aMat.Value (2, 2) - 0.5) < 1e-15)
          && (Abs (aMat.Value (2, 3) - 0.7071067811865476) < 1e-15)
          && (Abs (aMat.Value (3, 1) - 0.5) < 1e-15)
          && (Abs (aMat.Value (3, 2) + 0.5) < 1e-15)
          && (Abs (aMat.Value (3, 3) - 0.7071067811865476) < 1e-15))
    {
      return 3; // axo
    }

    return -1;
  }

   //====== TYPE 0 (inverse axonometric)
   // (0.7071067811865476, -0.5               ,  0.4999999999999999)
   // (0.7071067811865475,  0.5000000000000001, -0.5              )
   // (0.0,                 0.7071067811865475,  0.7071067811865476)

   // ====== TYPE 1 (top)
   // (1.0, 0.0, 0.0)
   // (0.0, 1.0, 0.0)
   // (0.0, 0.0, 1.0)

   // ======= TYPE 2 (front)
   // (1.0,  0.0                   , 0.0)
   // (0.0,  1.110223024625157e-16 , 1.0)
   // (0.0, -1.0                   , 1.110223024625157e-16)

   // ======= TYPE 3 (axonometric)
   // ( 0.7071067811865476, 0.7071067811865475, 0.0)
   // (-0.5               , 0.5000000000000001, 0.7071067811865475)
   // ( 0.4999999999999999, -0.5              , 0.7071067811865476)
}

// formula for derivating a perspective, from Mathematica

//        X'[t]      X[t] Z'[t]
// D1 =  -------- + -------------
//           Z[t]          Z[t] 2
//       1 - ----   f (1 - ----)
//            f             f

//=======================================================================
//function : Select3D_Projector
//purpose  :
//=======================================================================
Select3D_Projector::Select3D_Projector (const Handle(V3d_View)& theView)
: myPersp (Standard_False),
  myFocus (0.0),
  myType (-1)
{
  SetView (theView);
}

//=======================================================================
//function : Select3D_Projector
//purpose  :
//=======================================================================
Select3D_Projector::Select3D_Projector()
: myPersp (Standard_False),
  myFocus (0.0),
  myType (-1)
{
  Scaled();
}

//=======================================================================
//function : Select3D_Projector
//purpose  :
//=======================================================================
Select3D_Projector::Select3D_Projector (const gp_Ax2& theCS)
: myPersp (Standard_False),
  myFocus (0.0),
  myType (-1)
{
  myScaledTrsf.SetTransformation (theCS);
  myGTrsf.SetTrsf (myScaledTrsf);
  Scaled();
}

//=======================================================================
//function : Select3D_Projector
//purpose  :
//=======================================================================
Select3D_Projector::Select3D_Projector (const gp_Ax2& theCS, const Standard_Real theFocus)
: myPersp (Standard_True),
  myFocus (theFocus),
  myType (-1)
{
  myScaledTrsf.SetTransformation (theCS);
  myGTrsf.SetTrsf (myScaledTrsf);
  Scaled();
}

//=======================================================================
//function : Select3D_Projector
//purpose  :
//=======================================================================
Select3D_Projector::Select3D_Projector (const gp_Trsf& theViewTrsf,
                                        const Standard_Boolean theIsPersp,
                                        const Standard_Real theFocus)
: myPersp (theIsPersp),
  myFocus (theFocus),
  myGTrsf (theViewTrsf),
  myScaledTrsf (theViewTrsf),
  myType (-1)
{
  Scaled();
}

//=======================================================================
//function : Select3D_Projector
//purpose  :
//=======================================================================
Select3D_Projector::Select3D_Projector (const gp_GTrsf& theViewTrsf,
                                        const Standard_Boolean theIsPersp,
                                        const Standard_Real theFocus)
: myPersp (theIsPersp),
  myFocus (theFocus),
  myGTrsf (theViewTrsf),
  myScaledTrsf (theViewTrsf.Trsf()),
  myType (-1)
{
  Scaled();
}

//=======================================================================
//function : Select3D_Projector
//purpose  :
//=======================================================================
Select3D_Projector::Select3D_Projector (const Graphic3d_Mat4d& theViewTrsf,
                                        const Graphic3d_Mat4d& theProjTrsf)
: myPersp (Standard_False),
  myFocus (0.0),
  myType (-1)
{
  Set (theViewTrsf, theProjTrsf);
}

//=======================================================================
//function : Set
//purpose  :
//=======================================================================
void Select3D_Projector::Set (const gp_Trsf& theViewTrsf,
                              const Standard_Boolean theIsPersp,
                              const Standard_Real theFocus)
{
  myPersp      = theIsPersp;
  myFocus      = theFocus;
  myScaledTrsf = theViewTrsf;
  myProjTrsf.InitIdentity();
  Scaled();
}

//=======================================================================
//function : Set
//purpose  :
//=======================================================================
void Select3D_Projector::Set (const Graphic3d_Mat4d& theViewTrsf,
                              const Graphic3d_Mat4d& theProjTrsf)
{
  // Copy elements corresponding to common view-transformation
  for (Standard_Integer aRowIt = 0; aRowIt < 3; ++aRowIt)
  {
    for (Standard_Integer aColIt = 0; aColIt < 4; ++aColIt)
    {
      myGTrsf.SetValue (aRowIt + 1, aColIt + 1, theViewTrsf.GetValue (aRowIt, aColIt));
    }
  }

  // Adapt scaled transformation for compatibilty
  gp_Dir aViewY (theViewTrsf.GetValue (0, 1), theViewTrsf.GetValue (1, 1), theViewTrsf.GetValue (2, 1));
  gp_Dir aViewZ (theViewTrsf.GetValue (0, 2), theViewTrsf.GetValue (1, 2), theViewTrsf.GetValue (2, 2));
  gp_XYZ aViewT (theViewTrsf.GetValue (0, 3), theViewTrsf.GetValue (1, 3), theViewTrsf.GetValue (2, 3));
  gp_Dir aViewX = aViewY ^ aViewZ;
  gp_Ax3 aViewAx3 (gp_Pnt (aViewT), aViewZ, aViewX);
  myScaledTrsf.SetTransformation (aViewAx3);

  myPersp    = Standard_False;
  myFocus    = 0.0;
  myProjTrsf = theProjTrsf;
  Scaled();
}

//=======================================================================
//function : SetView
//purpose  :
//=======================================================================
void Select3D_Projector::SetView (const Handle(V3d_View)& theView)
{
  const Graphic3d_Mat4d& aViewTrsf = theView->Camera()->OrientationMatrix();
  const Graphic3d_Mat4d& aProjTrsf = theView->Camera()->ProjectionMatrix();

  gp_XYZ aFrameScale = theView->Camera()->ViewDimensions();
  Graphic3d_Mat4d aScale;
  aScale.ChangeValue (0, 0) = aFrameScale.X();
  aScale.ChangeValue (1, 1) = aFrameScale.Y();
  aScale.ChangeValue (2, 2) = aFrameScale.Z();
  Graphic3d_Mat4d aScaledProjTrsf = aScale * aProjTrsf;

  Set (aViewTrsf, aScaledProjTrsf);
}

//=======================================================================
//function : Scaled
//purpose  :
//=======================================================================
void Select3D_Projector::Scaled (const Standard_Boolean theToCheckOptimized)
{
  myType = -1;

  if (!theToCheckOptimized && !myPersp && myProjTrsf.IsIdentity())
  {
    myType = TrsfType (myGTrsf);
  }

  myInvTrsf = myGTrsf.Inverted();
}

//=======================================================================
//function : Project
//purpose  :
//=======================================================================
void Select3D_Projector::Project (const gp_Pnt& theP, gp_Pnt2d& thePout) const
{
  Standard_Real aXout = 0.0;
  Standard_Real aYout = 0.0;
  Standard_Real aZout = 0.0;
  Project (theP, aXout, aYout, aZout);
  thePout.SetCoord (aXout, aYout);
}

//=======================================================================
//function : Project
//purpose  :
//=======================================================================
void Select3D_Projector::Project (const gp_Pnt& theP,
                                  Standard_Real& theX,
                                  Standard_Real& theY,
                                  Standard_Real& theZ) const
{
  Graphic3d_Vec4d aTransformed (0.0, 0.0, 0.0, 1.0);

  // view transformation
  switch (myType)
  {
    case 0 : // inverse axo
    {
      Standard_Real aX07 = theP.X() * 0.7071067811865475;
      Standard_Real aY05 = theP.Y() * 0.5;
      Standard_Real aZ05 = theP.Z() * 0.5;
      aTransformed.x() = aX07 - aY05 + aZ05;
      aTransformed.y() = aX07 + aY05 - aZ05;
      aTransformed.z() = 0.7071067811865475 * (theP.Y() + theP.Z());
      break;
    }

    case 1 : // top
    {
      aTransformed.x() = theP.X();
      aTransformed.y() = theP.Y();
      aTransformed.z() = theP.Z();
      break;
    }

    case 2 : // front
    {
      aTransformed.x() =  theP.X();
      aTransformed.y() =  theP.Z();
      aTransformed.z() = -theP.Y();
      break;
    }

    case 3 : // axo
    {
      Standard_Real aXmy05 = (theP.X() - theP.Y()) * 0.5;
      Standard_Real aZ07 = theP.Z() * 0.7071067811865476;
      aTransformed.x() = 0.7071067811865476 * (theP.X() + theP.Y());
      aTransformed.y() = -aXmy05 + aZ07;
      aTransformed.z() =  aXmy05 + aZ07;
      break;
    }

    default :
    {
      gp_Pnt aTransformPnt = theP;
      Transform (aTransformPnt);
      aTransformed.x() = aTransformPnt.X();
      aTransformed.y() = aTransformPnt.Y();
      aTransformed.z() = aTransformPnt.Z();
    }
  }

  // projection transformation
  if (myPersp)
  {
    // simplified perspective
    Standard_Real aDistortion = 1.0 - aTransformed.z() / myFocus;
    theX = aTransformed.x() / aDistortion;
    theY = aTransformed.y() / aDistortion;
    theZ = aTransformed.z();
    return;
  }

  if (myProjTrsf.IsIdentity())
  {
    // no projection transformation
    theX = aTransformed.x();
    theY = aTransformed.y();
    theZ = aTransformed.z();
    return;
  }

  Graphic3d_Vec4d aProjected = myProjTrsf * aTransformed;

  theX = aProjected.x() / aProjected.w();
  theY = aProjected.y() / aProjected.w();
  theZ = aProjected.z() / aProjected.w();
}

//=======================================================================
//function : Project
//purpose  :
//=======================================================================
void Select3D_Projector::Project (const gp_Pnt& theP,
                                  const gp_Vec& theD1,
                                  gp_Pnt2d& thePout,
                                  gp_Vec2d& theD1out) const
{
  // view transformation
  gp_Pnt aTP = theP;
  Transform (aTP);

  gp_Vec aTD1 = theD1;
  Transform (aTD1);

  // projection transformation
  if (myPersp)
  {
    // simplified perspective
    Standard_Real aDist = 1.0 - aTP.Z() / myFocus;
    thePout.SetCoord (aTP.X() / aDist, aTP.Y() / aDist);
    theD1out.SetCoord (aTD1.X() / aDist + aTP.X() * aTD1.Z() / (myFocus * aDist * aDist),
                       aTD1.Y() / aDist + aTP.Y() * aTD1.Z() / (myFocus * aDist * aDist));
    return;
  }

  if (myProjTrsf.IsIdentity())
  {
    // no projection transformation
    thePout.SetCoord (aTP.X(), aTP.Y());
    theD1out.SetCoord (aTD1.X(), aTD1.Y());
  }

  Graphic3d_Vec4d aTransformedPnt1 (aTP.X(), aTP.Y(), aTP.Z(), 1.0);
  Graphic3d_Vec4d aTransformedPnt2 (aTP.X() + aTD1.X(), aTP.Y() + aTD1.Y(), aTP.Z() + aTD1.Z(), 1.0);

  Graphic3d_Vec4d aProjectedPnt1 = myProjTrsf * aTransformedPnt1;
  Graphic3d_Vec4d aProjectedPnt2 = myProjTrsf * aTransformedPnt2;

  aProjectedPnt1 /= aProjectedPnt1.w();
  aProjectedPnt2 /= aProjectedPnt2.w();

  Graphic3d_Vec4d aProjectedD1 = aProjectedPnt2 - aProjectedPnt1;

  thePout.SetCoord (aProjectedPnt1.x(), aProjectedPnt1.y());
  theD1out.SetCoord (aProjectedD1.x(), aProjectedD1.y());
}

//=======================================================================
//function : Shoot
//purpose  :
//=======================================================================
gp_Lin Select3D_Projector::Shoot (const Standard_Real theX, const Standard_Real theY) const
{
  gp_Lin aViewLin;

  if (myPersp)
  {
    // simplified perspective
    aViewLin = gp_Lin (gp_Pnt (0.0, 0.0, myFocus), gp_Dir (theX, theY, -myFocus));
  }
  else if (myProjTrsf.IsIdentity())
  {
    // no projection transformation
    aViewLin = gp_Lin (gp_Pnt (theX, theY, 0.0), gp_Dir (0.0, 0.0, -1.0));
  }
  else
  {
    // get direction of projection over the point in view space
    Graphic3d_Mat4d aProjInv;
    if (!myProjTrsf.Inverted (aProjInv))
    {
      return gp_Lin();
    }

    Graphic3d_Vec4d aVPnt1 = aProjInv * Graphic3d_Vec4d (theX, theY, 0.0, 1.0);
    Graphic3d_Vec4d aVPnt2 = aProjInv * Graphic3d_Vec4d (theX, theY, 10.0, 1.0);
    aVPnt1 /= aVPnt1.w();
    aVPnt2 /= aVPnt1.w();

    gp_Vec aViewDir (aVPnt2.x() - aVPnt1.x(), aVPnt2.y() - aVPnt1.y(), aVPnt2.z() - aVPnt1.z());

    aViewLin = gp_Lin (gp_Pnt (aVPnt1.x(), aVPnt1.y(), aVPnt1.z()), gp_Dir (aViewDir));
  }

  // view transformation
  Transform (aViewLin, myInvTrsf);

  return aViewLin;
}
