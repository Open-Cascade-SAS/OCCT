// Created on: 1996-08-23
// Created by: Benoit TANNIOU
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


#include <CSLib_NormalPolyDef.hxx>
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

  if(Abs(co) <= RealSmall() || Abs(si) <= RealSmall())
  {
    F = 0.;
    return Standard_True;
  }
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
  if(Abs(co) <= RealSmall() || Abs(si) <= RealSmall())
  {
    D = 0.;
    return Standard_True;
  }
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
  if(Abs(co) <= RealSmall() || Abs(si) <= RealSmall())
  {
    F = 0.;
    D = 0.;
    return Standard_True;
  }
  for(Standard_Integer i=0;i<=myK0;i++){
    F=F+PLib::Bin(myK0,i)*pow(co,i)*pow(si,(myK0-i))*myTABli(i);
    D=D+PLib::Bin(myK0,i)*pow(co,(i-1))*
        pow(si,(myK0-i-1))*(myK0*co*co-i)*myTABli(i);
   }
  return Standard_True;
}





