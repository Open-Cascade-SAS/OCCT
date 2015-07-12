// Created on: 1997-11-21
// Created by: ZOV
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _V3d_PositionLight_HeaderFile
#define _V3d_PositionLight_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Graphic3d_Vertex.hxx>
#include <V3d_TypeOfRepresentation.hxx>
#include <V3d_Light.hxx>
#include <V3d_Coordinate.hxx>
#include <Quantity_Parameter.hxx>
#include <V3d_TypeOfPickLight.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class V3d_BadValue;
class V3d_Viewer;
class V3d_View;
class Graphic3d_Group;


class V3d_PositionLight;
DEFINE_STANDARD_HANDLE(V3d_PositionLight, V3d_Light)

//! Base class for Positional, Spot and Directional Light classes
class V3d_PositionLight : public V3d_Light
{

public:

  
  //! Defines the position of the light source. Should be redefined!
  Standard_EXPORT virtual void SetPosition (const V3d_Coordinate X, const V3d_Coordinate Y, const V3d_Coordinate Z) = 0;
  
  //! Defines the target of the light (the center
  //! of the sphere)
  Standard_EXPORT void SetTarget (const V3d_Coordinate X, const V3d_Coordinate Y, const V3d_Coordinate Z);
  
  //! Define the radius.
  Standard_EXPORT void SetRadius (const Quantity_Parameter Radius);
  
  //! Calculate the position of the light, on the hide face
  //! of the picking sphere.
  Standard_EXPORT void OnHideFace (const Handle(V3d_View)& aView);
  
  //! Calculate the position of the light, on the seen face
  //! of the picking sphere.
  Standard_EXPORT void OnSeeFace (const Handle(V3d_View)& aView);
  
  //! Tracking the light position, or the light space,
  //! or the radius of the light space, that depends of
  //! initial picking "WhatPick" (see the pick method).
  //! If WhatPick is SPACELIGHT, then the parameters
  //! Xpix, Ypix are the coordinates of a translation vector.
  Standard_EXPORT void Tracking (const Handle(V3d_View)& aView, const V3d_TypeOfPickLight WathPick, const Standard_Integer Xpix, const Standard_Integer Ypix);
  
  //! Display the graphic structure of light source
  //! in the choosen view. We have three type of representation
  //! - SIMPLE   : Only the light source is displayed.
  //! - PARTIAL  : The light source and the light space are
  //! displayed.
  //! - COMPLETE : The light source, the light space and the
  //! radius of light space are displayed.
  //! We can choose the "SAMELAST" as parameter of representation
  //! In this case the graphic structure representation will be
  //! the last displayed.
  Standard_EXPORT virtual void Display (const Handle(V3d_View)& aView, const V3d_TypeOfRepresentation Representation = V3d_SIMPLE);
  
  //! Erase the graphic structure of light source.
  Standard_EXPORT void Erase();
  
  //! Returns the radius of the picking sphere.
  Standard_EXPORT Quantity_Parameter Radius() const;
  
  //! Returns the visibility status
  //! If True the source is visible.
  //! If False it's hidden.
  Standard_EXPORT Standard_Boolean SeeOrHide (const Handle(V3d_View)& aView) const;
  
  //! Returns the position of the light source.
  Standard_EXPORT virtual void Position (V3d_Coordinate& X, V3d_Coordinate& Y, V3d_Coordinate& Z) const = 0;
  
  //! Returns the position of the target of the light source.
  Standard_EXPORT void Target (V3d_Coordinate& X, V3d_Coordinate& Y, V3d_Coordinate& Z) const;




  DEFINE_STANDARD_RTTI(V3d_PositionLight,V3d_Light)

protected:

  
  Standard_EXPORT V3d_PositionLight(const Handle(V3d_Viewer)& VM);

  Graphic3d_Vertex MyTarget;
  V3d_TypeOfRepresentation MyTypeOfRepresentation;


private:

  
  //! Defines representation of the light source.
  Standard_EXPORT virtual void Symbol (const Handle(Graphic3d_Group)& gsymbol, const Handle(V3d_View)& aView) const = 0;



};







#endif // _V3d_PositionLight_HeaderFile
