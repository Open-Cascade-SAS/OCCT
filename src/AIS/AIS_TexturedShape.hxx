// Created on: 2001-07-02
// Created by: Mathias BOSSHARD
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef _AIS_TexturedShape_HeaderFile
#define _AIS_TexturedShape_HeaderFile

#include <AIS_Shape.hxx>
#include <gp_Pnt2d.hxx>
#include <Graphic3d_NameOfTexture2D.hxx>
#include <Image_PixMap.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_Texture2Dmanual.hxx>
#include <Prs3d_Presentation.hxx>
#include <PrsMgr_PresentationManager3d.hxx>

class Graphic3d_AspectFillArea3d;
class Graphic3d_Texture2Dmanual;

//! This class allows to map textures on shapes.
//! Presentations modes AIS_WireFrame (0) and AIS_Shaded (1) behave in the same manner as in AIS_Shape,
//! whilst new modes 2 (bounding box) and 3 (texture mapping) extends it functionality.
//!
//! The texture itself is parametrized in (0,1)x(0,1).
//! Each face of a shape located in UV space is provided with these parameters:
//! - Umin - starting position in U
//! - Umax - ending   position in U
//! - Vmin - starting position in V
//! - Vmax - ending   position in V
//! Each face is triangulated and a texel is assigned to each node.
//! Facets are then filled using a linear interpolation of texture between each 'three texels'.
//! User can act on:
//! - the number of occurrences of the texture on the face
//! - the position of the origin of the texture
//! - the scale factor of the texture
class AIS_TexturedShape : public AIS_Shape
{

public: //! @name main methods

  //! Initializes the textured shape.
  Standard_EXPORT AIS_TexturedShape (const TopoDS_Shape& theShape);

  //! Sets the texture source. <theTextureFileName> can specify path to texture image or one of the standard predefined textures.
  //! The accepted file types are those used in Image_AlienPixMap with extensions such as rgb, png, jpg and more.
  //! To specify the standard predefined texture, the <theTextureFileName> should contain integer - the Graphic3d_NameOfTexture2D enumeration index.
  //! Setting texture source using this method resets the source pixmap (if was set previously).
  Standard_EXPORT virtual void SetTextureFileName (const TCollection_AsciiString& theTextureFileName);

  //! Sets the texture source. <theTexturePixMap> specifies image data.
  //! Please note that the data should be in Bottom-Up order, the flag of Image_PixMap::IsTopDown() will be ignored by graphic driver.
  //! Setting texture source using this method resets the source by filename (if was set previously).
  Standard_EXPORT virtual void SetTexturePixMap (const Handle(Image_PixMap)& theTexturePixMap);

  //! @return flag to control texture mapping (for presentation mode 3)
  Standard_Boolean TextureMapState() const { return myToMapTexture; }

  //! Enables texture mapping
  Standard_EXPORT void SetTextureMapOn();

  //! Disables texture mapping
  Standard_EXPORT void SetTextureMapOff();

  //! @return path to the texture file
  Standard_CString TextureFile() const { return myTextureFile.ToCString(); }

  //! @return the source pixmap for texture map
  const Handle(Image_PixMap)& TexturePixMap() const { return myTexturePixMap; }

public: //! @name methods to alter texture mapping properties

  //! Use this method to display the textured shape without recomputing the whole presentation.
  //! Use this method when ONLY the texture content has been changed.
  //! If other parameters (ie: scale factors, texture origin, texture repeat...) have changed, the whole presentation has to be recomputed:
  //! @code
  //! if (myShape->DisplayMode() == 3)
  //! {
  //!   myAISContext->RecomputePrsOnly (myShape);
  //! }
  //! else
  //! {
  //!   myAISContext->SetDisplayMode (myShape, 3, Standard_False);
  //!   myAISContext->Display        (myShape, Standard_True);
  //! }
  //! @endcode
  Standard_EXPORT void UpdateAttributes();

  //! Sets the color.
  Standard_EXPORT virtual void SetColor (const Quantity_Color& theColor) Standard_OVERRIDE;

  //! Removes settings for the color.
  Standard_EXPORT virtual void UnsetColor() Standard_OVERRIDE;

  //! Sets the material aspect.
  Standard_EXPORT virtual void SetMaterial (const Graphic3d_MaterialAspect& theAspect) Standard_OVERRIDE;

  //! Removes settings for material aspect.
  Standard_EXPORT virtual void UnsetMaterial() Standard_OVERRIDE;

  //! Enables texture modulation
  Standard_EXPORT void EnableTextureModulate();

  //! Disables texture modulation
  Standard_EXPORT void DisableTextureModulate();

  //! @return texture repeat flag
  Standard_Boolean TextureRepeat() const { return myToRepeat; }

  //! @return texture repeat U value
  Standard_Real URepeat() const { return myUVRepeat.X(); }

  //! @return texture repeat V value
  Standard_Real VRepeat() const { return myUVRepeat.Y(); }

  //! Sets the number of occurrences of the texture on each face. The texture itself is parameterized in (0,1) by (0,1).
  //! Each face of the shape to be textured is parameterized in UV space (Umin,Umax) by (Vmin,Vmax).
  //! If RepeatYN is set to false, texture coordinates are clamped in the range (0,1)x(0,1) of the face.
  Standard_EXPORT void SetTextureRepeat (const Standard_Boolean theToRepeat,
                                         const Standard_Real    theURepeat = 1.0,
                                         const Standard_Real    theVRepeat = 1.0);

  //! @return true if texture UV origin has been modified
  Standard_Boolean TextureOrigin() const { return myIsCustomOrigin; }

  //! @return texture origin U position (0.0 by default)
  Standard_Real TextureUOrigin() const { return myUVOrigin.X(); }

  //! @return texture origin V position (0.0 by default)
  Standard_Real TextureVOrigin() const { return myUVOrigin.Y(); }

  //! Use this method to change the origin of the texture. The texel (0,0) will be mapped to the surface (UOrigin,VOrigin)
  Standard_EXPORT void SetTextureOrigin (const Standard_Boolean theToSetTextureOrigin,
                                         const Standard_Real    theUOrigin = 0.0,
                                         const Standard_Real    theVOrigin = 0.0);

  //! @return true if scale factor should be applied to texture mapping
  Standard_Boolean TextureScale() const { return myToScale; }

  //! @return scale factor for U coordinate (1.0 by default)
  Standard_Real TextureScaleU() const { return myUVScale.X(); }

  //! @return scale factor for V coordinate (1.0 by default)
  Standard_Real TextureScaleV() const { return myUVScale.Y(); }

  //! Use this method to scale the texture (percent of the face).
  //! You can specify a scale factor for both U and V.
  //! Example: if you set ScaleU and ScaleV to 0.5 and you enable texture repeat,
  //!          the texture will appear twice on the face in each direction.
  Standard_EXPORT void SetTextureScale (const Standard_Boolean theToSetTextureScale,
                                        const Standard_Real    theScaleU = 1.0,
                                        const Standard_Real    theScaleV = 1.0);

  //! @return true if displaying of triangles is requested
  Standard_Boolean ShowTriangles() const { return myToShowTriangles; }

  //! Use this method to show the triangulation of the shape (for debugging etc.).
  Standard_EXPORT void ShowTriangles (const Standard_Boolean theToShowTriangles);

  //! @return true if texture color modulation is turned on
  Standard_Boolean TextureModulate() const { return myModulate; }

protected: //! @name overridden methods

  //! Compute presentation with texture mapping support.
  Standard_EXPORT virtual void Compute (const Handle(PrsMgr_PresentationManager3d)& thePrsMgr,
                                        const Handle(Prs3d_Presentation)&           thePrs,
                                        const Standard_Integer                      theMode) Standard_OVERRIDE;

  Standard_EXPORT void updateAttributes (const Handle(Prs3d_Presentation)& thePrs);

protected: //! @name presentation fields

  Handle(Graphic3d_Texture2Dmanual)  myTexture;
  Handle(Graphic3d_AspectFillArea3d) myAspect;

protected: //! @name texture source fields

  Handle(Image_PixMap)               myTexturePixMap;
  TCollection_AsciiString            myTextureFile;
  Graphic3d_NameOfTexture2D          myPredefTexture;

protected: //! @name texture mapping properties

  Standard_Boolean                   myToMapTexture;
  Standard_Boolean                   myModulate;
  Standard_Boolean                   myIsCustomOrigin;
  Standard_Boolean                   myToRepeat;
  Standard_Boolean                   myToScale;
  Standard_Boolean                   myToShowTriangles;

public:

  DEFINE_STANDARD_RTTIEXT(AIS_TexturedShape,AIS_Shape)

};

DEFINE_STANDARD_HANDLE (AIS_TexturedShape, AIS_Shape)

#endif // _AIS_TexturedShape_HeaderFile
