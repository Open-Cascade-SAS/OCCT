// Created on: 1998-08-21
// Created by: Yves FRICAUD
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <TopOpeBRepDS_GapTool.ixx>
#include <TopOpeBRepDS_ListIteratorOfListOfInterference.hxx>
#include <Standard_OutOfRange.hxx>
#include <TopOpeBRepDS_CurvePointInterference.hxx>

#ifdef DEB
Standard_IMPORT Standard_Boolean TopOpeBRepDS_GettraceGAP();
#endif

//=======================================================================
//function : TopOpeBRepDS_GapTool
//purpose  : 
//=======================================================================

TopOpeBRepDS_GapTool::TopOpeBRepDS_GapTool()
{
}

//=======================================================================
//function : TopOpeBRepDS_GapTool
//purpose  : 
//=======================================================================

TopOpeBRepDS_GapTool::TopOpeBRepDS_GapTool(const Handle(TopOpeBRepDS_HDataStructure)& HDS)
{
  Init(HDS);
}

//=======================================================================
//function : StoreGToI
//purpose  : 
//=======================================================================

static void StoreGToI(TopOpeBRepDS_DataMapOfIntegerListOfInterference& GToI,
		      const Handle(TopOpeBRepDS_Interference)&         I)
{
  Standard_Integer G = I->Geometry();
  if (!GToI.IsBound(G)) {
    TopOpeBRepDS_ListOfInterference empty;
    GToI.Bind(G,empty);
  }
  GToI(G).Append(I);
}
//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void TopOpeBRepDS_GapTool::Init(const Handle(TopOpeBRepDS_HDataStructure)& HDS) 
{
  myHDS = HDS;
  Standard_Integer i,Nb = myHDS->NbShapes();
  for (i = 1; i <= Nb; i++) {
    const TopoDS_Shape& S = myHDS->Shape(i);
    const TopOpeBRepDS_ListOfInterference& LI = myHDS->DS().ShapeInterferences(S);
    for (TopOpeBRepDS_ListIteratorOfListOfInterference it(LI); it.More(); it.Next()) {
      if (it.Value()->GeometryType() == TopOpeBRepDS_POINT) { 
	myInterToShape.Bind(it.Value(),S);
	StoreGToI(myGToI,it.Value());
      }
    }
  }
  Standard_Integer NbCurves = myHDS->NbCurves();
  for (i = 1; i <= NbCurves; i++) {
    TopOpeBRepDS_ListOfInterference& LI = myHDS->ChangeDS().ChangeCurveInterferences(i);
    for (TopOpeBRepDS_ListIteratorOfListOfInterference it(LI); it.More(); it.Next()) {
      if (it.Value()->GeometryType() == TopOpeBRepDS_POINT) 
	StoreGToI(myGToI,it.Value());
    }
  }
}

//=======================================================================
//function :Curve 
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_GapTool::Curve(const Handle(TopOpeBRepDS_Interference)& I,
					       TopOpeBRepDS_Curve& C) const
{
  if (I->GeometryType() == TopOpeBRepDS_POINT) {
    TopOpeBRepDS_Kind GK,SK;
    Standard_Integer  G,S;
    
    I->GKGSKS(GK,G,SK,S);
    if (SK == TopOpeBRepDS_CURVE) {
      C = myHDS->Curve(S);
      return 1;
    }
    const TopOpeBRepDS_ListOfInterference& LI = myGToI(G);
    for (TopOpeBRepDS_ListIteratorOfListOfInterference it(LI); it.More(); it.Next()) {
      it.Value()->GKGSKS(GK,G,SK,S);
      if (SK == TopOpeBRepDS_CURVE) {
	C = myHDS->Curve(S);
	return 1;
      }
    }
  }
#ifdef DEB 
  if (TopOpeBRepDS_GettraceGAP()) {
    cout <<"TopOpeBRepDS_GapTool::Curve Point "<<I->Geometry()<<" -> Pas sur Courbe "<<endl;
  }
#endif 
  return 0;
}


//=======================================================================
//function :Interferences
//purpose  :
//=======================================================================

const TopOpeBRepDS_ListOfInterference& TopOpeBRepDS_GapTool::Interferences(const Standard_Integer IP) const
{
  return myGToI(IP);
}


//=======================================================================
//function : SameInterferences
//purpose  : 
//=======================================================================

const TopOpeBRepDS_ListOfInterference& TopOpeBRepDS_GapTool::SameInterferences
(const Handle(TopOpeBRepDS_Interference)& I) const
{
  return myGToI(I->Geometry());
}


//=======================================================================
//function : ChangeSameInterferences
//purpose  : 
//=======================================================================

TopOpeBRepDS_ListOfInterference& TopOpeBRepDS_GapTool::ChangeSameInterferences
(const Handle(TopOpeBRepDS_Interference)& I) 
{
  return myGToI.ChangeFind (I->Geometry());
}


//=======================================================================
//function : ShapeSupport
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_GapTool::EdgeSupport(const Handle(TopOpeBRepDS_Interference)& I,
						     TopoDS_Shape&                            E) const
{
  if (I->GeometryType() == TopOpeBRepDS_POINT) { 
    if (myInterToShape.IsBound(I)) {
      const TopoDS_Shape& S = myInterToShape(I);
      if (S.ShapeType() == TopAbs_EDGE) {
	E = S;
	return 1;
      }
    }
    const TopOpeBRepDS_ListOfInterference& LI = myGToI(I->Geometry());
    for (TopOpeBRepDS_ListIteratorOfListOfInterference it(LI); it.More(); it.Next()) {
      const Handle(TopOpeBRepDS_Interference)& II = it.Value();
      if (myInterToShape.IsBound(II)) {
	const TopoDS_Shape& S = myInterToShape(II);
	if (S.ShapeType() == TopAbs_EDGE) {
	  E = S;
	  return 1;
	}
      }
    }
  }
  return 0;
}


//=======================================================================
//function : FacesSupport
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_GapTool::FacesSupport(const Handle(TopOpeBRepDS_Interference)& I,
						      TopoDS_Shape& F1,
						      TopoDS_Shape& F2) const
{
  TopOpeBRepDS_Curve C;
  if (Curve(I,C)) {
    C.GetShapes(F1,F2);
    return 1;
  }
  return 0;
}


//=======================================================================
//function : ParameterOnEdge
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_GapTool::ParameterOnEdge(const Handle(TopOpeBRepDS_Interference)& I,
							 const TopoDS_Shape& E,
							 Standard_Real&      U) const
{
  if (I->GeometryType() == TopOpeBRepDS_POINT) { 
    if (myInterToShape.IsBound(I)) {
      const TopoDS_Shape& S = myInterToShape(I);
      if (S.IsSame(E)) {
	U = (*((Handle(TopOpeBRepDS_CurvePointInterference)*)&I))->Parameter();
	return 1;
      }
    }
    const TopOpeBRepDS_ListOfInterference& LI = myGToI(I->Geometry());
    for (TopOpeBRepDS_ListIteratorOfListOfInterference it(LI); it.More(); it.Next()) {
      const Handle(TopOpeBRepDS_Interference)& II = it.Value();
      if (myInterToShape.IsBound(II)) {
	const TopoDS_Shape& S = myInterToShape(II);
	if (S.IsSame(E)) {
	  U = (*((Handle(TopOpeBRepDS_CurvePointInterference)*)&II))->Parameter();
	  return 1;
	}	
      }
    }
  }
  return 0;
}

//=======================================================================
//function : ParameterOnEdge
//purpose  : 
//=======================================================================

void TopOpeBRepDS_GapTool::SetParameterOnEdge(const Handle(TopOpeBRepDS_Interference)& I,
						const TopoDS_Shape&       E,
						const Standard_Real       U) 
{
  if (I->GeometryType() == TopOpeBRepDS_POINT) { 
    if (myInterToShape.IsBound(I)) {
      const TopoDS_Shape& S = myInterToShape(I);
      if (S.IsSame(E)) {
	(*((Handle(TopOpeBRepDS_CurvePointInterference)*)&I))->Parameter(U);
      }
    }
    const TopOpeBRepDS_ListOfInterference& LI = myGToI(I->Geometry());
    for (TopOpeBRepDS_ListIteratorOfListOfInterference it(LI); it.More(); it.Next()) {
      const Handle(TopOpeBRepDS_Interference)& II = it.Value();
      if (myInterToShape.IsBound(II)) {
	const TopoDS_Shape& S = myInterToShape(II);
	if (S.IsSame(E)) {
	  (*((Handle(TopOpeBRepDS_CurvePointInterference)*)&II))->Parameter(U);
	}	
      }
    }
  }
}

//=======================================================================
//function : SetPoint
//purpose  : 
//=======================================================================

void TopOpeBRepDS_GapTool::SetPoint(const Handle(TopOpeBRepDS_Interference)& I,
				      const Standard_Integer            IP)
{
  if (IP != I->Geometry()) {
    const TopOpeBRepDS_ListOfInterference& LI = myGToI(I->Geometry());
    for (TopOpeBRepDS_ListIteratorOfListOfInterference it(LI); it.More(); it.Next()) {
      Handle(TopOpeBRepDS_Interference) II = it.Value();
      II->Geometry(IP);  
      StoreGToI(myGToI,II);
    }
  }
}


