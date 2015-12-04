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


#include <IFSelect_IntParam.hxx>
#include <IFSelect_SelectEntityNumber.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_SelectEntityNumber,IFSelect_SelectBase)

IFSelect_SelectEntityNumber::IFSelect_SelectEntityNumber ()    {  }

    void  IFSelect_SelectEntityNumber::SetNumber
  (const Handle(IFSelect_IntParam)& num)
      {  thenum = num;  }

    Handle(IFSelect_IntParam)  IFSelect_SelectEntityNumber::Number () const 
      {  return thenum;  }

    Interface_EntityIterator  IFSelect_SelectEntityNumber::RootResult
  (const Interface_Graph& G) const 
{
  Interface_EntityIterator iter;
  Standard_Integer num = 0;
  if (!thenum.IsNull()) num = thenum->Value();
  if (num < 1) return iter;        // vide si num < 1 ou num > NbEntities
  if (num <= G.Size()) iter.GetOneItem(G.Entity(num));
  return iter;
}

    TCollection_AsciiString  IFSelect_SelectEntityNumber::Label () const 
{
  Standard_Integer num = 0;
  if (!thenum.IsNull()) num = thenum->Value();
  TCollection_AsciiString labl(num);
  labl.InsertBefore (1,"Entity Number ");
  return labl;
}
