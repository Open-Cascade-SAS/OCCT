// Created on: 1993-03-23
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

#ifndef _TopOpeBRepBuild_LoopSet_HeaderFile
#define _TopOpeBRepBuild_LoopSet_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopOpeBRepBuild_ListOfLoop.hxx>
#include <TopOpeBRepBuild_ListIteratorOfListOfLoop.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class TopOpeBRepBuild_Loop;



class TopOpeBRepBuild_LoopSet 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRepBuild_LoopSet();
  
  Standard_EXPORT virtual ~TopOpeBRepBuild_LoopSet();
  
  Standard_EXPORT TopOpeBRepBuild_ListOfLoop& ChangeListOfLoop();
  
  Standard_EXPORT virtual void InitLoop();
  
  Standard_EXPORT virtual Standard_Boolean MoreLoop() const;
  
  Standard_EXPORT virtual void NextLoop();
  
  Standard_EXPORT virtual Handle(TopOpeBRepBuild_Loop) Loop() const;




protected:





private:



  TopOpeBRepBuild_ListOfLoop myListOfLoop;
  TopOpeBRepBuild_ListIteratorOfListOfLoop myLoopIterator;
  Standard_Integer myLoopIndex;
  Standard_Integer myNbLoop;


};







#endif // _TopOpeBRepBuild_LoopSet_HeaderFile
