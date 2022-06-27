// Created on: 1993-11-10
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _TopOpeBRep_WPointInter_HeaderFile
#define _TopOpeBRep_WPointInter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopOpeBRep_PPntOn2S.hxx>
class gp_Pnt2d;
class gp_Pnt;



class TopOpeBRep_WPointInter 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRep_WPointInter();
  
  Standard_EXPORT void Set (const IntSurf_PntOn2S& P);
  
  Standard_EXPORT void ParametersOnS1 (Standard_Real& U, Standard_Real& V) const;
  
  Standard_EXPORT void ParametersOnS2 (Standard_Real& U, Standard_Real& V) const;
  
  Standard_EXPORT void Parameters (Standard_Real& U1, Standard_Real& V1, Standard_Real& U2, Standard_Real& V2) const;
  
  Standard_EXPORT gp_Pnt2d ValueOnS1() const;
  
  Standard_EXPORT gp_Pnt2d ValueOnS2() const;
  
  Standard_EXPORT const gp_Pnt& Value() const;
  
  Standard_EXPORT TopOpeBRep_PPntOn2S PPntOn2SDummy() const;




protected:





private:



  TopOpeBRep_PPntOn2S myPP2S;


};







#endif // _TopOpeBRep_WPointInter_HeaderFile
