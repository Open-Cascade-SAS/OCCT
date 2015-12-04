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

#ifndef _AIS_ConcentricRelation_HeaderFile
#define _AIS_ConcentricRelation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Pnt.hxx>
#include <Standard_Real.hxx>
#include <gp_Dir.hxx>
#include <AIS_Relation.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <Standard_Integer.hxx>
#include <SelectMgr_Selection.hxx>
class TopoDS_Shape;
class Geom_Plane;
class Prs3d_Presentation;
class Prs3d_Projector;
class Geom_Transformation;


class AIS_ConcentricRelation;
DEFINE_STANDARD_HANDLE(AIS_ConcentricRelation, AIS_Relation)

//! A framework to define a constraint by a relation of
//! concentricity between two or more interactive datums.
//! The display of this constraint is also defined.
//! A plane is used to create an axis along which the
//! relation of concentricity can be extended.
class AIS_ConcentricRelation : public AIS_Relation
{

public:

  
  //! Constructs the display object for concentric relations
  //! between shapes.
  //! This object is defined by the two shapes, aFShape
  //! and aSShape and the plane aPlane.
  //! aPlane is provided to create an axis along which the
  //! relation of concentricity can be extended.
  Standard_EXPORT AIS_ConcentricRelation(const TopoDS_Shape& aFShape, const TopoDS_Shape& aSShape, const Handle(Geom_Plane)& aPlane);
  
  //! computes the presentation according to a point of view
  //! given by <aProjector>.
  //! To be Used when the associated degenerated Presentations
  //! have been transformed by <aTrsf> which is not a Pure
  //! Translation. The HLR Prs can't be deducted automatically
  //! WARNING :<aTrsf> must be applied
  //! to the object to display before computation  !!!
  Standard_EXPORT virtual void Compute (const Handle(Prs3d_Projector)& aProjector, const Handle(Geom_Transformation)& aTrsf, const Handle(Prs3d_Presentation)& aPresentation) Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(AIS_ConcentricRelation,AIS_Relation)

protected:




private:

  
  Standard_EXPORT void Compute (const Handle(PrsMgr_PresentationManager3d)& aPresentationManager, const Handle(Prs3d_Presentation)& aPresentation, const Standard_Integer aMode = 0) Standard_OVERRIDE;
  
  Standard_EXPORT void Compute (const Handle(Prs3d_Projector)& aProjector, const Handle(Prs3d_Presentation)& aPresentation) Standard_OVERRIDE;
  
  Standard_EXPORT void ComputeTwoEdgesConcentric (const Handle(Prs3d_Presentation)& aPresentationManager);
  
  Standard_EXPORT void ComputeEdgeVertexConcentric (const Handle(Prs3d_Presentation)& aPresentationManager);
  
  Standard_EXPORT void ComputeTwoVerticesConcentric (const Handle(Prs3d_Presentation)& aPresentationManager);
  
  Standard_EXPORT virtual void ComputeSelection (const Handle(SelectMgr_Selection)& aSelection, const Standard_Integer aMode) Standard_OVERRIDE;

  gp_Pnt myCenter;
  Standard_Real myRad;
  gp_Dir myDir;
  gp_Pnt myPnt;


};







#endif // _AIS_ConcentricRelation_HeaderFile
