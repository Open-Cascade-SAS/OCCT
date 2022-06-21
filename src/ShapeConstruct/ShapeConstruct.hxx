// Created on: 1998-07-14
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

#ifndef _ShapeConstruct_HeaderFile
#define _ShapeConstruct_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopAbs_Orientation.hxx>
class Geom_BSplineCurve;
class Geom_Curve;
class Geom2d_BSplineCurve;
class Geom2d_Curve;
class Geom_BSplineSurface;
class Geom_Surface;
class TopoDS_Face;
class TopoDS_Edge;


//! This package provides new algorithms for constructing
//! new geometrical objects and topological shapes. It
//! complements and extends algorithms available in Open
//! CASCADE topological and geometrical toolkist.
//! The functionality provided by this package are the
//! following:
//! projecting curves on surface,
//! adjusting curve to have given start and end points. P
class ShapeConstruct 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Tool for wire triangulation
  Standard_EXPORT static Handle(Geom_BSplineCurve) ConvertCurveToBSpline (const Handle(Geom_Curve)& C3D, const Standard_Real First, const Standard_Real Last, const Standard_Real Tol3d, const GeomAbs_Shape Continuity, const Standard_Integer MaxSegments, const Standard_Integer MaxDegree);
  
  Standard_EXPORT static Handle(Geom2d_BSplineCurve) ConvertCurveToBSpline (const Handle(Geom2d_Curve)& C2D, const Standard_Real First, const Standard_Real Last, const Standard_Real Tol2d, const GeomAbs_Shape Continuity, const Standard_Integer MaxSegments, const Standard_Integer MaxDegree);
  
  Standard_EXPORT static Handle(Geom_BSplineSurface) ConvertSurfaceToBSpline (const Handle(Geom_Surface)& surf, const Standard_Real UF, const Standard_Real UL, const Standard_Real VF, const Standard_Real VL, const Standard_Real Tol3d, const GeomAbs_Shape Continuity, const Standard_Integer MaxSegments, const Standard_Integer MaxDegree);
  
  //! join pcurves of the <theEdge> on the <theFace>
  //! try to use pcurves from originas edges <theEdges>
  //! Returns false if cannot join pcurves
  Standard_EXPORT static Standard_Boolean JoinPCurves (const Handle(TopTools_HSequenceOfShape)& theEdges, const TopoDS_Face& theFace, TopoDS_Edge& theEdge);
  
  //! Method for joininig curves 3D.
  //! Parameters : c3d1,ac3d2 - initial curves
  //! Orient1, Orient2 - initial edges orientations.
  //! first1,last1,first2,last2 - parameters for trimming curves
  //! (re-calculate with account of orientation edges)
  //! c3dOut - result curve
  //! isRev1,isRev2 - out parameters indicative on possible errors.
  //! Return value : True - if curves were joined successfully,
  //! else - False.
  Standard_EXPORT static Standard_Boolean JoinCurves (const Handle(Geom_Curve)& c3d1, const Handle(Geom_Curve)& ac3d2, const TopAbs_Orientation Orient1, const TopAbs_Orientation Orient2, Standard_Real& first1, Standard_Real& last1, Standard_Real& first2, Standard_Real& last2, Handle(Geom_Curve)& c3dOut, Standard_Boolean& isRev1, Standard_Boolean& isRev2);
  
  //! Method for joininig curves 3D.
  //! Parameters : c3d1,ac3d2 - initial curves
  //! Orient1, Orient2 - initial edges orientations.
  //! first1,last1,first2,last2 - parameters for trimming curves
  //! (re-calculate with account of orientation edges)
  //! c3dOut - result curve
  //! isRev1,isRev2 - out parameters indicative on possible errors.
  //! isError - input parameter indicative possible errors due to that one from edges have one vertex
  //! Return value : True - if curves were joined successfully,
  //! else - False.
  Standard_EXPORT static Standard_Boolean JoinCurves (const Handle(Geom2d_Curve)& c2d1, const Handle(Geom2d_Curve)& ac2d2, const TopAbs_Orientation Orient1, const TopAbs_Orientation Orient2, Standard_Real& first1, Standard_Real& last1, Standard_Real& first2, Standard_Real& last2, Handle(Geom2d_Curve)& c2dOut, Standard_Boolean& isRev1, Standard_Boolean& isRev2, const Standard_Boolean isError = Standard_False);

};

#endif // _ShapeConstruct_HeaderFile
