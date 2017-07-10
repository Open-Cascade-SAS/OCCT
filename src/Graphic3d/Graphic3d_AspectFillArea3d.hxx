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

#include <Aspect_AspectFillAreaDefinitionError.hxx>
#include <Aspect_PolygonOffsetMode.hxx>
#include <Aspect_InteriorStyle.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Graphic3d_HatchStyle.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <Graphic3d_PolygonOffset.hxx>
#include <Graphic3d_ShaderProgram.hxx>
#include <Graphic3d_TextureMap.hxx>
#include <Graphic3d_TextureSet.hxx>
#include <Standard.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_ShortReal.hxx>
#include <Standard_Real.hxx>
#include <Standard_Type.hxx>
#include <Quantity_ColorRGBA.hxx>

//! This class defines graphic attributes for opaque 3d primitives (polygons, triangles, quadrilaterals).
class Graphic3d_AspectFillArea3d : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_AspectFillArea3d, Standard_Transient)
public:

  //! Creates a context table for fill area primitives defined with the following default values:
  //!
  //! InteriorStyle : Aspect_IS_EMPTY
  //! InteriorColor : Quantity_NOC_CYAN1
  //! EdgeColor     : Quantity_NOC_WHITE
  //! EdgeLineType  : Aspect_TOL_SOLID
  //! EdgeWidth     : 1.0
  //! FrontMaterial : NOM_BRASS
  //! BackMaterial  : NOM_BRASS
  //! HatchStyle    : Aspect_HS_SOLID
  //!
  //! Display of back-facing filled polygons.
  //! No distinction between external and internal faces of FillAreas.
  //! The edges are not drawn.
  //! Polygon offset parameters: mode = Aspect_POM_None, factor = 1., units = 0.
  Standard_EXPORT Graphic3d_AspectFillArea3d();
  
  //! Creates a context table for fill area primitives defined with the specified values.
  //! Display of back-facing filled polygons.
  //! No distinction between external and internal faces of FillAreas.
  //! The edges are not drawn.
  //! Polygon offset parameters: mode = Aspect_POM_None, factor = 1., units = 0.
  Standard_EXPORT Graphic3d_AspectFillArea3d (const Aspect_InteriorStyle theInterior,
                                              const Quantity_Color&      theInteriorColor,
                                              const Quantity_Color&      theEdgeColor,
                                              const Aspect_TypeOfLine    theEdgeLineType,
                                              const Standard_Real        theEdgeWidth,
                                              const Graphic3d_MaterialAspect& theFrontMaterial,
                                              const Graphic3d_MaterialAspect& theBackMaterial);

  //! Return interior rendering style (Aspect_IS_EMPTY by default, which means nothing will be rendered!).
  Aspect_InteriorStyle InteriorStyle() const { return myInteriorStyle; }

  //! Modifies the interior type used for rendering
  void SetInteriorStyle (const Aspect_InteriorStyle theStyle) { myInteriorStyle = theStyle; }

  //! Return interior color.
  const Quantity_Color& InteriorColor() const { return myInteriorColor.GetRGB(); }

  //! Return interior color.
  const Quantity_ColorRGBA& InteriorColorRGBA() const { return myInteriorColor; }

  //! Modifies the color of the interior of the face
  void SetInteriorColor (const Quantity_Color& theColor) { myInteriorColor.SetRGB (theColor); }

  //! Modifies the color of the interior of the face
  void SetInteriorColor (const Quantity_ColorRGBA& theColor) { myInteriorColor = theColor; }

  //! Return back interior color.
  const Quantity_Color& BackInteriorColor() const { return myBackInteriorColor.GetRGB(); }

  //! Return back interior color.
  const Quantity_ColorRGBA& BackInteriorColorRGBA() const { return myBackInteriorColor; }

  //! Modifies the color of the interior of the back face
  void SetBackInteriorColor (const Quantity_Color& theColor) { myBackInteriorColor.SetRGB (theColor); }

  //! Modifies the color of the interior of the back face
  void SetBackInteriorColor (const Quantity_ColorRGBA& theColor) { myBackInteriorColor = theColor; }

  //! Returns the surface material of external faces
  const Graphic3d_MaterialAspect& FrontMaterial() const { return myFrontMaterial; }

  //! Returns the surface material of external faces
  Graphic3d_MaterialAspect& ChangeFrontMaterial() { return myFrontMaterial; }

  //! Modifies the surface material of external faces
  void SetFrontMaterial (const Graphic3d_MaterialAspect& theMaterial) { myFrontMaterial = theMaterial; }

  //! Returns the surface material of internal faces
  const Graphic3d_MaterialAspect& BackMaterial() const { return myBackMaterial; }

  //! Returns the surface material of internal faces
  Graphic3d_MaterialAspect& ChangeBackMaterial() { return myBackMaterial; }

  //! Modifies the surface material of internal faces
  void SetBackMaterial (const Graphic3d_MaterialAspect& theMaterial) { myBackMaterial = theMaterial; }

  //! Returns true if back faces should be suppressed (true by default).
  bool ToSuppressBackFaces() const { return myToSuppressBackFaces; }

  //! Assign back faces culling flag.
  void SetSuppressBackFaces (bool theToSuppress) { myToSuppressBackFaces = theToSuppress; }

  //! Returns true if back faces should be suppressed (true by default).
  bool BackFace() const { return myToSuppressBackFaces; }

  //! Allows the display of back-facing filled polygons.
  void AllowBackFace() { myToSuppressBackFaces = false; }

  //! Suppress the display of back-facing filled polygons.
  //! A back-facing polygon is defined as a polygon whose
  //! vertices are in a clockwise order with respect to screen coordinates.
  void SuppressBackFace() { myToSuppressBackFaces = true; }

  //! Returns true if material properties should be distinguished for back and front faces (false by default).
  bool Distinguish() const { return myToDistinguishMaterials; }

  //! Set material distinction between front and back faces.
  void SetDistinguish (bool toDistinguish) { myToDistinguishMaterials = toDistinguish; }

  //! Allows material distinction between front and back faces.
  void SetDistinguishOn() { myToDistinguishMaterials = true; }

  //! Forbids material distinction between front and back faces.
  void SetDistinguishOff() { myToDistinguishMaterials = false; }

  //! Return shader program.
  const Handle(Graphic3d_ShaderProgram)& ShaderProgram() const { return myProgram; }

  //! Sets up OpenGL/GLSL shader program.
  void SetShaderProgram (const Handle(Graphic3d_ShaderProgram)& theProgram) { myProgram = theProgram; }

  //! Return texture array to be mapped.
  const Handle(Graphic3d_TextureSet)& TextureSet() const { return myTextureSet; }

  //! Setup texture array to be mapped.
  void SetTextureSet (const Handle(Graphic3d_TextureSet)& theTextures) { myTextureSet = theTextures; }

  //! Return texture to be mapped.
  //Standard_DEPRECATED("Deprecated method, TextureSet() should be used instead")
  Handle(Graphic3d_TextureMap) TextureMap() const
  {
    return !myTextureSet.IsNull() && !myTextureSet->IsEmpty()
          ? myTextureSet->First()
          : Handle(Graphic3d_TextureMap)();
  }

  //! Assign texture to be mapped.
  //! See also SetTextureMapOn() to actually activate texture mapping.
  //Standard_DEPRECATED("Deprecated method, SetTextureSet() should be used instead")
  Standard_EXPORT void SetTextureMap (const Handle(Graphic3d_TextureMap)& theTexture);

  //! Return true if texture mapping is enabled (false by default).
  bool ToMapTexture() const { return myToMapTexture; }

  //! Return true if texture mapping is enabled (false by default).
  bool TextureMapState() const { return myToMapTexture; }

  //! Enable or disable texture mapping (has no effect if texture is not set).
  void SetTextureMapOn (bool theToMap) { myToMapTexture = theToMap; }

  //! Enable texture mapping (has no effect if texture is not set).
  void SetTextureMapOn() { myToMapTexture = true; }

  //! Disable texture mapping.
  void SetTextureMapOff() { myToMapTexture = false; }

  //! Returns current polygon offsets settings.
  const Graphic3d_PolygonOffset& PolygonOffset() const { return myPolygonOffset; }

  //! Returns current polygon offsets settings.
  void PolygonOffsets (Standard_Integer&   theMode,
                       Standard_ShortReal& theFactor,
                       Standard_ShortReal& theUnits) const
  {
    theMode   = myPolygonOffset.Mode;
    theFactor = myPolygonOffset.Factor;
    theUnits  = myPolygonOffset.Units;
  }

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
  //! Default settings for OCC 3D viewer: mode = Aspect_POM_Fill, factor = 1., units = 0.
  //!
  //! Negative offset values move polygons closer to the viewport,
  //! while positive values shift polygons away.
  //! Consult OpenGL reference for details (glPolygonOffset function description).
  void SetPolygonOffsets (const Standard_Integer   theMode,
                          const Standard_ShortReal theFactor = 1.0f,
                          const Standard_ShortReal theUnits  = 0.0f)
  {
    myPolygonOffset.Mode   = (Aspect_PolygonOffsetMode )(theMode & Aspect_POM_Mask);
    myPolygonOffset.Factor = theFactor;
    myPolygonOffset.Units  = theUnits;
  }

public:

  //! Returns true if edges should be drawn (false by default).
  bool ToDrawEdges() const { return myToDrawEdges; }

  //! Set if edges should be drawn or not.
  void SetDrawEdges (bool theToDraw) { myToDrawEdges = theToDraw; }

  //! Returns true if edges should be drawn.
  bool Edge() const { return myToDrawEdges; }

  //! The edges of FillAreas are drawn.
  void SetEdgeOn() { myToDrawEdges = true; }

  //! The edges of FillAreas are not drawn.
  void SetEdgeOff() { myToDrawEdges = false; }

  //! Return color of edges.
  const Quantity_Color& EdgeColor() const { return myEdgeColor.GetRGB(); }

  //! Return color of edges.
  const Quantity_ColorRGBA& EdgeColorRGBA() const { return myEdgeColor; }

  //! Modifies the color of the edge of the face
  void SetEdgeColor (const Quantity_Color& theColor) { myEdgeColor.SetRGB (theColor); }

  //! Return edges line type.
  Aspect_TypeOfLine EdgeLineType() const { return myEdgeType; }

  //! Modifies the edge line type
  void SetEdgeLineType (const Aspect_TypeOfLine theType) { myEdgeType = theType; }

  //! Return width for edges in pixels.
  Standard_ShortReal EdgeWidth() const { return myEdgeWidth; }

  //! Modifies the edge thickness
  //! Warning: Raises AspectFillAreaDefinitionError if the width is a negative value.
  void SetEdgeWidth (const Standard_Real theWidth)
  {
    if (theWidth <= 0.0)
    {
      throw Aspect_AspectFillAreaDefinitionError("Bad value for EdgeLineWidth");
    }
    myEdgeWidth = (float )theWidth;
  }

public:

  //! Returns the hatch type used when InteriorStyle is IS_HATCH
  const Handle(Graphic3d_HatchStyle)& HatchStyle() const { return myHatchStyle; }

  //! Modifies the hatch type used when InteriorStyle is IS_HATCH
  void SetHatchStyle (const Handle(Graphic3d_HatchStyle)& theStyle) { myHatchStyle = theStyle; }

  //! Modifies the hatch type used when InteriorStyle is IS_HATCH
  //! @warning This method always creates a new handle for a given hatch style
  void SetHatchStyle (const Aspect_HatchStyle theStyle)
  {
    if (theStyle == Aspect_HS_SOLID)
    {
      myHatchStyle.Nullify();
      return;
    }

    myHatchStyle = new Graphic3d_HatchStyle (theStyle);
  }

  //! Returns the current values.
  Standard_DEPRECATED("Deprecated method Values() should be replaced by individual property getters")
  void Values (Aspect_InteriorStyle& theStyle,
               Quantity_Color&       theIntColor,
               Quantity_Color&       theEdgeColor,
               Aspect_TypeOfLine&    theType,
               Standard_Real&        theWidth) const
  {
    theStyle    = myInteriorStyle;
    theIntColor = myInteriorColor.GetRGB();
    theEdgeColor= myEdgeColor.GetRGB();
    theType     = myEdgeType;
    theWidth    = myEdgeWidth;
  }

  //! Returns the current values.
  Standard_DEPRECATED("Deprecated method Values() should be replaced by individual property getters")
  void Values (Aspect_InteriorStyle& theStyle,
               Quantity_Color&       theIntColor,
               Quantity_Color&       theBackIntColor,
               Quantity_Color&       theEdgeColor,
               Aspect_TypeOfLine&    theType,
               Standard_Real&        theWidth) const
  {
    theStyle       = myInteriorStyle;
    theIntColor    = myInteriorColor.GetRGB();
    theBackIntColor= myBackInteriorColor.GetRGB();
    theEdgeColor   = myEdgeColor.GetRGB();
    theType        = myEdgeType;
    theWidth       = myEdgeWidth;
  }

protected:

  Handle(Graphic3d_ShaderProgram) myProgram;
  Handle(Graphic3d_TextureSet)    myTextureSet;
  Graphic3d_MaterialAspect        myFrontMaterial;
  Graphic3d_MaterialAspect        myBackMaterial;

  Quantity_ColorRGBA           myInteriorColor;
  Quantity_ColorRGBA           myBackInteriorColor;
  Quantity_ColorRGBA           myEdgeColor;
  Aspect_InteriorStyle         myInteriorStyle;
  Aspect_TypeOfLine            myEdgeType;
  Standard_ShortReal           myEdgeWidth;
  Handle(Graphic3d_HatchStyle) myHatchStyle;

  Graphic3d_PolygonOffset myPolygonOffset;
  bool                    myToDistinguishMaterials;
  bool                    myToDrawEdges;
  bool                    myToSuppressBackFaces;
  bool                    myToMapTexture;

};

DEFINE_STANDARD_HANDLE(Graphic3d_AspectFillArea3d, Standard_Transient)

#endif // _Graphic3d_AspectFillArea3d_HeaderFile
