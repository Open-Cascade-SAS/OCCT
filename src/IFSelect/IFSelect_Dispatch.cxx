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

#include <IFSelect_Dispatch.ixx>
#include <IFGraph_Compare.hxx>


void  IFSelect_Dispatch::SetRootName
  (const Handle(TCollection_HAsciiString)& name)
{
  thename = name;
}

    Standard_Boolean  IFSelect_Dispatch::HasRootName () const
      {  return (!thename.IsNull());  }

    const Handle(TCollection_HAsciiString)&  IFSelect_Dispatch::RootName () const
      {  return thename;  }

    void  IFSelect_Dispatch::SetFinalSelection
  (const Handle(IFSelect_Selection)& sel)
      {  thefinal = sel;  }

    Handle(IFSelect_Selection)  IFSelect_Dispatch::FinalSelection () const 
      {  return thefinal;  }

    IFSelect_SelectionIterator  IFSelect_Dispatch::Selections () const 
{
  IFSelect_SelectionIterator iter;
  iter.AddItem(thefinal);
  for(; iter.More(); iter.Next()) {
    iter.Value()->FillIterator(iter);    // Iterateur qui se court apres
  }
  return iter;
}


    Standard_Boolean  IFSelect_Dispatch::CanHaveRemainder () const 
      {  return Standard_False;  }

    Standard_Boolean  IFSelect_Dispatch::LimitedMax
  (const Standard_Integer , Standard_Integer& max) const 
      {  max = 0;  return Standard_False;  }

    Interface_EntityIterator  IFSelect_Dispatch::GetEntities
  (const Interface_Graph& G) const
      {  return thefinal->UniqueResult(G);  }

    Standard_Boolean  IFSelect_Dispatch::PacketsCount
  (const Interface_Graph& , Standard_Integer& pcount) const 
      {  pcount = 0;  return Standard_False;  }    // par defaut 0

    Interface_EntityIterator  IFSelect_Dispatch::Packeted
  (const Interface_Graph& G) const
{
  Interface_EntityIterator total  = GetEntities(G);
  Interface_EntityIterator remain = Remainder(G);
  if (remain.NbEntities() == 0) return total;
//  sinon, faire la difference !
  IFGraph_Compare GC(G);
  GC.GetFromIter (total, Standard_True);
  GC.GetFromIter (remain,Standard_False);
  return GC.FirstOnly();
}

    Interface_EntityIterator  IFSelect_Dispatch::Remainder
  (const Interface_Graph& ) const
      {  Interface_EntityIterator iter;  return iter;  }    // par defaut vide
