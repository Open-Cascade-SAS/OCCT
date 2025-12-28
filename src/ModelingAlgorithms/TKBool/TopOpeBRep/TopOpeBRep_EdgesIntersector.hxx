// Created on: 1994-10-13
// Created by: Jean Yves LEBEY
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _TopOpeBRep_EdgesIntersector_HeaderFile
#define _TopOpeBRep_EdgesIntersector_HeaderFile

#include <BRepAdaptor_Surface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <IntRes2d_Domain.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <NCollection_Sequence.hxx>
#include <IntRes2d_IntersectionSegment.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopoDS_Face.hxx>
#include <TopOpeBRep_Point2d.hxx>
#include <TopOpeBRep_P2Dstatus.hxx>
#include <TopOpeBRepDS_Config.hxx>

class Bnd_Box;
class BRepAdaptor_Surface;
class TCollection_AsciiString;
class TopOpeBRep_Point2d;
class IntRes2d_IntersectionSegment;
class IntRes2d_IntersectionPoint;
class TopOpeBRepDS_Transition;
class gp_Pnt;

//! Describes the intersection of two edges on the same surface
class TopOpeBRep_EdgesIntersector
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopOpeBRep_EdgesIntersector();

  Standard_EXPORT virtual ~TopOpeBRep_EdgesIntersector();

  Standard_EXPORT void SetFaces(const TopoDS_Shape& F1, const TopoDS_Shape& F2);

  Standard_EXPORT void SetFaces(const TopoDS_Shape& F1,
                                const TopoDS_Shape& F2,
                                const Bnd_Box&      B1,
                                const Bnd_Box&      B2);

  Standard_EXPORT void ForceTolerances(const double Tol1, const double Tol2);

  Standard_EXPORT void Dimension(const int D);

  //! set working space dimension D = 1 for E &|| W, 2 for E in F
  Standard_EXPORT int Dimension() const;

  Standard_EXPORT void Perform(const TopoDS_Shape& E1,
                               const TopoDS_Shape& E2,
                               const bool          ReduceSegments = true);

  Standard_EXPORT bool IsEmpty();

  //! true if at least one intersection segment.
  Standard_EXPORT bool HasSegment() const;

  //! = mySameDomain.
  Standard_EXPORT bool SameDomain() const;

  Standard_EXPORT const TopoDS_Shape& Edge(const int Index) const;

  Standard_EXPORT const Geom2dAdaptor_Curve& Curve(const int Index) const;

  Standard_EXPORT const TopoDS_Shape& Face(const int Index) const;

  Standard_EXPORT const BRepAdaptor_Surface& Surface(const int Index) const;

  Standard_EXPORT bool SurfacesSameOriented() const;

  Standard_EXPORT bool FacesSameOriented() const;

  Standard_EXPORT double ToleranceMax() const;

  Standard_EXPORT void Tolerances(double& tol1, double& tol2) const;

  Standard_EXPORT int NbPoints() const;

  Standard_EXPORT int NbSegments() const;

  Standard_EXPORT void Dump(const TCollection_AsciiString& str,
                            const int                      ie1 = 0,
                            const int                      ie2 = 0);

  Standard_EXPORT void InitPoint(const bool selectkeep = true);

  Standard_EXPORT bool MorePoint() const;

  Standard_EXPORT void NextPoint();

  Standard_EXPORT const NCollection_Sequence<TopOpeBRep_Point2d>& Points() const;

  Standard_EXPORT const TopOpeBRep_Point2d& Point() const;

  Standard_EXPORT const TopOpeBRep_Point2d& Point(const int I) const;

  Standard_EXPORT virtual bool ReduceSegment(TopOpeBRep_Point2d& P1,
                                             TopOpeBRep_Point2d& P2,
                                             TopOpeBRep_Point2d& Pn) const;

  Standard_EXPORT TopOpeBRep_P2Dstatus Status1() const;

private:
  Standard_EXPORT void Find();

  //! process if current edges can be considered as SameDomain
  Standard_EXPORT bool ComputeSameDomain();

  //! set field mySameDomain to B and return B value
  Standard_EXPORT bool SetSameDomain(const bool B);

  Standard_EXPORT void MakePoints2d();

  Standard_EXPORT void ReduceSegments();

  Standard_EXPORT const IntRes2d_IntersectionSegment& Segment1() const;

  Standard_EXPORT bool IsOpposite1() const;

  Standard_EXPORT void InitPoint1();

  Standard_EXPORT bool MorePoint1() const;

  Standard_EXPORT void NextPoint1();

  Standard_EXPORT const IntRes2d_IntersectionPoint& Point1() const;

  Standard_EXPORT TopOpeBRepDS_Transition Transition1(const int                Index,
                                                      const TopAbs_Orientation EO) const;

  Standard_EXPORT double Parameter1(const int Index) const;

  Standard_EXPORT bool IsVertex1(const int Index);

  Standard_EXPORT const TopoDS_Shape& Vertex1(const int Index);

  Standard_EXPORT gp_Pnt Value1() const;

  Standard_EXPORT bool IsPointOfSegment1() const;

  Standard_EXPORT int Index1() const;

  //! geometric configuration of E1,E2 at current intersection point :
  //! UNSHGEOMETRY if the edges do not share geometry.
  //! SAMEORIENTED if the edges share geometry and are same oriented.
  //! DIFFORIENTED if the edges share geometry and are not same oriented.
  Standard_EXPORT TopOpeBRepDS_Config EdgesConfig1() const;

  TopoDS_Face                                        myFace1;
  TopoDS_Face                                        myFace2;
  occ::handle<BRepAdaptor_Surface>                   mySurface1;
  occ::handle<BRepAdaptor_Surface>                   mySurface2;
  GeomAbs_SurfaceType                                mySurfaceType1;
  GeomAbs_SurfaceType                                mySurfaceType2;
  bool                                               mySurfacesSameOriented;
  bool                                               myFacesSameOriented;
  IntRes2d_Domain                                    myDomain1;
  IntRes2d_Domain                                    myDomain2;
  TopoDS_Edge                                        myEdge1;
  TopoDS_Edge                                        myEdge2;
  Geom2dAdaptor_Curve                                myCurve1;
  Geom2dAdaptor_Curve                                myCurve2;
  double                                             myTol1;
  double                                             myTol2;
  bool                                               myTolForced;
  Geom2dInt_GInter                                   myIntersector;
  NCollection_Sequence<IntRes2d_IntersectionPoint>   mylpnt;
  NCollection_Sequence<IntRes2d_IntersectionSegment> mylseg;
  int                                                myNbPoints;
  int                                                myNbSegments;
  int                                                myTrueNbPoints;
  int                                                myPointIndex;
  int                                                myIsVertexPointIndex;
  int                                                myIsVertexIndex;
  bool                                               myIsVertexValue;
  TopoDS_Vertex                                      myIsVertexVertex;
  int                                                myDimension;
  bool                                               myHasSegment;
  bool                                               mySameDomain;
  bool                                               myf1surf1F_sameoriented;
  bool                                               myf2surf1F_sameoriented;
  NCollection_Sequence<TopOpeBRep_Point2d>           mysp2d;
  int                                                myip2d;
  int                                                mynp2d;
  bool                                               myselectkeep;
};

#endif // _TopOpeBRep_EdgesIntersector_HeaderFile
