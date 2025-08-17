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

#include <IFGraph_Cumulate.hxx>
#include <IFGraph_StrongComponants.hxx>
#include <IFSelect_SelectRootComps.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_SelectRootComps, IFSelect_SelectExtract)

IFSelect_SelectRootComps::IFSelect_SelectRootComps() {}

// Redone to work at once
// WARNING, we must not be interested in ENTITIES but in COMPONENTS
// i.e. manage CYCLES if there are any

Interface_EntityIterator IFSelect_SelectRootComps::RootResult(const Interface_Graph& G) const
{
  Interface_EntityIterator IEIinput = InputResult(G);
  Interface_EntityIterator iter;
  //  HERE, extract the Components, then consider one Entity from each
  IFGraph_StrongComponants comps(G, Standard_False);
  comps.SetLoad();
  comps.GetFromIter(IEIinput);
  Interface_EntityIterator inp1; // IEIinput reduced to one Entity per Component

  IFGraph_Cumulate GC(G);

  //  We note in the graph: the cumulation of each set (Entity + Shared all
  //  levels). Initial Roots counted only once are good
  //  For Entity: one per Component (doesn't matter)
  for (comps.Start(); comps.More(); comps.Next())
  {
    Handle(Standard_Transient) ent = comps.FirstEntity();
    GC.GetFromEntity(ent);
    inp1.GetOneItem(ent);
  }
  //  Now, we retain, among the inputs, those counted only once
  //  (N.B.: we take inp1, which gives ONE entity per component, simple or cycle)
  for (inp1.Start(); inp1.More(); inp1.Next())
  {
    const Handle(Standard_Transient)& ent = inp1.Value();
    if ((GC.NbTimes(ent) <= 1) == IsDirect())
      iter.GetOneItem(ent);
  }
  return iter;
}

Standard_Boolean IFSelect_SelectRootComps::HasUniqueResult() const
{
  return Standard_True;
}

Standard_Boolean IFSelect_SelectRootComps::Sort(const Standard_Integer,
                                                const Handle(Standard_Transient)&,
                                                const Handle(Interface_InterfaceModel)&) const
{
  return Standard_True;
}

TCollection_AsciiString IFSelect_SelectRootComps::ExtractLabel() const
{
  return TCollection_AsciiString("Local Root Components");
}
