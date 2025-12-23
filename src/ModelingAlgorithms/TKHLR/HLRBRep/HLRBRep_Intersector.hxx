// Created on: 1992-08-26
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _HLRBRep_Intersector_HeaderFile
#define _HLRBRep_Intersector_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Standard_Integer.hxx>
#include <HLRBRep_CInter.hxx>
#include <HLRBRep_InterCSurf.hxx>

class gp_Lin;
class HLRBRep_EdgeData;
class HLRBRep_Surface;
class IntCurveSurface_IntersectionPoint;
class IntRes2d_IntersectionSegment;
class IntCurveSurface_IntersectionSegment;

//! The Intersector computes 2D intersections of the projections of 3D curves.
//! It can also computes the intersection of a 3D line and a surface.
class HLRBRep_Intersector
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT HLRBRep_Intersector();

  //! Performs the auto intersection of an edge.
  //! The edge domain is cut at start with da1*(b-a) and at end with db1*(b-a).
  Standard_EXPORT void Perform(HLRBRep_EdgeData* theEdge1,
                               const double      theDa1,
                               const double      theDb1);

  //! Performs the intersection between the two edges.
  //! The edges domains are cut at start with da*(b-a) and at end with db*(b-a).
  Standard_EXPORT void Perform(const int         theNA,
                               HLRBRep_EdgeData* theEdge1,
                               const double      theDa1,
                               const double      theDb1,
                               const int         theNB,
                               HLRBRep_EdgeData* theEdge2,
                               const double      theDa2,
                               const double      theDb2,
                               const bool        theNoBound);

  //! Create a single IntersectionPoint (U on theEdge1) (V on theEdge2)
  //! The point is middle on both curves.
  Standard_EXPORT void SimulateOnePoint(HLRBRep_EdgeData* theEdge1,
                                        const double      theU,
                                        HLRBRep_EdgeData* theEdge2,
                                        const double      theV);

  Standard_EXPORT void Load(HLRBRep_Surface* theSurface);

  Standard_EXPORT void Perform(const gp_Lin& theL, const double theP);

  Standard_EXPORT Standard_Boolean IsDone() const;

  Standard_EXPORT Standard_Integer NbPoints() const;

  Standard_EXPORT const IntRes2d_IntersectionPoint& Point(const Standard_Integer N) const;

  Standard_EXPORT const IntCurveSurface_IntersectionPoint& CSPoint(const Standard_Integer N) const;

  Standard_EXPORT Standard_Integer NbSegments() const;

  Standard_EXPORT const IntRes2d_IntersectionSegment& Segment(const Standard_Integer N) const;

  Standard_EXPORT const IntCurveSurface_IntersectionSegment& CSSegment(
    const Standard_Integer N) const;

  Standard_EXPORT void Destroy();

  ~HLRBRep_Intersector() { Destroy(); }

private:
  IntRes2d_IntersectionPoint         mySinglePoint;
  int                                myTypePerform;
  HLRBRep_CInter                     myIntersector;
  HLRBRep_InterCSurf                 myCSIntersector;
  HLRBRep_Surface*                   mySurface;
  HLRBRep_ThePolyhedronOfInterCSurf* myPolyhedron;
};

#endif // _HLRBRep_Intersector_HeaderFile
