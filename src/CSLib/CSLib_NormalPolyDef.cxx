// File:	SingularityAna_PolyStyle.cxx
// Created:	Fri Aug 23 11:00:24 1996
// Author:	Benoit TANNIOU
//		<bt1@sgi65>


#include <CSLib_NormalPolyDef.ixx>
#include <PLib.hxx>

//=============================================================================
CSLib_NormalPolyDef::CSLib_NormalPolyDef(const Standard_Integer k0,
						const TColStd_Array1OfReal& li)
//=============================================================================
 :myTABli(0,k0)
{
  myK0=k0;
  for(Standard_Integer i=0;i<=k0;i++)
    myTABli(i)=li(i);
}

//=============================================================================
Standard_Boolean CSLib_NormalPolyDef::Value(const Standard_Real X,
						 Standard_Real& F)
//=============================================================================
{
  F=0.0;
  Standard_Real co,si;
  co=cos(X);
  si=sin(X);
  PLib::Binomial(myK0);

  for(Standard_Integer i=0;i<=myK0;i++){
    F=F+PLib::Bin(myK0,i)*pow(co,i)*pow(si,(myK0-i))*myTABli(i);
  }
  return Standard_True;
}

//=============================================================================
Standard_Boolean CSLib_NormalPolyDef::Derivative(const Standard_Real X,
						      Standard_Real& D)
//=============================================================================
{
  D=0.0;
  Standard_Real co,si;
  co=cos(X);
  si=sin(X);
  for(Standard_Integer i=0;i<=myK0;i++){
    D=D+PLib::Bin(myK0,i)*pow(co,(i-1))*pow(si,(myK0-i-1))*(myK0*co*co-i);
  }
  return Standard_True;
}

//=============================================================================
Standard_Boolean CSLib_NormalPolyDef::Values(const Standard_Real X,
						  Standard_Real& F,
						  Standard_Real& D)
//=============================================================================
{
  F=0;
  D=0;
  Standard_Real co,si;
  co=cos(X);
  si=sin(X);
  for(Standard_Integer i=0;i<=myK0;i++){
    F=F+PLib::Bin(myK0,i)*pow(co,i)*pow(si,(myK0-i))*myTABli(i);
    D=D+PLib::Bin(myK0,i)*pow(co,(i-1))*
        pow(si,(myK0-i-1))*(myK0*co*co-i)*myTABli(i);
   }
  return Standard_True;
}





