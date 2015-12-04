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

#ifndef _AIS_SymmetricRelation_HeaderFile
#define _AIS_SymmetricRelation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Shape.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <AIS_Relation.hxx>
#include <Standard_Boolean.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <Standard_Integer.hxx>
#include <SelectMgr_Selection.hxx>
class TopoDS_Shape;
class Geom_Plane;
class Prs3d_Presentation;
class Prs3d_Projector;
class Geom_Transformation;


class AIS_SymmetricRelation;
DEFINE_STANDARD_HANDLE(AIS_SymmetricRelation, AIS_Relation)

//! A framework to display constraints of symmetricity
//! between two or more datum Interactive Objects.
//! A plane serves as the axis of symmetry between the
//! shapes of which the datums are parts.
class AIS_SymmetricRelation : public AIS_Relation
{

public:

  
  //! Constructs an object to display constraints of symmetricity.
  //! This object is defined by a tool aSymmTool, a first
  //! shape FirstShape, a second shape SecondShape, and a plane aPlane.
  //! aPlane serves as the axis of symmetry.
  //! aSymmTool is the shape composed of FirstShape
  //! SecondShape and aPlane. It may be queried and
  //! edited using the functions GetTool and SetTool.
  //! The two shapes are typically two edges, two vertices or two points.
  Standard_EXPORT AIS_SymmetricRelation(const TopoDS_Shape& aSymmTool, const TopoDS_Shape& FirstShape, const TopoDS_Shape& SecondShape, const Handle(Geom_Plane)& aPlane);
  
  //! Returns true if the symmetric constraint display is movable.
    virtual Standard_Boolean IsMovable() const Standard_OVERRIDE;
  
  //! Sets the tool aSymmetricTool composed of a first
  //! shape, a second shape, and a plane.
  //! This tool is initially created at construction time.
    void SetTool (const TopoDS_Shape& aSymmetricTool);
  
  //! Returns the tool composed of a first shape, a second
  //! shape, and a plane. This tool is created at construction time.
    const TopoDS_Shape& GetTool() const;
  
  //! computes the presentation according to a point of view
  //! given by <aProjector>.
  //! To be Used when the associated degenerated Presentations
  //! have been transformed by <aTrsf> which is not a Pure
  //! Translation. The HLR Prs can't be deducted automatically
  //! WARNING :<aTrsf> must be applied
  //! to the object to display before computation  !!!
  Standard_EXPORT virtual void Compute (const Handle(Prs3d_Projector)& aProjector, const Handle(Geom_Transformation)& aTrsf, const Handle(Prs3d_Presentation)& aPresentation) Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(AIS_SymmetricRelation,AIS_Relation)

protected:




private:

  
  Standard_EXPORT void Compute (const Handle(PrsMgr_PresentationManager3d)& aPresentationManager, const Handle(Prs3d_Presentation)& aPresentation, const Standard_Integer aMode = 0) Standard_OVERRIDE;
  
  Standard_EXPORT void Compute (const Handle(Prs3d_Projector)& aProjector, const Handle(Prs3d_Presentation)& aPresentation) Standard_OVERRIDE;
  
  Standard_EXPORT void ComputeSelection (const Handle(SelectMgr_Selection)& aSelection, const Standard_Integer aMode) Standard_OVERRIDE;
  
  Standard_EXPORT void ComputeTwoFacesSymmetric (const Handle(Prs3d_Presentation)& aprs);
  
  Standard_EXPORT void ComputeTwoEdgesSymmetric (const Handle(Prs3d_Presentation)& aprs);
  
  Standard_EXPORT void ComputeTwoVerticesSymmetric (const Handle(Prs3d_Presentation)& aprs);

  TopoDS_Shape myTool;
  gp_Pnt myFAttach;
  gp_Pnt mySAttach;
  gp_Dir myFDirAttach;
  gp_Dir myAxisDirAttach;


};


#include <AIS_SymmetricRelation.lxx>





#endif // _AIS_SymmetricRelation_HeaderFile
