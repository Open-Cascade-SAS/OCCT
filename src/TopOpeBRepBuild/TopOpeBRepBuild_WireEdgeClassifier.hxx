// Created on: 1993-06-17
// Created by: Jean Yves LEBEY
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

#ifndef _TopOpeBRepBuild_WireEdgeClassifier_HeaderFile
#define _TopOpeBRepBuild_WireEdgeClassifier_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <gp_Pnt2d.hxx>
#include <BRepClass_Edge.hxx>
#include <BRepClass_FacePassiveClassifier.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepBuild_CompositeClassifier.hxx>
#include <TopAbs_State.hxx>
class TopOpeBRepBuild_BlockBuilder;
class TopOpeBRepBuild_Loop;



//! Classify edges and wires.
//! shapes are Wires, Element are Edge.
class TopOpeBRepBuild_WireEdgeClassifier  : public TopOpeBRepBuild_CompositeClassifier
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a classifier on edge <F>.
  //! Used to compare edges and wires on the edge <F>.
  Standard_EXPORT TopOpeBRepBuild_WireEdgeClassifier(const TopoDS_Shape& F, const TopOpeBRepBuild_BlockBuilder& BB);
  
  Standard_EXPORT virtual TopAbs_State Compare (const Handle(TopOpeBRepBuild_Loop)& L1, const Handle(TopOpeBRepBuild_Loop)& L2) Standard_OVERRIDE;
  
  Standard_EXPORT TopoDS_Shape LoopToShape (const Handle(TopOpeBRepBuild_Loop)& L);
  
  //! classify wire <B1> with wire <B2>
  Standard_EXPORT TopAbs_State CompareShapes (const TopoDS_Shape& B1, const TopoDS_Shape& B2) Standard_OVERRIDE;
  
  //! classify edge <E> with wire <B>
  Standard_EXPORT TopAbs_State CompareElementToShape (const TopoDS_Shape& E, const TopoDS_Shape& B) Standard_OVERRIDE;
  
  //! prepare classification involving wire <B>
  //! calls ResetElement on first edge of <B>
  Standard_EXPORT void ResetShape (const TopoDS_Shape& B) Standard_OVERRIDE;
  
  //! prepare classification involving edge <E>
  //! define 2D point (later used in Compare()) on first vertex of edge <E>.
  Standard_EXPORT void ResetElement (const TopoDS_Shape& E) Standard_OVERRIDE;
  
  //! Add the edge <E> in the set of edges used in 2D point
  //! classification.
  Standard_EXPORT Standard_Boolean CompareElement (const TopoDS_Shape& E) Standard_OVERRIDE;
  
  //! Returns state of classification of 2D point, defined by
  //! ResetElement, with the current set of edges, defined by Compare.
  Standard_EXPORT TopAbs_State State() Standard_OVERRIDE;




protected:





private:



  Standard_Boolean myFirstCompare;
  gp_Pnt2d myPoint2d;
  BRepClass_Edge myBCEdge;
  BRepClass_FacePassiveClassifier myFPC;
  TopoDS_Shape myShape;


};







#endif // _TopOpeBRepBuild_WireEdgeClassifier_HeaderFile
