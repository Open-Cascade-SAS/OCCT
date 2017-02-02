// Created on: 1993-07-12
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


#include <Bnd_Box.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Surface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Pln.hxx>
#include <Standard_ProgramError.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepTool_box.hxx>
#include <TopOpeBRepTool_BoxSort.hxx>
#include <TopOpeBRepTool_define.hxx>
#include <TopOpeBRepTool_HBoxTool.hxx>

#ifdef OCCT_DEBUG
#define TBOX TopOpeBRepTool_GettraceBOX()
#endif

#define MTOhbt Handle(TopOpeBRepTool_HBoxTool)
#define MTClioloi TColStd_ListIteratorOfListOfInteger

//=======================================================================
//function : TopOpeBRepTool_BoxSort
//purpose  : 
//=======================================================================
TopOpeBRepTool_BoxSort::TopOpeBRepTool_BoxSort()
{
}

//=======================================================================
//function : TopOpeBRepTool_BoxSort
//purpose  : 
//=======================================================================
TopOpeBRepTool_BoxSort::TopOpeBRepTool_BoxSort(const MTOhbt& HBT)
{
  SetHBoxTool(HBT);
}

//modified by NIZNHY-PKV Mon Dec 16 10:26:00 2002 f
//=======================================================================
//function : ~TopOpeBRepTool_BoxSort
//purpose  : 
//=======================================================================
TopOpeBRepTool_BoxSort::~TopOpeBRepTool_BoxSort()
{
  if (!myHBT.IsNull()) {
    myHBT->Clear();
  }
}
//modified by NIZNHY-PKV Mon Dec 16 10:26:02 2002 t

//=======================================================================
//function : SetHBoxTool
//purpose  : 
//=======================================================================
void TopOpeBRepTool_BoxSort::SetHBoxTool(const MTOhbt& HBT)
{
  myHBT = HBT;
}

//=======================================================================
//function : HBoxTool
//purpose  : 
//=======================================================================
const MTOhbt& TopOpeBRepTool_BoxSort::HBoxTool() const
{
  return myHBT;
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void TopOpeBRepTool_BoxSort::Clear()
{
  myCOB.SetVoid();
//  myHAB.Nullify();
//  myHAI.Nullify();
}

//=======================================================================
//function : AddBoxes
//purpose  : 
//=======================================================================
void TopOpeBRepTool_BoxSort::AddBoxes(const TopoDS_Shape& S,const TopAbs_ShapeEnum TS,const TopAbs_ShapeEnum TA)
{
  if (myHBT.IsNull()) myHBT = new TopOpeBRepTool_HBoxTool();
  myHBT->AddBoxes(S,TS,TA);
}  

//=======================================================================
//function : MakeHAB
//purpose  : 
//=======================================================================
void TopOpeBRepTool_BoxSort::MakeHAB(const TopoDS_Shape& S,const TopAbs_ShapeEnum TS,const TopAbs_ShapeEnum TA)
{
#ifdef OCCT_DEBUG
  TopAbs_ShapeEnum t =
#endif
                       S.ShapeType();
  Standard_Integer n = 0; TopExp_Explorer ex;
  for (ex.Init(S,TS,TA);ex.More();ex.Next()) n++;

  myHAB = new Bnd_HArray1OfBox(0,n);
  Bnd_Array1OfBox& AB = myHAB->ChangeArray1();
  myHAI = new TColStd_HArray1OfInteger(0,n);
  TColStd_Array1OfInteger& AI = myHAI->ChangeArray1();
  
  Standard_Integer i = 0;
  for (ex.Init(S,TS,TA);ex.More();ex.Next()) {
    i++;
    const TopoDS_Shape& ss = ex.Current();    
    Standard_Boolean hb = myHBT->HasBox(ss);
    if (!hb) myHBT->AddBox(ss);
    Standard_Integer im = myHBT->Index(ss);
    const Bnd_Box& B = myHBT->Box(ss);
    AI.ChangeValue(i) = im;
    AB.ChangeValue(i) = B;
  }

#ifdef OCCT_DEBUG
  if (TBOX) {
    cout<<"# BS::MakeHAB : ";TopAbs::Print(t,cout);cout<<" : "<<n<<"\n";
    cout.flush();
  }
#endif

}

//=======================================================================
//function : HAB
//purpose  : 
//=======================================================================
const Handle(Bnd_HArray1OfBox)& TopOpeBRepTool_BoxSort::HAB() const
{
  return myHAB;
}

//=======================================================================
//function : MakeHABCOB
//purpose  : 
//=======================================================================
void TopOpeBRepTool_BoxSort::MakeHABCOB(const Handle(Bnd_HArray1OfBox)& HAB,
					Bnd_Box& COB)
{
  COB.SetVoid();
  Standard_Integer n = HAB->Upper();
  const Bnd_Array1OfBox& AB = HAB->Array1();
  for (Standard_Integer i = 1; i <= n; i++) {
    const Bnd_Box& B = AB(i);
    COB.Add(B);
  }
}

//=======================================================================
//function : HABShape
//purpose  : 
//=======================================================================
const TopoDS_Shape& TopOpeBRepTool_BoxSort::HABShape(const Standard_Integer I) const
{
  Standard_Integer iu = myHAI->Upper();
  Standard_Boolean b = (I >= 1 && I <= iu);
  if (!b) {
    throw Standard_ProgramError("BS::Box3");
  }
  Standard_Integer im = myHAI->Value(I);
  const TopoDS_Shape& S = myHBT->Shape(im);
  return S;
}

//=======================================================================
//function : MakeCOB
//purpose  : 
//=======================================================================
void TopOpeBRepTool_BoxSort::MakeCOB(const TopoDS_Shape& S,const TopAbs_ShapeEnum TS,const TopAbs_ShapeEnum TA)
{
  MakeHAB(S,TS,TA);
  MakeHABCOB(myHAB,myCOB);
  myBSB.Initialize(myCOB,myHAB);
#ifdef OCCT_DEBUG
  if (TBOX) {myHBT->DumpB(myCOB);cout<<";# BS::MakeCOB"<<endl;}
#endif
}

//=======================================================================
//function : AddBoxesMakeCOB
//purpose  : 
//=======================================================================
void TopOpeBRepTool_BoxSort::AddBoxesMakeCOB(const TopoDS_Shape& S,const TopAbs_ShapeEnum TS,const TopAbs_ShapeEnum TA)
{
  AddBoxes(S,TS,TA);
  MakeCOB(S,TS,TA);
}

//=======================================================================
//function : Compare
//purpose  : 
//=======================================================================
const MTClioloi& TopOpeBRepTool_BoxSort::Compare(const TopoDS_Shape &S)
{
  if ( myHBT.IsNull() ) myHBT = new TopOpeBRepTool_HBoxTool();
  
  gp_Pln P;
  Standard_Boolean isPlane = Standard_False;
  TopAbs_ShapeEnum t = S.ShapeType();
  Standard_Boolean hasb = myHBT->HasBox(S);
  if (!hasb) myHBT->AddBox(S);

  myLastCompareShape = S;
  myLastCompareShapeBox.SetVoid();
  
  if ( t == TopAbs_FACE) {
    const TopoDS_Face& F = TopoDS::Face(S);
    Standard_Boolean natu = BRep_Tool::NaturalRestriction(F);
    if (natu) {
      Handle(Geom_Surface) surf = BRep_Tool::Surface(F);
      GeomAdaptor_Surface GAS(surf); 
      GeomAbs_SurfaceType suty = GAS.GetType();
      isPlane = (suty == GeomAbs_Plane);
      if (isPlane) P = GAS.Plane();
      else {
	myLastCompareShapeBox = myHBT->Box(F);
      }
    }
    else {
      myLastCompareShapeBox = myHBT->Box(F);
    }
  }
  else if (t == TopAbs_EDGE) {
    const TopoDS_Edge& E = TopoDS::Edge(S);
    TopoDS_Vertex V1,V2; TopExp::Vertices(E,V1,V2);
    Standard_Boolean perso = (V1.IsNull() || V2.IsNull());
    if (perso) {
      myHBT->ComputeBoxOnVertices(E,myLastCompareShapeBox);
    } 
    else {
      myLastCompareShapeBox = myHBT->Box(E);
    }
  }

  const TColStd_ListOfInteger* L;
  if (isPlane) L = &myBSB.Compare(P);
  else L = &myBSB.Compare(myLastCompareShapeBox);
  myIterator.Initialize(*L);
  
#ifdef OCCT_DEBUG
  if (TBOX) {
    Standard_Integer nl = (*L).Extent();
    cout<<"#------------------------"<<endl;
    myHBT->DumpB(myLastCompareShapeBox);cout<<"; # BS::Compare"<<endl;
    cout<<"# touche "<<nl<<" boites ";cout.flush();
    Standard_Integer il;
    for (MTClioloi idd((*L));idd.More();idd.Next()) {
      il=idd.Value();cout<<il<<" ";cout.flush();
    }
    cout<<endl<<"#------------------------"<<endl;
  }
#endif
 
 return myIterator;
}
	
//=======================================================================
//function : TouchedShape
//purpose  : 
//=======================================================================
const TopoDS_Shape& TopOpeBRepTool_BoxSort::TouchedShape(const MTClioloi& LI) const
{
  Standard_Integer icur = LI.Value();
  const TopoDS_Shape& Scur = HABShape(icur);
  return Scur;
}

//=======================================================================
//function : Box
//purpose  : 
//=======================================================================
const Bnd_Box& TopOpeBRepTool_BoxSort::Box(const TopoDS_Shape& S) const
{
  if ( myHBT.IsNull() ) {
    *((MTOhbt*)&myHBT) = new TopOpeBRepTool_HBoxTool();
  }

  if ( myHBT->HasBox(S) ) {
    const Bnd_Box& B = myHBT->Box(S);
    return B;
  }
  else if ( !myLastCompareShape.IsNull() ) {
    if ( S.IsEqual(myLastCompareShape) ) {
      if ( !myLastCompareShapeBox.IsVoid() ) {
	return myLastCompareShapeBox;
      }
    }
  }
  
  const Bnd_Box& B = myHBT->Box(S);
  return B;
}
