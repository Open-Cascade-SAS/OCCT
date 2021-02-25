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

#ifndef _PrsDim_EqualRadiusRelation_HeaderFile
#define _PrsDim_EqualRadiusRelation_HeaderFile

#include <PrsDim_Relation.hxx>

class Geom_Plane;

DEFINE_STANDARD_HANDLE(PrsDim_EqualRadiusRelation, PrsDim_Relation)

class PrsDim_EqualRadiusRelation : public PrsDim_Relation
{
  DEFINE_STANDARD_RTTIEXT(PrsDim_EqualRadiusRelation, PrsDim_Relation)
public:

  //! Creates equal relation of two arc's radiuses.
  //! If one of edges is not in the given plane,
  //! the presentation method projects it onto the plane.
  Standard_EXPORT PrsDim_EqualRadiusRelation(const TopoDS_Edge& aFirstEdge, const TopoDS_Edge& aSecondEdge, const Handle(Geom_Plane)& aPlane);

private:

  Standard_EXPORT virtual void Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                        const Handle(Prs3d_Presentation)& thePrs,
                                        const Standard_Integer theMode) Standard_OVERRIDE;

  Standard_EXPORT virtual void ComputeSelection (const Handle(SelectMgr_Selection)& theSel,
                                                 const Standard_Integer theMode) Standard_OVERRIDE;

  Standard_EXPORT void ComputeRadiusPosition();

private:

  gp_Pnt myFirstCenter;
  gp_Pnt mySecondCenter;
  gp_Pnt myFirstPoint;
  gp_Pnt mySecondPoint;

};

#endif // _PrsDim_EqualRadiusRelation_HeaderFile
