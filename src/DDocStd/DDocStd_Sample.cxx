// Created on: 1999-12-28
// Created by: Sergey RUIN
// Copyright (c) 1999-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <TDF_Data.hxx>
#include <TDF_Label.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TDocStd_XLinkTool.hxx> 
#include <CDF_Session.hxx> 
//#include <AppSketch_Application.hxx>


// Unused :
#ifdef DEB
static void Sample()
{

  
  //...Creating application (example of sketch)

  Handle(TDocStd_Application) app; // = new AppSketch_Application ();

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
  app->NewDocument("NewDocumentFormat", doc);
  
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

  //Saving document in the file "/tmp/example.caf" give the full path

  app->SaveAs(doc, "/tmp/example.caf");

  //Closing document

  app->Close(doc);

  //Opening document stored in file

  app->Open("/tmp/example.caf", doc);




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
