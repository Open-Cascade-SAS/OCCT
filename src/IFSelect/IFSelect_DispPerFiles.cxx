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


#include <IFGraph_SCRoots.hxx>
#include <IFGraph_SubPartsIterator.hxx>
#include <IFSelect_DispPerFiles.hxx>
#include <IFSelect_IntParam.hxx>
#include <IFSelect_Selection.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceError.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_DispPerFiles,IFSelect_Dispatch)

IFSelect_DispPerFiles::IFSelect_DispPerFiles ()    {  }

    Handle(IFSelect_IntParam)  IFSelect_DispPerFiles::Count () const 
      {  return thecount;  }

    void  IFSelect_DispPerFiles::SetCount
  (const Handle(IFSelect_IntParam)& pcount)
      {  thecount = pcount;  }

    Standard_Integer  IFSelect_DispPerFiles::CountValue () const
{
  Standard_Integer pcount = 0;
  if (!thecount.IsNull()) pcount = thecount->Value();
  if (pcount <= 0) pcount = 1;    // option prise par defaut
  return pcount;
}

    TCollection_AsciiString  IFSelect_DispPerFiles::Label () const
{
  TCollection_AsciiString lab(CountValue());
  lab.Insert(1,"Maximum ");
  lab.AssignCat(" Files");
  return lab;
}


    Standard_Boolean  IFSelect_DispPerFiles::LimitedMax
  (const Standard_Integer /* nbent */, Standard_Integer& pcount) const 
{
  pcount = CountValue();
  return Standard_True;
}

    void  IFSelect_DispPerFiles::Packets
  (const Interface_Graph& G, IFGraph_SubPartsIterator& packs) const 
{
//  Ressemble a DispPerOne, mais fait "count" AddPart racines
  Standard_Integer pcount = CountValue();

  IFGraph_SCRoots roots(G,Standard_False);
  roots.SetLoad();
  roots.GetFromIter(FinalSelection()->UniqueResult(G));
//   SCRoots a initie la resolution : decoupage en StrongComponants + selection
//   des racines. Un paquet correspond des lors a <count> racines
//   Donc, il faut iterer sur les Parts de roots et les prendre par <count>
  roots.Start();                            // Start fait Evaluate specifique
  Standard_Integer nb = roots.NbParts();
  if (pcount > 0) pcount = (nb-1) / pcount +1;    // par packet

  Standard_Integer i = 0;
  for (; roots.More(); roots.Next()) {      // Start deja fait
    if (i == 0) packs.AddPart();
    i ++;  if (i >= pcount) i = 0;  // regroupement selon "count"
    packs.GetFromIter(roots.Entities());
  }
}
