// Created on: 1999-02-25
// Created by: Pavel DURANDIN
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

#ifndef _ShapeAnalysis_ShapeContents_HeaderFile
#define _ShapeAnalysis_ShapeContents_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
class TopoDS_Shape;

//! Dumps shape contents
class ShapeAnalysis_ShapeContents
{
public:
  DEFINE_STANDARD_ALLOC

  //! Initialize fields and call ClearFlags()
  Standard_EXPORT ShapeAnalysis_ShapeContents();

  //! Clears all accumulated statistics
  Standard_EXPORT void Clear();

  //! Clears all flags
  Standard_EXPORT void ClearFlags();

  //! Counts quantities of sun-shapes in shape and
  //! stores sub-shapes according to flags
  Standard_EXPORT void Perform(const TopoDS_Shape& shape);

  //! Returns (modifiable) the flag which defines whether to store faces
  //! with edges if its 3D curves has more than 8192 poles.
  bool& ModifyBigSplineMode() { return myBigSplineMode; }

  //! Returns (modifiable) the flag which defines whether to store faces on indirect surfaces.
  bool& ModifyIndirectMode() { return myIndirectMode; }

  //! Returns (modifiable) the flag which defines whether to store faces on offset surfaces.
  bool& ModifyOffsetSurfaceMode() { return myOffsetSurfaceMode; }

  //! Returns (modifiable) the flag which defines whether to store faces
  //! with edges if its 3D curves are trimmed curves
  bool& ModifyTrimmed3dMode() { return myTrimmed3dMode; }

  //! Returns (modifiable) the flag which defines whether to store faces
  //! with edges if its 3D curves and pcurves are offset curves
  bool& ModifyOffsetCurveMode() { return myOffsetCurveMode; }

  //! Returns (modifiable) the flag which defines whether to store faces
  //! with edges if its pcurves are trimmed curves
  bool& ModifyTrimmed2dMode() { return myTrimmed2dMode; }

  int NbSolids() const { return myNbSolids; }

  int NbShells() const { return myNbShells; }

  int NbFaces() const { return myNbFaces; }

  int NbWires() const { return myNbWires; }

  int NbEdges() const { return myNbEdges; }

  int NbVertices() const { return myNbVertices; }

  int NbSolidsWithVoids() const { return myNbSolidsWithVoids; }

  int NbBigSplines() const { return myNbBigSplines; }

  int NbC0Surfaces() const { return myNbC0Surfaces; }

  int NbC0Curves() const { return myNbC0Curves; }

  int NbOffsetSurf() const { return myNbOffsetSurf; }

  int NbIndirectSurf() const { return myNbIndirectSurf; }

  int NbOffsetCurves() const { return myNbOffsetCurves; }

  int NbTrimmedCurve2d() const { return myNbTrimmedCurve2d; }

  int NbTrimmedCurve3d() const { return myNbTrimmedCurve3d; }

  int NbBSplibeSurf() const { return myNbBSplibeSurf; }

  int NbBezierSurf() const { return myNbBezierSurf; }

  int NbTrimSurf() const { return myNbTrimSurf; }

  int NbWireWitnSeam() const { return myNbWireWitnSeam; }

  int NbWireWithSevSeams() const { return myNbWireWithSevSeams; }

  int NbFaceWithSevWires() const { return myNbFaceWithSevWires; }

  int NbNoPCurve() const { return myNbNoPCurve; }

  int NbFreeFaces() const { return myNbFreeFaces; }

  int NbFreeWires() const { return myNbFreeWires; }

  int NbFreeEdges() const { return myNbFreeEdges; }

  int NbSharedSolids() const { return myNbSharedSolids; }

  int NbSharedShells() const { return myNbSharedShells; }

  int NbSharedFaces() const { return myNbSharedFaces; }

  int NbSharedWires() const { return myNbSharedWires; }

  int NbSharedFreeWires() const { return myNbSharedFreeWires; }

  int NbSharedFreeEdges() const { return myNbSharedFreeEdges; }

  int NbSharedEdges() const { return myNbSharedEdges; }

  int NbSharedVertices() const { return myNbSharedVertices; }

  const occ::handle<NCollection_HSequence<TopoDS_Shape>>& BigSplineSec() const
  {
    return myBigSplineSec;
  }

  const occ::handle<NCollection_HSequence<TopoDS_Shape>>& IndirectSec() const
  {
    return myIndirectSec;
  }

  const occ::handle<NCollection_HSequence<TopoDS_Shape>>& OffsetSurfaceSec() const
  {
    return myOffsetSurfaceSec;
  }

  const occ::handle<NCollection_HSequence<TopoDS_Shape>>& Trimmed3dSec() const
  {
    return myTrimmed3dSec;
  }

  const occ::handle<NCollection_HSequence<TopoDS_Shape>>& OffsetCurveSec() const
  {
    return myOffsetCurveSec;
  }

  const occ::handle<NCollection_HSequence<TopoDS_Shape>>& Trimmed2dSec() const
  {
    return myTrimmed2dSec;
  }

public:
  Standard_DEPRECATED("ModifyOffsetSurfaceMode() should be used instead")
  bool& ModifyOffestSurfaceMode() { return myOffsetSurfaceMode; }

private:
  int                                              myNbSolids;
  int                                              myNbShells;
  int                                              myNbFaces;
  int                                              myNbWires;
  int                                              myNbEdges;
  int                                              myNbVertices;
  int                                              myNbSolidsWithVoids;
  int                                              myNbBigSplines;
  int                                              myNbC0Surfaces;
  int                                              myNbC0Curves;
  int                                              myNbOffsetSurf;
  int                                              myNbIndirectSurf;
  int                                              myNbOffsetCurves;
  int                                              myNbTrimmedCurve2d;
  int                                              myNbTrimmedCurve3d;
  int                                              myNbBSplibeSurf;
  int                                              myNbBezierSurf;
  int                                              myNbTrimSurf;
  int                                              myNbWireWitnSeam;
  int                                              myNbWireWithSevSeams;
  int                                              myNbFaceWithSevWires;
  int                                              myNbNoPCurve;
  int                                              myNbFreeFaces;
  int                                              myNbFreeWires;
  int                                              myNbFreeEdges;
  int                                              myNbSharedSolids;
  int                                              myNbSharedShells;
  int                                              myNbSharedFaces;
  int                                              myNbSharedWires;
  int                                              myNbSharedFreeWires;
  int                                              myNbSharedFreeEdges;
  int                                              myNbSharedEdges;
  int                                              myNbSharedVertices;
  bool                                             myBigSplineMode;
  bool                                             myIndirectMode;
  bool                                             myOffsetSurfaceMode;
  bool                                             myTrimmed3dMode;
  bool                                             myOffsetCurveMode;
  bool                                             myTrimmed2dMode;
  occ::handle<NCollection_HSequence<TopoDS_Shape>> myBigSplineSec;
  occ::handle<NCollection_HSequence<TopoDS_Shape>> myIndirectSec;
  occ::handle<NCollection_HSequence<TopoDS_Shape>> myOffsetSurfaceSec;
  occ::handle<NCollection_HSequence<TopoDS_Shape>> myTrimmed3dSec;
  occ::handle<NCollection_HSequence<TopoDS_Shape>> myOffsetCurveSec;
  occ::handle<NCollection_HSequence<TopoDS_Shape>> myTrimmed2dSec;
};

#endif // _ShapeAnalysis_ShapeContents_HeaderFile
