// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <BRepGraph_ParentIterator.hxx>

#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_ReverseIterator.hxx>
#include <BRepGraph_TopoView.hxx>

//=================================================================================================

BRepGraph_ParentIterator::BRepGraph_ParentIterator(const BRepGraph&       theGraph,
                                                   const BRepGraph_NodeId theChild)
    : myGraph(&theGraph),
      myChild(theChild)
{
  seek();
}

//=================================================================================================

BRepGraph_ParentIterator::BRepGraph_ParentIterator(const BRepGraph&       theGraph,
                                                   const BRepGraph_NodeId theChild,
                                                   const Position&        thePosition)
    : myGraph(&theGraph),
      myChild(theChild),
      myPosition(thePosition)
{
  seek();
}

//=================================================================================================

void BRepGraph_ParentIterator::Next()
{
  seek();
}

//=================================================================================================

const TopLoc_Location& BRepGraph_ParentIterator::CurrentLocalLocation() const
{
  static const TopLoc_Location THE_IDENTITY;
  if (!myHasCurrent)
  {
    return THE_IDENTITY;
  }
  return myGraph->Refs().Gen().LocalLocation(myCurrent.Ref);
}

//=================================================================================================

bool BRepGraph_ParentIterator::setParent(const BRepGraph_NodeId theParent,
                                         const BRepGraph_RefId  theExpectedRef)
{
  const BRepGraph::TopoView& aTopo = myGraph->Topo();
  if (!aTopo.Gen().IsActive(theParent))
  {
    return false;
  }

  if (theExpectedRef.IsValid())
  {
    const BRepGraph::RefsView& aRefs = myGraph->Refs();
    if (!aRefs.Gen().IsActive(theExpectedRef) || aRefs.Gen().ChildNode(theExpectedRef) != myChild)
    {
      return false;
    }

    const int aStep = aRefs.Gen().StepOfRef(theParent, theExpectedRef);
    if (aStep < 0)
    {
      return false;
    }
    const TopAbs_Orientation anOrientation =
      theExpectedRef.RefKind == BRepGraph_RefId::Kind::Occurrence
        ? TopAbs_FORWARD
        : aRefs.Gen().Orientation(theExpectedRef);
    myCurrent    = ValueType{theParent,
                             theExpectedRef,
                             static_cast<uint32_t>(aStep),
                             anOrientation,
                             LinkKind::Reference};
    myHasCurrent = true;
    return true;
  }

  switch (theParent.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Wire: {
      if (myChild.NodeKind != BRepGraph_NodeId::Kind::CoEdge)
      {
        return false;
      }
      const NCollection_LinearVector<BRepGraph_CoEdgeId>& aChildren =
        aTopo.Wires().Relations(BRepGraph_WireId::FromNodeId(theParent)).CoEdgeIds;
      for (size_t aStep = 0; aStep < aChildren.Size(); ++aStep)
      {
        if (BRepGraph_NodeId(aChildren.Value(aStep)) == myChild)
        {
          myCurrent    = ValueType{theParent,
                                   BRepGraph_RefId(),
                                   static_cast<uint32_t>(aStep),
                                   TopAbs_FORWARD,
                                   LinkKind::Structural};
          myHasCurrent = true;
          return true;
        }
      }
      return false;
    }
    case BRepGraph_NodeId::Kind::CoEdge: {
      if (myChild.NodeKind != BRepGraph_NodeId::Kind::Edge)
      {
        return false;
      }
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        aTopo.CoEdges().Definition(BRepGraph_CoEdgeId::FromNodeId(theParent));
      if (BRepGraph_NodeId(aCoEdge.ChildEdgeId) != myChild)
      {
        return false;
      }
      myCurrent =
        ValueType{theParent, BRepGraph_RefId(), 0, aCoEdge.Orientation, LinkKind::Structural};
      myHasCurrent = true;
      return true;
    }
    case BRepGraph_NodeId::Kind::Occurrence: {
      const BRepGraphInc::OccurrenceDef& anOccurrence =
        aTopo.Occurrences().Definition(BRepGraph_OccurrenceId::FromNodeId(theParent));
      if (anOccurrence.ChildNodeId != myChild)
      {
        return false;
      }
      myCurrent = ValueType{theParent, BRepGraph_RefId(), 0, TopAbs_FORWARD, LinkKind::Structural};
      myHasCurrent = true;
      return true;
    }
    default:
      return false;
  }
}

//=================================================================================================

void BRepGraph_ParentIterator::seek()
{
  using Kind = BRepGraph_NodeId::Kind;

  myHasCurrent = false;
  if (!myChild.IsValid() || myChild.IsRemoved(*myGraph)
      || myGraph->Topo().Gen().TopoEntity(myChild) == nullptr)
  {
    myPosition.CurrentSource = RelationSource::End;
    return;
  }

  const BRepGraph::TopoView& aTopo = myGraph->Topo();
  while (myPosition.CurrentSource != RelationSource::End)
  {
    if (myPosition.CurrentSource == RelationSource::Primary)
    {
      switch (myChild.NodeKind)
      {
        case Kind::Vertex: {
          BRepGraph_RefsEdgesOfVertex anIt(*myGraph,
                                           aTopo.Vertices().Edges(BRepGraph_VertexId(myChild)),
                                           BRepGraph_VertexId(myChild),
                                           myPosition.Index);
          while (anIt.More())
          {
            const BRepGraph_NodeId aParent = anIt.CurrentParentId();
            const BRepGraph_RefId  aRef    = anIt.CurrentRefId();
            myPosition.Index               = anIt.Index() + 1;
            if (setParent(aParent, aRef))
            {
              return;
            }
            anIt.Next();
          }
          break;
        }
        case Kind::Edge: {
          BRepGraph_CoEdgesOfEdge anIt(*myGraph,
                                       aTopo.Edges().CoEdges(BRepGraph_EdgeId(myChild)),
                                       myPosition.Index);
          while (anIt.More())
          {
            const BRepGraph_NodeId aParent = anIt.CurrentId();
            myPosition.Index               = anIt.Index() + 1;
            if (setParent(aParent))
            {
              return;
            }
            anIt.Next();
          }
          break;
        }
        case Kind::Wire: {
          BRepGraph_FacesOfWire anIt(
            *myGraph,
            aTopo.Wires().Relations(BRepGraph_WireId(myChild)).ParentWireRefIds,
            myPosition.Index);
          while (anIt.More())
          {
            const BRepGraph_NodeId aParent = anIt.CurrentId();
            const BRepGraph_RefId  aRef    = anIt.CurrentRefId();
            myPosition.Index               = anIt.Index() + 1;
            if (setParent(aParent, aRef))
            {
              return;
            }
            anIt.Next();
          }
          break;
        }
        case Kind::Face: {
          BRepGraph_ShellsOfFace anIt(
            *myGraph,
            aTopo.Faces().Relations(BRepGraph_FaceId(myChild)).ParentFaceRefIds,
            myPosition.Index);
          while (anIt.More())
          {
            const BRepGraph_NodeId aParent = anIt.CurrentId();
            const BRepGraph_RefId  aRef    = anIt.CurrentRefId();
            myPosition.Index               = anIt.Index() + 1;
            if (setParent(aParent, aRef))
            {
              return;
            }
            anIt.Next();
          }
          break;
        }
        case Kind::Shell: {
          BRepGraph_SolidsOfShell anIt(
            *myGraph,
            aTopo.Shells().Relations(BRepGraph_ShellId(myChild)).ParentShellRefIds,
            myPosition.Index);
          while (anIt.More())
          {
            const BRepGraph_NodeId aParent = anIt.CurrentId();
            const BRepGraph_RefId  aRef    = anIt.CurrentRefId();
            myPosition.Index               = anIt.Index() + 1;
            if (setParent(aParent, aRef))
            {
              return;
            }
            anIt.Next();
          }
          break;
        }
        case Kind::Solid: {
          BRepGraph_CompSolidsOfSolid anIt(
            *myGraph,
            aTopo.Solids().Relations(BRepGraph_SolidId(myChild)).ParentSolidRefIds,
            myPosition.Index);
          while (anIt.More())
          {
            const BRepGraph_NodeId aParent = anIt.CurrentId();
            const BRepGraph_RefId  aRef    = anIt.CurrentRefId();
            myPosition.Index               = anIt.Index() + 1;
            if (setParent(aParent, aRef))
            {
              return;
            }
            anIt.Next();
          }
          break;
        }
        case Kind::CoEdge: {
          if (myPosition.Index++ == 0)
          {
            const BRepGraph_WireId aParent = aTopo.CoEdges().Wire(BRepGraph_CoEdgeId(myChild));
            if (setParent(aParent))
            {
              return;
            }
          }
          break;
        }
        case Kind::Product: {
          if (aTopo.Gen().HasOccurrenceParents(myChild))
          {
            BRepGraph_OccurrencesOfProduct anIt(*myGraph,
                                                aTopo.Gen().OccurrenceRefIds(myChild),
                                                myPosition.Index);
            while (anIt.More())
            {
              const BRepGraph_NodeId aParent = anIt.CurrentId();
              myPosition.Index               = anIt.Index() + 1;
              if (setParent(aParent))
              {
                return;
              }
              anIt.Next();
            }
          }
          break;
        }
        case Kind::Occurrence: {
          BRepGraph_ProductsOfOccurrence anIt(
            *myGraph,
            aTopo.Occurrences().Relations(BRepGraph_OccurrenceId(myChild)).ParentOccurrenceRefIds,
            myPosition.Index);
          while (anIt.More())
          {
            const BRepGraph_NodeId aParent = anIt.CurrentId();
            const BRepGraph_RefId  aRef    = anIt.CurrentRefId();
            myPosition.Index               = anIt.Index() + 1;
            if (setParent(aParent, aRef))
            {
              return;
            }
            anIt.Next();
          }
          break;
        }
        case Kind::Compound:
        case Kind::CompSolid:
          break;
      }
    }
    else if (myPosition.CurrentSource == RelationSource::Compound)
    {
      if (aTopo.Gen().HasCompoundParents(myChild))
      {
        BRepGraph_CompoundsOfChild anIt(*myGraph,
                                        aTopo.Gen().CompoundRefIds(myChild),
                                        myPosition.Index);
        while (anIt.More())
        {
          const BRepGraph_NodeId aParent = anIt.CurrentId();
          const BRepGraph_RefId  aRef    = anIt.CurrentRefId();
          myPosition.Index               = anIt.Index() + 1;
          if (setParent(aParent, aRef))
          {
            return;
          }
          anIt.Next();
        }
      }
    }
    else if (myPosition.CurrentSource == RelationSource::Occurrence)
    {
      if (myChild.NodeKind != Kind::Occurrence && myChild.NodeKind != Kind::Product
          && aTopo.Gen().HasOccurrenceParents(myChild))
      {
        BRepGraph_OccurrencesOfChild anIt(*myGraph,
                                          aTopo.Gen().OccurrenceRefIds(myChild),
                                          myPosition.Index);
        while (anIt.More())
        {
          const BRepGraph_NodeId aParent = anIt.CurrentId();
          myPosition.Index               = anIt.Index() + 1;
          if (setParent(aParent))
          {
            return;
          }
          anIt.Next();
        }
      }
    }

    switch (myPosition.CurrentSource)
    {
      case RelationSource::Primary:
        myPosition.CurrentSource = RelationSource::Compound;
        break;
      case RelationSource::Compound:
        myPosition.CurrentSource = RelationSource::Occurrence;
        break;
      case RelationSource::Occurrence:
        myPosition.CurrentSource = RelationSource::End;
        break;
      case RelationSource::End:
        break;
    }
    myPosition.Index = 0;
  }
}
