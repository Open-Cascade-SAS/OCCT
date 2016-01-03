// Created on: 1993-06-23
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

#ifndef _TopOpeBRep_DSFiller_HeaderFile
#define _TopOpeBRep_DSFiller_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopOpeBRep_ShapeIntersector.hxx>
#include <TopOpeBRep_ShapeIntersector2d.hxx>
#include <TopOpeBRep_FacesFiller.hxx>
#include <TopOpeBRep_EdgesFiller.hxx>
#include <TopOpeBRep_FaceEdgeFiller.hxx>
#include <TopOpeBRepTool_PShapeClassifier.hxx>
#include <Standard_Boolean.hxx>
class TopoDS_Shape;
class TopOpeBRepDS_HDataStructure;
class TopoDS_Face;
class TopOpeBRep_ShapeIntersector;
class TopOpeBRep_ShapeIntersector2d;
class TopOpeBRep_FacesFiller;
class TopOpeBRep_EdgesFiller;
class TopOpeBRep_FaceEdgeFiller;


//! Provides class  methods  to  fill  a datastructure
//! with  results  of intersections.
//!
//! 1.  Use  an    Intersector  to   find    pairs  of
//! intersecting GeomShapes
//!
//! 2. For each  pair fill the DataStructure using the
//! appropriate Filler.
//!
//! 3. Complete the  DataStructure to record shapes to
//! rebuild (shells, wires )
class TopOpeBRep_DSFiller 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRep_DSFiller();
  
  Standard_EXPORT ~TopOpeBRep_DSFiller();
  
  //! return field myPShapeClassifier.
  //! set field myPShapeClassifier.
  Standard_EXPORT TopOpeBRepTool_PShapeClassifier PShapeClassifier() const;
  
  //! Stores in <DS> the intersections of <S1> and <S2>.
  //! if orientFORWARD = True
  //! S FORWARD,REVERSED   --> FORWARD
  //! S EXTERNAL,INTERNAL --> EXTERNAL,INTERNAL
  Standard_EXPORT void Insert (const TopoDS_Shape& S1, const TopoDS_Shape& S2, const Handle(TopOpeBRepDS_HDataStructure)& HDS, const Standard_Boolean orientFORWARD = Standard_True);
  
  //! Stores in <DS> the intersections of <S1> and <S2>.
  //! if orientFORWARD = True
  //! S FORWAR,REVERSED   --> FORWARD
  //! S EXTERNAL,INTERNAL --> EXTERNAL,INTERNAL
  Standard_EXPORT void InsertIntersection (const TopoDS_Shape& S1, const TopoDS_Shape& S2, const Handle(TopOpeBRepDS_HDataStructure)& HDS, const Standard_Boolean orientFORWARD = Standard_True);
  
  Standard_EXPORT void Complete (const Handle(TopOpeBRepDS_HDataStructure)& HDS);
  
  //! Stores in <DS> the intersections of <S1> and <S2>.
  //! S1 et S2 contain only SameDomain Face
  Standard_EXPORT void Insert2d (const TopoDS_Shape& S1, const TopoDS_Shape& S2, const Handle(TopOpeBRepDS_HDataStructure)& HDS);
  
  //! S1, S2 set of tangent face
  //! lance les intersections 2d pour coder correctement
  //! les faces SameDomain.
  Standard_EXPORT void InsertIntersection2d (const TopoDS_Shape& S1, const TopoDS_Shape& S2, const Handle(TopOpeBRepDS_HDataStructure)& HDS);
  
  Standard_EXPORT Standard_Boolean IsMadeOf1d (const TopoDS_Shape& S) const;
  
  Standard_EXPORT Standard_Boolean IsContext1d (const TopoDS_Shape& S) const;
  
  //! Stores in <DS> the intersections of <S1> and <S2>.
  //! S1 and S2 are edges or wires.
  //! S1 edges have a 2d representation in face F1
  //! S2 edges have a 2d representation in face F2
  //! F1 is the face which surface is taken as reference
  //! for 2d description of S1 and S2 edges.
  //! if orientFORWARD = True
  //! S FORWARD,REVERSED  --> FORWARD
  //! S EXTERNAL,INTERNAL --> EXTERNAL,INTERNAL
  Standard_EXPORT void Insert1d (const TopoDS_Shape& S1, const TopoDS_Shape& S2, const TopoDS_Face& F1, const TopoDS_Face& F2, const Handle(TopOpeBRepDS_HDataStructure)& HDS, const Standard_Boolean orientFORWARD = Standard_False);
  
  Standard_EXPORT TopOpeBRep_ShapeIntersector& ChangeShapeIntersector();
  
  Standard_EXPORT TopOpeBRep_ShapeIntersector2d& ChangeShapeIntersector2d();
  
  Standard_EXPORT TopOpeBRep_FacesFiller& ChangeFacesFiller();
  
  Standard_EXPORT TopOpeBRep_EdgesFiller& ChangeEdgesFiller();
  
  Standard_EXPORT TopOpeBRep_FaceEdgeFiller& ChangeFaceEdgeFiller();
  
  Standard_EXPORT void GapFiller (const Handle(TopOpeBRepDS_HDataStructure)& HDS) const;
  
  //! Update   the  data      structure  with   relevant
  //! informations deduced from the intersections.
  //!
  //! Shells containing an intersected face.
  //! Wires  containing an intersected edge.
  Standard_EXPORT void CompleteDS (const Handle(TopOpeBRepDS_HDataStructure)& HDS) const;
  
  Standard_EXPORT void Filter (const Handle(TopOpeBRepDS_HDataStructure)& HDS) const;
  
  Standard_EXPORT void Reducer (const Handle(TopOpeBRepDS_HDataStructure)& HDS) const;
  
  Standard_EXPORT void RemoveUnsharedGeometry (const Handle(TopOpeBRepDS_HDataStructure)& HDS);
  
  Standard_EXPORT void Checker (const Handle(TopOpeBRepDS_HDataStructure)& HDS) const;
  
  //! Update   the  data      structure  with   relevant
  //! informations deduced from the intersections 2d.
  //!
  //! Shells containing an intersected face.
  //! Wires  containing an intersected edge.
  //!
  //! search for interference identity using edge connexity //NYI
  Standard_EXPORT void CompleteDS2d (const Handle(TopOpeBRepDS_HDataStructure)& HDS) const;




protected:





private:

  
  Standard_EXPORT Standard_Boolean CheckInsert (const TopoDS_Shape& S1, const TopoDS_Shape& S2) const;
  
  Standard_EXPORT Standard_Boolean ClearShapeSameDomain (const TopoDS_Shape& S1, const TopoDS_Shape& S2, const Handle(TopOpeBRepDS_HDataStructure)& HDS);


  TopOpeBRep_ShapeIntersector myShapeIntersector;
  TopOpeBRep_ShapeIntersector2d myShapeIntersector2d;
  TopOpeBRep_FacesFiller myFacesFiller;
  TopOpeBRep_EdgesFiller myEdgesFiller;
  TopOpeBRep_FaceEdgeFiller myFaceEdgeFiller;
  TopOpeBRepTool_PShapeClassifier myPShapeClassifier;


};







#endif // _TopOpeBRep_DSFiller_HeaderFile
