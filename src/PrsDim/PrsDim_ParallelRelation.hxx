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

#ifndef _PrsDim_ParallelRelation_HeaderFile
#define _PrsDim_ParallelRelation_HeaderFile

#include <PrsDim_Relation.hxx>
#include <DsgPrs_ArrowSide.hxx>

DEFINE_STANDARD_HANDLE(PrsDim_ParallelRelation, PrsDim_Relation)

//! A framework to display constraints of parallelism
//! between two or more Interactive Objects. These
//! entities can be faces or edges.
class PrsDim_ParallelRelation : public PrsDim_Relation
{
  DEFINE_STANDARD_RTTIEXT(PrsDim_ParallelRelation, PrsDim_Relation)
public:
  //! Constructs an object to display parallel constraints.
  //! This object is defined by the first shape aFShape and
  //! the second shape aSShape and the plane aPlane.
  Standard_EXPORT PrsDim_ParallelRelation(const TopoDS_Shape&       aFShape,
                                          const TopoDS_Shape&       aSShape,
                                          const Handle(Geom_Plane)& aPlane);

  //! Constructs an object to display parallel constraints.
  //! This object is defined by the first shape aFShape and
  //! the second shape aSShape the plane aPlane, the
  //! position aPosition, the type of arrow, aSymbolPrs and
  //! its size anArrowSize.
  Standard_EXPORT PrsDim_ParallelRelation(const TopoDS_Shape&       aFShape,
                                          const TopoDS_Shape&       aSShape,
                                          const Handle(Geom_Plane)& aPlane,
                                          const gp_Pnt&             aPosition,
                                          const DsgPrs_ArrowSide    aSymbolPrs,
                                          const Standard_Real       anArrowSize = 0.01);

  //! Returns true if the parallelism is movable.
  virtual Standard_Boolean IsMovable() const Standard_OVERRIDE { return Standard_True; }

private:
  Standard_EXPORT virtual void Compute(const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                       const Handle(Prs3d_Presentation)&         thePrs,
                                       const Standard_Integer theMode) Standard_OVERRIDE;

  Standard_EXPORT virtual void ComputeSelection(const Handle(SelectMgr_Selection)& theSel,
                                                const Standard_Integer theMode) Standard_OVERRIDE;

  Standard_EXPORT void ComputeTwoFacesParallel(const Handle(Prs3d_Presentation)& aPresentation);

  Standard_EXPORT void ComputeTwoEdgesParallel(const Handle(Prs3d_Presentation)& aPresentation);

private:
  gp_Pnt myFAttach;
  gp_Pnt mySAttach;
  gp_Dir myDirAttach;
};

#endif // _PrsDim_ParallelRelation_HeaderFile
