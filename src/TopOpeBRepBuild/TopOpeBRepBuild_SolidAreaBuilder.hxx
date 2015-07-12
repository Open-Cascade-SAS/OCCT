// Created on: 1995-12-21
// Created by: Jean Yves LEBEY
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

#ifndef _TopOpeBRepBuild_SolidAreaBuilder_HeaderFile
#define _TopOpeBRepBuild_SolidAreaBuilder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopOpeBRepBuild_Area3dBuilder.hxx>
#include <Standard_Boolean.hxx>
class TopOpeBRepBuild_LoopSet;
class TopOpeBRepBuild_LoopClassifier;



//! The SolidAreaBuilder algorithm is used to construct Solids from a LoopSet,
//! where the Loop is the composite topological object of the boundary,
//! here wire or block of edges.
//! The LoopSet gives an iteration on Loops.
//! For each Loop  it indicates if it is on the boundary (wire) or if it
//! results from  an interference (block of edges).
//! The result of the SolidAreaBuilder is an iteration on areas.
//! An area is described by a set of Loops.
class TopOpeBRepBuild_SolidAreaBuilder  : public TopOpeBRepBuild_Area3dBuilder
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRepBuild_SolidAreaBuilder();
  
  //! Creates a SolidAreaBuilder to build Solids on
  //! the (shells,blocks of face) of <LS>, using the classifier <LC>.
  Standard_EXPORT TopOpeBRepBuild_SolidAreaBuilder(TopOpeBRepBuild_LoopSet& LS, TopOpeBRepBuild_LoopClassifier& LC, const Standard_Boolean ForceClass = Standard_False);
  
  Standard_EXPORT void InitSolidAreaBuilder (TopOpeBRepBuild_LoopSet& LS, TopOpeBRepBuild_LoopClassifier& LC, const Standard_Boolean ForceClass = Standard_False);




protected:





private:





};







#endif // _TopOpeBRepBuild_SolidAreaBuilder_HeaderFile
