// Created on: 1995-12-04
// Created by: Laurent BOURESCHE
// Copyright (c) 1995-1999 Matra Datavision
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



#include <GeomFill_CoonsAlgPatch.ixx>
#include <Law_Linear.hxx>
#include <gp_XYZ.hxx>

//=======================================================================
//function : GeomFill_CoonsAlgPatch
//purpose  : 
//=======================================================================

GeomFill_CoonsAlgPatch::GeomFill_CoonsAlgPatch
(const Handle(GeomFill_Boundary)& B1, 
 const Handle(GeomFill_Boundary)& B2, 
 const Handle(GeomFill_Boundary)& B3, 
 const Handle(GeomFill_Boundary)& B4)
{
  bound[0] = B1; bound[1] = B2; bound[2] = B3; bound[3] = B4; 
  Standard_Real deb0, deb1, fin0, fin1;
  B2->Bounds(deb1,fin1);
  a[0] = new Law_Linear();
  (*((Handle_Law_Linear*) &a[0]))->Set(deb1,1.,fin1,0.);
  B1->Bounds(deb0,fin0);
  a[1] = new Law_Linear();
  (*((Handle_Law_Linear*) &a[1]))->Set(deb0,0.,fin0,1.);

  gp_XYZ temp;
  temp = B4->Value(deb1).XYZ().Added(B1->Value(deb0).XYZ());
  temp.Multiply(0.5);
  c[0].SetXYZ(temp);
  temp = B1->Value(fin0).XYZ().Added(B2->Value(deb1).XYZ());
  temp.Multiply(0.5);
  c[1].SetXYZ(temp);
  temp = B2->Value(fin1).XYZ().Added(B3->Value(fin0).XYZ());
  temp.Multiply(0.5);
  c[2].SetXYZ(temp);
  temp = B3->Value(deb0).XYZ().Added(B4->Value(fin1).XYZ());
  temp.Multiply(0.5);
  c[3].SetXYZ(temp);
}


//=======================================================================
//function : SetFunc
//purpose  : 
//=======================================================================

void GeomFill_CoonsAlgPatch::SetFunc(const Handle(Law_Function)& f1, 
				     const Handle(Law_Function)& f2)
{
  a[0] = f1;
  a[1] = f2;
}


//=======================================================================
//function : Func
//purpose  : 
//=======================================================================

void GeomFill_CoonsAlgPatch::Func(Handle(Law_Function)& f1, 
				  Handle(Law_Function)& f2)const 
{
  f1 = a[0];
  f2 = a[1];
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

//gp_Pnt GeomFill_CoonsAlgPatch::Value(const Standard_Real U, 
gp_Pnt GeomFill_CoonsAlgPatch::Value(const Standard_Real , 
				     const Standard_Real V) const 
{
  Standard_Real a0,a1,a2,a3;
  a0 = a[0]->Value(V);
  a1 = a[1]->Value(V);
  a2 = 1. - a0;
  a3 = 1. - a1;
  gp_XYZ cor,cortemp;

  cor = bound[0]->Value(V).XYZ();
  cor.Multiply(a0);

  cortemp = bound[1]->Value(V).XYZ();
  cortemp.Multiply(a1);
  cor.Add(cortemp);

  cortemp = bound[2]->Value(V).XYZ();
  cortemp.Multiply(a2);
  cor.Add(cortemp);

  cortemp = bound[3]->Value(V).XYZ();
  cortemp.Multiply(a3);
  cor.Add(cortemp);

  cortemp = c[0].XYZ();
  cortemp.Multiply(-a0*a3);
  cor.Add(cortemp);
  
  cortemp = c[1].XYZ();
  cortemp.Multiply(-a0*a1);
  cor.Add(cortemp);
  
  cortemp = c[2].XYZ();
  cortemp.Multiply(-a1*a2);
  cor.Add(cortemp);
  
  cortemp = c[3].XYZ();
  cortemp.Multiply(-a2*a3);
  cor.Add(cortemp);

  return gp_Pnt(cor);
}


//=======================================================================
//function : D1U
//purpose  : 
//=======================================================================

gp_Vec GeomFill_CoonsAlgPatch::D1U(const Standard_Real U, 
				   const Standard_Real V) const 
{
  Standard_Real a0,a1,a2,a3,bid;
  a0 = a[0]->Value(V);
  a[1]->D1(U,bid,a1);
  a2 = 1 - a0;
  a3 = -a1;
  gp_XYZ cor,cortemp;
  gp_Pnt pbid;
  gp_Vec vbid;

  bound[0]->D1(U,pbid,vbid);
  cor = vbid.XYZ();
  cor.Multiply(a0);

  cortemp = bound[1]->Value(V).XYZ();
  cortemp.Multiply(a1);
  cor.Add(cortemp);

  bound[2]->D1(U,pbid,vbid);
  cortemp = vbid.XYZ();
  cortemp.Multiply(a2);
  cor.Add(cortemp);

  cortemp = bound[3]->Value(V).XYZ();
  cortemp.Multiply(a3);
  cor.Add(cortemp);

  cortemp = c[0].XYZ();
  cortemp.Multiply(-a0*a3);
  cor.Add(cortemp);
  
  cortemp = c[1].XYZ();
  cortemp.Multiply(-a0*a1);
  cor.Add(cortemp);
  
  cortemp = c[2].XYZ();
  cortemp.Multiply(-a1*a2);
  cor.Add(cortemp);
  
  cortemp = c[3].XYZ();
  cortemp.Multiply(-a2*a3);
  cor.Add(cortemp);
  
  vbid.SetXYZ(cor);
  return vbid;
}


//=======================================================================
//function : D1V
//purpose  : 
//=======================================================================

gp_Vec GeomFill_CoonsAlgPatch::D1V(const Standard_Real U, 
				   const Standard_Real V) const 
{
  Standard_Real a0,a1,a2,a3,bid;
  a[0]->D1(V,bid,a0);
  a1 = a[1]->Value(U);
  a2 = -a0;
  a3 = 1. - a1;
  gp_XYZ cor,cortemp;
  gp_Pnt pbid;
  gp_Vec vbid;

  cor = bound[0]->Value(U).XYZ();
  cor.Multiply(a0);

  bound[1]->D1(V,pbid,vbid);
  cortemp = vbid.XYZ();
  cortemp.Multiply(a1);
  cor.Add(cortemp);

  cortemp = bound[2]->Value(U).XYZ();
  cortemp.Multiply(a2);
  cor.Add(cortemp);

  bound[3]->D1(V,pbid,vbid);
  cortemp = vbid.XYZ();
  cortemp.Multiply(a3);
  cor.Add(cortemp);

  cortemp = c[0].XYZ();
  cortemp.Multiply(-a0*a3);
  cor.Add(cortemp);
  
  cortemp = c[1].XYZ();
  cortemp.Multiply(-a0*a1);
  cor.Add(cortemp);
  
  cortemp = c[2].XYZ();
  cortemp.Multiply(-a1*a2);
  cor.Add(cortemp);
  
  cortemp = c[3].XYZ();
  cortemp.Multiply(-a2*a3);
  cor.Add(cortemp);
  
  vbid.SetXYZ(cor);
  return vbid;
}


//=======================================================================
//function : DUV
//purpose  : 
//=======================================================================

gp_Vec GeomFill_CoonsAlgPatch::DUV(const Standard_Real U, 
				   const Standard_Real V) const 
{
  Standard_Real a0,a1,a2,a3,bid;
  a[0]->D1(V,bid,a0);
  a[1]->D1(U,bid,a1);
  a2 = -a0;
  a3 = -a1;

  gp_XYZ cor,cortemp;
  gp_Pnt pbid;
  gp_Vec vbid;

  bound[0]->D1(U,pbid,vbid);
  cor = vbid.XYZ();
  cor.Multiply(a0);

  bound[1]->D1(V,pbid,vbid);
  cortemp = vbid.XYZ();
  cortemp.Multiply(a1);
  cor.Add(cortemp);

  bound[2]->D1(U,pbid,vbid);
  cortemp = vbid.XYZ();
  cortemp.Multiply(a2);
  cor.Add(cortemp);

  bound[3]->D1(V,pbid,vbid);
  cortemp = vbid.XYZ();
  cortemp.Multiply(a3);
  cor.Add(cortemp);

  cortemp = c[0].XYZ();
  cortemp.Multiply(-a0*a3);
  cor.Add(cortemp);
  
  cortemp = c[1].XYZ();
  cortemp.Multiply(-a0*a1);
  cor.Add(cortemp);
  
  cortemp = c[2].XYZ();
  cortemp.Multiply(-a1*a2);
  cor.Add(cortemp);
  
  cortemp = c[3].XYZ();
  cortemp.Multiply(-a2*a3);
  cor.Add(cortemp);
  
  vbid.SetXYZ(cor);
  return vbid;
}


//=======================================================================
//function : Bound
//purpose  : 
//=======================================================================

const Handle(GeomFill_Boundary)& GeomFill_CoonsAlgPatch::Bound
(const Standard_Integer I) const 
{
  return bound[I];
}


//=======================================================================
//function : Corner
//purpose  : 
//=======================================================================

const gp_Pnt& GeomFill_CoonsAlgPatch::Corner(const Standard_Integer I) const 
{
  return c[I];
}

//=======================================================================
//function : Func
//purpose  : 
//=======================================================================

const Handle(Law_Function)& GeomFill_CoonsAlgPatch::Func
(const Standard_Integer I)const
{
  return a[I];
}
