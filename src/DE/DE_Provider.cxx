// Copyright (c) 2022 OPEN CASCADE SAS
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

#include <DE_Provider.hxx>

#include <DE_ConfigurationNode.hxx>
#include <Message.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DE_Provider, Standard_Transient)

//=======================================================================
// function : DE_Provider
// purpose  :
//=======================================================================
DE_Provider::DE_Provider()
{}

//=======================================================================
// function : DE_Provider
// purpose  :
//=======================================================================
DE_Provider::DE_Provider(const Handle(DE_ConfigurationNode)& theNode)
  :myNode(theNode)
{}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
Standard_Boolean DE_Provider::Read(const TCollection_AsciiString& thePath,
                                   const Handle(TDocStd_Document)& theDocument,
                                   Handle(XSControl_WorkSession)& theWS,
                                   const Message_ProgressRange& theProgress)
{
  (void)thePath;
  (void)theDocument;
  (void)theWS;
  (void)theProgress;
  Message::SendFail() << "Error: provider " << GetFormat() <<
    " " << GetVendor() <<" doesn't support read operation";
  return Standard_False;
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
Standard_Boolean DE_Provider::Write(const TCollection_AsciiString& thePath,
                                    const Handle(TDocStd_Document)& theDocument,
                                    Handle(XSControl_WorkSession)& theWS,
                                    const Message_ProgressRange& theProgress)
{
  (void)thePath;
  (void)theDocument;
  (void)theWS;
  (void)theProgress;
  Message::SendFail() << "Error: provider " << GetFormat() <<
    " " << GetVendor() << " doesn't support write operation";
  return Standard_False;
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
Standard_Boolean DE_Provider::Read(const TCollection_AsciiString& thePath,
                                   const Handle(TDocStd_Document)& theDocument,
                                   const Message_ProgressRange& theProgress)
{
  (void)thePath;
  (void)theDocument;
  (void)theProgress;
  Message::SendFail() << "Error: provider " << GetFormat() <<
    " " << GetVendor() << " doesn't support read operation";
  return Standard_False;
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
Standard_Boolean DE_Provider::Write(const TCollection_AsciiString& thePath,
                                    const Handle(TDocStd_Document)& theDocument,
                                    const Message_ProgressRange& theProgress)
{
  (void)thePath;
  (void)theDocument;
  (void)theProgress;
  Message::SendFail() << "Error: provider " << GetFormat() <<
    " " << GetVendor() << " doesn't support write operation";
  return Standard_False;
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
Standard_Boolean DE_Provider::Read(const TCollection_AsciiString& thePath,
                                   TopoDS_Shape& theShape,
                                   Handle(XSControl_WorkSession)& theWS,
                                   const Message_ProgressRange& theProgress)
{
  (void)thePath;
  (void)theShape;
  (void)theWS;
  (void)theProgress;
  Message::SendFail() << "Error: provider " << GetFormat() <<
    " " << GetVendor() << " doesn't support read operation";
  return Standard_False;
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
Standard_Boolean DE_Provider::Write(const TCollection_AsciiString& thePath,
                                    const TopoDS_Shape& theShape,
                                    Handle(XSControl_WorkSession)& theWS,
                                    const Message_ProgressRange& theProgress)
{
  (void)thePath;
  (void)theShape;
  (void)theWS;
  (void)theProgress;
  Message::SendFail() << "Error: provider " << GetFormat() <<
    " " << GetVendor() << " doesn't support write operation";
  return Standard_False;
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
Standard_Boolean DE_Provider::Read(const TCollection_AsciiString& thePath,
                                   TopoDS_Shape& theShape,
                                   const Message_ProgressRange& theProgress)
{
  (void)thePath;
  (void)theShape;
  (void)theProgress;
  Message::SendFail() << "Error: provider " << GetFormat() <<
    " " << GetVendor() << " doesn't support read operation";
  return Standard_False;
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
Standard_Boolean DE_Provider::Write(const TCollection_AsciiString& thePath,
                                    const TopoDS_Shape& theShape,
                                    const Message_ProgressRange& theProgress)
{
  (void)thePath;
  (void)theShape;
  (void)theProgress;
  Message::SendFail() << "Error: provider " << GetFormat() <<
    " " << GetVendor() << " doesn't support write operation";
  return Standard_False;
}
