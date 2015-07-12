// Created on: 1992-01-22
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

#ifndef _V3d_DirectionalLight_HeaderFile
#define _V3d_DirectionalLight_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Graphic3d_Vertex.hxx>
#include <V3d_PositionLight.hxx>
#include <V3d_TypeOfOrientation.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Standard_Boolean.hxx>
#include <V3d_Coordinate.hxx>
#include <Quantity_Parameter.hxx>
#include <Standard_Real.hxx>
#include <V3d_TypeOfRepresentation.hxx>
class V3d_BadValue;
class V3d_Viewer;
class V3d_View;
class Graphic3d_Group;


class V3d_DirectionalLight;
DEFINE_STANDARD_HANDLE(V3d_DirectionalLight, V3d_PositionLight)

//! Create and modify a directional light source
//! in a viewer.
class V3d_DirectionalLight : public V3d_PositionLight
{

public:

  
  //! Creates a directional light source in the viewer.
  Standard_EXPORT V3d_DirectionalLight(const Handle(V3d_Viewer)& VM, const V3d_TypeOfOrientation Direction = V3d_XposYposZpos, const Quantity_NameOfColor Color = Quantity_NOC_WHITE, const Standard_Boolean Headlight = Standard_False);
  
  //! Creates a directional light source in the viewer.
  //! Xt,Yt,Zt : Coordinate of light source Target.
  //! Xp,Yp,Zp : Coordinate of light source Position.
  //! The others parameters describe before.
  Standard_EXPORT V3d_DirectionalLight(const Handle(V3d_Viewer)& VM, const V3d_Coordinate Xt, const V3d_Coordinate Yt, const V3d_Coordinate Zt, const V3d_Coordinate Xp, const V3d_Coordinate Yp, const V3d_Coordinate Zp, const Quantity_NameOfColor Color = Quantity_NOC_WHITE, const Standard_Boolean Headlight = Standard_False);
  
  //! Defines the direction of the light source
  //! by a predefined orientation.
  Standard_EXPORT void SetDirection (const V3d_TypeOfOrientation Direction);
  
  //! Defines the direction of the light source by the predefined
  //! vector Xm,Ym,Zm.
  //! Warning: raises  BadValue from V3d if the vector is null.
  Standard_EXPORT void SetDirection (const Quantity_Parameter Xm, const Quantity_Parameter Ym, const Quantity_Parameter Zm);
  
  //! Defines the point of light source representation.
  Standard_EXPORT void SetDisplayPosition (const V3d_Coordinate X, const V3d_Coordinate Y, const V3d_Coordinate Z);
  
  //! Calls SetDisplayPosition method.
  Standard_EXPORT virtual void SetPosition (const V3d_Coordinate Xp, const V3d_Coordinate Yp, const V3d_Coordinate Zp) Standard_OVERRIDE;
  
  //! Modifies the smoothing angle (in radians)
  Standard_EXPORT void SetSmoothAngle (const Standard_Real theValue);
  
  //! Display the graphic structure of light source
  //! in the choosen view. We have three type of representation
  //! - SIMPLE   : Only the light source is displayed.
  //! - PARTIAL  : The light source and the light space are
  //! displayed.
  //! - COMPLETE : The same representation as PARTIAL.
  //! We can choose the "SAMELAST" as parameter of representation
  //! In this case the graphic structure representation will be
  //! the last displayed.
  Standard_EXPORT void Display (const Handle(V3d_View)& aView, const V3d_TypeOfRepresentation Representation) Standard_OVERRIDE;
  
  //! Calls DisplayPosition method.
  Standard_EXPORT virtual void Position (V3d_Coordinate& X, V3d_Coordinate& Y, V3d_Coordinate& Z) const Standard_OVERRIDE;
  
  //! Returns the choosen position to represent the light
  //! source.
  Standard_EXPORT void DisplayPosition (V3d_Coordinate& X, V3d_Coordinate& Y, V3d_Coordinate& Z) const;
  
  //! Returns the Vx,Vy,Vz direction of the light source.
  Standard_EXPORT void Direction (Quantity_Parameter& Vx, Quantity_Parameter& Vy, Quantity_Parameter& Vz) const;




  DEFINE_STANDARD_RTTI(V3d_DirectionalLight,V3d_PositionLight)

protected:




private:

  
  //! Defines the representation of the directional light source.
  Standard_EXPORT void Symbol (const Handle(Graphic3d_Group)& gsymbol, const Handle(V3d_View)& aView) const Standard_OVERRIDE;

  Graphic3d_Vertex MyDisplayPosition;


};







#endif // _V3d_DirectionalLight_HeaderFile
