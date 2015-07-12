// Created on: 2000-05-18
// Created by: Peter KURNEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _IntTools_HeaderFile
#define _IntTools_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <IntTools_SequenceOfRoots.hxx>
#include <Standard_Integer.hxx>
class TopoDS_Edge;
class gp_Pnt;
class Geom_Curve;
class BRepAdaptor_Curve;
class IntTools_CArray1OfReal;
class IntTools_Context;
class IntTools_ShrunkRange;
class IntTools_Range;
class IntTools_CommonPrt;
class IntTools_Root;
class IntTools_EdgeEdge;
class IntTools_EdgeFace;
class IntTools_FClass2d;
class IntTools_MarkedRangeSet;
class IntTools_BaseRangeSample;
class IntTools_CurveRangeSample;
class IntTools_SurfaceRangeSample;
class IntTools_CurveRangeLocalizeData;
class IntTools_SurfaceRangeLocalizeData;
class IntTools_BeanFaceIntersector;
class IntTools_Curve;
class IntTools_PntOnFace;
class IntTools_PntOn2Faces;
class IntTools_TopolTool;
class IntTools_FaceFace;
class IntTools_Tools;
class IntTools_CArray1;
class IntTools_CArray1OfInteger;
class IntTools_CArray1OfReal;
class IntTools_CurveRangeSampleMapHasher;
class IntTools_SurfaceRangeSampleMapHasher;


//! Contains classes for intersection and classification
//! purposes and accompanying classes
class IntTools 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! returns the length of the edge;
  Standard_EXPORT static Standard_Real Length (const TopoDS_Edge& E);
  
  //! Remove from  the  sequence aSeq the Roots  that  have
  //! values ti and tj such as  |ti-tj]  <  anEpsT.
  Standard_EXPORT static void RemoveIdenticalRoots (IntTools_SequenceOfRoots& aSeq, const Standard_Real anEpsT);
  
  //! Sort the sequence aSeq of the Roots to arrange the
  //! Roons  in  increasing  order
  Standard_EXPORT static void SortRoots (IntTools_SequenceOfRoots& aSeq, const Standard_Real anEpsT);
  
  //! Find the states (before  and  after) for  each  Root
  //! from  the sequence aSeq
  Standard_EXPORT static void FindRootStates (IntTools_SequenceOfRoots& aSeq, const Standard_Real anEpsNull);
  
  Standard_EXPORT static Standard_Integer Parameter (const gp_Pnt& P, const Handle(Geom_Curve)& Curve, Standard_Real& aParm);
  
  Standard_EXPORT static Standard_Integer GetRadius (const BRepAdaptor_Curve& C, const Standard_Real t1, const Standard_Real t3, Standard_Real& R);
  
  Standard_EXPORT static Standard_Integer PrepareArgs (BRepAdaptor_Curve& C, const Standard_Real tMax, const Standard_Real tMin, const Standard_Integer Discret, const Standard_Real Deflect, IntTools_CArray1OfReal& anArgs);




protected:





private:




friend class IntTools_Context;
friend class IntTools_ShrunkRange;
friend class IntTools_Range;
friend class IntTools_CommonPrt;
friend class IntTools_Root;
friend class IntTools_EdgeEdge;
friend class IntTools_EdgeFace;
friend class IntTools_FClass2d;
friend class IntTools_MarkedRangeSet;
friend class IntTools_BaseRangeSample;
friend class IntTools_CurveRangeSample;
friend class IntTools_SurfaceRangeSample;
friend class IntTools_CurveRangeLocalizeData;
friend class IntTools_SurfaceRangeLocalizeData;
friend class IntTools_BeanFaceIntersector;
friend class IntTools_Curve;
friend class IntTools_PntOnFace;
friend class IntTools_PntOn2Faces;
friend class IntTools_TopolTool;
friend class IntTools_FaceFace;
friend class IntTools_Tools;
friend class IntTools_CArray1;
friend class IntTools_CArray1OfInteger;
friend class IntTools_CArray1OfReal;
friend class IntTools_CurveRangeSampleMapHasher;
friend class IntTools_SurfaceRangeSampleMapHasher;

};







#endif // _IntTools_HeaderFile
