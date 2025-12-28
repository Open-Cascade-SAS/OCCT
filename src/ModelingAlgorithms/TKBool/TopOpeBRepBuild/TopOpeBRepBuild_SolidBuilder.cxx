// Created on: 1996-01-05
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

#include <TopoDS_Shape.hxx>
#include <TopOpeBRepBuild_ShellFaceClassifier.hxx>
#include <TopOpeBRepBuild_ShellFaceSet.hxx>
#include <TopOpeBRepBuild_SolidBuilder.hxx>

//=================================================================================================

TopOpeBRepBuild_SolidBuilder::TopOpeBRepBuild_SolidBuilder() {}

//=================================================================================================

TopOpeBRepBuild_SolidBuilder::TopOpeBRepBuild_SolidBuilder(TopOpeBRepBuild_ShellFaceSet& SFS,
                                                           const bool                    ForceClass)
{
  InitSolidBuilder(SFS, ForceClass);
}

//=================================================================================================

void TopOpeBRepBuild_SolidBuilder::InitSolidBuilder(TopOpeBRepBuild_ShellFaceSet& SFS,
                                                    const bool                    ForceClass)
{
  MakeLoops(SFS);
  TopOpeBRepBuild_BlockBuilder&       BB = myBlockBuilder;
  TopOpeBRepBuild_LoopSet&            LS = myLoopSet;
  TopOpeBRepBuild_ShellFaceClassifier SFC(BB);
  mySolidAreaBuilder.InitSolidAreaBuilder(LS, SFC, ForceClass);
}

//=================================================================================================

int TopOpeBRepBuild_SolidBuilder::InitSolid()
{
  int n = mySolidAreaBuilder.InitArea();
  return n;
}

//=================================================================================================

bool TopOpeBRepBuild_SolidBuilder::MoreSolid() const
{
  bool b = mySolidAreaBuilder.MoreArea();
  return b;
}

//=================================================================================================

void TopOpeBRepBuild_SolidBuilder::NextSolid()
{
  mySolidAreaBuilder.NextArea();
}

//=================================================================================================

int TopOpeBRepBuild_SolidBuilder::InitShell()
{
  int n = mySolidAreaBuilder.InitLoop();
  return n;
}

//=================================================================================================

bool TopOpeBRepBuild_SolidBuilder::MoreShell() const
{
  bool b = mySolidAreaBuilder.MoreLoop();
  return b;
}

//=================================================================================================

void TopOpeBRepBuild_SolidBuilder::NextShell()
{
  mySolidAreaBuilder.NextLoop();
}

//=================================================================================================

bool TopOpeBRepBuild_SolidBuilder::IsOldShell() const
{
  const occ::handle<TopOpeBRepBuild_Loop>& L = mySolidAreaBuilder.Loop();
  bool                                     b = L->IsShape();
  return b;
}

//=================================================================================================

const TopoDS_Shape& TopOpeBRepBuild_SolidBuilder::OldShell() const
{
  const occ::handle<TopOpeBRepBuild_Loop>& L = mySolidAreaBuilder.Loop();
  const TopoDS_Shape&                      B = L->Shape();
  return B;
}

//=================================================================================================

int TopOpeBRepBuild_SolidBuilder::InitFace()
{
  const occ::handle<TopOpeBRepBuild_Loop>& L = mySolidAreaBuilder.Loop();
  if (L->IsShape())
    throw Standard_DomainError("TopOpeBRepBuild_SolidBuilder:InitFace");
  else
  {
    myBlockIterator = L->BlockIterator();
    myBlockIterator.Initialize();
  }
  int n = myBlockIterator.Extent();
  return n;
}

//=================================================================================================

bool TopOpeBRepBuild_SolidBuilder::MoreFace() const
{
  bool b = myBlockIterator.More();
  return b;
}

//=================================================================================================

void TopOpeBRepBuild_SolidBuilder::NextFace()
{
  myBlockIterator.Next();
}

//=================================================================================================

const TopoDS_Shape& TopOpeBRepBuild_SolidBuilder::Face() const
{
#ifdef OCCT_DEBUG
//  const int i = myBlockIterator.Value(); // DEB
#endif
  const TopoDS_Shape& F = myBlockBuilder.Element(myBlockIterator);
  return F;
}

//=================================================================================================

void TopOpeBRepBuild_SolidBuilder::MakeLoops(TopOpeBRepBuild_ShapeSet& SS)
{
  TopOpeBRepBuild_BlockBuilder&                        BB = myBlockBuilder;
  NCollection_List<occ::handle<TopOpeBRepBuild_Loop>>& LL = myLoopSet.ChangeListOfLoop();

  // Build blocks on elements of SS
  BB.MakeBlock(SS);

  // make list of loop (LL) of the LoopSet
  // - on shapes of the ShapeSet (SS)
  // - on blocks of the BlockBuilder (BB)

  LL.Clear();

  // Add shapes of SS as shape loops
  for (SS.InitShapes(); SS.MoreShapes(); SS.NextShape())
  {
    const TopoDS_Shape&               S         = SS.Shape();
    occ::handle<TopOpeBRepBuild_Loop> ShapeLoop = new TopOpeBRepBuild_Loop(S);
    LL.Append(ShapeLoop);
  }

  // Add blocks of BB as block loops
  for (BB.InitBlock(); BB.MoreBlock(); BB.NextBlock())
  {
    TopOpeBRepBuild_BlockIterator     BI        = BB.BlockIterator();
    occ::handle<TopOpeBRepBuild_Loop> BlockLoop = new TopOpeBRepBuild_Loop(BI);
    LL.Append(BlockLoop);
  }
}
