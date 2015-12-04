// Created on: 1995-12-04
// Created by: Laurent BOURESCHE
// Copyright (c) 1995-1999 Matra Datavision
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


#include <GeomFill_Boundary.hxx>
#include <GeomFill_CoonsAlgPatch.hxx>
#include <GeomFill_TgtOnCoons.hxx>
#include <gp_Vec.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomFill_TgtOnCoons,GeomFill_TgtField)

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
  Standard_Real U = 0.,V = 0.,bid = 0.;
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
  Standard_Real U = 0.,V = 0.,bid = 0.;
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
  tgsc.Added(scaln);

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
  Standard_Real U = 0.,V = 0.,bid = 0.;
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
