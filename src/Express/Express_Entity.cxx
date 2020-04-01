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

#include <Express_Entity.hxx>

#include <Express.hxx>
#include <Express_Alias.hxx>
#include <Express_Boolean.hxx>
#include <Express_ComplexType.hxx>
#include <Express_Enum.hxx>
#include <Express_Field.hxx>
#include <Express_HSequenceOfEntity.hxx>
#include <Express_HSequenceOfField.hxx>
#include <Express_Integer.hxx>
#include <Express_Logical.hxx>
#include <Express_NamedType.hxx>
#include <Express_Number.hxx>
#include <Express_Real.hxx>
#include <Express_Select.hxx>
#include <Express_String.hxx>
#include <Express_Type.hxx>
#include <Message.hxx>
#include <OSD_Directory.hxx>
#include <OSD_FileSystem.hxx>
#include <OSD_Path.hxx>
#include <OSD_Protection.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Express_Entity, Express_Item)

//=======================================================================
// function : Express_Entity
// purpose  :
//=======================================================================

Express_Entity::Express_Entity (const Standard_CString theName,
                                const Handle(TColStd_HSequenceOfHAsciiString)& theInherit,
                                const Handle(Express_HSequenceOfField)& theFields)
: Express_Item (theName), mySupers (theInherit), myFields (theFields)
{
  // make empty lists to avoid checking every time
  myInherit = new Express_HSequenceOfEntity;
  if (mySupers.IsNull())
  {
    mySupers = new TColStd_HSequenceOfHAsciiString;
  }
  if (myFields.IsNull())
  {
    myFields = new Express_HSequenceOfField;
  }
  myIsAbstract = Standard_False;
}

//=======================================================================
// function : SuperTypes
// purpose  :
//=======================================================================

const Handle(TColStd_HSequenceOfHAsciiString)& Express_Entity::SuperTypes() const
{
  return mySupers;
}

//=======================================================================
// function : Inherit
// purpose  :
//=======================================================================

const Handle(Express_HSequenceOfEntity)& Express_Entity::Inherit() const
{
  return myInherit;
}

//=======================================================================
// function : Fields
// purpose  :
//=======================================================================

const Handle(Express_HSequenceOfField)& Express_Entity::Fields() const
{
  return myFields;
}
//=======================================================================
// function : NbFields
// purpose  :
//=======================================================================

Standard_Integer Express_Entity::NbFields (const Standard_Boolean theInherited) const
{
  Standard_Integer aNb = myFields->Length();
  if (theInherited)
  {
    for (Standard_Integer i = 1; i <= myInherit->Length(); i++)
    {
      aNb += myInherit->Value (i)->NbFields (theInherited);
    }
  }
  return aNb;
}

//=======================================================================
// function : WriteGetMethod
// purpose  :
//=======================================================================

static void writeGetMethod (
        Standard_OStream& theOS,
        const TCollection_AsciiString& theName,
        const TCollection_AsciiString& theField,
        const TCollection_AsciiString& theType,
        const Standard_Boolean isHandle,
        const Standard_Boolean /*isSimple*/)
{
  const TCollection_AsciiString& aMethod = theField;
  Express::WriteMethodStamp (theOS, aMethod);
  theOS << (isHandle ? "Handle(" : "") << theType << (isHandle ? ") " : " ") << theName << "::" << aMethod << "() const\n"
           "{\n"
           "  return my" << theField << ";\n"
           "}\n";
}

//=======================================================================
// function : WriteSetMethod
// purpose  :
//=======================================================================

static void writeSetMethod (
        Standard_OStream& theOS,
        const TCollection_AsciiString& theName,
        const TCollection_AsciiString& theField,
        const TCollection_AsciiString& theType,
        const Standard_Boolean isHandle,
        const Standard_Boolean isSimple)
{
  TCollection_AsciiString aMethod = "Set";
  aMethod += theField;
  Express::WriteMethodStamp (theOS, aMethod);
  theOS << "void " << theName << "::" << aMethod << " (const " << (isHandle ? "Handle(" : "") << theType << (isHandle ? ")" : "") <<
           (isSimple ? "" : "&") << " the" << theField << ")\n"
           "{\n"
           "  my" << theField << " = the" << theField << ";\n"
           "}\n";
}

//=======================================================================
// function : WriteSpaces
// purpose  :
//=======================================================================

static inline void writeSpaces (Standard_OStream& theOS, Standard_Integer theNum)
{
  for (Standard_Integer i = 0; i < theNum; i++)
    theOS << " ";
}

//=======================================================================
// function : GenerateClass
// purpose  :
//=======================================================================

Standard_Boolean Express_Entity::GenerateClass() const
{
  const TCollection_AsciiString aCPPName = CPPName();
  Message::SendInfo() << "Generating ENTITY " << aCPPName;

  // create a package directory (if not yet exist)
  OSD_Protection aProt (OSD_RWXD, OSD_RWXD, OSD_RX, OSD_RX);
  TCollection_AsciiString aPack = GetPackageName();
  OSD_Path aPath (aPack);
  OSD_Directory aDir (aPath);
  aDir.Build (aProt);
  aPack += "/";
  aPack += aCPPName;
  const Handle(OSD_FileSystem)& aFileSystem = OSD_FileSystem::DefaultFileSystem();

  //===============================
  // Step 1: generating HXX
  TCollection_AsciiString anInheritName;
  {
    // Open HXX file
    std::shared_ptr<std::ostream> aStreamPtr = aFileSystem->OpenOStream (aPack.Cat (".hxx"), std::ios::out | std::ios::binary);
    Standard_OStream& anOS = *aStreamPtr;

    // write header
    Express::WriteFileStamp (anOS);

    // write include protection and "standard" includes
    anOS << "#ifndef _" << aCPPName << "_HeaderFile_\n"
            "#define _" << aCPPName << "_HeaderFile_\n"
            "\n"
            "#include <Standard.hxx>\n"
            "#include <Standard_Type.hxx>\n";
    if (myInherit->Length() < 1)
    {
      anInheritName = "Standard_Transient";
      anOS << "#include <" << anInheritName << ".hxx>\n";
    }
    else
    {
      // first inherited class will be actually inherited
      Handle(Express_Entity) anEntity = myInherit->Value (1);
      anInheritName = anEntity->CPPName();
      // make warning if there are more than one inherits
      if (myInherit->Length() > 1)
      {
        Message::SendWarning() << "Warning: ENTITY " << Name() << " defined with multiple inheritance;";
        Message::SendWarning() << "Warning: only first base class " << anInheritName << " is actually inherited, others are made fields";
      }
    }
    // write other includes (inherits and used types)
    writeIncludes (anOS);

    // write HANDLE MACRO
    anOS << "\n"
            "DEFINE_STANDARD_HANDLE(" << aCPPName << ", " << anInheritName << ")\n"
            "\n";

    // write start of declaration (inheritance)
    anOS << "//! Representation of STEP entity " << Name() << "\n"
            "class " << aCPPName << " : public " << anInheritName << "\n"
            "{\n"
            "\n"
            "public :\n"
            "\n";

    // write constructor
    anOS << "  //! default constructor\n"
            "  Standard_EXPORT " << aCPPName << "();\n"
            "\n";

    // write Init methods
    if (myInherit->Length() > 1 || myFields->Length() > 0)
    {
      anOS << "  //! Initialize all fields (own and inherited)\n"
              "  Standard_EXPORT void Init (";
      makeInit (anOS, 29, Standard_True, 0);
      anOS << ");\n"
              "\n";
    }

    // write methods Get/Set
    for (Standard_Integer i = 2; i <= myInherit->Length(); i++)
    {
      Handle(Express_Entity) anEntity = myInherit->Value (i);
      const TCollection_AsciiString& aName = anEntity->Name();
      const TCollection_AsciiString& anEntityCPPName = anEntity->CPPName();
      anOS << "  //! Returns data for supertype " << aName << "\n"
              "  Standard_EXPORT Handle(" << anEntityCPPName << ") " << aName << "() const;\n"
              "  //! Sets data for supertype " << aName << "\n"
              "  Standard_EXPORT void Set" << aName << " (const Handle(" << anEntityCPPName << ")& the" << aName << ");\n"
              "\n";
    }
    for (Standard_Integer i = 1; i <= myFields->Length(); i++)
    {
      Handle(Express_Field) aField = myFields->Value (i);
      Handle(Express_Type) aFieldType = aField->Type();
      TCollection_AsciiString aFieldCPPName = aFieldType->CPPName();
      anOS << "  //! Returns field " << aField->Name() << "\n";
      if (aFieldType->IsHandle())
      {
        anOS << "  Standard_EXPORT Handle(" << aFieldCPPName << ") " << aField->Name() << "() const;\n";
      }
      else
      {
        anOS << "  Standard_EXPORT " << aFieldCPPName << " " << aField->Name() << "() const;\n";
      }
      anOS << "\n"
              "  //! Sets field " << aField->Name() << "\n";
      if (aFieldType->IsHandle())
      {
        anOS << "  Standard_EXPORT void Set" << aField->Name() << " (const Handle("
             << aFieldCPPName << ")& the" << aField->Name() << ");\n";
      }
      else
      {
        anOS << "  Standard_EXPORT void Set" << aField->Name() << " (const "
             << aFieldCPPName << (aFieldType->IsSimple() ? "" : "&")
             << " the" << aField->Name() << ");\n";
      }
      if (aField->IsOptional())
      {
        anOS << "\n"
                "  //! Returns True if optional field " << aField->Name() << " is defined\n"
                "  Standard_EXPORT Standard_Boolean Has" << aField->Name() << "() const;\n";
      }
      anOS << "\n";

      // prepare additional methods for HArray1 field (NbField() and <field_elem> FieldValue(int num))
      Standard_Integer aFindPos = aFieldCPPName.Search ("_HArray");
      if (aFindPos > -1)
      {
        TCollection_AsciiString aNamePack = aFieldCPPName.SubString (1, aFindPos);
        TCollection_AsciiString aNameClass;
        if (aNamePack.IsEqual ("TColStd_"))
        {
          Standard_Integer aFindPos2 = aFieldCPPName.Search ("Integer");
          if (aFindPos2 > -1)
          {
            aNameClass = "Standard_Integer";
          }
          else
          {
            aFindPos2 = aFieldCPPName.Search ("Real");
            if (aFindPos2 > -1)
            {
              aNameClass = "Standard_Real";
            }
            else
            {
              aNameClass = "Standard_Boolean";
            }
          }
        }
        else
        {
          if (aNamePack.IsEqual ("Interface_"))
          {
            aNameClass = "Handle(TCollection_HAsciiString)";
          }
          else
          {
            aNameClass = "Handle(";
            aNameClass += aNamePack;
            aNameClass += aFieldCPPName.SubString (aFindPos + 10, aFieldCPPName.Length());
            aNameClass += ")";
          }
        }
        anOS << "  //! Returns number of " << aField->Name() << "\n"
                "  Standard_EXPORT Standard_Integer Nb" << aField->Name() << "() const;\n"
                "\n"
                "  //! Returns value of " << aField->Name() << " by its num\n"
                "  Standard_EXPORT " << aNameClass << " " << aField->Name() << "Value (const Standard_Integer theNum) const;\n"
                "\n";
      }
    }

    anOS << "  DEFINE_STANDARD_RTTIEXT(" << aCPPName << ", " << anInheritName << ")\n"
            "\n";

    // write fields section
    if (myInherit->Length() > 1 || myFields->Length() > 0)
    {
      anOS << "private:\n"
              "\n";
      for (Standard_Integer i = 2; i <= myInherit->Length(); i++)
      {
        Handle(Express_Entity) anEntity = myInherit->Value (i);
        anOS << "  Handle(" << anEntity->CPPName() << ") my" << anEntity->Name() << "; //!< supertype\n";
      }
      for (Standard_Integer i = 1; i <= myFields->Length(); i++)
      {
        Handle(Express_Field) aField = myFields->Value (i);
        Handle(Express_Type) aFieldType = aField->Type();
        if (aFieldType->IsHandle())
        {
          anOS << "  Handle(" << aFieldType->CPPName() << ") my" << aField->Name() << ";";
        }
        else
        {
          anOS << "  " << aFieldType->CPPName() << " my" << aField->Name() << ";";
        }
        if (aField->IsOptional())
        {
          anOS << " //!< optional";
        }
        anOS << "\n";
      }
      // optional fields: flag 'is field set'
      for (Standard_Integer i = 1; i <= myFields->Length(); i++)
      {
        Handle(Express_Field) aField = myFields->Value (i);
        if (!aField->IsOptional())
        {
          continue;
        }
        anOS << "  Standard_Boolean myHas" << aField->Name() << "; //!< flag \"is "
             << aField->Name() << " defined\"\n";
      }
      anOS << "\n";
    }

    // write end
    anOS << "};\n"
            "\n"
            "#endif // _" << aCPPName << "_HeaderFile_\n";
    aStreamPtr.reset();
  }
  //===============================
  // Step 2: generating CXX
  {
    // Open CXX file
    std::shared_ptr<std::ostream> aStreamPtr = aFileSystem->OpenOStream (aPack.Cat (".cxx"), std::ios::out | std::ios::binary);
    Standard_OStream& anOS = *aStreamPtr;

    // write header
    Express::WriteFileStamp (anOS);

    // write include section
    anOS << "#include <" << aCPPName << ".hxx>\n"
            "\n"
            "IMPLEMENT_STANDARD_RTTIEXT(" << aCPPName << ", " << anInheritName << ")\n";

    // write constructor
    Express::WriteMethodStamp (anOS, aCPPName);
    anOS << aCPPName << "::" << aCPPName << "()\n"
            "{\n";
    for (Standard_Integer i = 1; i <= myFields->Length(); i++)
    {
      Handle(Express_Field) aField = myFields->Value (i);
      if (aField->IsOptional())
        anOS << "  myHas" << aField->Name() << " = Standard_False;\n";
    }
    anOS << "}\n";

    // write method Init()
    if (myInherit->Length() > 1 || myFields->Length() > 0)
    {
      Express::WriteMethodStamp (anOS, "Init");
      anOS << "void " << aCPPName << "::Init (";
      makeInit (anOS, 13 + aCPPName.Length(), Standard_True, 1);
      anOS << ")\n"
              "{";
      makeInit (anOS, -2, Standard_True, 3);
      anOS << "\n"
              "}\n";
    }

    // write "methods" section
    for (Standard_Integer i = 2; i <= myInherit->Length(); i++)
    {
      Handle(Express_Entity) anEntity = myInherit->Value (i);
      const TCollection_AsciiString& anEntityCPPName = anEntity->CPPName();
      writeGetMethod (anOS, aCPPName, anEntity->Name(), anEntityCPPName, Standard_True, Standard_False);
      writeSetMethod (anOS, aCPPName, anEntity->Name(), anEntityCPPName, Standard_True, Standard_False);
    }
    for (Standard_Integer i = 1; i <= myFields->Length(); i++)
    {
      Handle(Express_Field) aField = myFields->Value (i);
      Handle(Express_Type) aFieldType = aField->Type();
      TCollection_AsciiString aFieldCPPName = aFieldType->CPPName();
      writeGetMethod (anOS, aCPPName, aField->Name(), aFieldCPPName, aFieldType->IsHandle(), aFieldType->IsSimple());
      writeSetMethod (anOS, aCPPName, aField->Name(), aFieldCPPName, aFieldType->IsHandle(), aFieldType->IsSimple());
      if (aField->IsOptional())
      {
        TCollection_AsciiString aMethod = "Has";
        aMethod += aField->Name();
        Express::WriteMethodStamp (anOS, aMethod);
        anOS << "Standard_Boolean " << aCPPName << "::" << aMethod << "() const\n"
                "{\n"
                "  return myHas" << aField->Name() << ";\n"
                "}\n";
      }

      // prepare additional methods for HArray1 field
      Standard_Integer aFindPos = aFieldCPPName.Search ("_HArray1");
      if (aFindPos > -1)
      {
        TCollection_AsciiString aNamePack = aFieldCPPName.SubString (1, aFindPos);
        TCollection_AsciiString aNameClass ("");
        if (aNamePack.IsEqual ("TColStd_"))
        {
          Standard_Integer aFindPos2 = aFieldCPPName.Search ("Integer");
          if (aFindPos2 > -1)
          {
            aNameClass = "Standard_Integer";
          }
          else
          {
            aFindPos2 = aFieldCPPName.Search ("Real");
            if (aFindPos2 > -1)
            {
              aNameClass = "Standard_Real";
            }
            else
            {
              aNameClass = "Standard_Boolean";
            }
          }
        }
        else
        {
          if (aNamePack.IsEqual ("Interface_"))
          {
            aNameClass = "Handle(TCollection_HAsciiString)";
          }
          else
          {
            aNameClass = "Handle(";
            aNameClass += aNamePack;
            aNameClass += aFieldCPPName.SubString (aFindPos + 10, aFieldCPPName.Length());
            aNameClass += ")";
          }
        }
        // write method ::NbField()
        anOS << "\n";
        TCollection_AsciiString aMethodName = "Nb";
        aMethodName += aField->Name();
        Express::WriteMethodStamp (anOS, aMethodName);
        anOS << "Standard_Integer " << aCPPName << "::Nb" << aField->Name() << "() const;\n"
                "{\n"
                "  if (my" << aField->Name() << ".IsNull())\n"
                "  {\n"
                "    return 0;\n"
                "  }\n"
                "  return my" << aField->Name() << "->Length();\n"
                "}\n";
        // write method ::FieldValue(num)
        anOS << "\n";
        aMethodName = aField->Name();
        aMethodName += "Value";
        Express::WriteMethodStamp (anOS, aMethodName);
        anOS << aNameClass << " " << aCPPName << "::" << aField->Name() << "Value (const Standard_Integer theNum) const;\n"
                "{\n"
                "  return my" << aField->Name() << "->Value (theNum);\n"
                "}\n";
      }
    }

    // close
    aStreamPtr.reset();

    if (AbstractFlag())
    {
      // generation of RW class is not needed
      return Standard_True;
    }
  }
  //===============================
  // Step 3: generating HXX for Reader/Writer class
  TCollection_AsciiString aRWCPPName;
  {
    // Open HXX file
    aRWCPPName = "RW";
    aRWCPPName += GetPackageName();
    aRWCPPName += "_RW";
    aRWCPPName += Name();

    aPack = "RW";
    aPack += GetPackageName();
    OSD_Path aRWPath (aPack);
    OSD_Directory aRWDir (aRWPath);
    aRWDir.Build (aProt);
    aPack += "/";
    aPack += aRWCPPName;

    std::shared_ptr<std::ostream> aStreamPtr = aFileSystem->OpenOStream (aPack.Cat (".hxx"), std::ios::out | std::ios::binary);
    Standard_OStream& anOS = *aStreamPtr;

    // write header
    Express::WriteFileStamp (anOS);

    anOS << "#ifndef _" << aRWCPPName << "_HeaderFile_\n"
            "#define _" << aRWCPPName << "_HeaderFile_\n"
            "\n";

    // write includes
    anOS << "#include <Standard.hxx>\n"
            "#include <Standard_DefineAlloc.hxx>\n"
            "#include <Standard_Handle.hxx>\n"
            "\n"
            "class StepData_StepReaderData;\n"
            "class Interface_Check;\n"
            "class StepData_StepWriter;\n"
            "class Interface_EntityIterator;\n"
            "class " << aCPPName << ";\n"
            "\n";

    // write start of declaration (inheritance)
    anOS << "//! Read & Write tool for " << Name() << "\n"
            "class " << aRWCPPName << "\n"
            "{\n"
            "\n"
            "public:\n"
            "\n"
            "  DEFINE_STANDARD_ALLOC\n"
            "\n";

    // default constructor
    anOS << "  Standard_EXPORT " << aRWCPPName << "();\n"
            "\n";

    // read step
    anOS << "  Standard_EXPORT void ReadStep (const Handle(StepData_StepReaderData)& theData,\n"
            "                                 const Standard_Integer theNum,\n"
            "                                 Handle(Interface_Check)& theCheck,\n"
            "                                 const Handle(" << aCPPName << ")& theEnt) const;\n"
            "\n";

    // write step
    anOS << "  Standard_EXPORT void WriteStep (StepData_StepWriter& theSW,\n"
            "                                  const Handle(" << aCPPName << ")& theEnt) const;\n"
            "\n";

    // share
    anOS << "  Standard_EXPORT void Share (const Handle(" << aCPPName << ")& theEnt,\n"
            "                              Interface_EntityIterator& theIter) const;\n"
            "\n";

    // write end
    anOS << "};\n"
            "\n"
            "#endif // _" << aRWCPPName << "_HeaderFile_\n";
    aStreamPtr.reset();
  }
  //===============================
  // Step 4: generating CXX for Read/Write tool
  {
    // Open CXX file
    std::shared_ptr<std::ostream> aStreamPtr = aFileSystem->OpenOStream (aPack.Cat (".cxx"), std::ios::out | std::ios::binary);
    Standard_OStream& anOS = *aStreamPtr;

    // write header
    Express::WriteFileStamp (anOS);

    // write include section
    anOS << "#include <" << aRWCPPName << ".hxx>\n";
    anOS << "#include <" << aCPPName << ".hxx>\n";
    anOS << "#include <Interface_EntityIterator.hxx>\n"
            "#include <StepData_StepReaderData.hxx>\n"
            "#include <StepData_StepWriter.hxx>\n";
    // write constructor
    Express::WriteMethodStamp (anOS, aRWCPPName);
    anOS << aRWCPPName << "::" << aRWCPPName << "() {}\n"
            "\n";

    // write method ReadStep
    Express::WriteMethodStamp (anOS, "ReadStep");
    if (aRWCPPName.Length() < 40)
    {
      anOS << "void " << aRWCPPName << "::ReadStep (const Handle(StepData_StepReaderData)& theData,\n";
      writeSpaces (anOS, 17 + aRWCPPName.Length());
      anOS << "const Standard_Integer theNum,\n";
      writeSpaces (anOS, 17 + aRWCPPName.Length());
      anOS << "Handle(Interface_Check)& theCheck,\n";
      writeSpaces (anOS, 17 + aRWCPPName.Length());
      anOS << "const Handle(" << aCPPName << ")& theEnt) const\n";
    }
    else
    {
      anOS << "void " << aRWCPPName << "::ReadStep (\n"
              "  const Handle(StepData_StepReaderData)& theData,\n"
              "  const Standard_Integer theNum,\n"
              "  Handle(Interface_Check)& theCheck,\n"
              "  const Handle(" << aCPPName << ")& theEnt) const\n";
    }
    anOS << "{\n";
    Standard_Integer aNbFields = NbFields (Standard_True);

    anOS << "  // Check number of parameters\n"
            "  if (!theData->CheckNbParams (theNum, " << aNbFields << ", theCheck, \"" << Express::ToStepName (Name()) << "\"))\n"
            "  {\n"
            "    return;\n"
            "  }\n";
    writeRWReadCode (anOS, 1, Standard_True); // write code for reading inherited and own fields
    anOS << "\n"
            "  // Initialize entity\n"
            "  theEnt->Init (";
    makeInit (anOS, 15, Standard_True, 4);
    anOS << ");\n"
            "}\n";

    // write method WriteStep
    Express::WriteMethodStamp (anOS, "WriteStep");
    if (aRWCPPName.Length() < 40)
    {
      anOS << "void " << aRWCPPName << "::WriteStep (StepData_StepWriter& theSW,\n";
      writeSpaces (anOS, 18 + aRWCPPName.Length());
      anOS << "const Handle(" << aCPPName << ")& theEnt) const\n";
    }
    else
    {
      anOS << "void " << aRWCPPName << "::WriteStep (\n"
              "  StepData_StepWriter& theSW,\n"
              "  const Handle(" << aCPPName << ")& theEnt) const\n";
    }
    anOS << "{\n";

    writeRWWriteCode (anOS, 0, Standard_True); // write code for writing inherited and own fields
    anOS << "}\n";

    // write method Share
    Express::WriteMethodStamp (anOS, "Share");
    std::ostringstream aStringOS;
    Standard_Integer aNnFileds2 = writeRWShareCode (aStringOS, 1, Standard_True); // write code for filling graph of references
    if (aRWCPPName.Length() < 40)
    {
      anOS << "void " << aRWCPPName << "::Share (const Handle(" << aCPPName << ")&" << ((aNnFileds2 > 1) ? "theEnt," : ",") << "\n";
      writeSpaces (anOS, 14 + aRWCPPName.Length());
      if (aNnFileds2 > 1)
      {
        anOS << "Interface_EntityIterator& theIter) const\n";
      }
      else
      {
        anOS << "Interface_EntityIterator&) const\n";
      }
    }
    else
    {
      anOS << "void " << aRWCPPName << "::Share(\n"
              "  const Handle(" << aCPPName << ")&" << ((aNnFileds2 > 1) ? "theEnt," : ",") << "\n";
      if (aNnFileds2 > 1)
      {
        anOS << "Interface_EntityIterator& theIter) const\n";
      }
      else
      {
        anOS << "Interface_EntityIterator&) const\n";
      }
    }
    anOS << "{\n";
    if (aNnFileds2 > 1)
    {
      anOS << aStringOS.str();
    }
    anOS << "}\n";
    if (CheckFlag())
    {
      Express::WriteMethodStamp (anOS, "Check");
      anOS << "void " << aRWCPPName << "::Check (const Handle(Standard_Transient)& entt,\n";
      writeSpaces (anOS, 18 + aRWCPPName.Length());
      anOS << "const Interface_ShareTool& shares,\n";
      writeSpaces (anOS, 18 + aRWCPPName.Length());
      anOS << "Interface_Check& check) const\n"
              "{\n";
      // DownCast entity to it's type
      anOS << "  Handle(" << aCPPName << ") ent = Handle(" << aCPPName << ")::DownCast(entt);\n"
              "}\n";
    }
    if (FillSharedFlag())
    {
      Express::WriteMethodStamp (anOS, "FillShared");
      anOS << "void " << aRWCPPName << "::Share(const Handle(Interface_InterfaceModel)& model,\n";
      writeSpaces (anOS, 18 + aRWCPPName.Length());
      anOS << "const Handle(Standard_Transient)& entt,\n";
      writeSpaces (anOS, 18 + aRWCPPName.Length());
      anOS << "Interface_EntityIterator& iter) const\n"
              "{\n";
      // DownCast entity to it's type
      anOS << "  Handle(" << aCPPName << ") ent = Handle(" << aCPPName << ")::DownCast(entt)\n"
              "}\n";
    }

    // close
    aStreamPtr.reset();
  }
  //===============================
  // Step 5: adding method for registration of entities and include
  {
    Standard_Integer anIndex = Express_Item::Index();
    TCollection_AsciiString aRegDir = "Registration";
    OSD_Path aPathReg (aRegDir);
    OSD_Directory aDirReg (aRegDir);
    aDirReg.Build (aProt);
    aRegDir += "/";

    // write file with includes
    {
      TCollection_AsciiString aPackNameInc = "inc.txt";
      std::shared_ptr<std::ostream> aStreamPtr = aFileSystem->OpenOStream (aRegDir.Cat (aPackNameInc), std::ios::out | std::ios::binary | std::ios::app);
      Standard_OStream& anOS = *aStreamPtr;
      anOS << "#include <" << aCPPName << ".hxx>\n";
      aStreamPtr.reset();
    }
    // write file with RW includes
    {
      TCollection_AsciiString aPackNameRWInc = "rwinc.txt";
      std::shared_ptr<std::ostream> aStreamPtr = aFileSystem->OpenOStream (aRegDir.Cat (aPackNameRWInc), std::ios::out | std::ios::binary | std::ios::app);
      Standard_OStream& anOS = *aStreamPtr;
      anOS << "#include <" << aRWCPPName << ".hxx>\n";
      aStreamPtr.reset();
    }

    // generate data for entity registration
    if (anIndex > 0)
    {
      // StepAP214_Protocol.cxx
      {
        TCollection_AsciiString aPackNameProtocol = "protocol.txt";
        std::shared_ptr<std::ostream> aStreamPtr = aFileSystem->OpenOStream (aRegDir.Cat (aPackNameProtocol), std::ios::out | std::ios::binary | std::ios::app);
        Standard_OStream& anOS = *aStreamPtr;
        anOS << "types.Bind (STANDARD_TYPE(" << aCPPName << "), " << anIndex << ");\n";
        aStreamPtr.reset();
      }
      // RWStepAP214_GeneralModule.cxx
      // FillSharedCase
      {
        TCollection_AsciiString aPackNameFillShared = "fillshared.txt";
        std::shared_ptr<std::ostream> aStreamPtr = aFileSystem->OpenOStream (aRegDir.Cat (aPackNameFillShared), std::ios::out | std::ios::binary | std::ios::app);
        Standard_OStream& anOS = *aStreamPtr;
        anOS << "  case " << anIndex << ":\n"
                "  {\n"
                "    DeclareAndCast (" << aCPPName << ", anent, ent);\n"
                "    " << aRWCPPName << " aTool;\n"
                "    aTool.Share(anent, iter);\n"
                "  }\n"
                "  break;\n";
        aStreamPtr.reset();
      }
      // NewVoid
      {
        TCollection_AsciiString aPackNameNewVoid = "newvoid.txt";
        std::shared_ptr<std::ostream> aStreamPtr = aFileSystem->OpenOStream (aRegDir.Cat (aPackNameNewVoid), std::ios::out | std::ios::binary | std::ios::app);
        Standard_OStream& anOS = *aStreamPtr;
        anOS << "  case " << anIndex << ":\n"
                "    ent = new " << aCPPName << ";\n"
                "  break;\n";
        aStreamPtr.reset();
      }
      // CategoryNumber
      {
        TCollection_AsciiString aPackNameCategory = "category.txt";
        std::shared_ptr<std::ostream> aStreamPtr = aFileSystem->OpenOStream (aRegDir.Cat (aPackNameCategory), std::ios::out | std::ios::binary | std::ios::app);
        Standard_OStream& anOS = *aStreamPtr;
        anOS << "  case " << anIndex << ": return " << Category() << ";\n";
        aStreamPtr.reset();
      }
      // RWStepAP214_ReadWriteModule.cxx
      // Reco
      {
        TCollection_AsciiString aRecoName = Express::ToStepName (Name());
        aRecoName.UpperCase();
        TCollection_AsciiString aPackNameReco = "reco.txt";
        std::shared_ptr<std::ostream> aStreamPtr = aFileSystem->OpenOStream (aRegDir.Cat (aPackNameReco), std::ios::out | std::ios::binary | std::ios::app);
        Standard_OStream& anOS = *aStreamPtr;
        anOS << "  static TCollection_AsciiString Reco_" << Name() << " (\"" << aRecoName << "\");\n";
        aStreamPtr.reset();
      }
      // type bind
      {
        TCollection_AsciiString aPackNameTypeBind = "typebind.txt";
        std::shared_ptr<std::ostream> aStreamPtr = aFileSystem->OpenOStream (aRegDir.Cat (aPackNameTypeBind), std::ios::out | std::ios::binary | std::ios::app);
        Standard_OStream& anOS = *aStreamPtr;
        anOS << "  typenums.Bind (Reco_" << Name() << ", " << anIndex << ");\n";
        aStreamPtr.reset();
      }
      // StepType
      {
        TCollection_AsciiString aPackNameStepType = "steptype.txt";
        std::shared_ptr<std::ostream> aStreamPtr = aFileSystem->OpenOStream (aRegDir.Cat (aPackNameStepType), std::ios::out | std::ios::binary | std::ios::app);
        Standard_OStream& anOS = *aStreamPtr;
        anOS << "  case " << anIndex << ": return Reco_" << Name() << ";\n";
        aStreamPtr.reset();
      }
      // ReadStep
      {
        TCollection_AsciiString aPackNameReadStep = "readstep.txt";
        std::shared_ptr<std::ostream> aStreamPtr = aFileSystem->OpenOStream (aRegDir.Cat (aPackNameReadStep), std::ios::out | std::ios::binary | std::ios::app);
        Standard_OStream& anOS = *aStreamPtr;
        anOS << "  case " << anIndex << ":\n"
                "  {\n"
                "    DeclareAndCast (" << aCPPName << ", anent, ent);\n"
                "    " << aRWCPPName << " aTool;\n"
                "    aTool.ReadStep (data, num, ach, anent);\n"
                "  }\n"
                "  break;\n";
        aStreamPtr.reset();
      }
      // WriteStep
      {
        TCollection_AsciiString aPackNameWriteStep = "writestep.txt";
        std::shared_ptr<std::ostream> aStreamPtr = aFileSystem->OpenOStream (aRegDir.Cat (aPackNameWriteStep), std::ios::out | std::ios::binary | std::ios::app);
        Standard_OStream& anOS = *aStreamPtr;
        anOS << "  case " << anIndex << ":\n"
                "  {\n"
                "    DeclareAndCast (" << aCPPName << ", anent, ent);\n"
                "    " << aRWCPPName << " aTool;\n"
                "    aTool.WriteStep (SW, anent);\n"
                "  }\n"
                "  break;\n";
        aStreamPtr.reset();
      }
      Express_Item::SetIndex (anIndex + 1);
    }
  }
  return Standard_True;
}

//=======================================================================
// function : PropagateUse
// purpose  :
//=======================================================================

void Express_Entity::PropagateUse() const
{
  const TCollection_AsciiString& aPack = GetPackageName();
  const TCollection_AsciiString& aName = Name();

  for (Standard_Integer i = 1; i <= myInherit->Length(); i++)
  {
    Handle(Express_Entity) anEntity = myInherit->Value (i);
    anEntity->Use2 (aName, aPack);
  }
  for (Standard_Integer i = 1; i <= myFields->Length(); i++)
  {
    Handle(Express_Type) aType = myFields->Value (i)->Type();
    aType->Use2 (aName, aPack);
  }
}

//=======================================================================
// function : WriteIncludes
// purpose  :
//=======================================================================

Standard_Boolean Express_Entity::writeIncludes (Standard_OStream& theOS) const
{
  DataMapOfStringInteger aDict;

  for (Standard_Integer i = 1; i <= myInherit->Length(); i++)
  {
    Handle(Express_Entity) anEntity = myInherit->Value (i);
    anEntity->Use();
    const TCollection_AsciiString& anEntityCPPName = anEntity->CPPName();
    if (aDict.IsBound (anEntityCPPName))
    {
      continue; // avoid duplicating
    }
    aDict.Bind (anEntityCPPName, 1);
    theOS << "#include <" << anEntityCPPName << ".hxx>\n";
  }
  // write includes for own fields
  for (Standard_Integer i = 1; i <= myFields->Length(); i++)
  {
    Handle(Express_Type) aType = myFields->Value (i)->Type();
    aType->Use();
    const TCollection_AsciiString aTypeCPPName = aType->CPPName();
    if (aDict.IsBound (aTypeCPPName))
    {
      continue; // avoid duplicating
    }
    aDict.Bind (aTypeCPPName, 1);
    if (aType->IsStandard())
    {
      continue;
    }
    theOS << "#include <" << aTypeCPPName << ".hxx>\n";
    const TCollection_AsciiString& aPack = GetPackageName();
    // check that last include is for array
    TCollection_AsciiString aStrForSearch = aPack;
    aStrForSearch += "_HArray";
    Standard_Integer aFindPos = aTypeCPPName.Search (aStrForSearch.ToCString());
    if (aFindPos > -1)
    {
      // prepare file names
      aFindPos = aPack.Length();
      const TCollection_AsciiString& aNameHA = aTypeCPPName;
      TCollection_AsciiString aNameA = aNameHA.SubString (1, aFindPos + 1);
      aNameA += aNameHA.SubString (aFindPos + 3, aNameHA.Length());
      TCollection_AsciiString aNameClass = aNameHA.SubString (aFindPos + 11, aNameHA.Length());
      const Handle(OSD_FileSystem)& aFileSystem = OSD_FileSystem::DefaultFileSystem();
      // create a package directory (if not yet exist)
      OSD_Protection aProt (OSD_RWXD, OSD_RWXD, OSD_RX, OSD_RX);
      OSD_Path aPath (aPack);
      OSD_Directory aDir (aPath);
      aDir.Build (aProt);
      // create hxx files for Array1
      {
        // Open HXX file
        TCollection_AsciiString aFileName = aPack;
        aFileName += "/";
        aFileName += aNameA;
        std::shared_ptr<std::ostream> aStreamPtr = aFileSystem->OpenOStream (aFileName.Cat (".hxx"), std::ios::out | std::ios::binary);
        Standard_OStream& anOS = *aStreamPtr;
        // write header
        Express::WriteFileStamp (anOS);
        anOS << "\n"
                "#ifndef " << aNameA << "_HeaderFile\n"
                "#define " << aNameA << "_HeaderFile\n"
                "\n"
                "#include <" << aPack << "_" << aNameClass << ".hxx>\n"
                "#include <NCollection_" << aNameHA.SubString (aFindPos + 3, aFindPos + 8) << ".hxx>\n"
                "\n"
                "typedef NCollection_" << aNameHA.SubString (aFindPos + 3, aFindPos + 8) << "<Handle(" << aPack << "_" << aNameClass << ")> " << aNameA << ";\n"
                "\n"
                "#endif\n";
        aStreamPtr.reset();
      }
      // create hxx files for HArray1
      {
        // Open HXX file
        TCollection_AsciiString aFileName = aPack;
        aFileName += "/";
        aFileName += aNameHA;
        std::shared_ptr<std::ostream> aStreamPtr = aFileSystem->OpenOStream (aFileName.Cat (".hxx"), std::ios::out | std::ios::binary);
        Standard_OStream& anOS = *aStreamPtr;
        // write header
        Express::WriteFileStamp (anOS);
        anOS << "\n"
                "#ifndef " << aNameHA << "_HeaderFile\n"
                "#define " << aNameHA << "_HeaderFile\n"
                "\n"
                "#include <" << aNameA << ".hxx>\n"
                "#include <NCollection_" << aNameHA.SubString (aFindPos + 2, aFindPos + 8) << ".hxx>\n"
                "\n"
                "DEFINE_HARRAY" << aNameHA.SubString (aFindPos + 8, aFindPos + 8) << "(" << aNameHA << ", " << aNameA << ");\n"
                "\n"
                "#endif\n";
        aStreamPtr.reset();
      }
    }
  }

  return Standard_True;
}

//=======================================================================
// function : typeToSTEPName
// purpose  : auxiliary for WriteRWReadField
//=======================================================================

static TCollection_AsciiString typeToSTEPName (const Handle(Express_Type)& theType)
{
  TCollection_AsciiString aCPPName = theType->CPPName();
  TCollection_AsciiString aCls = aCPPName.Token ("_", 2);
  if (aCls.Length() < 1)
  {
    aCls = aCPPName;
  }
  return Express::ToStepName (aCls);
}

//=======================================================================
// function : WriteRWReadField
// purpose  :
//=======================================================================

static void writeRWReadField (Standard_OStream& theOS,
                              const TCollection_AsciiString& theIndex,
                              const TCollection_AsciiString& theSTEPName,
                              const TCollection_AsciiString& theVarName,
                              const Handle(Express_Type)& theVarType,
                              const Standard_Integer theLevel,
                              const Standard_Boolean isOptional)
{
  // indent
  TCollection_AsciiString anIdent ("  ");
  Standard_Integer aLevel = 0;
  for (; aLevel < theLevel; aLevel++)
  {
    anIdent += "  ";
  }
  aLevel += 2;

  // name of variable identifying number of parameter in data
  TCollection_AsciiString aParam ("theNum");
  if (theLevel > 0)
  {
    aParam = TCollection_AsciiString ("aNum");
    aParam += TCollection_AsciiString (theLevel);
  }

  // decode aliased types
  Handle(Express_Type) aType = theVarType;
  const TCollection_AsciiString aTypeCPPName = aType->CPPName();
  while (aType->IsKind (STANDARD_TYPE(Express_NamedType)))
  {
    Handle(Express_NamedType) aNamedType = Handle(Express_NamedType)::DownCast (aType);
    if (!aNamedType->Item()->IsKind (STANDARD_TYPE(Express_Alias)))
    {
      break;
    }
    Handle(Express_Alias) anAlias = Handle(Express_Alias)::DownCast (aNamedType->Item());
    aType = anAlias->Type();
  }

  // declare variable
  if (aType->IsHandle())
  {
    theOS << anIdent << "Handle(" << aTypeCPPName << ") a";
  }
  else
  {
    theOS << anIdent << aTypeCPPName << " a";
  }
  theOS << theVarName << ";\n";

  if (isOptional)
  {
    theOS << anIdent << "Standard_Boolean has" << theVarName << " = Standard_True;\n" <<
             anIdent << "if (theData->IsParamDefined (" << aParam << ", " << theIndex << "))\n" <<
             anIdent << "{\n";
    anIdent += "  ";
    aLevel++;
  }

  // read variable depending on its type
  if (aType->IsKind (STANDARD_TYPE(Express_NamedType)))
  {
    Handle(Express_NamedType) aNamedType = Handle(Express_NamedType)::DownCast (aType);
    if (aNamedType->Item()->IsKind (STANDARD_TYPE(Express_Entity)))
    {
      theOS << anIdent << "theData->ReadEntity (" << aParam << ", " << theIndex << ", \"" << theSTEPName << "\", theCheck,\n" <<
               anIdent << "  STANDARD_TYPE(" << aNamedType->CPPName() << "), a" << theVarName << ");\n";
    }
    else if (aNamedType->Item()->IsKind (STANDARD_TYPE(Express_Select)))
    {
      theOS << anIdent << "theData->ReadEntity (" << aParam << ", " << theIndex << ", \"" << theSTEPName <<
               "\", theCheck, a" << theVarName << ");\n";
    }
    else if (aNamedType->Item()->IsKind (STANDARD_TYPE(Express_Enum)))
    {
      theOS << anIdent << "if (theData->ParamType(" << aParam << ", " << theIndex << ") == Interface_ParamEnum)\n" <<
               anIdent << "{\n" <<
               anIdent << "  Standard_CString text = theData->ParamCValue(" << aParam << ", " << theIndex << ");\n";
      Handle(Express_Enum) anEnum = Handle(Express_Enum)::DownCast (aNamedType->Item());
      TCollection_AsciiString aPrefix = Express::EnumPrefix (anEnum->Name());
      Handle(TColStd_HSequenceOfHAsciiString) aNames = anEnum->Names();
      TCollection_AsciiString anEnumPack = anEnum->GetPackageName();
      for (Standard_Integer i = 1; i <= aNames->Length(); i++)
      {
        TCollection_AsciiString anEnumName = Express::ToStepName (aNames->Value (i)->String());
        anEnumName.UpperCase();
        theOS << anIdent << (i == 1 ? "  if     " : "  else if") << " (strcmp (text, \"." << anEnumName << ".\")) a" <<
              theVarName << " = " << anEnumPack << "_" << aPrefix << aNames->Value (i)->String() << ";\n";
      }
      theOS << anIdent << "  else\n" <<
               anIdent << "  {\n" <<
               anIdent << "    theCheck->AddFail (\"Parameter #" << theIndex << " (" << theSTEPName << ") has not allowed value\");\n" <<
               anIdent << "  }\n" <<
               anIdent << "}\n" <<
               anIdent << "else\n" <<
               anIdent << "{\n" <<
               anIdent << "  theCheck->AddFail (\"Parameter #" << theIndex << " (" << theSTEPName << ") is not enumeration\");\n" <<
               anIdent << "}\n";
    }
  }
  else if (aType->IsKind (STANDARD_TYPE(Express_ComplexType)))
  {
    Handle(Express_ComplexType) aComplex = Handle(Express_ComplexType)::DownCast (aType);
    theOS << anIdent << "Standard_Integer aSub" << theIndex << " = 0;\n" <<
             anIdent << "if (theData->ReadSubList (" << aParam << ", " << theIndex << ", \"" << theSTEPName << "\", theCheck, aSub" << theIndex << "))\n" <<
             anIdent << "{\n" <<
             anIdent << "  Standard_Integer aNb" << theLevel << " = theData->NbParams (aSub" << theIndex << ");\n";
    TCollection_AsciiString anIterI = theLevel;
    anIterI.Prepend ("i");
    TCollection_AsciiString aVar = theLevel;
    aVar.Prepend ("nIt");
    if (aComplex->Type()->IsKind (STANDARD_TYPE(Express_ComplexType)))
    {
      // array 2
      Handle(Express_ComplexType) aComplex2 = Handle(Express_ComplexType)::DownCast (aComplex->Type());
      TCollection_AsciiString anIterJ = theLevel;
      anIterJ.Prepend ("j");
      theOS << anIdent << "  Standard_Integer nbj" << theLevel << " = theData->NbParams (theData->ParamNumber (aSub" << theIndex << ", 1));\n" <<
               anIdent << "  a" << theVarName << " = new " << aTypeCPPName << "(1, aNb" << theLevel << ", 1, nbj" << theLevel << ");\n" <<
               anIdent << "  for (Standard_Integer " << anIterI << " = 1; " << anIterI << " <= aNb" << theLevel << "; " << anIterI << "++)\n" <<
               anIdent << "  {\n" <<
               anIdent << "    Standard_Integer subj" << theIndex << " = 0;\n" <<
               anIdent << "    if ( theData->ReadSubList (aSub" << theIndex << ", " << anIterI << ", \"sub-part(" << theSTEPName << ")\", theCheck, subj" << theIndex << ") ) {\n" <<
               anIdent << "      Standard_Integer aNum" << aLevel + 2 << " = subj" << theIndex << ";\n" <<
               anIdent << "      for (Standard_Integer " << anIterJ << " = 1; " << anIterJ << " <= nbj" << theLevel << "; " << anIterJ << "++)\n" <<
               anIdent << "      {\n";
      TCollection_AsciiString aSubName = typeToSTEPName (aComplex2->Type());
      writeRWReadField (theOS, anIterJ, aSubName, aVar, aComplex2->Type(), aLevel + 2, Standard_False);
      theOS << anIdent << "        a" << theVarName << "->SetValue(" << anIterI << "," << anIterJ << ", a" << aVar << ");\n" <<
               anIdent << "      }\n" <<
               anIdent << "    }\n";
    }
    else
    {
      // simple array
      theOS << anIdent << "  a" << theVarName << " = new " << aTypeCPPName << "(1, aNb" << theLevel << ");\n" <<
               anIdent << "  Standard_Integer aNum" << aLevel << " = aSub" << theIndex << ";\n" <<
               anIdent << "  for (Standard_Integer " << anIterI << " = 1; " << anIterI << " <= aNb" << theLevel << "; " << anIterI << "++)\n" <<
               anIdent << "  {\n";
      TCollection_AsciiString aSubName = typeToSTEPName (aComplex->Type());
      writeRWReadField (theOS, anIterI, aSubName, aVar, aComplex->Type(), aLevel, Standard_False);
      theOS << anIdent << "    a" << theVarName << "->SetValue(" << anIterI << ", a" << aVar << ");\n";
    }
    theOS << anIdent << "  }\n" <<
             anIdent << "}\n";
  }
  else if (aType->IsKind (STANDARD_TYPE(Express_String)))
  {
    if (theSTEPName.Length() + theVarName.Length() < 70)
    {
      theOS << anIdent << "theData->ReadString (" << aParam << ", " << theIndex << ", \"" << theSTEPName << "\", theCheck, a" << theVarName << ");\n";
    }
    else
    {
      theOS << anIdent << "theData->ReadString (" << aParam << ", " << theIndex << ", \"" << theSTEPName << "\", theCheck,\n" <<
               anIdent << "  a" << theVarName << ");\n";
    }
  }
  else if (aType->IsKind (STANDARD_TYPE(Express_Logical)))
  {
    theOS << anIdent << "theData->ReadLogical (" << aParam << ", " << theIndex << ", \"" << theSTEPName << "\", theCheck, a" << theVarName << ");\n";
  }
  else if (aType->IsKind (STANDARD_TYPE(Express_Boolean)))
  {
    theOS << anIdent << "theData->ReadBoolean (" << aParam << ", " << theIndex << ", \"" << theSTEPName << "\", theCheck, a" << theVarName << ");\n";
  }
  else if (aType->IsKind (STANDARD_TYPE(Express_Number)) || aType->IsKind (STANDARD_TYPE(Express_Integer)))
  {
    theOS << anIdent << "theData->ReadInteger (" << aParam << ", " << theIndex << ", \"" << theSTEPName << "\", theCheck, a" << theVarName << ");\n";
  }
  else if (aType->IsKind (STANDARD_TYPE(Express_Real)))
  {
    theOS << anIdent << "theData->ReadReal (" << aParam << ", " << theIndex << ", \"" << theSTEPName << "\", theCheck, a" << theVarName << ");\n";
  }

  if (isOptional)
  {
    anIdent.Remove (1, 2);
    theOS << anIdent << "}\n" <<
             anIdent << "else\n" <<
             anIdent << "{\n" <<
             anIdent << "  has" << theVarName << " = Standard_False;\n" <<
             anIdent << "  a" << theVarName;
    if (aType->IsHandle())
    {
      theOS << ".Nullify();";
    }
    else if (aType->IsStandard())
    {
      theOS << " = 0;";
    }
    else
    {
      theOS << " = " << aTypeCPPName << "();";
    }
    theOS << "\n";
    theOS << anIdent << "}\n";
  }
}

//=======================================================================
// function : WriteRWReadCode
// purpose  :
//=======================================================================

Standard_Integer Express_Entity::writeRWReadCode (Standard_OStream& theOS,
                                                  const Standard_Integer theStart,
                                                  const Standard_Integer theOwn) const
{
  Standard_Integer aNum = theStart;

  // write code for reading inherited fields
  for (Standard_Integer i = 1; i <= myInherit->Length(); i++)
  {
    aNum = myInherit->Value (i)->writeRWReadCode (theOS, aNum, Standard_False);
  }

  // write code for reading own fields
  if (myFields->Length() > 0)
  {
    if (aNum > 0)
      theOS << "\n";
    theOS << "  // " << (theOwn ? "Own" : "Inherited") << " fields of " << Name() << "\n";
  }

  for (Standard_Integer i = 1; i <= myFields->Length(); i++)
  {
    Handle(Express_Field) aField = myFields->Value (i);
    TCollection_AsciiString aSTEPName = Express::ToStepName (aField->Name());
    if (!theOwn)
    {
      aSTEPName.Prepend (Express::ToStepName (Name().Cat (".")));
    }
    TCollection_AsciiString aVarName (aField->Name());
    if (!theOwn)
    {
      aVarName.Prepend (Name().Cat ("_"));
    }
    theOS << "\n";
    writeRWReadField (theOS, TCollection_AsciiString (aNum), aSTEPName, aVarName, aField->Type(), 0, aField->IsOptional());
    aNum++;
  }

  return aNum;
}

//=======================================================================
// function : WriteRWWriteField
// purpose  :
//=======================================================================

static void writeRWWriteField (Standard_OStream& theOS,
                               const TCollection_AsciiString& theVarName,
                               const Handle(Express_Type)& theVarType,
                               const Standard_Integer theIndex,
                               const Standard_Integer theLevel)
{
  // indent
  TCollection_AsciiString anIdent ("  ");
  Standard_Integer aLevel = 0;
  for (; aLevel < theLevel; aLevel++)
  {
    anIdent += "  ";
  }
  aLevel++;

  // decode aliased types
  Handle(Express_Type) aType = theVarType;
  while (aType->IsKind (STANDARD_TYPE(Express_NamedType)))
  {
    Handle(Express_NamedType) aNamedType = Handle(Express_NamedType)::DownCast (aType);
    if (!aNamedType->Item()->IsKind (STANDARD_TYPE(Express_Alias)))
    {
      break;
    }
    Handle(Express_Alias) anAlias = Handle(Express_Alias)::DownCast (aNamedType->Item());
    aType = anAlias->Type();
  }

  // write variable depending on its type
  if (aType->IsKind (STANDARD_TYPE(Express_ComplexType)))
  {
    Handle(Express_ComplexType) aComplex = Handle(Express_ComplexType)::DownCast (aType);
    aType = aComplex->Type();
    TCollection_AsciiString aVar (theLevel);
    aVar.Prepend ("aVar");
    theOS << anIdent << "theSW.OpenSub();\n" <<
             anIdent << "for (Standard_Integer i" << theIndex << " = 1; i" << theIndex << " <= ";
    if (aType->IsKind (STANDARD_TYPE(Express_ComplexType)))
    {
      // array 2
      Handle(Express_ComplexType) complex2 = Handle(Express_ComplexType)::DownCast (aType);
      aType = complex2->Type();
      theOS << theVarName << "->RowLength(); i" << theIndex << "++)\n" <<
               anIdent << "{\n" <<
               anIdent << "  theSW.NewLine(Standard_False);\n" <<
               anIdent << "  theSW.OpenSub();\n" <<
               anIdent << "  for (Standard_Integer j" << theIndex << " = 1; j" << theIndex << " <= " << theVarName << "->ColLength(); j" << theIndex << "++)\n" <<
               anIdent << "  {\n" <<
               anIdent << "  " << (aType->IsHandle() ? "  Handle(" : "  ") << aType->CPPName() << (aType->IsHandle() ? ") " : " ") << aVar << " = " << theVarName << "->Value (i" << theIndex << ", j" << theIndex << ");\n";
      writeRWWriteField (theOS, aVar, aType, theIndex + 1, aLevel + 1);
      theOS << anIdent << "  }\n" <<
               anIdent << "  theSW.CloseSub();\n";
    }
    else
    {
      // simple array
      theOS << theVarName << "->Length(); i" << theIndex << "++)\n" <<
               anIdent << "{\n" <<
               anIdent << (aType->IsHandle() ? "  Handle(" : "  ") << aType->CPPName() << (aType->IsHandle() ? ") " : " ") << aVar << " = " << theVarName << "->Value (i" << theIndex << ");\n";
      writeRWWriteField (theOS, aVar, aType, theIndex + 1, aLevel);
    }
    theOS << anIdent << "}\n" <<
             anIdent << "theSW.CloseSub();\n";
  }
  else if (aType->IsKind (STANDARD_TYPE(Express_Boolean)))
  {
    theOS << anIdent << "theSW.SendBoolean (" << theVarName << ");\n";
  }
  else if (aType->IsKind (STANDARD_TYPE(Express_Logical)))
  {
    theOS << anIdent << "theSW.SendLogical (" << theVarName << ");\n";
  }
  else
  {
    Handle(Express_NamedType) aNamedType = Handle(Express_NamedType)::DownCast (aType);
    if (!aNamedType.IsNull() && aNamedType->Item()->IsKind (STANDARD_TYPE(Express_Enum)))
    {
      Handle(Express_Enum) anEnum = Handle(Express_Enum)::DownCast (aNamedType->Item());
      TCollection_AsciiString aPrefix = Express::EnumPrefix (anEnum->Name());
      Handle(TColStd_HSequenceOfHAsciiString) aNames = anEnum->Names();
      TCollection_AsciiString anEnumPack = anEnum->GetPackageName();
      theOS << anIdent << "switch (" << theVarName << ")\n";
      theOS << anIdent << "{\n";
      for (Standard_Integer i = 1; i <= aNames->Length(); i++)
      {
        TCollection_AsciiString anEnumName = Express::ToStepName (aNames->Value (i)->String());
        anEnumName.UpperCase();
        theOS << anIdent << "  case " << anEnumPack << "_" << aPrefix << aNames->Value (i)->String() << ": theSW.SendEnum (\"." << anEnumName << ".\"); break;\n";
      }
      theOS << anIdent << "}\n";
    }
    else
    {
      theOS << anIdent << "theSW.Send (" << theVarName;
      if (!aNamedType.IsNull() && aNamedType->Item()->IsKind (STANDARD_TYPE(Express_Select)))
      {
        theOS << ".Value()";
      }
      theOS << ");\n";
    }
  }
}

//=======================================================================
// function : WriteRWWriteCode
// purpose  :
//=======================================================================

Standard_Integer Express_Entity::writeRWWriteCode (Standard_OStream& theOS,
                                                   const Standard_Integer theStart,
                                                   const Standard_Integer theOwn) const
{
  Standard_Integer aNum = theStart;

  // write code for writing inherited fields
  for (Standard_Integer i = 1; i <= myInherit->Length(); i++)
  {
    aNum = myInherit->Value (i)->writeRWWriteCode (theOS, aNum, (i > 1 ? -1 : 1));
  }

  // write code for writing own fields
  if (myFields->Length() > 0)
  {
    theOS << "\n"
             "  // " << (theOwn == 1 ? "Own" : "Inherited") << " fields of " << Name() << "\n";
  }

  for (Standard_Integer i = 1; i <= myFields->Length(); i++)
  {
    Handle(Express_Field) aField = myFields->Value (i);
    TCollection_AsciiString aVarName (aField->Name());
    if (!theOwn)
    {
      aVarName.Prepend (CPPName().Cat ("::"));
    }
    else if (theOwn == -1)
    {
      // inherited base class implemented as field
      aVarName.Prepend (Name().Cat ("()->"));
    }
    aVarName.Prepend ("theEnt->");
    aVarName += "()";
    theOS << "\n";

    if (aField->IsOptional())
    {
      theOS << "  if (theEnt->";
      if (!theOwn)
      {
        theOS << CPPName() << "::";
      }
      else if (theOwn == -1)
      {
        theOS << Name() << "()->";
      }
      theOS << "Has" << aField->Name() << "())\n";
      theOS << "  {\n";
    }
    writeRWWriteField (theOS, aVarName, aField->Type(), aNum, (aField->IsOptional() ? 1 : 0));
    if (aField->IsOptional())
    {
      theOS << "  }\n"
               "  else\n"
               "  {\n"
               "    theSW.SendUndef();\n"
               "  }\n";
    }
    aNum++;
  }

  return aNum;
}

//=======================================================================
// function : WriteRWShareField
// purpose  :
//=======================================================================

static Standard_Boolean writeRWShareField (Standard_OStream& theOS,
                                           const TCollection_AsciiString& theVarName,
                                           const Handle(Express_Type)& theVarType,
                                           const Standard_Integer theIndex,
                                           const Standard_Integer theLevel)
{
  // indent
  TCollection_AsciiString anIdent ("  ");
  Standard_Integer aLevel = 0;
  for (; aLevel < theLevel; aLevel++)
  {
    anIdent += "  ";
  }
  aLevel++;

  // decode aliased types
  Handle(Express_Type) aType = theVarType;
  while (aType->IsKind (STANDARD_TYPE(Express_NamedType)))
  {
    Handle(Express_NamedType) aNamedType = Handle(Express_NamedType)::DownCast (aType);
    if (!aNamedType->Item()->IsKind (STANDARD_TYPE(Express_Alias)))
    {
      break;
    }
    Handle(Express_Alias) anAlias = Handle(Express_Alias)::DownCast (aNamedType->Item());
    aType = anAlias->Type();
  }

  // write variable depending on its type
  if (aType->IsKind (STANDARD_TYPE(Express_ComplexType)))
  {
    Handle(Express_ComplexType) aComplex = Handle(Express_ComplexType)::DownCast (aType);
    aType = aComplex->Type();
    TCollection_AsciiString aVar (theLevel);
    aVar.Prepend ("aVar");
    std::ostringstream aStringOS;
    if (!writeRWShareField (aStringOS, aVar, aType, theIndex + 1, aLevel))
    {
      return Standard_False;
    }
    theOS << anIdent << "for (Standard_Integer i" << theIndex << " = 1; i" << theIndex << " <= " << theVarName << "->Length(); i" << theIndex << "++)\n" <<
             anIdent << "{\n" <<
             anIdent << (aType->IsHandle() ? "  Handle(" : "  ") << aType->CPPName() << (aType->IsHandle() ? ") " : " ") << aVar << " = " << theVarName << "->Value (i" << theIndex << ");\n";
    theOS << aStringOS.str();
    theOS << anIdent << "}\n";
    return Standard_True;
  }
  if (aType->IsKind (STANDARD_TYPE(Express_NamedType)))
  {
    Handle(Express_NamedType) aNamedType = Handle(Express_NamedType)::DownCast (aType);
    if (aNamedType->Item()->IsKind (STANDARD_TYPE(Express_Entity)))
    {
      theOS << anIdent << "theIter.AddItem (" << theVarName << ");\n";
      return Standard_True;
    }
    if (aNamedType->Item()->IsKind (STANDARD_TYPE(Express_Select)))
    {
      theOS << anIdent << "theIter.AddItem (" << theVarName << ".Value());\n";
      return Standard_True;
    }
  }

  return Standard_False;
}

//=======================================================================
// function : WriteRWShareCode
// purpose  :
//=======================================================================

Standard_Integer Express_Entity::writeRWShareCode (Standard_OStream& theOS,
                                                   const Standard_Integer theStart,
                                                   const Standard_Integer theOwn) const
{
  Standard_Integer aNum = theStart;

  // write code for sharing inherited fields
  for (Standard_Integer i = 1; i <= myInherit->Length(); i++)
  {
    aNum = myInherit->Value (i)->writeRWShareCode (theOS, aNum, (i > 1 ? -1 : Standard_False));
  }

  // write code for sharing own fields
  if (myFields->Length() > 0)
  {
    theOS << "\n"
             "  // " << (theOwn == 1 ? "Own" : "Inherited") << " fields of " << Name() << "\n";
  }

  for (Standard_Integer i = 1; i <= myFields->Length(); i++)
  {
    Handle(Express_Field) aField = myFields->Value (i);
    TCollection_AsciiString aVarName (aField->Name());
    if (!theOwn)
    {
      aVarName.Prepend (CPPName().Cat ("::"));
    }
    else if (theOwn == -1)
    {
      // inherited base class implemented as field
      aVarName.Prepend (Name().Cat ("()->"));
    }
    aVarName.Prepend ("theEnt->");
    aVarName += "()";

    std::ostringstream aStringOS;
    if (!writeRWShareField (aStringOS, aVarName, aField->Type(), aNum, (aField->IsOptional() ? 1 : 0)))
    {
      continue;
    }
    aNum++;
    theOS << "\n";
    if (aField->IsOptional())
    {
      theOS << "  if (theEnt->";
      if (!theOwn)
        theOS << CPPName() << "::";
      else if (theOwn == -1)
        theOS << Name() << "()->";
      theOS << "Has" << aField->Name() << "())\n"
               "  {\n";
    }
    theOS << aStringOS.str();
    if (aField->IsOptional())
    {
      theOS << "  }\n";
    }
  }

  return aNum;
}

//=======================================================================
// function : MakeInit
// purpose  :
//=======================================================================

Standard_Integer Express_Entity::makeInit (Standard_OStream& theOS,
                                           const Standard_Integer theShift,
                                           const Standard_Integer theOwn,
                                           const Standard_Integer theMode) const
{
  Standard_Integer aShift = theShift;

  // write code for inherited fields
  for (Standard_Integer i = 1; i <= myInherit->Length(); i++)
  {
    Handle(Express_Entity) anEntity = myInherit->Value (i);
    const TCollection_AsciiString& anEntityCPPName = anEntity->CPPName();
    if (theMode == 3)
    {
      Standard_Integer aShift2 = 0;
      if (i > 1)
      {
        theOS << "\n  my" << anEntity->Name() << " = new " << anEntityCPPName << ";"
                 "\n  my" << anEntity->Name() << "->Init (";
        aShift2 = 12 + anEntity->Name().Length();
      }
      else
      {
        theOS << "\n  " << anEntityCPPName << "::Init (";
        aShift2 = 10 + anEntityCPPName.Length();
      }
      anEntity->makeInit (theOS, aShift2, Standard_False, 2);
      theOS << ");";
    }
    else
    {
      aShift = anEntity->makeInit (theOS, aShift, (i > 1 ? -1 : Standard_False), theMode);
    }
  }

  // write code for own fields
  for (Standard_Integer i = 1; i <= myFields->Length(); i++)
  {
    Handle(Express_Field) aField = myFields->Value (i);
    Handle(Express_Type) aFieldType = aField->Type();
    TCollection_AsciiString aVarName(aField->Name());
    if (theOwn != 1)
    {
      aVarName.Prepend (Name().Cat ("_"));
    }

    // make CR and indent
    TCollection_AsciiString aSpaces = "";
    for (Standard_Integer aSpaceNum = 0; aSpaceNum < abs (aShift); aSpaceNum++)
    {
      aSpaces += " ";
    }
    Standard_Character aDelim = (theMode == 0 ? ',' : (theMode == 3 ? '\n' : ','));
    if (aShift < 0)
    {
      if (i == 1 && myInherit->Length() == 0 && theMode == 3)
      {
        theOS << aDelim << aSpaces;
      }
      else if (theMode == 4)
      {
        theOS << ", ";
      }
      else
      {
        theOS << aDelim << "\n" << aSpaces;
      }
    }
    else
    {
      aShift = -aShift;
    }

    if (aField->IsOptional())
    {
      if (theMode == 0)
      {
        theOS << "const Standard_Boolean theHas" << aVarName << ",\n" << aSpaces;
      }
      else if (theMode == 1)
      {
        theOS << "const Standard_Boolean theHas" << aVarName << ",\n" << aSpaces;
      }
      else if (theMode == 2)
      {
        theOS << "has" << aVarName << ",\n" << aSpaces;
      }
      else if (theMode == 4)
      {
        theOS << "has" << aVarName << ", ";
      }
    }

    // write field
    if (theMode == 0 || theMode == 1)
    {
      theOS << "const " << (aFieldType->IsHandle() ? "Handle(" : "") << aFieldType->CPPName() << (aFieldType->IsHandle() ? ")" : "") <<
               (aFieldType->IsSimple() ? " the" : "& the") << aVarName;
    }
    else if (theMode == 2)
    {
      theOS << "the" << aVarName;
    }
    else if (theMode == 4)
    {
      theOS << "a" << aVarName;
    }
    else
    {
      if (aField->IsOptional())
      {
        theOS << "myHas" << aField->Name() << " = theHas" << aVarName << ";\n"
                 "  if (myHas" << aField->Name() << ")\n"
                 "  {\n"
                 "    my" << aField->Name() << " = the" << aVarName << ";\n"
                 "  }\n"
                 "  else\n"
                 "  {\n"
                 "    my" << aField->Name();
        if (aFieldType->IsHandle())
        {
          theOS << ".Nullify();";
        }
        else if (aFieldType->IsStandard())
        {
          theOS << " = 0;";
        }
        else
        {
          theOS << " = " << aFieldType->CPPName() << "();";
        }
        theOS << "\n  }";
      }
      else
      {
        theOS << "my" << aField->Name() << " = the" << aVarName << ";";
      }
    }
    if (aShift > 0)
    {
      aShift = -aShift;
    }
  }

  return aShift;
}

//=======================================================================
// function : SetAbstractFlag
// purpose  :
//=======================================================================

void Express_Entity::SetAbstractFlag (const Standard_Boolean isAbstract)
{
  myIsAbstract = isAbstract;
}

//=======================================================================
// function : AbstractFlag
// purpose  :
//=======================================================================

Standard_Boolean Express_Entity::AbstractFlag() const
{
  return myIsAbstract;
}
