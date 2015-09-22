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

/***********************************************************************/
/*     FUNCTION :
     ----------
        File V3d_View_1.cxx :     */
/*----------------------------------------------------------------------*/
/*
 * Includes
  */

#include <Aspect_GradientBackground.hxx>
#include <Aspect_Grid.hxx>
#include <Aspect_Window.hxx>
#include <Bnd_Box.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_TextureEnv.hxx>
#include <Graphic3d_Vector.hxx>
#include <Quantity_Color.hxx>
#include <Standard_MultiplyDefined.hxx>
#include <Standard_TypeMismatch.hxx>
#include <V3d_BadValue.hxx>
#include <V3d_Light.hxx>
#include <V3d_UnMapped.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

// =======================================================================
// function : SetAntialiasingOn
// purpose  :
// =======================================================================
void V3d_View::SetAntialiasingOn()
{
  myView->SetAntialiasingEnabled (Standard_True);
}

// =======================================================================
// function : SetAntialiasingOff
// purpose  :
// =======================================================================
void V3d_View::SetAntialiasingOff()
{
  myView->SetAntialiasingEnabled (Standard_False);
}

//=============================================================================
//function : SetZClippingDepth
//purpose  :
//=============================================================================
void V3d_View::SetZClippingDepth (const Standard_Real theDepth)
{
  Standard_Real aFront = 0., aBack = 0., aWidth = 0.;

  aFront = myView->ZClippingFrontPlane();
  aBack  = myView->ZClippingBackPlane();
  aWidth = aFront - aBack;
  aFront = theDepth + aWidth / 2.;
  aBack  = theDepth - aWidth / 2.;
  myView->SetZClippingBackPlane  (aBack);
  myView->SetZClippingFrontPlane (aFront);
}

//=============================================================================
//function : SetZClippingWidth
//purpose  :
//=============================================================================
void V3d_View::SetZClippingWidth (const Standard_Real theWidth)
{
  Standard_Real aFront = 0., aBack = 0., aDepth = 0.;
  V3d_BadValue_Raise_if (theWidth <= 0., "V3d_View::SetZClippingWidth, bad width");

  aFront = myView->ZClippingFrontPlane();
  aBack  = myView->ZClippingBackPlane();
  aDepth = (aFront + aBack)  / 2.;
  aFront = aDepth + theWidth / 2.;
  aBack  = aDepth - theWidth / 2.;
  myView->SetZClippingBackPlane  (aBack);
  myView->SetZClippingFrontPlane (aFront);
}

//=============================================================================
//function : SetZClippingType
//purpose  :
//=============================================================================
void V3d_View::SetZClippingType (const V3d_TypeOfZclipping theType)
{
  switch (theType)
  {
    case V3d_OFF :
    myView->SetFrontZClippingOn (Standard_False);
    myView->SetBackZClippingOn  (Standard_False);
    break;

    case V3d_BACK :
    myView->SetBackZClippingOn  (Standard_True);
    myView->SetFrontZClippingOn (Standard_False);
    break;

    case V3d_FRONT :
    myView->SetFrontZClippingOn (Standard_True);
    myView->SetBackZClippingOn  (Standard_False);
    break;

    case V3d_SLICE :
    myView->SetFrontZClippingOn (Standard_False);
    myView->SetBackZClippingOn  (Standard_False);
    break;
  }
}

//=============================================================================
//function : ZClipping
//purpose  :
//=============================================================================
V3d_TypeOfZclipping V3d_View::ZClipping (Standard_Real& theDepth, Standard_Real& theWidth) const
{
  V3d_TypeOfZclipping aType = V3d_OFF;

  Standard_Real aBack        = myView->ZClippingBackPlane();
  Standard_Real aFront       = myView->ZClippingFrontPlane();
  Standard_Boolean isBackOn  = myView->BackZClippingIsOn();
  Standard_Boolean isFrontOn = myView->FrontZClippingIsOn();

  theWidth = aFront - aBack ;
  theDepth = (aFront + aBack) / 2.0;
  if (isBackOn)
  {
    if (isFrontOn)
    {
      aType = V3d_SLICE;
    }
    else
    {
      aType = V3d_BACK;
    }
  }
  else if (isFrontOn)
  {
    aType = V3d_FRONT;
  }

  return aType;
}

//=============================================================================
//function : SetZCueingDepth
//purpose  :
//=============================================================================
void V3d_View::SetZCueingDepth (const Standard_Real theDepth)
{
  Standard_Real aFront = myView->DepthCueingFrontPlane();
  Standard_Real aBack  = myView->DepthCueingBackPlane();
  Standard_Real aWidth = aFront - aBack;
  aFront = theDepth + aWidth / 2.;
  aBack  = theDepth - aWidth / 2.;
  myView->SetDepthCueingBackPlane  (aBack);
  myView->SetDepthCueingFrontPlane (aFront);
}

//=============================================================================
//function : SetZCueingWidth
//purpose  :
//=============================================================================
void V3d_View::SetZCueingWidth (const Standard_Real theWidth)
{
  V3d_BadValue_Raise_if (theWidth <= 0., "V3d_View::SetZCueingWidth, bad width");

  Standard_Real aFront = myView->DepthCueingFrontPlane();
  Standard_Real aBack  = myView->DepthCueingBackPlane();
  Standard_Real aDepth = (aFront + aBack) / 2.0;
  aFront = aDepth + theWidth / 2.0;
  aBack  = aDepth - theWidth / 2.0;
  myView->SetDepthCueingBackPlane  (aBack);
  myView->SetDepthCueingFrontPlane (aFront);
}

//=============================================================================
//function : SetZCueingOn
//purpose  :
//=============================================================================
void V3d_View::SetZCueingOn()
{
  myView->SetDepthCueingOn (Standard_True);
}

//=============================================================================
//function : SetZCueingOff
//purpose  :
//=============================================================================
void V3d_View::SetZCueingOff()
{
  myView->SetDepthCueingOn (Standard_False);
}

//=============================================================================
//function : ZCueing
//purpose  :
//=============================================================================
Standard_Boolean V3d_View::ZCueing (Standard_Real& theDepth, Standard_Real& theWidth) const
{
  Standard_Boolean isOn = myView->DepthCueingIsOn();
  Standard_Real aFront  = myView->DepthCueingFrontPlane();
  Standard_Real aBack   = myView->DepthCueingBackPlane();

  theWidth = aFront - aBack ;
  theDepth = (aFront + aBack) / 2.;

  return isOn;
}
