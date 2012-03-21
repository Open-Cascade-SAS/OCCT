// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
