// Created on: 1998-06-03
// Created by: data exchange team
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _ShapeAnalysis_HeaderFile
#define _ShapeAnalysis_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
class TopoDS_Wire;
class TopoDS_Face;
class ShapeExtend_WireData;
class TopoDS_Shape;
class TopoDS_Vertex;
class ShapeAnalysis_Geom;
class ShapeAnalysis_Curve;
class ShapeAnalysis_Surface;
class ShapeAnalysis_Edge;
class ShapeAnalysis_Wire;
class ShapeAnalysis_WireOrder;
class ShapeAnalysis_WireVertex;
class ShapeAnalysis_CheckSmallFace;
class ShapeAnalysis_Shell;
class ShapeAnalysis_ShapeTolerance;
class ShapeAnalysis_ShapeContents;
class ShapeAnalysis_FreeBounds;
class ShapeAnalysis_FreeBoundData;
class ShapeAnalysis_FreeBoundsProperties;
class ShapeAnalysis_TransferParameters;
class ShapeAnalysis_TransferParametersProj;


//! This package is intended to analyze geometrical objects
//! and topological shapes. Analysis domain includes both
//! exploring geometrical and topological properties of
//! shapes and checking their conformance to Open CASCADE requirements.
//! The directions of analysis provided by tools of this package are:
//! computing quantities of subshapes,
//! computing parameters of points on curve and surface,
//! computing surface singularities,
//! checking edge and wire consistency,
//! checking edges order in the wire,
//! checking face bounds orientation,
//! checking small faces,
//! analyzing shape tolerances,
//! analyzing of free bounds of the shape.
class ShapeAnalysis 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns the outer wire on the face <Face>.
  //! This is replacement of the method BRepTools::OuterWire
  //! until it works badly.
  //! Returns the first wire oriented as outer according to
  //! FClass2d_Classifier. If none, last wire is returned.
  Standard_EXPORT static TopoDS_Wire OuterWire (const TopoDS_Face& face);
  
  //! Returns a total area of 2d wire
  Standard_EXPORT static Standard_Real TotCross2D (const Handle(ShapeExtend_WireData)& sewd, const TopoDS_Face& aFace);
  
  //! Returns a total area of 3d wire
  Standard_EXPORT static Standard_Real ContourArea (const TopoDS_Wire& theWire);
  
  //! Returns True if <F> has outer bound.
  Standard_EXPORT static Standard_Boolean IsOuterBound (const TopoDS_Face& face);
  
  //! Returns a shift required to move point
  //! <Val> to the range [ToVal-Period/2,ToVal+Period/2].
  //! This shift will be the divisible by Period.
  //! Intended for adjusting parameters on periodic surfaces.
  Standard_EXPORT static Standard_Real AdjustByPeriod (const Standard_Real Val, const Standard_Real ToVal, const Standard_Real Period);
  
  //! Returns a shift required to move point
  //! <Val> to the range [ValMin,ValMax].
  //! This shift will be the divisible by Period
  //! with Period = ValMax - ValMin.
  //! Intended for adjusting parameters on periodic surfaces.
  Standard_EXPORT static Standard_Real AdjustToPeriod (const Standard_Real Val, const Standard_Real ValMin, const Standard_Real ValMax);
  
  //! Finds the start and end vertices of the shape
  //! Shape can be of the following type:
  //! vertex: V1 and V2 are the same and equal to <shape>,
  //! edge  : V1 is start and V2 is end vertex (see ShapeAnalysis_Edge
  //! methods FirstVertex and LastVertex),
  //! wire  : V1 is start vertex of the first edge, V2 is end vertex
  //! of the last edge (also see ShapeAnalysis_Edge).
  //! If wire contains no edges V1 and V2 are nullified
  //! If none of the above V1 and V2 are nullified
  Standard_EXPORT static void FindBounds (const TopoDS_Shape& shape, TopoDS_Vertex& V1, TopoDS_Vertex& V2);
  
  //! Computes exact UV bounds of all wires on the face
  Standard_EXPORT static void GetFaceUVBounds (const TopoDS_Face& F, Standard_Real& Umin, Standard_Real& Umax, Standard_Real& Vmin, Standard_Real& Vmax);




protected:





private:




friend class ShapeAnalysis_Geom;
friend class ShapeAnalysis_Curve;
friend class ShapeAnalysis_Surface;
friend class ShapeAnalysis_Edge;
friend class ShapeAnalysis_Wire;
friend class ShapeAnalysis_WireOrder;
friend class ShapeAnalysis_WireVertex;
friend class ShapeAnalysis_CheckSmallFace;
friend class ShapeAnalysis_Shell;
friend class ShapeAnalysis_ShapeTolerance;
friend class ShapeAnalysis_ShapeContents;
friend class ShapeAnalysis_FreeBounds;
friend class ShapeAnalysis_FreeBoundData;
friend class ShapeAnalysis_FreeBoundsProperties;
friend class ShapeAnalysis_TransferParameters;
friend class ShapeAnalysis_TransferParametersProj;

};







#endif // _ShapeAnalysis_HeaderFile
