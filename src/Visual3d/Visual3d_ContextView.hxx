// Created on: 1991-09-05
// Created by: NW,JPB,CAL
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Visual3d_ContextView_HeaderFile
#define _Visual3d_ContextView_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_ShortReal.hxx>
#include <Visual3d_TypeOfModel.hxx>
#include <Visual3d_TypeOfVisualization.hxx>
#include <TColStd_SequenceOfAddress.hxx>
#include <Visual3d_TypeOfSurfaceDetail.hxx>
#include <Graphic3d_SequenceOfHClipPlane.hxx>
#include <Standard_Real.hxx>
#include <Visual3d_HSequenceOfLight.hxx>
#include <Standard_Integer.hxx>
class Graphic3d_TextureEnv;
class Visual3d_ClipDefinitionError;
class Visual3d_DepthCueingDefinitionError;
class Visual3d_LightDefinitionError;
class Visual3d_ZClippingDefinitionError;
class Visual3d_Light;


//! This class manages the creation and update of
//! a visualization context for one view in the viewer.
//! A context is defined by :
//! Antialiasing.
//! ZClipping.
//! Depth-cueing.
//! The type of visualization.
//! The light sources.
class Visual3d_ContextView 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a context from default values
  //!
  //! Aliasing		  : OFF
  //! BackZClipping	  : OFF
  //! FrontZClipping	  : OFF
  //! Depth-cueing	  : OFF
  //! Light source	  : 0
  //! Clipping plane	  : 0
  //! Type Of Visualization : TOV_WIREFRAME
  //! Type Of Model	  : TOM_NONE
  //! Type Of SurfaceDetail : TOSD_NONE
  Standard_EXPORT Visual3d_ContextView();
  
  //! Selects the kind of rendering
  //! default to: TOSD_NONE
  Standard_EXPORT void SetSurfaceDetail (const Visual3d_TypeOfSurfaceDetail TOSD);
  
  //! Sets the environment texture to use
  //! no environment texture by default
  Standard_EXPORT void SetTextureEnv (const Handle(Graphic3d_TextureEnv)& ATexture);
  
  //! Activates antialiasing.
  //! Antialiasing can be activated on all the structures
  //! in the view
  Standard_EXPORT void SetAliasingOn();
  
  //! Deactivates the antialiasing.
  Standard_EXPORT void SetAliasingOff();
  
  //! Modifies the back depth-cueing plane.
  //! Category: Methods to modify the class definition
  //! Warning: Raises DepthCueingDefinitionError if <BackPlane>
  //! is front of <FrontPlane> and DepthCueing is ON.
  Standard_EXPORT void SetDepthCueingBackPlane (const Standard_Real ABack);
  
  //! Modifies the front depth-cueing plane.
  //! Category: Methods to modify the class definition
  //! Warning: Raises DepthCueingDefinitionError if <BackPlane> is
  //! front of <FrontPlane> and DepthCueing is ON.
  Standard_EXPORT void SetDepthCueingFrontPlane (const Standard_Real ABack);
  
  //! Activates the depth-cueing.
  //! Depth-cueing can be activated on all structures
  //! present in the view.
  //! Category: Methods to modify the class definition
  //! Warning: Raises DepthCueingDefinitionError if <BackPlane> is
  //! front of <FrontPlane>.
  Standard_EXPORT void SetDepthCueingOn();
  
  //! Deactivates the depth-cueing.
  Standard_EXPORT void SetDepthCueingOff();
  
  //! Returns sequence of clip planes.
  //! @return sequence of clip planes.
  Standard_EXPORT const Graphic3d_SequenceOfHClipPlane& ClipPlanes() const;
  
  //! Change clip planes.
  //! @return sequence of clip planes.
  Standard_EXPORT Graphic3d_SequenceOfHClipPlane& ChangeClipPlanes();
  
  //! Activates the light source <ALight>
  Standard_EXPORT void SetLightOn (const Handle(Visual3d_Light)& ALight);
  
  //! Deactivates the light source <ALight>
  Standard_EXPORT void SetLightOff (const Handle(Visual3d_Light)& ALight);
  
  //! Modifies the shading model when the type of visualization is TOV_SHADING
  Standard_EXPORT void SetModel (const Visual3d_TypeOfModel AModel);
  
  //! Modifies the mode of visualization.
  //!
  //! TypeOfVisualization	: TOV_WIREFRAME
  //! TOV_SHADING
  Standard_EXPORT void SetVisualization (const Visual3d_TypeOfVisualization AVisual);
  
  //! Modifies the back Z-clipping plane.
  //! Category: Methods to modify the class definition
  //! Warning: Raises ZClippingDefinitionError if <BackPlane> is
  //! front of <FrontPlane> and ZClipping is ON.
  Standard_EXPORT void SetZClippingBackPlane (const Standard_Real ABack);
  
  //! Modifies the front Z-clipping plane.
  //! Category: Methods to modify the class definition
  //! Warning: Raises ZClippingDefinitionError if <BackPlane> is
  //! front of <FrontPlane> and ZClipping is ON.
  Standard_EXPORT void SetZClippingFrontPlane (const Standard_Real AFront);
  
  //! Activates the Z-clipping planes defined by
  //! SetZClippingFrontPlane and SetZClippingBackPlane.
  //! Category: Methods to modify the class definition
  //! Warning: Raises ZClippingDefinitionError if <BackPlane> is
  //! front of <FrontPlane>.
  Standard_EXPORT void SetZClippingOn();
  
  //! Deactivates the Z-clipping planes defined by
  //! SetFrontPlane and SetBackPlane.
  Standard_EXPORT void SetZClippingOff();
  
  //! Activates the front Z-clipping plane defined by
  //! SetFrontPlane method.
  Standard_EXPORT void SetFrontZClippingOn();
  
  //! Deactivates the front Z-clipping plane defined by
  //! SetFrontPlane method.
  Standard_EXPORT void SetFrontZClippingOff();
  
  //! Activates the back Z-clipping plane defined by
  //! SetBackPlane method.
  //! Category: Methods to modify the class definition
  //! Warning: Raises ZClippingDefinitionError if <BackPlane> is
  //! front of <FrontPlane>.
  Standard_EXPORT void SetBackZClippingOn();
  
  //! Deactivates the back Z-clipping plane defined by
  //! SetBackPlane method.
  Standard_EXPORT void SetBackZClippingOff();
  
  //! Returns the group of active light sources
  //! in the view of context <me>.
  Standard_EXPORT Handle(Visual3d_HSequenceOfLight) ActivatedLights() const;
  
  //! Returns the number of active light sources
  //! in the view of context <me>.
  Standard_EXPORT Standard_Integer NumberOfActivatedLights() const;
  
  Standard_EXPORT Handle(Visual3d_Light) ActivatedLight (const Standard_Integer AnIndex) const;
  
  //! Returns the activity of the aliasing.
  Standard_EXPORT Standard_Boolean AliasingIsOn() const;
  
  //! Returns the activity of the ZClipping.
  Standard_EXPORT Standard_Boolean BackZClippingIsOn() const;
  
  //! Returns the definition of the back depth-cueing plane.
  Standard_EXPORT Standard_Real DepthCueingBackPlane() const;
  
  //! Returns the definition of the front depth-cueing plane.
  Standard_EXPORT Standard_Real DepthCueingFrontPlane() const;
  
  //! Returns the activity of the depth-cueing.
  Standard_EXPORT Standard_Boolean DepthCueingIsOn() const;
  
  //! Returns the activity of the ZClipping.
  Standard_EXPORT Standard_Boolean FrontZClippingIsOn() const;
  
  //! Returns the shading model.
  Standard_EXPORT Visual3d_TypeOfModel Model() const;
  
  //! Returns the mode of visualization.
  Standard_EXPORT Visual3d_TypeOfVisualization Visualization() const;
  
  //! Returns the definition of the back Z-clipping plane.
  Standard_EXPORT Standard_Real ZClippingBackPlane() const;
  
  //! Returns the definition of the front Z-clipping plane.
  Standard_EXPORT Standard_Real ZClippingFrontPlane() const;
  
  Standard_EXPORT Visual3d_TypeOfSurfaceDetail SurfaceDetail() const;
  
  Standard_EXPORT Handle(Graphic3d_TextureEnv) TextureEnv() const;




protected:





private:



  Standard_Boolean AliasingIsActive;
  Standard_Boolean ZcueingIsActive;
  Standard_Boolean FrontZclippingIsActive;
  Standard_Boolean BackZclippingIsActive;
  Standard_ShortReal MyZclippingFrontPlane;
  Standard_ShortReal MyZclippingBackPlane;
  Standard_ShortReal MyDepthCueingFrontPlane;
  Standard_ShortReal MyDepthCueingBackPlane;
  Visual3d_TypeOfModel MyModel;
  Visual3d_TypeOfVisualization MyVisual;
  TColStd_SequenceOfAddress MyLights;
  Handle(Graphic3d_TextureEnv) MyTextureEnv;
  Visual3d_TypeOfSurfaceDetail MySurfaceDetail;
  Graphic3d_SequenceOfHClipPlane myClipPlanes;


};







#endif // _Visual3d_ContextView_HeaderFile
