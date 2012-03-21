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

#include <IFSelect_SelectRootComps.ixx>
#include <IFGraph_StrongComponants.hxx>
#include <IFGraph_Cumulate.hxx>


IFSelect_SelectRootComps::IFSelect_SelectRootComps ()    {  }


// Refait pour travailler en une fois
// ATTENTION, il ne faut pas s interesser aux ENTITES mais aux COMPOSANTS
// c-a-d gerer les CYCLES s il y en a

    Interface_EntityIterator  IFSelect_SelectRootComps::RootResult
  (const Interface_Graph& G) const
{
  Interface_EntityIterator IEIinput = InputResult(G);
  Interface_EntityIterator iter;
//  ICI, extraire les Componants, puis considerer une Entite de chacun
  IFGraph_StrongComponants comps(G,Standard_False);
  comps.SetLoad();
  comps.GetFromIter(IEIinput);
  Interface_EntityIterator inp1;  // IEIinput reduit a une Entite par Composant

  IFGraph_Cumulate GC(G);

//  On note dans le graphe : le cumul de chaque ensemble (Entite + Shared tous
//  niveaux). Les Roots initiales comptees une seule fois sont bonnes
//  Pour Entite : une par Componant (peu importe)
  for (comps.Start(); comps.More(); comps.Next()) {
    Handle(Standard_Transient) ent = comps.FirstEntity();
    GC.GetFromEntity(ent);
    inp1.GetOneItem(ent);
  }
//  A present, on retient, parmi les inputs, celles comptees une seule fois
//  (N.B.: on prend inp1, qui donne UNE entite par composant, simple ou cycle)
  for (inp1.Start(); inp1.More(); inp1.Next()) {
    Handle(Standard_Transient) ent = inp1.Value();
    if ((Standard_Boolean)(GC.NbTimes(ent) <= 1) == IsDirect()) iter.GetOneItem(ent);
  }
  return iter;
}

    Standard_Boolean  IFSelect_SelectRootComps::HasUniqueResult () const
      {  return Standard_True;  }


    Standard_Boolean  IFSelect_SelectRootComps::Sort
  (const Standard_Integer , const Handle(Standard_Transient)& ,
   const Handle(Interface_InterfaceModel)& ) const 
      {  return Standard_True;  }

    TCollection_AsciiString  IFSelect_SelectRootComps::ExtractLabel () const 
{  return TCollection_AsciiString("Local Root Componants");  }
