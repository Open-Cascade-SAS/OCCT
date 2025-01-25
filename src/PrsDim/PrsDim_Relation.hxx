// Created on: 1997-02-27
// Created by: Odile Olivier
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

#ifndef _PrsDim_Relation_HeaderFile
#define _PrsDim_Relation_HeaderFile

#include <AIS_InteractiveObject.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Aspect_TypeOfMarker.hxx>
#include <Bnd_Box.hxx>
#include <DsgPrs_ArrowSide.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <PrsDim_KindOfDimension.hxx>
#include <PrsDim_KindOfSurface.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TopoDS_Shape.hxx>

class Geom_Curve;
class Geom_Plane;
class Geom_Surface;
class TopoDS_Edge;
class TopoDS_Vertex;

//! One of the four types of interactive object in
//! AIS,comprising dimensions and constraints. Serves
//! as the abstract class for the seven relation classes as
//! well as the seven dimension classes.
//! The statuses available for relations between shapes are as follows:
//! -   0 - there is no connection to a shape;
//! -   1 - there is a connection to the first shape;
//! -   2 - there is a connection to the second shape.
//! The connection takes the form of an edge between the two shapes.
class PrsDim_Relation : public AIS_InteractiveObject
{
  DEFINE_STANDARD_RTTIEXT(PrsDim_Relation, AIS_InteractiveObject)
public:
  //! Allows you to provide settings for the color theColor
  //! of the lines representing the relation between the two shapes.
  Standard_EXPORT void SetColor(const Quantity_Color& theColor) Standard_OVERRIDE;

  //! Allows you to remove settings for the color of the
  //! lines representing the relation between the two shapes.
  Standard_EXPORT void UnsetColor() Standard_OVERRIDE;

  virtual AIS_KindOfInteractive Type() const Standard_OVERRIDE
  {
    return AIS_KindOfInteractive_Relation;
  }

  //! Indicates that the type of dimension is unknown.
  virtual PrsDim_KindOfDimension KindOfDimension() const { return PrsDim_KOD_NONE; }

  //! Returns true if the interactive object is movable.
  virtual Standard_Boolean IsMovable() const { return Standard_False; }

  const TopoDS_Shape& FirstShape() const { return myFShape; }

  virtual void SetFirstShape(const TopoDS_Shape& aFShape) { myFShape = aFShape; }

  //! Returns the second shape.
  const TopoDS_Shape& SecondShape() const { return mySShape; }

  //! Allows you to identify the second shape aSShape
  //! relative to the first.
  virtual void SetSecondShape(const TopoDS_Shape& aSShape) { mySShape = aSShape; }

  void SetBndBox(const Standard_Real theXmin,
                 const Standard_Real theYmin,
                 const Standard_Real theZmin,
                 const Standard_Real theXmax,
                 const Standard_Real theYmax,
                 const Standard_Real theZmax)
  {
    myBndBox.Update(theXmin, theYmin, theZmin, theXmax, theYmax, theZmax);
    myIsSetBndBox = Standard_True;
  }

  void UnsetBndBox() { myIsSetBndBox = Standard_False; }

  //! Returns the plane.
  const Handle(Geom_Plane)& Plane() const { return myPlane; }

  //! Allows you to set the plane thePlane. This is used to
  //! define relations and dimensions in several daughter classes.
  void SetPlane(const Handle(Geom_Plane)& thePlane) { myPlane = thePlane; }

  //! Returns the value of each object in the relation.
  Standard_Real Value() const { return myVal; }

  //! Allows you to provide settings for the value theVal for each object in the relation.
  void SetValue(const Standard_Real theVal) { myVal = theVal; }

  //! Returns the position set using SetPosition.
  const gp_Pnt& Position() const { return myPosition; }

  //! Allows you to provide the objects in the relation with
  //! settings for a non-default position.
  void SetPosition(const gp_Pnt& thePosition)
  {
    myPosition          = thePosition;
    myAutomaticPosition = Standard_False;
  }

  //! Returns settings for text aspect.
  const TCollection_ExtendedString& Text() const { return myText; }

  //! Allows you to provide the settings theText for text aspect.
  void SetText(const TCollection_ExtendedString& theText) { myText = theText; }

  //! Returns the value for the size of the arrow identifying
  //! the relation between the two shapes.
  Standard_Real ArrowSize() const { return myArrowSize; }

  //! Allows you to provide settings for the size of the
  //! arrow theArrowSize identifying the relation between the two shapes.
  void SetArrowSize(const Standard_Real theArrowSize)
  {
    myArrowSize          = theArrowSize;
    myArrowSizeIsDefined = Standard_True;
  }

  //! Returns the value of the symbol presentation. This will be one of:
  //! -   AS_NONE - none
  //! -   AS_FIRSTAR - first arrow
  //! -   AS_LASTAR - last arrow
  //! -   AS_BOTHAR - both arrows
  //! -   AS_FIRSTPT - first point
  //! -   AS_LASTPT - last point
  //! -   AS_BOTHPT - both points
  //! -   AS_FIRSTAR_LASTPT - first arrow, last point
  //! -   AS_FIRSTPT_LASTAR - first point, last arrow
  DsgPrs_ArrowSide SymbolPrs() const { return mySymbolPrs; }

  //! Allows you to provide settings for the symbol presentation.
  void SetSymbolPrs(const DsgPrs_ArrowSide theSymbolPrs) { mySymbolPrs = theSymbolPrs; }

  //! Allows you to set the status of the extension shape by
  //! the index aIndex.
  //! The status will be one of the following:
  //! -   0 - there is no connection to a shape;
  //! -   1 - there is a connection to the first shape;
  //! -   2 - there is a connection to the second shape.
  void SetExtShape(const Standard_Integer theIndex) { myExtShape = theIndex; }

  //! Returns the status index of the extension shape.
  Standard_Integer ExtShape() const { return myExtShape; }

  //! Returns true if the display mode aMode is accepted
  //! for the Interactive Objects in the relation.
  //! ComputeProjPresentation(me;
  //! aPres    : Presentation from Prs3d;
  //! Curve1   : Curve                from Geom;
  //! Curve2   : Curve                from Geom;
  //! FirstP1  : Pnt                  from gp;
  //! LastP1   : Pnt                  from gp;
  //! FirstP2  : Pnt                  from gp;
  //! LastP2   : Pnt                  from gp;
  //! aColor   : NameOfColor          from Quantity = Quantity_NOC_PURPLE;
  //! aWidth   : Real                 from Standard = 2;
  //! aProjTOL : TypeOfLine           from Aspect   = Aspect_TOL_DASH;
  //! aCallTOL : TypeOfLine           from Aspect   = Aspect_TOL_DOT)
  virtual Standard_Boolean AcceptDisplayMode(const Standard_Integer theMode) const Standard_OVERRIDE
  {
    return theMode == 0;
  }

  void SetAutomaticPosition(const Standard_Boolean theStatus) { myAutomaticPosition = theStatus; }

  Standard_Boolean AutomaticPosition() const { return myAutomaticPosition; }

protected:
  Standard_EXPORT PrsDim_Relation(
    const PrsMgr_TypeOfPresentation3d aTypeOfPresentation3d = PrsMgr_TOP_AllView);

  //! Calculates the presentation aPres of the edge
  //! anEdge and the curve it defines, ProjCurve. The later
  //! is also specified by the first point FirstP and the last point LastP.
  //! The presentation includes settings for color aColor,
  //! type - aProjTOL and aCallTOL -   and width of line, aWidth.
  Standard_EXPORT void ComputeProjEdgePresentation(
    const Handle(Prs3d_Presentation)& aPres,
    const TopoDS_Edge&                anEdge,
    const Handle(Geom_Curve)&         ProjCurve,
    const gp_Pnt&                     FirstP,
    const gp_Pnt&                     LastP,
    const Quantity_NameOfColor        aColor   = Quantity_NOC_PURPLE,
    const Standard_Real               aWidth   = 2,
    const Aspect_TypeOfLine           aProjTOL = Aspect_TOL_DASH,
    const Aspect_TypeOfLine           aCallTOL = Aspect_TOL_DOT) const;

  //! Calculates the presentation aPres of the vertex
  //! aVertex and the point it defines, ProjPoint.
  //! The presentation includes settings for color aColor,
  //! type - aProjTOM and aCallTOL -   and width of line, aWidth.
  Standard_EXPORT void ComputeProjVertexPresentation(
    const Handle(Prs3d_Presentation)& aPres,
    const TopoDS_Vertex&              aVertex,
    const gp_Pnt&                     ProjPoint,
    const Quantity_NameOfColor        aColor   = Quantity_NOC_PURPLE,
    const Standard_Real               aWidth   = 2,
    const Aspect_TypeOfMarker         aProjTOM = Aspect_TOM_PLUS,
    const Aspect_TypeOfLine           aCallTOL = Aspect_TOL_DOT) const;

protected:
  TopoDS_Shape               myFShape;
  TopoDS_Shape               mySShape;
  Handle(Geom_Plane)         myPlane;
  Standard_Real              myVal;
  gp_Pnt                     myPosition;
  TCollection_ExtendedString myText;
  Standard_Real              myArrowSize;
  Standard_Boolean           myAutomaticPosition;
  DsgPrs_ArrowSide           mySymbolPrs;
  Standard_Integer           myExtShape;
  gp_Pln                     myFirstPlane;
  gp_Pln                     mySecondPlane;
  Handle(Geom_Surface)       myFirstBasisSurf;
  Handle(Geom_Surface)       mySecondBasisSurf;
  PrsDim_KindOfSurface       myFirstSurfType;
  PrsDim_KindOfSurface       mySecondSurfType;
  Standard_Real              myFirstOffset;
  Standard_Real              mySecondOffset;
  Bnd_Box                    myBndBox;
  Standard_Boolean           myIsSetBndBox;
  Standard_Boolean           myArrowSizeIsDefined;
};

DEFINE_STANDARD_HANDLE(PrsDim_Relation, AIS_InteractiveObject)

#endif // _AIS_Relation_HeaderFile
