// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef _IntTools_EdgeEdge_HeaderFile
#define _IntTools_EdgeEdge_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepAdaptor_Curve.hxx>
#include <Standard_Integer.hxx>
#include <IntTools_CommonPrt.hxx>
#include <NCollection_Sequence.hxx>
#include <IntTools_Range.hxx>
#include <NCollection_Sequence.hxx>
#include <TopAbs_ShapeEnum.hxx>
class Geom_Curve;
class Bnd_Box;

//! The class provides Edge/Edge intersection algorithm
//! based on the intersection between edges bounding boxes.
class IntTools_EdgeEdge
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  IntTools_EdgeEdge();

  //! Destructor
  ~IntTools_EdgeEdge();

  //! Constructor
  IntTools_EdgeEdge(const TopoDS_Edge& theEdge1, const TopoDS_Edge& theEdge2);

  //! Constructor
  IntTools_EdgeEdge(const TopoDS_Edge&  theEdge1,
                    const double aT11,
                    const double aT12,
                    const TopoDS_Edge&  theEdge2,
                    const double aT21,
                    const double aT22);

  //! Sets the first edge
  void SetEdge1(const TopoDS_Edge& theEdge);

  //! Sets the first edge and its range
  void SetEdge1(const TopoDS_Edge& theEdge, const double aT1, const double aT2);

  //! Sets the range for the first edge
  void SetRange1(const IntTools_Range& theRange1);

  //! Sets the range for the first edge
  void SetRange1(const double aT1, const double aT2);

  //! Sets the second edge
  void SetEdge2(const TopoDS_Edge& theEdge);

  //! Sets the first edge and its range
  void SetEdge2(const TopoDS_Edge& theEdge, const double aT1, const double aT2);

  //! Sets the range for the second edge
  void SetRange2(const IntTools_Range& theRange);

  //! Sets the range for the second edge
  void SetRange2(const double aT1, const double aT2);

  //! Sets the Fuzzy value
  void SetFuzzyValue(const double theFuzz);

  //! Performs the intersection between edges
  Standard_EXPORT void Perform();

  //! Returns TRUE if common part(s) is(are) found
  bool IsDone() const;

  //! Returns Fuzzy value
  double FuzzyValue() const;

  //! Returns common parts
  const NCollection_Sequence<IntTools_CommonPrt>& CommonParts() const;

  //! Sets the flag myQuickCoincidenceCheck
  void UseQuickCoincidenceCheck(const bool bFlag) { myQuickCoincidenceCheck = bFlag; }

  //! Returns the flag myQuickCoincidenceCheck
  bool IsCoincidenceCheckedQuickly() { return myQuickCoincidenceCheck; }

protected:
  //! Checks the data
  void CheckData();

  //! Prepares the data
  Standard_EXPORT void Prepare();

  //! Computes Line/Line intersection.
  Standard_EXPORT void ComputeLineLine();

  //! Intermediate function
  Standard_EXPORT void FindSolutions(NCollection_Sequence<IntTools_Range>& theRanges1,
                                     NCollection_Sequence<IntTools_Range>& theRanges2,
                                     bool&          bSplit2);

  //! Looking for the exact intersection ranges
  Standard_EXPORT void FindSolutions(const IntTools_Range&      theR1,
                                     const Bnd_Box&             theBox1,
                                     const IntTools_Range&      theR2,
                                     const Bnd_Box&             theBox2,
                                     NCollection_Sequence<IntTools_Range>& theRanges1,
                                     NCollection_Sequence<IntTools_Range>& theRanges2);

  //! Merges found solutions
  Standard_EXPORT void MergeSolutions(const NCollection_Sequence<IntTools_Range>& theRanges1,
                                      const NCollection_Sequence<IntTools_Range>& theRanges2,
                                      const bool           bSplit2);

  //! Looking for the range of the edge which is in the box
  Standard_EXPORT static bool FindParameters(const BRepAdaptor_Curve& theBAC,
                                                         const double      aT1,
                                                         const double      aT2,
                                                         const double      theTol,
                                                         const double      theRes,
                                                         const double      thePTol,
                                                         const double      theResCoeff,
                                                         const Bnd_Box&           theCBox,
                                                         double&           aTB1,
                                                         double&           aTB2);

  //! Checks if edges coincide on the ranges
  Standard_EXPORT int CheckCoincidence(const double aT11,
                                                    const double aT12,
                                                    const double aT21,
                                                    const double aT22,
                                                    const double theCriteria,
                                                    const double theCurveRes1);

  //! Adds common part of the given type to myCommonParts
  Standard_EXPORT void AddSolution(const double    aT11,
                                   const double    aT12,
                                   const double    aT21,
                                   const double    aT22,
                                   const TopAbs_ShapeEnum theType);

  //! Looking for the minimal distance between edges on the ranges
  Standard_EXPORT void FindBestSolution(const double aT11,
                                        const double aT12,
                                        const double aT21,
                                        const double aT22,
                                        double&      aT1,
                                        double&      aT2);

  //! Checks is there an intersection between edges on the given ranges
  //! (for nearly conicident edges)
  Standard_EXPORT bool IsIntersection(const double aT11,
                                                  const double aT12,
                                                  const double aT21,
                                                  const double aT22);

  //! Checks if the edges are coincident really.
  Standard_EXPORT bool IsCoincident();

  TopoDS_Edge                   myEdge1;
  TopoDS_Edge                   myEdge2;
  occ::handle<Geom_Curve>            myGeom1;
  occ::handle<Geom_Curve>            myGeom2;
  BRepAdaptor_Curve             myCurve1;
  BRepAdaptor_Curve             myCurve2;
  double                 myTol1;
  double                 myTol2;
  double                 myTol;
  double                 myFuzzyValue;
  double                 myRes1;
  double                 myRes2;
  double                 myResCoeff1;
  double                 myResCoeff2;
  double                 myPTol1;
  double                 myPTol2;
  IntTools_Range                myRange1;
  IntTools_Range                myRange2;
  bool              mySwap;
  int              myErrorStatus;
  NCollection_Sequence<IntTools_CommonPrt> myCommonParts;

  //! Allows avoiding use Edge-Edge intersection
  //! algorithm (i.e. speeding up the Boolean algorithm)
  //! if the edges are coincided really.
  //! If it is not evidently set of this flag should
  //! be avoided (otherwise, the performance of
  //! Boolean algorithm will be slower).
  bool myQuickCoincidenceCheck;

};

#include <IntTools_EdgeEdge.lxx>

#endif // _IntTools_EdgeEdge_HeaderFile
