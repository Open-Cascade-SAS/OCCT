// Created on: 2000-03-01
// Created by: Denis PASCAL
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#include <DDocStd.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Viewer.hxx>
#include <DDocStd_DrawDocument.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TDataStd_Name.hxx>
#include <Draw.hxx>
#include <Standard_GUID.hxx>
#include <Standard_ExtString.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDF.hxx>
#include <TDF_Data.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Tool.hxx> 

#include <OSD_Path.hxx>
#include <OSD_OpenFile.hxx>
#include <TDocStd_PathParser.hxx>
#include <XmlLDrivers.hxx>

#include <AIS_InteractiveContext.hxx>
#include <TPrsStd_AISViewer.hxx>
#include <ViewerTest.hxx>
#include <V3d_Viewer.hxx>

#ifndef _WIN32
extern Draw_Viewer dout;
#else
Standard_IMPORT Draw_Viewer dout;
#endif

//=======================================================================
//function : ListDocuments
//purpose  : 
//=======================================================================

static Standard_Integer DDocStd_ListDocuments (Draw_Interpretor& di,
					       Standard_Integer nb,
					       const char** /*a*/)
{  
  if (nb == 1) {
    Handle(TDocStd_Application) A = DDocStd::GetApplication();
    Handle(TDocStd_Document) D;
    Standard_Integer nbdoc = A->NbDocuments();
    for (Standard_Integer i = 1; i <= nbdoc; i++) {
      A->GetDocument(i,D);
      di <<"document " << i;
      if (D->IsSaved()) {
	TCollection_AsciiString GetNameAsciiString(D->GetName().ToExtString(),'?');
	TCollection_AsciiString GetPathAsciiString(D->GetPath().ToExtString(),'?');
	//cout << " name : " << D->GetName();
	//cout << " path : " << D->GetPath();
	di << " name : " << GetNameAsciiString.ToCString();
	di << " path : " << GetPathAsciiString.ToCString();
      }
      else di << " not saved";
      di << "\n";
    }
    return 0;
  }
  di << "DDocStd_ListDocuments : Error\n";
  return 1; 
}


//=======================================================================
//function : NewDocument
//purpose  : 
//=======================================================================

static Standard_Integer DDocStd_NewDocument (Draw_Interpretor& di,
					     Standard_Integer nb,
					     const char** a)
{    
  Handle(TDocStd_Document) D; 
  Handle(DDocStd_DrawDocument) DD;
  if (nb == 2) {
    if (!DDocStd::GetDocument(a[1],D,Standard_False)) { 
      D = new TDocStd_Document("dummy");  
      DD = new DDocStd_DrawDocument(D);  
      Draw::Set(a[1],DD);       
      di << "document (not handled by application)  " << a[1] << " created\n";  
      DDocStd::ReturnLabel(di,D->Main()); 
    }    
    else di << a[1] << " is already a document\n";
    return 0;
  }
  if (nb == 3) {   
    if (!DDocStd::GetDocument(a[1],D,Standard_False)) {  
      Handle(TDocStd_Application) A = DDocStd::GetApplication();
      A->NewDocument(a[2],D);  
      DD = new DDocStd_DrawDocument(D);  
      TDataStd_Name::Set(D->GetData()->Root(),a[1]);  
      Draw::Set(a[1],DD);    
      di << "document " << a[1] << " created\n";    
      DDocStd::ReturnLabel(di,D->Main()); 
    }
    else di << a[1] << " is already a document\n";
    return 0;
  }   
  di << "DDocStd_NewDocument : Error\n";
  return 1;
}

//=======================================================================
//function : Open
//purpose  : 
//=======================================================================

static Standard_Integer DDocStd_Open (Draw_Interpretor& di,
				      Standard_Integer nb,
				      const char** a)
{   
  if (nb >= 3) {
    TCollection_ExtendedString path (a[1]); 
    Handle(TDocStd_Application) A = DDocStd::GetApplication();
    Handle(TDocStd_Document) D;
    Standard_Integer insession = A->IsInSession(path);
    if (insession > 0) {  
      di <<"document " << insession << "  is already in session\n";
      return 0;
    }
    PCDM_ReaderStatus theStatus;

    Standard_Boolean anUseStream = Standard_False;
    for ( Standard_Integer i = 3; i < nb; i++ )
    {
      if (!strcmp (a[i], "-stream"))
      {
        di << "standard SEEKABLE stream is used\n";
        anUseStream = Standard_True;
        break;
      }
    }

    if (anUseStream)
    {
      std::ifstream aFileStream;
      OSD_OpenStream (aFileStream, path, std::ios::in | std::ios::binary);

      theStatus = A->Open (aFileStream, D);
    }
    else
    {
      theStatus = A->Open(path,D);
    }
    if (theStatus == PCDM_RS_OK && !D.IsNull()) {
      Handle(DDocStd_DrawDocument) DD = new DDocStd_DrawDocument(D);
      TDataStd_Name::Set(D->GetData()->Root(),a[2]);
      Draw::Set(a[2],DD);
      return 0; 
    } else {
      switch ( theStatus ) {
      case PCDM_RS_AlreadyRetrieved: 
      case PCDM_RS_AlreadyRetrievedAndModified: {
	di << " already retrieved \n" ;  
	break;
      }
      case PCDM_RS_NoDriver: {
	di << " could not retrieve , no Driver to make it \n" ;
	break ;
      }
      case PCDM_RS_UnknownDocument:
      case PCDM_RS_NoModel: {
	di << " could not retrieve , Unknown Document or No Model \n";
	break ; 
      }
      case PCDM_RS_TypeNotFoundInSchema:
      case PCDM_RS_UnrecognizedFileFormat: {
	di << " could not retrieve , Type not found or Unrecognized File Format\n";
	break ;
      }
      case PCDM_RS_PermissionDenied: {
	di << " could not retrieve , permission denied \n" ;  
	break;
      }
      default:
	di << " could not retrieve \n" ;  
	break;
      }
      di << "DDocStd_Open : Error\n";
    }	
  }
  return 1;
}

//=======================================================================
//function : Save
//purpose  : 
//=======================================================================

static Standard_Integer DDocStd_Save (Draw_Interpretor& di,
				      Standard_Integer nb,
				      const char** a)
{  
  if (nb == 2) {
    Handle(TDocStd_Document) D;    
    if (!DDocStd::GetDocument(a[1],D)) return 1;
    Handle(TDocStd_Application) A = DDocStd::GetApplication();
    if (!D->IsSaved()) {
      di << "this document has never been saved\n";
      return 0;
    }
    A->Save(D);
    return 0; 
  }
  di << "DDocStd_Save : Error\n";
  return 1;
}

//=======================================================================
//function : SaveAs
//purpose  : 
//=======================================================================

static Standard_Integer DDocStd_SaveAs (Draw_Interpretor& di,
                                        Standard_Integer nb,
                                        const char** a)
{
  if (nb >= 3) {
    Handle(TDocStd_Document) D;    
    if (!DDocStd::GetDocument(a[1],D)) return 1;  
    TCollection_ExtendedString path (a[2]); 
    Handle(TDocStd_Application) A = DDocStd::GetApplication();
    PCDM_StoreStatus theStatus;

    Standard_Boolean anUseStream(Standard_False), isSaveEmptyLabels(Standard_False);
    for ( Standard_Integer i = 3; i < nb; i++ )
    {
      if (!strcmp (a[i], "-stream"))
      {
        di << "standard SEEKABLE stream is used\n";
        anUseStream = Standard_True;
        break;
      } else {
        isSaveEmptyLabels =  ((atoi (a[3])) != 0);
        D->SetEmptyLabelsSavingMode(isSaveEmptyLabels);
      }
    }
    if (anUseStream)
    {
      std::ofstream aFileStream;
      OSD_OpenStream (aFileStream, path, std::ios::out | std::ios::binary);
      theStatus = A->SaveAs (D, aFileStream);
    }
    else
    {
      theStatus = A->SaveAs(D,path);
    }

    if (theStatus != PCDM_SS_OK ) {
      switch ( theStatus ) {
      case PCDM_SS_DriverFailure: {
        di << "Error saving document: Could not store , no driver found to make it\n";
        break ;
                                  }
      case PCDM_SS_WriteFailure: {
        di << "Error saving document: Write access failure\n";
        break;
                                 }
      case PCDM_SS_Failure: {
        di << "Error saving document: Write failure\n" ;
        break;
                            }
      case PCDM_SS_Doc_IsNull: {
        di << "Error saving document: No document to save\n";
        break ;
                               }
      case PCDM_SS_No_Obj: {
        di << "Error saving document: No objects written\n";
        break;
                           }
      case PCDM_SS_Info_Section_Error: {
        di << "Error saving document: Write info section failure\n" ;
        break;
                                       }
      default:
        break;
      }
      return 1;
    } else {
      return 0; 
    }
  }
  di << "DDocStd_SaveAs : Error not enough argument\n";
  return 1;
}

//=======================================================================
//function : Close
//purpose  : 
//=======================================================================

static Standard_Integer DDocStd_Close (Draw_Interpretor& /*theDI*/,
                                       Standard_Integer  theArgNb,
                                       const char**      theArgVec)
{   
  if (theArgNb != 2)
  {
    std::cout << "DDocStd_Close : Error\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  Standard_CString aDocName = theArgVec[1];
  if (!DDocStd::GetDocument (aDocName, aDoc))
  {
    return 1;
  }

  TDF_Label aRoot = aDoc->GetData()->Root();
  Handle(TPrsStd_AISViewer) aDocViewer;
  if (TPrsStd_AISViewer::Find (aRoot, aDocViewer)
   && !aDocViewer->GetInteractiveContext().IsNull())
  {
    Handle(V3d_Viewer) aViewer = aDocViewer->GetInteractiveContext()->CurrentViewer();
    V3d_ListOfView aViews;
    for (V3d_ListOfViewIterator aViewIter (aDocViewer->GetInteractiveContext()->CurrentViewer()->DefinedViewIterator()); aViewIter.More(); aViewIter.Next())
    {
      aViews.Append (aViewIter.Value());
    }
    for (V3d_ListOfViewIterator aViewIter (aViews); aViewIter.More(); aViewIter.Next())
    {
      Handle(V3d_View) aView = aViewIter.Value();
      ViewerTest::RemoveView (aView);
    }
  }

  Handle(TDocStd_Application) aDocApp = DDocStd::GetApplication();

  aDocApp->Close (aDoc);

  Handle(Draw_Drawable3D) aDrawable = Draw::Get (aDocName, Standard_False);
  dout.RemoveDrawable (aDrawable);
  Draw::Set (theArgVec[1], Handle(Draw_Drawable3D)());
  return 0;
}

//=======================================================================
//function : IsInSession
//purpose  : 
//=======================================================================

static Standard_Integer DDocStd_IsInSession (Draw_Interpretor& di,
					     Standard_Integer nb,
					     const char** a)
{   
  if (nb == 2) {   
    Handle(TDocStd_Application) A = DDocStd::GetApplication();
    di << A->IsInSession(a[1]);
    return 0;
  }  
  di << "DDocStd_IsInSession : Error\n";
  return 1;
}

 
//=======================================================================
//function : OSDPath
//purpose  : 
//=======================================================================

static Standard_Integer DDocStd_OSDPath (Draw_Interpretor& di,
                                         Standard_Integer nb,
                                         const char** a)
{   
  if (nb == 2) { 
    OSD_Path path (a[1]);
    di << "Node      : " << path.Node().ToCString() << "\n";  
    di << "UserName  : " << path.UserName().ToCString() << "\n"; 
    di << "Password  : " << path.Password().ToCString() << "\n";  
    di << "Disk      : " << path.Disk().ToCString() << "\n";
    di << "Trek      : " << path.Trek().ToCString() << "\n"; 
    di << "Name      : " << path.Name().ToCString() << "\n";  
    di << "Extension : " << path.Extension().ToCString() << "\n";
    return 0;
  }
  di << "DDocStd_OSDPath : Error\n";
  return 1;
}


//=======================================================================
//function : Path
//purpose  : 
//=======================================================================

static Standard_Integer DDocStd_Path (Draw_Interpretor& di,
				       Standard_Integer nb,
				       const char** a)
{   
  if (nb == 2) { 
    TDocStd_PathParser path (a[1]);
    //cout << "Trek      : " << path.Trek() << endl;  
    //cout << "Name      : " << path.Name() << endl; 
    //cout << "Extension : " << path.Extension() << endl;
    //cout << "Path      : " << path.Path() << endl;
    TCollection_AsciiString TrekAsciiString(path.Trek().ToExtString(),'?');
    TCollection_AsciiString NameAsciiString(path.Name().ToExtString(),'?');
    TCollection_AsciiString ExtensionAsciiString(path.Extension().ToExtString(),'?');
    TCollection_AsciiString PathAsciiString(path.Path().ToExtString(),'?');
    di << "Trek      : " << TrekAsciiString.ToCString() << "\n";  
    di << "Name      : " << NameAsciiString.ToCString() << "\n"; 
    di << "Extension : " << ExtensionAsciiString.ToCString() << "\n";
    di << "Path      : " << PathAsciiString.ToCString() << "\n";
    return 0;
  }
  di << "DDocStd_Path : Error\n";
  return 1;
}

//=======================================================================
//function : AddComment
//purpose  : 
//=======================================================================
static Standard_Integer DDocStd_AddComment (Draw_Interpretor& di,
                                            Standard_Integer nb,
                                            const char** a)
{  
  if (nb == 3) {
    Handle(TDocStd_Document) D;    
    if (!DDocStd::GetDocument(a[1],D)) return 1;  
    TCollection_ExtendedString comment (a[2]); 
//    Handle(TDocStd_Application) A = DDocStd::GetApplication();
//    A->AddComment(D,comment);
    D->AddComment(comment);
    return 0; 
  }
  di << "DDocStd_AddComment : Wrong arguments number\n";
  return 1;
}

//=======================================================================
//function : PrintComments
//purpose  : 
//=======================================================================
static Standard_Integer DDocStd_PrintComments (Draw_Interpretor& di,
                                               Standard_Integer nb,
                                               const char** a)
{  
  if (nb == 2) {
    Handle(TDocStd_Document) D;    
    if (!DDocStd::GetDocument(a[1],D)) return 1;  

    TColStd_SequenceOfExtendedString comments;
    D->Comments(comments);

    for (int i = 1; i <= comments.Length(); i++)
    {
      //cout << comments(i) << endl;
      TCollection_AsciiString commentAsciiString(comments(i).ToExtString(),'?');
      di << commentAsciiString.ToCString() << "\n";
    }

    return 0; 
  }
  di << "DDocStd_PrintComments : Wrong arguments number\n";
  return 1;
}

//=======================================================================
//function : SetStorageVerison
//purpose  : 
//=======================================================================
static Standard_Integer DDocStd_SetStorageVersion (Draw_Interpretor& ,
                                                   Standard_Integer nb,
                                                   const char** a)
{  
  if (nb == 2)
  {
    const int version = atoi(a[1]);
    XmlLDrivers::SetStorageVersion(version);
    return 0;
  }
  return 1;
}

//=======================================================================
//function : GetStorageVerison
//purpose  : 
//=======================================================================
static Standard_Integer DDocStd_GetStorageVersion (Draw_Interpretor& di,
                                                   Standard_Integer ,
                                                   const char** )
{  
  di << XmlLDrivers::StorageVersion() << "\n" ;
  return 0;
}

//=======================================================================
//function : ApplicationCommands
//purpose  : 
//=======================================================================

void DDocStd::ApplicationCommands(Draw_Interpretor& theCommands) 
{
  
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;

  const char* g = "DDocStd application commands";

  // user application commands
  theCommands.Add("ListDocuments",
		  "ListDocuments",
		  __FILE__, DDocStd_ListDocuments, g);  

  theCommands.Add("NewDocument",
		  "NewDocument docname format",
		  __FILE__, DDocStd_NewDocument, g);  

  theCommands.Add("Open",
		  "Open path docname [-stream]",
		  __FILE__, DDocStd_Open, g);   

  theCommands.Add("SaveAs",
		  "SaveAs DOC path [saveEmptyLabels: 0|1] [-stream]",
		  __FILE__, DDocStd_SaveAs, g);  

  theCommands.Add("Save",
		  "Save",
		  __FILE__, DDocStd_Save, g);  

  theCommands.Add("Close",
		  "Close DOC",
		  __FILE__, DDocStd_Close, g);   

  theCommands.Add("IsInSession",
		  "IsInSession path",
		  __FILE__, DDocStd_IsInSession, g);  

  theCommands.Add("OSDPath",
		  "OSDPath string",
		  __FILE__, DDocStd_OSDPath, g);  

  theCommands.Add("Path",
		  "Path string",
		  __FILE__, DDocStd_Path, g);

  theCommands.Add("AddComment",
		  "AddComment Doc string",
		  __FILE__, DDocStd_AddComment, g);

  theCommands.Add("PrintComments",
		  "PrintComments Doc",
		  __FILE__, DDocStd_PrintComments, g);

  theCommands.Add("GetStorageVersion",
		  "GetStorageVersion",
		  __FILE__, DDocStd_GetStorageVersion, g);
  theCommands.Add("SetStorageVersion",
		  "SetStorageVersion Version",
		  __FILE__, DDocStd_SetStorageVersion, g);
}
