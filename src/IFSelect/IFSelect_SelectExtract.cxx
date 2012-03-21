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

#include <IFSelect_SelectExtract.ixx>


IFSelect_SelectExtract::IFSelect_SelectExtract ()
      {  thesort = Standard_True;  }

    Standard_Boolean  IFSelect_SelectExtract::IsDirect () const 
      {  return thesort;  }

    void  IFSelect_SelectExtract::SetDirect (const Standard_Boolean direct)
      {  thesort = direct;  }


    Interface_EntityIterator  IFSelect_SelectExtract::RootResult
  (const Interface_Graph& G) const 
{
  Interface_EntityIterator iter;
  Interface_EntityIterator inputer = InputResult(G);  // tient compte de tout
  Handle(Interface_InterfaceModel) model = G.Model();
  Standard_Integer rank = 0;
  for (inputer.Start(); inputer.More(); inputer.Next()) {
    Handle(Standard_Transient) ent = inputer.Value();
    rank ++;
    if (SortInGraph(rank,ent,G) == thesort) iter.GetOneItem(ent);
  }
  return iter;
}


    Standard_Boolean  IFSelect_SelectExtract::SortInGraph
  (const Standard_Integer rank, const Handle(Standard_Transient)& ent,
   const Interface_Graph& G) const
      {  return Sort (rank, ent, G.Model());  }


    TCollection_AsciiString  IFSelect_SelectExtract::Label () const
{
  TCollection_AsciiString labl;
  if ( thesort) labl.AssignCat("Picked: ");
  if (!thesort) labl.AssignCat("Removed: ");
  labl.AssignCat(ExtractLabel());
  return labl;
}
