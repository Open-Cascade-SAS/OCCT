// Created on: 2000-01-25
// Created by: Peter KURNEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _TopOpeBRepBuild_CorrectFace2d_HeaderFile
#define _TopOpeBRepBuild_CorrectFace2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Face.hxx>
#include <Standard_Integer.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_IndexedMapOfOrientedShape.hxx>
#include <TopTools_IndexedDataMapOfShapeShape.hxx>
class TopoDS_Edge;
class gp_Pnt2d;
class TopoDS_Shape;
class gp_Vec2d;
class Geom2d_Curve;
class Bnd_Box2d;



class TopOpeBRepBuild_CorrectFace2d 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRepBuild_CorrectFace2d();
  
  Standard_EXPORT TopOpeBRepBuild_CorrectFace2d(const TopoDS_Face& aFace, const TopTools_IndexedMapOfOrientedShape& anAvoidMap, TopTools_IndexedDataMapOfShapeShape& aMap);
  
  Standard_EXPORT const TopoDS_Face& Face() const;
  
  Standard_EXPORT void Perform();
  
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  Standard_EXPORT Standard_Integer ErrorStatus() const;
  
  Standard_EXPORT const TopoDS_Face& CorrectedFace() const;
  
  Standard_EXPORT void SetMapOfTrans2dInfo (TopTools_IndexedDataMapOfShapeShape& aMap);
  
  Standard_EXPORT TopTools_IndexedDataMapOfShapeShape& MapOfTrans2dInfo();
  
  Standard_EXPORT static void GetP2dFL (const TopoDS_Face& aFace, const TopoDS_Edge& anEdge, gp_Pnt2d& P2dF, gp_Pnt2d& P2dL);
  
  Standard_EXPORT static void CheckList (const TopoDS_Face& aFace, TopTools_ListOfShape& aHeadList);




protected:





private:

  
  Standard_EXPORT void CheckFace();
  
  Standard_EXPORT Standard_Integer MakeRightWire();
  
  Standard_EXPORT void MakeHeadList (const TopoDS_Shape& aFirstEdge, TopTools_ListOfShape& aHeadList) const;
  
  Standard_EXPORT void TranslateCurve2d (const TopoDS_Edge& anEdge, const TopoDS_Face& aFace, const gp_Vec2d& aTranslateVec, Handle(Geom2d_Curve)& aCurve2d);
  
  Standard_EXPORT Standard_Integer OuterWire (TopoDS_Wire& anOuterWire) const;
  
  Standard_EXPORT void BndBoxWire (const TopoDS_Wire& aWire, Bnd_Box2d& aB2d) const;
  
  Standard_EXPORT void MoveWire2d (TopoDS_Wire& aWire, const gp_Vec2d& aTrV);
  
  Standard_EXPORT void MoveWires2d (TopoDS_Wire& aWire);
  
  Standard_EXPORT void UpdateEdge (const TopoDS_Edge& E, const Handle(Geom2d_Curve)& C, const TopoDS_Face& F, const Standard_Real Tol);
  
  Standard_EXPORT void UpdateEdge (const TopoDS_Edge& E, const Handle(Geom2d_Curve)& C1, const Handle(Geom2d_Curve)& C2, const TopoDS_Face& F, const Standard_Real Tol);
  
  Standard_EXPORT void BuildCopyData (const TopoDS_Face& F, const TopTools_IndexedMapOfOrientedShape& anAvoidMap, TopoDS_Face& aCopyFace, TopTools_IndexedMapOfOrientedShape& aCopyAvoidMap, const Standard_Boolean aNeedToUsePMap);
  
  Standard_EXPORT Standard_Integer ConnectWire (TopoDS_Face& aCopyFace, const TopTools_IndexedMapOfOrientedShape& aCopyAvoidMap, const Standard_Boolean aTryToConnectFlag);


  TopoDS_Face myFace;
  TopoDS_Face myCorrectedFace;
  Standard_Boolean myIsDone;
  Standard_Integer myErrorStatus;
  Standard_Real myFaceTolerance;
  TopoDS_Wire myCurrentWire;
  TopTools_ListOfShape myOrderedWireList;
  TopTools_IndexedMapOfOrientedShape myAvoidMap;
  Standard_Address myMap;
  TopoDS_Face myCopyFace;
  TopTools_IndexedMapOfOrientedShape myCopyAvoidMap;
  TopTools_IndexedDataMapOfShapeShape myEdMapInversed;


};







#endif // _TopOpeBRepBuild_CorrectFace2d_HeaderFile
