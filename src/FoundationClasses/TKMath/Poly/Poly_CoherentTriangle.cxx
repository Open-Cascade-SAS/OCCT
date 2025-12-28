// Created on: 2007-11-25
// Created by: Alexander GRIGORIEV
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#include <Poly_CoherentTriangle.hxx>
#include <Standard_ProgramError.hxx>

//=======================================================================
// function : Poly_CoherentTriangle()
// purpose  : Empty Constructor
//=======================================================================

Poly_CoherentTriangle::Poly_CoherentTriangle()
    : myNConnections(0)
{
  myNodes[0]            = -1;
  myNodes[1]            = -1;
  myNodes[2]            = -1;
  myNodesOnConnected[0] = -1;
  myNodesOnConnected[1] = -1;
  myNodesOnConnected[2] = -1;
  mypConnected[0]       = nullptr;
  mypConnected[1]       = nullptr;
  mypConnected[2]       = nullptr;
  mypLink[0]            = nullptr;
  mypLink[1]            = nullptr;
  mypLink[2]            = nullptr;
}

//=================================================================================================

Poly_CoherentTriangle::Poly_CoherentTriangle(const int iNode0, const int iNode1, const int iNode2)
    : myNConnections(0)
{
  myNodes[0]            = iNode0;
  myNodes[1]            = iNode1;
  myNodes[2]            = iNode2;
  myNodesOnConnected[0] = -1;
  myNodesOnConnected[1] = -1;
  myNodesOnConnected[2] = -1;
  mypConnected[0]       = nullptr;
  mypConnected[1]       = nullptr;
  mypConnected[2]       = nullptr;
  mypLink[0]            = nullptr;
  mypLink[1]            = nullptr;
  mypLink[2]            = nullptr;
}

//=================================================================================================

bool Poly_CoherentTriangle::SetConnection(const int iConn, Poly_CoherentTriangle& theTr)
{
  bool             aResult(false);
  static const int II[] = {2, 0, 1, 2, 0};

  if (theTr.Node(0) == myNodes[II[iConn + 2]])
  {
    if (theTr.Node(2) == myNodes[II[iConn]])
    {
      RemoveConnection(iConn);
      myNodesOnConnected[iConn] = theTr.Node(1);
      mypConnected[iConn]       = &theTr;
      myNConnections++;
      theTr.RemoveConnection(1);
      theTr.myNodesOnConnected[1] = myNodes[iConn];
      theTr.mypConnected[1]       = this;
      theTr.myNConnections++;
      aResult = true;
    }
  }
  else if (theTr.Node(1) == myNodes[II[iConn + 2]])
  {
    if (theTr.Node(0) == myNodes[II[iConn]])
    {
      RemoveConnection(iConn);
      myNodesOnConnected[iConn] = theTr.Node(2);
      mypConnected[iConn]       = &theTr;
      myNConnections++;
      theTr.RemoveConnection(2);
      theTr.myNodesOnConnected[2] = myNodes[iConn];
      theTr.mypConnected[2]       = this;
      theTr.myNConnections++;
      aResult = true;
    }
  }
  else if (theTr.Node(2) == myNodes[II[iConn + 2]])
  {
    if (theTr.Node(1) == myNodes[II[iConn]])
    {
      RemoveConnection(iConn);
      myNodesOnConnected[iConn] = theTr.Node(0);
      mypConnected[iConn]       = &theTr;
      myNConnections++;
      theTr.RemoveConnection(0);
      theTr.myNodesOnConnected[0] = myNodes[iConn];
      theTr.mypConnected[0]       = this;
      theTr.myNConnections++;
      aResult = true;
    }
  }
  return aResult;
}

//=================================================================================================

bool Poly_CoherentTriangle::SetConnection(Poly_CoherentTriangle& theTr)
{
  bool aResult(false);
  if (myNodes[0] == theTr.Node(0))
  {
    if (myNodes[1] == theTr.Node(2) && mypConnected[2] != &theTr)
    {
      RemoveConnection(2);
      myNodesOnConnected[2] = theTr.Node(1);
      mypConnected[2]       = &theTr;
      myNConnections++;
      theTr.RemoveConnection(1);
      theTr.myNodesOnConnected[1] = myNodes[2];
      theTr.mypConnected[1]       = this;
      theTr.myNConnections++;
      aResult = true;
    }
    else if (myNodes[2] == theTr.Node(1) && mypConnected[1] != &theTr)
    {
      RemoveConnection(1);
      myNodesOnConnected[1] = theTr.Node(2);
      mypConnected[1]       = &theTr;
      myNConnections++;
      theTr.RemoveConnection(2);
      theTr.myNodesOnConnected[2] = myNodes[1];
      theTr.mypConnected[2]       = this;
      theTr.myNConnections++;
      aResult = true;
    }
  }
  else if (myNodes[0] == theTr.Node(1))
  {
    if (myNodes[1] == theTr.Node(0) && mypConnected[2] != &theTr)
    {
      RemoveConnection(2);
      myNodesOnConnected[2] = theTr.Node(2);
      mypConnected[2]       = &theTr;
      myNConnections++;
      theTr.RemoveConnection(2);
      theTr.myNodesOnConnected[2] = myNodes[2];
      theTr.mypConnected[2]       = this;
      theTr.myNConnections++;
      aResult = true;
    }
    else if (myNodes[2] == theTr.Node(2) && mypConnected[1] != &theTr)
    {
      RemoveConnection(1);
      myNodesOnConnected[1] = theTr.Node(0);
      mypConnected[1]       = &theTr;
      myNConnections++;
      theTr.RemoveConnection(0);
      theTr.myNodesOnConnected[0] = myNodes[1];
      theTr.mypConnected[0]       = this;
      theTr.myNConnections++;
      aResult = true;
    }
  }
  else if (myNodes[0] == theTr.Node(2))
  {
    if (myNodes[1] == theTr.Node(1) && mypConnected[2] != &theTr)
    {
      RemoveConnection(2);
      myNodesOnConnected[2] = theTr.Node(0);
      mypConnected[2]       = &theTr;
      myNConnections++;
      theTr.RemoveConnection(0);
      theTr.myNodesOnConnected[0] = myNodes[2];
      theTr.mypConnected[0]       = this;
      theTr.myNConnections++;
      aResult = true;
    }
    else if (myNodes[2] == theTr.Node(0) && mypConnected[1] != &theTr)
    {
      RemoveConnection(1);
      myNodesOnConnected[1] = theTr.Node(1);
      mypConnected[1]       = &theTr;
      myNConnections++;
      theTr.RemoveConnection(1);
      theTr.myNodesOnConnected[1] = myNodes[1];
      theTr.mypConnected[1]       = this;
      theTr.myNConnections++;
      aResult = true;
    }
  }
  else if (mypConnected[0] != &theTr)
  {
    if (myNodes[1] == theTr.Node(0) && myNodes[2] == theTr.Node(2))
    {
      RemoveConnection(0);
      myNodesOnConnected[0] = theTr.Node(1);
      mypConnected[0]       = &theTr;
      myNConnections++;
      theTr.RemoveConnection(1);
      theTr.myNodesOnConnected[1] = myNodes[0];
      theTr.mypConnected[1]       = this;
      theTr.myNConnections++;
      aResult = true;
    }
    else if (myNodes[1] == theTr.Node(2) && myNodes[2] == theTr.Node(1))
    {
      RemoveConnection(0);
      myNodesOnConnected[0] = theTr.Node(0);
      mypConnected[0]       = &theTr;
      myNConnections++;
      theTr.RemoveConnection(0);
      theTr.myNodesOnConnected[0] = myNodes[0];
      theTr.mypConnected[0]       = this;
      theTr.myNConnections++;
      aResult = true;
    }
    else if (myNodes[1] == theTr.Node(1) && myNodes[2] == theTr.Node(0))
    {
      RemoveConnection(0);
      myNodesOnConnected[0] = theTr.Node(2);
      mypConnected[0]       = &theTr;
      myNConnections++;
      theTr.RemoveConnection(2);
      theTr.myNodesOnConnected[2] = myNodes[0];
      theTr.mypConnected[2]       = this;
      theTr.myNConnections++;
      aResult = true;
    }
  }
  return aResult;
}

//=================================================================================================

void Poly_CoherentTriangle::RemoveConnection(const int iConn)
{
  Poly_CoherentTriangle* pConnectedTri = const_cast<Poly_CoherentTriangle*>(mypConnected[iConn]);
  if (pConnectedTri)
  {
    int iConn1(0);
    if (pConnectedTri->mypConnected[0] != this)
    {
      if (pConnectedTri->mypConnected[1] == this)
        iConn1 = 1;
      else if (pConnectedTri->mypConnected[2] == this)
        iConn1 = 2;
      else
        throw Standard_ProgramError("Poly_CoherentTriangle::RemoveConnection: "
                                    "wrong connection between triangles");
    }
    pConnectedTri->mypConnected[iConn1]       = nullptr;
    pConnectedTri->myNodesOnConnected[iConn1] = -1;
    pConnectedTri->myNConnections--;
    mypConnected[iConn]       = nullptr;
    myNodesOnConnected[iConn] = -1;
    myNConnections--;
  }
}

//=================================================================================================

bool Poly_CoherentTriangle::RemoveConnection(Poly_CoherentTriangle& theTri)
{
  const int iConn = FindConnection(theTri);
  if (iConn >= 0)
    RemoveConnection(iConn);
  return (iConn >= 0);
}

//=================================================================================================

int Poly_CoherentTriangle::FindConnection(const Poly_CoherentTriangle& theTri) const
{
  int aResult;
  if (mypConnected[0] == &theTri)
    aResult = 0;
  else if (mypConnected[1] == &theTri)
    aResult = 1;
  else if (mypConnected[2] == &theTri)
    aResult = 2;
  else
    aResult = -1;
  return aResult;
}
