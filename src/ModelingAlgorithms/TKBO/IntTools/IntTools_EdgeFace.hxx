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
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <Standard_Boolean.hxx>
#include <IntTools_CommonPrt.hxx>
#include <NCollection_Sequence.hxx>
#include <IntTools_Range.hxx>
class IntTools_Context;
class gp_Pnt;
class IntTools_CommonPrt;

//! The class provides Edge/Face intersection algorithm to determine
//! common parts between edge and face in 3-d space.
//! Common parts between Edge and Face can be:
//! - Vertices - in case of intersection or touching;
//! - Edge - in case of full coincidence of the edge with the face.
class IntTools_EdgeFace
{
public:
  DEFINE_STANDARD_ALLOC

public: //! @name Constructors
  //! Empty Constructor
  Standard_EXPORT IntTools_EdgeFace();

public: //! @name Setters/Getters
  //! Sets the edge for intersection
  void SetEdge(const TopoDS_Edge& theEdge) { myEdge = theEdge; }

  //! Returns the edge
  const TopoDS_Edge& Edge() const { return myEdge; }

  //! Sets the face for intersection
  void SetFace(const TopoDS_Face& theFace) { myFace = theFace; }

  //! Returns the face
  const TopoDS_Face& Face() const { return myFace; }

  //! Sets the boundaries for the edge.
  //! The algorithm processes edge inside these boundaries.
  void SetRange(const IntTools_Range& theRange) { myRange = theRange; }

  //! Sets the boundaries for the edge.
  //! The algorithm processes edge inside these boundaries.
  void SetRange(const double theFirst, const double theLast)
  {
    myRange.SetFirst(theFirst);
    myRange.SetLast(theLast);
  }

  //! Returns intersection range of the edge
  const IntTools_Range& Range() const { return myRange; }

  //! Sets the intersection context
  void SetContext(const occ::handle<IntTools_Context>& theContext) { myContext = theContext; }

  //! Returns the intersection context
  const occ::handle<IntTools_Context>& Context() const { return myContext; }

  //! Sets the Fuzzy value
  void SetFuzzyValue(const double theFuzz)
  {
    myFuzzyValue = (std::max)(theFuzz, Precision::Confusion());
  }

  //! Returns the Fuzzy value
  double FuzzyValue() const { return myFuzzyValue; }

  //! Sets the flag for quick coincidence check.
  //! It is safe to use the quick check for coincidence only if both
  //! of the following conditions are met:
  //! - The vertices of edge are lying on the face;
  //! - The edge does not intersect the boundaries of the face on the given range.
  void UseQuickCoincidenceCheck(const bool theFlag) { myQuickCoincidenceCheck = theFlag; }

  //! Returns the flag myQuickCoincidenceCheck
  bool IsCoincidenceCheckedQuickly() { return myQuickCoincidenceCheck; }

public: //! @name Performing the operation
  //! Launches the process
  Standard_EXPORT void Perform();

public: //! @name Checking validity of the intersection
  //! Returns TRUE if computation was successful.
  //! Otherwise returns FALSE.
  bool IsDone() const { return myIsDone; }

  //! Returns the code of completion:
  //! 0 - means successful completion;
  //! 1 - the process was not started;
  //! 2,3 - invalid source data for the algorithm;
  //! 4 - projection failed.
  int ErrorStatus() const { return myErrorStatus; }

public: //! @name Obtaining results
  //! Returns resulting common parts
  const NCollection_Sequence<IntTools_CommonPrt>& CommonParts() const { return mySeqOfCommonPrts; }

  //! Returns the minimal distance found between edge and face
  double MinimalDistance() const { return myMinDistance; }

protected: //! @name Protected methods performing the intersection
  Standard_EXPORT static bool IsEqDistance(const gp_Pnt&              aP,
                                           const BRepAdaptor_Surface& aS,
                                           const double               aT,
                                           double&                    aD);
  Standard_EXPORT void        CheckData();

  Standard_EXPORT bool IsProjectable(const double t) const;

  Standard_EXPORT double DistanceFunction(const double t);

  Standard_EXPORT int MakeType(IntTools_CommonPrt& aCP);

  Standard_EXPORT bool CheckTouch(const IntTools_CommonPrt& aCP, double& aTX);

  Standard_EXPORT bool CheckTouchVertex(const IntTools_CommonPrt& aCP, double& aTX);

  //! Checks if the edge is in the face really.
  Standard_EXPORT bool IsCoincident();

protected:
  TopoDS_Edge                              myEdge;
  TopoDS_Face                              myFace;
  double                                   myFuzzyValue;
  BRepAdaptor_Curve                        myC;
  BRepAdaptor_Surface                      myS;
  double                                   myCriteria;
  bool                                     myIsDone;
  int                                      myErrorStatus;
  occ::handle<IntTools_Context>            myContext;
  NCollection_Sequence<IntTools_CommonPrt> mySeqOfCommonPrts;
  IntTools_Range                           myRange;
  bool                                     myQuickCoincidenceCheck;
  double                                   myMinDistance; //!< Minimal distance found
};

#endif // _IntTools_EdgeFace_HeaderFile
