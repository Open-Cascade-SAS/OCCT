// Created on: 1993-05-11
// Created by: Didier PIFFAULT
// Copyright (c) 1993-1999 Matra Datavision
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


#include <BRepMesh_DataStructureOfDelaun.ixx>

//=======================================================================
//function : BRepMesh_DataStructureOfDelaun
//purpose  : 
//=======================================================================
BRepMesh_DataStructureOfDelaun::BRepMesh_DataStructureOfDelaun(const BRepMesh_BaseAllocator& theAlloc,
                                                               const Standard_Integer NodeNumber)
                                                               : myNodes(NodeNumber, theAlloc),
                                                               myLinks(NodeNumber*3),
                                                               myDelLinks(theAlloc),
                                                               myElements(NodeNumber*2),
                                                               // Not_Debuged_Yet myDelElements(theAlloc),
                                                               myElemOfDomain(NodeNumber*2,theAlloc),
                                                               myLinkOfDomain(NodeNumber*2,theAlloc)
{
  myAllocator = theAlloc;
}

//=======================================================================
//function : AddNode
//purpose  : 
//=======================================================================
Standard_Integer  BRepMesh_DataStructureOfDelaun::AddNode(const BRepMesh_Vertex& theNode)
{
  return myNodes.Add(theNode);
}

//=======================================================================
//function : GetNode
//purpose  : 
//=======================================================================
const BRepMesh_Vertex&  BRepMesh_DataStructureOfDelaun::GetNode(const Standard_Integer Index)
{
  return myNodes.FindKey(Index);
}

//=======================================================================
//function : GetNodeList
//purpose  : 
//=======================================================================
const BRepMesh_ListOfInteger& BRepMesh_DataStructureOfDelaun::GetNodeList(const Standard_Integer Index)
{
  return myNodes.FindFromIndex(Index);
}

//=======================================================================
//function : ForceRemoveNode
//purpose  : 
//=======================================================================
void  BRepMesh_DataStructureOfDelaun::ForceRemoveNode(const Standard_Integer Index)
{
  if ( myNodes.FindFromIndex(Index).Extent()==0) {
    myNodes.Delete(Index);
  }
}

//=======================================================================
//function : ReplaceNodes
//purpose  : 
//=======================================================================
void  BRepMesh_DataStructureOfDelaun::ReplaceNodes(const BRepMesh_VertexTool& NewNodes)
{
  if ( NewNodes.IsEmpty() )
    return;

  myNodes = NewNodes;
}

//=======================================================================
//function : ForceRemoveLink
//purpose  : 
//=======================================================================
void  BRepMesh_DataStructureOfDelaun::ForceRemoveLink(const Standard_Integer Index)
{
  //Warning, the static cast from const& to & is called for
  //performance reasons. This is applicable only in case if later
  //modification of element (field movability) does not influent on
  //has calculation.
  BRepMesh_Edge& lref=(BRepMesh_Edge&)myLinks.FindKey(Index);
  if (lref.Movability()!=BRepMesh_Deleted) {
    if (myLinks.FindFromIndex(Index).Extent()==0) {
      BRepMesh_ListOfInteger::Iterator tit;
      BRepMesh_ListOfInteger& aList1 = myNodes(lref.FirstNode());
      for(tit.Init(aList1); tit.More(); tit.Next()){
        if (tit.Value()==Index) {
          aList1.Remove(tit);
          break;
        }
      }

      BRepMesh_ListOfInteger& aList2 = myNodes(lref.LastNode());
      for(tit.Init(aList2); tit.More(); tit.Next()){
        if (tit.Value()==Index) {
          aList2.Remove(tit);
          break;
        }
      }
      myLinkOfDomain.Remove(Index);
      lref.SetMovability(BRepMesh_Deleted);
      myDelLinks.Append(Index);
    }
  }
}

//=======================================================================
//function : RemoveNode
//purpose  : 
//=======================================================================
void  BRepMesh_DataStructureOfDelaun::RemoveNode(const Standard_Integer Index)
{
  if (myNodes.FindKey(Index).Movability() == BRepMesh_Free &&
      myNodes.FindFromIndex(Index).Extent() == 0) {
      myNodes.Delete(Index);
  }
}

//=======================================================================
//function : MoveNode
//purpose  : 
//=======================================================================
Standard_Boolean BRepMesh_DataStructureOfDelaun::MoveNode(const Standard_Integer Index, 
                                                          const BRepMesh_Vertex& newNode)
{
  if (myNodes.FindIndex(newNode) == 0) {
    const BRepMesh_ListOfInteger& refLink = myNodes(Index);
    myNodes.Substitute(Index, newNode, refLink);
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : NbNodes
//purpose  : 
//=======================================================================
Standard_Integer  BRepMesh_DataStructureOfDelaun::NbNodes()const
{
  return myNodes.Extent();
}

//=======================================================================
//function : AddLink
//purpose  : 
//=======================================================================
Standard_Integer  BRepMesh_DataStructureOfDelaun::AddLink(const BRepMesh_Edge& theLink)
{
  Standard_Integer LinkIndex=myLinks.FindIndex(theLink);
  if (LinkIndex<=0) {
    BRepMesh_PairOfIndex aPair;
    if (!myDelLinks.IsEmpty()) {
      LinkIndex=myDelLinks.First();
      myLinks.Substitute(LinkIndex, theLink, aPair);
      myDelLinks.RemoveFirst();
    }
    else {
      LinkIndex=myLinks.Add(theLink, aPair);
    }
    myNodes(theLink.FirstNode()).Append(Abs(LinkIndex));
    myNodes(theLink.LastNode()).Append(Abs(LinkIndex));
    myLinkOfDomain.Add(LinkIndex);
  }
  else if (!theLink.SameOrientation(myLinks.FindKey(LinkIndex))) 
    LinkIndex=-LinkIndex;

  return LinkIndex;
}

//=======================================================================
//function : GetLink
//purpose  : 
//=======================================================================
const BRepMesh_Edge&  BRepMesh_DataStructureOfDelaun::GetLink(const Standard_Integer Index)
{
  return myLinks.FindKey(Index);
}

//=======================================================================
//function : RemoveLink
//purpose  : 
//=======================================================================
void  BRepMesh_DataStructureOfDelaun::RemoveLink(const Standard_Integer Index)
{
  //Warning, the static cast from const& to & is called for
  //performance reasons. This is applicable only in case if later
  //modification of element (field movability) does not influent on
  //has calculation.
  BRepMesh_Edge& lref=(BRepMesh_Edge&)myLinks.FindKey(Index);
  if (lref.Movability()!=BRepMesh_Deleted) {
    if (lref.Movability()==BRepMesh_Free && 
      myLinks.FindFromIndex(Index).Extent()==0) {
        BRepMesh_ListOfInteger::Iterator tit;
        BRepMesh_ListOfInteger& aList1 = myNodes(lref.FirstNode());
        for(tit.Init(aList1); tit.More(); tit.Next()){
          if (tit.Value()==Index) {
            aList1.Remove(tit);
            break;
          }
        }
        BRepMesh_ListOfInteger& aList2 = myNodes(lref.LastNode());
        for(tit.Init(aList2); tit.More(); tit.Next()){
          if (tit.Value()==Index) {
            aList2.Remove(tit);
            break;
          }
        }
        myLinkOfDomain.Remove(Index);
        lref.SetMovability(BRepMesh_Deleted);
        myDelLinks.Append(Index);
    }
  }
}

//=======================================================================
//function : SubstituteLink
//purpose  : 
//=======================================================================
Standard_Boolean  BRepMesh_DataStructureOfDelaun::SubstituteLink(const Standard_Integer Index,
                                                                 const BRepMesh_Edge& newLink)
{
  //BRepMesh_ListOfInteger thelist(myAllocator);
  BRepMesh_PairOfIndex aPair;
  BRepMesh_Edge lref=myLinks.FindKey(Index);
  if (lref.Movability()==BRepMesh_Deleted) 
    myLinks.Substitute(Index, newLink, aPair);
  else {
    if (myLinks.FindIndex(newLink)!=0) 
      return Standard_False;

    lref.SetMovability(BRepMesh_Deleted);
    myLinks.Substitute(Index, lref, aPair);

    BRepMesh_ListOfInteger::Iterator tit;
    for(tit.Init(myNodes(lref.FirstNode())); tit.More(); tit.Next()){
      if (tit.Value()==Index) {
        myNodes(lref.FirstNode()).Remove(tit);
        break;
      }
    }
    for(tit.Init(myNodes(lref.LastNode())); tit.More(); tit.Next()){
      if (tit.Value()==Index) {
        myNodes(lref.LastNode()).Remove(tit);
        break;
      }
    }
    myLinks.Substitute(Index, newLink, aPair);
    myNodes(newLink.FirstNode()).Append(Abs(Index));
    myNodes(newLink.LastNode()).Append(Abs(Index));
  }
  return Standard_True;
}

//=======================================================================
//function : NbLinks
//purpose  : 
//=======================================================================
Standard_Integer  BRepMesh_DataStructureOfDelaun::NbLinks()const
{
  return myLinks.Extent();
}

//=======================================================================
//function : AddElement
//purpose  : 
//=======================================================================
Standard_Integer  BRepMesh_DataStructureOfDelaun::AddElement(const BRepMesh_Triangle& theElement)
{
  Standard_Integer ElemIndex=myElements.FindIndex(theElement);

  if (ElemIndex<=0) {
    /* Not_Debuged_Yet 
    if (!myDelElements.IsEmpty()) {
    ElemIndex=myDelElements.First();
    myElements.Substitute(ElemIndex, theElement);
    myDelElements.RemoveFirst();
    }
    else */
    ElemIndex=myElements.Add(theElement);

    myElemOfDomain.Add(ElemIndex);

    Standard_Integer ed1, ed2, ed3;
    Standard_Boolean or1, or2, or3;
    theElement.Edges(ed1, ed2, ed3, or1, or2, or3);
    myLinks(ed1).Append(ElemIndex);
    myLinks(ed2).Append(ElemIndex);
    myLinks(ed3).Append(ElemIndex);
  }

  return ElemIndex;
}

//=======================================================================
//function : GetElement
//purpose  : 
//=======================================================================
const BRepMesh_Triangle&  BRepMesh_DataStructureOfDelaun::GetElement(const Standard_Integer Index)
{
  return myElements.FindKey(Index);
}

//=======================================================================
//function : RemoveElement
//purpose  : 
//=======================================================================
void  BRepMesh_DataStructureOfDelaun::RemoveElement(const Standard_Integer Index)
{
  //Warning, the static cast from const& to & is called for
  //performance reasons. This is applicable only in case if later
  //modification of element (field movability) does not influent on
  //has calculation.
  BRepMesh_Triangle& lelem=(BRepMesh_Triangle&)myElements.FindKey(Index);
  if (lelem.Movability()!=BRepMesh_Deleted) {
    ClearElement(Index, lelem);
    lelem.SetMovability(BRepMesh_Deleted);
    // Not_Debuged_Yet myDelElements.Append(Index);
    myElemOfDomain.Remove(Index);
  }
}

static void removeElementIndex(BRepMesh_PairOfIndex& thePair,
                               const Standard_Integer Index)
{
  for(Standard_Integer i = 1, n = thePair.Extent(); i <= n; i++) {
    if (thePair.Index(i)==Index) {
      thePair.RemoveIndex(i);
      break;
    }
  }
}

void  BRepMesh_DataStructureOfDelaun::ClearElement(const Standard_Integer Index,
                                                   const BRepMesh_Triangle& theElem)
{
  if (theElem.Movability()==BRepMesh_Free) {
    Standard_Integer ed1, ed2, ed3;
    Standard_Boolean or1, or2, or3;
    theElem.Edges(ed1, ed2, ed3, or1, or2, or3);
    removeElementIndex(myLinks(ed1),Index);
    removeElementIndex(myLinks(ed2),Index);
    removeElementIndex(myLinks(ed3),Index);
  }
}

//=======================================================================
//function : SubstituteElement
//purpose  : 
//=======================================================================
Standard_Boolean  BRepMesh_DataStructureOfDelaun::SubstituteElement
(const Standard_Integer Index, const BRepMesh_Triangle& newElement)
{
  const BRepMesh_Triangle& lelem=myElements.FindKey(Index);
  if (lelem.Movability()==BRepMesh_Deleted) 
    myElements.Substitute(Index, newElement);
  else {
    if (myElements.FindIndex(newElement)==0) {
      ClearElement(Index, lelem);
      // Warning: here new element and old element should have different Hash code
      myElements.Substitute(Index, newElement);

      Standard_Integer ed1, ed2, ed3;
      Standard_Boolean or1, or2, or3;
      newElement.Edges(ed1, ed2, ed3, or1, or2, or3);
      myLinks(ed1).Append(Index);
      myLinks(ed2).Append(Index);
      myLinks(ed3).Append(Index);
    }
    else return Standard_False;
  }
  return Standard_True;
}

//=======================================================================
//function : ClearDomain
//purpose  : 
//=======================================================================
void BRepMesh_DataStructureOfDelaun::ClearDomain()
{
  BRepMesh_MapOfInteger freeEdges;
  Standard_Integer ed1, ed2, ed3;
  Standard_Boolean or1, or2, or3;
  BRepMesh_MapOfInteger::Iterator itDom(myElemOfDomain);
  //Warning, the static cast from const& to & is called for
  //performance reasons. This is applicable only in case if later
  //modification of element (field movability) does not influent on
  //has calculation.
  for (;itDom.More(); itDom.Next()) {
    BRepMesh_Triangle& lelem=(BRepMesh_Triangle&)myElements.FindKey(itDom.Key());
    lelem.Edges(ed1, ed2, ed3, or1, or2, or3);
    freeEdges.Add(ed1);
    freeEdges.Add(ed2);
    freeEdges.Add(ed3);
    ClearElement(itDom.Key(), lelem);
    lelem.SetMovability(BRepMesh_Deleted);
    // Not_Debuged_Yet myDelElements.Append(itDom.Key());
  }
  myElemOfDomain.Clear();
  BRepMesh_MapOfInteger::Iterator edgeIt(freeEdges);
  for (; edgeIt.More(); edgeIt.Next())
    RemoveLink(edgeIt.Key());
}

//=======================================================================
//function : NbElements
//purpose  : 
//=======================================================================
Standard_Integer  BRepMesh_DataStructureOfDelaun::NbElements()const 
{
  return myElements.Extent();
}

//=======================================================================
//function : IndexOf
//purpose  : 
//=======================================================================
Standard_Integer BRepMesh_DataStructureOfDelaun::IndexOf(const BRepMesh_Vertex& aNode)
{
  return myNodes.FindIndex(aNode);
}

//=======================================================================
//function : IndexOf
//purpose  : 
//=======================================================================
Standard_Integer BRepMesh_DataStructureOfDelaun::IndexOf(const BRepMesh_Edge& aLink)const
{
  return myLinks.FindIndex(aLink);
}

//=======================================================================
//function : IndexOf
//purpose  : 
//=======================================================================
Standard_Integer BRepMesh_DataStructureOfDelaun::IndexOf(const BRepMesh_Triangle& anElement)const
{
  return myElements.FindIndex(anElement);
}

//=======================================================================
//function : LinkNeighboursOf
//purpose  : 
//=======================================================================
const BRepMesh_ListOfInteger&  BRepMesh_DataStructureOfDelaun::LinkNeighboursOf
(const Standard_Integer theNode)const 
{
  return myNodes.FindFromIndex(theNode);
}

//=======================================================================
//function : ElemConnectedTo
//purpose  : 
//=======================================================================
const BRepMesh_PairOfIndex&  BRepMesh_DataStructureOfDelaun::ElemConnectedTo 
(const Standard_Integer theLink)const 
{
  return myLinks.FindFromIndex(theLink);
}

//=======================================================================
//function : ElemOfDomain
//purpose  : 
//=======================================================================
const BRepMesh_MapOfInteger&  BRepMesh_DataStructureOfDelaun::ElemOfDomain () const
{
  return myElemOfDomain;
}

//=======================================================================
//function : LinkOfDomain
//purpose  : 
//=======================================================================
const BRepMesh_MapOfInteger&  BRepMesh_DataStructureOfDelaun::LinkOfDomain () const
{
  return myLinkOfDomain;
}

//=======================================================================
//function : ClearDeleted
//purpose  : 
//=======================================================================
void BRepMesh_DataStructureOfDelaun::ClearDeleted()
{

  // Traitement des Elements 

  Standard_Integer IndexDelItem;

  Standard_Integer lastNonDelItem=myElements.Extent();
  /* // Not_Debuged_Yet 
  while (!myDelElements.IsEmpty()) {
  while (lastNonDelItem>0) {
  if (myElements.FindKey(lastNonDelItem).Movability()!=BRepMesh_Deleted)
  break;
  myElements.RemoveLast();
  lastNonDelItem--;
  }

  IndexDelItem=myDelElements.First();
  myDelElements.RemoveFirst();

  if (IndexDelItem<lastNonDelItem) {
  BRepMesh_Triangle eItem=myElements.FindKey(lastNonDelItem);
  myElements.RemoveLast();
  myElements.Substitute(IndexDelItem, eItem);
  myElemOfDomain.Remove(lastNonDelItem);
  myElemOfDomain.Add(IndexDelItem);
  lastNonDelItem--;

  Standard_Integer ed[3], ied;
  Standard_Boolean orient[3];
  eItem.Edges(ed[0], ed[1], ed[2], orient[0], orient[1], orient[2]);
  BRepMesh_ListOfInteger::Iterator itList;
  for (ied=0; ied<3; ied++) {
  BRepMesh_PairOfIndex& aPair = myLinks(ed[ied]);
  for(Standard_Integer j = 1, jn = aPair.Extent(); j <= jn; j++)
  if (aPair.Index(j)==(lastNonDelItem+1)) {
  aPair.SetIndex(j,IndexDelItem);
  break;
  }
  }
  }
  }
  */

  // Process Links:
  lastNonDelItem=myLinks.Extent();

  while (!myDelLinks.IsEmpty()) {
    while (lastNonDelItem>0) {
      if (myLinks.FindKey(lastNonDelItem).Movability()!=BRepMesh_Deleted)
        break;
      myLinks.RemoveLast();
      lastNonDelItem--;
    }

    IndexDelItem = myDelLinks.First();
    myDelLinks.RemoveFirst();

    if (IndexDelItem < lastNonDelItem) {
      BRepMesh_Edge lItem=myLinks.FindKey(lastNonDelItem);
      BRepMesh_PairOfIndex Data(myLinks(lastNonDelItem));
      myLinks.RemoveLast();
      myLinks.Substitute(IndexDelItem, lItem, Data);
      myLinkOfDomain.Remove(lastNonDelItem);
      myLinkOfDomain.Add(IndexDelItem);
      lastNonDelItem--;

      Standard_Integer iv[2], ivx;
      iv[0]=lItem.FirstNode();
      iv[1]=lItem.LastNode();

      BRepMesh_ListOfInteger::Iterator itLis;
      for (ivx=0; ivx<2; ivx++) {
        for (itLis.Init(myNodes(iv[ivx])); 
          itLis.More(); itLis.Next()) {
            if (itLis.Value()==(lastNonDelItem+1)) {
              itLis.ChangeValue()=IndexDelItem;
              break;
            }
        }
      }
      for(Standard_Integer j = 1, jn = Data.Extent(); j <= jn; j++) {
        const BRepMesh_Triangle& Elem=myElements.FindKey(Data.Index(j));

        Standard_Integer el[3], iel;
        Standard_Boolean orl[3];
        Elem.Edges(el[0], el[1], el[2], orl[0], orl[1], orl[2]);
        for (iel=0; iel<3; iel++) {
          if (el[iel]==lastNonDelItem+1) {
            el[iel]=IndexDelItem;
            break;
          }
        }
        myElements.Substitute(itLis.Value(),
          BRepMesh_Triangle(el[0], el[1], el[2],
          orl[0], orl[1], orl[2],
          Elem.Movability() ));
      }
    }
  }


  // Process Nodes:
  lastNonDelItem = myNodes.Extent();
  BRepMesh_ListOfInteger &aDelNodes = (BRepMesh_ListOfInteger &)myNodes.GetListOfDelNodes();

  while (!aDelNodes.IsEmpty()) {
    while (lastNonDelItem > 0) {
      if (myNodes.FindKey(lastNonDelItem).Movability()!=BRepMesh_Deleted)
        break;
      myNodes.RemoveLast();
      lastNonDelItem--;
    }
    IndexDelItem = aDelNodes.First();
    aDelNodes.RemoveFirst();

    if (IndexDelItem<lastNonDelItem) {
      BRepMesh_Vertex nItem = myNodes.FindKey(lastNonDelItem);
      BRepMesh_ListOfInteger Data;
      Data.Append(myNodes(lastNonDelItem));
      myNodes.RemoveLast();
      lastNonDelItem--;
      myNodes.Substitute(IndexDelItem, nItem, Data);

      BRepMesh_ListOfInteger::Iterator itLi;
      for (itLi.Init(Data); itLi.More(); itLi.Next()) {
        const BRepMesh_Edge& li=myLinks.FindKey(itLi.Value());
        BRepMesh_PairOfIndex conx(myLinks(itLi.Value()));
        Standard_Integer iv1=li.FirstNode();
        Standard_Integer iv2=li.LastNode();
        if      (iv1==lastNonDelItem+1) iv1=IndexDelItem;
        else if (iv2==lastNonDelItem+1) iv2=IndexDelItem;

        myLinks.Substitute(itLi.Value(),
          BRepMesh_Edge(iv1, iv2, li.Movability()), conx);
      }
    }
  }
}

//=======================================================================
//function : Statistics
//purpose  : 
//=======================================================================
void BRepMesh_DataStructureOfDelaun::Statistics(Standard_OStream& S) const
{
  S << " Map de nodes : \n";
  myNodes.Statistics(S);
  S << "\n Deleted nodes : " << myNodes.GetListOfDelNodes().Extent() << endl;

  S << "\n\n Map de Links : \n";
  myLinks.Statistics(S);
  S << "\n Deleted links : " << myDelLinks.Extent() << endl;

  S << "\n\n Map d elements : \n";
  myElements.Statistics(S);
  // Not_Debuged_Yet S << "\n Deleted elements : " << myDelElements.Extent() << endl;
}

//=======================================================================
//function : Allocator()
//purpose  : 
//=======================================================================
const BRepMesh_BaseAllocator& BRepMesh_DataStructureOfDelaun::Allocator() const
{
  return myAllocator;
}

//=======================================================================
//function : Data
//purpose  : 
//=======================================================================
BRepMesh_VertexTool& BRepMesh_DataStructureOfDelaun::Data()
{
  return myNodes;
}
