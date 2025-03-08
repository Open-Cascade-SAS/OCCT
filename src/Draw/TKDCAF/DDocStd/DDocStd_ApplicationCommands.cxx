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
#include <Draw_Interpretor.hxx>
#include <Draw_Viewer.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <DDocStd_DrawDocument.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TDataStd_Name.hxx>
#include <Draw.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDF_Data.hxx>
#include <TDF_ChildIterator.hxx>
#include <PCDM_ReaderFilter.hxx>

#include <OSD_FileSystem.hxx>
#include <TDocStd_PathParser.hxx>

#include <AIS_InteractiveContext.hxx>
#include <TPrsStd_AISViewer.hxx>
#include <ViewerTest.hxx>
#include <V3d_Viewer.hxx>

#ifndef _WIN32
extern Draw_Viewer dout;
#else
Standard_IMPORT Draw_Viewer dout;
#endif

//=================================================================================================

static Standard_Integer DDocStd_ListDocuments(Draw_Interpretor& di,
                                              Standard_Integer  nb,
                                              const char** /*a*/)
{
  if (nb == 1)
  {
    Handle(TDocStd_Application) A = DDocStd::GetApplication();
    Handle(TDocStd_Document)    D;
    Standard_Integer            nbdoc = A->NbDocuments();
    for (Standard_Integer i = 1; i <= nbdoc; i++)
    {
      A->GetDocument(i, D);
      di << "document " << i;
      if (D->IsSaved())
      {
        di << " name : " << D->GetName();
        di << " path : " << D->GetPath();
      }
      else
        di << " not saved";
      di << "\n";
    }
    return 0;
  }
  di << "DDocStd_ListDocuments : Error\n";
  return 1;
}

//=================================================================================================

static Standard_Integer DDocStd_NewDocument(Draw_Interpretor& di,
                                            Standard_Integer  nb,
                                            const char**      a)
{
  Handle(TDocStd_Document)     D;
  Handle(DDocStd_DrawDocument) DD;
  if (nb == 2)
  {
    if (!DDocStd::GetDocument(a[1], D, Standard_False))
    {
      D  = new TDocStd_Document("dummy");
      DD = new DDocStd_DrawDocument(D);
      Draw::Set(a[1], DD);
      di << "document (not handled by application)  " << a[1] << " created\n";
      DDocStd::ReturnLabel(di, D->Main());
    }
    else
      di << a[1] << " is already a document\n";
    return 0;
  }
  if (nb == 3)
  {
    if (!DDocStd::GetDocument(a[1], D, Standard_False))
    {
      Handle(TDocStd_Application) A = DDocStd::GetApplication();
      A->NewDocument(a[2], D);
      DD = new DDocStd_DrawDocument(D);
      TDataStd_Name::Set(D->GetData()->Root(), a[1]);
      Draw::Set(a[1], DD);
      di << "document " << a[1] << " created\n";
      DDocStd::ReturnLabel(di, D->Main());
    }
    else
      di << a[1] << " is already a document\n";
    return 0;
  }
  di << "DDocStd_NewDocument : Error\n";
  return 1;
}

//=================================================================================================

static Standard_Integer DDocStd_Open(Draw_Interpretor& di, Standard_Integer nb, const char** a)
{
  if (nb >= 3)
  {
    TCollection_ExtendedString  path(a[1], Standard_True);
    Standard_CString            DocName = a[2];
    Handle(TDocStd_Application) A       = DDocStd::GetApplication();
    Handle(TDocStd_Document)    D;
    PCDM_ReaderStatus           theStatus;

    Standard_Boolean          anUseStream = Standard_False;
    Handle(PCDM_ReaderFilter) aFilter     = new PCDM_ReaderFilter;
    for (Standard_Integer i = 3; i < nb; i++)
    {
      TCollection_AsciiString anArg(a[i]);
      if (anArg == "-append")
      {
        aFilter->Mode() = PCDM_ReaderFilter::AppendMode_Protect;
      }
      else if (anArg == "-overwrite")
      {
        aFilter->Mode() = PCDM_ReaderFilter::AppendMode_Overwrite;
      }
      else if (anArg == "-stream")
      {
        di << "standard SEEKABLE stream is used\n";
        anUseStream = Standard_True;
      }
      else if (anArg.StartsWith("-skip"))
      {
        TCollection_AsciiString anAttrType = anArg.SubString(6, anArg.Length());
        aFilter->AddSkipped(anAttrType);
      }
      else if (anArg.StartsWith("-read"))
      {
        TCollection_AsciiString aValue = anArg.SubString(6, anArg.Length());
        if (aValue.Value(1) == '0') // path
        {
          aFilter->AddPath(aValue);
        }
        else // attribute to read
        {
          aFilter->AddRead(aValue);
        }
      }
    }

    if (aFilter->IsAppendMode() && !DDocStd::GetDocument(DocName, D, Standard_False))
    {
      di << "for append mode document " << DocName << " must be already created\n";
      return 1;
    }
    Handle(Draw_ProgressIndicator) aProgress = new Draw_ProgressIndicator(di, 1);
    if (anUseStream)
    {
      const Handle(OSD_FileSystem)& aFileSystem = OSD_FileSystem::DefaultFileSystem();
      std::shared_ptr<std::istream> aFileStream =
        aFileSystem->OpenIStream(path, std::ios::in | std::ios::binary);

      theStatus = A->Open(*aFileStream, D, aFilter, aProgress->Start());
    }
    else
    {
      theStatus = A->Open(path, D, aFilter, aProgress->Start());
    }
    if (theStatus == PCDM_RS_OK && !D.IsNull())
    {
      if (!aFilter->IsAppendMode())
      {
        Handle(DDocStd_DrawDocument) DD = new DDocStd_DrawDocument(D);
        TDataStd_Name::Set(D->GetData()->Root(), DocName);
        Draw::Set(DocName, DD);
      }
      return 0;
    }
    else
    {
      switch (theStatus)
      {
        case PCDM_RS_UserBreak: {
          di << " could not retrieve , user break \n";
          break;
        }
        case PCDM_RS_AlreadyRetrieved:
        case PCDM_RS_AlreadyRetrievedAndModified: {
          di << " already retrieved \n";
          break;
        }
        case PCDM_RS_NoDriver: {
          di << " could not retrieve , no Driver to make it \n";
          break;
        }
        case PCDM_RS_UnknownDocument:
        case PCDM_RS_NoModel: {
          di << " could not retrieve , Unknown Document or No Model \n";
          break;
        }
        case PCDM_RS_TypeNotFoundInSchema:
        case PCDM_RS_UnrecognizedFileFormat: {
          di << " could not retrieve , Type not found or Unrecognized File Format\n";
          break;
        }
        case PCDM_RS_PermissionDenied: {
          di << " could not retrieve , permission denied \n";
          break;
        }
        default:
          di << " could not retrieve \n";
          break;
      }
      di << "DDocStd_Open : Error\n";
    }
  }
  return 1;
}

//=================================================================================================

static Standard_Integer DDocStd_Save(Draw_Interpretor& di, Standard_Integer nb, const char** a)
{
  if (nb == 2)
  {
    Handle(TDocStd_Document) D;
    if (!DDocStd::GetDocument(a[1], D))
      return 1;
    Handle(TDocStd_Application) A = DDocStd::GetApplication();
    if (!D->IsSaved())
    {
      di << "this document has never been saved\n";
      return 0;
    }

    Handle(Draw_ProgressIndicator) aProgress = new Draw_ProgressIndicator(di, 1);
    A->Save(D, aProgress->Start());
    return 0;
  }
  di << "DDocStd_Save : Error\n";
  return 1;
}

//=================================================================================================

static Standard_Integer DDocStd_SaveAs(Draw_Interpretor& di, Standard_Integer nb, const char** a)
{
  if (nb >= 3)
  {
    Handle(TDocStd_Document) D;
    if (!DDocStd::GetDocument(a[1], D))
      return 1;
    TCollection_ExtendedString  path(a[2], Standard_True);
    Handle(TDocStd_Application) A = DDocStd::GetApplication();
    PCDM_StoreStatus            theStatus;

    Standard_Boolean anUseStream(Standard_False), isSaveEmptyLabels(Standard_False);
    for (Standard_Integer i = 3; i < nb; i++)
    {
      if (!strcmp(a[i], "-stream"))
      {
        di << "standard SEEKABLE stream is used\n";
        anUseStream = Standard_True;
        break;
      }
      else
      {
        isSaveEmptyLabels = ((atoi(a[3])) != 0);
        D->SetEmptyLabelsSavingMode(isSaveEmptyLabels);
      }
    }

    Handle(Draw_ProgressIndicator) aProgress = new Draw_ProgressIndicator(di, 1);
    if (anUseStream)
    {
      const Handle(OSD_FileSystem)& aFileSystem = OSD_FileSystem::DefaultFileSystem();
      std::shared_ptr<std::ostream> aFileStream =
        aFileSystem->OpenOStream(path, std::ios::out | std::ios::binary);
      theStatus = A->SaveAs(D, *aFileStream, aProgress->Start());
    }
    else
    {
      theStatus = A->SaveAs(D, path, aProgress->Start());
    }

    if (theStatus != PCDM_SS_OK)
    {
      switch (theStatus)
      {
        case PCDM_SS_DriverFailure: {
          di << "Error saving document: Could not store , no driver found to make it\n";
          break;
        }
        case PCDM_SS_WriteFailure: {
          di << "Error saving document: Write access failure\n";
          break;
        }
        case PCDM_SS_Failure: {
          di << "Error saving document: Write failure\n";
          break;
        }
        case PCDM_SS_Doc_IsNull: {
          di << "Error saving document: No document to save\n";
          break;
        }
        case PCDM_SS_No_Obj: {
          di << "Error saving document: No objects written\n";
          break;
        }
        case PCDM_SS_Info_Section_Error: {
          di << "Error saving document: Write info section failure\n";
          break;
        }
        case PCDM_SS_UserBreak: {
          di << "Error saving document: User break \n";
          break;
        }
        default:
          break;
      }
      return 1;
    }
    else
    {
      return 0;
    }
  }
  di << "DDocStd_SaveAs : Error not enough argument\n";
  return 1;
}

//=================================================================================================

static Standard_Integer DDocStd_Close(Draw_Interpretor& theDI,
                                      Standard_Integer  theArgNb,
                                      const char**      theArgVec)
{
  bool                                      toComplain = true;
  NCollection_List<TCollection_AsciiString> aDocNames;
  for (Standard_Integer anArgIt = 1; anArgIt < theArgNb; ++anArgIt)
  {
    const TCollection_AsciiString anArg(theArgVec[anArgIt]);
    TCollection_AsciiString       anArgCase(anArg);
    anArgCase.LowerCase();
    if (anArgCase == "*" || anArgCase == "-all" || anArgCase == "all")
    {
      for (NCollection_Map<Handle(Draw_Drawable3D)>::Iterator anIter(Draw::Drawables());
           anIter.More();
           anIter.Next())
      {
        if (Handle(DDocStd_DrawDocument) aDrawDocument =
              Handle(DDocStd_DrawDocument)::DownCast(anIter.Value()))
        {
          aDocNames.Append(aDrawDocument->Name());
        }
      }
      if (aDocNames.IsEmpty())
      {
        return 0;
      }
    }
    else if (anArgCase == "-silent")
    {
      toComplain = false;
    }
    else
    {
      aDocNames.Append(anArg);
    }
  }

  if (aDocNames.IsEmpty())
  {
    theDI << "Syntax error: wrong number of arguments";
    return 1;
  }

  Handle(TDocStd_Application) aDocApp = DDocStd::GetApplication();
  for (NCollection_List<TCollection_AsciiString>::Iterator aDocNameIter(aDocNames);
       aDocNameIter.More();
       aDocNameIter.Next())
  {
    Standard_CString         aDocName = aDocNameIter.Value().ToCString();
    Handle(TDocStd_Document) aDoc;
    if (DDocStd::GetDocument(aDocName, aDoc, toComplain))
    {
      TDF_Label                 aRoot = aDoc->GetData()->Root();
      Handle(TPrsStd_AISViewer) aDocViewer;
      if (TPrsStd_AISViewer::Find(aRoot, aDocViewer)
          && !aDocViewer->GetInteractiveContext().IsNull())
      {
        Handle(V3d_Viewer) aViewer = aDocViewer->GetInteractiveContext()->CurrentViewer();
        V3d_ListOfView     aViews;
        for (V3d_ListOfViewIterator aViewIter(
               aDocViewer->GetInteractiveContext()->CurrentViewer()->DefinedViewIterator());
             aViewIter.More();
             aViewIter.Next())
        {
          aViews.Append(aViewIter.Value());
        }
        for (V3d_ListOfViewIterator aViewIter(aViews); aViewIter.More(); aViewIter.Next())
        {
          Handle(V3d_View) aView = aViewIter.Value();
          ViewerTest::RemoveView(aView);
        }
      }
      aDocApp->Close(aDoc);
    }
    else if (toComplain)
    {
      return 1;
    }

    if (Handle(Draw_Drawable3D) aDrawable = Draw::GetExisting(aDocName))
    {
      dout.RemoveDrawable(aDrawable);
    }
    Draw::Set(aDocName, Handle(Draw_Drawable3D)());
  }
  return 0;
}

//=================================================================================================

static Standard_Integer DDocStd_IsInSession(Draw_Interpretor& di,
                                            Standard_Integer  nb,
                                            const char**      a)
{
  if (nb == 2)
  {
    Handle(TDocStd_Application) A = DDocStd::GetApplication();
    di << A->IsInSession(a[1]);
    return 0;
  }
  di << "DDocStd_IsInSession : Error\n";
  return 1;
}

//=================================================================================================

static Standard_Integer DDocStd_OSDPath(Draw_Interpretor& di, Standard_Integer nb, const char** a)
{
  if (nb == 2)
  {
    OSD_Path path(a[1]);
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

//=================================================================================================

static Standard_Integer DDocStd_Path(Draw_Interpretor& di, Standard_Integer nb, const char** a)
{
  if (nb == 2)
  {
    TDocStd_PathParser path(TCollection_ExtendedString(a[1], Standard_True));
    di << "Trek      : " << path.Trek() << "\n";
    di << "Name      : " << path.Name() << "\n";
    di << "Extension : " << path.Extension() << "\n";
    di << "Path      : " << path.Path() << "\n";
    return 0;
  }
  di << "DDocStd_Path : Error\n";
  return 1;
}

//=================================================================================================

static Standard_Integer DDocStd_AddComment(Draw_Interpretor& di,
                                           Standard_Integer  nb,
                                           const char**      a)
{
  if (nb == 3)
  {
    Handle(TDocStd_Document) D;
    if (!DDocStd::GetDocument(a[1], D))
      return 1;
    TCollection_ExtendedString comment(a[2], Standard_True);
    //    Handle(TDocStd_Application) A = DDocStd::GetApplication();
    //    A->AddComment(D,comment);
    D->AddComment(comment);
    return 0;
  }
  di << "DDocStd_AddComment : Wrong arguments number\n";
  return 1;
}

//=================================================================================================

static Standard_Integer DDocStd_PrintComments(Draw_Interpretor& di,
                                              Standard_Integer  nb,
                                              const char**      a)
{
  if (nb == 2)
  {
    Handle(TDocStd_Document) D;
    if (!DDocStd::GetDocument(a[1], D))
      return 1;

    TColStd_SequenceOfExtendedString comments;
    D->Comments(comments);

    for (int i = 1; i <= comments.Length(); i++)
    {
      di << comments(i) << "\n";
    }

    return 0;
  }
  di << "DDocStd_PrintComments : Wrong arguments number\n";
  return 1;
}

//=================================================================================================

static Standard_Integer DDocStd_StorageFormatVersion(Draw_Interpretor& theDI,
                                                     Standard_Integer  theNbArgs,
                                                     const char**      theArgVec)
{
  if (theNbArgs != 2 && theNbArgs != 3)
  {
    theDI << "Syntax error: wrong number of arguments";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!DDocStd::GetDocument(theArgVec[1], aDoc))
  {
    theDI << "Syntax error: " << theArgVec[1] << " is not a document";
    return 1;
  }

  if (theNbArgs == 2)
  {
    theDI << aDoc->StorageFormatVersion() << "\n";
    return 0;
  }

  Standard_Integer aVerInt = 0;
  if (!Draw::ParseInteger(theArgVec[2], aVerInt) || aVerInt < TDocStd_FormatVersion_LOWER
      || aVerInt > TDocStd_FormatVersion_UPPER)
  {
    theDI << "Syntax error: unknown version '" << theArgVec[2] << "' (valid range is "
          << TDocStd_FormatVersion_LOWER << ".." << TDocStd_FormatVersion_UPPER << ")";
    return 1;
  }

  aDoc->ChangeStorageFormatVersion((TDocStd_FormatVersion)aVerInt);
  return 0;
}

//=================================================================================================

void DDocStd::ApplicationCommands(Draw_Interpretor& theCommands)
{

  static Standard_Boolean done = Standard_False;
  if (done)
    return;
  done = Standard_True;

  const char* g = "DDocStd application commands";

  // user application commands
  theCommands.Add("ListDocuments", "ListDocuments", __FILE__, DDocStd_ListDocuments, g);

  theCommands.Add("NewDocument", "NewDocument docname format", __FILE__, DDocStd_NewDocument, g);

  theCommands.Add(
    "Open",
    "Open path docname [-stream] [-skipAttribute] [-readAttribute] [-readPath] [-append|-overwrite]"
    "\n\t\t The options are:"
    "\n\t\t   -stream : opens path as a stream"
    "\n\t\t   -skipAttribute : class name of the attribute to skip during open, for example "
    "-skipTDF_Reference"
    "\n\t\t   -readAttribute : class name of the attribute to read only during open, for example "
    "-readTDataStd_Name loads only such attributes"
    "\n\t\t   -append : to read file into already existing document once again, append new "
    "attributes and don't touch existing"
    "\n\t\t   -overwrite : to read file into already existing document once again, overwriting "
    "existing attributes",
    __FILE__,
    DDocStd_Open,
    g);

  theCommands.Add("SaveAs",
                  "SaveAs DOC path [saveEmptyLabels: 0|1] [-stream]",
                  __FILE__,
                  DDocStd_SaveAs,
                  g);

  theCommands.Add("Save", "Save", __FILE__, DDocStd_Save, g);

  theCommands.Add(
    "Close",
    "Close the specific document or all documents\n"
    "Close DOC [-silent]"
    "\n\t\t: Close the specific document."
    "\n\t\t: -silent not raise an exception or print error message for an empty document \n"
    "Close *"
    "\n\t\t: Close all open documents.",
    __FILE__,
    DDocStd_Close,
    g);

  theCommands.Add("IsInSession", "IsInSession path", __FILE__, DDocStd_IsInSession, g);

  theCommands.Add("OSDPath", "OSDPath string", __FILE__, DDocStd_OSDPath, g);

  theCommands.Add("Path", "Path string", __FILE__, DDocStd_Path, g);

  theCommands.Add("AddComment", "AddComment Doc string", __FILE__, DDocStd_AddComment, g);

  theCommands.Add("PrintComments", "PrintComments Doc", __FILE__, DDocStd_PrintComments, g);

  static const TCollection_AsciiString THE_SET_VER_HELP =
    TCollection_AsciiString()
    + "StorageFormatVersion Doc [Version]"
      "\n\t\t: Print or set storage format version within range "
    + int(TDocStd_FormatVersion_LOWER) + ".." + int(TDocStd_FormatVersion_UPPER)
    + "\n\t\t: defined by TDocStd_FormatVersion enumeration.";
  theCommands.Add("StorageFormatVersion",
                  THE_SET_VER_HELP.ToCString(),
                  __FILE__,
                  DDocStd_StorageFormatVersion,
                  g);
  theCommands.Add("GetStorageFormatVersion",
                  "GetStorageFormatVersion Doc"
                  "\n\t\t: Alias to StorageFormatVersion",
                  __FILE__,
                  DDocStd_StorageFormatVersion,
                  g);
  theCommands.Add("SetStorageFormatVersion",
                  "\n\t\t: Alias to StorageFormatVersion",
                  __FILE__,
                  DDocStd_StorageFormatVersion,
                  g);
}
