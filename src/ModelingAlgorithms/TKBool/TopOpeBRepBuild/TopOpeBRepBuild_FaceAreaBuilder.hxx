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

#ifndef _TopOpeBRepBuild_FaceAreaBuilder_HeaderFile
#define _TopOpeBRepBuild_FaceAreaBuilder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopOpeBRepBuild_Area2dBuilder.hxx>
#include <Standard_Boolean.hxx>
class TopOpeBRepBuild_LoopSet;
class TopOpeBRepBuild_LoopClassifier;

//! The FaceAreaBuilder algorithm is used to construct Faces from a LoopSet,
//! where the Loop is the composite topological object of the boundary,
//! here wire or block of edges.
//! The LoopSet gives an iteration on Loops.
//! For each Loop  it indicates if it is on the boundary (wire) or if it
//! results from  an interference (block of edges).
//! The result of the FaceAreaBuilder is an iteration on areas.
//! An area is described by a set of Loops.
class TopOpeBRepBuild_FaceAreaBuilder : public TopOpeBRepBuild_Area2dBuilder
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopOpeBRepBuild_FaceAreaBuilder();

  //! Creates a FaceAreaBuilder to build faces on
  //! the (wires,blocks of edge) of <LS>, using the classifier <LC>.
  Standard_EXPORT TopOpeBRepBuild_FaceAreaBuilder(
    TopOpeBRepBuild_LoopSet&        LS,
    TopOpeBRepBuild_LoopClassifier& LC,
    const Standard_Boolean          ForceClass = Standard_False);

  Standard_EXPORT void InitFaceAreaBuilder(TopOpeBRepBuild_LoopSet&        LS,
                                           TopOpeBRepBuild_LoopClassifier& LC,
                                           const Standard_Boolean ForceClass = Standard_False);

protected:
private:
};

#endif // _TopOpeBRepBuild_FaceAreaBuilder_HeaderFile
