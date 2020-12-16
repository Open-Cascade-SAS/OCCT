// Created on: 1993-01-28
// Created by: Laurent BUCHARD
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

#ifndef _IntPatch_PrmPrmIntersection_HeaderFile
#define _IntPatch_PrmPrmIntersection_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <IntPatch_SequenceOfLine.hxx>
#include <IntSurf_ListOfPntOn2S.hxx>

class Adaptor3d_TopolTool;
class IntPatch_Polyhedron;
class IntPatch_PrmPrmIntersection_T3Bits;
class IntSurf_LineOn2S;

//! Implementation of the Intersection between two bi-parametrised surfaces.
//!
//! To avoid multiple constructions of the approximated
//! polyhedron of the surfaces, the algorithm can be
//! called with the two surfaces and their associated polyhedron.
class IntPatch_PrmPrmIntersection 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty Constructor
  Standard_EXPORT IntPatch_PrmPrmIntersection();
  
  //! Performs the intersection between <Caro1>  and
  //! <Caro2>.  Associated Polyhedrons <Polyhedron1>
  //! and <Polyhedron2> are given.
  Standard_EXPORT void Perform (const Handle(Adaptor3d_Surface)& Caro1, const IntPatch_Polyhedron& Polyhedron1, const Handle(Adaptor3d_TopolTool)& Domain1, const Handle(Adaptor3d_Surface)& Caro2, const IntPatch_Polyhedron& Polyhedron2, const Handle(Adaptor3d_TopolTool)& Domain2, const Standard_Real TolTangency, const Standard_Real Epsilon, const Standard_Real Deflection, const Standard_Real Increment);
  
  Standard_EXPORT void Perform (const Handle(Adaptor3d_Surface)& Caro1, const IntPatch_Polyhedron& Polyhedron1, const Handle(Adaptor3d_TopolTool)& Domain1, const Standard_Real TolTangency, const Standard_Real Epsilon, const Standard_Real Deflection, const Standard_Real Increment);
  
  //! Performs the intersection between <Caro1>  and
  //! <Caro2>. The method computes the polyhedron on
  //! each surface.
  Standard_EXPORT void Perform (const Handle(Adaptor3d_Surface)& Caro1, const Handle(Adaptor3d_TopolTool)& Domain1, const Handle(Adaptor3d_Surface)& Caro2, const Handle(Adaptor3d_TopolTool)& Domain2, const Standard_Real TolTangency, const Standard_Real Epsilon, const Standard_Real Deflection, const Standard_Real Increment, const Standard_Boolean ClearFlag = Standard_True);
  
  //! Performs the intersection between <Caro1>  and
  //! <Caro2>. The method computes the polyhedron on
  //! each surface.
  Standard_EXPORT void Perform (const Handle(Adaptor3d_Surface)& Caro1, const Handle(Adaptor3d_TopolTool)& Domain1, const Handle(Adaptor3d_Surface)& Caro2, const Handle(Adaptor3d_TopolTool)& Domain2, const Standard_Real TolTangency, const Standard_Real Epsilon, const Standard_Real Deflection, const Standard_Real Increment, IntSurf_ListOfPntOn2S& ListOfPnts);
  
  //! Performs the intersection between <Caro1>  and
  //! <Caro2>. The method computes the polyhedron on
  //! each surface.
  Standard_EXPORT void Perform (const Handle(Adaptor3d_Surface)& Caro1, const Handle(Adaptor3d_TopolTool)& Domain1, const Handle(Adaptor3d_Surface)& Caro2, const Handle(Adaptor3d_TopolTool)& Domain2, const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2, const Standard_Real TolTangency, const Standard_Real Epsilon, const Standard_Real Deflection, const Standard_Real Increment);
  
  //! Performs the intersection between <Caro1>  and
  //! <Caro2>. The method computes the polyhedron on
  //! each surface.
  Standard_EXPORT void Perform (const Handle(Adaptor3d_Surface)& Caro1, const Handle(Adaptor3d_TopolTool)& Domain1, const Standard_Real TolTangency, const Standard_Real Epsilon, const Standard_Real Deflection, const Standard_Real Increment);
  
  //! Performs  the intersection between <Caro1> and
  //! <Caro2>.
  //!
  //! The polyhedron which approximates     <Caro2>,
  //! <Polyhedron2> is given. The other one is
  //! computed.
  Standard_EXPORT void Perform (const Handle(Adaptor3d_Surface)& Caro1, const Handle(Adaptor3d_TopolTool)& Domain1, const Handle(Adaptor3d_Surface)& Caro2, const IntPatch_Polyhedron& Polyhedron2, const Handle(Adaptor3d_TopolTool)& Domain2, const Standard_Real TolTangency, const Standard_Real Epsilon, const Standard_Real Deflection, const Standard_Real Increment);
  
  //! Performs  the intersection between <Caro1> and
  //! <Caro2>.
  //!
  //! The polyhedron which approximates     <Caro1>,
  //! <Polyhedron1> is given. The other one is
  //! computed.
  Standard_EXPORT void Perform (const Handle(Adaptor3d_Surface)& Caro1, const IntPatch_Polyhedron& Polyhedron1, const Handle(Adaptor3d_TopolTool)& Domain1, const Handle(Adaptor3d_Surface)& Caro2, const Handle(Adaptor3d_TopolTool)& Domain2, const Standard_Real TolTangency, const Standard_Real Epsilon, const Standard_Real Deflection, const Standard_Real Increment);
  
  //! Returns true if the calculus was successful.
    Standard_Boolean IsDone() const;
  
  //! Returns true if the is no intersection.
    Standard_Boolean IsEmpty() const;
  
  //! Returns the number of intersection lines.
    Standard_Integer NbLines() const;
  
  //! Returns the line of range Index.
  //! An exception is raised if Index<=0 or Index>NbLine.
    const Handle(IntPatch_Line)& Line (const Standard_Integer Index) const;
  
  //! Computes about <NbPoints>  Intersection Points  on
  //! the Line <IndexLine> between  the Points of  Index
  //! <LowPoint> and <HighPoint>.
  //!
  //! All  the points  of the line  of index <IndexLine>
  //! with an index  between <LowPoint>  and <HighPoint>
  //! are in the returned  line. New Points are inserted
  //! between existing points  if  those  points are not
  //! too closed.
  //!
  //! An exception is raised if Index<=0 or Index>NbLine.
  //! or if IsDone returns False
  Standard_EXPORT Handle(IntPatch_Line) NewLine (const Handle(Adaptor3d_Surface)& Caro1, const Handle(Adaptor3d_Surface)& Caro2, const Standard_Integer IndexLine, const Standard_Integer LowPoint, const Standard_Integer HighPoint, const Standard_Integer NbPoints) const;
  
    Standard_Integer GrilleInteger (const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz) const;
  
    void IntegerGrille (const Standard_Integer t, Standard_Integer& ix, Standard_Integer& iy, Standard_Integer& iz) const;
  
    Standard_Integer DansGrille (const Standard_Integer t) const;
  
    Standard_Integer NbPointsGrille() const;
  
  Standard_EXPORT void RemplitLin (const Standard_Integer x1, const Standard_Integer y1, const Standard_Integer z1, const Standard_Integer x2, const Standard_Integer y2, const Standard_Integer z2, IntPatch_PrmPrmIntersection_T3Bits& Map) const;
  
  Standard_EXPORT void RemplitTri (const Standard_Integer x1, const Standard_Integer y1, const Standard_Integer z1, const Standard_Integer x2, const Standard_Integer y2, const Standard_Integer z2, const Standard_Integer x3, const Standard_Integer y3, const Standard_Integer z3, IntPatch_PrmPrmIntersection_T3Bits& Map) const;
  
  Standard_EXPORT void Remplit (const Standard_Integer a, const Standard_Integer b, const Standard_Integer c, IntPatch_PrmPrmIntersection_T3Bits& Map) const;
  
  Standard_Integer CodeReject (const Standard_Real x1, const Standard_Real y1, const Standard_Real z1, const Standard_Real x2, const Standard_Real y2, const Standard_Real z2, const Standard_Real x3, const Standard_Real y3, const Standard_Real z3) const;
  
  Standard_EXPORT void PointDepart (Handle(IntSurf_LineOn2S)& LineOn2S, const Handle(Adaptor3d_Surface)& S1, const Standard_Integer SU1, const Standard_Integer SV1, const Handle(Adaptor3d_Surface)& S2, const Standard_Integer SU2, const Standard_Integer SV2) const;




protected:





private:



  Standard_Boolean done;
  Standard_Boolean empt;
  IntPatch_SequenceOfLine SLin;


};


#include <IntPatch_PrmPrmIntersection.lxx>





#endif // _IntPatch_PrmPrmIntersection_HeaderFile
