// Created on: 1996-12-05
// Created by: Jean-Pierre COMBE/Odile Olivier
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

#ifndef _PrsDim_PerpendicularRelation_HeaderFile
#define _PrsDim_PerpendicularRelation_HeaderFile

#include <PrsDim_Relation.hxx>

//! A framework to display constraints of perpendicularity
//! between two or more interactive datums. These
//! datums can be edges or faces.
class PrsDim_PerpendicularRelation : public PrsDim_Relation
{
  DEFINE_STANDARD_RTTIEXT(PrsDim_PerpendicularRelation, PrsDim_Relation)
public:
  //! Constructs an object to display constraints of
  //! perpendicularity on shapes.
  //! This object is defined by a first shape aFShape, a
  //! second shape aSShape, and a plane aPlane.
  //! aPlane is the plane of reference to show and test the
  //! perpendicular relation between two shapes, at least
  //! one of which has a revolved surface.
  Standard_EXPORT PrsDim_PerpendicularRelation(const TopoDS_Shape&            aFShape,
                                               const TopoDS_Shape&            aSShape,
                                               const occ::handle<Geom_Plane>& aPlane);

  //! Constructs an object to display constraints of
  //! perpendicularity on shapes.
  //! This object is defined by a first shape aFShape and a
  //! second shape aSShape.
  Standard_EXPORT PrsDim_PerpendicularRelation(const TopoDS_Shape& aFShape,
                                               const TopoDS_Shape& aSShape);

private:
  Standard_EXPORT void Compute(const occ::handle<PrsMgr_PresentationManager>& thePrsMgr,
                                       const occ::handle<Prs3d_Presentation>&         thePrs,
                                       const int theMode) override;

  Standard_EXPORT void ComputeSelection(const occ::handle<SelectMgr_Selection>& theSel,
                                                const int theMode) override;

  Standard_EXPORT void ComputeTwoFacesPerpendicular(
    const occ::handle<Prs3d_Presentation>& aPresentation);

  Standard_EXPORT void ComputeTwoEdgesPerpendicular(
    const occ::handle<Prs3d_Presentation>& aPresentation);

private:
  gp_Pnt myFAttach;
  gp_Pnt mySAttach;
};

#endif // _PrsDim_PerpendicularRelation_HeaderFile
