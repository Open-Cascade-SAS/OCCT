// Created on: 1993-07-12
// Created by: Yves FRICAUD
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _MAT2d_Tool2d_HeaderFile
#define _MAT2d_Tool2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomAbs_JoinType.hxx>
#include <Standard_Integer.hxx>
#include <Bisector_Bisec.hxx>
#include <NCollection_DataMap.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <NCollection_Sequence.hxx>
#include <MAT_Side.hxx>
class MAT2d_Circuit;
class MAT_Bisector;
class Bisector_Bisec;
class Geom2d_Geometry;
class gp_Pnt2d;
class gp_Vec2d;

//! Set of the methods useful for the MAT's computation.
//! Tool2d contains the geometry of the bisecting locus.
class MAT2d_Tool2d
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty Constructor.
  Standard_EXPORT MAT2d_Tool2d();

  //! <aSide> defines the side of the computation of the map.
  Standard_EXPORT void Sense(const MAT_Side aside);

  Standard_EXPORT void SetJoinType(const GeomAbs_JoinType aJoinType);

  //! InitItems cuts the line in Items.
  //! this Items are the geometrics representations of
  //! the BasicElts from MAT.
  Standard_EXPORT void InitItems(const occ::handle<MAT2d_Circuit>& aCircuit);

  //! Returns the Number of Items .
  Standard_EXPORT int NumberOfItems() const;

  //! Returns tolerance to test the confusion of two points.
  Standard_EXPORT double ToleranceOfConfusion() const;

  //! Creates the point at the origin of the bisector between
  //! anitem and the previous item.
  //! dist is the distance from the FirstPoint to <anitem>.
  //! Returns the index of this point in <theGeomPnts>.
  Standard_EXPORT int FirstPoint(const int anitem, double& dist);

  //! Creates the Tangent at the end of the Item defined
  //! by <anitem>. Returns the index of this vector in
  //! <theGeomVecs>
  Standard_EXPORT int TangentBefore(const int anitem, const bool IsOpenResult);

  //! Creates the Reversed Tangent at the origin of the Item
  //! defined by <anitem>. Returns the index of this vector in
  //! <theGeomVecs>
  Standard_EXPORT int TangentAfter(const int anitem, const bool IsOpenResult);

  //! Creates the Tangent at the end of the bisector defined
  //! by <bisector>. Returns the index of this vector in
  //! <theGeomVecs>
  Standard_EXPORT int Tangent(const int bisector);

  //! Creates the geometric bisector defined by <abisector>.
  Standard_EXPORT void CreateBisector(const occ::handle<MAT_Bisector>& abisector);

  //! Trims the geometric bisector by the <firstparameter>
  //! of <abisector>.
  //! If the parameter is out of the bisector, Return FALSE.
  //! else Return True.
  Standard_EXPORT bool TrimBisector(const occ::handle<MAT_Bisector>& abisector);

  //! Trims the geometric bisector by the point of index
  //! <apoint> in <theGeomPnts>.
  //! If the point is out of the bisector, Return FALSE.
  //! else Return True.
  Standard_EXPORT bool TrimBisector(const occ::handle<MAT_Bisector>& abisector, const int apoint);

  //! Computes the point of intersection between the
  //! bisectors defined by <bisectorone> and
  //! <bisectortwo> .
  //! If this point exists, <intpnt> is its index
  //! in <theGeomPnts> and Return the distance of the point
  //! from the bisector else Return <RealLast>.
  Standard_EXPORT double IntersectBisector(const occ::handle<MAT_Bisector>& bisectorone,
                                           const occ::handle<MAT_Bisector>& bisectortwo,
                                           int&                             intpnt);

  //! Returns the distance between the two points designed
  //! by their parameters on <abisector>.
  Standard_EXPORT double Distance(const occ::handle<MAT_Bisector>& abisector,
                                  const double                     param1,
                                  const double                     param2) const;

  //! displays information about the bisector defined by
  //! <bisector>.
  Standard_EXPORT void Dump(const int bisector, const int erease) const;

  //! Returns the <Bisec> of index <Index> in
  //! <theGeomBisectors>.
  Standard_EXPORT const Bisector_Bisec& GeomBis(const int Index) const;

  //! Returns the Geometry of index <Index> in <theGeomElts>.
  Standard_EXPORT occ::handle<Geom2d_Geometry> GeomElt(const int Index) const;

  //! Returns the point of index <Index> in the <theGeomPnts>.
  Standard_EXPORT const gp_Pnt2d& GeomPnt(const int Index) const;

  //! Returns the vector of index <Index> in the
  //! <theGeomVecs>.
  Standard_EXPORT const gp_Vec2d& GeomVec(const int Index) const;

  Standard_EXPORT occ::handle<MAT2d_Circuit> Circuit() const;

  Standard_EXPORT void BisecFusion(const int Index1, const int Index2);

  //! Returns the <Bisec> of index <Index> in
  //! <theGeomBisectors>.
  Standard_EXPORT Bisector_Bisec& ChangeGeomBis(const int Index);

private:
  //! Returns True if the point <apoint> is equidistant to
  //! the elements separated by bisectors <bisectorone> and
  //! <bisectortwo>.
  //! In this case <adistance> is the distance of the point
  //! from the bisectors.
  Standard_EXPORT bool IsSameDistance(const occ::handle<MAT_Bisector>& bisectorone,
                                      const occ::handle<MAT_Bisector>& bisectortwo,
                                      const gp_Pnt2d&                  apoint,
                                      double&                          adistance) const;

  //! Return <True> if the Point can be projected
  //! on the element designed by <IndexElt>.
  //! In this case <Distance> is the minimum of distance
  //! between Point and its projections.
  Standard_EXPORT bool Projection(const int       IndexElt,
                                  const gp_Pnt2d& Point,
                                  double&         Distance) const;

  Standard_EXPORT void TrimBisec(Bisector_Bisec& Bis,
                                 const int       IndexEdge,
                                 const bool      OnLine,
                                 const int       StartOrEnd) const;

  double                                   theDirection;
  GeomAbs_JoinType                         theJoinType;
  int                                      theNumberOfBisectors;
  int                                      theNumberOfPnts;
  int                                      theNumberOfVecs;
  occ::handle<MAT2d_Circuit>               theCircuit;
  NCollection_DataMap<int, Bisector_Bisec> theGeomBisectors;
  NCollection_DataMap<int, gp_Pnt2d>       theGeomPnts;
  NCollection_DataMap<int, gp_Vec2d>       theGeomVecs;
  NCollection_Sequence<int>                theLinesLength;
};

#endif // _MAT2d_Tool2d_HeaderFile
