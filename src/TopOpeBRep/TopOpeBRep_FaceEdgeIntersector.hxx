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

#ifndef _TopOpeBRep_FaceEdgeIntersector_HeaderFile
#define _TopOpeBRep_FaceEdgeIntersector_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <IntCurveSurface_SequenceOfPnt.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <Standard_Integer.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopAbs_State.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_ShapeEnum.hxx>
class TopoDS_Shape;
class gp_Pnt;
class gp_Pnt2d;
class TopOpeBRepDS_Transition;
class TopoDS_Vertex;


//! Describes the intersection of a face and an edge.
class TopOpeBRep_FaceEdgeIntersector 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRep_FaceEdgeIntersector();
  
  Standard_EXPORT void Perform (const TopoDS_Shape& F, const TopoDS_Shape& E);
  
  Standard_EXPORT Standard_Boolean IsEmpty();
  
  //! returns intersected face or edge according to
  //! value of <Index> = 1 or 2
  Standard_EXPORT const TopoDS_Shape& Shape (const Standard_Integer Index) const;
  

  //! Force the tolerance values used by the next Perform(S1,S2) call.
  Standard_EXPORT void ForceTolerance (const Standard_Real tol);
  

  //! Return the tolerance value used in the last Perform() call
  //! If ForceTolerance() has been called, return the given value.
  //! If not, return value extracted from shapes.
  Standard_EXPORT Standard_Real Tolerance() const;
  
  Standard_EXPORT Standard_Integer NbPoints() const;
  
  Standard_EXPORT void InitPoint();
  
  Standard_EXPORT Standard_Boolean MorePoint() const;
  
  Standard_EXPORT void NextPoint();
  
  //! return the 3D point of the current intersection point.
  Standard_EXPORT gp_Pnt Value() const;
  
  //! parametre de Value() sur l'arete
  Standard_EXPORT Standard_Real Parameter() const;
  
  //! parametre de Value() sur la face
  Standard_EXPORT void UVPoint (gp_Pnt2d& P) const;
  
  //! IN ou ON / a la face. Les points OUT ne sont pas retournes.
  Standard_EXPORT TopAbs_State State() const;
  

  //! Index = 1 transition par rapport a la face, en cheminant sur l'arete
  Standard_EXPORT TopOpeBRepDS_Transition Transition (const Standard_Integer Index, const TopAbs_Orientation FaceOrientation) const;
  
  Standard_EXPORT Standard_Boolean IsVertex (const TopoDS_Shape& S, const gp_Pnt& P, const Standard_Real Tol, TopoDS_Vertex& V);
  
  Standard_EXPORT Standard_Boolean IsVertex (const Standard_Integer I, TopoDS_Vertex& V);
  
  //! trace only
  Standard_EXPORT Standard_Integer Index() const;




protected:





private:

  
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


  TopoDS_Face myFace;
  TopoDS_Edge myEdge;
  Standard_Real myTol;
  Standard_Boolean myForceTolerance;
  GeomAdaptor_Curve myCurve;
  Standard_Boolean myIntersectionDone;
  IntCurveSurface_SequenceOfPnt mySequenceOfPnt;
  TColStd_SequenceOfInteger mySequenceOfState;
  Standard_Integer myPointIndex;
  Standard_Integer myNbPoints;
  TopExp_Explorer myVertexExplorer;
  TopoDS_Shape myNullShape;
  TopoDS_Vertex myNullVertex;


};







#endif // _TopOpeBRep_FaceEdgeIntersector_HeaderFile
