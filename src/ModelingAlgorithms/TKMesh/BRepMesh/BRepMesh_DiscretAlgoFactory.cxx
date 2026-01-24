// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <BRepMesh_DiscretAlgoFactory.hxx>

#include <BRepMesh_DiscretRoot.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepMesh_DiscretAlgoFactory, Standard_Transient)

namespace
{
static NCollection_List<Handle(BRepMesh_DiscretAlgoFactory)>& getFactories()
{
  static NCollection_List<Handle(BRepMesh_DiscretAlgoFactory)> TheFactories;
  return TheFactories;
}
} // namespace

//==================================================================================================

const NCollection_List<Handle(BRepMesh_DiscretAlgoFactory)>& BRepMesh_DiscretAlgoFactory::Factories()
{
  return getFactories();
}

//==================================================================================================

void BRepMesh_DiscretAlgoFactory::RegisterFactory(
  const Handle(BRepMesh_DiscretAlgoFactory)& theFactory,
  bool                                       theIsPreferred)
{
  const TCollection_AsciiString                        aName      = theFactory->Name();
  NCollection_List<Handle(BRepMesh_DiscretAlgoFactory)>& aFactories = getFactories();
  if (theIsPreferred)
  {
    UnregisterFactory(aName);
    aFactories.Prepend(theFactory);
    return;
  }

  for (NCollection_List<Handle(BRepMesh_DiscretAlgoFactory)>::Iterator anIter(aFactories);
       anIter.More();
       anIter.Next())
  {
    if (TCollection_AsciiString::IsSameString(anIter.Value()->Name(), aName, false))
    {
      return;
    }
  }
  aFactories.Append(theFactory);
}

//==================================================================================================

void BRepMesh_DiscretAlgoFactory::UnregisterFactory(const TCollection_AsciiString& theName)
{
  NCollection_List<Handle(BRepMesh_DiscretAlgoFactory)>& aFactories = getFactories();
  for (NCollection_List<Handle(BRepMesh_DiscretAlgoFactory)>::Iterator anIter(aFactories);
       anIter.More();)
  {
    if (TCollection_AsciiString::IsSameString(anIter.Value()->Name(), theName, false))
    {
      aFactories.Remove(anIter);
    }
    else
    {
      anIter.Next();
    }
  }
}

//==================================================================================================

Handle(BRepMesh_DiscretAlgoFactory) BRepMesh_DiscretAlgoFactory::DefaultFactory()
{
  const NCollection_List<Handle(BRepMesh_DiscretAlgoFactory)>& aFactories = getFactories();
  return !aFactories.IsEmpty() ? aFactories.First() : Handle(BRepMesh_DiscretAlgoFactory)();
}

//==================================================================================================

Handle(BRepMesh_DiscretAlgoFactory) BRepMesh_DiscretAlgoFactory::FindFactory(
  const TCollection_AsciiString& theName)
{
  const NCollection_List<Handle(BRepMesh_DiscretAlgoFactory)>& aFactories = getFactories();
  for (NCollection_List<Handle(BRepMesh_DiscretAlgoFactory)>::Iterator anIter(aFactories);
       anIter.More();
       anIter.Next())
  {
    if (TCollection_AsciiString::IsSameString(anIter.Value()->Name(), theName, false))
    {
      return anIter.Value();
    }
  }
  return Handle(BRepMesh_DiscretAlgoFactory)();
}

//==================================================================================================

BRepMesh_DiscretAlgoFactory::BRepMesh_DiscretAlgoFactory(const TCollection_AsciiString& theName)
    : myName(theName)
{
}
