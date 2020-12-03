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
#include <TopOpeBRep_HArray1OfLineInter.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

class BRepTopAdaptor_TopolTool;
class Bnd_Box;

//! Describes the intersection of two faces.
class TopOpeBRep_FacesIntersector 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRep_FacesIntersector();
  
  //! Computes the intersection of faces S1 and S2.
  Standard_EXPORT void Perform (const TopoDS_Shape& S1, const TopoDS_Shape& S2);
  
  //! Computes the intersection of faces S1 and S2.
  Standard_EXPORT void Perform (const TopoDS_Shape& S1, const TopoDS_Shape& S2, const Bnd_Box& B1, const Bnd_Box& B2);
  
  Standard_EXPORT Standard_Boolean IsEmpty();
  
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns True if Perform() arguments are two faces with the
  //! same surface.
  Standard_EXPORT Standard_Boolean SameDomain() const;
  
  //! returns first or second intersected face.
  Standard_EXPORT const TopoDS_Shape& Face (const Standard_Integer Index) const;
  
  //! Returns True if Perform() arguments are two faces
  //! SameDomain() and normals on both side.
  //! Raise if SameDomain is False
  Standard_EXPORT Standard_Boolean SurfacesSameOriented() const;
  
  //! returns true if edge <E> is found as same as the edge
  //! associated with a RESTRICTION line.
  Standard_EXPORT Standard_Boolean IsRestriction (const TopoDS_Shape& E) const;
  
  //! returns the map of edges found as TopeBRepBRep_RESTRICTION
  Standard_EXPORT const TopTools_IndexedMapOfShape& Restrictions() const;
  
  Standard_EXPORT void PrepareLines();
  
  Standard_EXPORT Handle(TopOpeBRep_HArray1OfLineInter) Lines();
  
  Standard_EXPORT Standard_Integer NbLines() const;
  
  Standard_EXPORT void InitLine();
  
  Standard_EXPORT Standard_Boolean MoreLine() const;
  
  Standard_EXPORT void NextLine();
  
  Standard_EXPORT TopOpeBRep_LineInter& CurrentLine();
  
  Standard_EXPORT Standard_Integer CurrentLineIndex() const;
  
  Standard_EXPORT TopOpeBRep_LineInter& ChangeLine (const Standard_Integer IL);
  

  //! Force the tolerance values used by the next Perform(S1,S2) call.
  Standard_EXPORT void ForceTolerances (const Standard_Real tolarc, const Standard_Real toltang);
  

  //! Return the tolerance values used in the last Perform() call
  //! If ForceTolerances() has been called, return the given values.
  //! If not, return values extracted from shapes.
  Standard_EXPORT void GetTolerances (Standard_Real& tolarc, Standard_Real& toltang) const;




protected:





private:

  
  Standard_EXPORT void FindLine();
  
  Standard_EXPORT void ResetIntersection();
  
  //! extract tolerance values from shapes <S1>,<S2>,
  //! in order to perform intersection between <S1> and <S2>
  //! with tolerance values "fitting" the shape tolerances.
  //! (called by Perform() by default, when ForceTolerances() has not
  //! been called)
  Standard_EXPORT void ShapeTolerances (const TopoDS_Shape& S1, const TopoDS_Shape& S2);
  
  //! returns the max tolerance of sub-shapes of type <T>
  //! found in shape <S>. If no such sub-shape found, return
  //! Precision::Intersection()
  //! (called by ShapeTolerances())
  Standard_EXPORT Standard_Real ToleranceMax (const TopoDS_Shape& S, const TopAbs_ShapeEnum T) const;


  IntPatch_Intersection myIntersector;
  Standard_Boolean myIntersectionDone;
  Standard_Real myTol1;
  Standard_Real myTol2;
  Standard_Boolean myForceTolerances;
  Handle(TopOpeBRep_HArray1OfLineInter) myHAL;
  TopOpeBRep_LineInter myLine;
  Standard_Integer myLineIndex;
  Standard_Boolean myLineFound;
  Standard_Integer myLineNb;
  TopoDS_Face myFace1;
  TopoDS_Face myFace2;
  Handle(BRepAdaptor_Surface) mySurface1;
  Handle(BRepAdaptor_Surface) mySurface2;
  GeomAbs_SurfaceType mySurfaceType1;
  GeomAbs_SurfaceType mySurfaceType2;
  Standard_Boolean mySurfacesSameOriented;
  Handle(BRepTopAdaptor_TopolTool) myDomain1;
  Handle(BRepTopAdaptor_TopolTool) myDomain2;
  TopTools_IndexedMapOfShape myEdgeRestrictionMap;
  TopoDS_Shape myNullShape;


};







#endif // _TopOpeBRep_FacesIntersector_HeaderFile
