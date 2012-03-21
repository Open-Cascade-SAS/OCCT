// Created on: 1996-02-05
// Created by: Jean Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
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


#ifndef _TestTopOpe_BOOP_HeaderFile
#define _TestTopOpe_BOOP_HeaderFile

#include <TestTopOpe_VarsTopo.hxx>
#include <TopOpeBRep_DSFiller.hxx>
#include <TopoDS_Compound.hxx>
#include <TopOpeBRep_DSFiller.hxx>
#include <TopOpeBRep_define.hxx>
#include <TopOpeBRepBuild_define.hxx>

#define TESTTOPOPE_NKEYS 8
enum Operation_t {BOOP_UND = -1,BOOP_TOPX = 0,BOOP_SECC = 1,BOOP_SECE = 2,BOOP_SEC = 3,
	      BOOP_COM = 4,BOOP_C12 = 5,BOOP_C21 = 6,BOOP_FUS = 7} ;

#define TESTTOPOPE_NTKEYS 20

class TestTopOpe_BOOP {
  
public:

  TestTopOpe_BOOP(const char*[],const char* resnamdef);
  Operation_t Operation(const char* key) const;
  Standard_Boolean IsBooope(const char* key) const;

  void   Tinit();
  const TCollection_AsciiString& Tkeys(const Standard_Integer ik) const;
  const TCollection_AsciiString& Thelp(const Standard_Integer ik) const;
  Standard_Integer    Tstep(const TCollection_AsciiString& s) const;
  Standard_Integer    Tnb() const;
  Standard_Boolean    Tadd(const TCollection_AsciiString& k,const Standard_Integer ic,const TCollection_AsciiString& h);

  VarsTopo& ChangeVarsTopo();
  void SetCurrentHB(const Handle(TopOpeBRepBuild_HBuilder)& HB);
  void SetCurrentHDS(const Handle(TopOpeBRepDS_HDataStructure)& HDS);
  void SetShape1(const TopoDS_Shape& S);
  void SetShape2(const TopoDS_Shape& S);
  Handle(TopOpeBRepBuild_HBuilder)& ChangeCurrentHB();
  Handle(TopOpeBRepDS_HDataStructure)& ChangeCurrentDS();
  TopoDS_Shape& ChangeShape1();
  TopoDS_Shape& ChangeShape2();
  const TCollection_AsciiString& nameS1() const; 
  const TCollection_AsciiString& nameS2() const; 
  //
  Standard_Integer Prepare(const char* ns1,const char* ns2);
  Standard_Integer LoadShapes(const char* ns1,const char* ns2);  
  Standard_Integer LoadnewDS();
  Standard_Integer DSF_SetInterTolerances(TopOpeBRep_DSFiller& DSF); //
  Standard_Integer DSF_Insert(); // 
  Standard_Integer DSF_Insert(TopOpeBRep_DSFiller& DSF);
  Standard_Integer DSF_Insert_InsertIntersection(TopOpeBRep_DSFiller& DSF);
  Standard_Integer DSF_Insert_Complete_GapFiller(TopOpeBRep_DSFiller& DSF);
  Standard_Integer DSF_Insert_Complete_CompleteDS(TopOpeBRep_DSFiller& DSF);
  Standard_Integer DSF_Insert_Complete_Filter(TopOpeBRep_DSFiller& DSF);
  Standard_Integer DSF_Insert_Complete_Reducer(TopOpeBRep_DSFiller& DSF);
  Standard_Integer DSF_Insert_Complete_RemoveUnsharedGeometry(TopOpeBRep_DSFiller& DSF);
  Standard_Integer DSF_Insert_Complete_Checker(TopOpeBRep_DSFiller& DSF);
  Standard_Integer LoadnewHB();
  Standard_Integer Builder_Perform();
  Standard_Integer FaceFilter();
  Standard_Integer FaceReducer();
  //
  void SetTol(const TopOpeBRepDS_Kind K,const Standard_Integer index,const Standard_Real tol);
  void SetTol(const Standard_Integer index,const Standard_Real tol);
  void SetPnt(const Standard_Integer index,const Standard_Real x,const Standard_Real y,const Standard_Real z);

  void Booope(const char* key, const char* namres);
  void GetSplit(const TopAbs_ShapeEnum Typ,const TopAbs_State sta);
  void GetSplit(const TopAbs_ShapeEnum Typ,const TopAbs_State sta,const Standard_Integer is);
  void GetSplit(const TopAbs_State sta,const Standard_Integer is);
  void GetSplit(const TopAbs_State sta);
  void GetSplit(const Standard_Integer is);

  void GetMerged(const TopAbs_ShapeEnum Typ,const TopAbs_State sta);
  void GetMerged(const TopAbs_ShapeEnum Typ,const TopAbs_State sta,const Standard_Integer is);
  void GetMerged(const TopAbs_State sta,const Standard_Integer is);
  void GetMerged(const TopAbs_State sta);
  void GetMerged(const Standard_Integer is);

  void ClearResult();
  void AddResult(const TopTools_ListOfShape& L);
  void MakeResult(const TopTools_ListOfShape& L);
  void StoreResult(const char* key, const char* namres, char* extent=NULL) const;
  const Handle(TopOpeBRepBuild_HBuilder)& HBuilder() const;
  void SetMode(const Standard_Integer mode);
  Standard_Integer  GetMode() const;
  Standard_Boolean CheckBooope(const char* key);
  Standard_Integer FindShape(const TCollection_AsciiString& str);
  Standard_Integer FindShape(const Standard_Integer ids,const TopoDS_Shape& Sref);
  Standard_Integer FindShape(const TopoDS_Shape& S,const TopoDS_Shape& Sref);

  Standard_Integer mylastPREP;
  Standard_Integer mytodoPREP;
  Standard_Integer mymet[100];
  Standard_Integer mynmet;
  TopOpeBRep_DSFiller* myPDSF;

private:

  const char* mykeys[TESTTOPOPE_NKEYS];
  const char* myresnamdef;
  VarsTopo myVarsTopo;
  Handle_TopOpeBRepDS_HDataStructure myHDS;
  Handle_TopOpeBRepBuild_HBuilder myHB;
  TCollection_AsciiString mynameS1,mynameS2;
  TopoDS_Shape myS1,myS2;
  TopAbs_ShapeEnum myS1type,myS2type;
  Standard_Boolean myESP;
  Standard_Boolean myPREPdone;
  TopoDS_Compound myCresult;
  Standard_Integer myNresult;
  Standard_Boolean myBOOdone;

  Standard_Integer   mycomn;
  TCollection_AsciiString mycomk[TESTTOPOPE_NTKEYS];
  TCollection_AsciiString mycomh[TESTTOPOPE_NTKEYS];
  Standard_Integer   mycomi[TESTTOPOPE_NTKEYS];
};

#define ISINTEGER(Mstr) ((strspn((Mstr),"0123456789")==strlen((Mstr))))

#endif
