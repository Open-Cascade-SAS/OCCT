// Created on: 2007-12-08
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

#include <Poly_CoherentTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <NCollection_List.hxx>
#include <Precision.hxx>
#include <Standard_ProgramError.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TShort_Array1OfShortReal.hxx> 
#include <TShort_HArray1OfShortReal.hxx> 


IMPLEMENT_STANDARD_RTTIEXT(Poly_CoherentTriangulation,Standard_Transient)

//=======================================================================
//function : Poly_CoherentTriangulation
//purpose  : Empty constructor
//=======================================================================

Poly_CoherentTriangulation::Poly_CoherentTriangulation
                        (const Handle(NCollection_BaseAllocator)& theAlloc)
  : myAlloc (theAlloc.IsNull() ? NCollection_BaseAllocator::CommonBaseAllocator()
             : theAlloc),
    myDeflection (0.)
  {}

//=======================================================================
//function : Poly_CoherentTriangulation
//purpose  : Constructor
//=======================================================================

Poly_CoherentTriangulation::Poly_CoherentTriangulation
                        (const Handle(Poly_Triangulation)& theTriangulation,
                         const Handle(NCollection_BaseAllocator)& theAlloc)
  : myAlloc (theAlloc.IsNull() ? NCollection_BaseAllocator::CommonBaseAllocator()
             : theAlloc)
{
  if (theTriangulation.IsNull() == Standard_False) {
    const TColgp_Array1OfPnt&    arrNodes    = theTriangulation->Nodes();
    const Poly_Array1OfTriangle& arrTriangle = theTriangulation->Triangles();
    const Standard_Integer nNodes = theTriangulation->NbNodes();
    const Standard_Integer nTri   = theTriangulation->NbTriangles();
    Standard_Integer i;

    // Copy the nodes
    for (i = 0; i < nNodes; i++) {
      const Standard_Integer anOldInd = i + arrNodes.Lower();
      const Standard_Integer aNewInd = SetNode(arrNodes(anOldInd).XYZ(), i);
      Poly_CoherentNode& aCopiedNode = myNodes(aNewInd);
      aCopiedNode.SetIndex(anOldInd);
    }

    // Copy the triangles
    for (i = 0; i < nTri; i++) {
      Standard_Integer iNode[3];
      arrTriangle(i + arrTriangle.Lower()).Get(iNode[0], iNode[1], iNode[2]);
      if (iNode[0] != iNode[1] && iNode[1] != iNode[2] && iNode[2] != iNode[0])
        AddTriangle (iNode[0]-1, iNode[1]-1, iNode[2]-1);
    }

    // Copy UV coordinates of nodes
    if (theTriangulation->HasUVNodes()) {
      const TColgp_Array1OfPnt2d& arrNodes2d = theTriangulation->UVNodes();
      for (i = 0; i < nNodes; i++) {
        const gp_Pnt2d& anUV = arrNodes2d(i + arrNodes2d.Lower());
        myNodes(i).SetUV(anUV.X(), anUV.Y());
      }
    }

    // Copy the normals at nodes
    if (theTriangulation->HasNormals()) {
      const TShort_Array1OfShortReal& arrNorm = theTriangulation->Normals();
      for (i = 0; i < nNodes; i++) {
        const gp_XYZ aNormal (arrNorm(3 * i + 0 + arrNorm.Lower()),
                              arrNorm(3 * i + 1 + arrNorm.Lower()),
                              arrNorm(3 * i + 2 + arrNorm.Lower()));
        myNodes(i).SetNormal(aNormal);
      }
    }
    myDeflection = theTriangulation->Deflection();
  }
}

//=======================================================================
//function : ~Poly_CoherentTriangulation()
//purpose  : Destructor
//=======================================================================

Poly_CoherentTriangulation::~Poly_CoherentTriangulation ()
{
  NCollection_Vector<Poly_CoherentNode>::Iterator anIter (myNodes);
  for (; anIter.More(); anIter.Next()) {
    anIter.ChangeValue().Clear(myAlloc);
  }
}

//=======================================================================
//function : GetTriangulation
//purpose  : 
//=======================================================================

Handle(Poly_Triangulation) Poly_CoherentTriangulation::GetTriangulation() const
{
  Handle(Poly_Triangulation) aResult;
  const Standard_Integer nNodes = NNodes();
  const Standard_Integer nTriangles = NTriangles();
  if (nNodes > 0 && nTriangles > 0) {
    aResult = new Poly_Triangulation(nNodes, nTriangles, Standard_True);
    const Handle(TShort_HArray1OfShortReal) harrNormal =
      new TShort_HArray1OfShortReal(1, 3 * nNodes);
    Standard_ShortReal * arrNormal = &harrNormal->ChangeValue(1);

    TColgp_Array1OfPnt&    arrNodes    = aResult->ChangeNodes();
    TColgp_Array1OfPnt2d&  arrNodesUV  = aResult->ChangeUVNodes();
    Poly_Array1OfTriangle& arrTriangle = aResult->ChangeTriangles();
    NCollection_Vector<Standard_Integer> vecNodeId;
    Standard_Integer i, aCount(0);
    Standard_Boolean hasUV (Standard_False);
    Standard_Boolean hasNormals (Standard_False);

    // Copy the nodes (3D and 2D coordinates)
    for (i = 0; i < myNodes.Length(); i++) {
      const Poly_CoherentNode& aNode = myNodes(i);
      if (aNode.IsFreeNode())
        vecNodeId.SetValue(i, 0);
      else {
        const gp_XYZ aNormal = aNode.GetNormal();
        arrNormal[3 * aCount + 0] = static_cast<Standard_ShortReal>(aNormal.X());
        arrNormal[3 * aCount + 1] = static_cast<Standard_ShortReal>(aNormal.Y());
        arrNormal[3 * aCount + 2] = static_cast<Standard_ShortReal>(aNormal.Z());

        vecNodeId.SetValue(i, ++aCount);
        arrNodes.SetValue(aCount, aNode);

        arrNodesUV.SetValue(aCount, gp_Pnt2d(aNode.GetU(), aNode.GetV()));
        if (aNode.GetU()*aNode.GetU() + aNode.GetV()*aNode.GetV() >
            Precision::Confusion())
          hasUV = Standard_True;
        if (aNormal.SquareModulus() >  Precision::Confusion())
          hasNormals = Standard_True;
      }
    }
    if (hasUV == Standard_False)
      aResult->RemoveUVNodes();

    // Copy the triangles
    aCount = 0;
    NCollection_Vector<Poly_CoherentTriangle>::Iterator anIterT (myTriangles);
    for (; anIterT.More(); anIterT.Next()) {
      const Poly_CoherentTriangle& aTri = anIterT.Value();
      if (aTri.IsEmpty() == Standard_False) {
        const Poly_Triangle aPolyTriangle (vecNodeId(aTri.Node(0)),
                                           vecNodeId(aTri.Node(1)),
                                           vecNodeId(aTri.Node(2)));
        arrTriangle.SetValue(++aCount, aPolyTriangle);
      }
    }
    if (hasNormals)
      aResult->SetNormals (harrNormal);

    aResult->Deflection(myDeflection);
  }

  return aResult;
}

//=======================================================================
//function : GetFreeNodes
//purpose  : Create a list of free nodes.
//=======================================================================

Standard_Boolean Poly_CoherentTriangulation::GetFreeNodes
                        (NCollection_List<Standard_Integer>& lstNodes) const
{
  lstNodes.Clear();
  Standard_Integer i;
  for (i = 0; i < myNodes.Length(); i++) {
    const Poly_CoherentNode& aNode = myNodes(i);
    if (aNode.IsFreeNode())
      lstNodes.Append(i);
  }
  return !lstNodes.IsEmpty();
}

//=======================================================================
//function : RemoveDegenerated
//purpose  : Find and remove degenerated triangles in Triangulation.
//=======================================================================

Standard_Boolean Poly_CoherentTriangulation::RemoveDegenerated
  (const Standard_Real                                         theTol,
   NCollection_List<Poly_CoherentTriangulation::TwoIntegers> * pLstRemovedNode)
{
  Standard_Boolean aResult(Standard_False);
  const Standard_Real aTol2 = theTol * theTol;
  const Standard_Integer ind0[] = {2, 0, 1, 2, 0};
  const Standard_Integer * ind = &ind0[1];
  if (pLstRemovedNode)
    pLstRemovedNode->Clear();

  //NCollection_Vector<Poly_CoherentTriangle>::Iterator anIterT(myTriangles);
  Poly_CoherentTriangulation::IteratorOfTriangle anIterT(this);
  for (; anIterT.More(); anIterT.Next()) {
    Poly_CoherentTriangle& aTri = anIterT.ChangeValue();
    Poly_CoherentNode * pNode[3] = {
      &ChangeNode(aTri.Node(0)),
      &ChangeNode(aTri.Node(1)),
      &ChangeNode(aTri.Node(2))
    };
    const Standard_Real aLen2[3] = {
      pNode[2]->Subtracted(* pNode[1]).SquareModulus(),
      pNode[0]->Subtracted(* pNode[2]).SquareModulus(),
      pNode[1]->Subtracted(* pNode[0]).SquareModulus()
    };
    for (Standard_Integer i = 0; i < 3; i++) {
      if (aLen2[i] < aTol2) {
        const Standard_Integer im1(aTri.Node(ind[i-1]));
        const Standard_Integer ip1(aTri.Node(ind[i+1]));

        // Disconnect from both neighbours
        Poly_CoherentTriangle * pTriConn[2] = {
          const_cast<Poly_CoherentTriangle *>(aTri.GetConnectedTri(ind[i-1])),
          const_cast<Poly_CoherentTriangle *>(aTri.GetConnectedTri(ind[i+1]))
        };
        RemoveTriangle(aTri);

        // Reconnect all triangles from Node(ind[i+1]) to Node(ind[i-1])        
        Poly_CoherentTriPtr::Iterator anIterConn =
          pNode[ind[i+1]]->TriangleIterator();
        for (; anIterConn.More(); anIterConn.Next()) {
          Poly_CoherentTriangle& aTriConn = anIterConn.ChangeValue();
          if (&aTriConn != &aTri) {
            if (aTriConn.Node(0) == ip1)
              aTriConn.myNodes[0] = im1;
            else if (aTriConn.Node(1) == ip1)
              aTriConn.myNodes[1] = im1;
            else if (aTriConn.Node(2) == ip1)
              aTriConn.myNodes[2] = im1;
            pNode[ind[i+1]]->RemoveTriangle(aTriConn, myAlloc);
            pNode[ind[i-1]]->AddTriangle(aTriConn, myAlloc);
          }
        }
        // Set the new mutual connection between the neighbours of the
        // removed degenerated triangle.
        if (pTriConn[0] && pTriConn[1]) {
          pTriConn[0]->SetConnection(* pTriConn[1]);
        }
        if (pLstRemovedNode) {
          pLstRemovedNode->Append(TwoIntegers(ip1, im1));
        }
        aResult = Standard_True;
        break;
      }
    }
  }
  return aResult;
}

//=======================================================================
//function : IteratorOfTriangle::IteratorOfTriangle
//purpose  : Constructor
//=======================================================================

Poly_CoherentTriangulation::IteratorOfTriangle::IteratorOfTriangle
                        (const Handle(Poly_CoherentTriangulation)& theTri)
{
  if (!theTri.IsNull()) {
    Init(theTri->myTriangles);
    while (More()) {
      const Poly_CoherentTriangle& aTri = Value();
      if (aTri.IsEmpty() == Standard_False)
        break;
      Poly_BaseIteratorOfCoherentTriangle::Next();
    }
  }
}

//=======================================================================
//function : IteratorOfTriangle::Next
//purpose  : 
//=======================================================================

void Poly_CoherentTriangulation::IteratorOfTriangle::Next()
{
  Poly_BaseIteratorOfCoherentTriangle::Next();
  while (More()) {
    const Poly_CoherentTriangle& aTri = Value();
    if (aTri.IsEmpty() == Standard_False)
      break;
    Poly_BaseIteratorOfCoherentTriangle::Next();
  }
}

//=======================================================================
//function : IteratorOfNode::IteratorOfNode
//purpose  : Constructor
//=======================================================================

Poly_CoherentTriangulation::IteratorOfNode::IteratorOfNode
                        (const Handle(Poly_CoherentTriangulation)& theTri)
{
  if (!theTri.IsNull()) {
    Init(theTri->myNodes);
    while (More()) {
      if (Value().IsFreeNode() == Standard_False)
        break;
      Poly_BaseIteratorOfCoherentNode::Next();
    }
  }
}

//=======================================================================
//function : IteratorOfNode::Next
//purpose  : 
//=======================================================================

void Poly_CoherentTriangulation::IteratorOfNode::Next()
{
  Poly_BaseIteratorOfCoherentNode::Next();
  while (More()) {
    if (Value().IsFreeNode() == Standard_False)
      break;
    Poly_BaseIteratorOfCoherentNode::Next();
  }
}

//=======================================================================
//function : IteratorOfLink::IteratorOfLink
//purpose  : Constructor
//=======================================================================

Poly_CoherentTriangulation::IteratorOfLink::IteratorOfLink
                        (const Handle(Poly_CoherentTriangulation)& theTri)
{
  if (!theTri.IsNull()) {
    Init(theTri->myLinks);
    while (More()) {
      if (Value().IsEmpty() == Standard_False)
        break;
      Poly_BaseIteratorOfCoherentLink::Next();
    }
  }
}

//=======================================================================
//function : IteratorOfLink::Next
//purpose  : 
//=======================================================================

void Poly_CoherentTriangulation::IteratorOfLink::Next()
{
  Poly_BaseIteratorOfCoherentLink::Next();
  while (More()) {
    if (Value().IsEmpty() == Standard_False)
      break;
    Poly_BaseIteratorOfCoherentLink::Next();
  }
}

//=======================================================================
//function : NNodes
//purpose  : 
//=======================================================================

Standard_Integer Poly_CoherentTriangulation::NNodes () const
{
  Standard_Integer aCount(0);
  NCollection_Vector<Poly_CoherentNode>::Iterator anIter (myNodes);
  for (; anIter.More(); anIter.Next())
    if (anIter.Value().IsFreeNode() == Standard_False)
      aCount++;
  return aCount;
}

//=======================================================================
//function : NTriangles
//purpose  : 
//=======================================================================

Standard_Integer Poly_CoherentTriangulation::NTriangles () const
{
  Standard_Integer aCount(0);
  NCollection_Vector<Poly_CoherentTriangle>::Iterator anIter (myTriangles);
  for (; anIter.More(); anIter.Next()) {
    const Poly_CoherentTriangle& aTri = anIter.Value();
    if (aTri.IsEmpty() == Standard_False)
      aCount++;
  }
  return aCount;
}

//=======================================================================
//function : NLinks
//purpose  : 
//=======================================================================

Standard_Integer Poly_CoherentTriangulation::NLinks () const
{
  Standard_Integer aCount(0);
  NCollection_Vector<Poly_CoherentLink>::Iterator anIter (myLinks);
  for (; anIter.More(); anIter.Next()) {
    if (anIter.Value().IsEmpty() == Standard_False)
      aCount++;
  }
  return aCount;
}

//=======================================================================
//function : SetNode
//purpose  : 
//=======================================================================

Standard_Integer Poly_CoherentTriangulation::SetNode
                                        (const gp_XYZ&          thePnt,
                                         const Standard_Integer iNode)
{
  Standard_Integer aResult = myNodes.Length();
  if (iNode < 0)
    myNodes.Append(Poly_CoherentNode(thePnt));
  else {
    myNodes.SetValue(iNode, Poly_CoherentNode(thePnt));
    aResult = iNode;
  }
  return aResult;
}

//=======================================================================
//function : RemoveTriangle
//purpose  : 
//=======================================================================

Standard_Boolean Poly_CoherentTriangulation::RemoveTriangle
                                (Poly_CoherentTriangle& theTriangle)
{
  Standard_Boolean aResult(Standard_False);
  for (Standard_Integer i = 0; i < 3; i++) {
    if (theTriangle.Node(i) >= 0) {
      Poly_CoherentNode& aNode = myNodes(theTriangle.Node(i));
      if (aNode.RemoveTriangle(theTriangle, myAlloc)) {
        theTriangle.myNodes[i] = -1;
        aResult = Standard_True;
      }
      // If Links exist in this Triangulation, remove or update a Link
      Poly_CoherentLink * aLink =
        const_cast<Poly_CoherentLink *>(theTriangle.mypLink[i]);
      if (aLink) {
        const Poly_CoherentTriangle * pTriOpp = theTriangle.GetConnectedTri(i);
        Standard_Boolean toRemoveLink(Standard_True);
        if (pTriOpp != 0L) {
          // A neighbour is detected. If a Link exists on it, update it,
          // otherwise remove this link
          for (Standard_Integer j = 0; j < 3; j++) {
            if (aLink == pTriOpp->GetLink(j)) {
              if (aLink->OppositeNode(0) == theTriangle.Node(i)) {
                aLink->myOppositeNode[0] = 0L;
                toRemoveLink = Standard_False;
              } else if (aLink->OppositeNode(1) == theTriangle.Node(i)) {
                aLink->myOppositeNode[1] = 0L;
                toRemoveLink = Standard_False;
              }
              break;
            }
          }
        }
        if (toRemoveLink)
          RemoveLink(* aLink);
      }
    }
    theTriangle.RemoveConnection(i);
  }
  return aResult;
}

//=======================================================================
//function : AddTriangle
//purpose  : 
//=======================================================================

Poly_CoherentTriangle * Poly_CoherentTriangulation::AddTriangle
                                        (const Standard_Integer iNode0,
                                         const Standard_Integer iNode1,
                                         const Standard_Integer iNode2)
{
  Poly_CoherentTriangle * pTriangle = 0L;
  if (iNode0 >= 0 && iNode1 >= 0 && iNode2 >= 0)
  {
    pTriangle = & myTriangles.Append(Poly_CoherentTriangle());
    ReplaceNodes(*pTriangle, iNode0, iNode1, iNode2);
  }
  return pTriangle;
}

//=======================================================================
//function : ReplaceNodes
//purpose  : 
//=======================================================================

Standard_Boolean Poly_CoherentTriangulation::ReplaceNodes
                                        (Poly_CoherentTriangle& theTriangle,
                                         const Standard_Integer iNode0,
                                         const Standard_Integer iNode1,
                                         const Standard_Integer iNode2)
{
  if (!theTriangle.IsEmpty())
    RemoveTriangle(theTriangle);
  if (iNode0 >= 0 && iNode1 >= 0 && iNode2 >= 0)
  {
    theTriangle = Poly_CoherentTriangle (iNode0, iNode1, iNode2);
    for (Standard_Integer i = 0; i < 3; i++) {
      Poly_CoherentNode& aNode = myNodes(theTriangle.Node(i));
      Poly_CoherentTriPtr::Iterator anIterT = aNode.TriangleIterator();
      for (; anIterT.More(); anIterT.Next()) {
        anIterT.ChangeValue().SetConnection(theTriangle);
      }
      aNode.AddTriangle(theTriangle, myAlloc);
    }

    // If Links exist in this Triangulation, create or update a Link
    if (myLinks.Length() > 0) {
      for (Standard_Integer i = 0; i < 3; i++) {
        const Poly_CoherentTriangle * pTriOpp = theTriangle.GetConnectedTri(i);
        Standard_Boolean toAddLink(Standard_True);
        if (pTriOpp != 0L) {
          // A neighbour is detected. If a Link exists on it, update it,
          // otherwise create a new link.
          for (Standard_Integer j = 0; j < 3; j++) {
            if (theTriangle.Node(i) == pTriOpp->GetConnectedNode(j)) {
              Poly_CoherentLink * aLink =
                const_cast<Poly_CoherentLink *>(pTriOpp->GetLink(j));
              if (aLink != 0L) {
                if (aLink->OppositeNode(0) == pTriOpp->Node(j)) {
                  aLink->myOppositeNode[1] = theTriangle.Node(i);
                  toAddLink = Standard_False;
                } else if (aLink->OppositeNode(1) == pTriOpp->Node(j)) {
                  aLink->myOppositeNode[0] = theTriangle.Node(i);
                  toAddLink = Standard_False;
                }
              }
              break;
            }
          }
        }
        if (toAddLink) {
          // No neighbor on this side, the new Link is created.
          AddLink (theTriangle, i);
        }
      }
    }
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : RemoveLink
//purpose  : 
//=======================================================================

void Poly_CoherentTriangulation::RemoveLink (Poly_CoherentLink& theLink)
{
  const Poly_CoherentTriangle * pTri[2] = { 0L, 0L };
  if (FindTriangle (theLink, pTri)) {
    for (Standard_Integer i = 0; i < 2; i++) {
      const Standard_Integer iNode = theLink.OppositeNode(i);
      if (iNode >= 0 && pTri[i] != 0L) {
        if (iNode == pTri[i]->Node(0))
          const_cast<Poly_CoherentTriangle *>(pTri[i])->mypLink[0] = 0L;
        else if (iNode == pTri[i]->Node(1))
          const_cast<Poly_CoherentTriangle *>(pTri[i])->mypLink[1] = 0L;
        else if (iNode == pTri[i]->Node(2))
          const_cast<Poly_CoherentTriangle *>(pTri[i])->mypLink[2] = 0L;
        else
          throw Standard_ProgramError("Poly_CoherentTriangulation::RemoveLink: "
                                      " wrong connectivity between triangles");
      }
    }
  } 
  theLink = Poly_CoherentLink();
}

//=======================================================================
//function : AddLink
//purpose  : 
//=======================================================================

Poly_CoherentLink * Poly_CoherentTriangulation::AddLink
                                (const Poly_CoherentTriangle& theTri,
                                 const Standard_Integer    theConn)
{
  Poly_CoherentLink * pLink = 0L;
  if (theTri.IsEmpty() == Standard_False) {
    pLink = &myLinks.Append(Poly_CoherentLink (theTri, theConn));
    const_cast<Poly_CoherentTriangle&>(theTri).mypLink[theConn] = pLink;
    const Poly_CoherentTriangle* pTriOpp = theTri.GetConnectedTri(theConn);

    if(!pTriOpp) return pLink;
    if(pTriOpp->IsEmpty()) return pLink;

    if (pTriOpp) {
      if (pTriOpp->Node(0) == theTri.GetConnectedNode(theConn))
        const_cast<Poly_CoherentTriangle *>(pTriOpp)->mypLink[0] = pLink;
      else if (pTriOpp->Node(1) == theTri.GetConnectedNode(theConn))
        const_cast<Poly_CoherentTriangle *>(pTriOpp)->mypLink[1] = pLink;
      else if (pTriOpp->Node(2) == theTri.GetConnectedNode(theConn))
        const_cast<Poly_CoherentTriangle *>(pTriOpp)->mypLink[2] = pLink;
      else
        throw Standard_ProgramError("Poly_CoherentTriangulation::AddLink: "
                                    "Bad connectivity of triangles");
    }
  }
  return pLink;
}

//=======================================================================
//function : FindTriangle
//purpose  : 
//=======================================================================

Standard_Boolean Poly_CoherentTriangulation::FindTriangle
                                (const Poly_CoherentLink&       theLink,
                                 const Poly_CoherentTriangle*   pTri[2]) const
{
  pTri[0] = 0L;
  pTri[1] = 0L;
  const Standard_Integer iNode0 = theLink.Node(0);
  if (theLink.IsEmpty() == Standard_False &&
      iNode0 < myNodes.Length() && theLink.Node(1) < myNodes.Length())
  {
    Poly_CoherentTriPtr::Iterator anIter0 = myNodes(iNode0).TriangleIterator();
    for (; anIter0.More(); anIter0.Next()) {
      const Poly_CoherentTriangle& aTri = anIter0.Value();
      if (aTri.Node(0) == iNode0) {
        if (aTri.Node(1) == theLink.Node(1))
          pTri[0] = &aTri;
        else if (aTri.Node(2) == theLink.Node(1))
          pTri[1] = &aTri;
      } else if (aTri.Node(1) == iNode0) {
        if (aTri.Node(2) == theLink.Node(1))
          pTri[0] = &aTri;
        else if (aTri.Node(0) == theLink.Node(1))
          pTri[1] = &aTri;
      } else if (aTri.Node(2) == iNode0) {
        if (aTri.Node(0) == theLink.Node(1))
          pTri[0] = &aTri;
        else if (aTri.Node(1) == theLink.Node(1))
          pTri[1] = &aTri;
      } else
        throw Standard_ProgramError("Poly_CoherentTriangulation::FindTriangle : "
                                    " Data incoherence detected");
      if (pTri[0] && pTri[1])
        break;
    }
  }
  return (pTri[0] != 0L || pTri[1] != 0L);
}

//=======================================================================
//function : ComputeLinks
//purpose  : 
//=======================================================================

Standard_Integer Poly_CoherentTriangulation::ComputeLinks ()
{
  myLinks.Clear();
  NCollection_Vector<Poly_CoherentTriangle>::Iterator anIter (myTriangles);
  for (; anIter.More(); anIter.Next()) {
    const Poly_CoherentTriangle& aTriangle = anIter.Value();

    if(aTriangle.IsEmpty()) continue;

    if (aTriangle.Node(0) < aTriangle.Node(1))
      AddLink (aTriangle, 2);
    if (aTriangle.Node(1) < aTriangle.Node(2))
      AddLink (aTriangle, 0);
    if (aTriangle.Node(2) < aTriangle.Node(0))
      AddLink (aTriangle, 1);
  }
  // Above algorithm does not create all boundary links, so
  // it is necessary to check triangles and add absentee links
  anIter.Init(myTriangles);
  Standard_Integer i;
  for (; anIter.More(); anIter.Next()) {
    Poly_CoherentTriangle& aTriangle = anIter.ChangeValue();

    if(aTriangle.IsEmpty()) continue;

    for (i = 0; i < 3; ++i) {
      if (aTriangle.mypLink[i] == 0L) {
        AddLink(aTriangle, i);
      }
    }
  }
  return myLinks.Length();
}

//=======================================================================
//function : ClearLinks
//purpose  : 
//=======================================================================

void Poly_CoherentTriangulation::ClearLinks ()
{
  myLinks.Clear();
  NCollection_Vector<Poly_CoherentTriangle>::Iterator anIter (myTriangles);
  for (; anIter.More(); anIter.Next()) {
    Poly_CoherentTriangle& aTriangle = anIter.ChangeValue();
    aTriangle.mypLink[0] = 0L;
    aTriangle.mypLink[1] = 0L;
    aTriangle.mypLink[2] = 0L;
  }
}

//=======================================================================
//function : Clone
//purpose  : 
//=======================================================================

Handle(Poly_CoherentTriangulation)  Poly_CoherentTriangulation::Clone
       (const Handle(NCollection_BaseAllocator)& theAlloc) const
{
  Handle(Poly_CoherentTriangulation) newTri; 

  if (NTriangles() != 0 && NNodes() != 0) {
    Handle(Poly_Triangulation) theTriangulation = GetTriangulation();
    newTri = new Poly_CoherentTriangulation(theTriangulation, theAlloc);
    newTri->SetDeflection(theTriangulation->Deflection());
  }

  return newTri;
}
//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Poly_CoherentTriangulation::Dump (Standard_OStream& theStream) const
{
  for (Standard_Integer iNode = 0; iNode < myNodes.Length(); iNode++) {
    const Poly_CoherentNode& aNode = myNodes(iNode);
    if (aNode.IsFreeNode())
      continue;
    theStream << "Node " << iNode;
    aNode.Dump(theStream);
  }
}
