// Created on: 1997-10-22
// Created by: Jean Yves LEBEY
// Copyright (c) 1997-1999 Matra Datavision
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


#ifdef DEB
#include <TopOpeBRepDS_traceDSX.hxx>

//////////////////////////////////////////////////////////////

TopOpeBRepDS_traceDS::TopOpeBRepDS_traceDS()
{}

void TopOpeBRepDS_traceDS::SetHDS(const Handle(TopOpeBRepDS_HDataStructure)& HDS)
{
  myHDS = HDS;
  myne = 0;
  myhe.Nullify();
  Allocate();
}

void TopOpeBRepDS_traceDS::Allocate()
{
  Standard_Integer n = Nelem();
  Standard_Boolean all = (myhe.IsNull()) || (n > myne);
  if (all) {
    if (n == 0) n = 1000;
    myhe = new TColStd_HArray1OfBoolean(0,n);
    myhe->Init(Standard_False);
  }
  if (n) myne = n;
}

Standard_Integer TopOpeBRepDS_traceDS::Nelem() const 
{ 
  return 0;
}

const Handle(TopOpeBRepDS_HDataStructure)& TopOpeBRepDS_traceDS::GetHDS() const 
{
  return myHDS;
}

void TopOpeBRepDS_traceDS::Set(const Standard_Integer ie, const Standard_Boolean b)
{
  Allocate();
  if (!(ie>=1 && ie<=myne)) return;
  myhe->SetValue(ie,b); 
}

void TopOpeBRepDS_traceDS::Set(const Standard_Boolean b, Standard_Integer na, char** a)
{
  Allocate();
  Standard_Integer ia;
  if (!na) myhe->Init(b);
  else for (ia=0; ia<na; ia++) Set(atoi(a[ia]),b);
}

Standard_Boolean TopOpeBRepDS_traceDS::GetI(const Standard_Integer ie) const
{
  if (myhe.IsNull()) return Standard_False;
  if (!(ie>=1 && ie<=myne)) return Standard_False;
  return myhe->Value(ie);
}

//////////////////////////////////////////////////////////////

TopOpeBRepDS_traceCURVE::TopOpeBRepDS_traceCURVE(){}
Standard_Integer TopOpeBRepDS_traceCURVE::Nelem() const
{
  if (myHDS.IsNull()) return 0;
  else return myHDS->NbCurves();
}

//////////////////////////////////////////////////////////////

#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

TopOpeBRepDS_traceSHAPE::TopOpeBRepDS_traceSHAPE() {myns = 0;}
void TopOpeBRepDS_traceSHAPE::SetSS(const TopoDS_Shape& s1,const TopoDS_Shape& s2)
{
  TopTools_IndexedMapOfShape M;
  M.Clear();
  //JMB s1 ou s2 peut etre nul (voir ChFi3d chez lvt)
  if (!s1.IsNull()) {
    TopExp::MapShapes(s1,M);
  }
  Standard_Integer n1 = M.Extent();
  M.Clear();
  if (!s2.IsNull()) {
    TopExp::MapShapes(s2,M);
  }
  Standard_Integer n2 = M.Extent();
  myns = n1+n2;
}

void TopOpeBRepDS_traceSHAPE::SetSSHDS(const TopoDS_Shape& s1,const TopoDS_Shape& s2,const Handle(TopOpeBRepDS_HDataStructure)& HDS)
{
  SetSS(s1,s2);
  SetHDS(HDS);
}

Standard_Integer TopOpeBRepDS_traceSHAPE::Nelem() const
{
  return myns;
}

Standard_Integer TopOpeBRepDS_traceSHAPE::Index(const TopoDS_Shape& S) const
{
  if (myHDS.IsNull()) return 0;
  Standard_Integer i = myHDS->Shape(S);
  return i;
}

Standard_Boolean TopOpeBRepDS_traceSHAPE::GetS(const TopoDS_Shape& S) const
{
  if (myHDS.IsNull()) return Standard_False;
  Standard_Integer is = myHDS->Shape(S);
  Standard_Boolean b = GetI(is);
  return b;
}

//////////////////////////////////////////////////////////////
static TopOpeBRepDS_traceCURVE VCX;
Standard_EXPORT void TopOpeBRepDS_SettraceCX(const Standard_Boolean b,Standard_Integer n,char** a) { VCX.Set(b,n,a); }
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettraceCX(const Standard_Integer i) { return VCX.GetI(i); }

//////////////////////////////////////////////////////////////
static TopOpeBRepDS_traceSHAPE VSPSX;
static TopOpeBRepDS_traceSHAPE VSPSXX;

Standard_EXPORT void TopOpeBRepDS_SettraceSPSX(const Standard_Boolean b,Standard_Integer n,char** a) { VSPSX.Set(b,n,a); }
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettraceSPSX(const Standard_Integer i) { return VSPSX.GetI(i); }
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettraceSPSX(const TopoDS_Shape& S) { return VSPSX.GetS(S); }
Standard_EXPORT Standard_Integer TopOpeBRepDS_GetindexSPSX(const TopoDS_Shape& S) { return VSPSX.Index(S); }
Standard_EXPORT void TopOpeBRepDS_SettraceSPSX(const Standard_Integer i,const Standard_Boolean b) { VSPSX.Set(i,b); }
Standard_EXPORT void TopOpeBRepDS_SettraceSPSXX(const Standard_Boolean b, Standard_Integer n, char** a) { VSPSXX.Set(b,n,a);}
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettraceSPSXX(const Standard_Integer i1,const Standard_Integer i2)
{ Standard_Boolean b1 = VSPSXX.GetI(i1); Standard_Boolean b2 = VSPSXX.GetI(i2); return (b1 && b2); }
Standard_EXPORT void TopOpeBRepDS_SettraceSPSX_SS(const TopoDS_Shape& S1,const TopoDS_Shape& S2) 
{ VSPSX.SetSS(S1,S2); VSPSXX.SetSS(S1,S2); }
Standard_EXPORT void TopOpeBRepDS_SettraceSPSX_HDS(const Handle(TopOpeBRepDS_HDataStructure)& HDS)
{ VSPSX.SetHDS(HDS); VSPSXX.SetHDS(HDS); }
Standard_EXPORT void TopOpeBRepDS_SettraceSPSX_SSHDS
(const TopoDS_Shape& a,const TopoDS_Shape& b,const Handle(TopOpeBRepDS_HDataStructure)& HDS)
{ VSPSX.SetSSHDS(a,b,HDS); VSPSXX.SetSSHDS(a,b,HDS); }

#endif
