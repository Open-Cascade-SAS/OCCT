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


#include <IFSelect_ContextWrite.hxx>
#include <IFSelect_WorkLibrary.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Protocol.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_WorkLibrary,Standard_Transient)

//  all deferred but Copy (virtual default)
IFSelect_WorkLibrary::IFSelect_WorkLibrary ()  {  thelevdef = 0;  }

    Standard_Boolean  IFSelect_WorkLibrary::CopyModel
  (const Handle(Interface_InterfaceModel)& /*original*/,
   const Handle(Interface_InterfaceModel)& newmodel,
   const Interface_EntityIterator& list,
   Interface_CopyTool& TC) const
{
  for (list.Start(); list.More(); list.Next())
    TC.TransferEntity (list.Value());

  TC.FillModel(newmodel);

  return Standard_True;
}


    void  IFSelect_WorkLibrary::DumpEntity
  (const Handle(Interface_InterfaceModel)& model,
   const Handle(Interface_Protocol)& protocol,
   const Handle(Standard_Transient)& entity,
   const Handle(Message_Messenger)& S) const
{
  if (thelevhlp.IsNull()) DumpEntity (model,protocol,entity,S,0);
  else                    DumpEntity (model,protocol,entity,S,thelevdef);
}


    void  IFSelect_WorkLibrary::SetDumpLevels
  (const Standard_Integer def, const Standard_Integer max)
{
  thelevdef = def;
  thelevhlp.Nullify();
  if (max >= 0) thelevhlp = new Interface_HArray1OfHAsciiString (0,max);
}

    void  IFSelect_WorkLibrary::DumpLevels
  (Standard_Integer& def, Standard_Integer& max) const
{
  def = thelevdef;
  if (thelevhlp.IsNull()) {  def = 0;  max = -1;  }
  else max = thelevhlp->Upper();
}

    void  IFSelect_WorkLibrary::SetDumpHelp
  (const Standard_Integer level, const Standard_CString help)
{
  if (thelevhlp.IsNull()) return;
  if (level < 0 || level > thelevhlp->Upper()) return;
  Handle(TCollection_HAsciiString) str = new TCollection_HAsciiString (help);
  thelevhlp->SetValue (level,str);
}

    Standard_CString  IFSelect_WorkLibrary::DumpHelp
  (const Standard_Integer level) const
{
  if (thelevhlp.IsNull()) return "";
  if (level < 0 || level > thelevhlp->Upper()) return "";
  Handle(TCollection_HAsciiString) str = thelevhlp->Value (level);
  if (str.IsNull()) return "";
  return str->ToCString();
}
