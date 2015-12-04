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


#include <IFSelect_SelectFlag.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_SelectFlag,IFSelect_SelectExtract)

IFSelect_SelectFlag::IFSelect_SelectFlag (const Standard_CString flagname)
    : thename (flagname)    {  }

    Standard_CString  IFSelect_SelectFlag::FlagName () const
      {  return thename.ToCString();  }


    Interface_EntityIterator  IFSelect_SelectFlag::RootResult
  (const Interface_Graph& G) const
{
  Standard_Boolean direct = IsDirect();
  Interface_EntityIterator res;
  const Interface_BitMap& bm = G.BitMap();
  Standard_Integer flag = bm.FlagNumber (thename.ToCString());
  if (flag == 0) return res;
  Interface_EntityIterator inp = InputResult(G);

  for (inp.Start(); inp.More(); inp.Next()) {
    Standard_Integer num = G.EntityNumber(inp.Value());
    if (num == 0) continue;
    if (direct == bm.Value(num,flag)) res.AddItem (G.Entity(num));
  }
  return res;
}

    Standard_Boolean  IFSelect_SelectFlag::Sort
  (const Standard_Integer ,
   const Handle(Standard_Transient)&,
   const Handle(Interface_InterfaceModel)& ) const
      {  return Standard_False;  }

    TCollection_AsciiString  IFSelect_SelectFlag::ExtractLabel () const
{
  TCollection_AsciiString lab ("Entities Flagged by ");
  lab.AssignCat (thename);
  return lab;
}
