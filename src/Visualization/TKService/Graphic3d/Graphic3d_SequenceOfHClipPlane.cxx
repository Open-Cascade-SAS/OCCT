// Copyright (c) 2018 OPEN CASCADE SAS
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

#include <Graphic3d_SequenceOfHClipPlane.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_SequenceOfHClipPlane, Standard_Transient)

//=================================================================================================

Graphic3d_SequenceOfHClipPlane::Graphic3d_SequenceOfHClipPlane()
    : myToOverrideGlobal(false)
{
}

//=================================================================================================

bool Graphic3d_SequenceOfHClipPlane::Append(const occ::handle<Graphic3d_ClipPlane>& theItem)
{
  for (NCollection_Sequence<occ::handle<Graphic3d_ClipPlane>>::Iterator anItemIter(myItems);
       anItemIter.More();
       anItemIter.Next())
  {
    if (anItemIter.Value() == theItem)
    {
      return false;
    }
  }
  myItems.Append(theItem);
  return true;
}

//=================================================================================================

bool Graphic3d_SequenceOfHClipPlane::Remove(const occ::handle<Graphic3d_ClipPlane>& theItem)
{
  for (NCollection_Sequence<occ::handle<Graphic3d_ClipPlane>>::Iterator anItemIter(myItems);
       anItemIter.More();
       anItemIter.Next())
  {
    if (anItemIter.Value() == theItem)
    {
      myItems.Remove(anItemIter);
      return true;
    }
  }
  return false;
}

//=================================================================================================

void Graphic3d_SequenceOfHClipPlane::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myToOverrideGlobal)

  for (NCollection_Sequence<occ::handle<Graphic3d_ClipPlane>>::Iterator anIterator(myItems);
       anIterator.More();
       anIterator.Next())
  {
    const occ::handle<Graphic3d_ClipPlane>& aClipPlane = anIterator.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, aClipPlane.get())
  }
}
