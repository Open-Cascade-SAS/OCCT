// Created on: 1999-08-24
// Created by: Sergei ZERTCHANINOV
// Copyright (c) 1999 Matra Datavision
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

#ifndef _ShapeFix_Wireframe_HeaderFile
#define _ShapeFix_Wireframe_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Shape.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <ShapeFix_Root.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <ShapeExtend_Status.hxx>
class TopoDS_Shape;


class ShapeFix_Wireframe;
DEFINE_STANDARD_HANDLE(ShapeFix_Wireframe, ShapeFix_Root)

//! Provides methods for fixing wireframe of shape
class ShapeFix_Wireframe : public ShapeFix_Root
{

public:

  
  Standard_EXPORT ShapeFix_Wireframe();
  
  Standard_EXPORT ShapeFix_Wireframe(const TopoDS_Shape& shape);
  
  //! Clears all statuses
  Standard_EXPORT virtual void ClearStatuses();
  
  //! Loads a shape, resets statuses
  Standard_EXPORT void Load (const TopoDS_Shape& shape);
  
  //! Fixes gaps between ends of curves of adjacent edges
  //! (both 3d and pcurves) in wires
  //! If precision is 0.0, uses Precision::Confusion().
  Standard_EXPORT Standard_Boolean FixWireGaps();
  
  //! Fixes small edges in shape by merging adjacent edges
  //! If precision is 0.0, uses Precision::Confusion().
  Standard_EXPORT Standard_Boolean FixSmallEdges();
  
  //! Auxiliary tool for FixSmallEdges which checks for small edges and fills the maps.
  //! Returns True if at least one small edge has been found.
  Standard_EXPORT Standard_Boolean CheckSmallEdges (TopTools_MapOfShape& theSmallEdges, TopTools_DataMapOfShapeListOfShape& theEdgeToFaces, TopTools_DataMapOfShapeListOfShape& theFaceWithSmall, TopTools_MapOfShape& theMultyEdges);
  
  //! Auxiliary tool for FixSmallEdges which merges small edges.
  //! If theModeDrop is equal to Standard_True then small edges,
  //! which cannot be connected with adjacent edges are dropped.
  //! Otherwise they are kept.
  //! theLimitAngle specifies maximum allowed tangency
  //! discontinuity between adjacent edges.
  //! If theLimitAngle is equal to -1, this angle is not taken into account.
  Standard_EXPORT Standard_Boolean MergeSmallEdges (TopTools_MapOfShape& theSmallEdges, TopTools_DataMapOfShapeListOfShape& theEdgeToFaces, TopTools_DataMapOfShapeListOfShape& theFaceWithSmall, TopTools_MapOfShape& theMultyEdges, const Standard_Boolean theModeDrop = Standard_False, const Standard_Real theLimitAngle = -1);
  
  //! Decodes the status of the last FixWireGaps.
  //! OK - No gaps were found
  //! DONE1 - Some gaps in 3D were fixed
  //! DONE2 - Some gaps in 2D were fixed
  //! FAIL1 - Failed to fix some gaps in 3D
  //! FAIL2 - Failed to fix some gaps in 2D
    Standard_Boolean StatusWireGaps (const ShapeExtend_Status status) const;
  
  //! Decodes the status of the last FixSmallEdges.
  //! OK - No small edges were found
  //! DONE1 - Some small edges were fixed
  //! FAIL1 - Failed to fix some small edges
    Standard_Boolean StatusSmallEdges (const ShapeExtend_Status status) const;
  
    TopoDS_Shape Shape();
  
  //! Returns mode managing removing small edges.
    Standard_Boolean& ModeDropSmallEdges();
  
  //! Set limit angle for merging edges.
    void SetLimitAngle (const Standard_Real theLimitAngle);
  
  //! Get limit angle for merging edges.
    Standard_Real LimitAngle() const;




  DEFINE_STANDARD_RTTIEXT(ShapeFix_Wireframe,ShapeFix_Root)

protected:


  TopoDS_Shape myShape;


private:


  Standard_Boolean myModeDrop;
  Standard_Real myLimitAngle;
  Standard_Integer myStatusWireGaps;
  Standard_Integer myStatusSmallEdges;


};


#include <ShapeFix_Wireframe.lxx>





#endif // _ShapeFix_Wireframe_HeaderFile
