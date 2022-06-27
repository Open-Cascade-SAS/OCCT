// Created on: 1993-05-07
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

#ifndef _TopOpeBRep_VPointInterIterator_HeaderFile
#define _TopOpeBRep_VPointInterIterator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopOpeBRep_PLineInter.hxx>
#include <Standard_Integer.hxx>
class TopOpeBRep_VPointInter;



class TopOpeBRep_VPointInterIterator 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRep_VPointInterIterator();
  
  Standard_EXPORT TopOpeBRep_VPointInterIterator(const TopOpeBRep_LineInter& LI);
  
  Standard_EXPORT void Init (const TopOpeBRep_LineInter& LI, const Standard_Boolean checkkeep = Standard_False);
  
  Standard_EXPORT void Init();
  
  Standard_EXPORT Standard_Boolean More() const;
  
  Standard_EXPORT void Next();
  
  Standard_EXPORT const TopOpeBRep_VPointInter& CurrentVP();
  
  Standard_EXPORT Standard_Integer CurrentVPIndex() const;
  
  Standard_EXPORT TopOpeBRep_VPointInter& ChangeCurrentVP();
  
  Standard_EXPORT TopOpeBRep_PLineInter PLineInterDummy() const;




protected:





private:



  TopOpeBRep_PLineInter myLineInter;
  Standard_Integer myVPointIndex;
  Standard_Integer myVPointNb;
  Standard_Boolean mycheckkeep;


};







#endif // _TopOpeBRep_VPointInterIterator_HeaderFile
