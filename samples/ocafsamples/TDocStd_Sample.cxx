// Created on: 1999-12-28
// Created by: Sergey RUIN
// Copyright (c) 1999-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <TDF_Data.hxx>
#include <TDF_Label.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TDocStd_XLinkTool.hxx> 
#include <CDF_Session.hxx> 

// ====================================================================================
// This sample contains template for typical actions with OCAF document at application 
// level (store / retrieve)
// ====================================================================================

#ifdef DEB
static void Sample()
{

  
  //...Creating application 

  Handle(TDocStd_Application) app; 

  // the application is now handled by the CDF_Session variable


  //...Retrieving the  application


  if (!CDF_Session::Exists()) {
    Handle(CDF_Session) S = CDF_Session::CurrentSession();  
    if (!S->HasCurrentApplication())  
    Standard_DomainError::Raise("DDocStd::Find no applicative session");
    app = Handle(TDocStd_Application)::DownCast(S->CurrentApplication());
  }
  else {
    // none active application
  }
  
  //...Creating the new document (document conatins a framework) 

  Handle(TDocStd_Document) doc;
  app->NewDocument("Standard", doc);
  
  //...Getting application to which the document belongs

  app =  Handle(TDocStd_Application)::DownCast(doc->Application());


  //...Getting application to which the document belongs

  app =  Handle(TDocStd_Application)::DownCast(doc->Application());
 

  //...Getting data framework from document

  Handle(TDF_Data) framework = doc->GetData();

  //...Retrieving the document from a label of its framework

  TDF_Label label; 
  doc =  TDocStd_Document::Get(label);

  //... Filling document with data

  //Saving document in the file "/tmp/example.std" give the full path

  app->SaveAs(doc, "/tmp/example.std");

  //Closing document

  app->Close(doc);

  //Opening document stored in file

  app->Open("/tmp/example.std", doc);




  //Coping content of a document to another document with possibility update copy in future

  Handle(TDocStd_Document) doc1;  
  Handle(TDocStd_Document) doc2;


  TDF_Label source = doc1->GetData()->Root();
  TDF_Label target = doc2->GetData()->Root();
  TDocStd_XLinkTool XLinkTool;

  //Coping content of a document to another document with possibility update copy in future

  XLinkTool.CopyWithLink(target,source); //Now target document has a copy of source document , the copy also has
                                                 //a link to have possibility update content of the copy if orginal changed

  //...Something is chaneged in source document

  //Updating copy in target document 

  XLinkTool.UpdateLink(target);

  //Cping content of a document to another document

  XLinkTool.Copy(target, source); //Now target document has a copy of source document, there is no link between
                                  //the copy  and original
  
  
}
#endif
