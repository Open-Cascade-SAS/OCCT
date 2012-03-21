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

#include <IFSelect_SelectRoots.ixx>
#include <IFGraph_Cumulate.hxx>


IFSelect_SelectRoots::IFSelect_SelectRoots ()    {  }


// Refait pour travailler en une fois

    Interface_EntityIterator  IFSelect_SelectRoots::RootResult
  (const Interface_Graph& G) const
{
  Interface_EntityIterator input = InputResult(G);
  Interface_EntityIterator iter;
  IFGraph_Cumulate GC(G);

//  On note dans le graphe : le cumul de chaque ensemble (Entite + Shared tous
//  niveaux). Les Roots initiales comptees une seule fois sont bonnes
  for (input.Start(); input.More(); input.Next()) {
    Handle(Standard_Transient) ent = input.Value();
    GC.GetFromEntity(ent);
  }
//  A present, on retient, parmi les inputs, celles comptees une seule fois
  for (input.Start(); input.More(); input.Next()) {
    Handle(Standard_Transient) ent = input.Value();
    if ((Standard_Boolean)(GC.NbTimes(ent) <= 1) == IsDirect()) iter.GetOneItem(ent);
  }
  return iter;
}

    Standard_Boolean  IFSelect_SelectRoots::HasUniqueResult () const
      {  return Standard_True;  }

    Standard_Boolean  IFSelect_SelectRoots::Sort
  (const Standard_Integer , const Handle(Standard_Transient)& ,
   const Handle(Interface_InterfaceModel)& ) const 
      {  return Standard_True;  }

    TCollection_AsciiString  IFSelect_SelectRoots::ExtractLabel () const 
      {  return TCollection_AsciiString("Local Roots");  }
