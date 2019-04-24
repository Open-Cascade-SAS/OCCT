// Created on: 1998-01-17
// Created by: Julia GERASIMOVA
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _AIS_EqualRadiusRelation_HeaderFile
#define _AIS_EqualRadiusRelation_HeaderFile

#include <AIS_Relation.hxx>

class Geom_Plane;

DEFINE_STANDARD_HANDLE(AIS_EqualRadiusRelation, AIS_Relation)

class AIS_EqualRadiusRelation : public AIS_Relation
{
  DEFINE_STANDARD_RTTIEXT(AIS_EqualRadiusRelation, AIS_Relation)
public:

  //! Creates equal relation of two arc's radiuses.
  //! If one of edges is not in the given plane,
  //! the presentation method projects it onto the plane.
  Standard_EXPORT AIS_EqualRadiusRelation(const TopoDS_Edge& aFirstEdge, const TopoDS_Edge& aSecondEdge, const Handle(Geom_Plane)& aPlane);
  
  //! computes the presentation according to a point of view
  //! given by <aProjector>.
  //! To be Used when the associated degenerated Presentations
  //! have been transformed by <aTrsf> which is not a Pure
  //! Translation. The HLR Prs can't be deducted automatically
  //! WARNING :<aTrsf> must be applied
  //! to the object to display before computation  !!!
  Standard_EXPORT virtual void Compute (const Handle(Prs3d_Projector)& aProjector, const Handle(Geom_Transformation)& aTrsf, const Handle(Prs3d_Presentation)& aPresentation) Standard_OVERRIDE;

private:

  Standard_EXPORT void Compute (const Handle(PrsMgr_PresentationManager3d)& aPresentationManager, const Handle(Prs3d_Presentation)& aPresentation, const Standard_Integer aMode = 0) Standard_OVERRIDE;
  
  Standard_EXPORT void Compute (const Handle(Prs3d_Projector)& aProjector, const Handle(Prs3d_Presentation)& aPresentation) Standard_OVERRIDE;
  
  Standard_EXPORT void ComputeSelection (const Handle(SelectMgr_Selection)& aSelection, const Standard_Integer aMode) Standard_OVERRIDE;
  
  Standard_EXPORT void ComputeRadiusPosition();

  gp_Pnt myFirstCenter;
  gp_Pnt mySecondCenter;
  gp_Pnt myFirstPoint;
  gp_Pnt mySecondPoint;

};

#endif // _AIS_EqualRadiusRelation_HeaderFile
