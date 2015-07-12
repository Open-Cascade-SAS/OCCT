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
  
  //! Clears all accumulated statictics
  Standard_EXPORT void Clear();
  
  //! Clears all flags
  Standard_EXPORT void ClearFlags();
  
  //! Counts quantities of sun-shapes in shape and
  //! stores sub-shapes according to flags
  Standard_EXPORT void Perform (const TopoDS_Shape& shape);
  
  //! Returns (modifiable) the flag which defines whether to store faces
  //! with edges if its 3D curves has more than 8192 poles.
    Standard_Boolean& ModifyBigSplineMode();
  
  //! Returns (modifiable) the flag which defines whether to store faces
  //! on indirect surfaces
    Standard_Boolean& ModifyIndirectMode();
  
  //! Returns (modifiable) the flag which defines whether to store faces
  //! on offset surfaces.
    Standard_Boolean& ModifyOffestSurfaceMode();
  
  //! Returns (modifiable) the flag which defines whether to store faces
  //! with edges if ist 3D curves are trimmed curves
    Standard_Boolean& ModifyTrimmed3dMode();
  
  //! Returns (modifiable) the flag which defines whether to store faces
  //! with edges if its 3D curves and pcurves are offest curves
    Standard_Boolean& ModifyOffsetCurveMode();
  
  //! Returns (modifiable) the flag which defines whether to store faces
  //! with edges if its  pcurves are trimmed curves
    Standard_Boolean& ModifyTrimmed2dMode();
  
    Standard_Integer NbSolids() const;
  
    Standard_Integer NbShells() const;
  
    Standard_Integer NbFaces() const;
  
    Standard_Integer NbWires() const;
  
    Standard_Integer NbEdges() const;
  
    Standard_Integer NbVertices() const;
  
    Standard_Integer NbSolidsWithVoids() const;
  
    Standard_Integer NbBigSplines() const;
  
    Standard_Integer NbC0Surfaces() const;
  
    Standard_Integer NbC0Curves() const;
  
    Standard_Integer NbOffsetSurf() const;
  
    Standard_Integer NbIndirectSurf() const;
  
    Standard_Integer NbOffsetCurves() const;
  
    Standard_Integer NbTrimmedCurve2d() const;
  
    Standard_Integer NbTrimmedCurve3d() const;
  
    Standard_Integer NbBSplibeSurf() const;
  
    Standard_Integer NbBezierSurf() const;
  
    Standard_Integer NbTrimSurf() const;
  
    Standard_Integer NbWireWitnSeam() const;
  
    Standard_Integer NbWireWithSevSeams() const;
  
    Standard_Integer NbFaceWithSevWires() const;
  
    Standard_Integer NbNoPCurve() const;
  
    Standard_Integer NbFreeFaces() const;
  
    Standard_Integer NbFreeWires() const;
  
    Standard_Integer NbFreeEdges() const;
  
    Standard_Integer NbSharedSolids() const;
  
    Standard_Integer NbSharedShells() const;
  
    Standard_Integer NbSharedFaces() const;
  
    Standard_Integer NbSharedWires() const;
  
    Standard_Integer NbSharedFreeWires() const;
  
    Standard_Integer NbSharedFreeEdges() const;
  
    Standard_Integer NbSharedEdges() const;
  
    Standard_Integer NbSharedVertices() const;
  
    Handle(TopTools_HSequenceOfShape) BigSplineSec() const;
  
    Handle(TopTools_HSequenceOfShape) IndirectSec() const;
  
    Handle(TopTools_HSequenceOfShape) OffsetSurfaceSec() const;
  
    Handle(TopTools_HSequenceOfShape) Trimmed3dSec() const;
  
    Handle(TopTools_HSequenceOfShape) OffsetCurveSec() const;
  
    Handle(TopTools_HSequenceOfShape) Trimmed2dSec() const;




protected:





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
  Standard_Boolean myOffestSurfaceMode;
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


#include <ShapeAnalysis_ShapeContents.lxx>





#endif // _ShapeAnalysis_ShapeContents_HeaderFile
