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
#include <TopOpeBRepBuild_BlockBuilder.hxx>
#include <TopOpeBRepBuild_Loop.hxx>
#include <TopOpeBRepBuild_LoopSet.hxx>
#include <TopOpeBRepBuild_ShapeSet.hxx>
#include <TopOpeBRepBuild_ShellFaceClassifier.hxx>
#include <TopOpeBRepBuild_ShellFaceSet.hxx>
#include <TopOpeBRepBuild_SolidBuilder.hxx>

//=======================================================================
//function : TopOpeBRepBuild_SolidBuilder
//purpose  : 
//=======================================================================
TopOpeBRepBuild_SolidBuilder::TopOpeBRepBuild_SolidBuilder()
{
}

//=======================================================================
//function : TopOpeBRepBuild_SolidBuilder
//purpose  : 
//=======================================================================

TopOpeBRepBuild_SolidBuilder::TopOpeBRepBuild_SolidBuilder
(TopOpeBRepBuild_ShellFaceSet& SFS, const Standard_Boolean ForceClass)
{
  InitSolidBuilder(SFS,ForceClass);
}

//=======================================================================
//function : InitSolidBuilder
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_SolidBuilder::InitSolidBuilder
(TopOpeBRepBuild_ShellFaceSet& SFS, const Standard_Boolean ForceClass)
{
  MakeLoops(SFS);
  TopOpeBRepBuild_BlockBuilder& BB = myBlockBuilder;
  TopOpeBRepBuild_LoopSet& LS = myLoopSet;
  TopOpeBRepBuild_ShellFaceClassifier SFC(BB);
  mySolidAreaBuilder.InitSolidAreaBuilder(LS,SFC,ForceClass);
}

//=======================================================================
//function : InitSolid
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRepBuild_SolidBuilder::InitSolid()
{
  Standard_Integer n = mySolidAreaBuilder.InitArea();
  return n;
}

//=======================================================================
//function : MoreSolid
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepBuild_SolidBuilder::MoreSolid() const
{
  Standard_Boolean b = mySolidAreaBuilder.MoreArea();
  return b;
}

//=======================================================================
//function : NextSolid
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_SolidBuilder::NextSolid()
{
  mySolidAreaBuilder.NextArea();
}

//=======================================================================
//function : InitShell
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRepBuild_SolidBuilder::InitShell()
{
  Standard_Integer n = mySolidAreaBuilder.InitLoop();
  return n;
}

//=======================================================================
//function : MoreShell
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepBuild_SolidBuilder::MoreShell() const
{
  Standard_Boolean b = mySolidAreaBuilder.MoreLoop();
  return b;
}

//=======================================================================
//function : NextShell
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_SolidBuilder::NextShell()
{
  mySolidAreaBuilder.NextLoop();
}

//=======================================================================
//function : IsOldShell
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepBuild_SolidBuilder::IsOldShell() const
{
  const Handle(TopOpeBRepBuild_Loop)& L = mySolidAreaBuilder.Loop();
  Standard_Boolean b = L->IsShape();
  return b;
}

//=======================================================================
//function : OldShell
//purpose  : 
//=======================================================================

const TopoDS_Shape& TopOpeBRepBuild_SolidBuilder::OldShell() const
{
  const Handle(TopOpeBRepBuild_Loop)& L = mySolidAreaBuilder.Loop();
  const TopoDS_Shape& B = L->Shape();
  return B;
}

//=======================================================================
//function : InitFace
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRepBuild_SolidBuilder::InitFace()
{
  const Handle(TopOpeBRepBuild_Loop)& L = mySolidAreaBuilder.Loop();
  if ( L->IsShape() )
    throw Standard_DomainError("TopOpeBRepBuild_SolidBuilder:InitFace");
  else {
    myBlockIterator = L->BlockIterator();
    myBlockIterator.Initialize();
  }
  Standard_Integer n = myBlockIterator.Extent();
  return n;
}

//=======================================================================
//function : MoreFace
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepBuild_SolidBuilder::MoreFace() const
{
  Standard_Boolean b = myBlockIterator.More();
  return b;
}

//=======================================================================
//function : NextFace
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_SolidBuilder::NextFace()
{
  myBlockIterator.Next();
}

//=======================================================================
//function : Face
//purpose  : 
//=======================================================================

const TopoDS_Shape& TopOpeBRepBuild_SolidBuilder::Face() const
{
#ifdef OCCT_DEBUG
//  const Standard_Integer i = myBlockIterator.Value(); // DEB
#endif
  const TopoDS_Shape& F = myBlockBuilder.Element(myBlockIterator);
  return F;
}

//=======================================================================
//function : MakeLoops
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_SolidBuilder::MakeLoops(TopOpeBRepBuild_ShapeSet& SS)
{
  TopOpeBRepBuild_BlockBuilder& BB = myBlockBuilder;
  TopOpeBRepBuild_ListOfLoop& LL = myLoopSet.ChangeListOfLoop();

  // Build blocks on elements of SS
  BB.MakeBlock(SS);

  // make list of loop (LL) of the LoopSet
  // - on shapes of the ShapeSet (SS)
  // - on blocks of the BlockBuilder (BB)

  LL.Clear();
  
  // Add shapes of SS as shape loops
  for(SS.InitShapes(); SS.MoreShapes(); SS.NextShape()) {
    const TopoDS_Shape& S = SS.Shape();
    Handle(TopOpeBRepBuild_Loop) ShapeLoop = new TopOpeBRepBuild_Loop(S);
    LL.Append(ShapeLoop);
  }
  
  // Add blocks of BB as block loops
  for (BB.InitBlock(); BB.MoreBlock(); BB.NextBlock()) {
    TopOpeBRepBuild_BlockIterator BI = BB.BlockIterator();
    Handle(TopOpeBRepBuild_Loop) BlockLoop = new TopOpeBRepBuild_Loop(BI);
    LL.Append(BlockLoop);
  }

}
