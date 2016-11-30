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
#include <IntTools_SequenceOfCommonPrts.hxx>
#include <IntTools_Range.hxx>
class IntTools_Context;
class TopoDS_Edge;
class TopoDS_Face;
class IntTools_Range;
class gp_Pnt;
class BRepAdaptor_Surface;
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
  

  //! Initializes algorithm by the face aFace
  Standard_EXPORT void SetFace (const TopoDS_Face& aFace);
  

  //! Returns edge
  Standard_EXPORT const TopoDS_Edge& Edge() const;
  

  //! Returns face
  Standard_EXPORT const TopoDS_Face& Face() const;
  

  //! Initializes algorithm by discretization value
  Standard_EXPORT void SetDiscretize (const Standard_Integer aDiscret);
  

  //! Initializes algorithm by deflection value
  Standard_EXPORT void SetDeflection (const Standard_Real aDeflection);
  

  //! Initializes algorithm by parameter tolerance
  Standard_EXPORT void SetEpsilonT (const Standard_Real anEpsT);
  
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
  
  //! Sets the Fuzzy value
  Standard_EXPORT void SetFuzzyValue(const Standard_Real theFuzz);

  //! Returns Fuzzy value
  Standard_Real FuzzyValue() const
  {
    return myFuzzyValue;
  }

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

  //! Sets the flag myQuickCoincidenceCheck
  void UseQuickCoincidenceCheck(const Standard_Boolean bFlag) {
    myQuickCoincidenceCheck=bFlag;
  }

  //! Returns the flag myQuickCoincidenceCheck
  Standard_Boolean IsCoincidenceCheckedQuickly () {
    return myQuickCoincidenceCheck;
  }


protected:
  Standard_EXPORT static Standard_Boolean IsEqDistance (const gp_Pnt& aP, const BRepAdaptor_Surface& aS, const Standard_Real aT, Standard_Real& aD);
  Standard_EXPORT void CheckData();
  
  Standard_EXPORT Standard_Boolean IsProjectable (const Standard_Real t) const;
  
  Standard_EXPORT Standard_Real DistanceFunction (const Standard_Real t);
  
  Standard_EXPORT Standard_Integer MakeType (IntTools_CommonPrt& aCP);
   
  Standard_EXPORT Standard_Boolean CheckTouch (const IntTools_CommonPrt& aCP, Standard_Real& aTX);
  
  Standard_EXPORT Standard_Boolean CheckTouchVertex (const IntTools_CommonPrt& aCP, Standard_Real& aTX);

  //! Checks if the edge is in the face really.
  Standard_EXPORT Standard_Boolean IsCoincident();



private:



  TopoDS_Edge myEdge;
  TopoDS_Face myFace;
  Standard_Real myFuzzyValue;
  Standard_Integer myDiscret;
  Standard_Real myEpsT;
  Standard_Real myDeflection;
  BRepAdaptor_Curve myC;
  BRepAdaptor_Surface myS;
  Standard_Real myCriteria;
  Standard_Boolean myIsDone;
  Standard_Integer myErrorStatus;
  Handle(IntTools_Context) myContext;
  IntTools_SequenceOfCommonPrts mySeqOfCommonPrts;
  IntTools_Range myRange;

  //! Allows avoiding use Edge-Face intersection
  //! algorithm (i.e. speeding up the Boolean algorithm)
  //! if the edges are coincided really.
  //! If it is not evidently set of this flag should
  //! be avoided (otherwise, the performance of
  //! Boolean algorithm will be slower).
  Standard_Boolean myQuickCoincidenceCheck;
};







#endif // _IntTools_EdgeFace_HeaderFile
