// Created on: 1996-08-30
// Created by: Yves FRICAUD
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _BRepOffset_Inter3d_HeaderFile
#define _BRepOffset_Inter3d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopAbs_State.hxx>
#include <Standard_Real.hxx>
#include <TopTools_ListOfShape.hxx>
#include <BRepOffset_DataMapOfShapeOffset.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <Standard_Boolean.hxx>
class BRepAlgo_AsDes;
class BRepAlgo_Image;
class TopoDS_Face;
class TopoDS_Shape;
class BRepOffset_Analyse;


//! Computes the intersection face face in a set of faces
//! Store the result in a SD as AsDes.
class BRepOffset_Inter3d 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepOffset_Inter3d(const Handle(BRepAlgo_AsDes)& AsDes, const TopAbs_State Side, const Standard_Real Tol);
  
  Standard_EXPORT void CompletInt (const TopTools_ListOfShape& SetOfFaces, const BRepAlgo_Image& InitOffsetFace);
  
  Standard_EXPORT void FaceInter (const TopoDS_Face& F1, const TopoDS_Face& F2, const BRepAlgo_Image& InitOffsetFace);
  
  Standard_EXPORT void ConnexIntByArc (const TopTools_ListOfShape& SetOfFaces, const TopoDS_Shape& ShapeInit, const BRepOffset_Analyse& Analyse, const BRepAlgo_Image& InitOffsetFace);
  
  Standard_EXPORT void ConnexIntByInt (const TopoDS_Shape& SI, const BRepOffset_DataMapOfShapeOffset& MapSF, const BRepOffset_Analyse& A, TopTools_DataMapOfShapeShape& MES, TopTools_DataMapOfShapeShape& Build, TopTools_ListOfShape& Failed, const Standard_Boolean bIsPlanar = Standard_False);
  
  Standard_EXPORT void ContextIntByInt (const TopTools_IndexedMapOfShape& ContextFaces, const Standard_Boolean ExtentContext, const BRepOffset_DataMapOfShapeOffset& MapSF, const BRepOffset_Analyse& A, TopTools_DataMapOfShapeShape& MES, TopTools_DataMapOfShapeShape& Build, TopTools_ListOfShape& Failed, const Standard_Boolean bIsPlanar = Standard_False);
  
  Standard_EXPORT void ContextIntByArc (const TopTools_IndexedMapOfShape& ContextFaces, const Standard_Boolean ExtentContext, const BRepOffset_Analyse& Analyse, const BRepAlgo_Image& InitOffsetFace, BRepAlgo_Image& InitOffsetEdge);
  
  Standard_EXPORT void AddCommonEdges (const TopTools_ListOfShape& SetOfFaces);
  
  Standard_EXPORT void SetDone (const TopoDS_Face& F1, const TopoDS_Face& F2);
  
  Standard_EXPORT Standard_Boolean IsDone (const TopoDS_Face& F1, const TopoDS_Face& F2) const;
  
  Standard_EXPORT TopTools_IndexedMapOfShape& TouchedFaces();
  
  Standard_EXPORT Handle(BRepAlgo_AsDes) AsDes() const;
  
  Standard_EXPORT TopTools_IndexedMapOfShape& NewEdges();




protected:





private:

  
  Standard_EXPORT void Store (const TopoDS_Face& F1, const TopoDS_Face& F2, const TopTools_ListOfShape& LInt1, const TopTools_ListOfShape& LInt2);


  Handle(BRepAlgo_AsDes) myAsDes;
  TopTools_IndexedMapOfShape myTouched;
  TopTools_DataMapOfShapeListOfShape myDone;
  TopTools_IndexedMapOfShape myNewEdges;
  TopAbs_State mySide;
  Standard_Real myTol;


};







#endif // _BRepOffset_Inter3d_HeaderFile
