// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <IFGraph_StrongComponants.ixx>
//#include <IFGraph_SortedStrongs.hxx>
#include <Interface_Graph.hxx>
#include <Interface_GraphContent.hxx>

//#define PRINTDEB
#define pbgraphtool


IFGraph_StrongComponants::IFGraph_StrongComponants
  (const Interface_Graph& agraph, const Standard_Boolean whole)
      : IFGraph_SubPartsIterator (agraph, whole)    {  }

    void  IFGraph_StrongComponants::Evaluate ()
{
  Interface_GraphContent iter = Loaded();
  Interface_Graph G(thegraph); G.GetFromIter(iter,0);
#ifdef pbgraphtool
  Standard_Integer nb = G.Size();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (!G.IsPresent(i)) continue;
    AddPart();
    GetFromEntity (G.Entity(i),Standard_False);
  }
#else
#ifdef PRINTDEB
  cout<<"StrongComponants :"<<endl;
#endif
  for (IFGraph_SortedStrongs res(G); res.More(); res.Next()) {
    Standard_Integer nb = res.NbVertices();
#ifdef PRINTDEB
    cout<<"  Iteration, Vertices:"<<nb<<" :";
#endif
    if (nb == 0) continue;
    AddPart();
    for (Standard_Integer i = 1; i <= nb; i ++)
#ifdef PRINTDEB
      {
	Handle(Standard_Transient) oneres = res.Value(i);
	cout<<" "<<G.EntityNumber(oneres);
	GetFromEntity(oneres,Standard_False);
      }
    cout<<endl;
#else
      GetFromEntity(res.Value(i),Standard_False);
#endif
  }
#endif
}
