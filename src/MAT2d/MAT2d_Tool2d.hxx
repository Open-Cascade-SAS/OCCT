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

#include <Standard_Real.hxx>
#include <GeomAbs_JoinType.hxx>
#include <Standard_Integer.hxx>
#include <MAT2d_DataMapOfIntegerBisec.hxx>
#include <MAT2d_DataMapOfIntegerPnt2d.hxx>
#include <MAT2d_DataMapOfIntegerVec2d.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <MAT_Side.hxx>
#include <Standard_Boolean.hxx>
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
  Standard_EXPORT void Sense (const MAT_Side aside);
  
  Standard_EXPORT void SetJoinType (const GeomAbs_JoinType aJoinType);
  
  //! InitItems cuts the line in Items.
  //! this Items are the geometrics representations of
  //! the BasicElts from MAT.
  Standard_EXPORT void InitItems (const Handle(MAT2d_Circuit)& aCircuit);
  
  //! Returns the Number of Items .
  Standard_EXPORT Standard_Integer NumberOfItems() const;
  
  //! Returns tolerance to test the confusion of two points.
  Standard_EXPORT Standard_Real ToleranceOfConfusion() const;
  
  //! Creates the point at the origin of the bisector between
  //! anitem and the previous  item.
  //! dist is the distance from the FirstPoint to <anitem>.
  //! Returns the index of this point in <theGeomPnts>.
  Standard_EXPORT Standard_Integer FirstPoint (const Standard_Integer anitem, Standard_Real& dist);
  
  //! Creates the Tangent at the end of the Item defined
  //! by <anitem>. Returns the index of this vector in
  //! <theGeomVecs>
  Standard_EXPORT Standard_Integer TangentBefore (const Standard_Integer anitem, const Standard_Boolean IsOpenResult);
  
  //! Creates the Reversed Tangent at the origin of the Item
  //! defined by <anitem>. Returns the index of this vector in
  //! <theGeomVecs>
  Standard_EXPORT Standard_Integer TangentAfter (const Standard_Integer anitem, const Standard_Boolean IsOpenResult);
  
  //! Creates the Tangent at the end of the bisector defined
  //! by <bisector>. Returns the index of this vector in
  //! <theGeomVecs>
  Standard_EXPORT Standard_Integer Tangent (const Standard_Integer bisector);
  
  //! Creates the geometric bisector defined by <abisector>.
  Standard_EXPORT void CreateBisector (const Handle(MAT_Bisector)& abisector);
  
  //! Trims the geometric bisector by the <firstparameter>
  //! of <abisector>.
  //! If the parameter is out of the bisector, Return FALSE.
  //! else Return True.
  Standard_EXPORT Standard_Boolean TrimBisector (const Handle(MAT_Bisector)& abisector);
  
  //! Trims the geometric bisector by the point of index
  //! <apoint> in <theGeomPnts>.
  //! If the point is out of the bisector, Return FALSE.
  //! else Return True.
  Standard_EXPORT Standard_Boolean TrimBisector (const Handle(MAT_Bisector)& abisector, const Standard_Integer apoint);
  
  //! Computes  the point  of  intersection between  the
  //! bisectors defined  by  <bisectorone>  and
  //! <bisectortwo> .
  //! If this point exists,  <intpnt> is its  index
  //! in <theGeomPnts> and Return the distance of the point
  //! from the bisector else Return <RealLast>.
  Standard_EXPORT Standard_Real IntersectBisector (const Handle(MAT_Bisector)& bisectorone, const Handle(MAT_Bisector)& bisectortwo, Standard_Integer& intpnt);
  
  //! Returns the distance between the two points designed
  //! by their parameters on <abisector>.
  Standard_EXPORT Standard_Real Distance (const Handle(MAT_Bisector)& abisector, const Standard_Real param1, const Standard_Real param2) const;
  
  //! displays informations about the bisector defined by
  //! <bisector>.
  Standard_EXPORT void Dump (const Standard_Integer bisector, const Standard_Integer erease) const;
  
  //! Returns the <Bisec> of index <Index> in
  //! <theGeomBisectors>.
  Standard_EXPORT const Bisector_Bisec& GeomBis (const Standard_Integer Index) const;
  
  //! Returns the Geometry of index <Index> in <theGeomElts>.
  Standard_EXPORT Handle(Geom2d_Geometry) GeomElt (const Standard_Integer Index) const;
  
  //! Returns the point of index <Index> in the <theGeomPnts>.
  Standard_EXPORT const gp_Pnt2d& GeomPnt (const Standard_Integer Index) const;
  
  //! Returns the  vector  of index <Index> in the
  //! <theGeomVecs>.
  Standard_EXPORT const gp_Vec2d& GeomVec (const Standard_Integer Index) const;
  
  Standard_EXPORT Handle(MAT2d_Circuit) Circuit() const;
  
  Standard_EXPORT void BisecFusion (const Standard_Integer Index1, const Standard_Integer Index2);
  
  //! Returns the <Bisec> of index <Index> in
  //! <theGeomBisectors>.
  Standard_EXPORT Bisector_Bisec& ChangeGeomBis (const Standard_Integer Index);




protected:





private:

  
  //! Returns True if the point <apoint> is equidistant to
  //! the elements separated by bisectors <bisectorone> and
  //! <bisectortwo>.
  //! In this case <adistance> is the distance of the point
  //! from the bisectors.
  Standard_EXPORT Standard_Boolean IsSameDistance (const Handle(MAT_Bisector)& bisectorone, const Handle(MAT_Bisector)& bisectortwo, const gp_Pnt2d& apoint, Standard_Real& adistance) const;
  
  //! Return <True> if the Point can be projected
  //! on the element designed by <IndexElt>.
  //! In this case <Distance> is the minimum of distance
  //! between Point and its projections.
  Standard_EXPORT Standard_Boolean Projection (const Standard_Integer IndexElt, const gp_Pnt2d& Point, Standard_Real& Distance) const;
  
  Standard_EXPORT void TrimBisec (Bisector_Bisec& Bis, const Standard_Integer IndexEdge, const Standard_Boolean OnLine, const Standard_Integer StartOrEnd) const;


  Standard_Real theDirection;
  GeomAbs_JoinType theJoinType;
  Standard_Integer theNumberOfBisectors;
  Standard_Integer theNumberOfPnts;
  Standard_Integer theNumberOfVecs;
  Handle(MAT2d_Circuit) theCircuit;
  MAT2d_DataMapOfIntegerBisec theGeomBisectors;
  MAT2d_DataMapOfIntegerPnt2d theGeomPnts;
  MAT2d_DataMapOfIntegerVec2d theGeomVecs;
  TColStd_SequenceOfInteger theLinesLength;


};







#endif // _MAT2d_Tool2d_HeaderFile
