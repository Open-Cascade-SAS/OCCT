// Created:	Mon Nov  1 12:50:27 1999
// Author:	Andrey BETENEV
// Copyright (c) 1999-2020 OPEN CASCADE SAS
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

#include <cstring>
#include <Express_Item.hxx>
#include <Express_Schema.hxx>
#include <Message.hxx>
#include <OSD_File.hxx>
#include <OSD_FileSystem.hxx>
#include <OSD_OpenFile.hxx>
#include <TCollection_HAsciiString.hxx>
#include "exptocas.tab.hxx"

//=======================================================================
// function : GetSchema
// purpose  : interface to parser
//=======================================================================
Handle(Express_Schema) GetSchema (const char* theFileName)
{
  std::ifstream aFileStream;
  OSD_OpenStream(aFileStream, theFileName, std::ios_base::in | std::ios_base::binary);
  exptocas::scanner aScanner(&aFileStream);
  aScanner.yyrestart(&aFileStream);
  // uncomment next string for debug of parser
  //aScanner.set_debug(1);
  exptocas::parser aParser(&aScanner);
  aParser.parse();
  return Express::Schema();
}

//=======================================================================
// function : LoadList
// purpose  : Load list of (class name, package name) from the file
//            Package names and optional mark flag are set to items in the schema
//=======================================================================
static Standard_Boolean LoadList (const char *theFileName,
                                  const Handle(Express_Schema)& theSchema,
                                  const Standard_Boolean theMark)
{
  const Handle(OSD_FileSystem)& aFileSystem = OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::istream> aStreamPtr = aFileSystem->OpenIStream (theFileName, std::ios::in);
  Standard_IStream& anIS = *aStreamPtr;

  if (aStreamPtr == nullptr)
  {
    Message::SendFail() << "Error: cannot open " << theFileName;
    return Standard_False;
  }

  Message::SendInfo() << "Loading " << theFileName << "...";
  char aBuf[512];
  Standard_Integer aLineNum = 0;
  // a line entry in file has the next format:
  // item_name package_name [shortname [check_flag(0 or 1) [fillshared_flag(0 or 1) [category]]]]
  while (anIS.getline (aBuf, 512))
  {
    char* aCurPos = aBuf;
    char* anItemName;
    char* aPackageName;
    char* aShortName;
    char* aCheckFlag;
    char* aFillSharedFlag;
    char* aCategoryName;
    Standard_Size aSepIdx = 0;
    aLineNum += 1;
    // -------------------------------------------------------------
    // first word is an item name
    // -------------------------------------------------------------
    // skip any whitespace character in the line
    while (*aCurPos && (aSepIdx = strcspn (aCurPos, " \t\r\n")) == 0)
    {
      aCurPos++;
    }
    // to next line if first word isn't found or comment started
    if (*aCurPos == '\0' || *aCurPos == '#')
    {
      continue;
    }
    // get the name of the item
    anItemName = aCurPos;
    // shift the position
    if (aCurPos[aSepIdx] == '\0')
    {
      aCurPos += aSepIdx;
    }
    else
    {
      aCurPos[aSepIdx] = '\0';
      aCurPos += aSepIdx + 1;
    }
    Handle(Express_Item) anItem = theSchema->Item (anItemName, Standard_True);
    // skip any class name not in schema
    if (anItem.IsNull())
    {
      continue;
    }
    // -------------------------------------------------------------
    // second word is a package name
    // -------------------------------------------------------------
    // skip any whitespace character in the rest of the line
    while (*aCurPos && (aSepIdx = strcspn (aCurPos, " \t\r\n")) == 0)
    {
      aCurPos++;
    }
    // to next line if second word isn't found or comment started
    if (*aCurPos == '\0' || *aCurPos == '#')
    {
      continue;
    }
    // get the name of the package
    aPackageName = aCurPos;
    // shift the position
    if (aCurPos[aSepIdx] == '\0')
    {
      aCurPos += aSepIdx;
    }
    else
    {
      aCurPos[aSepIdx] = '\0';
      aCurPos += aSepIdx + 1;
    }
    // make warning if there is another package for the item
    if (anItem->IsPackageNameSet() && anItem->GetPackageName().IsDifferent (aPackageName))
    {
      Message::SendWarning() << "Warning: Package is redefined for item " << anItemName;
    }
    anItem->SetPackageName (aPackageName);
    anItem->SetGenMode (theMark ? Express_Item::GM_GenByUser : Express_Item::GM_NoGen);
    // -------------------------------------------------------------
    // third word is an item short name (optional)
    // -------------------------------------------------------------
    // skip any whitespace character in the line
    while (*aCurPos && (aSepIdx = strcspn (aCurPos, " \t\r\n")) == 0)
    {
      aCurPos++;
    }
    // to next line if third word isn't found or comment started
    if (*aCurPos == '\0' || *aCurPos == '#')
    {
      continue;
    }
    // get the short name
    aShortName = aCurPos;
    // shift the position
    if (aCurPos[aSepIdx] == '\0')
    {
      aCurPos += aSepIdx;
    }
    else
    {
      aCurPos[aSepIdx] = '\0';
      aCurPos += aSepIdx + 1;
    }
    if (!std::isalpha (*aShortName))
    {
      Message::SendWarning() << "Warning: Not recognized a shortname at the line " << aLineNum;
      continue;
    }
    // if short name "-" then just do not set it
    if (*aShortName != '-')
    {
      Handle(TCollection_HAsciiString) anItemShortName = new TCollection_HAsciiString(aShortName);
      if (anItemShortName->Length() > 0)
      {
        anItem->SetShortName (anItemShortName);
      }
    }
    // -------------------------------------------------------------
    // fourth word is an item check flag (optional)
    // -------------------------------------------------------------
    // skip any whitespace character in the line
    while (*aCurPos && (aSepIdx = strcspn (aCurPos, " \t\r\n")) == 0)
    {
      aCurPos++;
    }
    // to next line if fourth word isn't found or comment started
    if (*aCurPos == '\0' || *aCurPos == '#')
    {
      continue;
    }
    // get the check flag
    aCheckFlag = aCurPos;
    // shift the position
    if (aCurPos[aSepIdx] == '\0')
    {
      aCurPos += aSepIdx;
    }
    else
    {
      aCurPos[aSepIdx] = '\0';
      aCurPos += aSepIdx + 1;
    }
    if (!(*aCheckFlag == '0' || *aCheckFlag == '1'))
    {
      Message::SendWarning() << "Warning: Not recognized a check flag at the line " << aLineNum;
      continue;
    }
    Standard_Boolean hasCheck = (*aCheckFlag == '0' ? Standard_False : Standard_True);
    anItem->SetCheckFlag (hasCheck);
    // -------------------------------------------------------------
    // fifth word is an item fill share flag (optional)
    // -------------------------------------------------------------
    // skip any whitespace character in the line
    while (*aCurPos && (aSepIdx = strcspn (aCurPos, " \t\r\n")) == 0)
    {
      aCurPos++;
    }
    // to next line if fifth word isn't found or comment started
    if (*aCurPos == '\0' || *aCurPos == '#')
    {
      continue;
    }
    // get the fill share flag
    aFillSharedFlag = aCurPos;
    // shift the position
    if (aCurPos[aSepIdx] == '\0')
    {
      aCurPos += aSepIdx;
    }
    else
    {
      aCurPos[aSepIdx] = '\0';
      aCurPos += aSepIdx + 1;
    }
    if (!(*aFillSharedFlag == '0' || *aFillSharedFlag == '1'))
    {
      Message::SendWarning() << "Warning: Not recognized a fill shared flag at the line " << aLineNum;
      continue;
    }
    Standard_Boolean hasFillShared = (*aFillSharedFlag == '0' ? Standard_False : Standard_True);
    anItem->SetFillSharedFlag (hasFillShared);
    // -------------------------------------------------------------
    // sixth word is an item category name (optional)
    // -------------------------------------------------------------
    // skip any whitespace character in the line
    while (*aCurPos && (aSepIdx = strcspn (aCurPos, " \t\r\n")) == 0)
    {
      aCurPos++;
    }
    // to next line if sixth word isn't found or comment started
    if (*aCurPos == '\0' || *aCurPos == '#')
    {
      continue;
    }
    // get the category name
    aCategoryName = aCurPos;
    aCurPos[aSepIdx] = '\0';
    if (!std::isalpha (*aCategoryName))
    {
      Message::SendWarning() << "Warning: Not recognized a category name at the line " << aLineNum;
      continue;
    }
    // if category name "-" then just do not set it
    if (*aCategoryName != '-')
    {
      Handle(TCollection_HAsciiString) anItemCategoryName = new TCollection_HAsciiString(aCategoryName);
      if (anItemCategoryName->Length() > 0)
      {
        anItem->SetShortName (anItemCategoryName);
      }
    }
  }
  Message::SendInfo() << " Done";

  return Standard_True;
}

//=======================================================================
// function : main
// purpose  :
//=======================================================================
int main (int argc, char* argv[])
{
  if (argc < 2)
  {
    Message::SendInfo() << "EXPRESS -> CASCADE/XSTEP classes generator 3.0\n"
                           "Use: ExpToCas <schema.exp> [<create.lst> [<packaging.lst> [start_index]]]\n"
                           "Where: \n"
                           "- schema.exp is a file with EXPRESS schema \n"
                           "- create.lst is a file with list of types to generate (all if none)\n"
                           "  (or \"-\" for creating all entities in the schema)\n"
                           "- packaging.lst is a file with classes distribution per package\n"
                           "  in the form of the list (one item per line) \"<TypeName> <Package>\"\n"
                           "  If package not defined for some type, \"StepStep\" assumed\n"
                           "- start_index - a first number for auxiliary generated files with data\n"
                           "  to copy into StepAP214_Protocol.cxx, RWStepAP214_GeneralModule.cxx and\n"
                           "  RWStepAP214_ReadWriteModule.cxx";
    return 0;
  }

  //=================================
  // Step 1: parsing EXPRESS file
  // open schema file
  OSD_Path aPath (argv[1]);
  OSD_File aFile (aPath);
  if (!aFile.IsReadable())
  {
    Message::SendFail() << "Error: Cannot open " << argv[1];
    return -1;
  }

  // parse
  Message::SendInfo() << "Starting parsing " << argv[1];
  Handle(Express_Schema) aSchema = GetSchema (argv[1]);

  if (aSchema.IsNull())
  {
    Message::SendFail() << "Error: Parsing finished with no result";
    return -1;
  }
  else
  {
    Message::SendInfo() << "Schema " << aSchema->Name()->ToCString() << " successfully parsed";
  }
  Message::SendInfo() << "Total " << aSchema->NbItems() << " items";

  //=================================
  // Step 2: Prepare data for creating classes

  // load packaging information
  if (argc > 3)
  {
    if (!LoadList (argv[3], aSchema, Standard_False))
    {
      return -1;
    }
  }
  // load list of classes to generate
  if (argc > 2)
  {
    if (argv[2][0] == '-')
    {
      // set mark for all items
      for (Standard_Integer num = 1; num <= aSchema->NbItems(); num++)
      {
        aSchema->Item (num)->SetGenMode (Express_Item::GM_GenByUser);
      }
    }
    else if (!LoadList (argv[2], aSchema, Standard_True))
    {
      return -1;
    }
  }

  // get starting index
  Standard_Integer anIndex = -1;
  if (argc > 4)
  {
    char* aStopSymbol;
    anIndex = (Standard_Integer) strtol (argv[4], &aStopSymbol, 10);
    if (*aStopSymbol != '\0')
    {
      Message::SendFail() << "Error: invalid starting index: " << argv[4];
      return -1;
    }
  }

  //=================================
  // Step 3: Iterate over new items and set the package name if need
  const TCollection_AsciiString aUserName("user");
  for (Standard_Integer aNum = 1; aNum <= aSchema->NbItems(); aNum++)
  {
    if (aSchema->Item (aNum)->GetGenMode() == Express_Item::GM_GenByUser)
    {
      aSchema->Item (aNum)->Use2(aUserName, Express_Item::GetUnknownPackageName());
    }
  }

  //=================================
  // Step 4: Iterate by items and generate classes
  Standard_Boolean isDone = Standard_False;
  Express_Item::SetIndex (anIndex);
  do
  {
    isDone = Standard_False;
    for (Standard_Integer aNum = 1; aNum <= aSchema->NbItems(); aNum++)
    {
      isDone = isDone || aSchema->Item (aNum)->Generate();
    }
  } while (isDone);

  return 0;
}
