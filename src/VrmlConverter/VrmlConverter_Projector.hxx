// Created on: 1997-06-06
// Created by: Alexander BRIVIN
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

#ifndef _VrmlConverter_Projector_HeaderFile
#define _VrmlConverter_Projector_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <HLRAlgo_Projector.hxx>
#include <Vrml_PerspectiveCamera.hxx>
#include <Vrml_OrthographicCamera.hxx>
#include <Vrml_DirectionalLight.hxx>
#include <Vrml_PointLight.hxx>
#include <Vrml_SpotLight.hxx>
#include <VrmlConverter_TypeOfCamera.hxx>
#include <VrmlConverter_TypeOfLight.hxx>
#include <Vrml_MatrixTransform.hxx>
#include <Standard_Transient.hxx>
#include <TopTools_Array1OfShape.hxx>
#include <Standard_OStream.hxx>


class VrmlConverter_Projector;
DEFINE_STANDARD_HANDLE(VrmlConverter_Projector, Standard_Transient)


//! defines projector  and calculates properties of cameras and lights from Vrml
//! ( OrthograpicCamera, PerspectiveCamera, DirectionalLight, PointLight, SpotLight
//! and  MatrixTransform  )  to display all scene  shapes  with  arbitrary locations
//! for requested the Projection Vector,  High Point Direction and the Focus
//! and adds them ( method Add ) to anOSream.
class VrmlConverter_Projector : public Standard_Transient
{

public:

  
  Standard_EXPORT VrmlConverter_Projector(const TopTools_Array1OfShape& Shapes, const Standard_Real Focus, const Standard_Real DX, const Standard_Real DY, const Standard_Real DZ, const Standard_Real XUp, const Standard_Real YUp, const Standard_Real ZUp, const VrmlConverter_TypeOfCamera Camera = VrmlConverter_NoCamera, const VrmlConverter_TypeOfLight Light = VrmlConverter_NoLight);
  
  Standard_EXPORT void SetCamera (const VrmlConverter_TypeOfCamera aCamera);
  
  Standard_EXPORT VrmlConverter_TypeOfCamera Camera() const;
  
  Standard_EXPORT void SetLight (const VrmlConverter_TypeOfLight aLight);
  
  Standard_EXPORT VrmlConverter_TypeOfLight Light() const;
  

  //! Adds  into anOStream  if  they  are  defined in  Create.
  //! PerspectiveCamera,
  //! OrthographicCamera,
  //! DirectionLight,
  //! PointLight,
  //! SpotLight
  //! with  MatrixTransform  from VrmlConverter;
  Standard_EXPORT void Add (Standard_OStream& anOStream) const;
  
  Standard_EXPORT HLRAlgo_Projector Projector() const;

  DEFINE_STANDARD_RTTIEXT(VrmlConverter_Projector,Standard_Transient)

private:


  HLRAlgo_Projector myProjector;
  Vrml_PerspectiveCamera myPerspectiveCamera;
  Vrml_OrthographicCamera myOrthographicCamera;
  Vrml_DirectionalLight myDirectionalLight;
  Vrml_PointLight myPointLight;
  Vrml_SpotLight mySpotLight;
  VrmlConverter_TypeOfCamera myTypeOfCamera;
  VrmlConverter_TypeOfLight myTypeOfLight;
  Vrml_MatrixTransform myMatrixTransform;


};

#endif // _VrmlConverter_Projector_HeaderFile
