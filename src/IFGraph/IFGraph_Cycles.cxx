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


#include <IFGraph_Cycles.hxx>
#include <IFGraph_StrongComponants.hxx>
#include <Interface_Graph.hxx>
#include <Interface_GraphContent.hxx>

//  Cycles utilise les services de StrongComponants :
//  Il retient les Strong Componants qui ne sont pas Single
IFGraph_Cycles::IFGraph_Cycles
  (const Interface_Graph& agraph, const Standard_Boolean whole)
      :  IFGraph_SubPartsIterator (agraph,whole)    {  }

    IFGraph_Cycles::IFGraph_Cycles (IFGraph_StrongComponants& subparts)
      :  IFGraph_SubPartsIterator (subparts)    {  }


    void  IFGraph_Cycles::Evaluate ()
{
  IFGraph_StrongComponants complist(Model(),Standard_False);
  complist.GetFromIter(Loaded());
  for (complist.Start(); complist.More(); complist.Next()) {
    if (complist.IsSingle()) continue;
    AddPart();
    GetFromIter(complist.Entities());
  }
}
