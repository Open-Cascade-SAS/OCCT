// Created on: 2017-08-09
// Created by: Sergey NIKONOV
// Copyright (c) 2016 OPEN CASCADE SAS
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
#include <OSD_File.hxx>
#include <OSD_Protection.hxx>
#include <TDocStd_Document.hxx>
#include <TDF_Tool.hxx>
#include <XCAFDoc_AssemblyItemRef.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_Note.hxx>
#include <XCAFDoc_NoteBalloon.hxx>
#include <XCAFDoc_NoteComment.hxx>
#include <XCAFDoc_NoteBinData.hxx>
#include <XCAFDoc_NotesTool.hxx>
#include <XDEDRAW_Notes.hxx>

struct cmd
{
  const char*      name;
  Standard_Integer nargsreq;
  const char*      use;
};

Draw_Interpretor& operator << (Draw_Interpretor& di, const cmd& c)
{
  di << "Use: " << c.use << "\n";
  return di;
}

Draw_Interpretor& operator << (Draw_Interpretor& di, const TDF_Label& L)
{
  TCollection_AsciiString anEntry;
  TDF_Tool::Entry(L, anEntry);
  di << anEntry;
  return di;
}

Draw_Interpretor& operator << (Draw_Interpretor& di, const Handle(XCAFDoc_Note)& note)
{
  TCollection_AsciiString anEntry;
  TDF_Tool::Entry(note->Label(), anEntry);
  di << anEntry;
  return di;
}

Draw_Interpretor& operator << (Draw_Interpretor& di, const Handle(XCAFDoc_AssemblyItemRef)& ref)
{
  TCollection_AsciiString anEntry;
  TDF_Tool::Entry(ref->Label(), anEntry);
  di << anEntry;
  return di;
}

//=======================================================================
//function : noteCount
//purpose  : returns number of annotated items, notes and orphan notes
//=======================================================================
static const cmd XNoteCount = {
  "XNoteCount", 2, "XNoteCount Doc"
};
static Standard_Integer
noteCount(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  static const cmd& myCommand = XNoteCount;

  if (argc < myCommand.nargsreq)
  {
    di << myCommand;
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull())
  {
    return 1;
  }

  Handle(XCAFDoc_NotesTool) aNotesTool = XCAFDoc_DocumentTool::NotesTool(aDoc->Main());

  di
    << aNotesTool->NbAnnotatedItems() << " "
    << aNotesTool->NbNotes() << " "
    << aNotesTool->NbOrphanNotes()
    ;
  return 0;
}

//=======================================================================
//function : noteNotes
//purpose  : returns list of all notes
//=======================================================================
static const cmd XNoteNotes = {
  "XNoteNotes", 2, "XNoteNotes Doc"
};
static Standard_Integer
noteNotes(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  static const cmd& myCommand = XNoteNotes;

  if (argc < myCommand.nargsreq)
  {
    di << myCommand;
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull())
  {
    return 1;
  }

  Handle(XCAFDoc_NotesTool) aNotesTool = XCAFDoc_DocumentTool::NotesTool(aDoc->Main());

  TDF_LabelSequence aNotes;
  aNotesTool->GetNotes(aNotes);
  for (TDF_LabelSequence::Iterator anIt(aNotes); anIt.More(); anIt.Next())
  {
    Handle(XCAFDoc_Note) aNote = XCAFDoc_Note::Get(anIt.Value());
    di << aNote << " ";
  }

  return 0;
}

//=======================================================================
//function : noteAnnotations
//purpose  : returns list of all notes
//=======================================================================
static const cmd XNoteAnnotations = {
  "XNoteAnnotations", 2, "XNoteAnnotations Doc"
};
static Standard_Integer
noteAnnotations(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  static const cmd& myCommand = XNoteAnnotations;

  if (argc < myCommand.nargsreq)
  {
    di << myCommand;
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull())
  {
    return 1;
  }

  Handle(XCAFDoc_NotesTool) aNotesTool = XCAFDoc_DocumentTool::NotesTool(aDoc->Main());

  TDF_LabelSequence aAnnotations;
  aNotesTool->GetAnnotatedItems(aAnnotations);
  for (TDF_LabelSequence::Iterator anIt(aAnnotations); anIt.More(); anIt.Next())
  {
    Handle(XCAFDoc_AssemblyItemRef) aRef = XCAFDoc_AssemblyItemRef::Get(anIt.Value());
    di << aRef << " ";
  }

  return 0;
}

//=======================================================================
//function : noteCreateBalloon
//purpose  : creates a 'balloon' note and returns a new note entry
//=======================================================================
static const cmd XNoteCreateBalloon = {
  "XNoteCreateBalloon", 3, "XNoteCreateBalloon Doc comment [--user name] [--time stamp]"
};
static Standard_Integer
noteCreateBalloon(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  static const cmd& myCommand = XNoteCreateBalloon;

  if (argc < myCommand.nargsreq)
  {
    di << myCommand;
    return 1;
  }

  Standard_Integer iarg = 0;

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[++iarg], aDoc);
  if (aDoc.IsNull())
  {
    return 1;
  }

  TCollection_ExtendedString aComment = argv[++iarg];
  TCollection_ExtendedString aUsername, aTimestamp;

  for (++iarg; iarg < argc; ++iarg)
  {
    TCollection_AsciiString opt = argv[iarg];
    if (opt == "--user")
    {
      if (++iarg >= argc)
      {
        di << "Error: user name is expected.\n" << myCommand;
        return 1;
      }
      aUsername = argv[iarg];
    }
    else if (opt == "--time")
    {
      if (++iarg >= argc)
      {
        di << "Error: timestamp is expected.\n" << myCommand;
        return 1;
      }
      aTimestamp = argv[iarg];
    }
  }

  Handle(XCAFDoc_NotesTool) aNotesTool = XCAFDoc_DocumentTool::NotesTool(aDoc->Main());
  Handle(XCAFDoc_Note) aNote = aNotesTool->CreateBalloon(aUsername, aTimestamp, aComment);
  if (!aNote)
  {
    di << "Error: couldn't create a comment note.\n";
    return 1;
  }

  di << aNote;
  return 0;
}

//=======================================================================
//function : noteCreateComment
//purpose  : creates a comment note and returns a new note entry
//=======================================================================
static const cmd XNoteCreateComment = {
  "XNoteCreateComment", 3, "XNoteCreateComment Doc comment [--user name] [--time stamp]"
};
static Standard_Integer
noteCreateComment(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  static const cmd& myCommand = XNoteCreateComment;

  if (argc < myCommand.nargsreq)
  {
    di << myCommand;
    return 1;
  }

  Standard_Integer iarg = 0;

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[++iarg], aDoc);
  if (aDoc.IsNull()) 
  {
    return 1;
  }

  TCollection_ExtendedString aComment = argv[++iarg];
  TCollection_ExtendedString aUsername, aTimestamp;

  for (++iarg; iarg < argc; ++iarg)
  {
    TCollection_AsciiString opt = argv[iarg];
    if (opt == "--user")
    {
      if (++iarg >= argc)
      {
        di << "Error: user name is expected.\n" << myCommand;
        return 1;
      }
      aUsername = argv[iarg];
    }
    else if (opt == "--time")
    {
      if (++iarg >= argc)
      {
        di << "Error: timestamp is expected.\n" << myCommand;
        return 1;
      }
      aTimestamp = argv[iarg];
    }
  }

  Handle(XCAFDoc_NotesTool) aNotesTool = XCAFDoc_DocumentTool::NotesTool(aDoc->Main());
  Handle(XCAFDoc_Note) aNote = aNotesTool->CreateComment(aUsername, aTimestamp, aComment);
  if (!aNote)
  {
    di << "Error: couldn't create a comment note.\n";
    return 1;
  }

  di << aNote;
  return 0;
}

//=======================================================================
//function : noteCreateBinData
//purpose  : creates a binary data note and returns a new note entry
//=======================================================================
static const cmd XNoteCreateBinData = {
  "XNoteCreateBinData", 5, "XNoteCreateBinData Doc title <--file path | --data data> [--mime type] [--user name] [--time stamp]"
};
static Standard_Integer
noteCreateBinData(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  static const cmd& myCommand = XNoteCreateBinData;

  if (argc < myCommand.nargsreq)
  {
    di << myCommand;
    return 1;
  }

  Standard_Integer iarg = 0;

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[++iarg], aDoc);
  if (aDoc.IsNull())
  {
    return 1;
  }

  Standard_Boolean aFromFile = Standard_False;
  Standard_Boolean aFromData = Standard_False;
  TCollection_ExtendedString aFilename;
  Handle(TColStd_HArray1OfByte) aData;
  TCollection_ExtendedString aTitle = argv[++iarg];
  TCollection_AsciiString aMIMEtype;
  TCollection_ExtendedString aUsername, aTimestamp;

  for (++iarg; iarg < argc; ++iarg)
  {
    TCollection_AsciiString opt = argv[iarg];
    if (opt == "--file")
    {
      if (aFromData)
      {
        di << "Error: data can be taken either from a file or a data array.\n" << myCommand;
        return 1;
      }
      if (++iarg >= argc)
      {
        di << "Error: file path is expected.\n" << myCommand;
        return 1;
      }
      aFilename = argv[iarg];
      aFromFile = Standard_True;
    }
    else if (opt == "--data")
    {
      if (aFromFile)
      {
        di << "Error: data can be taken either from a file or a data array.\n" << myCommand;
        return 1;
      }
      if (++iarg >= argc)
      {
        di << "Error: data array is expected.\n" << myCommand;
        return 1;
      }
      Standard_SStream ss(std::ios_base::in | std::ios_base::out | std::ios_base::binary);
      ss << argv[iarg];
      Standard_Integer len = static_cast<Standard_Integer>(ss.tellp());
      aData = new TColStd_HArray1OfByte(1, len);
      for (Standard_Integer i = 1; i <= len && !ss.eof(); ++i)
      {
        ss >> aData->ChangeValue(i);
      }
      aFromData = Standard_True;
    }
    else if (opt == "--mime")
    {
      if (++iarg >= argc)
      {
        di << "Error: MIME type is expected.\n" << myCommand;
        return 1;
      }
      aMIMEtype = argv[iarg];
    }
    else if (opt == "--user")
    {
      if (++iarg >= argc)
      {
        di << "Error: user name is expected.\n" << myCommand;
        return 1;
      }
      aUsername = argv[iarg];
    }
    else if (opt == "--time")
    {
      if (++iarg >= argc)
      {
        di << "Error: timestamp is expected.\n" << myCommand;
        return 1;
      }
      aTimestamp = argv[iarg];
    }
  }

  Handle(XCAFDoc_NotesTool) aNotesTool = XCAFDoc_DocumentTool::NotesTool(aDoc->Main());

  Handle(XCAFDoc_Note) aNote;
  if (aFromFile)
  {
    OSD_Path aPath(aFilename);
    OSD_File aFile(aPath);
    aFile.Open(OSD_ReadOnly, OSD_Protection());
    aNote = aNotesTool->CreateBinData(aUsername, aTimestamp, aTitle, aMIMEtype, aFile);
  }
  else if (aFromData)
  {
    aNote = aNotesTool->CreateBinData(aUsername, aTimestamp, aTitle, aMIMEtype, aData);
  }
  else
  {
    di << "Error: data can be taken either from a file or a data array.\n" << myCommand;
    return 1;
  }

  if (!aNote)
  {
    di << "Error: couldn't create a binary data note.\n";
    return 1;
  }

  di << aNote;
  return 0;
}

//=======================================================================
//function : noteDelete
//purpose  : deletes a note by the entry
//=======================================================================
static const cmd XNoteDelete = {
  "XNoteDelete", 3, "XNoteDelete Doc note"
};
static Standard_Integer
noteDelete(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  static const cmd& myCommand = XNoteDelete;

  if (argc < myCommand.nargsreq)
  {
    di << myCommand;
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull())
  {
    return 1;
  }

  TCollection_ExtendedString anEntry = argv[2];

  TDF_Label aLabel;
  TDF_Tool::Label(aDoc->GetData(), anEntry, aLabel);
  if (aLabel.IsNull())
  {
    di << anEntry << ": invalid note entry.\n";
    return 1;
  }

  Handle(XCAFDoc_NotesTool) aNotesTool = XCAFDoc_DocumentTool::NotesTool(aDoc->Main());
  if (!aNotesTool->DeleteNote(aLabel))
  {
    di << "Error: couldn't delete note.\n";
    return 1;
  }

  return 0;
}

//=======================================================================
//function : noteDeleteAll
//purpose  : deletes all notes
//=======================================================================
static const cmd XNoteDeleteAll = {
  "XNoteDeleteAll", 2, "XNoteDeleteAll Doc"
};
static Standard_Integer
noteDeleteAll(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  static const cmd& myCommand = XNoteDeleteAll;

  if (argc < myCommand.nargsreq)
  {
    di << myCommand;
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull())
  {
    return 1;
  }

  Handle(XCAFDoc_NotesTool) aNotesTool = XCAFDoc_DocumentTool::NotesTool(aDoc->Main());
  Standard_Integer nbDeleted = aNotesTool->DeleteAllNotes();

  di << nbDeleted;
  return 0;
}

//=======================================================================
//function : noteDeleteOrphan
//purpose  : deletes all orphan notes
//=======================================================================
static const cmd XNoteDeleteOrphan = {
  "XNoteDeleteOrphan", 2, "XNoteDeleteOrphan Doc"
};
static Standard_Integer
noteDeleteOrphan(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  static const cmd& myCommand = XNoteDeleteOrphan;

  if (argc < myCommand.nargsreq)
  {
    di << myCommand;
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull())
  {
    return 1;
  }

  Handle(XCAFDoc_NotesTool) aNotesTool = XCAFDoc_DocumentTool::NotesTool(aDoc->Main());
  Standard_Integer nbDeleted = aNotesTool->DeleteOrphanNotes();

  di << nbDeleted;
  return 0;
}

//=======================================================================
//function : noteAdd
//purpose  : adds a note to a labeled item
//=======================================================================
static const cmd XNoteAdd = {
  "XNoteAdd", 4, "XNoteAdd Doc note item [--attr guid | --subshape num]"
};
static Standard_Integer
noteAdd(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  static const cmd& myCommand = XNoteAdd;

  if (argc < myCommand.nargsreq)
  {
    di << myCommand;
    return 1;
  }

  Standard_Integer iarg = 0;

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[++iarg], aDoc);
  if (aDoc.IsNull())
  {
    return 1;
  }

  TCollection_AsciiString aNoteEntry = argv[++iarg];
  TCollection_AsciiString anItemEntry = argv[++iarg];

  TDF_Label aNoteLabel;
  TDF_Tool::Label(aDoc->GetData(), aNoteEntry, aNoteLabel);
  if (aNoteLabel.IsNull())
  {
    di << aNoteEntry << ": invalid note entry.\n";
    return 1;
  }
  XCAFDoc_AssemblyItemId anItemId(anItemEntry);
  TDF_Label anItemLabel;
  TDF_Tool::Label(aDoc->GetData(), anItemId.GetPath().Last(), anItemLabel);
  if (anItemLabel.IsNull())
  {
    di << anItemId.ToString() << ": invalid item id.\n";
    return 1;
  }

  Standard_GUID aGUID;
  Standard_Integer aSubshape = 0;
  Standard_Boolean aHasGUID = Standard_False;
  Standard_Boolean aHasSubshape = Standard_False;
  for (++iarg; iarg < argc; ++iarg)
  {
    TCollection_AsciiString opt = argv[iarg];
    if (opt == "--attr")
    {
      if (aHasSubshape)
      {
        di << "Error: either attribute guid or subshape index must be specified.\n" << myCommand;
        return 1;
      }
      if (++iarg >= argc)
      {
        di << "Error: attribute guid is expected.\n" << myCommand;
        return 1;
      }
      TCollection_AsciiString arg = argv[iarg];
      aGUID = arg.ToCString();
      aHasGUID = Standard_True;
    }
    else if (opt == "--subshape")
    {
      if (++iarg >= argc)
      {
        di << "Error: subshape index is expected.\n" << myCommand;
        return 1;
      }
      TCollection_AsciiString arg = argv[iarg];
      if (!arg.IsIntegerValue())
      {
        di << "Error: subshape index must be integer value.\n" << myCommand;
        return 1;
      }
      aSubshape = arg.IntegerValue();
      aHasSubshape = Standard_True;
    }
  }

  Handle(XCAFDoc_NotesTool) aNotesTool = XCAFDoc_DocumentTool::NotesTool(aDoc->Main());

  Handle(XCAFDoc_AssemblyItemRef) aRef;
  if (aHasGUID)
  {
    aRef = aNotesTool->AddNoteToAttr(aNoteLabel, anItemId, aGUID);
  }
  else if (aHasSubshape)
  {
    aRef = aNotesTool->AddNoteToSubshape(aNoteLabel, anItemId, aSubshape);
  }
  else
  {
    aRef = aNotesTool->AddNote(aNoteLabel, anItemId);
  }

  if (!aRef)
  {
    di << "Error: couldn't add note.\n";
    return 1;
  }

  if (aRef->IsOrphan())
  {
    di << "Error: annotated item is invalid\n";
    return 1;
  }

  di << aRef;
  return 0;
}

//=======================================================================
//function : noteRemove
//purpose  : removes a note from a labeled item
//=======================================================================
static const cmd XNoteRemove = {
  "XNoteRemove", 4, "XNoteRemove Doc item note [--attr guid | --subshape num] [--del-orphan]"
};
static Standard_Integer
noteRemove(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  static const cmd& myCommand = XNoteRemove;

  if (argc < myCommand.nargsreq)
  {
    di << myCommand;
    return 1;
  }

  Standard_Integer iarg = 0;

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[++iarg], aDoc);
  if (aDoc.IsNull())
  {
    return 1;
  }

  TCollection_AsciiString anItemEntry = argv[++iarg];
  TCollection_AsciiString aNoteEntry = argv[++iarg];

  TDF_Label aNoteLabel;
  TDF_Tool::Label(aDoc->GetData(), aNoteEntry, aNoteLabel);
  if (aNoteLabel.IsNull())
  {
    di << aNoteEntry << ": invalid note entry.\n";
    return 1;
  }
  XCAFDoc_AssemblyItemId anItemId(anItemEntry);
  TDF_Label anItemLabel;
  TDF_Tool::Label(aDoc->GetData(), anItemId.GetPath().Last(), anItemLabel);
  if (anItemLabel.IsNull())
  {
    di << anItemId.ToString() << ": invalid item id.\n";
    return 1;
  }

  Standard_GUID aGUID;
  Standard_Integer aSubshape = 0;
  Standard_Boolean aHasGUID = Standard_False;
  Standard_Boolean aHasSubshape = Standard_False;
  Standard_Boolean aDelOrphan = Standard_False;
  for (++iarg; iarg < argc; ++iarg)
  {
    TCollection_AsciiString opt = argv[iarg];
    if (opt == "--attr")
    {
      if (aHasSubshape)
      {
        di << "Error: either attribute guid or subshape index must be specified.\n" << myCommand;
        return 1;
      }
      if (++iarg >= argc)
      {
        di << "Error: attribute guid is expected.\n" << myCommand;
        return 1;
      }
      TCollection_AsciiString arg = argv[iarg];
      aGUID = arg.ToCString();
      aHasGUID = Standard_True;
    }
    else if (opt == "--subshape")
    {
      if (aHasGUID)
      {
        di << "Error: either attribute guid or subshape index must be specified.\n" << myCommand;
        return 1;
      }
      if (++iarg >= argc)
      {
        di << "Error: subshape index is expected.\n" << myCommand;
        return 1;
      }
      TCollection_AsciiString arg = argv[iarg];
      if (!arg.IsIntegerValue())
      {
        di << "Error: subshape index must be integer value.\n" << myCommand;
        return 1;
      }
      aSubshape = arg.IntegerValue();
      aHasSubshape = Standard_True;
    }
    else if (opt == "--del-orphan")
    {
      aDelOrphan = Standard_True;
    }
  }

  Handle(XCAFDoc_NotesTool) aNotesTool = XCAFDoc_DocumentTool::NotesTool(aDoc->Main());

  if (aHasGUID)
  {
    di << aNotesTool->RemoveAttrNote(aNoteLabel, anItemId, aGUID, aDelOrphan);
  }
  else if (aHasSubshape)
  {
    di << aNotesTool->RemoveSubshapeNote(aNoteLabel, anItemId, aSubshape, aDelOrphan);
  }
  else
  {
    di << aNotesTool->RemoveNote(aNoteLabel, anItemId, aDelOrphan);
  }

  return 0;
}

//=======================================================================
//function : noteRemoveAll
//purpose  : removes all notes from a labeled item
//=======================================================================
static const cmd XNoteRemoveAll = {
  "XNoteRemoveAll", 3, "XNoteRemoveAll Doc item [--attr guid] [--subshape num] [--del-orphan]"
};
static Standard_Integer
noteRemoveAll(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  static const cmd& myCommand = XNoteRemoveAll;

  if (argc < myCommand.nargsreq)
  {
    di << myCommand;
    return 1;
  }

  Standard_Integer iarg = 0;

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[++iarg], aDoc);
  if (aDoc.IsNull())
  {
    return 1;
  }

  TCollection_AsciiString anItemEntry = argv[++iarg];

  XCAFDoc_AssemblyItemId anItemId(anItemEntry);
  TDF_Label anItemLabel;
  TDF_Tool::Label(aDoc->GetData(), anItemId.GetPath().Last(), anItemLabel);
  if (anItemLabel.IsNull())
  {
    di << anItemId.ToString() << ": invalid item id.\n";
    return 1;
  }

  Standard_GUID aGUID;
  Standard_Integer aSubshape = 0;
  Standard_Boolean aHasGUID = Standard_False;
  Standard_Boolean aHasSubshape = Standard_False;
  Standard_Boolean aDelOrphan = Standard_False;
  for (++iarg; iarg < argc; ++iarg)
  {
    TCollection_AsciiString opt = argv[iarg];
    if (opt == "--attr")
    {
      if (aHasSubshape)
      {
        di << "Error: either attribute guid or subshape index must be specified.\n" << myCommand;
        return 1;
      }
      if (++iarg >= argc)
      {
        di << "Error: attribute guid is expected.\n" << myCommand;
        return 1;
      }
      TCollection_AsciiString arg = argv[iarg];
      aGUID = arg.ToCString();
      aHasGUID = Standard_True;
    }
    else if (opt == "--subshape")
    {
      if (aHasGUID)
      {
        di << "Error: either attribute guid or subshape index must be specified.\n" << myCommand;
        return 1;
      }
      if (++iarg >= argc)
      {
        di << "Error: subshape index is expected.\n" << myCommand;
        return 1;
      }
      TCollection_AsciiString arg = argv[iarg];
      if (!arg.IsIntegerValue())
      {
        di << "Error: subshape index must be integer value.\n" << myCommand;
        return 1;
      }
      aSubshape = arg.IntegerValue();
      aHasSubshape = Standard_True;
    }
    else if (opt == "--del-orphan")
    {
      aDelOrphan = Standard_True;
    }
  }

  Handle(XCAFDoc_NotesTool) aNotesTool = XCAFDoc_DocumentTool::NotesTool(aDoc->Main());

  if (aHasGUID)
  {
    di << aNotesTool->RemoveAllAttrNotes(anItemId, aGUID, aDelOrphan);
  }
  else if (aHasSubshape)
  {
    di << aNotesTool->RemoveAllSubshapeNotes(anItemId, aSubshape, aDelOrphan);
  }
  else
  {
    di << aNotesTool->RemoveAllNotes(anItemId, aDelOrphan);
  }

  return 0;
}

//=======================================================================
//function : noteFindAnnotated
//purpose  : find a label of the annotated item
//=======================================================================
static const cmd XNoteFindAnnotated = {
  "XNoteFindAnnotated", 3, "XNoteFindAnnotated Doc item [--attr guid | --subshape num]"
};
static Standard_Integer
noteFindAnnotated(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  static const cmd& myCommand = XNoteFindAnnotated;

  if (argc < myCommand.nargsreq)
  {
    di << myCommand;
    return 1;
  }

  Standard_Integer iarg = 0;

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[++iarg], aDoc);
  if (aDoc.IsNull())
  {
    return 1;
  }

  TCollection_AsciiString anItemEntry = argv[++iarg];

  XCAFDoc_AssemblyItemId anItemId(anItemEntry);
  TDF_Label anItemLabel;
  TDF_Tool::Label(aDoc->GetData(), anItemId.GetPath().Last(), anItemLabel);
  if (anItemLabel.IsNull())
  {
    di << anItemId.ToString() << ": invalid item id.\n";
    return 1;
  }

  Standard_GUID aGUID;
  Standard_Integer aSubshape = 0;
  Standard_Boolean aHasGUID = Standard_False;
  Standard_Boolean aHasSubshape = Standard_False;
  for (++iarg; iarg < argc; ++iarg)
  {
    TCollection_AsciiString opt = argv[iarg];
    if (opt == "--attr")
    {
      if (aHasSubshape)
      {
        di << "Error: either attribute guid or subshape index must be specified.\n" << myCommand;
        return 1;
      }
      if (++iarg >= argc)
      {
        di << "Error: attribute guid is expected.\n" << myCommand;
        return 1;
      }
      TCollection_AsciiString arg = argv[iarg];
      aGUID = arg.ToCString();
      aHasGUID = Standard_True;
    }
    else if (opt == "--subshape")
    {
      if (++iarg >= argc)
      {
        di << "Error: subshape index is expected.\n" << myCommand;
        return 1;
      }
      TCollection_AsciiString arg = argv[iarg];
      if (!arg.IsIntegerValue())
      {
        di << "Error: subshape index must be integer value.\n" << myCommand;
        return 1;
      }
      aSubshape = arg.IntegerValue();
      aHasSubshape = Standard_True;
    }
  }

  Handle(XCAFDoc_NotesTool) aNotesTool = XCAFDoc_DocumentTool::NotesTool(aDoc->Main());

  if (aHasGUID)
  {
    di << aNotesTool->FindAnnotatedItemAttr(anItemId, aGUID);
  }
  else if (aHasSubshape)
  {
    di << aNotesTool->FindAnnotatedItemSubshape(anItemId, aSubshape);
  }
  else
  {
    di << aNotesTool->FindAnnotatedItem(anItemId);
  }

  return 0;
}

//=======================================================================
//function : noteGetNotes
//purpose  : get notes of the labeled item
//=======================================================================
static const cmd XNoteGetNotes = {
  "XNoteGetNotes", 3, "XNoteGetNotes Doc item [--attr guid | --subshape num]"
};
static Standard_Integer
noteGetNotes(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  static const cmd& myCommand = XNoteGetNotes;

  if (argc < myCommand.nargsreq)
  {
    di << myCommand;
    return 1;
  }

  Standard_Integer iarg = 0;

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[++iarg], aDoc);
  if (aDoc.IsNull())
  {
    return 1;
  }

  TCollection_AsciiString anItemEntry = argv[++iarg];

  XCAFDoc_AssemblyItemId anItemId(anItemEntry);
  TDF_Label anItemLabel;
  TDF_Tool::Label(aDoc->GetData(), anItemId.GetPath().Last(), anItemLabel);
  if (anItemLabel.IsNull())
  {
    di << anItemId.ToString() << ": invalid item id.\n";
    return 1;
  }

  Standard_GUID aGUID;
  Standard_Integer aSubshape = 0;
  Standard_Boolean aHasGUID = Standard_False;
  Standard_Boolean aHasSubshape = Standard_False;
  for (++iarg; iarg < argc; ++iarg)
  {
    TCollection_AsciiString opt = argv[iarg];
    if (opt == "--attr")
    {
      if (aHasSubshape)
      {
        di << "Error: either attribute guid or subshape index must be specified.\n" << myCommand;
        return 1;
      }
      if (++iarg >= argc)
      {
        di << "Error: attribute guid is expected.\n" << myCommand;
        return 1;
      }
      TCollection_AsciiString arg = argv[iarg];
      aGUID = arg.ToCString();
      aHasGUID = Standard_True;
    }
    else if (opt == "--subshape")
    {
      if (++iarg >= argc)
      {
        di << "Error: subshape index is expected.\n" << myCommand;
        return 1;
      }
      TCollection_AsciiString arg = argv[iarg];
      if (!arg.IsIntegerValue())
      {
        di << "Error: subshape index must be integer value.\n" << myCommand;
        return 1;
      }
      aSubshape = arg.IntegerValue();
      aHasSubshape = Standard_True;
    }
  }

  Handle(XCAFDoc_NotesTool) aNotesTool = XCAFDoc_DocumentTool::NotesTool(aDoc->Main());

  TDF_LabelSequence aNotes;
  if (aHasGUID)
  {
    aNotesTool->GetAttrNotes(anItemId, aGUID, aNotes);
  }
  else if (aHasSubshape)
  {
    aNotesTool->GetSubshapeNotes(anItemId, aSubshape, aNotes);
  }
  else
  {
    aNotesTool->GetNotes(anItemId, aNotes);
  }

  for (TDF_LabelSequence::Iterator anIt(aNotes); anIt.More(); anIt.Next())
  {
    di << anIt.Value() << " ";
  }

  return 0;
}

//=======================================================================
//function : noteUsername
//purpose  : gets a note username
//=======================================================================
static const cmd XNoteUsername = {
  "XNoteUsername", 3, "XNoteUsername Doc note"
};
static Standard_Integer
noteUsername(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  static const cmd& myCommand = XNoteUsername;

  if (argc < myCommand.nargsreq)
  {
    di << myCommand;
    return 1;
  }

  Standard_Integer iarg = 0;

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[++iarg], aDoc);
  if (aDoc.IsNull())
  {
    return 1;
  }

  TCollection_AsciiString aNoteEntry = argv[++iarg];

  TDF_Label aNoteLabel;
  TDF_Tool::Label(aDoc->GetData(), aNoteEntry, aNoteLabel);
  if (aNoteLabel.IsNull())
  {
    di << aNoteEntry << ": invalid note entry.\n";
    return 1;
  }

  Handle(XCAFDoc_Note) aNote = XCAFDoc_Note::Get(aNoteLabel);
  if (aNote.IsNull())
  {
    di << aNoteEntry << ": not a note entry.\n";
    return 1;
  }

  di << aNote->UserName();

  return 0;
}

//=======================================================================
//function : noteTimestamp
//purpose  : gets a note timestamp
//=======================================================================
static const cmd XNoteTimestamp = {
  "XNoteTimestamp", 3, "XNoteTimestamp Doc note"
};
static Standard_Integer
noteTimestamp(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  static const cmd& myCommand = XNoteTimestamp;

  if (argc < myCommand.nargsreq)
  {
    di << myCommand;
    return 1;
  }

  Standard_Integer iarg = 0;

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[++iarg], aDoc);
  if (aDoc.IsNull())
  {
    return 1;
  }

  TCollection_AsciiString aNoteEntry = argv[++iarg];

  TDF_Label aNoteLabel;
  TDF_Tool::Label(aDoc->GetData(), aNoteEntry, aNoteLabel);
  if (aNoteLabel.IsNull())
  {
    di << aNoteEntry << ": invalid note entry.\n";
    return 1;
  }

  Handle(XCAFDoc_Note) aNote = XCAFDoc_Note::Get(aNoteLabel);
  if (aNote.IsNull())
  {
    di << aNoteEntry << ": not a note entry.\n";
    return 1;
  }

  di << aNote->TimeStamp();

  return 0;
}

//=======================================================================
//function : noteDump
//purpose  : dump a note contents
//=======================================================================
static const cmd XNoteDump = {
  "XNoteDump", 3, "XNoteDump Doc note"
};
static Standard_Integer
noteDump(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  static const cmd& myCommand = XNoteDump;

  if (argc < myCommand.nargsreq)
  {
    di << myCommand;
    return 1;
  }

  Standard_Integer iarg = 0;

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[++iarg], aDoc);
  if (aDoc.IsNull())
  {
    return 1;
  }

  TCollection_AsciiString aNoteEntry = argv[++iarg];

  TDF_Label aNoteLabel;
  TDF_Tool::Label(aDoc->GetData(), aNoteEntry, aNoteLabel);
  if (aNoteLabel.IsNull())
  {
    di << aNoteEntry << ": invalid note entry.\n";
    return 1;
  }

  Handle(XCAFDoc_Note) aNote = XCAFDoc_Note::Get(aNoteLabel);
  if (aNote.IsNull())
  {
    di << aNoteEntry << ": not a note entry.\n";
    return 1;
  }

  di << "Username  : " << aNote->UserName() << "\n";
  di << "Timestamp : " << aNote->TimeStamp() << "\n";
  di << "Type      : " << aNote->get_type_name() << "\n";
  if (Handle(XCAFDoc_NoteComment) aComment = Handle(XCAFDoc_NoteComment)::DownCast(aNote))
  {
    di << "Comment   : " << aComment->Comment() << "\n";
  }
  else if (Handle(XCAFDoc_NoteBalloon) aBalloon = Handle(XCAFDoc_NoteBalloon)::DownCast(aNote))
  {
    di << "Comment   : " << aBalloon->Comment() << "\n";
  }
  else if (Handle(XCAFDoc_NoteBinData) aBinData = Handle(XCAFDoc_NoteBinData)::DownCast(aNote))
  {
    di << "Title     : " << aBinData->Title() << "\n";
    di << "MIME type : " << aBinData->MIMEtype() << "\n";
    di << "Size      : " << aBinData->Size() << "\n";
    static Standard_Integer theMaxLen = 64;
    const Handle(TColStd_HArray1OfByte)& aData = aBinData->Data();
    if (!aData.IsNull())
    {
      di << "Data      : ";
      Standard_Integer aLen = Min(aData->Length(), theMaxLen);
      for (Standard_Integer i = 1; i <= aLen; ++i)
      {
        Standard_SStream ss; ss << aData->Value(i);
        di << ss.str().c_str();
      }
      if (aData->Length() > theMaxLen)
        di << "  ...";
      di << "\n";
    }
  }

  return 0;
}

//=======================================================================
//function : noteIsRefOrphan
//purpose  : checks if a ref is orphan
//=======================================================================
static const cmd XNoteRefDump = {
  "XNoteRefDump", 3, "XNoteRefDump Doc ref"
};
static Standard_Integer
noteRefDump(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  static const cmd& myCommand = XNoteRefDump;

  if (argc < myCommand.nargsreq)
  {
    di << myCommand;
    return 1;
  }

  Standard_Integer iarg = 0;

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[++iarg], aDoc);
  if (aDoc.IsNull())
  {
    return 1;
  }

  TCollection_AsciiString aRefEntry = argv[++iarg];

  TDF_Label aRefLabel;
  TDF_Tool::Label(aDoc->GetData(), aRefEntry, aRefLabel);
  if (aRefLabel.IsNull())
  {
    di << aRefEntry << ": invalid reference entry.\n";
    return 1;
  }

  Handle(XCAFDoc_AssemblyItemRef) aRef = XCAFDoc_AssemblyItemRef::Get(aRefLabel);
  if (aRef.IsNull())
  {
    di << aRefEntry << ": not a reference entry.\n";
    return 1;
  }

  di << "Item      : " << aRef->GetItem().ToString() << "\n";
  if (aRef->HasExtraRef())
  {
    if (aRef->IsGUID())
    {
      Standard_SStream ss; 
      aRef->GetGUID().ShallowDump(ss);
      di << "Attribute : " << ss.str().c_str() << "\n";
    }
    else if (aRef->IsSubshapeIndex())
      di << "Subshape  : " << aRef->GetSubshapeIndex() << "\n";
  }
  di << "Orphan    : " << (aRef->IsOrphan() ? "yes" : "no") << "\n";

  return 0;
}

//=======================================================================
//function : noteIsRefOrphan
//purpose  : checks if a ref is orphan
//=======================================================================
static const cmd XNoteIsRefOrphan = {
  "XNoteIsRefOrphan", 3, "XNoteIsRefOrphan Doc ref"
};
static Standard_Integer
noteIsRefOrphan(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  static const cmd& myCommand = XNoteIsRefOrphan;

  if (argc < myCommand.nargsreq)
  {
    di << myCommand;
    return 1;
  }

  Standard_Integer iarg = 0;

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[++iarg], aDoc);
  if (aDoc.IsNull())
  {
    return 1;
  }

  TCollection_AsciiString aRefEntry = argv[++iarg];

  TDF_Label aRefLabel;
  TDF_Tool::Label(aDoc->GetData(), aRefEntry, aRefLabel);
  if (aRefLabel.IsNull())
  {
    di << aRefEntry << ": invalid reference entry.\n";
    return 1;
  }

  Handle(XCAFDoc_AssemblyItemRef) aRef = XCAFDoc_AssemblyItemRef::Get(aRefLabel);
  if (aRef.IsNull())
  {
    di << aRefEntry << ": not a reference entry.\n";
    return 1;
  }

  di << aRef->IsOrphan();

  return 0;
}

//=======================================================================
//function : InitCommands
//purpose  : 
//=======================================================================
void XDEDRAW_Notes::InitCommands(Draw_Interpretor& di) 
{
  static Standard_Boolean initialized = Standard_False;
  if (initialized)
  {
    return;
  }
  initialized = Standard_True;
  
  Standard_CString g = "XDE Notes commands";

  di.Add(XNoteCount.name, XNoteCount.use,
    __FILE__, noteCount, g);
  di.Add(XNoteNotes.name, XNoteNotes.use,
    __FILE__, noteNotes, g);
  di.Add(XNoteAnnotations.name, XNoteAnnotations.use,
    __FILE__, noteAnnotations, g);

  di.Add(XNoteCreateBalloon.name, XNoteCreateBalloon.use,
    __FILE__, noteCreateBalloon, g);
  di.Add(XNoteCreateComment.name, XNoteCreateComment.use,
    __FILE__, noteCreateComment, g);
  di.Add(XNoteCreateBinData.name, XNoteCreateBinData.use,
    __FILE__, noteCreateBinData, g);
  di.Add(XNoteDelete.name, XNoteDelete.use,
    __FILE__, noteDelete, g);
  di.Add(XNoteDeleteAll.name, XNoteDeleteAll.use,
    __FILE__, noteDeleteAll, g);
  di.Add(XNoteDeleteOrphan.name, XNoteDeleteOrphan.use,
    __FILE__, noteDeleteOrphan, g);

  di.Add(XNoteAdd.name, XNoteAdd.use,
    __FILE__, noteAdd, g);
  di.Add(XNoteRemove.name, XNoteRemove.use,
    __FILE__, noteRemove, g);
  di.Add(XNoteRemoveAll.name, XNoteRemoveAll.use,
    __FILE__, noteRemoveAll, g);

  di.Add(XNoteFindAnnotated.name, XNoteFindAnnotated.use,
    __FILE__, noteFindAnnotated, g);
  di.Add(XNoteGetNotes.name, XNoteGetNotes.use,
    __FILE__, noteGetNotes, g);

  di.Add(XNoteUsername.name, XNoteUsername.use,
    __FILE__, noteUsername, g);
  di.Add(XNoteTimestamp.name, XNoteTimestamp.use,
    __FILE__, noteTimestamp, g);
  di.Add(XNoteDump.name, XNoteDump.use,
    __FILE__, noteDump, g);

  di.Add(XNoteRefDump.name, XNoteRefDump.use,
    __FILE__, noteRefDump, g);
  di.Add(XNoteIsRefOrphan.name, XNoteIsRefOrphan.use,
    __FILE__, noteIsRefOrphan, g);
}
