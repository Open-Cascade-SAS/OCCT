// Created on: 1991-11-04
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

#ifndef _Graphic3d_AspectFillArea3d_HeaderFile
#define _Graphic3d_AspectFillArea3d_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <Standard_Integer.hxx>
#include <Standard_ShortReal.hxx>
#include <Graphic3d_ShaderProgram.hxx>
#include <Aspect_AspectFillArea.hxx>
#include <Aspect_InteriorStyle.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Standard_Real.hxx>
class Graphic3d_TextureMap;
class Quantity_Color;
class Graphic3d_MaterialAspect;


class Graphic3d_AspectFillArea3d;
DEFINE_STANDARD_HANDLE(Graphic3d_AspectFillArea3d, Aspect_AspectFillArea)

//! This class permits the creation and updating of
//! a graphic attribute context for opaque 3d
//! primitives (polygons, triangles, quadrilaterals)
//! Keywords: Face, FillArea, Triangle, Quadrangle, Polygon,
//! TriangleMesh, QuadrangleMesh, Edge, Border, Interior,
//! Color, Type, Width, Style, Hatch, Material,
//! BackFaceRemoval, DistinguishMode
class Graphic3d_AspectFillArea3d : public Aspect_AspectFillArea
{

public:

  
  //! Creates a context table for fill area primitives
  //! defined with the following default values:
  //!
  //! InteriorStyle       : IS_EMPTY
  //! InteriorColor       : NOC_CYAN1
  //! EdgeColor           : NOC_WHITE
  //! EdgeLineType        : TOL_SOLID
  //! EdgeWidth           : 1.0
  //! FrontMaterial       : NOM_BRASS
  //! BackMaterial        : NOM_BRASS
  //!
  //! Display of back-facing filled polygons.
  //! No distinction between external and internal
  //! faces of FillAreas.
  //! The edges are not drawn.
  //! Polygon offset parameters: mode = Aspect_POM_None, factor = 1., units = 0.
  Standard_EXPORT Graphic3d_AspectFillArea3d();
  
  //! Creates a context table for fill area primitives
  //! defined with the specified values.
  //!
  //! Display of back-facing filled polygons.
  //! No distinction between external and internal
  //! faces of FillAreas.
  //! The edges are not drawn.
  //! Polygon offset parameters: mode = Aspect_POM_None, factor = 1., units = 0.
  //! Warning
  //! EdgeWidth is the "line width scale factor".   The
  //! nominal line width is 1 pixel.   The width of the line is
  //! determined by applying the line width scale factor to
  //! this nominal line width.   The supported line widths
  //! vary by 1-pixel units.
  Standard_EXPORT Graphic3d_AspectFillArea3d(const Aspect_InteriorStyle Interior, const Quantity_Color& InteriorColor, const Quantity_Color& EdgeColor, const Aspect_TypeOfLine EdgeLineType, const Standard_Real EdgeWidth, const Graphic3d_MaterialAspect& FrontMaterial, const Graphic3d_MaterialAspect& BackMaterial);
  
  //! Allows the display of back-facing filled
  //! polygons.
  Standard_EXPORT void AllowBackFace();
  
  //! Modifies the surface material of internal faces
  Standard_EXPORT void SetBackMaterial (const Graphic3d_MaterialAspect& AMaterial);
  
  //! Allows distinction between external and internal
  //! faces of FillAreas.
  Standard_EXPORT void SetDistinguishOn();
  
  //! Forbids distinction between external and internal
  //! faces of FillAreas.
  Standard_EXPORT void SetDistinguishOff();
  
  //! The edges of FillAreas are drawn.
  Standard_EXPORT void SetEdgeOn();
  
  //! The edges of FillAreas are not drawn.
  Standard_EXPORT void SetEdgeOff();
  
  //! Modifies the surface material of external faces
  Standard_EXPORT void SetFrontMaterial (const Graphic3d_MaterialAspect& AMaterial);
  
  //! Suppress the display of back-facing filled
  //! polygons.
  //! A back-facing polygon is defined as a polygon whose
  //! vertices are in a clockwise order with respect
  //! to screen coordinates.
  Standard_EXPORT void SuppressBackFace();
  
  Standard_EXPORT void SetTextureMap (const Handle(Graphic3d_TextureMap)& ATexture);
  
  Standard_EXPORT void SetTextureMapOn();
  
  Standard_EXPORT void SetTextureMapOff();
  
  //! Sets up OpenGL polygon offsets mechanism.
  //! <aMode> parameter can contain various combinations of
  //! Aspect_PolygonOffsetMode enumeration elements (Aspect_POM_None means
  //! that polygon offsets are not changed).
  //! If <aMode> is different from Aspect_POM_Off and Aspect_POM_None, then <aFactor> and <aUnits>
  //! arguments are used by graphic renderer to calculate a depth offset value:
  //!
  //! offset = <aFactor> * m + <aUnits> * r, where
  //! m - maximum depth slope for the polygon currently being displayed,
  //! r - minimum window coordinates depth resolution (implementation-specific)
  //!
  //! Deafult settings for OCC 3D viewer: mode = Aspect_POM_Fill, factor = 1., units = 0.
  //!
  //! Negative offset values move polygons closer to the viewport,
  //! while positive values shift polygons away.
  //! Consult OpenGL reference for details (glPolygonOffset function description).
  Standard_EXPORT void SetPolygonOffsets (const Standard_Integer aMode, const Standard_ShortReal aFactor = 1.0, const Standard_ShortReal aUnits = 0.0);
  
  //! Sets up OpenGL/GLSL shader program.
  Standard_EXPORT void SetShaderProgram (const Handle(Graphic3d_ShaderProgram)& theProgram);
  
  //! Returns the Back Face Removal status.
  //! Standard_True if SuppressBackFace is activated.
  Standard_EXPORT Standard_Boolean BackFace() const;
  
  //! Returns the Distinguish Mode status.
  Standard_EXPORT Standard_Boolean Distinguish() const;
  
  //! Returns Standard_True if the edges are drawn and
  //! Standard_False if the edges are not drawn.
  Standard_EXPORT Standard_Boolean Edge() const;
  
  //! Returns the surface material of internal faces
  Standard_EXPORT const Graphic3d_MaterialAspect& BackMaterial() const;
  
  //! Returns the surface material of external faces
  Standard_EXPORT const Graphic3d_MaterialAspect& FrontMaterial() const;
  
  Standard_EXPORT Handle(Graphic3d_TextureMap) TextureMap() const;
  
  Standard_EXPORT Standard_Boolean TextureMapState() const;
  
  //! Returns current polygon offsets settings.
  Standard_EXPORT void PolygonOffsets (Standard_Integer& aMode, Standard_ShortReal& aFactor, Standard_ShortReal& aUnits) const;
  
  Standard_EXPORT const Handle(Graphic3d_ShaderProgram)& ShaderProgram() const;




  DEFINE_STANDARD_RTTI(Graphic3d_AspectFillArea3d,Aspect_AspectFillArea)

protected:




private:


  Standard_Boolean DistinguishModeActive;
  Standard_Boolean EdgeModeActive;
  Standard_Boolean BackFaceRemovalActive;
  Handle(Graphic3d_TextureMap) MyTextureMap;
  Standard_Boolean MyTextureMapState;
  Graphic3d_MaterialAspect MyFrontMaterial;
  Graphic3d_MaterialAspect MyBackMaterial;
  Standard_Integer MyPolygonOffsetMode;
  Standard_ShortReal MyPolygonOffsetFactor;
  Standard_ShortReal MyPolygonOffsetUnits;
  Handle(Graphic3d_ShaderProgram) MyShaderProgram;


};







#endif // _Graphic3d_AspectFillArea3d_HeaderFile
