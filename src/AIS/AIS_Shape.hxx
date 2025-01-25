// Created on: 1996-12-20
// Created by: Robert COUBLANC
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _AIS_Shape_HeaderFile
#define _AIS_Shape_HeaderFile

#include <AIS_DisplayMode.hxx>
#include <AIS_InteractiveObject.hxx>
#include <Bnd_Box.hxx>
#include <TopoDS_Shape.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_TypeOfHLR.hxx>

//! A framework to manage presentation and selection of shapes.
//! AIS_Shape is the interactive object which is used the
//! most by   applications. There are standard functions
//! available which allow you to prepare selection
//! operations on the constituent elements of shapes -
//! vertices, edges, faces etc - in an open local context.
//! The selection modes specific to "Shape" type objects
//! are referred to as Standard Activation Mode. These
//! modes are only taken into account in open local
//! context and only act on Interactive Objects which
//! have redefined the virtual method
//! AcceptShapeDecomposition so that it returns true.
//! Several advanced functions are also available. These
//! include functions to manage deviation angle and
//! deviation coefficient - both HLR and non-HLR - of
//! an inheriting shape class. These services allow you to
//! select one type of shape interactive object for higher
//! precision drawing. When you do this, the
//! Prs3d_Drawer::IsOwn... functions corresponding to the
//! above deviation angle and coefficient functions return
//! true indicating that there is a local setting available
//! for the specific object.
//!
//! This class allows to map textures on shapes using native UV parametric space of underlying
//! surface of each Face (this means that texture will be visually duplicated on all Faces). To
//! generate texture coordinates, appropriate shading attribute should be set before computing
//! presentation in AIS_Shaded display mode:
//! @code
//!   Handle(AIS_Shape) aPrs = new AIS_Shape();
//!   aPrs->Attributes()->SetupOwnShadingAspect();
//!   aPrs->Attributes()->ShadingAspect()->Aspect()->SetTextureMapOn();
//!   aPrs->Attributes()->ShadingAspect()->Aspect()->SetTextureMap (new Graphic3d_Texture2Dmanual
//!   (Graphic3d_NOT_2D_ALUMINUM));
//! @endcode
//! The texture itself is parametrized in (0,1)x(0,1).
class AIS_Shape : public AIS_InteractiveObject
{
  DEFINE_STANDARD_RTTIEXT(AIS_Shape, AIS_InteractiveObject)
public:
  //! Initializes construction of the shape shap from wires,
  //! edges and vertices.
  Standard_EXPORT AIS_Shape(const TopoDS_Shape& shap);

  //! Returns index 0. This value refers to SHAPE from TopAbs_ShapeEnum
  virtual Standard_Integer Signature() const Standard_OVERRIDE { return 0; }

  //! Returns Object as the type of Interactive Object.
  virtual AIS_KindOfInteractive Type() const Standard_OVERRIDE
  {
    return AIS_KindOfInteractive_Shape;
  }

  //! Returns true if the Interactive Object accepts shape decomposition.
  virtual Standard_Boolean AcceptShapeDecomposition() const Standard_OVERRIDE
  {
    return Standard_True;
  }

  //! Return true if specified display mode is supported.
  virtual Standard_Boolean AcceptDisplayMode(const Standard_Integer theMode) const Standard_OVERRIDE
  {
    return theMode >= 0 && theMode <= 2;
  }

  //! Returns this shape object.
  const TopoDS_Shape& Shape() const { return myshape; }

  //! Constructs an instance of the shape object theShape.
  void SetShape(const TopoDS_Shape& theShape)
  {
    myshape  = theShape;
    myCompBB = Standard_True;
  }

  //! Alias for ::SetShape().
  void Set(const TopoDS_Shape& theShape) { SetShape(theShape); }

  //! Sets a local value for deviation coefficient for this specific shape.
  Standard_EXPORT Standard_Boolean SetOwnDeviationCoefficient();

  //! Sets a local value for deviation angle for this specific shape.
  Standard_EXPORT Standard_Boolean SetOwnDeviationAngle();

  //! Sets a local value for deviation coefficient for this specific shape.
  Standard_EXPORT void SetOwnDeviationCoefficient(const Standard_Real aCoefficient);

  //! this compute a new angle and Deviation from the value anAngle
  //! and set the values stored in myDrawer with these that become local to the shape
  Standard_EXPORT void SetAngleAndDeviation(const Standard_Real anAngle);

  //! gives back the angle initial value put by the User.
  Standard_EXPORT Standard_Real UserAngle() const;

  //! sets myOwnDeviationAngle field in Prs3d_Drawer & recomputes presentation
  Standard_EXPORT void SetOwnDeviationAngle(const Standard_Real anAngle);

  //! Returns true and the values of the deviation
  //! coefficient aCoefficient and the previous deviation
  //! coefficient aPreviousCoefficient. If these values are
  //! not already set, false is returned.
  Standard_EXPORT Standard_Boolean
    OwnDeviationCoefficient(Standard_Real& aCoefficient, Standard_Real& aPreviousCoefficient) const;

  //! Returns true and the values of the deviation angle
  //! anAngle and the previous deviation angle aPreviousAngle.
  //! If these values are not already set, false is returned.
  Standard_EXPORT Standard_Boolean OwnDeviationAngle(Standard_Real& anAngle,
                                                     Standard_Real& aPreviousAngle) const;

  //! Sets the type of HLR algorithm used by the shape
  void SetTypeOfHLR(const Prs3d_TypeOfHLR theTypeOfHLR) { myDrawer->SetTypeOfHLR(theTypeOfHLR); }

  //! Gets the type of HLR algorithm
  Prs3d_TypeOfHLR TypeOfHLR() const { return myDrawer->TypeOfHLR(); }

  //! Sets the color aColor in the reconstructed
  //! compound shape. Acts via the Drawer methods below on the appearance of:
  //! -   free boundaries:
  //! Prs3d_Drawer_FreeBoundaryAspect,
  //! -   isos: Prs3d_Drawer_UIsoAspect,
  //! Prs3dDrawer_VIsoAspect,
  //! -   shared boundaries:
  //! Prs3d_Drawer_UnFreeBoundaryAspect,
  //! -   shading: Prs3d_Drawer_ShadingAspect,
  //! -   visible line color in hidden line mode:
  //! Prs3d_Drawer_SeenLineAspect
  //! -   hidden line color in hidden line mode:
  //! Prs3d_Drawer_HiddenLineAspect.
  Standard_EXPORT virtual void SetColor(const Quantity_Color& theColor) Standard_OVERRIDE;

  //! Removes settings for color in the reconstructed compound shape.
  Standard_EXPORT virtual void UnsetColor() Standard_OVERRIDE;

  //! Sets the value aValue for line width in the reconstructed compound shape.
  //! Changes line aspects for lines presentation.
  Standard_EXPORT virtual void SetWidth(const Standard_Real aValue) Standard_OVERRIDE;

  //! Removes the setting for line width in the reconstructed compound shape.
  Standard_EXPORT virtual void UnsetWidth() Standard_OVERRIDE;

  //! Allows you to provide settings for the material aName
  //! in the reconstructed compound shape.
  Standard_EXPORT virtual void SetMaterial(const Graphic3d_MaterialAspect& aName) Standard_OVERRIDE;

  //! Removes settings for material in the reconstructed compound shape.
  Standard_EXPORT virtual void UnsetMaterial() Standard_OVERRIDE;

  //! Sets the value aValue for transparency in the reconstructed compound shape.
  Standard_EXPORT virtual void SetTransparency(const Standard_Real aValue = 0.6) Standard_OVERRIDE;

  //! Removes the setting for transparency in the reconstructed compound shape.
  Standard_EXPORT virtual void UnsetTransparency() Standard_OVERRIDE;

  //! Constructs a bounding box with which to reconstruct
  //! compound topological shapes for presentation.
  Standard_EXPORT virtual const Bnd_Box& BoundingBox();

  //! AIS_InteractiveObject defines another virtual method BoundingBox,
  //! which is not the same as above; keep it visible.
  using AIS_InteractiveObject::BoundingBox;

  //! Returns the Color attributes of the shape accordingly to
  //! the current facing model;
  Standard_EXPORT virtual void Color(Quantity_Color& aColor) const Standard_OVERRIDE;

  //! Returns the NameOfMaterial attributes of the shape accordingly to
  //! the current facing model;
  Standard_EXPORT virtual Graphic3d_NameOfMaterial Material() const Standard_OVERRIDE;

  //! Returns the transparency attributes of the shape accordingly to
  //! the current facing model;
  Standard_EXPORT virtual Standard_Real Transparency() const Standard_OVERRIDE;

  //! Return shape type for specified selection mode.
  static TopAbs_ShapeEnum SelectionType(const Standard_Integer theSelMode)
  {
    switch (theSelMode)
    {
      case 1:
        return TopAbs_VERTEX;
      case 2:
        return TopAbs_EDGE;
      case 3:
        return TopAbs_WIRE;
      case 4:
        return TopAbs_FACE;
      case 5:
        return TopAbs_SHELL;
      case 6:
        return TopAbs_SOLID;
      case 7:
        return TopAbs_COMPSOLID;
      case 8:
        return TopAbs_COMPOUND;
      case 0:
        return TopAbs_SHAPE;
    }
    return TopAbs_SHAPE;
  }

  //! Return selection mode for specified shape type.
  static Standard_Integer SelectionMode(const TopAbs_ShapeEnum theShapeType)
  {
    switch (theShapeType)
    {
      case TopAbs_VERTEX:
        return 1;
      case TopAbs_EDGE:
        return 2;
      case TopAbs_WIRE:
        return 3;
      case TopAbs_FACE:
        return 4;
      case TopAbs_SHELL:
        return 5;
      case TopAbs_SOLID:
        return 6;
      case TopAbs_COMPSOLID:
        return 7;
      case TopAbs_COMPOUND:
        return 8;
      case TopAbs_SHAPE:
        return 0;
    }
    return 0;
  }

public: //! @name methods to alter texture mapping properties
  //! Return texture repeat UV values; (1, 1) by default.
  const gp_Pnt2d& TextureRepeatUV() const { return myUVRepeat; }

  //! Sets the number of occurrences of the texture on each face. The texture itself is
  //! parameterized in (0,1) by (0,1). Each face of the shape to be textured is parameterized in UV
  //! space (Umin,Umax) by (Vmin,Vmax).
  void SetTextureRepeatUV(const gp_Pnt2d& theRepeatUV) { myUVRepeat = theRepeatUV; }

  //! Return texture origin UV position; (0, 0) by default.
  const gp_Pnt2d& TextureOriginUV() const { return myUVOrigin; }

  //! Use this method to change the origin of the texture.
  //! The texel (0,0) will be mapped to the surface (myUVOrigin.X(), myUVOrigin.Y()).
  void SetTextureOriginUV(const gp_Pnt2d& theOriginUV) { myUVOrigin = theOriginUV; }

  //! Return scale factor for UV coordinates; (1, 1) by default.
  const gp_Pnt2d& TextureScaleUV() const { return myUVScale; }

  //! Use this method to scale the texture (percent of the face).
  //! You can specify a scale factor for both U and V.
  //! Example: if you set ScaleU and ScaleV to 0.5 and you enable texture repeat,
  //!          the texture will appear twice on the face in each direction.
  void SetTextureScaleUV(const gp_Pnt2d& theScaleUV) { myUVScale = theScaleUV; }

protected:
  //! Compute normal presentation.
  Standard_EXPORT virtual void Compute(const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                       const Handle(Prs3d_Presentation)&         thePrs,
                                       const Standard_Integer theMode) Standard_OVERRIDE;

  //! Compute projected presentation.
  virtual void computeHLR(const Handle(Graphic3d_Camera)&   theProjector,
                          const Handle(TopLoc_Datum3D)&     theTrsf,
                          const Handle(Prs3d_Presentation)& thePrs) Standard_OVERRIDE
  {
    if (!theTrsf.IsNull() && theTrsf->Form() != gp_Identity)
    {
      const TopLoc_Location& aLoc   = myshape.Location();
      const TopoDS_Shape     aShape = myshape.Located(TopLoc_Location(theTrsf->Trsf()) * aLoc);
      computeHlrPresentation(theProjector, thePrs, aShape, myDrawer);
    }
    else
    {
      computeHlrPresentation(theProjector, thePrs, myshape, myDrawer);
    }
  }

  //! Compute selection.
  Standard_EXPORT virtual void ComputeSelection(const Handle(SelectMgr_Selection)& theSelection,
                                                const Standard_Integer theMode) Standard_OVERRIDE;

  //! Create own aspects (if they do not exist) and set color to them.
  //! @return TRUE if new aspects have been created
  Standard_EXPORT bool setColor(const Handle(Prs3d_Drawer)& theDrawer,
                                const Quantity_Color&       theColor) const;

  //! Create own aspects (if they do not exist) and set width to them.
  //! @return TRUE if new aspects have been created
  Standard_EXPORT bool setWidth(const Handle(Prs3d_Drawer)& theDrawer,
                                const Standard_Real         theWidth) const;

  Standard_EXPORT void setTransparency(const Handle(Prs3d_Drawer)& theDrawer,
                                       const Standard_Real         theValue) const;

  Standard_EXPORT void setMaterial(const Handle(Prs3d_Drawer)&     theDrawer,
                                   const Graphic3d_MaterialAspect& theMaterial,
                                   const Standard_Boolean          theToKeepColor,
                                   const Standard_Boolean          theToKeepTransp) const;

  //! Replace aspects of already computed groups from drawer link by the new own value.
  Standard_EXPORT void replaceWithNewOwnAspects();

public:
  //! Compute HLR presentation for specified shape.
  Standard_EXPORT static void computeHlrPresentation(const Handle(Graphic3d_Camera)&   theProjector,
                                                     const Handle(Prs3d_Presentation)& thePrs,
                                                     const TopoDS_Shape&               theShape,
                                                     const Handle(Prs3d_Drawer)&       theDrawer);

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

protected:
  TopoDS_Shape     myshape;    //!< shape to display
  Bnd_Box          myBB;       //!< cached bounding box of the shape
  gp_Pnt2d         myUVOrigin; //!< UV origin vector for generating texture coordinates
  gp_Pnt2d         myUVRepeat; //!< UV repeat vector for generating texture coordinates
  gp_Pnt2d         myUVScale;  //!< UV scale  vector for generating texture coordinates
  Standard_Real    myInitAng;
  Standard_Boolean myCompBB; //!< if TRUE, then bounding box should be recomputed
};

DEFINE_STANDARD_HANDLE(AIS_Shape, AIS_InteractiveObject)

#endif // _AIS_Shape_HeaderFile
