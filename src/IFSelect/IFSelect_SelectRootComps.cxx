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


#include <IFGraph_Cumulate.hxx>
#include <IFGraph_StrongComponants.hxx>
#include <IFSelect_SelectRootComps.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_SelectRootComps,IFSelect_SelectExtract)

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
    if ((GC.NbTimes(ent) <= 1) == IsDirect()) iter.GetOneItem(ent);
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
