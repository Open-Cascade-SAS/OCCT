// Created on: 1995-10-09
// Created by: Arnaud BOUZY/Odile Olivier
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _AIS_Trihedron_HeaderFile
#define _AIS_Trihedron_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <Quantity_NameOfColor.hxx>
#include <AIS_InteractiveObject.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <SelectMgr_Selection.hxx>
#include <AIS_KindOfInteractive.hxx>
#include <TColgp_Array1OfPnt.hxx>
class Geom_Axis2Placement;
class AIS_InteractiveObject;
class AIS_Axis;
class AIS_Point;
class AIS_Plane;
class AIS_InteractiveContext;
class Prs3d_Presentation;
class Prs3d_Projector;
class Geom_Transformation;
class gp_Trsf;
class Quantity_Color;


class AIS_Trihedron;
DEFINE_STANDARD_HANDLE(AIS_Trihedron, AIS_InteractiveObject)

//! Create a selectable trihedron
//! there are 4 modes of selection :
//! mode = 0 to select triedron    ,priority = 1
//! mode = 1 to select its origine ,priority = 5
//! mode = 2 to select its axis    ,priority = 3
//! mode = 3 to select its planes  ,priority = 2
//! a trihedron has 1 origine,3 axes,3 planes.
//! Warning
//! For the presentation of trihedra, the default unit of
//! length is the millimetre, and the default value for the
//! representation of the axes is 100. If you modify these
//! dimensions, you must temporarily recover the Drawer.
//! From inside it, you take the aspect in which the values
//! for length are stocked. For trihedra, this is
//! Prs3d_Drawer_FirstAxisAspect. You change the
//! values inside this Aspect and recalculate the presentation.
//! If you want to use extended selection modes, different than 0,
//! you should take care of removing of the shapes from the interactive
//! context that has been computed for selection; it might be necessary
//! when you change selection mode. You can use methods Axis, Point,
//! Plane to retrieve the shapes.
class AIS_Trihedron : public AIS_InteractiveObject
{

public:

  
  //! Initializes a trihedron entity.
  Standard_EXPORT AIS_Trihedron(const Handle(Geom_Axis2Placement)& aComponent);
  
  //! Returns the right-handed coordinate system set in SetComponent.
    const Handle(Geom_Axis2Placement)& Component() const;
  
  //! Constructs the right-handed coordinate system aComponent.
  Standard_EXPORT void SetComponent (const Handle(Geom_Axis2Placement)& aComponent);
  
  //! Returns true if the trihedron object has a size other
  //! than the default size of 100 mm. along each axis.
    Standard_Boolean HasOwnSize() const;
  
  //! Sets the size aValue for the trihedron object.
  //! The default value is 100 mm.
  Standard_EXPORT void SetSize (const Standard_Real aValue);
  
  //! Removes any non-default settings for size of this
  //! trihedron object.
  Standard_EXPORT void UnsetSize();
  
  Standard_EXPORT Standard_Real Size() const;
  
  //! Returns the "XAxis".
  Standard_EXPORT Handle(AIS_Axis) XAxis() const;
  
  //! Returns the "YAxis".
  Standard_EXPORT Handle(AIS_Axis) YAxis() const;
  
  //! Returns the main Axis.
  Standard_EXPORT Handle(AIS_Axis) Axis() const;
  
  //! Returns the origine.
  Standard_EXPORT Handle(AIS_Point) Position() const;
  
  //! Returns the "XYPlane".
  Standard_EXPORT Handle(AIS_Plane) XYPlane() const;
  
  //! Returns the "XZPlane".
  Standard_EXPORT Handle(AIS_Plane) XZPlane() const;
  
  //! Returns the "YZPlane".
  Standard_EXPORT Handle(AIS_Plane) YZPlane() const;
  
  //! connection to <aCtx> default drawer implies a recomputation
  //! of SubObjects values.
  Standard_EXPORT virtual void SetContext (const Handle(AIS_InteractiveContext)& aCtx) Standard_OVERRIDE;
  
  //! Returns true if the display mode selected, aMode, is
  //! valid for trihedron datums.
  Standard_EXPORT Standard_Boolean AcceptDisplayMode (const Standard_Integer aMode) const Standard_OVERRIDE;
  
  //! Computes the presentation according to a point of view
  //! given by <aProjector>.
  //! To be Used when the associated degenerated Presentations
  //! have been transformed by <aTrsf> which is not a Pure
  //! Translation. The HLR Prs can't be deducted automatically
  //! WARNING :<aTrsf> must be applied
  //! to the object to display before computation  !!!
  Standard_EXPORT virtual void Compute (const Handle(Prs3d_Projector)& aProjector, const Handle(Geom_Transformation)& aTrsf, const Handle(Prs3d_Presentation)& aPresentation) Standard_OVERRIDE;
  
  Standard_EXPORT void SetLocalTransformation (const gp_Trsf& theTransformation) Standard_OVERRIDE;
  
  //! Returns index 3, selection of the planes XOY, YOZ, XOZ.
  Standard_EXPORT virtual Standard_Integer Signature() const Standard_OVERRIDE;
  
  //! Indicates that the type of Interactive Object is datum.
  Standard_EXPORT virtual AIS_KindOfInteractive Type() const Standard_OVERRIDE;
  
  Standard_EXPORT void SetColor (const Quantity_NameOfColor aColor) Standard_OVERRIDE;
  
  //! Sets the color aColor for this trihedron object.
  Standard_EXPORT void SetColor (const Quantity_Color& aColor) Standard_OVERRIDE;
  
  Standard_EXPORT void SetTextColor (const Quantity_NameOfColor aColor);
  
  Standard_EXPORT Standard_Boolean HasTextColor() const;
  
  Standard_EXPORT Quantity_NameOfColor TextColor() const;
  
  Standard_EXPORT void SetArrowColor (const Quantity_NameOfColor aColor);
  
  Standard_EXPORT Standard_Boolean HasArrowColor() const;
  
  Standard_EXPORT Quantity_NameOfColor ArrowColor() const;
  
  //! Returns the four extremities of the trihedron from the
  //! array of points, TheExtrem.
  Standard_EXPORT void ExtremityPoints (TColgp_Array1OfPnt& TheExtrem) const;
  
  //! Removes the settings for color.
  Standard_EXPORT void UnsetColor() Standard_OVERRIDE;
  
  //! Removes the non-default settings for width set in SetWidth.
  Standard_EXPORT void UnsetWidth() Standard_OVERRIDE;

  DEFINE_STANDARD_RTTI(AIS_Trihedron,AIS_InteractiveObject)

protected:




private:

  
  Standard_EXPORT void Compute (const Handle(PrsMgr_PresentationManager3d)& aPresentationManager, const Handle(Prs3d_Presentation)& aPresentation, const Standard_Integer aMode = 0) Standard_OVERRIDE;
  
  Standard_EXPORT void Compute (const Handle(Prs3d_Projector)& aProjector, const Handle(Prs3d_Presentation)& aPresentation) Standard_OVERRIDE;
  
  Standard_EXPORT void ComputeSelection (const Handle(SelectMgr_Selection)& aSelection, const Standard_Integer aMode);
  
  Standard_EXPORT void LoadSubObjects();

protected:

  Handle(Geom_Axis2Placement) myComponent;
  Handle(AIS_InteractiveObject) myShapes[7];
  Standard_Boolean myHasOwnSize;
  Standard_Boolean myHasOwnTextColor;
  Quantity_NameOfColor myOwnTextColor;
  Standard_Boolean myHasOwnArrowColor;
  Quantity_NameOfColor myOwnArrowColor;
};


#include <AIS_Trihedron.lxx>





#endif // _AIS_Trihedron_HeaderFile
