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


#include <IFGraph_Compare.hxx>
#include <IFSelect_SelectIntersection.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_SelectIntersection,IFSelect_SelectCombine)

IFSelect_SelectIntersection::IFSelect_SelectIntersection ()    {  }


    Interface_EntityIterator  IFSelect_SelectIntersection::RootResult
  (const Interface_Graph& G) const 
{
  IFGraph_Compare GC(G);
  Standard_Integer nb = NbInputs();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    GC.GetFromIter(Input(i)->RootResult(G), (i==1));
    if (i > 1 && i < nb) {
      Interface_EntityIterator comm = GC.Common();
      GC.ResetData();
      GC.GetFromIter (comm,Standard_True);
    }
  }
  return GC.Common();
}

    TCollection_AsciiString  IFSelect_SelectIntersection::Label () const 
      {  return TCollection_AsciiString("Intersection (AND)");  }
