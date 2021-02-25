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

#ifndef _PrsDim_SymmetricRelation_HeaderFile
#define _PrsDim_SymmetricRelation_HeaderFile

#include <TopoDS_Shape.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <PrsDim_Relation.hxx>

DEFINE_STANDARD_HANDLE(PrsDim_SymmetricRelation, PrsDim_Relation)

//! A framework to display constraints of symmetricity
//! between two or more datum Interactive Objects.
//! A plane serves as the axis of symmetry between the
//! shapes of which the datums are parts.
class PrsDim_SymmetricRelation : public PrsDim_Relation
{
  DEFINE_STANDARD_RTTIEXT(PrsDim_SymmetricRelation, PrsDim_Relation)
public:

  //! Constructs an object to display constraints of symmetricity.
  //! This object is defined by a tool aSymmTool, a first
  //! shape FirstShape, a second shape SecondShape, and a plane aPlane.
  //! aPlane serves as the axis of symmetry.
  //! aSymmTool is the shape composed of FirstShape
  //! SecondShape and aPlane. It may be queried and
  //! edited using the functions GetTool and SetTool.
  //! The two shapes are typically two edges, two vertices or two points.
  Standard_EXPORT PrsDim_SymmetricRelation(const TopoDS_Shape& aSymmTool, const TopoDS_Shape& FirstShape, const TopoDS_Shape& SecondShape, const Handle(Geom_Plane)& aPlane);
  
  //! Returns true if the symmetric constraint display is movable.
  virtual Standard_Boolean IsMovable() const Standard_OVERRIDE { return Standard_True; }

  //! Sets the tool aSymmetricTool composed of a first
  //! shape, a second shape, and a plane.
  //! This tool is initially created at construction time.
  void SetTool (const TopoDS_Shape& aSymmetricTool) { myTool = aSymmetricTool; }

  //! Returns the tool composed of a first shape, a second
  //! shape, and a plane. This tool is created at construction time.
  const TopoDS_Shape& GetTool() const { return myTool; }

private:

  Standard_EXPORT virtual void Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                        const Handle(Prs3d_Presentation)& thePrs,
                                        const Standard_Integer theMode) Standard_OVERRIDE;

  Standard_EXPORT virtual void ComputeSelection (const Handle(SelectMgr_Selection)& theSel,
                                                 const Standard_Integer theMode) Standard_OVERRIDE;

  Standard_EXPORT void ComputeTwoFacesSymmetric (const Handle(Prs3d_Presentation)& aprs);

  Standard_EXPORT void ComputeTwoEdgesSymmetric (const Handle(Prs3d_Presentation)& aprs);

  Standard_EXPORT void ComputeTwoVerticesSymmetric (const Handle(Prs3d_Presentation)& aprs);

private:

  TopoDS_Shape myTool;
  gp_Pnt myFAttach;
  gp_Pnt mySAttach;
  gp_Dir myFDirAttach;
  gp_Dir myAxisDirAttach;

};

#endif // _PrsDim_SymmetricRelation_HeaderFile
