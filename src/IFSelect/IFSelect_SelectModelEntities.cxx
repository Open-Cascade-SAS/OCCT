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


#include <IFSelect_SelectModelEntities.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_SelectModelEntities,IFSelect_SelectBase)

IFSelect_SelectModelEntities::IFSelect_SelectModelEntities ()    {  }

    Interface_EntityIterator  IFSelect_SelectModelEntities::RootResult
  (const Interface_Graph& G) const
      {  return G.Model()->Entities();  }

    Interface_EntityIterator  IFSelect_SelectModelEntities::CompleteResult
  (const Interface_Graph& G) const 
      {  return G.Model()->Entities();  }

    TCollection_AsciiString  IFSelect_SelectModelEntities::Label () const 
{  return TCollection_AsciiString("All Entities from Model");  }
