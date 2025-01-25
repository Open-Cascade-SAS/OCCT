// Created:	Tue Nov  2 14:40:06 1999
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

#include <Express_Select.hxx>

#include <Express.hxx>
#include <Express_Alias.hxx>
#include <Express_ComplexType.hxx>
#include <Express_Entity.hxx>
#include <Express_Enum.hxx>
#include <Express_HSequenceOfItem.hxx>
#include <Express_Type.hxx>
#include <Message.hxx>
#include <OSD_Directory.hxx>
#include <OSD_FileSystem.hxx>
#include <OSD_Path.hxx>
#include <OSD_Protection.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Express_Select, Express_Item)

//=================================================================================================

Express_Select::Express_Select(const Standard_CString                         theName,
                               const Handle(TColStd_HSequenceOfHAsciiString)& theNames)
    : Express_Item(theName),
      myNames(theNames)
{
  myItems = new Express_HSequenceOfItem;
}

//=================================================================================================

const Handle(TColStd_HSequenceOfHAsciiString)& Express_Select::Names() const
{
  return myNames;
}

//=================================================================================================

const Handle(Express_HSequenceOfItem)& Express_Select::Items() const
{
  return myItems;
}

//=================================================================================================

Standard_Boolean Express_Select::GenerateClass() const
{
  const TCollection_AsciiString aCPPName = CPPName();

  Handle(TColStd_HSequenceOfInteger) aSeqMember   = new TColStd_HSequenceOfInteger;
  Handle(TColStd_HSequenceOfInteger) aSeqEntities = new TColStd_HSequenceOfInteger;
  for (Standard_Integer i = 1; i <= myItems->Length(); i++)
  {
    Handle(Express_Item) anItem = myItems->Value(i);
    if (anItem->IsKind(STANDARD_TYPE(Express_Entity))
        || anItem->IsKind(STANDARD_TYPE(Express_Select))
        || anItem->IsKind(STANDARD_TYPE(Express_Alias))
        || anItem->IsKind(STANDARD_TYPE(Express_ComplexType)))
    {
      aSeqEntities->Append(i);
    }
    else
    {
      aSeqMember->Append(i);
    }
  }
  Message::SendInfo() << "Generating SELECT " << aCPPName;
  if (!aSeqMember->IsEmpty())
  {
    Message::SendInfo() << "Generating SELECTMember " << aCPPName << "Member";
    generateSelectMember(aSeqMember);
  }
  // create a package directory (if not yet exist)
  OSD_Protection          aProt(OSD_RWXD, OSD_RWXD, OSD_RX, OSD_RX);
  TCollection_AsciiString aPack = GetPackageName();
  OSD_Path                aPath(aPack);
  OSD_Directory           aDir(aPath);
  aDir.Build(aProt);
  aPack += "/";
  aPack += aCPPName;
  const Handle(OSD_FileSystem)& aFileSystem = OSD_FileSystem::DefaultFileSystem();

  //===============================
  // Step 1: generating HXX
  {
    // Open HXX file
    std::shared_ptr<std::ostream> aStreamPtr =
      aFileSystem->OpenOStream(aPack.Cat(".hxx"), std::ios::out | std::ios::binary);
    Standard_OStream& anOS = *aStreamPtr;

    // write header
    Express::WriteFileStamp(anOS);

    // write start define
    anOS << "#ifndef _" << aCPPName
         << "_HeaderFile\n"
            "#define _"
         << aCPPName
         << "_HeaderFile\n"
            "\n";

    // write common includes
    anOS << "#include <Standard.hxx>\n"
            "#include <Standard_DefineAlloc.hxx>\n"
            "#include <Standard_Handle.hxx>\n"
            "#include <StepData_SelectType.hxx>\n"
            "#include <Standard_Integer.hxx>\n"
            "\n";

    anOS << "class Standard_Transient;\n";
    if (!aSeqMember->IsEmpty())
      anOS << "class StepData_SelectMember;\n";

    for (Standard_Integer i = 1; i <= myItems->Length(); i++)
    {
      Handle(Express_Item) anItem = myItems->Value(i);
      anItem->Use();
      if (anItem->IsKind(STANDARD_TYPE(Express_Alias)))
      {
        Handle(Express_Type) aType = Handle(Express_Alias)::DownCast(anItem)->Type();
        if (aType->IsStandard())
        {
          continue;
        }
      }
      anOS << "class " << anItem->CPPName() << ";\n";
    }
    anOS << "\n";

    // class declaration
    anOS << "//! Representation of STEP SELECT type " << Name()
         << "\n"
            "class "
         << aCPPName
         << " : public StepData_SelectType\n"
            "{\n"
            "\n"
            "public:\n"
            "\n"
            "  DEFINE_STANDARD_ALLOC\n"
            "\n";

    // default constructor
    anOS << "  //! Empty constructor\n"
            "  Standard_EXPORT "
         << aCPPName
         << "();\n"
            "\n";

    // write common methods section
    anOS << "  //! Recognizes a kind of " << Name() << " select type\n";
    for (Standard_Integer i = 1; i <= aSeqEntities->Length(); i++)
    {
      Standard_Integer anIdx = aSeqEntities->Value(i);
      anOS << "  //! -- " << i << " -> " << myNames->Value(anIdx)->String() << "\n";
    }
    anOS << "  Standard_EXPORT Standard_Integer CaseNum (const Handle(Standard_Transient)& theEnt) "
            "const Standard_OVERRIDE;\n"
            "\n";

    if (!aSeqMember->IsEmpty())
    {
      anOS << "  //! Recognizes items of select member " << Name() << "Member\n";
      for (Standard_Integer i = 1; i <= aSeqMember->Length(); i++)
      {
        Standard_Integer anIdx = aSeqMember->Value(i);
        anOS << "  //! -- " << i << " -> " << myNames->Value(anIdx)->String() << "\n";
      }
      anOS << "  //! -- 0 else\n"
              "  Standard_EXPORT virtual Standard_Integer CaseMem (const "
              "Handle(StepData_SelectMember)& theEnt) const Standard_OVERRIDE;\n"
              "\n"
              "  //! Returns a new select member the type "
           << Name()
           << "Member\n"
              "  Standard_EXPORT virtual Handle(StepData_SelectMember) NewMember() const "
              "Standard_OVERRIDE;\n"
              "\n";
    }

    // write methods get for entities
    for (Standard_Integer i = 1; i <= aSeqEntities->Length(); i++)
    {
      Standard_Integer               anIdx  = aSeqEntities->Value(i);
      Handle(Express_Item)           anItem = myItems->Value(anIdx);
      const TCollection_AsciiString& aName  = anItem->Name();
      anOS << "  //! Returns Value as " << aName
           << " (or Null if another type)\n"
              "  Standard_EXPORT Handle("
           << anItem->CPPName() << ") " << aName
           << "() const;\n"
              "\n";
    }

    // writes method set and get for enum , integer, real and string.
    for (Standard_Integer i = 1; i <= aSeqMember->Length(); i++)
    {
      Standard_Integer               anIdx         = aSeqMember->Value(i);
      Handle(Express_Item)           anItem        = myItems->Value(anIdx);
      const TCollection_AsciiString& aName         = anItem->Name();
      const TCollection_AsciiString& anItemCPPName = anItem->CPPName();
      anOS << "  //! Set Value for " << aName
           << "\n"
              "  Standard_EXPORT void Set"
           << aName << " (const " << anItemCPPName
           << " theVal);\n"
              "\n"
              "  //! Returns Value as "
           << aName
           << " (or Null if another type)\n"
              "  "
           << anItemCPPName << " " << aName
           << "() const;\n"
              "\n";
    }

    // write end
    anOS << "};\n"
            "#endif // _"
         << aCPPName << "_HeaderFile\n";
    aStreamPtr.reset();
  }
  //===============================
  // Step 2: generating CXX
  {
    // Open CXX file
    std::shared_ptr<std::ostream> aStreamPtr =
      aFileSystem->OpenOStream(aPack.Cat(".cxx"), std::ios::out | std::ios::binary);
    Standard_OStream& anOS = *aStreamPtr;

    // write header
    Express::WriteFileStamp(anOS);

    // write include section
    anOS << "#include <" << aCPPName << ".hxx>\n";
    if (!aSeqMember->IsEmpty())
    {
      anOS << "#include <" << aCPPName
           << "Member.hxx>\n"
              "#include <TCollection_HAsciiString.hxx>\n";
    }
    for (Standard_Integer i = 1; i <= aSeqEntities->Length(); i++)
    {
      Standard_Integer anIdx = aSeqEntities->Value(i);
      anOS << "#include <" << myItems->Value(anIdx)->CPPName() << ".hxx>\n";
    }

    // write constructor
    Express::WriteMethodStamp(anOS, aCPPName);
    anOS << aCPPName << "::" << aCPPName
         << "()\n"
            "{\n"
            "}\n";

    // write CaseNum method
    Express::WriteMethodStamp(anOS, "CaseNum");
    anOS << "Standard_Integer " << aCPPName
         << "::CaseNum (const Handle(Standard_Transient)& theEnt) const\n"
            "{\n";

    if (!aSeqEntities->IsEmpty())
    {
      anOS << "  if (theEnt.IsNull()) return 0;\n";
      for (Standard_Integer i = 1; i <= aSeqEntities->Length(); i++)
      {
        Standard_Integer anIdx = aSeqEntities->Value(i);
        anOS << "  if (theEnt->IsKind (STANDARD_TYPE(" << myItems->Value(anIdx)->CPPName()
             << "))) return " << i << ";\n";
      }
      anOS << "  return 0;\n"
              "}\n";
    }
    else
    {
      anOS << "  return 0;\n"
              "}\n";
    }

    if (!aSeqMember->IsEmpty())
    {
      // write CaseMem method
      Express::WriteMethodStamp(anOS, "CaseMem");
      anOS << "Standard_Integer " << aCPPName
           << "::CaseMem (const Handle(StepData_SelectMember)& theEnt) const\n"
              "{\n"
              "  if (theEnt.IsNull()) return 0;\n";
      for (int j = 1; j <= aSeqMember->Length(); j++)
      {
        Standard_Integer anIdx = aSeqMember->Value(j);
        if (j == 1)
        {
          anOS << "  if (theEnt->Matches (\"" << myNames->Value(anIdx)->String() << "\")) return "
               << j << ";\n";
        }
        else
        {
          anOS << "  else if (theEnt->Matches (\"" << myNames->Value(anIdx)->String()
               << "\")) return " << j << ";\n";
        }
      }
      anOS << "  else return 0;\n"
              "}\n";

      // write NewMember method
      Express::WriteMethodStamp(anOS, "NewMember");
      anOS << "Handle(StepData_SelectMember) " << aCPPName
           << "::NewMember() const\n"
              "{\n"
              "  return new "
           << aCPPName
           << "Member;\n"
              "}\n";
    }

    // write methods Get for entities.
    for (Standard_Integer i = 1; i <= aSeqEntities->Length(); i++)
    {
      Standard_Integer               anIdx         = aSeqEntities->Value(i);
      Handle(Express_Item)           anItem        = myItems->Value(anIdx);
      const TCollection_AsciiString& aName         = anItem->Name();
      const TCollection_AsciiString& anItemCPPName = anItem->CPPName();
      Express::WriteMethodStamp(anOS, aName);
      anOS << "Handle(" << anItemCPPName << ") " << aCPPName << "::" << aName
           << "() const\n"
              "{\n"
              "  return Handle("
           << anItemCPPName
           << ")::DownCast(Value());\n"
              "}\n";
    }

    // write methods Set and Get for Integer, Real, String and Enum
    for (Standard_Integer i = 1; i <= aSeqMember->Length(); i++)
    {
      Standard_Integer               anIdx         = aSeqMember->Value(i);
      Handle(Express_Item)           anItem        = myItems->Value(anIdx);
      const TCollection_AsciiString& aName         = anItem->Name();
      const TCollection_AsciiString& anItemCPPName = anItem->CPPName();
      TCollection_AsciiString        aStamp        = "Set";
      aStamp += aName;
      Express::WriteMethodStamp(anOS, aStamp);

      Standard_Boolean isString =
        (anItemCPPName.Location("HAsciiString", 1, anItemCPPName.Length()) > 0);
      TCollection_AsciiString aNameFunc;
      TCollection_AsciiString aFunc;
      Standard_Boolean        isEnum = anItem->IsKind(STANDARD_TYPE(Express_Enum));
      if (!isEnum)
      {
        if (isString)
        {
          aFunc += "String";
        }
        else
        {
          aNameFunc += anItemCPPName;
          aFunc                      = aNameFunc;
          Standard_Integer aSplitInd = aNameFunc.Location("_", 1, anItemCPPName.Length());
          if (aSplitInd)
          {
            aFunc = aNameFunc.Split(aSplitInd);
          }
        }
      }

      // write method set
      if (isString)
      {
        anOS << "void " << aCPPName << "::Set" << aName << " (const Handle(" << anItemCPPName
             << ") &theVal)\n";
      }
      else
      {
        anOS << "void " << aCPPName << "::Set" << aName << " (const " << anItemCPPName
             << " theVal)\n";
      }

      anOS << "{\n"
              "  Handle("
           << aCPPName << "Member) SelMem = Handle(" << aCPPName
           << "Member)::DownCast(Value());\n"
              "  if (SelMem.IsNull()) return;\n"
              "  Handle(TCollection_HAsciiString) aName = new TCollection_HAsciiString(\""
           << aName
           << "\");\n"
              "  SelMem->SetName (aName->ToCString());\n";
      if (isEnum)
      {
        anOS << "  SelMem->SetEnum ((Standard_Integer)theVal);\n";
      }
      else if (isString)
      {
        anOS << "  SelMem->Set" << aFunc << " (theVal->ToCString());\n";
      }
      else
      {
        anOS << "  SelMem->Set" << aFunc << " (theVal);\n";
      }
      anOS << "}\n";

      // write method get
      Express::WriteMethodStamp(anOS, aName);
      if (isString)
      {
        anOS << "Handle(" << anItemCPPName << ") " << aCPPName << "::" << aName << "() const\n";
      }
      else
      {
        anOS << anItemCPPName << " " << aCPPName << "::" << aName << "() const\n";
      }

      anOS << "{\n"
              "  Handle("
           << aCPPName << "Member) SelMem = Handle(" << aCPPName
           << "Member)::DownCast (Value());\n"
              "  if (SelMem.IsNull()) return 0;\n"
              "  Handle(TCollection_HAsciiString) aName = new TCollection_HAsciiString;\n"
              "  aName->AssignCat (SelMem->Name());\n"
              "  Handle(TCollection_HAsciiString) aNameItem = new TCollection_HAsciiString(\""
           << aName
           << "\");\n"
              "  if (aName->IsDifferent (aNameItem)) return 0;\n";
      if (isEnum)
      {
        anOS << "  Standard_Integer aNumIt = SelMem->Enum();\n"
                "  "
             << anItemCPPName
             << " aVal;\n"
                "  switch (aNumIt)\n"
                "  {\n";
        Handle(Express_Enum)                    anEnum    = Handle(Express_Enum)::DownCast(anItem);
        Handle(TColStd_HSequenceOfHAsciiString) anEnItems = anEnum->Names();
        TCollection_AsciiString                 aPrefix   = Express::EnumPrefix(aName);
        for (Standard_Integer k = 1; k <= anEnItems->Length(); k++)
        {
          anOS << "    case " << k << " : aVal = " << aName << "_" << aPrefix
               << anEnItems->Value(k)->String() << "; break;\n";
        }
        anOS << "    default : return 0; break;\n"
                "  }\n";
      }
      else if (isString)
      {
        anOS << "  Handle(TCollection_HAsciiString) aVal = new TCollection_HAsciiString;\n"
                "  aVal->AssignCat (SelMem->String());\n";
      }
      else
      {
        anOS << "  " << anItemCPPName << " aVal = SelMem->" << aFunc << "();\n";
      }

      anOS << "  return aVal;\n"
              "}\n";
    }
    aStreamPtr.reset();
  }

  return Standard_True;
}

//=================================================================================================

void Express_Select::PropagateUse() const
{
  const TCollection_AsciiString& aPack = GetPackageName();
  const TCollection_AsciiString& aName = Name();

  for (Standard_Integer i = 1; i <= myItems->Length(); i++)
  {
    Handle(Express_Item) anItem = myItems->Value(i);
    anItem->Use2(aName, aPack);
  }
}

//=================================================================================================

Standard_Boolean Express_Select::generateSelectMember(
  const Handle(TColStd_HSequenceOfInteger)& theSeqMember) const
{
  TCollection_AsciiString aCPPname = CPPName();
  aCPPname += "Member";

  // create a package directory (if not yet exist)
  OSD_Protection          aProt(OSD_RWXD, OSD_RWXD, OSD_RX, OSD_RX);
  TCollection_AsciiString aPack = GetPackageName();
  OSD_Path                aPath(aPack);
  OSD_Directory           aDir(aPath);
  aDir.Build(aProt);
  aPack += "/";
  aPack += aCPPname;
  const Handle(OSD_FileSystem)& aFileSystem = OSD_FileSystem::DefaultFileSystem();

  // Step 1: generating HXX
  {
    // Open HXX file
    std::shared_ptr<std::ostream> aStreamPtr =
      aFileSystem->OpenOStream(aPack.Cat(".hxx"), std::ios::out | std::ios::binary);
    Standard_OStream& anOS = *aStreamPtr;
    // write header
    Express::WriteFileStamp(anOS);

    // write start define
    anOS << "#ifndef _" << aCPPname
         << "_HeaderFile\n"
            "#define _"
         << aCPPname
         << "_HeaderFile\n"
            "\n";

    // includes
    anOS << "#include <Standard.hxx>\n"
            "#include <Standard_Type.hxx>\n"
            "#include <Standard_Boolean.hxx>\n"
            "#include <Standard_CString.hxx>\n"
            "#include <Standard_Integer.hxx>\n"
            "#include <StepData_SelectNamed.hxx>\n"
            "\n"
            "DEFINE_STANDARD_HANDLE("
         << aCPPname
         << ", StepData_SelectNamed)\n"
            "\n";

    // write start of declaration (inheritance)
    anOS << "  //! Representation of member for STEP SELECT type " << Name()
         << "\n"
            "class "
         << aCPPname
         << " : public StepData_SelectNamed\n"
            "{\n"
            "public:\n";

    // write methods
    anOS << "  //! Empty constructor\n"
            "  Standard_EXPORT "
         << aCPPname
         << "();\n"
            "\n"
            "  //! Returns True if has name\n"
            "  Standard_EXPORT virtual Standard_Boolean HasName() const Standard_OVERRIDE;\n"
            "\n"
            "  //! Returns name\n"
            "  Standard_EXPORT virtual Standard_CString Name() const Standard_OVERRIDE;\n"
            "\n"
            "  //! Set name\n"
            "  Standard_EXPORT virtual Standard_Boolean SetName(const Standard_CString name) "
            "Standard_OVERRIDE;\n"
            "\n"
            "  //! Tells if the name of a SelectMember matches a given one;\n"
            "  Standard_EXPORT virtual Standard_Boolean Matches (const Standard_CString name) "
            "const Standard_OVERRIDE;\n"
            "\n";

    // write fields
    anOS << "private:\n"
            "  Standard_Integer myCase;\n"
            "\n";
    // write end
    anOS << "};\n"
            "#endif // _"
         << aCPPname << "_HeaderFile\n";
    aStreamPtr.reset();
  }
  //===============================
  // Step 2: generating CXX
  {
    // Open CXX file
    std::shared_ptr<std::ostream> aStreamPtr =
      aFileSystem->OpenOStream(aPack.Cat(".cxx"), std::ios::out | std::ios::binary);
    Standard_OStream& anOS = *aStreamPtr;

    // write header
    Express::WriteFileStamp(anOS);

    // write include section
    anOS << "#include <" << aCPPname
         << ".hxx>\n"
            "#include <TCollection_HAsciiString.hxx>\n";
    // write constructor
    Express::WriteMethodStamp(anOS, aCPPname);
    anOS << aCPPname << "::" << aCPPname
         << "() : myCase(0) \n"
            "{\n"
            "}\n";

    // write method HasName
    Express::WriteMethodStamp(anOS, "HasName");
    anOS << "Standard_Boolean " << aCPPname
         << "::HasName() const\n"
            "{\n"
            "  return myCase > 0;\n"
            "}\n";

    Standard_Boolean hasEnum = Standard_False;
    // write method Name
    Express::WriteMethodStamp(anOS, "Name");
    anOS << "Standard_CString " << aCPPname
         << "::Name() const\n"
            "{\n"
            "  Handle(TCollection_HAsciiString) aName = new TCollection_HAsciiString;\n"
            "  switch (myCase)"
            "  {\n";
    for (Standard_Integer i = 1; i <= theSeqMember->Length(); i++)
    {
      Standard_Integer     anIdx  = theSeqMember->Value(i);
      Handle(Express_Item) anItem = myItems->Value(anIdx);
      if (anItem->IsKind(STANDARD_TYPE(Express_Enum)))
      {
        hasEnum = Standard_True;
      }
      anOS << "    case " << i << " : aName->AssignCat (\"" << myNames->Value(anIdx)->String()
           << "\"); break;\n";
    }
    anOS << "    default : break;\n"
            "  }\n"
            "  return aName->String();\n"
            "}\n";

    // write static method for compare name
    Express::WriteMethodStamp(anOS, "CompareNames");
    anOS << "static Standard_Integer CompareNames (const Standard_CString theName";
    if (hasEnum)
    {
      anOS << ", Standard_Integer &theNumEn)\n";
    }
    else
    {
      anOS << ")\n";
    }
    anOS << "{\n"
            "  Standard_Integer aCase = 0;\n"
            "  if (!theName || theName[0] == \'/0\') aCase = 0;\n";
    for (Standard_Integer i = 1; i <= theSeqMember->Length(); i++)
    {
      Standard_Integer     anIdx  = theSeqMember->Value(i);
      Handle(Express_Item) anItem = myItems->Value(anIdx);
      if (anItem->IsKind(STANDARD_TYPE(Express_Enum)))
      {
        Handle(Express_Enum) en = Handle(Express_Enum)::DownCast(anItem);
        for (Standard_Integer k = 1; k <= en->Names()->Length(); k++)
        {
          anOS << "  else if (!strcmp (theName, \"" << en->Names()->Value(k)->String()
               << "\"))\n"
                  "  {\n"
                  "    aCase = "
               << i
               << ";\n"
                  "    theNumEn = "
               << k
               << ";\n"
                  "  }\n";
        }
      }
      else
      {
        anOS << "  else if (!strcmp (theName, \"" << myNames->Value(anIdx)->String()
             << "\")) aCase = " << i << ";\n";
      }
    }
    anOS << "  return aCase;\n"
            "}\n";

    // write method SetName
    Express::WriteMethodStamp(anOS, "SetName");
    anOS << "Standard_Boolean " << aCPPname
         << "::SetName (const Standard_CString theName) \n"
            "{\n";
    if (hasEnum)
    {
      anOS << "  Standard_Integer aNumIt = 0;\n"
              "  myCase = CompareNames (theName, aNumIt);\n"
              "  if (aNumIt) SetInteger (aNumIt);\n";
    }
    else
    {
      anOS << "  myCase = CompareNames (theName);\n";
    }
    anOS << "  return (myCase > 0);\n"
            "}\n";

    // write method Matches
    Express::WriteMethodStamp(anOS, "Matches");
    anOS << "Standard_Boolean " << aCPPname
         << "::Matches (const Standard_CString theName) const\n"
            "{\n";
    if (hasEnum)
    {
      anOS << "  Standard_Integer aNumIt = 0;\n"
              "  Standard_Integer aCase = CompareNames (theName, aNumIt);\n";
    }
    else
    {
      anOS << "  Standard_Integer aCase = CompareNames (theName);\n";
    }
    anOS << "  return (aCase > 0);\n"
            "}\n";
    aStreamPtr.reset();
  }

  return Standard_True;
}
