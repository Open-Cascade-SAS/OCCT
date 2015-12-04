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

#ifndef _AIS_TangentRelation_HeaderFile
#define _AIS_TangentRelation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <AIS_Relation.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <SelectMgr_Selection.hxx>
class TopoDS_Shape;
class Geom_Plane;
class Prs3d_Presentation;
class Prs3d_Projector;
class Geom_Transformation;


class AIS_TangentRelation;
DEFINE_STANDARD_HANDLE(AIS_TangentRelation, AIS_Relation)

//! A framework to display tangency constraints between
//! two or more Interactive Objects of the datum type.
//! The datums are normally faces or edges.
class AIS_TangentRelation : public AIS_Relation
{

public:

  
  //! TwoFacesTangent or TwoEdgesTangent relation
  //! Constructs an object to display tangency constraints.
  //! This object is defined by the first shape aFShape, the
  //! second shape aSShape, the plane aPlane and the index anExternRef.
  //! aPlane serves as an optional axis.
  //! anExternRef set to 0 indicates that there is no relation.
  Standard_EXPORT AIS_TangentRelation(const TopoDS_Shape& aFShape, const TopoDS_Shape& aSShape, const Handle(Geom_Plane)& aPlane, const Standard_Integer anExternRef = 0);
  
  //! Returns the external reference for tangency.
  //! The values are as follows:
  //! -   0 - there is no connection;
  //! -   1 - there is a connection to the first shape;
  //! -   2 - there is a connection to the second shape.
  //! This reference is defined at construction time.
  Standard_EXPORT Standard_Integer ExternRef();
  
  //! Sets the external reference for tangency, aRef.
  //! The values are as follows:
  //! -   0 - there is no connection;
  //! -   1 - there is a connection to the first shape;
  //! -   2 - there is a connection to the second shape.
  //! This reference is initially defined at construction time.
  Standard_EXPORT void SetExternRef (const Standard_Integer aRef);
  
  //! computes the presentation according to a point of view
  //! given by <aProjector>.
  //! To be Used when the associated degenerated Presentations
  //! have been transformed by <aTrsf> which is not a Pure
  //! Translation. The HLR Prs can't be deducted automatically
  //! WARNING :<aTrsf> must be applied
  //! to the object to display before computation  !!!
  Standard_EXPORT virtual void Compute (const Handle(Prs3d_Projector)& aProjector, const Handle(Geom_Transformation)& aTrsf, const Handle(Prs3d_Presentation)& aPresentation) Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(AIS_TangentRelation,AIS_Relation)

protected:




private:

  
  Standard_EXPORT void Compute (const Handle(PrsMgr_PresentationManager3d)& aPresentationManager, const Handle(Prs3d_Presentation)& aPresentation, const Standard_Integer aMode = 0) Standard_OVERRIDE;
  
  Standard_EXPORT void Compute (const Handle(Prs3d_Projector)& aProjector, const Handle(Prs3d_Presentation)& aPresentation) Standard_OVERRIDE;
  
  Standard_EXPORT void ComputeSelection (const Handle(SelectMgr_Selection)& aSelection, const Standard_Integer aMode) Standard_OVERRIDE;
  
  Standard_EXPORT void ComputeTwoFacesTangent (const Handle(Prs3d_Presentation)& aPresentation);
  
  Standard_EXPORT void ComputeTwoEdgesTangent (const Handle(Prs3d_Presentation)& aPresentation);

  gp_Pnt myAttach;
  gp_Dir myDir;
  Standard_Real myLength;
  Standard_Integer myExternRef;


};







#endif // _AIS_TangentRelation_HeaderFile
