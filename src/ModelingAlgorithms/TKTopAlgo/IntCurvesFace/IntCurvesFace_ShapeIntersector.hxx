// Created on: 1998-01-27
// Created by: Laurent BUCHARD
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

#ifndef _IntCurvesFace_ShapeIntersector_HeaderFile
#define _IntCurvesFace_ShapeIntersector_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Real.hxx>
#include <IntCurveSurface_TransitionOnCurve.hxx>
#include <IntCurvesFace_Intersector.hxx>
#include <TopAbs_State.hxx>

class Adaptor3d_Curve;
class TopoDS_Shape;
class gp_Lin;
class gp_Pnt;
class TopoDS_Face;

class IntCurvesFace_ShapeIntersector
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT IntCurvesFace_ShapeIntersector();

  Standard_EXPORT void Load(const TopoDS_Shape& Sh, const double Tol);

  //! Perform the intersection between the
  //! segment L and the loaded shape.
  //!
  //! PInf is the smallest parameter on the line
  //! PSup is the highest parameter on the line
  //!
  //! For an infinite line PInf and PSup can be
  //! +/- RealLast.
  Standard_EXPORT void Perform(const gp_Lin& L, const double PInf, const double PSup);

  //! Perform the intersection between the
  //! segment L and the loaded shape.
  //!
  //! PInf is the smallest parameter on the line
  //! PSup is the highest parameter on the line
  //!
  //! For an infinite line PInf and PSup can be
  //! +/- RealLast.
  Standard_EXPORT void PerformNearest(const gp_Lin&       L,
                                      const double PInf,
                                      const double PSup);

  //! same method for a HCurve from Adaptor3d.
  //! PInf an PSup can also be -INF and +INF.
  Standard_EXPORT void Perform(const occ::handle<Adaptor3d_Curve>& HCu,
                               const double            PInf,
                               const double            PSup);

  //! True when the intersection has been computed.
  bool IsDone() const { return myIsDone; }

  //! Returns the number of the intersection points
  int NbPnt() const { return myIndexPt.Length(); }

  //! Returns the U parameter of the ith intersection point
  //! on the surface.
  double UParameter(const int I) const
  {
    occ::handle<IntCurvesFace_Intersector> anIntAdaptor = myIntersector(myIndexFace(myIndexPt(I)));
    return anIntAdaptor->UParameter(myIndexIntPnt(myIndexPt(I)));
  }

  //! Returns the V parameter of the ith intersection point
  //! on the surface.
  double VParameter(const int I) const
  {
    occ::handle<IntCurvesFace_Intersector> anIntAdaptor = myIntersector(myIndexFace(myIndexPt(I)));
    return anIntAdaptor->VParameter(myIndexIntPnt(myIndexPt(I)));
  }

  //! Returns the parameter of the ith intersection point
  //! on the line.
  double WParameter(const int I) const
  {
    occ::handle<IntCurvesFace_Intersector> anIntAdaptor = myIntersector(myIndexFace(myIndexPt(I)));
    return anIntAdaptor->WParameter(myIndexIntPnt(myIndexPt(I)));
  }

  //! Returns the geometric point of the ith intersection
  //! between the line and the surface.
  const gp_Pnt& Pnt(const int I) const
  {
    occ::handle<IntCurvesFace_Intersector> anIntAdaptor = myIntersector(myIndexFace(myIndexPt(I)));
    return anIntAdaptor->Pnt(myIndexIntPnt(myIndexPt(I)));
  }

  //! Returns the ith transition of the line on the surface.
  IntCurveSurface_TransitionOnCurve Transition(const int I) const
  {
    occ::handle<IntCurvesFace_Intersector> anIntAdaptor = myIntersector(myIndexFace(myIndexPt(I)));
    return anIntAdaptor->Transition(myIndexIntPnt(myIndexPt(I)));
  }

  //! Returns the ith state of the point on the face.
  //! The values can be either TopAbs_IN
  //! ( the point is in the face)
  //! or TopAbs_ON
  //! ( the point is on a boundary of the face).
  TopAbs_State State(const int I) const
  {
    occ::handle<IntCurvesFace_Intersector> anIntAdaptor = myIntersector(myIndexFace(myIndexPt(I)));
    return anIntAdaptor->State(myIndexIntPnt(myIndexPt(I)));
  }

  //! Returns the significant face used to determine
  //! the intersection.
  const TopoDS_Face& Face(const int I) const
  {
    occ::handle<IntCurvesFace_Intersector> anIntAdaptor = myIntersector(myIndexFace(myIndexPt(I)));
    return anIntAdaptor->Face();
  }

  //! Internal method. Sort the result on the Curve
  //! parameter.
  Standard_EXPORT void SortResult();

  Standard_EXPORT virtual ~IntCurvesFace_ShapeIntersector();

private:
  bool                                        myIsDone;
  int                                        myNbFaces;
  NCollection_Array1<int>                                 myPtrNums;
  NCollection_Array1<int>                                 myPtrIndexNums;
  NCollection_Sequence<occ::handle<IntCurvesFace_Intersector>> myIntersector;
  NCollection_Sequence<int>                               myIndexPt;
  NCollection_Sequence<int>                               myIndexFace;
  NCollection_Sequence<int>                               myIndexIntPnt;
  NCollection_Sequence<double>                                  myIndexPar;
};

#endif // _IntCurvesFace_ShapeIntersector_HeaderFile
