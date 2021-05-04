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
#include <Standard_Boolean.hxx>
#include <TopTools_HSequenceOfShape.hxx>
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
  Standard_EXPORT void Perform (const TopoDS_Shape& shape);

  //! Returns (modifiable) the flag which defines whether to store faces
  //! with edges if its 3D curves has more than 8192 poles.
  Standard_Boolean& ModifyBigSplineMode() { return myBigSplineMode; }

  //! Returns (modifiable) the flag which defines whether to store faces on indirect surfaces.
  Standard_Boolean& ModifyIndirectMode() { return myIndirectMode; }

  //! Returns (modifiable) the flag which defines whether to store faces on offset surfaces.
  Standard_Boolean& ModifyOffsetSurfaceMode() { return myOffsetSurfaceMode; }

  //! Returns (modifiable) the flag which defines whether to store faces
  //! with edges if its 3D curves are trimmed curves
  Standard_Boolean& ModifyTrimmed3dMode() { return myTrimmed3dMode; }

  //! Returns (modifiable) the flag which defines whether to store faces
  //! with edges if its 3D curves and pcurves are offset curves
  Standard_Boolean& ModifyOffsetCurveMode() { return myOffsetCurveMode; }

  //! Returns (modifiable) the flag which defines whether to store faces
  //! with edges if its  pcurves are trimmed curves
  Standard_Boolean& ModifyTrimmed2dMode() { return myTrimmed2dMode; }

  Standard_Integer NbSolids() const { return myNbSolids; }

  Standard_Integer NbShells() const { return myNbShells; }

  Standard_Integer NbFaces() const { return myNbFaces; }

  Standard_Integer NbWires() const { return myNbWires; }

  Standard_Integer NbEdges() const { return myNbEdges; }

  Standard_Integer NbVertices() const { return myNbVertices; }

  Standard_Integer NbSolidsWithVoids() const { return myNbSolidsWithVoids; }

  Standard_Integer NbBigSplines() const { return myNbBigSplines; }

  Standard_Integer NbC0Surfaces() const { return myNbC0Surfaces; }

  Standard_Integer NbC0Curves() const { return myNbC0Curves; }

  Standard_Integer NbOffsetSurf() const { return myNbOffsetSurf; }

  Standard_Integer NbIndirectSurf() const { return myNbIndirectSurf; }

  Standard_Integer NbOffsetCurves() const { return myNbOffsetCurves; }

  Standard_Integer NbTrimmedCurve2d() const { return myNbTrimmedCurve2d; }

  Standard_Integer NbTrimmedCurve3d() const { return myNbTrimmedCurve3d; }

  Standard_Integer NbBSplibeSurf() const { return myNbBSplibeSurf; }

  Standard_Integer NbBezierSurf() const { return myNbBezierSurf; }

  Standard_Integer NbTrimSurf() const { return myNbTrimSurf; }

  Standard_Integer NbWireWitnSeam() const { return myNbWireWitnSeam; }

  Standard_Integer NbWireWithSevSeams() const { return myNbWireWithSevSeams; }

  Standard_Integer NbFaceWithSevWires() const { return myNbFaceWithSevWires; }

  Standard_Integer NbNoPCurve() const { return myNbNoPCurve; }

  Standard_Integer NbFreeFaces() const { return myNbFreeFaces; }

  Standard_Integer NbFreeWires() const { return myNbFreeWires; }

  Standard_Integer NbFreeEdges() const { return myNbFreeEdges; }

  Standard_Integer NbSharedSolids() const { return myNbSharedSolids; }

  Standard_Integer NbSharedShells() const { return myNbSharedShells; }

  Standard_Integer NbSharedFaces() const { return myNbSharedFaces; }

  Standard_Integer NbSharedWires() const { return myNbSharedWires; }

  Standard_Integer NbSharedFreeWires() const { return myNbSharedFreeWires; }

  Standard_Integer NbSharedFreeEdges() const { return myNbSharedFreeEdges; }

  Standard_Integer NbSharedEdges() const { return myNbSharedEdges; }

  Standard_Integer NbSharedVertices() const { return myNbSharedVertices; }

  const Handle(TopTools_HSequenceOfShape)& BigSplineSec() const { return myBigSplineSec; }

  const Handle(TopTools_HSequenceOfShape)& IndirectSec() const { return myIndirectSec; }

  const Handle(TopTools_HSequenceOfShape)& OffsetSurfaceSec() const { return myOffsetSurfaceSec; }

  const Handle(TopTools_HSequenceOfShape)& Trimmed3dSec() const { return myTrimmed3dSec; }

  const Handle(TopTools_HSequenceOfShape)& OffsetCurveSec() const { return myOffsetCurveSec; }

  const Handle(TopTools_HSequenceOfShape)& Trimmed2dSec() const { return myTrimmed2dSec; }
  
public:

  Standard_DEPRECATED("ModifyOffsetSurfaceMode() should be used instead")
  Standard_Boolean& ModifyOffestSurfaceMode() { return myOffsetSurfaceMode; }

private:

  Standard_Integer myNbSolids;
  Standard_Integer myNbShells;
  Standard_Integer myNbFaces;
  Standard_Integer myNbWires;
  Standard_Integer myNbEdges;
  Standard_Integer myNbVertices;
  Standard_Integer myNbSolidsWithVoids;
  Standard_Integer myNbBigSplines;
  Standard_Integer myNbC0Surfaces;
  Standard_Integer myNbC0Curves;
  Standard_Integer myNbOffsetSurf;
  Standard_Integer myNbIndirectSurf;
  Standard_Integer myNbOffsetCurves;
  Standard_Integer myNbTrimmedCurve2d;
  Standard_Integer myNbTrimmedCurve3d;
  Standard_Integer myNbBSplibeSurf;
  Standard_Integer myNbBezierSurf;
  Standard_Integer myNbTrimSurf;
  Standard_Integer myNbWireWitnSeam;
  Standard_Integer myNbWireWithSevSeams;
  Standard_Integer myNbFaceWithSevWires;
  Standard_Integer myNbNoPCurve;
  Standard_Integer myNbFreeFaces;
  Standard_Integer myNbFreeWires;
  Standard_Integer myNbFreeEdges;
  Standard_Integer myNbSharedSolids;
  Standard_Integer myNbSharedShells;
  Standard_Integer myNbSharedFaces;
  Standard_Integer myNbSharedWires;
  Standard_Integer myNbSharedFreeWires;
  Standard_Integer myNbSharedFreeEdges;
  Standard_Integer myNbSharedEdges;
  Standard_Integer myNbSharedVertices;
  Standard_Boolean myBigSplineMode;
  Standard_Boolean myIndirectMode;
  Standard_Boolean myOffsetSurfaceMode;
  Standard_Boolean myTrimmed3dMode;
  Standard_Boolean myOffsetCurveMode;
  Standard_Boolean myTrimmed2dMode;
  Handle(TopTools_HSequenceOfShape) myBigSplineSec;
  Handle(TopTools_HSequenceOfShape) myIndirectSec;
  Handle(TopTools_HSequenceOfShape) myOffsetSurfaceSec;
  Handle(TopTools_HSequenceOfShape) myTrimmed3dSec;
  Handle(TopTools_HSequenceOfShape) myOffsetCurveSec;
  Handle(TopTools_HSequenceOfShape) myTrimmed2dSec;

};

#endif // _ShapeAnalysis_ShapeContents_HeaderFile
