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

#include <IFGraph_AllConnected.hxx>
#include <IFGraph_ConnectedComponants.hxx>
#include <Interface_Graph.hxx>
#include <Standard_Transient.hxx>

// To work, ConnectedComponants exploits AllConnected:
// Take a 1st Vertex, determine its AllConnected -> here is a 1st
//  Connected Component
// Restart until there are no more free Vertices
//  Honestly, if it doesn't work, see ConnectedVerticesIterator class
//  from GraphTools which does basically the same thing
IFGraph_ConnectedComponants::IFGraph_ConnectedComponants(const Interface_Graph& agraph,
                                                         const Standard_Boolean whole)
    : IFGraph_SubPartsIterator(agraph, whole)
{
}

void IFGraph_ConnectedComponants::Evaluate()
{
  //  Start from "loaded"
  //  For each: if it is noted in the graph, we pass
  //  Otherwise, add the AllConnected as sub-part
  Interface_EntityIterator loaded = Loaded();
  Reset();
  for (loaded.Start(); loaded.More(); loaded.Next())
  {
    const Handle(Standard_Transient)& ent = loaded.Value();
    if (IsInPart(ent))
      continue;
    IFGraph_AllConnected connect(Model(), ent);
    AddPart();
    GetFromIter(connect);
  }
}
