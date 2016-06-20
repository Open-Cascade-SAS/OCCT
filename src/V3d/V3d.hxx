// Created on: 1992-11-13
// Created by: GG
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

#ifndef _V3d_HeaderFile
#define _V3d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <V3d_TypeOfOrientation.hxx>
#include <V3d_Coordinate.hxx>
#include <V3d_Parameter.hxx>
#include <Quantity_PlaneAngle.hxx>
#include <Quantity_Length.hxx>
class Graphic3d_Vector;
class Graphic3d_Group;
class V3d_View;
class V3d_Viewer;
class V3d_Viewer;
class V3d_View;
class V3d_Light;
class V3d_AmbientLight;
class V3d_PositionLight;
class V3d_PositionalLight;
class V3d_DirectionalLight;
class V3d_SpotLight;
class V3d_RectangularGrid;
class V3d_CircularGrid;
class V3d_ColorScale;
class V3d_ColorScaleLayerItem;


//! This package contains the set of commands and services
//! of the 3D Viewer. It provides a set of high level commands
//! to control the views and viewing modes.
class V3d 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Determines the orientation vector corresponding
  //! to the predefined orientation type.
  Standard_EXPORT static Graphic3d_Vector GetProjAxis (const V3d_TypeOfOrientation Orientation);
  
  //! Compute the graphic structure of arrow.
  //! X0,Y0,Z0 : coordinate of the arrow.
  //! DX,DY,DZ : Direction of the arrow.
  //! Alpha    : Angle of arrow.
  //! Lng      : Length of arrow.
  Standard_EXPORT static void ArrowOfRadius (const Handle(Graphic3d_Group)& garrow, const V3d_Coordinate X0, const V3d_Coordinate Y0, const V3d_Coordinate Z0, const V3d_Parameter DX, const V3d_Parameter DY, const V3d_Parameter DZ, const Quantity_PlaneAngle Alpha, const V3d_Parameter Lng);
  
  //! Compute the graphic structure of circle.
  //! X0,Y0,Z0 : Center of circle.
  //! VX,VY,VZ : Axis of circle.
  //! Radius   : Radius of circle.
  Standard_EXPORT static void CircleInPlane (const Handle(Graphic3d_Group)& gcircle, const V3d_Coordinate X0, const V3d_Coordinate Y0, const V3d_Coordinate Z0, const V3d_Parameter VX, const V3d_Parameter VY, const V3d_Parameter VZ, const V3d_Parameter Radius);
  
  Standard_EXPORT static void SwitchViewsinWindow (const Handle(V3d_View)& aPreviousView, const Handle(V3d_View)& aNextView);

friend class V3d_Viewer;
friend class V3d_View;
friend class V3d_Light;
friend class V3d_AmbientLight;
friend class V3d_PositionLight;
friend class V3d_PositionalLight;
friend class V3d_DirectionalLight;
friend class V3d_SpotLight;
friend class V3d_RectangularGrid;
friend class V3d_CircularGrid;
friend class V3d_ColorScale;
friend class V3d_ColorScaleLayerItem;

};







#endif // _V3d_HeaderFile
