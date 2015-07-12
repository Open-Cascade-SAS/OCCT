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


#include <IFGraph_AllShared.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Standard_Transient.hxx>

IFGraph_AllShared::IFGraph_AllShared (const Interface_Graph& agraph)
      : thegraph (agraph)    {  }


    IFGraph_AllShared::IFGraph_AllShared
  (const Interface_Graph& agraph, const Handle(Standard_Transient)& ent)
      : thegraph (agraph)
{
  if (!agraph.Model()->Contains(ent)) return;
  GetFromEntity(ent);
}

    void  IFGraph_AllShared::GetFromEntity
  (const Handle(Standard_Transient)& ent)
      {  thegraph.GetFromEntity(ent,Standard_True);  }  // le fait pour nous

     void IFGraph_AllShared::GetFromIter (const Interface_EntityIterator& iter)
{
  for (iter.Start(); iter.More(); iter.Next())
    thegraph.GetFromEntity(iter.Value(),Standard_True);
}

     void IFGraph_AllShared::ResetData ()
      {  Reset();  thegraph.Reset();  }

     void IFGraph_AllShared::Evaluate()
      {  Reset();  GetFromGraph(thegraph);  }
