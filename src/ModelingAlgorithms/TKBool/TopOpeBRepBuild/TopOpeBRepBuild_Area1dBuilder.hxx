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

#ifndef _TopOpeBRepBuild_Area1dBuilder_HeaderFile
#define _TopOpeBRepBuild_Area1dBuilder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopOpeBRepBuild_AreaBuilder.hxx>
#include <Standard_Boolean.hxx>
#include <TopOpeBRepBuild_Loop.hxx>
#include <NCollection_List.hxx>
class TopOpeBRepBuild_PaveSet;
class TopOpeBRepBuild_PaveClassifier;
class TopOpeBRepBuild_LoopSet;
class TopOpeBRepBuild_LoopClassifier;
class TopOpeBRepBuild_Loop;

class TopOpeBRepBuild_Area1dBuilder : public TopOpeBRepBuild_AreaBuilder
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopOpeBRepBuild_Area1dBuilder();

  //! Creates a Area1dBuilder to find the areas of
  //! the shapes described by <LS> using the classifier <LC>.
  Standard_EXPORT TopOpeBRepBuild_Area1dBuilder(TopOpeBRepBuild_PaveSet&        LS,
                                                TopOpeBRepBuild_PaveClassifier& LC,
                                                const bool                      ForceClass = false);

  //! Sets a Area1dBuilder to find the areas of
  //! the shapes described by <LS> using the classifier <LC>.
  Standard_EXPORT void InitAreaBuilder(TopOpeBRepBuild_LoopSet&        LS,
                                       TopOpeBRepBuild_LoopClassifier& LC,
                                       const bool                      ForceClass = false) override;

  Standard_EXPORT void ADD_Loop_TO_LISTOFLoop(
    const occ::handle<TopOpeBRepBuild_Loop>&             L,
    NCollection_List<occ::handle<TopOpeBRepBuild_Loop>>& LOL,
    void* const                                          s = nullptr) const override;

  Standard_EXPORT void REM_Loop_FROM_LISTOFLoop(
    NCollection_List<occ::handle<TopOpeBRepBuild_Loop>>::Iterator& ITLOL,
    NCollection_List<occ::handle<TopOpeBRepBuild_Loop>>&           LOL,
    void* const                                                    s = nullptr) const override;

  Standard_EXPORT void ADD_LISTOFLoop_TO_LISTOFLoop(
    NCollection_List<occ::handle<TopOpeBRepBuild_Loop>>& LOL1,
    NCollection_List<occ::handle<TopOpeBRepBuild_Loop>>& LOL2,
    void* const                                          s  = nullptr,
    void* const                                          s1 = nullptr,
    void* const                                          s2 = nullptr) const override;

  Standard_EXPORT static void DumpList(
    const NCollection_List<occ::handle<TopOpeBRepBuild_Loop>>& L);
};

#endif // _TopOpeBRepBuild_Area1dBuilder_HeaderFile
