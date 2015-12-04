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


#include <IFSelect_ContextModif.hxx>
#include <IFSelect_ModifReorder.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Protocol.hxx>
#include <Interface_ShareTool.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_ModifReorder,IFSelect_Modifier)

IFSelect_ModifReorder::IFSelect_ModifReorder (const Standard_Boolean rootlast)
    : IFSelect_Modifier (Standard_True)    {  thertl = rootlast;  }

    void  IFSelect_ModifReorder::Perform (IFSelect_ContextModif& ctx, 
                                          const Handle(Interface_InterfaceModel)& target,
                                          const Handle(Interface_Protocol)& /*protocol*/, 
                                          Interface_CopyTool& /*TC*/) const
{
  Interface_ShareTool sht (ctx.OriginalGraph());
  Interface_EntityIterator list = sht.All (ctx.OriginalModel(),thertl);
  target->ClearEntities();
  for (list.Start(); list.More(); list.Next())  target->AddEntity (list.Value());
}

TCollection_AsciiString  IFSelect_ModifReorder::Label () const
{
  Standard_CString astr = (Standard_CString ) ( thertl ? "Reorder, Roots last" : "Reorder, Roots first");
  return TCollection_AsciiString( astr ) ;
//    ( thertl ? "Reorder, Roots last" : "Reorder, Roots first");
}
