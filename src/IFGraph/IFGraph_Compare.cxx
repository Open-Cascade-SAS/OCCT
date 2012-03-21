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

#include <IFGraph_Compare.ixx>
#include <IFGraph_AllShared.hxx>



//  Comparateur de deux sous-ensembles d un Modele
//  Au premier  sous-ensemble, est attribue le Status 1
//  Au deuxieme sous-ensemble, est attribue le Status 2
//  La partie commune se voit attribuer le Status 3

IFGraph_Compare::IFGraph_Compare (const Interface_Graph& agraph)
      : thegraph (agraph)    {  }

    void  IFGraph_Compare::GetFromEntity
  (const Handle(Standard_Transient)& ent, const Standard_Boolean first)
{
  IFGraph_AllShared iter(thegraph.Model(),ent);
  GetFromIter(iter,first);
}

    void  IFGraph_Compare::GetFromIter
  (const Interface_EntityIterator& iter, const Standard_Boolean first)
{
  Standard_Integer stat = 2;
  if (first) stat = 1;
  thegraph.GetFromIter(iter,stat,3,Standard_False);
}


    void  IFGraph_Compare::Merge ()
{
  thegraph.ChangeStatus (2,1);
  thegraph.ChangeStatus (3,1);
}

    void  IFGraph_Compare::RemoveSecond ()
{
  thegraph.ChangeStatus (3,1);
  thegraph.RemoveStatus (2);
}

    void  IFGraph_Compare::KeepCommon ()
{
  thegraph.RemoveStatus (1);
  thegraph.RemoveStatus (2);
  thegraph.ChangeStatus (3,1);
}

    void  IFGraph_Compare::ResetData ()
      {  Reset();  thegraph.Reset();  }

    void  IFGraph_Compare::Evaluate ()
{
  Reset();  GetFromGraph(thegraph);  // Evaluation deja faite par le graphe
}

    Interface_EntityIterator  IFGraph_Compare::Common () const
      {  return Interface_GraphContent(thegraph,3);  }

    Interface_EntityIterator  IFGraph_Compare::FirstOnly () const 
      {  return Interface_GraphContent(thegraph,1);  }

    Interface_EntityIterator  IFGraph_Compare::SecondOnly () const 
      {  return Interface_GraphContent(thegraph,2);  }
