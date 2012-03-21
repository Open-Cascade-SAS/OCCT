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

#include <IFGraph_SCRoots.ixx>
//#include <Interface_GraphContent.hxx>
#include <IFGraph_StrongComponants.hxx>
#include <IFGraph_ExternalSources.hxx>
#include <Standard_Transient.hxx>

//#define PRINTDEB


IFGraph_SCRoots::IFGraph_SCRoots
  (const Interface_Graph& agraph, const Standard_Boolean whole)
      : IFGraph_StrongComponants (agraph,whole)    {  }

    IFGraph_SCRoots::IFGraph_SCRoots (IFGraph_StrongComponants& subparts)
      : IFGraph_StrongComponants (subparts)    {  }

// StrongComponants racines d un ensemble donne
// On ne tient pas compte du reste eventuel (c est un autre probleme)
// On part du fait que StrongComponants donne les Composants dans l ordre de
// dependance, le premier ne dependant de rien (les autres, on ne sait pas ...)

    void  IFGraph_SCRoots::Evaluate ()
{
  IFGraph_StrongComponants complist (Model(),Standard_False);
  complist.GetFromIter(Loaded());
//  Interface_Graph G(Model());
  Interface_Graph G(thegraph);
#ifdef PRINTDEB
  cout<<" SCRoots:"<<endl;
#endif
  G.ResetStatus();
  for (complist.Start(); complist.More(); complist.Next()) {
    Handle(Standard_Transient) ent = complist.FirstEntity();
    Standard_Integer num = G.EntityNumber(ent);
#ifdef PRINTDEB
    cout<<"   Iteration,num="<<num<<(G.IsPresent(num) ? " Pris" : " A prendre")<<endl;
#endif
    if (!G.IsPresent(num)) {        //  enregistrer pour suivants
      G.GetFromEntity(ent,Standard_True);
      Interface_EntityIterator list = complist.Entities();
      AddPart();
      GetFromIter(list);
    }
  }
}

/*     ce qui suit, c etait autre chose : les SC qui n ont pas d ExternalSource
    Interface_EntityIterator list = complist.Entities();
    IFGraph_ExternalSources  eval (Model());
    eval.GetFromIter(list);
    if (eval.IsEmpty()) {
      AddPart();
      GetFromIter(list);
    }
  }
}
*/
