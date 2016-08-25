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


#include <IFGraph_SubPartsIterator.hxx>
#include <IFSelect_DispGlobal.hxx>
#include <IFSelect_Selection.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_DispGlobal,IFSelect_Dispatch)

// Genere un seul paquet avec la sortie finale
IFSelect_DispGlobal::IFSelect_DispGlobal ()    {  }

    TCollection_AsciiString  IFSelect_DispGlobal::Label () const
{  return TCollection_AsciiString ("One File for All Input");  }

    Standard_Boolean  IFSelect_DispGlobal::LimitedMax
  (const Standard_Integer /* nbent */, Standard_Integer& pcount) const 
      {  pcount = 1;  return Standard_True;  }

// 1 packet ( a partir de UniqueResult)
      void IFSelect_DispGlobal::Packets
  (const Interface_Graph& G, IFGraph_SubPartsIterator& packs) const 
{
  packs.AddPart();
  packs.GetFromIter(FinalSelection()->UniqueResult(G));
}
