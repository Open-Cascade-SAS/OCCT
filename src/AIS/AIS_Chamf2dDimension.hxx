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

#ifndef _AIS_Chamf2dDimension_HeaderFile
#define _AIS_Chamf2dDimension_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <AIS_Relation.hxx>
#include <Standard_Real.hxx>
#include <DsgPrs_ArrowSide.hxx>
#include <AIS_KindOfDimension.hxx>
#include <Standard_Boolean.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <Standard_Integer.hxx>
#include <SelectMgr_Selection.hxx>
class TopoDS_Shape;
class Geom_Plane;
class TCollection_ExtendedString;
class gp_Pnt;
class Prs3d_Presentation;
class Prs3d_Projector;
class Geom_Transformation;


class AIS_Chamf2dDimension;
DEFINE_STANDARD_HANDLE(AIS_Chamf2dDimension, AIS_Relation)

//! A framework to define display of 2D chamfers.
//! A chamfer is displayed with arrows and text. The text
//! gives the length of the chamfer if it is a symmetrical
//! chamfer, or the angle if it is not.
class AIS_Chamf2dDimension : public AIS_Relation
{

public:

  
  //! Constructs the display object for 2D chamfers.
  //! This object is defined by the face aFShape, the
  //! dimension aVal, the plane aPlane and the text aText.
  Standard_EXPORT AIS_Chamf2dDimension(const TopoDS_Shape& aFShape, const Handle(Geom_Plane)& aPlane, const Standard_Real aVal, const TCollection_ExtendedString& aText);
  
  //! Constructs the display object for 2D chamfers.
  //! This object is defined by the face aFShape, the plane
  //! aPlane, the dimension aVal, the position aPosition,
  //! the type of arrow aSymbolPrs with the size
  //! anArrowSize, and the text aText.
  Standard_EXPORT AIS_Chamf2dDimension(const TopoDS_Shape& aFShape, const Handle(Geom_Plane)& aPlane, const Standard_Real aVal, const TCollection_ExtendedString& aText, const gp_Pnt& aPosition, const DsgPrs_ArrowSide aSymbolPrs, const Standard_Real anArrowSize = 0.0);
  
  //! Indicates that we are concerned with a 2d length.
    virtual AIS_KindOfDimension KindOfDimension() const Standard_OVERRIDE;
  
  //! Returns true if the 2d chamfer dimension is movable.
    virtual Standard_Boolean IsMovable() const Standard_OVERRIDE;
  
  //! computes the presentation according to a point of view
  //! given by <aProjector>.
  //! To be Used when the associated degenerated Presentations
  //! have been transformed by <aTrsf> which is not a Pure
  //! Translation. The HLR Prs can't be deducted automatically
  //! WARNING :<aTrsf> must be applied
  //! to the object to display before computation  !!!
  Standard_EXPORT virtual void Compute (const Handle(Prs3d_Projector)& aProjector, const Handle(Geom_Transformation)& aTrsf, const Handle(Prs3d_Presentation)& aPresentation) Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(AIS_Chamf2dDimension,AIS_Relation)

protected:




private:

  
  Standard_EXPORT void Compute (const Handle(PrsMgr_PresentationManager3d)& aPresentationManager, const Handle(Prs3d_Presentation)& aPresentation, const Standard_Integer aMode = 0) Standard_OVERRIDE;
  
  Standard_EXPORT void Compute (const Handle(Prs3d_Projector)& aProjector, const Handle(Prs3d_Presentation)& aPresentation) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void ComputeSelection (const Handle(SelectMgr_Selection)& aSelection, const Standard_Integer aMode) Standard_OVERRIDE;

  gp_Pnt myPntAttach;
  gp_Dir myDir;


};


#include <AIS_Chamf2dDimension.lxx>





#endif // _AIS_Chamf2dDimension_HeaderFile
