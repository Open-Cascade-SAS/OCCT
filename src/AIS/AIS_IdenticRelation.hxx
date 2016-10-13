// Created on: 1997-03-03
// Created by: Jean-Pierre COMBE
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

#ifndef _AIS_IdenticRelation_HeaderFile
#define _AIS_IdenticRelation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <gp_Pnt.hxx>
#include <AIS_Relation.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <Standard_Integer.hxx>
#include <SelectMgr_Selection.hxx>
#include <Standard_Real.hxx>
class TopoDS_Shape;
class Geom_Plane;
class Prs3d_Presentation;
class Prs3d_Projector;
class Geom_Transformation;
class Geom_Line;
class gp_Pnt;
class Geom_Circle;
class Geom_Ellipse;
class TopoDS_Wire;
class TopoDS_Vertex;
class gp_Dir;


class AIS_IdenticRelation;
DEFINE_STANDARD_HANDLE(AIS_IdenticRelation, AIS_Relation)

//! Constructs a constraint by a relation of identity
//! between two or more datums figuring in shape
//! Interactive Objects.
class AIS_IdenticRelation : public AIS_Relation
{

public:

  //! Initializes the relation of identity between the two
  //! entities, FirstShape and SecondShape. The plane
  //! aPlane is initialized in case a visual reference is
  //! needed to show identity.
  Standard_EXPORT AIS_IdenticRelation(const TopoDS_Shape& FirstShape, const TopoDS_Shape& SecondShape, const Handle(Geom_Plane)& aPlane);

  Standard_Boolean HasUsers() const { return !myUsers.IsEmpty(); }

  const TColStd_ListOfTransient& Users() const { return myUsers; }

  void AddUser (const Handle(Standard_Transient)& theUser) { myUsers.Append (theUser); }

  void ClearUsers() { myUsers.Clear(); }

  //! Returns true if the interactive object is movable.
    virtual Standard_Boolean IsMovable() const Standard_OVERRIDE;
  
  //! computes the presentation according to a point of view
  //! given by <aProjector>.
  //! To be Used when the associated degenerated Presentations
  //! have been transformed by <aTrsf> which is not a Pure
  //! Translation. The HLR Prs can't be deducted automatically
  //! WARNING :<aTrsf> must be applied
  //! to the object to display before computation  !!!
  Standard_EXPORT virtual void Compute (const Handle(Prs3d_Projector)& aProjector, const Handle(Geom_Transformation)& aTrsf, const Handle(Prs3d_Presentation)& aPresentation) Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(AIS_IdenticRelation,AIS_Relation)

private:

  
  Standard_EXPORT virtual void Compute (const Handle(PrsMgr_PresentationManager3d)& aPresentationManager, const Handle(Prs3d_Presentation)& aPresentation, const Standard_Integer aMode = 0) Standard_OVERRIDE;
  
  Standard_EXPORT void Compute (const Handle(Prs3d_Projector)& aProjector, const Handle(Prs3d_Presentation)& aPresentation) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void ComputeSelection (const Handle(SelectMgr_Selection)& aSelection, const Standard_Integer aMode) Standard_OVERRIDE;
  
  Standard_EXPORT void ComputeOneEdgeOVertexPresentation (const Handle(Prs3d_Presentation)& aPresentation);
  
  Standard_EXPORT void ComputeTwoEdgesPresentation (const Handle(Prs3d_Presentation)& aPresentation);
  
  Standard_EXPORT void ComputeTwoLinesPresentation (const Handle(Prs3d_Presentation)& aPresentation, const Handle(Geom_Line)& aLin, gp_Pnt& Pnt1On1, gp_Pnt& Pnt2On1, gp_Pnt& Pnt1On2, gp_Pnt& Pnt2On2, const Standard_Boolean isInf1, const Standard_Boolean isInf2);
  
  Standard_EXPORT void ComputeTwoCirclesPresentation (const Handle(Prs3d_Presentation)& aPresentation, const Handle(Geom_Circle)& aCircle, const gp_Pnt& Pnt1On1, const gp_Pnt& Pnt2On1, const gp_Pnt& Pnt1On2, const gp_Pnt& Pnt2On2);
  
  //! Computes the presentation of the identic constraint
  //! between 2 arcs in the case of automatic presentation
  Standard_EXPORT void ComputeAutoArcPresentation (const Handle(Geom_Circle)& aCircle, const gp_Pnt& firstp, const gp_Pnt& lastp, const Standard_Boolean isstatic = Standard_False);
  
  //! Computes the presentation of the identic constraint
  //! between 2 circles in the case of non automatic presentation
  Standard_EXPORT void ComputeNotAutoCircPresentation (const Handle(Geom_Circle)& aCircle);
  
  //! Computes the presentation of the identic constraint
  //! between 2 arcs in the case of non automatic presentation
  Standard_EXPORT void ComputeNotAutoArcPresentation (const Handle(Geom_Circle)& aCircle, const gp_Pnt& pntfirst, const gp_Pnt& pntlast);
  
  Standard_EXPORT void ComputeTwoEllipsesPresentation (const Handle(Prs3d_Presentation)& aPrs, const Handle(Geom_Ellipse)& anEll, const gp_Pnt& Pnt1On1, const gp_Pnt& Pnt2On1, const gp_Pnt& Pnt1On2, const gp_Pnt& Pnt2On2);
  
  //! Computes the presentation of the identic constraint
  //! between 2 arcs in the case of automatic presentation
  Standard_EXPORT void ComputeAutoArcPresentation (const Handle(Geom_Ellipse)& theEll, const gp_Pnt& firstp, const gp_Pnt& lastp, const Standard_Boolean isstatic = Standard_False);
  
  //! Computes the presentation of the identic constraint
  //! between 2 ellipses in the case of non automatic presentation
  Standard_EXPORT void ComputeNotAutoElipsPresentation (const Handle(Geom_Ellipse)& theEll);
  
  //! Computes the presentation of the identic constraint
  //! between 2 arcs in the case of non automatic presentation
  Standard_EXPORT void ComputeNotAutoArcPresentation (const Handle(Geom_Ellipse)& theEll, const gp_Pnt& pntfirst, const gp_Pnt& pntlast);
  
  Standard_EXPORT void ComputeTwoVerticesPresentation (const Handle(Prs3d_Presentation)& aPresentation);
  
  Standard_EXPORT Standard_Real ComputeSegSize() const;
  
  Standard_EXPORT Standard_Boolean ComputeDirection (const TopoDS_Wire& aWire, const TopoDS_Vertex& aVertex, gp_Dir& aDir) const;
  
  Standard_EXPORT gp_Dir ComputeLineDirection (const Handle(Geom_Line)& aLin, const gp_Pnt& anExtremity) const;
  
  Standard_EXPORT gp_Dir ComputeCircleDirection (const Handle(Geom_Circle)& aCirc, const TopoDS_Vertex& ConnectedVertex) const;

private:

  TColStd_ListOfTransient myUsers;
  Standard_Boolean isCircle;
  gp_Pnt myFAttach;
  gp_Pnt mySAttach;
  gp_Pnt myCenter;

};


#include <AIS_IdenticRelation.lxx>





#endif // _AIS_IdenticRelation_HeaderFile
