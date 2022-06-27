// Created on: 1996-02-13
// Created by: Jean Yves LEBEY
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

#ifndef _TopOpeBRepBuild_GIter_HeaderFile
#define _TopOpeBRepBuild_GIter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <TopAbs_State.hxx>
#include <Standard_OStream.hxx>
class TopOpeBRepBuild_GTopo;



class TopOpeBRepBuild_GIter 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRepBuild_GIter();
  
  Standard_EXPORT TopOpeBRepBuild_GIter(const TopOpeBRepBuild_GTopo& G);
  
  Standard_EXPORT void Init();
  
  Standard_EXPORT void Init (const TopOpeBRepBuild_GTopo& G);
  
  Standard_EXPORT Standard_Boolean More() const;
  
  Standard_EXPORT void Next();
  
  Standard_EXPORT void Current (TopAbs_State& s1, TopAbs_State& s2) const;
  
  Standard_EXPORT void Dump (Standard_OStream& OS) const;




protected:





private:

  
  Standard_EXPORT void Find();


  Standard_Integer myII;
  Standard_Address mypG;


};







#endif // _TopOpeBRepBuild_GIter_HeaderFile
