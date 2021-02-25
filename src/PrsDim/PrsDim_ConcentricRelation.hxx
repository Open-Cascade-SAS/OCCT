// Created on: 1996-12-05
// Created by: Flore Lantheaume/Odile Olivier
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

#ifndef _PrsDim_ConcentricRelation_HeaderFile
#define _PrsDim_ConcentricRelation_HeaderFile

#include <PrsDim_Relation.hxx>
#include <gp_Dir.hxx>

class Geom_Plane;

DEFINE_STANDARD_HANDLE(PrsDim_ConcentricRelation, PrsDim_Relation)

//! A framework to define a constraint by a relation of
//! concentricity between two or more interactive datums.
//! The display of this constraint is also defined.
//! A plane is used to create an axis along which the
//! relation of concentricity can be extended.
class PrsDim_ConcentricRelation : public PrsDim_Relation
{
  DEFINE_STANDARD_RTTIEXT(PrsDim_ConcentricRelation, PrsDim_Relation)
public:

  //! Constructs the display object for concentric relations
  //! between shapes.
  //! This object is defined by the two shapes, aFShape
  //! and aSShape and the plane aPlane.
  //! aPlane is provided to create an axis along which the
  //! relation of concentricity can be extended.
  Standard_EXPORT PrsDim_ConcentricRelation(const TopoDS_Shape& aFShape, const TopoDS_Shape& aSShape, const Handle(Geom_Plane)& aPlane);

private:

  Standard_EXPORT virtual void Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                        const Handle(Prs3d_Presentation)& thePrs,
                                        const Standard_Integer theMode) Standard_OVERRIDE;

  Standard_EXPORT void ComputeTwoEdgesConcentric (const Handle(Prs3d_Presentation)& thePrsMgr);

  Standard_EXPORT void ComputeEdgeVertexConcentric (const Handle(Prs3d_Presentation)& thePrsMgr);

  Standard_EXPORT void ComputeTwoVerticesConcentric (const Handle(Prs3d_Presentation)& thePrsMgr);

  Standard_EXPORT virtual void ComputeSelection (const Handle(SelectMgr_Selection)& theSel,
                                                 const Standard_Integer theMode) Standard_OVERRIDE;

private:

  gp_Pnt myCenter;
  Standard_Real myRad;
  gp_Dir myDir;
  gp_Pnt myPnt;

};

#endif // _PrsDim_ConcentricRelation_HeaderFile
