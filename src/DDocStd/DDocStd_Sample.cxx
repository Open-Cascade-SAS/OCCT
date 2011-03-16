// File:	DDocStd_Sample.cxx
// Created:	Tue Dec 28 09:46:59 1999
// Author:	Sergey RUIN
//		<srn@popox.nnov.matra-dtv.fr>


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
