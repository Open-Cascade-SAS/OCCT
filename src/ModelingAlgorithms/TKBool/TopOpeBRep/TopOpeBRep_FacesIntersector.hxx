// Created on: 1994-10-11
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

#ifndef _TopOpeBRep_FacesIntersector_HeaderFile
#define _TopOpeBRep_FacesIntersector_HeaderFile

#include <BRepAdaptor_Surface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <IntPatch_Intersection.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopOpeBRep_LineInter.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedMap.hxx>

class BRepTopAdaptor_TopolTool;
class Bnd_Box;

//! Describes the intersection of two faces.
class TopOpeBRep_FacesIntersector
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopOpeBRep_FacesIntersector();

  //! Computes the intersection of faces S1 and S2.
  Standard_EXPORT void Perform(const TopoDS_Shape& S1, const TopoDS_Shape& S2);

  //! Computes the intersection of faces S1 and S2.
  Standard_EXPORT void Perform(const TopoDS_Shape& S1,
                               const TopoDS_Shape& S2,
                               const Bnd_Box&      B1,
                               const Bnd_Box&      B2);

  Standard_EXPORT bool IsEmpty();

  Standard_EXPORT bool IsDone() const;

  //! Returns True if Perform() arguments are two faces with the
  //! same surface.
  Standard_EXPORT bool SameDomain() const;

  //! returns first or second intersected face.
  Standard_EXPORT const TopoDS_Shape& Face(const int Index) const;

  //! Returns True if Perform() arguments are two faces
  //! SameDomain() and normals on both side.
  //! Raise if SameDomain is False
  Standard_EXPORT bool SurfacesSameOriented() const;

  //! returns true if edge <E> is found as same as the edge
  //! associated with a RESTRICTION line.
  Standard_EXPORT bool IsRestriction(const TopoDS_Shape& E) const;

  //! returns the map of edges found as TopeBRepBRep_RESTRICTION
  Standard_EXPORT const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& Restrictions() const;

  Standard_EXPORT void PrepareLines();

  Standard_EXPORT occ::handle<NCollection_HArray1<TopOpeBRep_LineInter>> Lines();

  Standard_EXPORT int NbLines() const;

  Standard_EXPORT void InitLine();

  Standard_EXPORT bool MoreLine() const;

  Standard_EXPORT void NextLine();

  Standard_EXPORT TopOpeBRep_LineInter& CurrentLine();

  Standard_EXPORT int CurrentLineIndex() const;

  Standard_EXPORT TopOpeBRep_LineInter& ChangeLine(const int IL);

  //! Force the tolerance values used by the next Perform(S1,S2) call.
  Standard_EXPORT void ForceTolerances(const double tolarc, const double toltang);

  //! Return the tolerance values used in the last Perform() call
  //! If ForceTolerances() has been called, return the given values.
  //! If not, return values extracted from shapes.
  Standard_EXPORT void GetTolerances(double& tolarc, double& toltang) const;

private:
  Standard_EXPORT void FindLine();

  Standard_EXPORT void ResetIntersection();

  //! extract tolerance values from shapes <S1>,<S2>,
  //! in order to perform intersection between <S1> and <S2>
  //! with tolerance values "fitting" the shape tolerances.
  //! (called by Perform() by default, when ForceTolerances() has not
  //! been called)
  Standard_EXPORT void ShapeTolerances(const TopoDS_Shape& S1, const TopoDS_Shape& S2);

  //! returns the max tolerance of sub-shapes of type <T>
  //! found in shape <S>. If no such sub-shape found, return
  //! Precision::Intersection()
  //! (called by ShapeTolerances())
  Standard_EXPORT double ToleranceMax(const TopoDS_Shape& S, const TopAbs_ShapeEnum T) const;

  IntPatch_Intersection                 myIntersector;
  bool                      myIntersectionDone;
  double                         myTol1;
  double                         myTol2;
  bool                      myForceTolerances;
  occ::handle<NCollection_HArray1<TopOpeBRep_LineInter>> myHAL;
  TopOpeBRep_LineInter                  myLine;
  int                      myLineIndex;
  bool                      myLineFound;
  int                      myLineNb;
  TopoDS_Face                           myFace1;
  TopoDS_Face                           myFace2;
  occ::handle<BRepAdaptor_Surface>           mySurface1;
  occ::handle<BRepAdaptor_Surface>           mySurface2;
  GeomAbs_SurfaceType                   mySurfaceType1;
  GeomAbs_SurfaceType                   mySurfaceType2;
  bool                      mySurfacesSameOriented;
  occ::handle<BRepTopAdaptor_TopolTool>      myDomain1;
  occ::handle<BRepTopAdaptor_TopolTool>      myDomain2;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>            myEdgeRestrictionMap;
  TopoDS_Shape                          myNullShape;
};

#endif // _TopOpeBRep_FacesIntersector_HeaderFile
