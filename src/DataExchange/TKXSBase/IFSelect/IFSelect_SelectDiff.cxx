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

#include <IFGraph_Compare.hxx>
#include <IFSelect_SelectDiff.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_SelectDiff, IFSelect_SelectControl)

IFSelect_SelectDiff::IFSelect_SelectDiff() = default;

Interface_EntityIterator IFSelect_SelectDiff::RootResult(const Interface_Graph& G) const
{
  IFGraph_Compare GC(G);
  GC.GetFromIter(MainInput()->RootResult(G), true); // first
  if (HasSecondInput())
    GC.GetFromIter(SecondInput()->RootResult(G), false); // second
  return GC.FirstOnly();
}

bool IFSelect_SelectDiff::HasUniqueResult() const
{
  return true;
} // naturellement unique

TCollection_AsciiString IFSelect_SelectDiff::Label() const
{
  return TCollection_AsciiString("Differences");
}
