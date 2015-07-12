// Created on: 1992-01-17
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

#ifndef _V3d_Light_HeaderFile
#define _V3d_Light_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <V3d_TypeOfLight.hxx>
#include <MMgt_TShared.hxx>
#include <V3d_View.hxx>
#include <Quantity_TypeOfColor.hxx>
#include <Quantity_Parameter.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Graphic3d_Vertex.hxx>
#include <V3d_Coordinate.hxx>
class Visual3d_Light;
class Graphic3d_Structure;
class V3d_BadValue;
class V3d_Viewer;
class Quantity_Color;
class V3d_View;


class V3d_Light;
DEFINE_STANDARD_HANDLE(V3d_Light, MMgt_TShared)

//! Defines services on Light type objects..
//! (base class for AmbientLight and PositionLight)
class V3d_Light : public MMgt_TShared
{

public:

  
  //! Defines the colour of a light source
  //! according to the type of colour definition
  //! and the three corresponding values.
  Standard_EXPORT void SetColor (const Quantity_TypeOfColor Type, const Quantity_Parameter V1, const Quantity_Parameter V2, const Quantity_Parameter V3);
  
  //! Defines the colour of a light source by giving
  //! the name of the colour in the form Quantity_NOC_xxxx .
  Standard_EXPORT void SetColor (const Quantity_NameOfColor Name);
  
  //! Defines the colour of a light source by giving
  //! the basic colour.
  Standard_EXPORT void SetColor (const Quantity_Color& Name);
  
  //! Returns the colour of the light source depending of
  //! the color type.
  Standard_EXPORT void Color (const Quantity_TypeOfColor Type, Quantity_Parameter& V1, Quantity_Parameter& V2, Quantity_Parameter& V3) const;
  
  //! Returns the colour of the light source.
  Standard_EXPORT void Color (Quantity_NameOfColor& Name) const;
  
  //! Returns the colour of the light source.
  Standard_EXPORT Quantity_Color Color() const;
  
  //! Returns the Type of the Light
  Standard_EXPORT V3d_TypeOfLight Type() const;
  
  //! returns true if the light is a headlight
  Standard_EXPORT Standard_Boolean Headlight() const;
  
  //! Setup headlight flag.
  Standard_EXPORT void SetHeadlight (const Standard_Boolean theValue);
  
  //! Modifies the intensity of light source.
  Standard_EXPORT void SetIntensity (const Standard_Real theValue);
  
  //! returns the intensity of light source
  Standard_EXPORT Standard_Real Intensity() const;
  
  //! returns the smoothness of light source
  Standard_EXPORT Standard_Real Smoothness() const;
  
  //! Returns TRUE when a light representation is displayed
  Standard_EXPORT Standard_Boolean IsDisplayed() const;


friend   
  //! Activates MyLight in the view.
  Standard_EXPORT void V3d_View::SetLightOn (const Handle(V3d_Light)& MyLight);
friend   
  //! Activates all the lights defined in this view.
  Standard_EXPORT void V3d_View::SetLightOn();
friend   
  //! Desactivate MyLight in this view.
  Standard_EXPORT void V3d_View::SetLightOff (const Handle(V3d_Light)& MyLight);
friend   
  //! Deactivate all the Lights defined in this view.
  Standard_EXPORT void V3d_View::SetLightOff();


  DEFINE_STANDARD_RTTI(V3d_Light,MMgt_TShared)

protected:

  
  Standard_EXPORT V3d_Light(const Handle(V3d_Viewer)& VM);
  
  //! Returns the symetric point coordinates of "aPoint"
  //! on the sphere of center "Center" and radius "Radius".
  //! VX,VY,VZ is the project vector of view.
  Standard_EXPORT static void SymetricPointOnSphere (const Handle(V3d_View)& aView, const Graphic3d_Vertex& Center, const Graphic3d_Vertex& aPoint, const Quantity_Parameter Radius, V3d_Coordinate& X, V3d_Coordinate& Y, V3d_Coordinate& Z, Quantity_Parameter& VX, Quantity_Parameter& VY, Quantity_Parameter& VZ);

  V3d_TypeOfLight MyType;
  Handle(Visual3d_Light) MyLight;
  Handle(Graphic3d_Structure) MyGraphicStructure;
  Handle(Graphic3d_Structure) MyGraphicStructure1;


private:

  
  //! Returns the Light of the associated Visual3d.
  Standard_EXPORT Handle(Visual3d_Light) Light() const;



};







#endif // _V3d_Light_HeaderFile
