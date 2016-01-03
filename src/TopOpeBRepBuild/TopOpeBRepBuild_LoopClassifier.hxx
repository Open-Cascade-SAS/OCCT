// Created on: 1993-03-03
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

#ifndef _TopOpeBRepBuild_LoopClassifier_HeaderFile
#define _TopOpeBRepBuild_LoopClassifier_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopAbs_State.hxx>
class TopOpeBRepBuild_Loop;


//! classify loops in order to build Areas
class TopOpeBRepBuild_LoopClassifier 
{
public:

  DEFINE_STANDARD_ALLOC

  Standard_EXPORT virtual ~TopOpeBRepBuild_LoopClassifier();
  
  //! Returns the state of loop <L1> compared with loop <L2>.
  Standard_EXPORT virtual TopAbs_State Compare (const Handle(TopOpeBRepBuild_Loop)& L1, const Handle(TopOpeBRepBuild_Loop)& L2) = 0;




protected:





private:





};







#endif // _TopOpeBRepBuild_LoopClassifier_HeaderFile
