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

#include <AIS_ColoredDrawer.hxx>
#include <NCollection_DataMap.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <AIS_Shape.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_Map.hxx>

class StdSelect_BRepOwner;

//! Presentation of the shape with customizable sub-shapes properties.
class AIS_ColoredShape : public AIS_Shape
{
public:
  //! Default constructor
  Standard_EXPORT AIS_ColoredShape(const TopoDS_Shape& theShape);

  //! Copy constructor
  Standard_EXPORT AIS_ColoredShape(const occ::handle<AIS_Shape>& theShape);

public: //! @name sub-shape aspects
  //! Customize properties of specified sub-shape.
  //! The shape will be stored in the map but ignored, if it is not sub-shape of main Shape!
  //! This method can be used to mark sub-shapes with customizable properties.
  Standard_EXPORT virtual occ::handle<AIS_ColoredDrawer> CustomAspects(
    const TopoDS_Shape& theShape);

  //! Reset the map of custom sub-shape aspects.
  Standard_EXPORT virtual void ClearCustomAspects();

  //! Reset custom properties of specified sub-shape.
  //! @param theToUnregister unregister or not sub-shape from the map
  Standard_EXPORT void UnsetCustomAspects(const TopoDS_Shape& theShape,
                                          const bool          theToUnregister = false);

  //! Customize color of specified sub-shape
  Standard_EXPORT void SetCustomColor(const TopoDS_Shape& theShape, const Quantity_Color& theColor);

  //! Customize transparency of specified sub-shape
  Standard_EXPORT void SetCustomTransparency(const TopoDS_Shape& theShape, double theTransparency);

  //! Customize line width of specified sub-shape
  Standard_EXPORT void SetCustomWidth(const TopoDS_Shape& theShape, const double theLineWidth);

  //! Return the map of custom aspects.
  const NCollection_DataMap<TopoDS_Shape, occ::handle<AIS_ColoredDrawer>, TopTools_ShapeMapHasher>&
    CustomAspectsMap() const
  {
    return myShapeColors;
  }

  //! Return the map of custom aspects.
  NCollection_DataMap<TopoDS_Shape, occ::handle<AIS_ColoredDrawer>, TopTools_ShapeMapHasher>&
    ChangeCustomAspectsMap()
  {
    return myShapeColors;
  }

public: //! @name global aspects
  //! Setup color of entire shape.
  Standard_EXPORT virtual void SetColor(const Quantity_Color& theColor) override;

  //! Setup line width of entire shape.
  Standard_EXPORT virtual void SetWidth(const double theLineWidth) override;

  //! Sets transparency value.
  Standard_EXPORT virtual void SetTransparency(const double theValue) override;

  //! Sets the material aspect.
  Standard_EXPORT virtual void SetMaterial(const Graphic3d_MaterialAspect& theAspect) override;

public:
  //! Removes the setting for transparency in the reconstructed compound shape.
  Standard_EXPORT virtual void UnsetTransparency() override;

  //! Setup line width of entire shape.
  Standard_EXPORT virtual void UnsetWidth() override;

protected: //! @name override presentation computation
  //! Compute presentation considering sub-shape color map.
  Standard_EXPORT virtual void Compute(const occ::handle<PrsMgr_PresentationManager>& thePrsMgr,
                                       const occ::handle<Prs3d_Presentation>&         thePrs,
                                       const int theMode) override;

  //! Compute selection considering sub-shape hidden state.
  Standard_EXPORT virtual void ComputeSelection(
    const occ::handle<SelectMgr_Selection>& theSelection,
    const int                               theMode) override;

protected:
  typedef NCollection_IndexedDataMap<occ::handle<AIS_ColoredDrawer>, TopoDS_Compound>
    DataMapOfDrawerCompd;

protected:
  //! Recursive function to map shapes.
  //! @param theParentDrawer   the drawer to be used for undetailed shapes (default colors)
  //! @param theShapeToParse   the subshape to be recursively parsed
  //! @param theShapeDrawerMap shapes map Subshape (in the base shape) -> Drawer
  //! @param theParentType     the parent subshape type
  //! @param theIsParentClosed flag indicating that specified shape is part of closed Solid
  //! @param theDrawerOpenedShapePerType the array of shape types to fill
  //! @param theDrawerClosedFaces        the map for closed faces
  Standard_EXPORT static bool dispatchColors(
    const occ::handle<AIS_ColoredDrawer>&               theParentDrawer,
    const TopoDS_Shape&                                 theShapeToParse,
    const NCollection_DataMap<TopoDS_Shape,
                              occ::handle<AIS_ColoredDrawer>,
                              TopTools_ShapeMapHasher>& theShapeDrawerMap,
    const TopAbs_ShapeEnum                              theParentType,
    const bool                                          theIsParentClosed,
    DataMapOfDrawerCompd*                               theDrawerOpenedShapePerType,
    DataMapOfDrawerCompd&                               theDrawerClosedFaces);

protected:
  //! Extract myShapeColors map (KeyshapeColored -> Color) to subshapes map (Subshape -> Color).
  //! This needed when colored shape is not part of BaseShape (but subshapes are) and actually
  //! container for subshapes.
  Standard_EXPORT void fillSubshapeDrawerMap(
    NCollection_DataMap<TopoDS_Shape, occ::handle<AIS_ColoredDrawer>, TopTools_ShapeMapHasher>&
      theSubshapeDrawerMap) const;

  //! Add shape to presentation
  //! @param thePrs the presentation
  //! @param theDrawerOpenedShapePerType the shapes map with unique attributes
  //! @param theDrawerClosedFaces the map of attributes for closed faces
  //! @param theMode display mode
  Standard_EXPORT void addShapesWithCustomProps(
    const occ::handle<Prs3d_Presentation>& thePrs,
    const DataMapOfDrawerCompd*            theDrawerOpenedShapePerType,
    const DataMapOfDrawerCompd&            theDrawerClosedFaces,
    const int                              theMode);

  //! Check all shapes from myShapeColorsfor visibility
  Standard_EXPORT bool isShapeEntirelyVisible() const;

  //! Resolve (parse) theKeyShape into subshapes, search in they for theBaseShape,
  //! bind all resolved subshapes with theOriginKeyShape and store all binds in theShapeDrawerMap
  //! @param theShapeDrawerMap shapes map: resolved and found theBaseShape subshape ->
  //! theOriginKeyShape
  //! @param theKeyShape       a shape to be resolved (parse) into smaller (in topological sense)
  //!                          subshapes for new bind cycle
  //! @param theDrawer         assigned drawer
  Standard_EXPORT void bindSubShapes(
    NCollection_DataMap<TopoDS_Shape, occ::handle<AIS_ColoredDrawer>, TopTools_ShapeMapHasher>&
                                          theShapeDrawerMap,
    const TopoDS_Shape&                   theKeyShape,
    const occ::handle<AIS_ColoredDrawer>& theDrawer) const;

  //! Add sub-shape to selection considering hidden state (recursively).
  //! @param theParentDrawer   drawer of parent shape
  //! @param theShapeDrawerMap shapes map
  //! @param theShape          shape to compute sensitive entities
  //! @param theOwner          selectable owner object
  //! @param theSelection      selection to append new sensitive entities
  //! @param theTypOfSel       type of selection
  //! @param theDeflection     linear deflection
  //! @param theDeflAngle      angular deflection
  Standard_EXPORT void computeSubshapeSelection(
    const occ::handle<AIS_ColoredDrawer>&               theParentDrawer,
    const NCollection_DataMap<TopoDS_Shape,
                              occ::handle<AIS_ColoredDrawer>,
                              TopTools_ShapeMapHasher>& theShapeDrawerMap,
    const TopoDS_Shape&                                 theShape,
    const occ::handle<StdSelect_BRepOwner>&             theOwner,
    const occ::handle<SelectMgr_Selection>&             theSelection,
    const TopAbs_ShapeEnum                              theTypOfSel,
    const int                                           thePriority,
    const double                                        theDeflection,
    const double                                        theDeflAngle);

protected:
  NCollection_DataMap<TopoDS_Shape, occ::handle<AIS_ColoredDrawer>, TopTools_ShapeMapHasher>
    myShapeColors;

public:
  DEFINE_STANDARD_RTTIEXT(AIS_ColoredShape, AIS_Shape)
};

#endif // _AIS_ColoredShape_HeaderFile
