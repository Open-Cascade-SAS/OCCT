// Created on: 1996-02-05
// Created by: Jea Yves LEBEY
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

#include <TestTopOpe_BOOP.hxx>
#include <TestTopOpe.hxx>
#include <TopOpeBRepTool.hxx>
#include <TopOpeBRepTool_GeomTool.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_Filter.hxx>
#include <TopOpeBRepDS_Reducer.hxx>
#include <TopOpeBRepDS_Check.hxx>
#include <TopOpeBRepDS_Surface.hxx>
#include <TopOpeBRepDS_Curve.hxx>
#include <TopOpeBRepDS_Point.hxx>
#include <TopOpeBRepDS.hxx>
#include <TopOpeBRep_FacesFiller.hxx>
#include <TopOpeBRep_FacesIntersector.hxx>
#include <TopOpeBRep_EdgesIntersector.hxx>
#include <TopOpeBRep_ShapeIntersector.hxx>
#include <TopOpeBRep_define.hxx>
#include <BRepAlgo_BooleanOperations.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopAbs.hxx>
#include <gp_Pnt.hxx>
#include <DBRep.hxx>
#include <Draw_Appli.hxx>
#ifdef WNT
Standard_IMPORT Draw_Viewer dout;
#endif

extern Standard_Integer TOPOPE_SeeShape(char *name);

#ifdef OCCT_DEBUG
#include <TopOpeBRepTool_KRO.hxx>
extern Standard_Boolean TopOpeBRepTool_GettraceKRO();
extern void PrintKRO_PREPA();
#endif

Standard_Integer TestTopOpe_BOOP::Prepare(const char* ns1,const char* ns2)
{
  Standard_Integer anErrorCode = 0;
  anErrorCode = LoadShapes(ns1,ns2);  // mylastPREP is 0
  if (anErrorCode)
    return anErrorCode;

  anErrorCode = LoadnewDS();          // mylastPREP is 100
  if (anErrorCode)
    return anErrorCode;

  anErrorCode = DSF_Insert();         // mylastPREP is (100,200)
  if (anErrorCode)
    return anErrorCode;

  anErrorCode = LoadnewHB();          // mylastPREP is 200
  if (anErrorCode)
    return anErrorCode;

  anErrorCode = Builder_Perform();    // mylastPREP is (200,300)
  return anErrorCode;
}

Standard_Integer TestTopOpe_BOOP::LoadShapes(const char* ns1, const char* ns2)
{
  if (ns1==NULL) return 1;TopoDS_Shape S1=DBRep::Get(ns1);if (S1.IsNull()) return 1;
  if (ns2==NULL) return 1;TopoDS_Shape S2=DBRep::Get(ns2);if (S2.IsNull()) return 1;
  SetShape1(S1); mynameS1 = ns1;
  SetShape2(S2); mynameS2 = ns2;
  mylastPREP = 0;
  return 0;
}

Standard_Integer TestTopOpe_BOOP::LoadnewDS()
{
  myHDS = new TopOpeBRepDS_HDataStructure();
  TestTopOpe::Shapes(myS1,myS2);
  TestTopOpe::CurrentDS(myHDS);
  if (myPDSF != NULL) { free((TopOpeBRep_DSFiller*)myPDSF); myPDSF = NULL; }
  myPDSF = new TopOpeBRep_DSFiller();
  mylastPREP = 100;
  return 0;
}

Standard_Integer TestTopOpe_BOOP::DSF_Insert()
{
  TopOpeBRep_DSFiller DSF;
  DSF_Insert(DSF);
  mylastPREP = 190;
  return 0;
}

Standard_Integer TestTopOpe_BOOP::LoadnewHB()
{
  TopOpeBRepTool_GeomTool GT;
  Standard_Boolean c2d = myVarsTopo.GetC2D();
  GT.Define(myVarsTopo.GetOCT(),Standard_True,c2d, c2d);
  Standard_Real t3,t2; myVarsTopo.GetTolx(t3,t2);
  GT.SetTolerances(t3,t2);
  TopOpeBRepDS_BuildTool BT(GT);
  myHB = new TopOpeBRepBuild_HBuilder(BT);
  mylastPREP = 200;
  return 0;
}

Standard_Integer TestTopOpe_BOOP::Builder_Perform()
{
  myHB->Perform(myHDS,myS1,myS2);
#ifdef OCCT_DEBUG
  if ( TopOpeBRepTool_GettraceKRO() ) PrintKRO_PREPA();
#endif
  myESP = myHDS->EdgesSameParameter();
  myPREPdone = Standard_True;
  myBOOdone = Standard_False;
  mylastPREP = 290;
  return 0;
}

Standard_Integer TestTopOpe_BOOP::FaceFilter()
{
  TopOpeBRepDS_Filter F(myHDS);
  F.ProcessFaceInterferences(myHB->ChangeBuilder().MSplit(TopAbs_ON));
  return 0;
}

Standard_Integer TestTopOpe_BOOP::FaceReducer()
{
  TopOpeBRepDS_Reducer R(myHDS);
  R.ProcessFaceInterferences(myHB->ChangeBuilder().MSplit(TopAbs_ON));
  return 0;
}

Standard_Boolean TestTopOpe_BOOP::CheckBooope(const char* key)
{
  Standard_Boolean res = Standard_True;
  Operation_t o = Operation(key);
  if (o == BOOP_UND) return Standard_False;
  
  if ( !myPREPdone) {
    cout<<"pas de preparation"<<endl;
    if ( myHB.IsNull() ) {
      res = Standard_False;
    }
  }
  else if ( !myESP ) {
    cout<<"edges non SameParameter()"<<endl;
    if (o == BOOP_C12 || o == BOOP_C21 || o == BOOP_COM || o == BOOP_FUS) res = Standard_False;
  }
  
  return res;
}

Standard_Integer TestTopOpe_BOOP::FindShape(const TCollection_AsciiString& name)
{
  Standard_Integer ix = 0;
  if (ISINTEGER(name.ToCString())) {
    if (myHDS.IsNull()) return 0;
    Standard_Integer is = Draw::Atoi(name.ToCString());
    Standard_Integer ns = myHDS->NbShapes();
    Standard_Integer i1 = 0, i2 = 0;
    if (is != 0) { 
      if (is > ns || ns < 1) {
	cout<<"index "<<is<<" is not assigned in DS"<<endl;
	return 0;
      }
      else {
	i1 = i2 = is;
      }
    }
    else {
      i1 = 1; i2 = ns; 
    }
    for (Standard_Integer ids=i1;ids<=i2;ids++) {
      ix = FindShape(ids,myS1);
      if (ix) cout<<"DS shape "<<ids<<" is same "<<TopOpeBRepDS::SPrint(myHDS->Shape(ids).ShapeType(),ix)<<" of "<<mynameS1<<" (1)"<<endl;
      ix = FindShape(ids,myS2);
      if (ix) cout<<"DS shape "<<ids<<" is same "<<TopOpeBRepDS::SPrint(myHDS->Shape(ids).ShapeType(),ix)<<" of "<<mynameS2<<" (2)"<<endl;
    }
  }
  else {
    Standard_CString aCString =  name.ToCString();
    TopoDS_Shape S = DBRep::Get(aCString);
    if (S.IsNull()) {cout<<"shape "<<name<<" not found"<<endl; return 0;}
    else {
      ix = FindShape(S,myS1);
      if (ix) cout<<"shape "<<name<<" is same "<<TopOpeBRepDS::SPrint(S.ShapeType(),ix)<<" of "<<mynameS1<<" (1)"<<endl;
      ix = FindShape(S,myS2);
      if (ix) cout<<"shape "<<name<<" is same "<<TopOpeBRepDS::SPrint(S.ShapeType(),ix)<<" of "<<mynameS2<<" (2)"<<endl;
    }
 }
  return 0;
}

Standard_Integer TestTopOpe_BOOP::FindShape(const Standard_Integer ids,const TopoDS_Shape& Sref)
{
  if (myHDS.IsNull()) return 0;
  const TopoDS_Shape& S = myHDS->Shape(ids);
  Standard_Integer i = FindShape(S,Sref);
  return i;
}

Standard_Integer TestTopOpe_BOOP::FindShape(const TopoDS_Shape& S,const TopoDS_Shape& Sref)
{
  if (Sref.IsNull()) return 0; TopExp_Explorer x; Standard_Integer i;
  for (i=1,x.Init(Sref,S.ShapeType());x.More();x.Next(),i++)
    if (x.Current().IsSame(S)) return i;
  return 0;
}

void TestTopOpe_BOOP::Booope(const char* key,const char *namres)
{
  Standard_Boolean res = CheckBooope(key);
  if (!res) return;
  if (myHB.IsNull()) return;
  
  Operation_t o = Operation(key);

  if       (o == BOOP_SECC || o == BOOP_SECE || o == BOOP_SEC) {
    Standard_Integer k;
    if      (o == BOOP_SECC) k = 1; // section sur courbes
    else if (o == BOOP_SECE) k = 2; // section sur aretes
    else if (o == BOOP_SEC)  k = 3; // toutes sections
    else return;
    TopTools_ListOfShape LE; myHB->InitSection(k);
    for (;myHB->MoreSection();myHB->NextSection())
      LE.Append(myHB->CurrentSection());
    MakeResult(LE); StoreResult(key,namres);
  }

  else if (o == BOOP_C12 || o == BOOP_C21 || o == BOOP_COM || o == BOOP_FUS) {

    TopAbs_State t1=TopAbs_UNKNOWN,t2=TopAbs_UNKNOWN;
    if      (o == BOOP_C12) { t1 = TopAbs_OUT; t2 = TopAbs_IN;  }
    else if (o == BOOP_C21) { t1 = TopAbs_IN;  t2 = TopAbs_OUT; }
    else if (o == BOOP_COM) { t1 = TopAbs_IN;  t2 = TopAbs_IN;  }
    else if (o == BOOP_FUS) { t1 = TopAbs_OUT; t2 = TopAbs_OUT; }
    
    if((myS2type > TopAbs_WIRE) && (o == BOOP_COM)) {
      BRepAlgo_BooleanOperations babo;
      babo.Shapes2d(myS1,myS2);
      TestTopOpe::CurrentDS(babo.DataStructure());
      babo.Common();
      myHB = babo.Builder();
      const TopTools_ListOfShape& LE = myHB->Splits(myS2, TopAbs_IN);
      MakeResult(LE); StoreResult(key,namres);
      return;
    }
    
    myHB->Clear();
    myHB->MergeShapes(myS1,t1,myS2,t2);
    cout<<"MergeShapes done"<<endl;
    
    const TopTools_ListOfShape& L1 = myHB->Merged(myS1,t1);
    MakeResult(L1);
    StoreResult(key,namres);
  }

  myBOOdone = Standard_True;
}

void TestTopOpe_BOOP::ClearResult()
{
  BRep_Builder BB;
  BB.MakeCompound(myCresult);
  myNresult = 0;
}

void TestTopOpe_BOOP::AddResult(const TopTools_ListOfShape& L)
{
  BRep_Builder BB;
  TopTools_ListIteratorOfListOfShape it(L);
  for (; it.More(); it.Next(), myNresult++) BB.Add(myCresult,it.Value());
}

void TestTopOpe_BOOP::MakeResult(const TopTools_ListOfShape& L)
{
  ClearResult();
  AddResult(L);
}

void TestTopOpe_BOOP::StoreResult
(const char* key,const char* namres, char* extent) const
{
  const char* nam = (namres == NULL) ? myresnamdef : namres;
  if (nam == NULL) return;
  char na[100]; strcpy(na,nam);if (extent!=NULL) strcat(na,extent);

  if (Operation(key) != BOOP_SEC && myVarsTopo.GetClear()) dout.Clear();
  DBRep::Set(na,myCresult);
  if (key != NULL) {
    cout<<"("<<myNresult<<" shape) ";
    cout<<key<<" result stored in variable "<<na<<endl;
  }
}

void TestTopOpe_BOOP::GetSplit(const TopAbs_ShapeEnum typ,const TopAbs_State sta)
{
  if ( myHB.IsNull() ) return;
  if ( myHB->DataStructure().IsNull() ) return;
  ClearResult();
  Standard_Integer ns = myHB->DataStructure()->DS().NbShapes();
  for (Standard_Integer is = 1; is <= ns; is++) {
    const TopoDS_Shape& S = myHB->DataStructure()->Shape(is);
    Standard_Boolean issplit = myHB->IsSplit(S,sta);
    if (issplit) {
      GetSplit(typ,sta,is);
    }
  }
}

void TestTopOpe_BOOP::GetSplit(const TopAbs_ShapeEnum typ,
			const TopAbs_State sta,
			const Standard_Integer isha)
{
  if (isha == 0) return;
  if ( myHB.IsNull() ) return;
  if ( myHB->DataStructure().IsNull() ) return;
  const TopoDS_Shape& S = myHB->DataStructure()->Shape(isha);
  if (S.IsNull()) { cout<<"shape "<<isha<<" nul"<<endl; return; }
  if (S.ShapeType() != typ) return;
  Standard_Boolean issplit = myHB->IsSplit(S,sta);
  if (issplit) {
    const TopTools_ListOfShape& LS = myHB->Splits(S,sta);
    ClearResult();
    AddResult(LS);
    TCollection_AsciiString namres;

    if      (typ==TopAbs_EDGE) namres = "e"; 
    else if (typ==TopAbs_FACE) namres = "f";
    else if (typ==TopAbs_SOLID) namres = "s";
    namres += TCollection_AsciiString(isha);
//    namres += "sp";
    if      (sta==TopAbs_IN) namres += "in";
    else if (sta==TopAbs_OUT) namres += "ou";
    else if (sta==TopAbs_ON) namres += "on";

    cout<<namres<<" : Split ";TopAbs::Print(sta,cout);
    cout<<" (";TopAbs::Print(typ,cout);cout<<","<<isha<<")";
    cout<<" --> "<<LS.Extent();
    cout<<endl;

    Standard_Boolean bcle = myVarsTopo.GetClear();
    myVarsTopo.SetClear(Standard_False);
    StoreResult(NULL,(char*)namres.ToCString());
    TOPOPE_SeeShape((char *)namres.ToCString());
    myVarsTopo.SetClear(bcle);
  }
}

void TestTopOpe_BOOP::GetSplit(const TopAbs_State sta,const Standard_Integer isha)
{
  if (isha == 0) return;
  if ( myHB.IsNull() ) return;
  if ( myHB->DataStructure().IsNull() ) return;
  const TopoDS_Shape& S = myHB->DataStructure()->Shape(isha);
  if (S.IsNull()) { cout<<"shape "<<isha<<" nul"<<endl; return; }
  TopAbs_ShapeEnum typ = S.ShapeType();
  Standard_Boolean issplit = myHB->IsSplit(S,sta);
  if (issplit) { 
    GetSplit(typ,sta,isha);
  }
}

void TestTopOpe_BOOP::GetSplit(const Standard_Integer isha)
{
  if (isha == 0) return;
  if ( myHB.IsNull() ) return;
  if ( myHB->DataStructure().IsNull() ) return;
  const TopoDS_Shape& S = myHB->DataStructure()->Shape(isha);
  if (S.IsNull()) { cout<<"shape "<<isha<<" nul"<<endl; return; }  
  GetSplit(TopAbs_OUT,isha);
  GetSplit(TopAbs_ON,isha);
  GetSplit(TopAbs_IN,isha);
}

void TestTopOpe_BOOP::GetSplit(const TopAbs_State sta)
{
  if ( myHB.IsNull() ) return;
  if ( myHB->DataStructure().IsNull() ) return;
  GetSplit(TopAbs_SOLID,sta);
  GetSplit(TopAbs_FACE,sta);
  GetSplit(TopAbs_EDGE,sta);
}

void TestTopOpe_BOOP::GetMerged(const TopAbs_ShapeEnum typ,const TopAbs_State sta)
{
  if ( myHB.IsNull() ) return;
  if ( myHB->DataStructure().IsNull() ) return;
  ClearResult();
  Standard_Integer ns = myHB->DataStructure()->DS().NbShapes();
  for (Standard_Integer is = 1; is <= ns; is++) {
    const TopoDS_Shape& S = myHB->DataStructure()->Shape(is);
    Standard_Boolean isMerged = myHB->IsMerged(S,sta);
    if (isMerged) {
      GetMerged(typ,sta,is);
    }
  }
}

void TestTopOpe_BOOP::GetMerged(const TopAbs_ShapeEnum typ,const TopAbs_State sta,const Standard_Integer isha)
{
  if (isha == 0) return;
  if ( myHB.IsNull() ) return;
  if ( myHB->DataStructure().IsNull() ) return;
  const TopoDS_Shape& S = myHB->DataStructure()->Shape(isha);
  if (S.IsNull()) { cout<<"shape "<<isha<<" nul"<<endl; return; }
  if (S.ShapeType() != typ) return;
  Standard_Boolean isMerged = myHB->IsMerged(S,sta);
  if (isMerged) {
    const TopTools_ListOfShape& LS = myHB->Merged(S,sta);
    ClearResult();
    AddResult(LS);
    TCollection_AsciiString namres;

    if      (typ==TopAbs_EDGE) namres = "e"; 
    else if (typ==TopAbs_FACE) namres = "f";
    else if (typ==TopAbs_SOLID) namres = "s";
    namres += TCollection_AsciiString(isha);
    namres += "me";
    if      (sta==TopAbs_IN) namres += "IN";
    else if (sta==TopAbs_OUT) namres += "OUT";
    else if (sta==TopAbs_ON) namres += "ON";

    cout<<namres<<" : Merged ";TopAbs::Print(sta,cout);
    cout<<" (";TopAbs::Print(typ,cout);cout<<","<<isha<<")";
    cout<<" --> "<<LS.Extent()<<" shape";
    cout<<endl;

    Standard_Boolean bcle = myVarsTopo.GetClear();
    myVarsTopo.SetClear(Standard_False);
    StoreResult(NULL,(char*)namres.ToCString());
    TOPOPE_SeeShape((char *)namres.ToCString());
    myVarsTopo.SetClear(bcle);
  }
}

void TestTopOpe_BOOP::GetMerged(const TopAbs_State sta,const Standard_Integer isha)
{
  if (isha == 0) return;
  if ( myHB.IsNull() ) return;
  if ( myHB->DataStructure().IsNull() ) return;
  const TopoDS_Shape& S = myHB->DataStructure()->Shape(isha);
  if (S.IsNull()) { cout<<"shape "<<isha<<" nul"<<endl; return; }
  TopAbs_ShapeEnum typ = S.ShapeType();
  Standard_Boolean isMerged = myHB->IsMerged(S,sta);
  if (isMerged) {
    GetMerged(typ,sta,isha);
  }
}

void TestTopOpe_BOOP::GetMerged(const Standard_Integer isha)
{
  if (isha == 0) return;
  if ( myHB.IsNull() ) return;
  if ( myHB->DataStructure().IsNull() ) return;
  const TopoDS_Shape& S = myHB->DataStructure()->Shape(isha);
  if (S.IsNull()) { cout<<"shape "<<isha<<" nul"<<endl; return; }  
  GetMerged(TopAbs_OUT,isha);
  GetMerged(TopAbs_ON,isha);
  GetMerged(TopAbs_IN,isha);
}

void TestTopOpe_BOOP::GetMerged(const TopAbs_State sta)
{ 
  if ( myHB.IsNull() ) return;
  if ( myHB->DataStructure().IsNull() ) return;
  GetMerged(TopAbs_SOLID,sta);
  GetMerged(TopAbs_FACE,sta);
  GetMerged(TopAbs_EDGE,sta);
}

// NYI void ChkIntg();
// NYI void ChkIntgInterf();
// NYI void ChkIntgSamDomain();
// NYI void ChkVertex();
