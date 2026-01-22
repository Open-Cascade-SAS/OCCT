// Created on: 2017-04-21
// Created by: Alexander Bobkov
// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <BRepTools_History.hxx>

#include <TopExp.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedMap.hxx>

// Implement the OCCT RTTI for the type.
IMPLEMENT_STANDARD_RTTIEXT(BRepTools_History, Standard_Transient)

namespace
{

//==============================================================================
// function : add
// purpose  : Adds the elements of the list to the map.
//==============================================================================
void add(NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMap,
         const NCollection_List<TopoDS_Shape>&                   theList)
{
  for (NCollection_List<TopoDS_Shape>::Iterator aSIt(theList); aSIt.More(); aSIt.Next())
  {
    theMap.Add(aSIt.Value());
  }
}

//==============================================================================
// function : add
// purpose  : Adds the elements of the collection to the list.
//==============================================================================
template <typename TCollection>
void add(NCollection_List<TopoDS_Shape>& theList, const TCollection& theCollection)
{
  for (typename TCollection::Iterator aSIt(theCollection); aSIt.More(); aSIt.Next())
  {
    theList.Append(aSIt.Value());
  }
}

} // namespace

//=================================================================================================

void BRepTools_History::AddGenerated(const TopoDS_Shape& theInitial,
                                     const TopoDS_Shape& theGenerated)
{
  if (!prepareGenerated(theInitial, theGenerated))
  {
    return;
  }

  NCollection_List<TopoDS_Shape>* aGenerations = myShapeToGenerated.ChangeSeek(theInitial);
  if (aGenerations == nullptr)
  {
    aGenerations = myShapeToGenerated.Bound(theInitial, NCollection_List<TopoDS_Shape>());
  }

  Standard_ASSERT_VOID(!aGenerations->Contains(theGenerated),
                       "Error: a duplicated generation of a shape.");

  aGenerations->Append(theGenerated);
}

//=================================================================================================

void BRepTools_History::AddModified(const TopoDS_Shape& theInitial, const TopoDS_Shape& theModified)
{
  if (!prepareModified(theInitial, theModified))
  {
    return;
  }

  NCollection_List<TopoDS_Shape>* aModifications = myShapeToModified.ChangeSeek(theInitial);
  if (aModifications == nullptr)
  {
    aModifications = myShapeToModified.Bound(theInitial, NCollection_List<TopoDS_Shape>());
  }

  Standard_ASSERT_VOID(!aModifications->Contains(theModified),
                       "Error: a duplicated modification of a shape.");

  aModifications->Append(theModified);
}

//=================================================================================================

void BRepTools_History::Remove(const TopoDS_Shape& theRemoved)
{
  // Apply the limitations.
  Standard_ASSERT_RETURN(IsSupportedType(theRemoved), myMsgUnsupportedType, Standard_VOID_RETURN);

  if (myShapeToModified.UnBind(theRemoved))
  {
    Standard_ASSERT_INVOKE_(, myMsgModifiedAndRemoved);
  }

  myRemoved.Add(theRemoved);
}

//=================================================================================================

void BRepTools_History::ReplaceGenerated(const TopoDS_Shape& theInitial,
                                         const TopoDS_Shape& theGenerated)
{
  if (!prepareGenerated(theInitial, theGenerated))
  {
    return;
  }

  NCollection_List<TopoDS_Shape>* aGenerations =
    myShapeToGenerated.Bound(theInitial, NCollection_List<TopoDS_Shape>());
  aGenerations->Append(theGenerated);
}

//=================================================================================================

void BRepTools_History::ReplaceModified(const TopoDS_Shape& theInitial,
                                        const TopoDS_Shape& theModified)
{
  if (!prepareModified(theInitial, theModified))
  {
    return;
  }

  NCollection_List<TopoDS_Shape>* aModifications =
    myShapeToModified.Bound(theInitial, NCollection_List<TopoDS_Shape>());
  aModifications->Append(theModified);
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& BRepTools_History::Generated(
  const TopoDS_Shape& theInitial) const
{
  // Apply the limitations.
  Standard_ASSERT_RETURN(theInitial.IsNull() || IsSupportedType(theInitial),
                         myMsgUnsupportedType,
                         emptyList());

  //
  const NCollection_List<TopoDS_Shape>* aGenerations = myShapeToGenerated.Seek(theInitial);
  return (aGenerations != nullptr) ? *aGenerations : emptyList();
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& BRepTools_History::Modified(
  const TopoDS_Shape& theInitial) const
{
  // Apply the limitations.
  Standard_ASSERT_RETURN(IsSupportedType(theInitial), myMsgUnsupportedType, emptyList());

  //
  const NCollection_List<TopoDS_Shape>* aModifications = myShapeToModified.Seek(theInitial);
  return (aModifications != nullptr) ? *aModifications : emptyList();
}

//=================================================================================================

bool BRepTools_History::IsRemoved(const TopoDS_Shape& theInitial) const
{
  // Apply the limitations.
  Standard_ASSERT_RETURN(IsSupportedType(theInitial), myMsgUnsupportedType, false);

  //
  return myRemoved.Contains(theInitial);
}

//=================================================================================================

void BRepTools_History::Merge(const occ::handle<BRepTools_History>& theHistory23)
{
  if (!theHistory23.IsNull())
    Merge(*theHistory23.get());
}

//=================================================================================================

void BRepTools_History::Merge(const BRepTools_History& theHistory23)
{
  if (!(theHistory23.HasModified() || theHistory23.HasGenerated() || theHistory23.HasRemoved()))
    // nothing to merge
    return;

  // Propagate R23 directly and M23 and G23 fully to M12 and G12.
  // Remember the propagated shapes.
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>*
    aS1ToGAndM[] = {&myShapeToGenerated, &myShapeToModified};
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aRPropagated;
  {
    // Propagate R23, M23 and G23 to M12 and G12 directly.
    // Remember the propagated shapes.
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMAndGPropagated;
    for (int aI = 0; aI < 2; ++aI)
    {
      for (NCollection_DataMap<TopoDS_Shape,
                               NCollection_List<TopoDS_Shape>,
                               TopTools_ShapeMapHasher>::Iterator aMIt1(*aS1ToGAndM[aI]);
           aMIt1.More();
           aMIt1.Next())
      {
        NCollection_List<TopoDS_Shape>& aL12 = aMIt1.ChangeValue();
        NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>
          aAdditions[2]; // The G and M additions.
        for (NCollection_List<TopoDS_Shape>::Iterator aSIt2(aL12); aSIt2.More();)
        {
          const TopoDS_Shape& aS2 = aSIt2.Value();
          if (theHistory23.IsRemoved(aS2))
          {
            aRPropagated.Add(aS2);
            aL12.Remove(aSIt2);
          }
          else
          {
            const NCollection_List<TopoDS_Shape>* pGen = theHistory23.myShapeToGenerated.Seek(aS2);
            if (pGen)
            {
              add(aAdditions[0], *pGen);
              aMAndGPropagated.Add(aS2);
            }

            const NCollection_List<TopoDS_Shape>* pMod = theHistory23.myShapeToModified.Seek(aS2);
            if (pMod)
            {
              add(aAdditions[aI], *pMod);
              aMAndGPropagated.Add(aS2);

              aL12.Remove(aSIt2);
            }
            else
            {
              aSIt2.Next();
            }
          }
        }

        add(aL12, aAdditions[aI]);
        if (aI != 0 && !aAdditions[0].IsEmpty())
        {
          const TopoDS_Shape&             aS1    = aMIt1.Key();
          NCollection_List<TopoDS_Shape>* aGAndM = aS1ToGAndM[0]->ChangeSeek(aS1);
          if (aGAndM == nullptr)
          {
            aGAndM = aS1ToGAndM[0]->Bound(aS1, NCollection_List<TopoDS_Shape>());
          }

          add(*aGAndM, aAdditions[0]);
        }
      }
    }

    // Propagate M23 and G23 to M12 and G12 sequentially.
    const NCollection_DataMap<TopoDS_Shape,
                              NCollection_List<TopoDS_Shape>,
                              TopTools_ShapeMapHasher>* aS2ToGAndM[] = {
      &theHistory23.myShapeToGenerated,
      &theHistory23.myShapeToModified};
    for (int aI = 0; aI < 2; ++aI)
    {
      for (NCollection_DataMap<TopoDS_Shape,
                               NCollection_List<TopoDS_Shape>,
                               TopTools_ShapeMapHasher>::Iterator aMIt2(*aS2ToGAndM[aI]);
           aMIt2.More();
           aMIt2.Next())
      {
        const TopoDS_Shape& aS2 = aMIt2.Key();
        if (!aMAndGPropagated.Contains(aS2))
        {
          if (!aS1ToGAndM[aI]->IsBound(aS2))
          {
            aS1ToGAndM[aI]->Bind(aS2, NCollection_List<TopoDS_Shape>());
          }

          NCollection_List<TopoDS_Shape> aM2 = aMIt2.Value();
          ((*aS1ToGAndM[aI])(aS2)).Append(aM2);
          myRemoved.Remove(aS2);
        }
      }
    }
  }

  // Unbound the empty M12 and G12.
  for (int aI = 0; aI < 2; ++aI)
  {
    for (NCollection_DataMap<TopoDS_Shape,
                             NCollection_List<TopoDS_Shape>,
                             TopTools_ShapeMapHasher>::Iterator aMIt1(*aS1ToGAndM[aI]);
         aMIt1.More();)
    {
      const TopoDS_Shape&                   aS1  = aMIt1.Key();
      const NCollection_List<TopoDS_Shape>& aL12 = aMIt1.Value();
      aMIt1.Next();
      if (aL12.IsEmpty())
      {
        myRemoved.Add(aS1);
        aS1ToGAndM[aI]->UnBind(aS1);
      }
    }
  }

  // Propagate R23 to R12 sequentially.
  for (NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator aRIt23(
         theHistory23.myRemoved);
       aRIt23.More();
       aRIt23.Next())
  {
    const TopoDS_Shape& aS2 = aRIt23.Value();
    if (!aRPropagated.Contains(aS2) && !myShapeToModified.IsBound(aS2)
        && !myShapeToGenerated.IsBound(aS2))
    {
      myRemoved.Add(aS2);
    }
  }
}

//=================================================================================================

bool BRepTools_History::prepareGenerated(const TopoDS_Shape& theInitial,
                                         const TopoDS_Shape& theGenerated)
{
  Standard_ASSERT_RETURN(theInitial.IsNull() || IsSupportedType(theInitial),
                         myMsgUnsupportedType,
                         false);

  if (myShapeToModified.IsBound(theInitial) && myShapeToModified(theInitial).Remove(theGenerated))
  {
    Standard_ASSERT_INVOKE_(, myMsgGeneratedAndModified);
  }

  return true;
}

//=================================================================================================

bool BRepTools_History::prepareModified(const TopoDS_Shape& theInitial,
                                        const TopoDS_Shape& theModified)
{
  Standard_ASSERT_RETURN(IsSupportedType(theInitial), myMsgUnsupportedType, false);

  if (myRemoved.Remove(theInitial))
  {
    Standard_ASSERT_INVOKE_(, myMsgModifiedAndRemoved);
  }

  if (myShapeToGenerated.IsBound(theInitial) && myShapeToGenerated(theInitial).Remove(theModified))
  {
    Standard_ASSERT_INVOKE_(, myMsgGeneratedAndModified);
  }

  return true;
}

//=================================================================================================

const NCollection_List<TopoDS_Shape> BRepTools_History::myEmptyList;

//=================================================================================================

const NCollection_List<TopoDS_Shape>& BRepTools_History::emptyList()
{
  return myEmptyList;
}

//=================================================================================================

const char* BRepTools_History::myMsgUnsupportedType = "Error: unsupported shape type.";

//=================================================================================================

const char* BRepTools_History::myMsgGeneratedAndRemoved =
  "Error: a shape is generated and removed simultaneously.";

//=================================================================================================

const char* BRepTools_History::myMsgModifiedAndRemoved =
  "Error: a shape is modified and removed simultaneously.";

//=================================================================================================

const char* BRepTools_History::myMsgGeneratedAndModified =
  "Error: a shape is generated and modified "
  "from the same shape simultaneously.";
