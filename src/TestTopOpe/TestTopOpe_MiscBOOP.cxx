// Created on: 1996-02-05
// Created by: Jea Yves LEBEY
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


#include <TestTopOpe_BOOP.hxx>
#include <TopOpeBRep_FacesIntersector.hxx>
#include <TopOpeBRep_EdgesIntersector.hxx>
#include <TopOpeBRep_ShapeIntersector.hxx>
#include <TopAbs.hxx>
#include <gp_Pnt.hxx>


TestTopOpe_BOOP::TestTopOpe_BOOP(const char* keys[],
				 const char* resnamdef)
{
  Standard_Integer i;
  for (i=0;i<TESTTOPOPE_NKEYS; i++) mykeys[i] = keys[i]; 
  myresnamdef = resnamdef;
  myS1type = myS2type = TopAbs_SHAPE;
  mynameS1 = ""; mynameS2 = "";
  myPREPdone = Standard_False;
  myBOOdone = Standard_False;
  myNresult = 0;
  mylastPREP = -1; mytodoPREP = 999;
  myPDSF = NULL;
  mycomn = 0;
}

void TestTopOpe_BOOP::Tinit()
{
  mycomn = 0;
}

Standard_Integer TestTopOpe_BOOP::Tnb() const
{
  return mycomn;
}

Standard_Boolean TestTopOpe_BOOP::Tadd(const TCollection_AsciiString& k,const Standard_Integer i,const TCollection_AsciiString& h) 
{
  if (mycomn + 1 == TESTTOPOPE_NTKEYS) return Standard_False;
  Standard_Integer r = mycomn++;
  mycomk[r] = k;
  mycomi[r] = i;
  mycomh[r] = h;
  return Standard_True;
}

static TCollection_AsciiString TestTopOpebibi = "";

const TCollection_AsciiString& TestTopOpe_BOOP::Tkeys(const Standard_Integer i) const
{
  if (i < mycomn ) return mycomk[i];
  return TestTopOpebibi;
}

const TCollection_AsciiString& TestTopOpe_BOOP::Thelp(const Standard_Integer i) const
{
  if (i < mycomn ) return mycomh[i];
  return TestTopOpebibi;
}

Standard_Integer TestTopOpe_BOOP::Tstep(const TCollection_AsciiString& s) const
{
  for (Standard_Integer i = 0; i<mycomn; i++) {
    if (mycomk[i] == s) return mycomi[i];
  }
  return -1;
}

Operation_t TestTopOpe_BOOP::Operation(const char* key) const 
{
  if (key == NULL) return BOOP_UND;

  Operation_t o = BOOP_UND;
  if      (!strcmp(key,mykeys[BOOP_TOPX])) o = BOOP_TOPX;
  else if (!strcmp(key,mykeys[BOOP_SECC])) o = BOOP_SECC;
  else if (!strcmp(key,mykeys[BOOP_SECE])) o = BOOP_SECE;
  else if (!strcmp(key,mykeys[BOOP_SEC])) o = BOOP_SEC;
  else if (!strcmp(key,mykeys[BOOP_COM])) o = BOOP_COM;
  else if (!strcmp(key,mykeys[BOOP_C12])) o = BOOP_C12;
  else if (!strcmp(key,mykeys[BOOP_C21])) o = BOOP_C21;
  else if (!strcmp(key,mykeys[BOOP_FUS])) o = BOOP_FUS;

  return o;
}

Standard_Boolean TestTopOpe_BOOP::IsBooope(const char* key) const
{
  Operation_t o = Operation(key);
  Standard_Boolean res = (o == BOOP_SECC || o == BOOP_SECE || o == BOOP_SEC || 
	     o == BOOP_C12 || o == BOOP_C21 || o == BOOP_COM || o == BOOP_FUS);
  return res;
}

void TestTopOpe_BOOP::SetCurrentHB(const Handle(TopOpeBRepBuild_HBuilder)& HB) {myHB = HB;}
void TestTopOpe_BOOP::SetCurrentHDS(const Handle(TopOpeBRepDS_HDataStructure)& HDS) {myHDS = HDS;}
void TestTopOpe_BOOP::SetShape1(const TopoDS_Shape& S1) 
{
  if (S1.IsNull()) { myS1.Nullify(); return;} 
  myS1=S1;myS1type=myS1.ShapeType();
}
void TestTopOpe_BOOP::SetShape2(const TopoDS_Shape& S2) 
{
  if (S2.IsNull()) { myS2.Nullify(); return;} 
  myS2=S2;myS2type=myS2.ShapeType();
}

Handle(TopOpeBRepBuild_HBuilder)& TestTopOpe_BOOP::ChangeCurrentHB() {return myHB;}
Handle(TopOpeBRepDS_HDataStructure)& TestTopOpe_BOOP::ChangeCurrentDS() {return myHDS;}
TopoDS_Shape& TestTopOpe_BOOP::ChangeShape1() {return myS1;}
TopoDS_Shape& TestTopOpe_BOOP::ChangeShape2() {return myS2;}
VarsTopo& TestTopOpe_BOOP::ChangeVarsTopo() { return myVarsTopo; }
const TCollection_AsciiString& TestTopOpe_BOOP::nameS1() const {return mynameS1;}
const TCollection_AsciiString& TestTopOpe_BOOP::nameS2() const {return mynameS2;}
const Handle(TopOpeBRepBuild_HBuilder)& TestTopOpe_BOOP::HBuilder() const { return myHB; }
void TestTopOpe_BOOP::SetMode(const Standard_Integer mode){myVarsTopo.SetMode(mode);}
Standard_Integer TestTopOpe_BOOP::GetMode() const { return myVarsTopo.GetMode(); }
