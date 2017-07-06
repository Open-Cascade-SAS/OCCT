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


#include <BRep_Tool.hxx>
#include <Standard_ProgramError.hxx>
#include <Standard_Type.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepDS.hxx>
#include <TopOpeBRepDS_Check.hxx>
#include <TopOpeBRepDS_Curve.hxx>
#include <TopOpeBRepDS_CurveIterator.hxx>
#include <TopOpeBRepDS_DataStructure.hxx>
#include <TopOpeBRepDS_define.hxx>
#include <TopOpeBRepDS_Dumper.hxx>
#include <TopOpeBRepDS_EdgeInterferenceTool.hxx>
#include <TopOpeBRepDS_Filter.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_ListIteratorOfListOfInterference.hxx>
#include <TopOpeBRepDS_ListOfInterference.hxx>
#include <TopOpeBRepDS_Point.hxx>
#include <TopOpeBRepDS_PointIterator.hxx>
#include <TopOpeBRepDS_Reducer.hxx>
#include <TopOpeBRepDS_Surface.hxx>
#include <TopOpeBRepDS_SurfaceCurveInterference.hxx>
#include <TopOpeBRepDS_SurfaceIterator.hxx>
#include <TopOpeBRepDS_Transition.hxx>
#include <TopOpeBRepTool_ShapeExplorer.hxx>
#include <TopTools_ListOfShape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TopOpeBRepDS_HDataStructure,Standard_Transient)

static void FUN_HDS_data(const Handle(TopOpeBRepDS_Interference)& I,
			 TopOpeBRepDS_Kind& GT1,Standard_Integer& G1,
			 TopOpeBRepDS_Kind& ST1,Standard_Integer& S1)
{	
  if (I.IsNull()) return;
  GT1 = I->GeometryType(); G1 = I->Geometry();
  ST1 = I->SupportType();  S1 = I->Support(); 
}


Standard_EXPORT Standard_Boolean FUN_HDS_FACESINTERFER
(const TopoDS_Shape& F1, const TopoDS_Shape& F2,
 const Handle(TopOpeBRepDS_HDataStructure)& HDS)
{
  Standard_Boolean yainterf = Standard_False;
  const TopOpeBRepDS_DataStructure& DS = HDS->DS();
  Standard_Boolean ya = DS.HasShape(F1);
  if ( !ya ) return Standard_False;

//                         DS.Shape(F1);
  Standard_Integer iF2 = DS.Shape(F2);

  const TopOpeBRepDS_ListOfInterference& L1 = DS.ShapeInterferences(F1);
  TopOpeBRepDS_ListIteratorOfListOfInterference itL1(L1);
  for (;itL1.More(); itL1.Next()) {
    const Handle(TopOpeBRepDS_Interference)& I = itL1.Value();
    TopOpeBRepDS_Kind GT = TopOpeBRepDS_UNKNOWN, ST = TopOpeBRepDS_UNKNOWN; Standard_Integer G,S = 0;
    FUN_HDS_data(I,GT,G,ST,S);
    // interference face1/edge/face2
    Standard_Boolean fef = Standard_True;
    fef = fef && (GT==TopOpeBRepDS_EDGE);
    fef = fef && (ST==TopOpeBRepDS_FACE);
    fef = fef && (S==iF2);
    if (fef) {
      yainterf = Standard_True;
      break;
    }
  }
  return yainterf;
}


//=======================================================================
//function : TopOpeBRepDS_HDataStructure
//purpose  : 
//=======================================================================
TopOpeBRepDS_HDataStructure::TopOpeBRepDS_HDataStructure()
{
}

//=======================================================================
//function : AddAncestors
//purpose  : 
//=======================================================================
void TopOpeBRepDS_HDataStructure::AddAncestors
(const TopoDS_Shape& S)
{
  AddAncestors(S,TopAbs_WIRE,TopAbs_EDGE);
  AddAncestors(S,TopAbs_FACE,TopAbs_WIRE);
  AddAncestors(S,TopAbs_SHELL,TopAbs_FACE);
  AddAncestors(S,TopAbs_SOLID,TopAbs_SHELL);
}

//=======================================================================
//function : AddAncestors
//purpose  : 
//=======================================================================
void TopOpeBRepDS_HDataStructure::AddAncestors
(const TopoDS_Shape& S,
 const TopAbs_ShapeEnum T1,const TopAbs_ShapeEnum T2)
{
  TopOpeBRepDS_DataStructure& BDS = ChangeDS();
  Standard_Integer rankS = myDS.AncestorRank(S);
  // find the shapes of type T1 containing HasShape() of type T2
  for (TopOpeBRepTool_ShapeExplorer eT1(S,T1); eT1.More(); eT1.Next()) {
    const TopoDS_Shape& ST1 = eT1.Current();
    for (TopOpeBRepTool_ShapeExplorer eT2(ST1,T2); eT2.More(); eT2.Next()) {
      const TopoDS_Shape& ST2 = eT2.Current();
      if (BDS.HasShape(ST2)) {
	BDS.AddShape(ST1,rankS);
	break;
      }
    }
  }
}

//=======================================================================
//function : ChkIntg
//purpose  : Check Integrity
//=======================================================================
void TopOpeBRepDS_HDataStructure::ChkIntg()
{
  // Check the integrity of the DS
  Handle(TopOpeBRepDS_Check) chk = new TopOpeBRepDS_Check(this);
  chk->ChkIntg();
//  chk->PrintIntg(cout);
}

//=======================================================================
//function : DS
//purpose  : 
//=======================================================================
const TopOpeBRepDS_DataStructure&  TopOpeBRepDS_HDataStructure::DS()const 
{
  return myDS;
}

//=======================================================================
//function : ChangeDS
//purpose  : 
//=======================================================================
TopOpeBRepDS_DataStructure&  TopOpeBRepDS_HDataStructure::ChangeDS()
{
  return myDS;
}

//=======================================================================
//function : NbSurfaces
//purpose  : 
//=======================================================================
Standard_Integer  TopOpeBRepDS_HDataStructure::NbSurfaces()const 
{
  return myDS.NbSurfaces();
}

//=======================================================================
//function : NbCurves
//purpose  : 
//=======================================================================
Standard_Integer  TopOpeBRepDS_HDataStructure::NbCurves()const 
{
  return myDS.NbCurves();
}

//=======================================================================
//function : NbPoints
//purpose  : 
//=======================================================================
Standard_Integer  TopOpeBRepDS_HDataStructure::NbPoints()const 
{
  return myDS.NbPoints();
}

//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================
const TopOpeBRepDS_Surface&  TopOpeBRepDS_HDataStructure::Surface
(const Standard_Integer I)const 
{
  return myDS.Surface(I);
}

//=======================================================================
//function : SurfaceCurves
//purpose  : 
//=======================================================================
TopOpeBRepDS_CurveIterator  TopOpeBRepDS_HDataStructure::SurfaceCurves
(const Standard_Integer I)const 
{
  const TopOpeBRepDS_ListOfInterference& L = myDS.SurfaceInterferences(I);
  return TopOpeBRepDS_CurveIterator(L);
}

//=======================================================================
//function : Curve
//purpose  : 
//=======================================================================
const TopOpeBRepDS_Curve&  TopOpeBRepDS_HDataStructure::Curve
(const Standard_Integer I)const
{
  return myDS.Curve(I);
}

//=======================================================================
//function : ChangeCurve
//purpose  : 
//=======================================================================
TopOpeBRepDS_Curve&  TopOpeBRepDS_HDataStructure::ChangeCurve
(const Standard_Integer I)
{
  return myDS.ChangeCurve(I);
}

//=======================================================================
//function : CurvePoints
//purpose  : 
//=======================================================================
TopOpeBRepDS_PointIterator  TopOpeBRepDS_HDataStructure::CurvePoints
(const Standard_Integer I)const 
{
  const TopOpeBRepDS_ListOfInterference& L = myDS.CurveInterferences(I);
  return TopOpeBRepDS_PointIterator(L);
}

//=======================================================================
//function : Point
//purpose  : 
//=======================================================================
const TopOpeBRepDS_Point&  TopOpeBRepDS_HDataStructure::Point
(const Standard_Integer I)const 
{
  return myDS.Point(I);
}

//=======================================================================
//function : HasGeometry
//purpose  : 
//=======================================================================
Standard_Boolean  TopOpeBRepDS_HDataStructure::HasGeometry
(const TopoDS_Shape& S)const 
{
  Standard_Boolean b = myDS.HasGeometry(S);
  return b;
}

//=======================================================================
//function : HasShape
//purpose  : 
//=======================================================================
Standard_Boolean  TopOpeBRepDS_HDataStructure::HasShape
(const TopoDS_Shape& S,const Standard_Boolean FindKeep) const 
{
  Standard_Boolean b = myDS.HasShape(S, FindKeep);
  return b;
}

//=======================================================================
//function : HasSameDomain
//purpose  : 
//=======================================================================
Standard_Boolean  TopOpeBRepDS_HDataStructure::HasSameDomain
(const TopoDS_Shape& S,const Standard_Boolean FindKeep)const 
{
  if ( ! HasShape(S, FindKeep) ) return Standard_False;
  const TopTools_ListOfShape& l = myDS.ShapeSameDomain(S);
  Standard_Boolean res = ! l.IsEmpty();
  return res;
}

//=======================================================================
//function : SameDomain
//purpose  : 
//=======================================================================
TopTools_ListIteratorOfListOfShape TopOpeBRepDS_HDataStructure::SameDomain
(const TopoDS_Shape& S)const 
{
  const TopTools_ListOfShape& L = myDS.ShapeSameDomain(S);
  return TopTools_ListIteratorOfListOfShape(L);
}

//=======================================================================
//function : SameDomainOrientation
//purpose  : 
//=======================================================================
TopOpeBRepDS_Config  TopOpeBRepDS_HDataStructure::SameDomainOrientation
(const TopoDS_Shape& S)const 
{
  if ( ! HasShape(S) ) return TopOpeBRepDS_UNSHGEOMETRY;
  return myDS.SameDomainOri(S);
}

//=======================================================================
//function : SameDomainReference
//purpose  : 
//=======================================================================
Standard_Integer TopOpeBRepDS_HDataStructure::SameDomainReference
(const TopoDS_Shape& S)const 
{
  if ( ! HasShape(S) ) return 0;
  return myDS.SameDomainRef(S);
}

//=======================================================================
//function : NbShapes
//purpose  : 
//=======================================================================
Standard_Integer  TopOpeBRepDS_HDataStructure::NbShapes()const 
{
  return myDS.NbShapes();
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================
const TopoDS_Shape&  TopOpeBRepDS_HDataStructure::Shape
(const Standard_Integer I,const Standard_Boolean FindKeep) const 
{
  return myDS.Shape(I, FindKeep);
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================
Standard_Integer TopOpeBRepDS_HDataStructure::Shape
(const TopoDS_Shape& S,const Standard_Boolean FindKeep)const
{
  return myDS.Shape(S, FindKeep);
}

//=======================================================================
//function : SolidSurfaces
//purpose  : 
//=======================================================================
TopOpeBRepDS_SurfaceIterator  TopOpeBRepDS_HDataStructure::SolidSurfaces
(const TopoDS_Shape& S) const 
{
  const TopOpeBRepDS_ListOfInterference& L = myDS.ShapeInterferences(S);
  return TopOpeBRepDS_SurfaceIterator(L);
}

//=======================================================================
//function : SolidSurfaces
//purpose  : 
//=======================================================================
TopOpeBRepDS_SurfaceIterator  TopOpeBRepDS_HDataStructure::SolidSurfaces
(const Standard_Integer I)const 
{
  const TopOpeBRepDS_ListOfInterference& L = myDS.ShapeInterferences(I);
  return TopOpeBRepDS_SurfaceIterator(L);
}

//=======================================================================
//function : FaceCurves
//purpose  : 
//=======================================================================
TopOpeBRepDS_CurveIterator  TopOpeBRepDS_HDataStructure::FaceCurves
(const TopoDS_Shape& F)const 
{
  const TopOpeBRepDS_ListOfInterference& L = myDS.ShapeInterferences(F);
  return TopOpeBRepDS_CurveIterator(L);
}

//=======================================================================
//function : FaceCurves
//purpose  : 
//=======================================================================
TopOpeBRepDS_CurveIterator  TopOpeBRepDS_HDataStructure::FaceCurves
(const Standard_Integer I)const 
{
  const TopOpeBRepDS_ListOfInterference& L = myDS.ShapeInterferences(I);
  return TopOpeBRepDS_CurveIterator(L);
}

//=======================================================================
//function : EdgePoints
//purpose  : 
//=======================================================================
TopOpeBRepDS_PointIterator  TopOpeBRepDS_HDataStructure::EdgePoints
(const TopoDS_Shape& E)const 
{
  const TopOpeBRepDS_ListOfInterference& L = myDS.ShapeInterferences(E);
  return TopOpeBRepDS_PointIterator(L);
}

//=======================================================================
//function : MakeCurve
//purpose  : 
//=======================================================================
Standard_Integer TopOpeBRepDS_HDataStructure::MakeCurve
(const TopOpeBRepDS_Curve& curC,TopOpeBRepDS_Curve& newC)
{
  // SCI1, SCI2 = new surface/curve interf. build from 
  // the surface/curve interf. of the curve curC
  const Handle(TopOpeBRepDS_Interference)& I1 = curC.GetSCI1();
  const Handle(TopOpeBRepDS_Interference)& I2 = curC.GetSCI2();
  Handle(TopOpeBRepDS_SurfaceCurveInterference) SCI1,SCI2; 
  if ( ! I1.IsNull() ) SCI1 = new TopOpeBRepDS_SurfaceCurveInterference(I1);
  if ( ! I2.IsNull() ) SCI2 = new TopOpeBRepDS_SurfaceCurveInterference(I2);

  const TopoDS_Shape& S1 = curC.Shape1();
  const TopoDS_Shape& S2 = curC.Shape2();

  // add the new SCIs in the lists of SCIs connected to the shapes
  if ( ! SCI1.IsNull() ) myDS.AddShapeInterference(S1,SCI1);
  if ( ! SCI2.IsNull() ) myDS.AddShapeInterference(S2,SCI2);

  // the shapes of the new curve are the shapes of curve curC
  newC.SetShapes(S1,S2);

  // set surface/curve interferences of the new curve
  newC.SetSCI(SCI1,SCI2);

  // newC is a daughter of curC
  newC.ChangeMother(curC.DSIndex());

  // add the new curve in the DS
  Standard_Integer inewC = myDS.AddCurve(newC);

  // the geometry of the new surface/curve interf. is new curve inewC
  if ( ! SCI1.IsNull() ) SCI1->Geometry(inewC);
  if ( ! SCI2.IsNull() ) SCI2->Geometry(inewC);

  return inewC;
}

//=======================================================================
//function : RemoveCurve
//purpose  : 
//=======================================================================
void TopOpeBRepDS_HDataStructure::RemoveCurve(const Standard_Integer icurC)
{
  myDS.RemoveCurve(icurC);
}

//=======================================================================
//function : NbGeometry
//purpose  : 
//=======================================================================
Standard_Integer TopOpeBRepDS_HDataStructure::NbGeometry
  (const TopOpeBRepDS_Kind K) const
{
  if ( ! TopOpeBRepDS::IsGeometry(K)) return 0;

  Standard_Integer n = 0;
  switch(K) {
  case TopOpeBRepDS_POINT : n = NbPoints(); break;
  case TopOpeBRepDS_CURVE : n = NbCurves(); break;
  case TopOpeBRepDS_SURFACE : n = NbSurfaces(); break;
  default : n = 0; break;
  }			
  return n;
}

//=======================================================================
//function : NbTopology
//purpose  : 
//=======================================================================
Standard_Integer TopOpeBRepDS_HDataStructure::NbTopology() const
{
  Standard_Integer n = myDS.NbShapes();
  return n;
}

//=======================================================================
//function : NbTopology
//purpose  : 
//=======================================================================
Standard_Integer TopOpeBRepDS_HDataStructure::NbTopology
(const TopOpeBRepDS_Kind K) const
{
  if ( ! TopOpeBRepDS::IsTopology(K) ) return 0;
  Standard_Integer res = 0;
  const Standard_Boolean FindKeep = Standard_False;

  TopAbs_ShapeEnum S = TopOpeBRepDS::KindToShape(K);
  for (Standard_Integer i = 1; i <= NbTopology(); i++ )
    if(myDS.KeepShape(i, FindKeep))
      if (myDS.Shape(i, FindKeep).ShapeType() == S) res++;
  return res;
}

//=======================================================================
//function : EdgesSameParameter
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_HDataStructure::EdgesSameParameter() const
{
  Standard_Integer i,n = myDS.NbShapes();
  for (i = 1 ; i <= n; i++) {
    const TopoDS_Shape& s = myDS.Shape(i);
    if ( s.ShapeType() == TopAbs_EDGE ) {
      const TopoDS_Edge& e = TopoDS::Edge(s);
      if ( ! BRep_Tool::SameParameter(e) ) {
	return Standard_False;
      }
    }
  }
  return Standard_True;
}

#include <TColStd_HArray1OfBoolean.hxx>
#include <TColStd_Array1OfBoolean.hxx>

Standard_EXPORT void FUN_TopOpeBRepDS_SortOnParameter
(const TopOpeBRepDS_ListOfInterference& List,
 TopOpeBRepDS_ListOfInterference& SList)
{
  // NYI : sort a list of Items, giving a sorting FUNCTION is impossible
  // NYI : --> foobar method complexity n2.

  Standard_Integer iIntf=0,nIntf = List.Extent();
  if (nIntf == 0) return;

  Handle(TColStd_HArray1OfBoolean) HT;
  HT = new TColStd_HArray1OfBoolean(1,nIntf,Standard_False);
  TColStd_Array1OfBoolean& T = HT->ChangeArray1();

  Handle(TopOpeBRepDS_Interference) Intf;
 
  for (Standard_Integer i = 1; i <= nIntf; i++) { 
    Standard_Real parmin = RealLast();
    TopOpeBRepDS_PointIterator it(List);
    for (Standard_Integer itest = 1; it.More(); it.Next(),itest++) {
      if ( ! T(itest) ) {
	Standard_Real par = it.Parameter();
	if (par < parmin) {
	  parmin = par;
	  Intf = it.Value();
	  iIntf = itest;
	}
      }
    }
    SList.Append(Intf);
    T(iIntf) = Standard_True;
  }
}

//=======================================================================
//function : SortOnParameter
//purpose  : 
//=======================================================================
void TopOpeBRepDS_HDataStructure::SortOnParameter
(const TopOpeBRepDS_ListOfInterference& List,
 TopOpeBRepDS_ListOfInterference& SList) const 
{
  // NYI : sort a list of Items, giving a sorting FUNCTION is impossible
  // NYI : --> foobar method complexity n2.

  ::FUN_TopOpeBRepDS_SortOnParameter(List,SList);

  // tete = interf FORWARD :
  // modifier TopOpeBRepBuild_ParametrizedVertexSet ::SortParametrizedVertex()
  Standard_Boolean found = Standard_False;
  TopOpeBRepDS_ListIteratorOfListOfInterference it(SList);
  TopOpeBRepDS_ListOfInterference L1,L2;

  for (; it.More(); it.Next() ) {
    Handle(TopOpeBRepDS_Interference) I = it.Value();
    if ( ! found) {
      TopAbs_Orientation o = I->Transition().Orientation(TopAbs_IN);
      if (o == TopAbs_FORWARD) {
	found = Standard_True;
	L1.Append(I);
      }
      else L2.Append(I);
    }
    else L1.Append(I);
  }

  SList.Clear();
  SList.Append(L1);
  SList.Append(L2);
}

//=======================================================================
//function : SortOnParameter
//purpose  : 
//=======================================================================
void TopOpeBRepDS_HDataStructure::SortOnParameter
(TopOpeBRepDS_ListOfInterference& List) const 
{
  TopOpeBRepDS_PointIterator it(List);
  if (it.More()) {
    TopOpeBRepDS_ListOfInterference SList;
    SortOnParameter(List,SList);
    List.Assign(SList);
  }
}

//=======================================================================
//function : MinMaxOnParameter
//purpose  : 
//=======================================================================
void TopOpeBRepDS_HDataStructure::MinMaxOnParameter
(const TopOpeBRepDS_ListOfInterference& List,
 Standard_Real& parmin,Standard_Real& parmax) const 
{
  if ( ! List.IsEmpty() ) {
    Standard_Real parline;
    parmin = RealLast(); parmax = RealFirst();
    TopOpeBRepDS_PointIterator it(List);
    for ( ; it.More(); it.Next()) {
      parline = it.Parameter();
      parmin = Min(parmin,parline); 
      parmax = Max(parmax,parline);
    } 
  }
}

//-----------------------------------------------------------------------
// Search, among a list of interferences accessed by the iterator <IT>,
// a geometry whose 3D point is identical yo the 3D point of a DS point <PDS>.
// return True if such an interference has been found, False else.
// if True, iterator <IT> points (by the Value() method) on the first 
// interference accessing an identical 3D point.
//-----------------------------------------------------------------------
//=======================================================================
//function : ScanInterfList
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRepDS_HDataStructure::ScanInterfList
(TopOpeBRepDS_ListIteratorOfListOfInterference& IT,
 const TopOpeBRepDS_Point& PDS) const
{
  for ( ; IT.More(); IT.Next() ) {
    TopOpeBRepDS_Kind GT = IT.Value()->GeometryType();
    Standard_Integer  G  = IT.Value()->Geometry();
    if ( GT == TopOpeBRepDS_POINT ) {
      const TopOpeBRepDS_Point& OOPDS = myDS.Point(G);
      Standard_Boolean iseq = PDS.IsEqual(OOPDS);
      if (iseq) return iseq;
    }
    else if ( GT == TopOpeBRepDS_VERTEX ) {
      TopOpeBRepDS_Point OOPDS(myDS.Shape(G));
      Standard_Boolean iseq = PDS.IsEqual(OOPDS);
      if (iseq) return iseq;
    }
  }
  return Standard_False;
}

//=======================================================================
//function : GetGeometry
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_HDataStructure::GetGeometry
(TopOpeBRepDS_ListIteratorOfListOfInterference& IT,
 const TopOpeBRepDS_Point&                      PDS,
 Standard_Integer&                              G,
 TopOpeBRepDS_Kind&                             K) const 
{
  Standard_Boolean found = ScanInterfList(IT,PDS);
  if (found) {
    G  = IT.Value()->Geometry();
    K  = IT.Value()->GeometryType();
  }
  return found;
}

//=======================================================================
//function : StoreInterference
//purpose  : Append an interference I to a list of interference LI
//           Append I to the interf. list connected to I Geometry()
//=======================================================================
void TopOpeBRepDS_HDataStructure::StoreInterference
(const Handle(TopOpeBRepDS_Interference)& I,TopOpeBRepDS_ListOfInterference& LI, const TCollection_AsciiString&)
{
  // append I to list LI
  LI.Append(I);
  Standard_Integer G = I->Geometry();

  // append I to list of interference connected to G = I->Geometry()
  switch (I->GeometryType()) {

  case TopOpeBRepDS_SOLID : case TopOpeBRepDS_FACE : 
  case TopOpeBRepDS_EDGE : case TopOpeBRepDS_VERTEX :
//    appendtoG = Standard_True;
//    myDS.ChangeShapeInterferences(G).Append(I);
    break;
    
  case TopOpeBRepDS_SURFACE :
    myDS.ChangeSurfaceInterferences(G).Append(I);
    break;
    
  case TopOpeBRepDS_CURVE :
    myDS.ChangeCurveInterferences(G).Append(I);
    break;
    
  case TopOpeBRepDS_POINT :
//    appendtoG = Standard_True;
//    myDS.ChangePointInterferences(G).Append(I);
    break;
  default:
    break;
  }
}

//=======================================================================
//function : StoreInterference
//purpose  : 
//=======================================================================
void TopOpeBRepDS_HDataStructure::StoreInterference
(const Handle(TopOpeBRepDS_Interference)& I,const TopoDS_Shape& S, const TCollection_AsciiString&)
{
  Standard_Boolean h = myDS.HasShape(S);
  if ( !h ) {
    throw Standard_ProgramError("StoreInterference on shape out of DS");
    return;
  }
  StoreInterference(I,myDS.ChangeShapeInterferences(S));
}

//=======================================================================
//function : StoreInterference
//purpose  : 
//=======================================================================

void TopOpeBRepDS_HDataStructure::StoreInterference
(const Handle(TopOpeBRepDS_Interference)& I,
 const Standard_Integer IS, const TCollection_AsciiString&)
{
  Standard_Integer n = myDS.NbShapes();
  if ( IS < 1 || IS > n ) {
    throw Standard_ProgramError("StoreInterference on index out of DS");
    return;
  }

  StoreInterference(I,myDS.ChangeShapeInterferences(IS));
}

//=======================================================================
//function : StoreInterference
//purpose  : 
//=======================================================================
void TopOpeBRepDS_HDataStructure::StoreInterferences
(const TopOpeBRepDS_ListOfInterference& LI,const Standard_Integer IS
// ,const TCollection_AsciiString& str)
 ,const TCollection_AsciiString& )
{
  TopOpeBRepDS_ListOfInterference& lids = myDS.ChangeShapeInterferences(IS);
  TopOpeBRepDS_ListIteratorOfListOfInterference it(LI);
  for (; it.More(); it.Next()){
    const Handle(TopOpeBRepDS_Interference)& I = it.Value();
    StoreInterference(I,lids);
  }
}

//=======================================================================
//function : StoreInterferences
//purpose  : 
//=======================================================================
void TopOpeBRepDS_HDataStructure::StoreInterferences
(const TopOpeBRepDS_ListOfInterference& LI,const TopoDS_Shape& S
// ,const TCollection_AsciiString& str)
 ,const TCollection_AsciiString& )
{
  TopOpeBRepDS_ListOfInterference& lids = myDS.ChangeShapeInterferences(S);
  TopOpeBRepDS_ListIteratorOfListOfInterference it(LI);
  for (; it.More(); it.Next()){
    const Handle(TopOpeBRepDS_Interference)& I = it.Value();
    StoreInterference(I,lids);
  }
}

//=======================================================================
//function : ClearStoreInterferences
//purpose  : 
//=======================================================================
void TopOpeBRepDS_HDataStructure::ClearStoreInterferences
(const TopOpeBRepDS_ListOfInterference& LI,const Standard_Integer IS
// ,const TCollection_AsciiString& str)
 ,const TCollection_AsciiString& )
{
  TopOpeBRepDS_ListOfInterference& lids = myDS.ChangeShapeInterferences(IS);
  lids.Clear();
  TopOpeBRepDS_ListIteratorOfListOfInterference it(LI);
  for (; it.More(); it.Next()){
    const Handle(TopOpeBRepDS_Interference)& I = it.Value();
    StoreInterference(I,lids);
  }
}

//=======================================================================
//function : ClearStoreInterferences
//purpose  : 
//=======================================================================
void TopOpeBRepDS_HDataStructure::ClearStoreInterferences
(const TopOpeBRepDS_ListOfInterference& LI,const TopoDS_Shape& S
// ,const TCollection_AsciiString& str)
 ,const TCollection_AsciiString& )
{
  TopOpeBRepDS_ListOfInterference& lids = myDS.ChangeShapeInterferences(S);
  lids.Clear();
  TopOpeBRepDS_ListIteratorOfListOfInterference it(LI);
  for (; it.More(); it.Next()){
    const Handle(TopOpeBRepDS_Interference)& I = it.Value();
    StoreInterference(I,lids);
  }
}
