// File:	GeomFill_TgtOnCoons.cxx
// Created:	Mon Dec  4 12:31:28 1995
// Author:	Laurent BOURESCHE
//		<lbo@phylox>


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
