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

#ifndef _PrsDim_TangentRelation_HeaderFile
#define _PrsDim_TangentRelation_HeaderFile

#include <PrsDim_Relation.hxx>

DEFINE_STANDARD_HANDLE(PrsDim_TangentRelation, PrsDim_Relation)

//! A framework to display tangency constraints between
//! two or more Interactive Objects of the datum type.
//! The datums are normally faces or edges.
class PrsDim_TangentRelation : public PrsDim_Relation
{
  DEFINE_STANDARD_RTTIEXT(PrsDim_TangentRelation, PrsDim_Relation)
public:

  //! TwoFacesTangent or TwoEdgesTangent relation
  //! Constructs an object to display tangency constraints.
  //! This object is defined by the first shape aFShape, the
  //! second shape aSShape, the plane aPlane and the index anExternRef.
  //! aPlane serves as an optional axis.
  //! anExternRef set to 0 indicates that there is no relation.
  Standard_EXPORT PrsDim_TangentRelation(const TopoDS_Shape& aFShape, const TopoDS_Shape& aSShape, const Handle(Geom_Plane)& aPlane, const Standard_Integer anExternRef = 0);
  
  //! Returns the external reference for tangency.
  //! The values are as follows:
  //! -   0 - there is no connection;
  //! -   1 - there is a connection to the first shape;
  //! -   2 - there is a connection to the second shape.
  //! This reference is defined at construction time.
  Standard_Integer ExternRef() { return myExternRef; }

  //! Sets the external reference for tangency, aRef.
  //! The values are as follows:
  //! -   0 - there is no connection;
  //! -   1 - there is a connection to the first shape;
  //! -   2 - there is a connection to the second shape.
  //! This reference is initially defined at construction time.
  void SetExternRef (const Standard_Integer aRef) { myExternRef = aRef; }

private:

  Standard_EXPORT virtual void Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                        const Handle(Prs3d_Presentation)& thePrs,
                                        const Standard_Integer theMode) Standard_OVERRIDE;

  Standard_EXPORT virtual void ComputeSelection (const Handle(SelectMgr_Selection)& theSel,
                                                 const Standard_Integer theMode) Standard_OVERRIDE;

  Standard_EXPORT void ComputeTwoFacesTangent (const Handle(Prs3d_Presentation)& aPresentation);

  Standard_EXPORT void ComputeTwoEdgesTangent (const Handle(Prs3d_Presentation)& aPresentation);

private:

  gp_Pnt myAttach;
  gp_Dir myDir;
  Standard_Real myLength;
  Standard_Integer myExternRef;

};

#endif // _PrsDim_TangentRelation_HeaderFile
