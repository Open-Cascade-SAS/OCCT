// Created on: 1996-01-05
// Created by: Jean Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <TopOpeBRepBuild_SolidBuilder.ixx>
#include <TopOpeBRepBuild_ShellFaceClassifier.hxx>
#include <TopOpeBRepBuild_Loop.hxx>
#include <TopOpeBRepBuild_BlockBuilder.hxx>
#include <TopOpeBRepBuild_LoopSet.hxx>


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
    Standard_DomainError::Raise("TopOpeBRepBuild_SolidBuilder:InitFace");
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
#ifdef DEB
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
