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

#ifndef _AIS_ParallelRelation_HeaderFile
#define _AIS_ParallelRelation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <AIS_Relation.hxx>
#include <DsgPrs_ArrowSide.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <Standard_Integer.hxx>
#include <SelectMgr_Selection.hxx>
class TopoDS_Shape;
class Geom_Plane;
class gp_Pnt;
class Prs3d_Presentation;
class Prs3d_Projector;
class Geom_Transformation;


class AIS_ParallelRelation;
DEFINE_STANDARD_HANDLE(AIS_ParallelRelation, AIS_Relation)

//! A framework to display constraints of parallelism
//! between two or more Interactive Objects. These
//! entities can be faces or edges.
class AIS_ParallelRelation : public AIS_Relation
{

public:

  
  //! Constructs an object to display parallel constraints.
  //! This object is defined by the first shape aFShape and
  //! the second shape aSShape and the plane aPlane.
  Standard_EXPORT AIS_ParallelRelation(const TopoDS_Shape& aFShape, const TopoDS_Shape& aSShape, const Handle(Geom_Plane)& aPlane);
  
  //! Constructs an object to display parallel constraints.
  //! This object is defined by the first shape aFShape and
  //! the second shape aSShape the plane aPlane, the
  //! position aPosition, the type of arrow, aSymbolPrs and
  //! its size anArrowSize.
  Standard_EXPORT AIS_ParallelRelation(const TopoDS_Shape& aFShape, const TopoDS_Shape& aSShape, const Handle(Geom_Plane)& aPlane, const gp_Pnt& aPosition, const DsgPrs_ArrowSide aSymbolPrs, const Standard_Real anArrowSize = 0.01);
  
  //! Returns true if the parallelism is movable.
    virtual Standard_Boolean IsMovable() const Standard_OVERRIDE;
  
  //! computes the presentation according to a point of view
  //! given by <aProjector>.
  //! To be Used when the associated degenerated Presentations
  //! have been transformed by <aTrsf> which is not a Pure
  //! Translation. The HLR Prs can't be deducted automatically
  //! WARNING :<aTrsf> must be applied
  //! to the object to display before computation  !!!
  Standard_EXPORT virtual void Compute (const Handle(Prs3d_Projector)& aProjector, const Handle(Geom_Transformation)& aTrsf, const Handle(Prs3d_Presentation)& aPresentation) Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(AIS_ParallelRelation,AIS_Relation)

protected:




private:

  
  Standard_EXPORT void Compute (const Handle(PrsMgr_PresentationManager3d)& aPresentationManager, const Handle(Prs3d_Presentation)& aPresentation, const Standard_Integer aMode = 0) Standard_OVERRIDE;
  
  Standard_EXPORT void Compute (const Handle(Prs3d_Projector)& aProjector, const Handle(Prs3d_Presentation)& aPresentation) Standard_OVERRIDE;
  
  Standard_EXPORT void ComputeSelection (const Handle(SelectMgr_Selection)& aSelection, const Standard_Integer aMode) Standard_OVERRIDE;
  
  Standard_EXPORT void ComputeTwoFacesParallel (const Handle(Prs3d_Presentation)& aPresentation);
  
  Standard_EXPORT void ComputeTwoEdgesParallel (const Handle(Prs3d_Presentation)& aPresentation);

  gp_Pnt myFAttach;
  gp_Pnt mySAttach;
  gp_Dir myDirAttach;


};


#include <AIS_ParallelRelation.lxx>





#endif // _AIS_ParallelRelation_HeaderFile
