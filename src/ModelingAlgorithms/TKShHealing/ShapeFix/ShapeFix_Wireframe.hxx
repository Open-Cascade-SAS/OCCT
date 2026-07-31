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
#include <Standard_Integer.hxx>
#include <ShapeFix_Root.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_List.hxx>
#include <NCollection_DataMap.hxx>
#include <ShapeExtend_Status.hxx>

//! Provides methods for fixing wireframe of shape
class ShapeFix_Wireframe : public ShapeFix_Root
{

public:
  Standard_EXPORT ShapeFix_Wireframe();

  Standard_EXPORT ShapeFix_Wireframe(const TopoDS_Shape& shape);

  //! Clears all statuses
  Standard_EXPORT virtual void ClearStatuses();

  //! Loads a shape, resets statuses
  Standard_EXPORT void Load(const TopoDS_Shape& shape);

  //! Fixes gaps between ends of curves of adjacent edges
  //! (both 3d and pcurves) in wires
  //! If precision is 0.0, uses Precision::Confusion().
  Standard_EXPORT bool FixWireGaps();

  //! Fixes small edges in shape by merging adjacent edges
  //! If precision is 0.0, uses Precision::Confusion().
  Standard_EXPORT bool FixSmallEdges();

  //! Auxiliary tool for FixSmallEdges which checks for small edges and fills the maps.
  //! Returns True if at least one small edge has been found.
  Standard_EXPORT bool CheckSmallEdges(
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theSmallEdges,
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
      theEdgeToFaces,
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                                            theFaceWithSmall,
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMultyEdges);

  //! Auxiliary tool for FixSmallEdges which merges small edges.
  //! If theModeDrop is equal to true then small edges,
  //! which cannot be connected with adjacent edges are dropped.
  //! Otherwise they are kept.
  //! theLimitAngle specifies maximum allowed tangency
  //! discontinuity between adjacent edges.
  //! If theLimitAngle is equal to -1, this angle is not taken into account.
  Standard_EXPORT bool MergeSmallEdges(
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theSmallEdges,
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
      theEdgeToFaces,
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                                            theFaceWithSmall,
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMultyEdges,
    const bool                                              theModeDrop   = false,
    const double                                            theLimitAngle = -1);

  //! Decodes the status of the last FixWireGaps.
  //! OK - No gaps were found
  //! DONE1 - Some gaps in 3D were fixed
  //! DONE2 - Some gaps in 2D were fixed
  //! FAIL1 - Failed to fix some gaps in 3D
  //! FAIL2 - Failed to fix some gaps in 2D
  bool StatusWireGaps(const ShapeExtend_Status status) const;

  //! Decodes the status of the last FixSmallEdges.
  //! OK - No small edges were found
  //! DONE1 - Some small edges were fixed
  //! FAIL1 - Failed to fix some small edges
  bool StatusSmallEdges(const ShapeExtend_Status status) const;

  TopoDS_Shape Shape();

  //! Returns mode managing removing small edges.
  bool& ModeDropSmallEdges();

  //! Set limit angle for merging edges.
  void SetLimitAngle(const double theLimitAngle);

  //! Get limit angle for merging edges.
  double LimitAngle() const;

  DEFINE_STANDARD_RTTIEXT(ShapeFix_Wireframe, ShapeFix_Root)

protected:
  TopoDS_Shape myShape;

private:
  bool   myModeDrop;
  double myLimitAngle;
  int    myStatusWireGaps;
  int    myStatusSmallEdges;
};

#include <ShapeFix_Wireframe.lxx>

#endif // _ShapeFix_Wireframe_HeaderFile
