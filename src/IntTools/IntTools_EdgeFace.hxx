// Created on: 2001-02-26
// Created by: Peter KURNEV
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef _IntTools_EdgeFace_HeaderFile
#define _IntTools_EdgeFace_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <Standard_Boolean.hxx>
#include <IntTools_SequenceOfRanges.hxx>
#include <IntTools_FClass2d.hxx>
#include <IntTools_CArray1OfReal.hxx>
#include <IntTools_SequenceOfRoots.hxx>
#include <IntTools_SequenceOfCommonPrts.hxx>
#include <IntTools_Range.hxx>
class IntTools_Context;
class TopoDS_Edge;
class TopoDS_Face;
class IntTools_Range;
class gp_Pnt;
class BRepAdaptor_Surface;
class IntTools_CArray1OfReal;
class IntTools_CommonPrt;


//! The  class  provides  Edge/Face  algorithm  to  determine
//! common  parts  between edge and face in  3-d space.
//! Common  parts can be :  Vertices  or Edges.
class IntTools_EdgeFace 
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! Empty Constructor
  Standard_EXPORT IntTools_EdgeFace();
  

  //! Initializes algorithm by the edge anEdge
  Standard_EXPORT void SetEdge (const TopoDS_Edge& anEdge);
  

  //! Initializes algorithm by edge tolerance
  Standard_EXPORT void SetTolE (const Standard_Real aTolEdge1);
  

  //! Initializes algorithm by the face aFace
  Standard_EXPORT void SetFace (const TopoDS_Face& aFace);
  

  //! Initializes algorithm by face tolerance
  Standard_EXPORT void SetTolF (const Standard_Real aTolFace);
  

  //! Returns edge
  Standard_EXPORT const TopoDS_Edge& Edge() const;
  

  //! Returns face
  Standard_EXPORT const TopoDS_Face& Face() const;
  

  //! Returns  tolerance of the edge
  Standard_EXPORT Standard_Real TolE() const;
  

  //! Returns  tolerance of the face
  Standard_EXPORT Standard_Real TolF() const;
  

  //! Initializes algorithm by discretization value
  Standard_EXPORT void SetDiscretize (const Standard_Integer aDiscret);
  

  //! Initializes algorithm by deflection value
  Standard_EXPORT void SetDeflection (const Standard_Real aDeflection);
  

  //! Initializes algorithm by parameter tolerance
  Standard_EXPORT void SetEpsilonT (const Standard_Real anEpsT);
  

  //! Initializes algorithm by distance tolerance
  Standard_EXPORT void SetEpsilonNull (const Standard_Real anEpsNull);
  

  //! Sets boundaries for edge.
  //! The algorithm processes edge inside these boundaries.
  Standard_EXPORT void SetRange (const IntTools_Range& aRange);
  

  //! Sets boundaries for edge.
  //! The algorithm processes edge inside these boundaries.
  Standard_EXPORT void SetRange (const Standard_Real aFirst, const Standard_Real aLast);
  

  //! Sets the intersecton context
  Standard_EXPORT void SetContext (const Handle(IntTools_Context)& theContext);
  

  //! Gets the intersecton context
  Standard_EXPORT const Handle(IntTools_Context)& Context() const;
  

  //! Launches the process
  Standard_EXPORT void Perform();
  

  //! Returns true if computation was done
  //! successfully, otherwise returns false
  Standard_EXPORT Standard_Boolean IsDone() const;
  

  //! Returns code of completion
  //! 0 - means successful completion
  //! 1 - the process was not started
  //! 2,3,4,5 - invalid source data for the algorithm
  //! 6 - discretization failed
  //! 7 - no projectable ranges found
  //! 11 - distance computing error
  Standard_EXPORT Standard_Integer ErrorStatus() const;
  

  //! Returns results
  Standard_EXPORT const IntTools_SequenceOfCommonPrts& CommonParts() const;
  

  //! Returns boundaries for edge
  Standard_EXPORT const IntTools_Range& Range() const;
  
  Standard_EXPORT static Standard_Boolean IsEqDistance (const gp_Pnt& aP, const BRepAdaptor_Surface& aS, const Standard_Real aT, Standard_Real& aD);




protected:

  
  Standard_EXPORT void CheckData();
  
  Standard_EXPORT void Prepare();
  
  Standard_EXPORT Standard_Boolean IsProjectable (const Standard_Real t) const;
  
  Standard_EXPORT void FindProjectableRoot (const Standard_Real t1, const Standard_Real t2, const Standard_Integer f1, const Standard_Integer f2, Standard_Real& tRoot);
  
  Standard_EXPORT Standard_Real DistanceFunction (const Standard_Real t);
  
  Standard_EXPORT Standard_Real DerivativeFunction (const Standard_Real t);
  
  Standard_EXPORT void PrepareArgsFuncArrays (const Standard_Real t1, const Standard_Real t2);
  
  Standard_EXPORT void AddDerivativePoints (const IntTools_CArray1OfReal& t, const IntTools_CArray1OfReal& f);
  
  Standard_EXPORT Standard_Real FindSimpleRoot (const Standard_Integer IP, const Standard_Real ta, const Standard_Real tb, const Standard_Real fA);
  
  Standard_EXPORT Standard_Real FindGoldRoot (const Standard_Real ta, const Standard_Real tb, const Standard_Real coeff);
  
  Standard_EXPORT Standard_Integer MakeType (IntTools_CommonPrt& aCP);
  
  Standard_EXPORT void IsIntersection (const Standard_Real ta, const Standard_Real tb);
  
  Standard_EXPORT void FindDerivativeRoot (const IntTools_CArray1OfReal& t, const IntTools_CArray1OfReal& f);
  
  Standard_EXPORT void RemoveIdenticalRoots();
  
  Standard_EXPORT Standard_Boolean CheckTouch (const IntTools_CommonPrt& aCP, Standard_Real& aTX);
  
  Standard_EXPORT Standard_Boolean CheckTouchVertex (const IntTools_CommonPrt& aCP, Standard_Real& aTX);




private:



  TopoDS_Edge myEdge;
  TopoDS_Face myFace;
  Standard_Real myTolE;
  Standard_Real myTolF;
  Standard_Integer myDiscret;
  Standard_Real myEpsT;
  Standard_Real myEpsNull;
  Standard_Real myDeflection;
  BRepAdaptor_Curve myC;
  Standard_Real myTmin;
  Standard_Real myTmax;
  BRepAdaptor_Surface myS;
  Standard_Real myCriteria;
  Standard_Boolean myIsDone;
  Standard_Integer myErrorStatus;
  Handle(IntTools_Context) myContext;
  IntTools_SequenceOfRanges myProjectableRanges;
  IntTools_FClass2d myFClass2d;
  IntTools_CArray1OfReal myFuncArray;
  IntTools_CArray1OfReal myArgsArray;
  IntTools_SequenceOfRoots mySequenceOfRoots;
  IntTools_SequenceOfCommonPrts mySeqOfCommonPrts;
  Standard_Real myPar1;
  Standard_Boolean myParallel;
  IntTools_Range myRange;


};







#endif // _IntTools_EdgeFace_HeaderFile
