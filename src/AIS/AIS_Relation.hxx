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

#ifndef _AIS_Relation_HeaderFile
#define _AIS_Relation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Shape.hxx>
#include <Standard_Real.hxx>
#include <gp_Pnt.hxx>
#include <TCollection_ExtendedString.hxx>
#include <Standard_Boolean.hxx>
#include <DsgPrs_ArrowSide.hxx>
#include <Standard_Integer.hxx>
#include <gp_Pln.hxx>
#include <AIS_KindOfSurface.hxx>
#include <Bnd_Box.hxx>
#include <AIS_InteractiveObject.hxx>
#include <PrsMgr_TypeOfPresentation3d.hxx>
#include <Quantity_NameOfColor.hxx>
#include <AIS_KindOfInteractive.hxx>
#include <AIS_KindOfDimension.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Aspect_TypeOfMarker.hxx>
class Geom_Plane;
class Geom_Surface;
class Quantity_Color;
class TopoDS_Shape;
class gp_Pnt;
class TCollection_ExtendedString;
class Prs3d_Presentation;
class TopoDS_Edge;
class Geom_Curve;
class TopoDS_Vertex;


class AIS_Relation;
DEFINE_STANDARD_HANDLE(AIS_Relation, AIS_InteractiveObject)

//! One of the four types of interactive object in
//! AIS,comprising dimensions and constraints. Serves
//! as the abstract class for the seven relation classes as
//! well as the seven dimension classes.
//! The statuses available for relations between shapes are as follows:
//! -   0 - there is no connection to a shape;
//! -   1 - there is a connection to the first shape;
//! -   2 - there is a connection to the second shape.
//! The connection takes the form of an edge between the two shapes.
class AIS_Relation : public AIS_InteractiveObject
{

public:

  
  //! Allows you to provide settings for the color aColor
  //! of the lines representing the relation between the two shapes.
  Standard_EXPORT void SetColor (const Quantity_NameOfColor aColor) Standard_OVERRIDE;
  
  Standard_EXPORT void SetColor (const Quantity_Color& aColor) Standard_OVERRIDE;
  
  //! Allows you to remove settings for the color of the
  //! lines representing the relation between the two shapes.
  Standard_EXPORT void UnsetColor() Standard_OVERRIDE;
  
    virtual AIS_KindOfInteractive Type() const Standard_OVERRIDE;
  
  //! Indicates that the type of dimension is unknown.
  Standard_EXPORT virtual AIS_KindOfDimension KindOfDimension() const;
  
  //! Returns true if the interactive object is movable.
  Standard_EXPORT virtual Standard_Boolean IsMovable() const;
  
    const TopoDS_Shape& FirstShape() const;
  
  Standard_EXPORT virtual void SetFirstShape (const TopoDS_Shape& aFShape);
  
  //! Returns the second shape.
    const TopoDS_Shape& SecondShape() const;
  
  //! Allows you to identify the second shape aSShape
  //! relative to the first.
  Standard_EXPORT virtual void SetSecondShape (const TopoDS_Shape& aSShape);
  
    void SetBndBox (const Standard_Real Xmin, const Standard_Real Ymin, const Standard_Real Zmin, const Standard_Real Xmax, const Standard_Real Ymax, const Standard_Real Zmax);
  
    void UnsetBndBox();
  
  //! Returns the plane.
    const Handle(Geom_Plane)& Plane() const;
  

  //! Allows you to set the plane aPlane. This is used to
  //! define relations and dimensions in several daughter classes.
    void SetPlane (const Handle(Geom_Plane)& aPlane);
  
  //! Returns the value of each object in the relation.
    Standard_Real Value() const;
  
  //! Allows you to provide settings for the value aVal for
  //! each object in the relation.
    void SetValue (const Standard_Real aVal);
  
  //! Returns the position set using SetPosition.
    const gp_Pnt& Position() const;
  

  //! Allows you to provide the objects in the relation with
  //! settings for a non-default position.
    void SetPosition (const gp_Pnt& aPosition);
  
  //! Returns settings for text aspect.
    const TCollection_ExtendedString& Text() const;
  
  //! Allows you to provide the settings aText for text aspect.
    void SetText (const TCollection_ExtendedString& aText);
  

  //! Returns the value for the size of the arrow identifying
  //! the relation between the two shapes.
    Standard_Real ArrowSize() const;
  

  //! Allows you to provide settings for the size of the
  //! arrow anArrowsize identifying the relation between the two shapes.
    void SetArrowSize (const Standard_Real anArrowSize);
  

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
    DsgPrs_ArrowSide SymbolPrs() const;
  

  //! Allows you to provide settings for the symbol presentation.
    void SetSymbolPrs (const DsgPrs_ArrowSide aSymbolPrs);
  

  //! Allows you to set the status of the extension shape by
  //! the index aIndex.
  //! The status will be one of the following:
  //! -   0 - there is no connection to a shape;
  //! -   1 - there is a connection to the first shape;
  //! -   2 - there is a connection to the second shape.
    void SetExtShape (const Standard_Integer aIndex);
  

  //! Returns the status index of the extension shape.
    Standard_Integer ExtShape() const;
  

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
  Standard_EXPORT virtual Standard_Boolean AcceptDisplayMode (const Standard_Integer aMode) const Standard_OVERRIDE;
  
    void SetAutomaticPosition (const Standard_Boolean aStatus);
  
    Standard_Boolean AutomaticPosition() const;




  DEFINE_STANDARD_RTTI(AIS_Relation,AIS_InteractiveObject)

protected:

  
  Standard_EXPORT AIS_Relation(const PrsMgr_TypeOfPresentation3d aTypeOfPresentation3d = PrsMgr_TOP_AllView);
  
  //! Calculates the presentation aPres of the the edge
  //! anEdge and the curve it defines, ProjCurve. The later
  //! is also specified by the first point FirstP and the last point LastP.
  //! The presentation includes settings for color aColor,
  //! type - aProjTOL and aCallTOL -   and width of line, aWidth.
  Standard_EXPORT void ComputeProjEdgePresentation (const Handle(Prs3d_Presentation)& aPres, const TopoDS_Edge& anEdge, const Handle(Geom_Curve)& ProjCurve, const gp_Pnt& FirstP, const gp_Pnt& LastP, const Quantity_NameOfColor aColor = Quantity_NOC_PURPLE, const Standard_Real aWidth = 2, const Aspect_TypeOfLine aProjTOL = Aspect_TOL_DASH, const Aspect_TypeOfLine aCallTOL = Aspect_TOL_DOT) const;
  
  //! Calculates the presentation aPres of the the vertex
  //! aVertex and the point it defines, ProjPoint.
  //! The presentation includes settings for color aColor,
  //! type - aProjTOM and aCallTOL -   and width of line, aWidth.
  Standard_EXPORT void ComputeProjVertexPresentation (const Handle(Prs3d_Presentation)& aPres, const TopoDS_Vertex& aVertex, const gp_Pnt& ProjPoint, const Quantity_NameOfColor aColor = Quantity_NOC_PURPLE, const Standard_Real aWidth = 2, const Aspect_TypeOfMarker aProjTOM = Aspect_TOM_PLUS, const Aspect_TypeOfLine aCallTOL = Aspect_TOL_DOT) const;

  TopoDS_Shape myFShape;
  TopoDS_Shape mySShape;
  Handle(Geom_Plane) myPlane;
  Standard_Real myVal;
  gp_Pnt myPosition;
  TCollection_ExtendedString myText;
  Standard_Real myArrowSize;
  Standard_Boolean myAutomaticPosition;
  DsgPrs_ArrowSide mySymbolPrs;
  Standard_Integer myExtShape;
  gp_Pln myFirstPlane;
  gp_Pln mySecondPlane;
  Handle(Geom_Surface) myFirstBasisSurf;
  Handle(Geom_Surface) mySecondBasisSurf;
  AIS_KindOfSurface myFirstSurfType;
  AIS_KindOfSurface mySecondSurfType;
  Standard_Real myFirstOffset;
  Standard_Real mySecondOffset;
  Bnd_Box myBndBox;
  Standard_Boolean myIsSetBndBox;
  Standard_Boolean myArrowSizeIsDefined;


private:




};


#include <AIS_Relation.lxx>





#endif // _AIS_Relation_HeaderFile
