// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _Graphic3d_Aspects_HeaderFile
#define _Graphic3d_Aspects_HeaderFile

#include <Aspect_InteriorStyle.hxx>
#include <Aspect_TypeOfDisplayText.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Aspect_TypeOfStyleText.hxx>
#include <Font_FontAspect.hxx>
#include <Graphic3d_AlphaMode.hxx>
#include <Graphic3d_MarkerImage.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <Graphic3d_HatchStyle.hxx>
#include <Graphic3d_PolygonOffset.hxx>
#include <Graphic3d_ShaderProgram.hxx>
#include <Graphic3d_TextureSet.hxx>
#include <Graphic3d_TypeOfBackfacingModel.hxx>
#include <Graphic3d_TypeOfShadingModel.hxx>
#include <TCollection_HAsciiString.hxx>

//! This class defines graphic attributes.
class Graphic3d_Aspects : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_Aspects, Standard_Transient)
public:

  //! Creates a context table for drawing primitives defined with the following default values:
  Standard_EXPORT Graphic3d_Aspects();

  //! Return interior rendering style; Aspect_IS_SOLID by default.
  Aspect_InteriorStyle InteriorStyle() const { return myInteriorStyle; }

  //! Modifies the interior type used for rendering
  void SetInteriorStyle (const Aspect_InteriorStyle theStyle) { myInteriorStyle = theStyle; }

  //! Returns shading model; Graphic3d_TypeOfShadingModel_DEFAULT by default.
  //! Graphic3d_TOSM_DEFAULT means that Shading Model set as default for entire Viewer will be used.
  Graphic3d_TypeOfShadingModel ShadingModel() const { return myShadingModel; }

  //! Sets shading model
  void SetShadingModel (const Graphic3d_TypeOfShadingModel theShadingModel) { myShadingModel = theShadingModel; }

  //! Returns the way how alpha value should be treated (Graphic3d_AlphaMode_BlendAuto by default, for backward compatibility).
  Graphic3d_AlphaMode AlphaMode() const { return myAlphaMode; }

  //! Returns alpha cutoff threshold, for discarding fragments within Graphic3d_AlphaMode_Mask mode (0.5 by default).
  //! If the alpha value is greater than or equal to this value then it is rendered as fully opaque, otherwise, it is rendered as fully transparent.
  Standard_ShortReal AlphaCutoff() const { return myAlphaCutoff; }

  //! Defines the way how alpha value should be treated.
  void SetAlphaMode (Graphic3d_AlphaMode theMode, Standard_ShortReal theAlphaCutoff = 0.5f)
  {
    myAlphaMode = theMode;
    myAlphaCutoff = theAlphaCutoff;
  }

  //! Return color
  const Quantity_ColorRGBA& ColorRGBA() const { return myInteriorColor; }

  //! Return the color.
  const Quantity_Color& Color() const { return myInteriorColor.GetRGB(); }

  //! Modifies the color.
  void SetColor (const Quantity_Color& theColor) { myInteriorColor.SetRGB(theColor); }

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

  //! Return face culling mode; Graphic3d_FaceCulling_BackClosed by default.
  //! A back-facing polygon is defined as a polygon whose
  //! vertices are in a clockwise order with respect to screen coordinates.
  Graphic3d_TypeOfBackfacingModel FaceCulling() const { return myFaceCulling; }

  //! Set face culling mode.
  void SetFaceCulling (Graphic3d_TypeOfBackfacingModel theCulling) { myFaceCulling = theCulling; }

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

  //! Sets polygon offsets settings.
  void SetPolygonOffset (const Graphic3d_PolygonOffset& theOffset) { myPolygonOffset = theOffset; }

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

//! @name parameters specific to Line primitive rendering
public:

  //! Return line type; Aspect_TOL_SOLID by default.
  Aspect_TypeOfLine LineType() const { return myLineType; }

  //! Modifies the line type
  void SetLineType (Aspect_TypeOfLine theType)
  {
    myLineType = theType;
    myLinePattern = DefaultLinePatternForType (theType);
  }

  //! Return custom stipple line pattern; 0xFFFF by default.
  uint16_t LinePattern() const { return myLinePattern; }

  //! Modifies the stipple line pattern, and changes line type to Aspect_TOL_USERDEFINED for non-standard pattern.
  void SetLinePattern (uint16_t thePattern)
  {
    myLineType = DefaultLineTypeForPattern (thePattern);
    myLinePattern = thePattern;
  }

  //! Return a multiplier for each bit in the line stipple pattern within [1, 256] range; 1 by default.
  uint16_t LineStippleFactor() const { return myLineFactor; }

  //! Set a multiplier for each bit in the line stipple pattern.
  void SetLineStippleFactor (uint16_t theFactor)
  {
    if (theFactor == 0 || theFactor > 256)
    {
      throw Standard_OutOfRange ("Graphic3d_Aspects::SetLineStippleFactor(), bad factor value");
    }
    myLineFactor = theFactor;
  }

  //! Return width for edges in pixels; 1.0 by default.
  Standard_ShortReal LineWidth() const { return myLineWidth; }

  //! Modifies the line thickness
  //! Warning: Raises Standard_OutOfRange if the width is a negative value.
  void SetLineWidth (Standard_ShortReal theWidth)
  {
    if (theWidth <= 0.0f)
    {
      throw Standard_OutOfRange ("Bad value for EdgeLineWidth");
    }
    myLineWidth = theWidth;
  }

  //! Return stipple line pattern for line type.
  static uint16_t DefaultLinePatternForType (Aspect_TypeOfLine theType)
  {
    switch (theType)
    {
      case Aspect_TOL_DASH:        return 0xFFC0;
      case Aspect_TOL_DOT:         return 0xCCCC;
      case Aspect_TOL_DOTDASH:     return 0xFF18;
      case Aspect_TOL_EMPTY:       return 0x0000;
      case Aspect_TOL_SOLID:       return 0xFFFF;
      case Aspect_TOL_USERDEFINED: return 0xFF24;
    }
    return 0xFFFF;
  }

  //! Return line type for stipple line pattern.
  static Aspect_TypeOfLine DefaultLineTypeForPattern (uint16_t thePattern)
  {
    switch (thePattern)
    {
      case 0x0000: return Aspect_TOL_EMPTY;
      case 0xFFC0: return Aspect_TOL_DASH;
      case 0xCCCC: return Aspect_TOL_DOT;
      case 0xFF18: return Aspect_TOL_DOTDASH;
      case 0xFFFF: return Aspect_TOL_SOLID;
      case 0xFF24: return Aspect_TOL_USERDEFINED;
    }
    return Aspect_TOL_USERDEFINED;
  }

//! @name parameters specific to Point (Marker) primitive rendering
public:

  //! Return marker type; Aspect_TOM_POINT by default.
  Aspect_TypeOfMarker MarkerType() const { return myMarkerType; }

  //! Modifies the type of marker.
  void SetMarkerType (Aspect_TypeOfMarker theType) { myMarkerType = theType; }

  //! Return marker scale factor; 1.0 by default.
  Standard_ShortReal MarkerScale() const { return myMarkerScale; }

  //! Modifies the scale factor.
  //! Marker type Aspect_TOM_POINT is not affected by the marker size scale factor.
  //! It is always the smallest displayable dot.
  //! Warning: Raises Standard_OutOfRange if the scale is a negative value.
  void SetMarkerScale (const Standard_ShortReal theScale)
  {
    if (theScale <= 0.0f)
    {
      throw Standard_OutOfRange ("Bad value for MarkerScale");
    }
    myMarkerScale = theScale;
  }

  //! Returns marker's image texture.
  //! Could be null handle if marker aspect has been initialized as default type of marker.
  const Handle(Graphic3d_MarkerImage)& MarkerImage() const { return myMarkerImage; }

  //! Set marker's image texture.
  void SetMarkerImage (const Handle(Graphic3d_MarkerImage)& theImage) { myMarkerImage = theImage; }

  //! Returns TRUE if marker should be drawn using marker sprite (either user-provided or generated).
  bool IsMarkerSprite() const
  {
    if (myMarkerType == Aspect_TOM_POINT
     || myMarkerType == Aspect_TOM_EMPTY)
    {
      return false;
    }

    return myMarkerType != Aspect_TOM_USERDEFINED
       || !myMarkerImage.IsNull();
  }

//! @name parameters specific to text rendering
public:

  //! Returns the font; NULL string by default.
  const Handle(TCollection_HAsciiString)& TextFont() const { return myTextFont; }

  //! Modifies the font.
  void SetTextFont (const Handle(TCollection_HAsciiString)& theFont) { myTextFont = theFont; }

  //! Returns text FontAspect
  Font_FontAspect TextFontAspect() const { return myTextFontAspect; }

  //! Turns usage of Aspect text
  void SetTextFontAspect (Font_FontAspect theFontAspect) { myTextFontAspect = theFontAspect; }

  //! Returns display type; Aspect_TODT_NORMAL by default.
  Aspect_TypeOfDisplayText TextDisplayType() const { return myTextDisplayType; }

  //! Sets display type.
  void SetTextDisplayType (Aspect_TypeOfDisplayText theType) { myTextDisplayType = theType; }

  //! Returns text background/shadow color; equals to EdgeColor() property.
  const Quantity_ColorRGBA& ColorSubTitleRGBA() const { return myEdgeColor; }

  //! Return text background/shadow color; equals to EdgeColor() property.
  const Quantity_Color& ColorSubTitle() const { return myEdgeColor.GetRGB(); }

  //! Modifies text background/shadow color; equals to EdgeColor() property.
  void SetColorSubTitle (const Quantity_Color& theColor) { myEdgeColor.SetRGB (theColor); }

  //! Modifies text background/shadow color; equals to EdgeColor() property.
  void SetColorSubTitle (const Quantity_ColorRGBA& theColor) { myEdgeColor = theColor; }

  //! Returns TRUE when the Text Zoomable is on.
  bool IsTextZoomable() const { return myIsTextZoomable; }

  //! Turns usage of text zoomable on/off
  void SetTextZoomable (bool theFlag) { myIsTextZoomable = theFlag; }

  //! Returns the text style; Aspect_TOST_NORMAL by default.
  Aspect_TypeOfStyleText TextStyle() const { return myTextStyle; }

  //! Modifies the style of the text.
  void SetTextStyle (Aspect_TypeOfStyleText theStyle) { myTextStyle = theStyle; }

  //! Returns Angle of degree
  Standard_ShortReal TextAngle() const { return myTextAngle; }

  //! Turns usage of text rotated
  void SetTextAngle (Standard_ShortReal theAngle) { myTextAngle = (Standard_ShortReal )theAngle; }

//! @name parameters specific to Mesh Edges (of triangulation primitive) rendering
public:

  //! Returns true if mesh edges should be drawn (false by default).
  bool ToDrawEdges() const { return myToDrawEdges && myLineType != Aspect_TOL_EMPTY; }

  //! Set if mesh edges should be drawn or not.
  void SetDrawEdges (bool theToDraw)
  {
    myToDrawEdges = theToDraw;
    if (myLineType == Aspect_TOL_EMPTY)
    {
      myLineType = Aspect_TOL_SOLID;
    }
  }

  //! The edges of FillAreas are drawn.
  void SetEdgeOn() { SetDrawEdges (true); }

  //! The edges of FillAreas are not drawn.
  void SetEdgeOff() { SetDrawEdges (false); }

  //! Return color of edges.
  const Quantity_Color& EdgeColor() const { return myEdgeColor.GetRGB(); }

  //! Return color of edges.
  const Quantity_ColorRGBA& EdgeColorRGBA() const { return myEdgeColor; }

  //! Modifies the color of the edge of the face
  void SetEdgeColor (const Quantity_Color& theColor) { myEdgeColor.SetRGB (theColor); }

  //! Modifies the color of the edge of the face
  void SetEdgeColor (const Quantity_ColorRGBA& theColor) { myEdgeColor = theColor; }

  //! Return edges line type (same as LineType()).
  Aspect_TypeOfLine EdgeLineType() const { return myLineType; }

  //! Modifies the edge line type (same as SetLineType())
  void SetEdgeLineType (Aspect_TypeOfLine theType) { SetLineType (theType); }

  //! Return width for edges in pixels (same as LineWidth()).
  Standard_ShortReal EdgeWidth() const { return myLineWidth; }

  //! Modifies the edge thickness (same as SetLineWidth())
  void SetEdgeWidth (Standard_Real theWidth) { SetLineWidth ((Standard_ShortReal )theWidth); }

  //! Returns TRUE if drawing element edges should discard first edge in triangle; FALSE by default.
  //! Graphics hardware works mostly with triangles, so that wireframe presentation will draw triangle edges by default.
  //! This flag allows rendering wireframe presentation of quad-only array split into triangles.
  //! For this, quads should be split in specific order, so that the quad diagonal (to be NOT rendered) goes first:
  //!     1------2
  //!    /      /   Triangle #1: 2-0-1; Triangle #2: 0-2-3
  //!   0------3
  bool ToSkipFirstEdge() const { return myToSkipFirstEdge; }

  //! Set skip first triangle edge flag for drawing wireframe presentation of quads array split into triangles.
  void SetSkipFirstEdge (bool theToSkipFirstEdge) { myToSkipFirstEdge = theToSkipFirstEdge; }

  //! Returns TRUE if silhouette (outline) should be drawn (with edge color and width); FALSE by default.
  bool ToDrawSilhouette() const { return myToDrawSilhouette; }

  //! Enables/disables drawing silhouette (outline).
  void SetDrawSilhouette (bool theToDraw) { myToDrawSilhouette = theToDraw; }

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

public:

  //! Check for equality with another aspects.
  bool IsEqual (const Graphic3d_Aspects& theOther)
  {
    if (this == &theOther)
    {
      return true;
    }

    return myProgram == theOther.myProgram
        && myTextureSet == theOther.myTextureSet
        && myMarkerImage == theOther.myMarkerImage
        && myInteriorColor == theOther.myInteriorColor
        && myBackInteriorColor == theOther.myBackInteriorColor
        && myFrontMaterial == theOther.myFrontMaterial
        && myBackMaterial  == theOther.myBackMaterial
        && myInteriorStyle == theOther.myInteriorStyle
        && myShadingModel == theOther.myShadingModel
        && myFaceCulling == theOther.myFaceCulling
        && myAlphaMode == theOther.myAlphaMode
        && myAlphaCutoff == theOther.myAlphaCutoff
        && myLineType  == theOther.myLineType
        && myEdgeColor == theOther.myEdgeColor
        && myLineWidth == theOther.myLineWidth
        && myLineFactor == theOther.myLineFactor
        && myLinePattern == theOther.myLinePattern
        && myMarkerType == theOther.myMarkerType
        && myMarkerScale == theOther.myMarkerScale
        && myHatchStyle == theOther.myHatchStyle
        && myTextFont == theOther.myTextFont
        && myPolygonOffset == theOther.myPolygonOffset
        && myTextStyle == theOther.myTextStyle
        && myTextDisplayType == theOther.myTextDisplayType
        && myTextFontAspect == theOther.myTextFontAspect
        && myTextAngle == theOther.myTextAngle
        && myToSkipFirstEdge == theOther.myToSkipFirstEdge
        && myToDistinguishMaterials == theOther.myToDistinguishMaterials
        && myToDrawEdges == theOther.myToDrawEdges
        && myToDrawSilhouette == theOther.myToDrawSilhouette
        && myToMapTexture == theOther.myToMapTexture
        && myIsTextZoomable == theOther.myIsTextZoomable;
  }

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

public:

  Standard_DEPRECATED("Deprecated method, FaceCulling() should be used instead")
  bool ToSuppressBackFaces() const
  {
    return myFaceCulling == Graphic3d_TypeOfBackfacingModel_BackCulled
        || myFaceCulling == Graphic3d_TypeOfBackfacingModel_Auto;
  }

  Standard_DEPRECATED("Deprecated method, SetFaceCulling() should be used instead")
  void SetSuppressBackFaces (bool theToSuppress) { myFaceCulling = theToSuppress ? Graphic3d_TypeOfBackfacingModel_Auto : Graphic3d_TypeOfBackfacingModel_DoubleSided; }

  Standard_DEPRECATED("Deprecated method, FaceCulling() should be used instead")
  bool BackFace() const
  {
    return myFaceCulling == Graphic3d_TypeOfBackfacingModel_BackCulled
        || myFaceCulling == Graphic3d_TypeOfBackfacingModel_Auto;
  }

  Standard_DEPRECATED("Deprecated method, SetFaceCulling() should be used instead")
  void AllowBackFace() { myFaceCulling = Graphic3d_TypeOfBackfacingModel_DoubleSided; }

  Standard_DEPRECATED("Deprecated method, SetFaceCulling() should be used instead")
  void SuppressBackFace() { myFaceCulling = Graphic3d_TypeOfBackfacingModel_Auto; }

protected:

  Handle(Graphic3d_ShaderProgram)  myProgram;
  Handle(Graphic3d_TextureSet)     myTextureSet;
  Handle(Graphic3d_MarkerImage)    myMarkerImage;
  Handle(Graphic3d_HatchStyle)     myHatchStyle;
  Handle(TCollection_HAsciiString) myTextFont;
  Graphic3d_MaterialAspect         myFrontMaterial;
  Graphic3d_MaterialAspect         myBackMaterial;

  Quantity_ColorRGBA           myInteriorColor;
  Quantity_ColorRGBA           myBackInteriorColor;
  Quantity_ColorRGBA           myEdgeColor;

  Graphic3d_PolygonOffset      myPolygonOffset;
  Aspect_InteriorStyle         myInteriorStyle;
  Graphic3d_TypeOfShadingModel myShadingModel;
  Graphic3d_TypeOfBackfacingModel myFaceCulling;
  Graphic3d_AlphaMode          myAlphaMode;
  Standard_ShortReal           myAlphaCutoff;

  Aspect_TypeOfLine            myLineType;
  Standard_ShortReal           myLineWidth;
  uint16_t                     myLineFactor;
  uint16_t                     myLinePattern;

  Aspect_TypeOfMarker          myMarkerType;
  Standard_ShortReal           myMarkerScale;

  Aspect_TypeOfStyleText   myTextStyle;
  Aspect_TypeOfDisplayText myTextDisplayType;
  Font_FontAspect          myTextFontAspect;
  Standard_ShortReal       myTextAngle;

  bool myToSkipFirstEdge;
  bool myToDistinguishMaterials;
  bool myToDrawEdges;
  bool myToDrawSilhouette;
  bool myToMapTexture;
  bool myIsTextZoomable;

};

DEFINE_STANDARD_HANDLE(Graphic3d_Aspects, Standard_Transient)

#endif // _Graphic3d_Aspects_HeaderFile
