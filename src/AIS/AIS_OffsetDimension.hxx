// Created on: 1997-03-04
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

#ifndef _AIS_OffsetDimension_HeaderFile
#define _AIS_OffsetDimension_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Trsf.hxx>
#include <AIS_Relation.hxx>
#include <Standard_Real.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <Standard_Integer.hxx>
#include <SelectMgr_Selection.hxx>
#include <AIS_KindOfDimension.hxx>
#include <Standard_Boolean.hxx>
class TopoDS_Shape;
class TCollection_ExtendedString;
class Prs3d_Presentation;
class Prs3d_Projector;
class Geom_Transformation;
class gp_Trsf;


class AIS_OffsetDimension;
DEFINE_STANDARD_HANDLE(AIS_OffsetDimension, AIS_Relation)

//! A framework to display dimensions of offsets.
//! The relation between the offset and the basis shape
//! is indicated. This relation is displayed with arrows and
//! text. The text gives the dsitance between the offset
//! and the basis shape.
class AIS_OffsetDimension : public AIS_Relation
{

public:

  
  //! Constructs the offset display object defined by the
  //! first shape aFShape, the second shape aSShape, the
  //! dimension aVal, and the text aText.
  Standard_EXPORT AIS_OffsetDimension(const TopoDS_Shape& FistShape, const TopoDS_Shape& SecondShape, const Standard_Real aVal, const TCollection_ExtendedString& aText);
  
  //! computes the presentation according to a point of view
  //! given by <aProjector>.
  //! To be Used when the associated degenerated Presentations
  //! have been transformed by <aTrsf> which is not a Pure
  //! Translation. The HLR Prs can't be deducted automatically
  //! WARNING :<aTrsf> must be applied
  //! to the object to display before computation  !!!
  Standard_EXPORT virtual void Compute (const Handle(Prs3d_Projector)& aProjector, const Handle(Geom_Transformation)& aTrsf, const Handle(Prs3d_Presentation)& aPresentation) Standard_OVERRIDE;
  

  //! Indicates that the dimension we are concerned with is an offset.
    virtual AIS_KindOfDimension KindOfDimension() const Standard_OVERRIDE;
  
  //! Returns true if the offset datum is movable.
    virtual Standard_Boolean IsMovable() const Standard_OVERRIDE;
  
  //! Sets a transformation aTrsf for presentation and
  //! selection to a relative position.
    void SetRelativePos (const gp_Trsf& aTrsf);




  DEFINE_STANDARD_RTTIEXT(AIS_OffsetDimension,AIS_Relation)

protected:




private:

  
  Standard_EXPORT virtual void Compute (const Handle(PrsMgr_PresentationManager3d)& aPresentationManager, const Handle(Prs3d_Presentation)& aPresentation, const Standard_Integer aMode = 0) Standard_OVERRIDE;
  
  Standard_EXPORT void Compute (const Handle(Prs3d_Projector)& aProjector, const Handle(Prs3d_Presentation)& aPresentation) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void ComputeSelection (const Handle(SelectMgr_Selection)& aSelection, const Standard_Integer aMode) Standard_OVERRIDE;
  
  Standard_EXPORT void ComputeTwoFacesOffset (const Handle(Prs3d_Presentation)& aPresentation, const gp_Trsf& aTrsf);
  
  Standard_EXPORT void ComputeTwoAxesOffset (const Handle(Prs3d_Presentation)& aPresentation, const gp_Trsf& aTrsf);
  
  Standard_EXPORT void ComputeAxeFaceOffset (const Handle(Prs3d_Presentation)& aPresentation, const gp_Trsf& aTrsf);

  gp_Pnt myFAttach;
  gp_Pnt mySAttach;
  gp_Dir myDirAttach;
  gp_Dir myDirAttach2;
  gp_Trsf myRelativePos;


};


#include <AIS_OffsetDimension.lxx>





#endif // _AIS_OffsetDimension_HeaderFile
