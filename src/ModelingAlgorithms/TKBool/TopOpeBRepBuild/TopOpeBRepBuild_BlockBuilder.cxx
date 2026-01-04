// Created on: 1993-03-11
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

#include <TopOpeBRepBuild_BlockBuilder.hxx>
#include <TopOpeBRepBuild_BlockIterator.hxx>
#include <TopOpeBRepBuild_ShapeSet.hxx>

//=================================================================================================

TopOpeBRepBuild_BlockBuilder::TopOpeBRepBuild_BlockBuilder()
    : myIsDone(false)
{
}

//=================================================================================================

TopOpeBRepBuild_BlockBuilder::TopOpeBRepBuild_BlockBuilder(TopOpeBRepBuild_ShapeSet& SS)
    : myIsDone(false)
{
  MakeBlock(SS);
}

//=================================================================================================

void TopOpeBRepBuild_BlockBuilder::MakeBlock(TopOpeBRepBuild_ShapeSet& SS)
{
  // Compute the set of connexity blocks of elements of element set SS
  //
  // Logic :
  // - A block is a set of connex elements of SS
  // - We assume that any element of SS appears in only 1 connexity block.
  //
  // Implementation :
  // - All the elements of all the blocks are stored in map myOrientedShapeMap(M).
  // - A connexity block is a segment [f,l] of element indices of M.
  // - myBlocks is a sequence of integer; each integer is the index of the
  //   first element (in M) of a connexity block.
  // - Bounds [f,l] of a connexity block are :
  //    f = myBlocks(i)
  //    l = myBlocks(i+1) - 1

  myOrientedShapeMap.Clear();
  myOrientedShapeMapIsValid.Clear();
  myBlocks.Clear();
  myBlocksIsRegular.Clear();

  bool IsRegular;
  int  CurNei;
  int  Mextent;
  int  Eindex;

  for (SS.InitStartElements(); SS.MoreStartElements(); SS.NextStartElement())
  {

    const TopoDS_Shape& E = SS.StartElement();
    Mextent               = myOrientedShapeMap.Extent();
    Eindex                = AddElement(E);

    // E = current element of the element set SS
    // Mextent = index of last element stored in map M, before E is added.
    // Eindex  = index of E added to M : Eindex > Mextent => E is new in M

    bool EnewinM = (Eindex > Mextent);
    if (EnewinM)
    {

      // make a new block starting at element Eindex
      myBlocks.Append(Eindex);
      IsRegular = true;
      CurNei    = 0;
      // put in current block all the elements connex to E :
      // while an element E has been added to M
      //    - compute neighbours of E : N(E)
      //    - add each element N of N(E) to M

      Mextent               = myOrientedShapeMap.Extent();
      bool searchneighbours = (Eindex <= Mextent);
      while (searchneighbours)
      {

        // E = element of M on which neighbours must be searched
        // first step : Eindex = index of starting element of SS
        // next steps : Eindex = index of neighbours of starting element of SS
        const TopoDS_Shape& E1 = myOrientedShapeMap(Eindex);
        CurNei                 = SS.MaxNumberSubShape(E1);
        bool condregu          = true;
        if (CurNei > 2)
          condregu = false;
        IsRegular = IsRegular && condregu;
        // compute neighbours of E : add them to M to increase M.Extent().
        SS.InitNeighbours(E1);

        for (; SS.MoreNeighbours(); SS.NextNeighbour())
        {
          const TopoDS_Shape& N = SS.Neighbour();
          AddElement(N);
        }

        Eindex++;
        Mextent          = myOrientedShapeMap.Extent();
        searchneighbours = (Eindex <= Mextent);

      } // while (searchneighbours)
      int iiregu = IsRegular ? 1 : 0;
      myBlocksIsRegular.Append(iiregu);
    } // if (EnewinM)
  } // for ()

  // To value the l bound of the last connexity block created above,
  // we create an artificial block of value = myOrientedShapeMap.Extent() + 1
  // The real number of connexity blocks is myBlocks.Length() - 1
  Mextent = myOrientedShapeMap.Extent();
  myBlocks.Append(Mextent + 1);
  myIsDone = true;
}

//=================================================================================================

void TopOpeBRepBuild_BlockBuilder::InitBlock()
{
  myBlockIndex = 1;
}

//=================================================================================================

bool TopOpeBRepBuild_BlockBuilder::MoreBlock() const
{
  // the length of myBlocks is 1 + number of connexity blocks
  int  l = myBlocks.Length();
  bool b = (myBlockIndex < l);
  return b;
}

//=================================================================================================

void TopOpeBRepBuild_BlockBuilder::NextBlock()
{
  myBlockIndex++;
}

//=================================================================================================

TopOpeBRepBuild_BlockIterator TopOpeBRepBuild_BlockBuilder::BlockIterator() const
{
  int lower = myBlocks(myBlockIndex);
  int upper = myBlocks(myBlockIndex + 1) - 1;
  return TopOpeBRepBuild_BlockIterator(lower, upper);
}

//=================================================================================================

const TopoDS_Shape& TopOpeBRepBuild_BlockBuilder::Element(
  const TopOpeBRepBuild_BlockIterator& BI) const
{
  bool isbound = BI.More();
  if (!isbound)
    throw Standard_Failure("OutOfRange");

  int                 index = BI.Value();
  const TopoDS_Shape& E     = myOrientedShapeMap(index);
  return E;
}

const TopoDS_Shape& TopOpeBRepBuild_BlockBuilder::Element(const int index) const
{
  bool isbound = myOrientedShapeMapIsValid.IsBound(index);
  if (!isbound)
    throw Standard_Failure("OutOfRange");

  const TopoDS_Shape& E = myOrientedShapeMap(index);
  return E;
}

int TopOpeBRepBuild_BlockBuilder::Element(const TopoDS_Shape& E) const
{
  bool isbound = myOrientedShapeMap.Contains(E);
  if (!isbound)
    throw Standard_Failure("OutOfRange");

  int I = myOrientedShapeMap.FindIndex(E);
  return I;
}

//=================================================================================================

bool TopOpeBRepBuild_BlockBuilder::ElementIsValid(const TopOpeBRepBuild_BlockIterator& BI) const
{
  bool isbound = BI.More();
  if (!isbound)
    return false;

  int  Sindex  = BI.Value();
  int  isb     = myOrientedShapeMapIsValid.Find(Sindex);
  bool isvalid = isb == 1;

  return isvalid;
}

bool TopOpeBRepBuild_BlockBuilder::ElementIsValid(const int Sindex) const
{
  bool isbound = myOrientedShapeMapIsValid.IsBound(Sindex);
  if (!isbound)
    return false;

  int  isb     = myOrientedShapeMapIsValid.Find(Sindex);
  bool isvalid = isb == 1;

  return isvalid;
}

//=================================================================================================

int TopOpeBRepBuild_BlockBuilder::AddElement(const TopoDS_Shape& S)
{
  int Sindex = myOrientedShapeMap.Add(S);
  myOrientedShapeMapIsValid.Bind(Sindex, 1);

  return Sindex;
}

//=================================================================================================

void TopOpeBRepBuild_BlockBuilder::SetValid(const TopOpeBRepBuild_BlockIterator& BI,
                                            const bool                           isvalid)
{
  bool isbound = BI.More();
  if (!isbound)
    return;

  int Sindex = BI.Value();
  int i      = (isvalid) ? 1 : 0;
  myOrientedShapeMapIsValid.Bind(Sindex, i);
}

void TopOpeBRepBuild_BlockBuilder::SetValid(const int Sindex, const bool isvalid)
{
  bool isbound = myOrientedShapeMapIsValid.IsBound(Sindex);
  if (!isbound)
    return;

  int i = (isvalid) ? 1 : 0;
  myOrientedShapeMapIsValid.Bind(Sindex, i);
}

//=================================================================================================

bool TopOpeBRepBuild_BlockBuilder::CurrentBlockIsRegular()
{
  bool b = false;
  int  i = myBlocksIsRegular.Value(myBlockIndex);
  if (i == 1)
    b = true;
  return b;
}
