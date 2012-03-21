// Created on: 1996-08-23
// Created by: Benoit TANNIOU
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





