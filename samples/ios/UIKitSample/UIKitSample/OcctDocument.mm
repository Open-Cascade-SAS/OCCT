// Copyright (c) 2017 OPEN CASCADE SAS
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

#import <Foundation/Foundation.h>

#include "OcctDocument.h"

#include <Standard_ErrorHandler.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>

#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OcctDocument, Standard_Transient)

// =======================================================================
// function : OcctViewer
// purpose  :
// =======================================================================
OcctDocument::OcctDocument()
{
  try {
    OCC_CATCH_SIGNALS
    
    myApp = XCAFApp_Application::GetApplication();
  } catch (Standard_Failure) {
    Message::DefaultMessenger()->Send (TCollection_AsciiString("Error in creating application") +
                                       Standard_Failure::Caught()->GetMessageString(), Message_Fail);
  }
}

// =======================================================================
// function : OcctDocument
// purpose  :
// =======================================================================
OcctDocument::~OcctDocument()
{
}

// =======================================================================
// function : InitDoc
// purpose  :
// =======================================================================
void OcctDocument::InitDoc()
{
  // close old document
  if (!myOcafDoc.IsNull())
  {
    if (myOcafDoc->HasOpenCommand())
    {
      myOcafDoc->AbortCommand();
    }
    
    myOcafDoc->Main().Root().ForgetAllAttributes(Standard_True);
    
    myApp->Close(myOcafDoc);
    
    myOcafDoc.Nullify();
  }
  
  // create a new document
  myApp->NewDocument(TCollection_ExtendedString("BinXCAF"), myOcafDoc);
  
  // set maximum number of available "undo" actions
  if (!myOcafDoc.IsNull())
  {
    myOcafDoc->SetUndoLimit(10);
  }
}



