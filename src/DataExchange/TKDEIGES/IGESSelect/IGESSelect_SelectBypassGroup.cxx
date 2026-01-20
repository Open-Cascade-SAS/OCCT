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

#include <IGESBasic_GroupWithoutBackP.hxx>
#include <IGESSelect_SelectBypassGroup.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <MoniTool_Macros.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSelect_SelectBypassGroup, IFSelect_SelectExplore)

#define TypePourGroup 402

IGESSelect_SelectBypassGroup::IGESSelect_SelectBypassGroup(const int level)
    : IFSelect_SelectExplore(level)
{
}

bool IGESSelect_SelectBypassGroup::Explore(const int /*level*/,
                                                       const occ::handle<Standard_Transient>& ent,
                                                       const Interface_Graph& /*G*/,
                                                       Interface_EntityIterator& explored) const
{
  DeclareAndCast(IGESBasic_Group, gr, ent); // Group groups them all
  if (gr.IsNull())
    return true;

  int i, nb = gr->NbEntities();
  for (i = 1; i <= nb; i++)
    explored.AddItem(gr->Entity(i));
  return true;
}

TCollection_AsciiString IGESSelect_SelectBypassGroup::ExploreLabel() const
{
  return TCollection_AsciiString("Content of Groups");
}
