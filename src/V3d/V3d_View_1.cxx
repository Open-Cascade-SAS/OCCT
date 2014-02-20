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

#include <V3d_View.jxx>

/*----------------------------------------------------------------------*/

void V3d_View::SetAntialiasingOn() {
  MyViewContext.SetAliasingOn() ;
  MyView->SetContext(MyViewContext) ;
}

void V3d_View::SetAntialiasingOff() {
  MyViewContext.SetAliasingOff() ;
  MyView->SetContext(MyViewContext) ;
}

/*----------------------------------------------------------------------*/

void V3d_View::SetZClippingDepth(const Standard_Real Depth) {
  Standard_Real Front,Back,Width ;

  Front = MyViewContext.ZClippingFrontPlane() ;
  Back = MyViewContext.ZClippingBackPlane() ;
  Width = Front - Back ;
  Front = Depth + Width/2. ;
  Back = Depth - Width/2. ;
  MyViewContext.SetZClippingBackPlane(Back) ;
  MyViewContext.SetZClippingFrontPlane(Front) ;
  MyView->SetContext(MyViewContext) ;
}

/*----------------------------------------------------------------------*/

void V3d_View::SetZClippingWidth(const Standard_Real Width) {
  Standard_Real Front,Back,Depth ;
  V3d_BadValue_Raise_if( Width <= 0.,"V3d_View::SetZClippingWidth, bad width");

  Front = MyViewContext.ZClippingFrontPlane() ;
  Back = MyViewContext.ZClippingBackPlane() ;
  Depth = (Front + Back)/2. ;
  Front = Depth + Width/2. ;
  Back = Depth - Width/2. ;
  MyViewContext.SetZClippingBackPlane(Back) ;
  MyViewContext.SetZClippingFrontPlane(Front) ;
  MyView->SetContext(MyViewContext) ;
}

/*----------------------------------------------------------------------*/

void V3d_View::SetZClippingType(const V3d_TypeOfZclipping Type) {

  switch (Type) {
  case V3d_OFF :
    MyViewContext.SetZClippingOff() ;
    break ;
  case V3d_BACK :
    MyViewContext.SetBackZClippingOn() ;
    MyViewContext.SetFrontZClippingOff() ;
    break ;
  case V3d_FRONT :
    MyViewContext.SetFrontZClippingOn() ;
    MyViewContext.SetBackZClippingOff() ;
    break ;
  case V3d_SLICE :
    MyViewContext.SetZClippingOn() ;
    break ;
  }
  MyView->SetContext(MyViewContext) ;
}

/*----------------------------------------------------------------------*/

V3d_TypeOfZclipping V3d_View::ZClipping(Standard_Real& Depth, Standard_Real& Width) const {
  V3d_TypeOfZclipping T = V3d_OFF ;
  Standard_Real Front,Back ;
  Standard_Boolean Aback,Afront ;

  Back = MyViewContext.ZClippingBackPlane() ;
  Front = MyViewContext.ZClippingFrontPlane() ;
  Aback = MyViewContext.BackZClippingIsOn() ;
  Afront = MyViewContext.FrontZClippingIsOn() ;
  Width = Front - Back ;
  Depth = (Front + Back)/2. ;
  if( Aback ) {
    if( Afront ) T = V3d_SLICE ;
    else	 T = V3d_BACK ;
  } else if( Afront ) T = V3d_FRONT ;

  return T ;
}


/*----------------------------------------------------------------------*/

void V3d_View::SetZCueingDepth(const Standard_Real Depth) {
  Standard_Real Front,Back,Width ;

  Front = MyViewContext.DepthCueingFrontPlane() ;
  Back = MyViewContext.DepthCueingBackPlane() ;
  Width = Front - Back ;
  Front = Depth + Width/2. ;
  Back = Depth - Width/2. ;
  MyViewContext.SetDepthCueingBackPlane(Back) ;
  MyViewContext.SetDepthCueingFrontPlane(Front) ;
  MyView->SetContext(MyViewContext) ;
}

/*----------------------------------------------------------------------*/

void V3d_View::SetZCueingWidth(const Standard_Real Width) {
  Standard_Real Front,Back,Depth ;
  V3d_BadValue_Raise_if( Width <= 0.,"V3d_View::SetZCueingWidth, bad width");

  Front = MyViewContext.DepthCueingFrontPlane() ;
  Back = MyViewContext.DepthCueingBackPlane() ;
  Depth = (Front + Back)/2. ;
  Front = Depth + Width/2. ;
  Back = Depth - Width/2. ;
  MyViewContext.SetDepthCueingBackPlane(Back) ;
  MyViewContext.SetDepthCueingFrontPlane(Front) ;
  MyView->SetContext(MyViewContext) ;
}

/*----------------------------------------------------------------------*/

void V3d_View::SetZCueingOn() {

  MyViewContext.SetDepthCueingOn() ;
  MyView->SetContext(MyViewContext) ;
}

/*----------------------------------------------------------------------*/

void V3d_View::SetZCueingOff() {

  MyViewContext.SetDepthCueingOff() ;
  MyView->SetContext(MyViewContext) ;
}

/*----------------------------------------------------------------------*/

Standard_Boolean V3d_View::ZCueing(Standard_Real& Depth, Standard_Real& Width)const  {
  Standard_Boolean Z = MyViewContext.DepthCueingIsOn() ;
  Standard_Real Front,Back ;

Back = MyViewContext.DepthCueingBackPlane() ;
  Front = MyViewContext.DepthCueingFrontPlane() ;
  Width = Front - Back ;
  Depth = (Front + Back)/2. ;

  return Z ;
}

/*----------------------------------------------------------------------*/
