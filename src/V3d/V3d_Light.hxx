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

#include <Graphic3d_CLight.hxx>
#include <Graphic3d_Vertex.hxx>
#include <Standard_Transient.hxx>
#include <Quantity_Color.hxx>
#include <Standard.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Standard_Type.hxx>
#include <V3d_TypeOfLight.hxx>
#include <V3d_View.hxx>

class Graphic3d_Structure;
class V3d_Viewer;

class V3d_Light;
DEFINE_STANDARD_HANDLE(V3d_Light, Standard_Transient)

//! Defines services on Light type objects..
//! (base class for AmbientLight and PositionLight)
class V3d_Light : public Standard_Transient
{
public:

  //! Defines the color of a light source by giving the basic color.
  Standard_EXPORT void SetColor (const Quantity_Color& theColor);

  //! Returns the color of the light source.
  Quantity_Color Color() const { return Quantity_Color (myLight.Color.rgb()); }

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
  //! Updates the lights of the view. The view is redrawn.
  Standard_EXPORT void V3d_View::UpdateLights() const;

  DEFINE_STANDARD_RTTIEXT(V3d_Light,Standard_Transient)

protected:

  Standard_EXPORT V3d_Light (const Handle(V3d_Viewer)& theViewer);

  //! Sets type of the light.
  Standard_EXPORT void SetType (const V3d_TypeOfLight theType);

  //! Returns the symetric point coordinates of "aPoint"
  //! on the sphere of center "Center" and radius "Radius".
  //! VX,VY,VZ is the project vector of view.
  Standard_EXPORT static void SymetricPointOnSphere (const Handle(V3d_View)& aView,
                                                     const Graphic3d_Vertex& Center,
                                                     const Graphic3d_Vertex& aPoint,
                                                     const Standard_Real Radius,
                                                     Standard_Real& X, Standard_Real& Y, Standard_Real& Z,
                                                     Standard_Real& VX, Standard_Real& VY, Standard_Real& VZ);

protected:

  //! Return light properties associated to this light source.
  //! Hidden method exposed only to V3d_View.
  Standard_EXPORT const Graphic3d_CLight& Light() const { return myLight; }

protected:

  Graphic3d_CLight myLight;
  Handle(Graphic3d_Structure) myGraphicStructure;
  Handle(Graphic3d_Structure) myGraphicStructure1;
};

#endif // _V3d_Light_HeaderFile
