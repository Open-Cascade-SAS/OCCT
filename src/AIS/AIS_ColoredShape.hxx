// Created on: 2014-04-24
// Created by: Kirill Gavrilov
// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef _AIS_ColoredShape_HeaderFile
#define _AIS_ColoredShape_HeaderFile

#include <Prs3d_Drawer.hxx>
#include <AIS_Shape.hxx>

#include <NCollection_DataMap.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS_Compound.hxx>
#include <StdPrs_Volume.hxx>

//! Customizable properties.
class AIS_ColoredDrawer : public Prs3d_Drawer
{
public:

  AIS_ColoredDrawer (const Handle(Prs3d_Drawer)& theLink)
  : myIsHidden    (Standard_False),
    myHasOwnColor (Standard_False),
    myHasOwnWidth (Standard_False)
  {
    Link (theLink);
  }

  Standard_Boolean IsHidden()    const                              { return myIsHidden; }
  void             SetHidden (const Standard_Boolean theToHide)     { myIsHidden = theToHide;  }
  Standard_Boolean HasOwnColor() const                              { return myHasOwnColor; }
  void             UnsetOwnColor()                                  { myHasOwnColor = Standard_False; }
  void             SetOwnColor (const Quantity_Color& /*theColor*/) { myHasOwnColor = Standard_True;  }
  Standard_Boolean HasOwnWidth() const                              { return myHasOwnWidth; }
  void             UnsetOwnWidth()                                  { myHasOwnWidth = Standard_False; }
  void             SetOwnWidth (const Standard_Real /*theWidth*/)   { myHasOwnWidth = Standard_True;  }

public:  //! @name list of overridden properties

  Standard_Boolean myIsHidden;
  Standard_Boolean myHasOwnColor;
  Standard_Boolean myHasOwnWidth;

public:
  DEFINE_STANDARD_RTTI(AIS_ColoredDrawer);

};

DEFINE_STANDARD_HANDLE(AIS_ColoredDrawer, Prs3d_Drawer)

//! Presentation of the shape with customizable sub-shapes properties.
class AIS_ColoredShape : public AIS_Shape
{
public:

  //! Default constructor
  Standard_EXPORT AIS_ColoredShape (const TopoDS_Shape& theShape);

  //! Copy constructor
  Standard_EXPORT AIS_ColoredShape (const Handle(AIS_Shape)& theShape);

public: //! @name sub-shape aspects

  //! Customize properties of specified sub-shape.
  //! The shape will be stored in the map but ignored, if it is not sub-shape of main Shape!
  //! This method can be used to mark sub-shapes with customizable properties.
  Standard_EXPORT Handle(AIS_ColoredDrawer) CustomAspects (const TopoDS_Shape& theShape);

  //! Reset the map of custom sub-shape aspects.
  Standard_EXPORT void ClearCustomAspects();

  //! Reset custom properties of specified sub-shape.
  //! @param theToUnregister unregister or not sub-shape from the map
  Standard_EXPORT void UnsetCustomAspects (const TopoDS_Shape&    theShape,
                                           const Standard_Boolean theToUnregister = Standard_False);

  //! Customize color of specified sub-shape
  Standard_EXPORT void SetCustomColor (const TopoDS_Shape&   theShape,
                                       const Quantity_Color& theColor);

  //! Customize line width of specified sub-shape
  Standard_EXPORT void SetCustomWidth (const TopoDS_Shape& theShape,
                                       const Standard_Real theLineWidth);

public: //! @name global aspects

  //! Setup color of entire shape.
  Standard_EXPORT virtual void SetColor (const Quantity_Color& theColor) Standard_OVERRIDE;

  //! Setup line width of entire shape.
  Standard_EXPORT virtual void SetWidth (const Standard_Real theLineWidth) Standard_OVERRIDE;

  //! Sets transparency value.
  Standard_EXPORT virtual void SetTransparency (const Standard_Real theValue) Standard_OVERRIDE;

  //! Sets the material aspect.
  Standard_EXPORT virtual void SetMaterial (const Graphic3d_MaterialAspect& theAspect) Standard_OVERRIDE;

protected: //! @name override presentation computation

  Standard_EXPORT virtual void Compute (const Handle(PrsMgr_PresentationManager3d)& thePrsMgr,
                                        const Handle(Prs3d_Presentation)&           thePrs,
                                        const Standard_Integer                      theMode) Standard_OVERRIDE;

protected:

  typedef NCollection_DataMap<TopoDS_Shape, Handle(AIS_ColoredDrawer), TopTools_ShapeMapHasher> DataMapOfShapeColor;
  typedef NCollection_DataMap<TopoDS_Shape, TopoDS_Shape,              TopTools_ShapeMapHasher> DataMapOfShapeShape;
  typedef NCollection_IndexedDataMap<TopoDS_Shape, TopoDS_Compound,    TopTools_ShapeMapHasher> DataMapOfShapeCompd;

protected:

  //! Recursive function to map shapes.
  //! @param theBaseKey                    the key to be used for undetailed shapes (default colors)
  //! @param theSubshapeToParse            the subshape to be parsed
  //! @param theSubshapeKeyshapeMap        shapes map Subshape (in the base shape) -> Keyshape (detailed shape)
  //! @param theParentType                 the parent subshape type
  //! @param theTypeKeyshapeDrawshapeArray the array of shape types to fill
  Standard_EXPORT static Standard_Boolean dispatchColors (const TopoDS_Shape&        theBaseKey,
                                                          const TopoDS_Shape&        theSubshapeToParse,
                                                          const DataMapOfShapeShape& theSubshapeKeyshapeMap,
                                                          const TopAbs_ShapeEnum     theParentType,
                                                          DataMapOfShapeCompd*       theTypeKeyshapeDrawshapeArray);

  Standard_EXPORT static void dispatchColors (const TopoDS_Shape&        theBaseShape,
                                              const DataMapOfShapeColor& theKeyshapeColorMap,
                                              DataMapOfShapeCompd*       theTypeKeyshapeDrawshapeArray);

protected:

  //! Add shape to presentation
  //! @param thePrs         the presentation
  //! @param theDispatched  the shapes map with unique attributes
  //! @param theMode        display mode
  //! @param theVolume      how to interpret theDispatched shapes - as Closed volumes, as Open volumes
  //!                       or to perform Autodetection
  Standard_EXPORT void addShapesWithCustomProps (const Handle(Prs3d_Presentation)& thePrs,
                                                 DataMapOfShapeCompd*              theDispatched,
                                                 const Standard_Integer            theMode,
                                                 const StdPrs_Volume               theVolume);

  //! Check all shapes from myShapeColorsfor visibility
  Standard_EXPORT Standard_Boolean isShapeEntirelyVisible() const;

  //! Check a shape with unique attributes for visibility of all 2d subshape
  Standard_EXPORT Standard_Boolean isShapeEntirelyVisible (DataMapOfShapeCompd* theDispatched) const;

  //! Resolve (parse) theKeyShape into subshapes, search in they for theBaseShape,
  //! bind all resolved subshapes with theOriginKeyShape and store all binds in theSubshapeKeyshapeMap
  //! @param theSubshapeKeyshapeMap        shapes map: resolved and found theBaseShape subshape -> theOriginKeyShape 
  //! @param theBaseShape                  a shape to be sought
  //! @param theBaseKey                    a shape to be resolved (parse) into smaller (in topological sense)
  //!                                      subshapes for new bind cycle
  //! @param theOriginKeyShape             the key to be used for undetailed shapes (default colors)
  Standard_EXPORT static void bindSubShapes (DataMapOfShapeShape& theSubshapeKeyshapeMap,
                                             const TopoDS_Shape&  theBaseShape,
                                             const TopoDS_Shape&  theKeyShape,
                                             const TopoDS_Shape&  theOriginKeyShape);

protected:

  DataMapOfShapeColor myShapeColors;

public:

  DEFINE_STANDARD_RTTI(AIS_ColoredShape);

};

DEFINE_STANDARD_HANDLE(AIS_ColoredShape, AIS_Shape)

#endif // _AIS_ColoredShape_HeaderFile
