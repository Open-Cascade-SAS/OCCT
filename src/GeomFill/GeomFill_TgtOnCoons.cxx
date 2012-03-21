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



#include <GeomFill_TgtOnCoons.ixx>
#include <GeomFill_Boundary.hxx>

//=======================================================================
//function : GeomFill_TgtOnCoons
//purpose  : 
//=======================================================================

GeomFill_TgtOnCoons::GeomFill_TgtOnCoons
(const Handle(GeomFill_CoonsAlgPatch)& K, 
 const Standard_Integer I) : myK(K), ibound(I)
{
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

gp_Vec GeomFill_TgtOnCoons::Value(const Standard_Real W) const 
{
  Standard_Real U,V,bid;
  switch (ibound){
  case 0 :
    myK->Bound(1)->Bounds(V,bid);
    break;
  case 1 :
    myK->Bound(0)->Bounds(bid,U);
    break;
  case 2 :
    myK->Bound(1)->Bounds(bid,V);
    break;
  case 3 :
    myK->Bound(0)->Bounds(U,bid);
    break;
  }

  gp_Vec tgk;

  switch (ibound){
  case 0 :
  case 2 :
    U = W;
    tgk = myK->D1V(U,V);
    break;
  case 1 :
  case 3 :
    V = W;
    tgk = myK->D1U(U,V);
    break;
  }

  gp_Vec n = myK->Bound(ibound)->Norm(W);
  Standard_Real scal = tgk.Dot(n);
  n.Multiply(-scal);
  tgk.Add(n);
  return tgk;
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

gp_Vec GeomFill_TgtOnCoons::D1(const Standard_Real W) const 
{
  Standard_Real U,V,bid;
  switch (ibound){
  case 0 :
    myK->Bound(1)->Bounds(V,bid);
    break;
  case 1 :
    myK->Bound(0)->Bounds(bid,U);
    break;
  case 2 :
    myK->Bound(1)->Bounds(bid,V);
    break;
  case 3 :
    myK->Bound(0)->Bounds(U,bid);
    break;
  }

  gp_Vec tgsc,dtgsc;

  switch (ibound){
  case 0 :
  case 2 :
    U = W;
    tgsc = myK->D1V(U,V);
    break;
  case 1 :
  case 3 :
    V = W;
    tgsc = myK->D1U(U,V);
    break;
  }
  dtgsc = myK->DUV(U,V);
  
  gp_Vec n, dn;
  myK->Bound(ibound)->D1Norm(W,n,dn);

  Standard_Real scal = tgsc.Dot(n);
  gp_Vec scaln = n.Multiplied(-scal);
#ifdef DEB
  gp_Vec tpur = tgsc.Added(scaln);
#else
  tgsc.Added(scaln);
#endif

  gp_Vec scaldn = dn.Multiplied(-scal);

  Standard_Real scal2 = -dtgsc.Dot(n) - tgsc.Dot(dn);
  gp_Vec temp = n.Multiplied(scal2);

  temp.Add(scaldn);
  gp_Vec dtpur = dtgsc.Added(temp);

  return dtpur; 
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void GeomFill_TgtOnCoons::D1(const Standard_Real W, gp_Vec& T, gp_Vec& DT) const 
{
  Standard_Real U,V,bid;
  switch (ibound){
  case 0 :
    myK->Bound(1)->Bounds(V,bid);
    break;
  case 1 :
    myK->Bound(0)->Bounds(bid,U);
    break;
  case 2 :
    myK->Bound(1)->Bounds(bid,V);
    break;
  case 3 :
    myK->Bound(0)->Bounds(U,bid);
    break;
  }

  gp_Vec tgsc,dtgsc;

  switch (ibound){
  case 0 :
  case 2 :
    U = W;
    tgsc = myK->D1V(U,V);
    break;
  case 1 :
  case 3 :
    V = W;
    tgsc = myK->D1U(U,V);
    break;
  }
  dtgsc = myK->DUV(U,V);
  
  gp_Vec n, dn;
  myK->Bound(ibound)->D1Norm(W,n,dn);

  Standard_Real scal = tgsc.Dot(n);
  gp_Vec scaln = n.Multiplied(-scal);
  T = tgsc.Added(scaln);

  gp_Vec scaldn = dn.Multiplied(-scal);

  Standard_Real scal2 = -dtgsc.Dot(n) - tgsc.Dot(dn);
  gp_Vec temp = n.Multiplied(scal2);

  temp.Add(scaldn);
  DT = dtgsc.Added(temp);
}
