// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <OSD_Directory.hxx>
#include <OSD_Environment.hxx>
#include <OSD_File.hxx>
#include <OSD_Path.hxx>
#include <OSD_Protection.hxx>
#include <Resource_DataMapIteratorOfDataMapOfAsciiStringAsciiString.hxx>
#include <Resource_LexicalCompare.hxx>
#include <Resource_Manager.hxx>
#include <Resource_NoSuchResource.hxx>
#include <Resource_Unicode.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <Standard_TypeMismatch.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColStd_Array1OfAsciiString.hxx>

#include <algorithm>
#include <errno.h>

IMPLEMENT_STANDARD_RTTIEXT(Resource_Manager,Standard_Transient)

//! Auxiliary enumeration for function WhatKindOfLine().
enum Resource_KindOfLine
{
  Resource_KOL_End,
  Resource_KOL_Empty,
  Resource_KOL_Comment,
  Resource_KOL_Resource,
  Resource_KOL_Error
};

static Resource_KindOfLine WhatKindOfLine(OSD_File& aFile,
				       TCollection_AsciiString& aToken1,
				       TCollection_AsciiString& aToken2);

static Standard_Integer GetLine(OSD_File& aFile,TCollection_AsciiString& aLine);

static Standard_Boolean Debug;

Resource_Manager::Resource_Manager(const Standard_CString aName,
				   TCollection_AsciiString& aDefaultsDirectory,
				   TCollection_AsciiString& anUserDefaultsDirectory,
				   const Standard_Boolean Verbose) : myName(aName), myVerbose(Verbose)
{
  if ( !aDefaultsDirectory.IsEmpty() ) {
    OSD_Path anOSDPath(aDefaultsDirectory);
    if (!anOSDPath.Name().IsEmpty())
    {
      anOSDPath.DownTrek (anOSDPath.Name () + anOSDPath.Extension ());
    }
    anOSDPath.SetName(aName);
    anOSDPath.SetExtension("");
    TCollection_AsciiString aPath;
    anOSDPath.SystemName(aPath);
    Load(aPath,myRefMap);
  }
  else
    if (myVerbose)
      cout << "Resource Manager Warning: aDefaultsDirectory is empty." << endl;

  if ( !anUserDefaultsDirectory.IsEmpty() ) {
    OSD_Path anOSDPath(anUserDefaultsDirectory);
    if (!anOSDPath.Name().IsEmpty())
    {
      anOSDPath.DownTrek (anOSDPath.Name () + anOSDPath.Extension ());
    }
    anOSDPath.SetName(aName);
    anOSDPath.SetExtension("");
    TCollection_AsciiString aPath;
    anOSDPath.SystemName(aPath);
    Load(aPath,myRefMap);
  }
  else
    if (myVerbose)
      cout << "Resource Manager Warning: anUserDefaultsDirectory is empty." << endl;
}

Resource_Manager::Resource_Manager(const Standard_CString aName,
				   const Standard_Boolean Verbose) : myName(aName), myVerbose(Verbose)
{
  OSD_Environment envDebug("ResourceDebug");
  Debug = (!envDebug.Value().IsEmpty()) ;

  TCollection_AsciiString Directory ;

  OSD_Environment envVerbose("CSF_ResourceVerbose");
  if (!envVerbose.Value().IsEmpty())
    myVerbose = Standard_True;

  TCollection_AsciiString aPath,aUserPath;
  GetResourcePath(aPath,aName,Standard_False);
  GetResourcePath(aUserPath,aName,Standard_True);

  if (!aPath.IsEmpty())
    Load(aPath,myRefMap);
  else if (myVerbose)
    cout << "Resource Manager Warning: Environment variable \"CSF_" << aName << "Defaults\" not set." << endl;

  if (!aUserPath.IsEmpty())
    Load(aUserPath,myRefMap);
  else if (myVerbose)
    cout << "Resource Manager Warning: Environment variable \"CSF_" << aName << "UserDefaults\" not set." << endl;
}

void Resource_Manager::Load(TCollection_AsciiString& aPath,
                            Resource_DataMapOfAsciiStringAsciiString& aMap)
{
  Resource_KindOfLine aKind;
  TCollection_AsciiString Token1, Token2;
  OSD_Path Path(aPath);
  OSD_File File = Path;
  TCollection_AsciiString FileName = Path.Name();
  File.Open(OSD_ReadOnly,OSD_Protection());
  if (File.Failed()) {
    if (myVerbose)
      cout << "Resource Manager Warning: Cannot read file \"" << FileName
	   << "\". File not found or permission denied." << endl;
    return;
  }
  Standard_Integer LineNumber = 1;
  while ((aKind = WhatKindOfLine(File, Token1, Token2)) != Resource_KOL_End) {
    switch (aKind) {
    case Resource_KOL_End:
    case Resource_KOL_Comment:
    case Resource_KOL_Empty:
      break ;
    case Resource_KOL_Resource:
      if (!aMap.Bind(Token1,Token2))
        aMap(Token1) = Token2;
      break;
    case Resource_KOL_Error:
      if (myVerbose)
	cout << "Resource Manager: Syntax error at line "
	  << LineNumber << " in file : " << FileName << endl;
      break;
    }
    LineNumber++;
  }
  File.Close();
  if (myVerbose)
    cout << "Resource Manager: " << ((&aMap == &myUserMap) ? "User" : "Reference")
         << " file \"" << FileName << "\" loaded" << endl;
}

static Resource_KindOfLine WhatKindOfLine(OSD_File& aFile,
				       TCollection_AsciiString& aToken1,
				       TCollection_AsciiString& aToken2)
{
  TCollection_AsciiString WhiteSpace = " \t" ;
  Standard_Integer Pos1,Pos2,Pos ;
  TCollection_AsciiString Line ;

  if (!GetLine(aFile,Line))
    return Resource_KOL_End;

  if (Line.Value(1) == '!')
    return Resource_KOL_Comment;

  Pos1 = Line.FirstLocationNotInSet(WhiteSpace, 1, Line.Length());
  if (Line.Value(Pos1) == '\n')
    return Resource_KOL_Empty;

  Pos2 = Line.Location(1,':',Pos1,Line.Length());
  if (!Pos2 || Pos1 == Pos2)
    return Resource_KOL_Error;

  for (Pos = Pos2-1; Line.Value(Pos) == '\t' || Line.Value(Pos) == ' ' ; Pos--);
  aToken1 = Line.SubString(Pos1, Pos);

  if (Debug)
    cout << "Key = '" << aToken1 << flush ;

  Pos = Line.FirstLocationNotInSet(WhiteSpace, Pos2+1, Line.Length());
  if (Pos) {
    if (Line.Value(Pos) == '\\')
      switch(Line.Value(Pos+1)) {
      case '\\' :
      case ' '  :
      case '\t' :
	Pos++;
	break;
      }
  }
  if (Pos == Line.Length())
    aToken2.Clear();
  else {
    Line.Remove(1,Pos-1);
    Line.Remove(Line.Length());
    aToken2 = Line;
  }
  if (Debug)
    cout << "'\t Value = '" << aToken2 << "'" << endl << flush;
  return Resource_KOL_Resource;
}

// Retourne 0 (EOF) ou une ligne toujours terminee par <NL>.

static Standard_Integer GetLine(OSD_File& aFile,TCollection_AsciiString& aLine)
{
  TCollection_AsciiString Buffer;
  Standard_Integer BufSize = 10;
  Standard_Integer Len;

  aLine.Clear();
  do {
    aFile.ReadLine(Buffer,BufSize,Len);
    aLine += Buffer;
    if (aFile.IsAtEnd()) {
      if (!aLine.Length()) return 0;
      else aLine += "\n";
    }
  } while (aLine.Value(aLine.Length()) != '\n');

  return 1;
}

//=======================================================================
//function : Save
//purpose  : Sort and save the user resources in the user file.
//           Creates the file if it does not exist.
//=======================================================================
Standard_Boolean Resource_Manager::Save() const
{
  TCollection_AsciiString anEnvVar("CSF_");
  anEnvVar += myName;
  anEnvVar += "UserDefaults";

  TCollection_AsciiString dir;
  OSD_Environment anEnv(anEnvVar);
  dir = anEnv.Value();
  if (dir.IsEmpty()) {
    if (myVerbose)
      cout << "Resource Manager Warning: environment variable \""
	   << anEnvVar << "\" not set.  Cannot save resources." << endl ;
    return Standard_False;
  }

  TCollection_AsciiString aFilePath(dir);
  OSD_Path anOSDPath(aFilePath);
  OSD_Directory Dir = anOSDPath;
  Standard_Boolean aStatus = Standard_True;
  if ( !Dir.Exists() ) {
    {
      try {
        OCC_CATCH_SIGNALS
        Dir.Build(OSD_Protection(OSD_RX, OSD_RWXD, OSD_RX, OSD_RX));
      }
      catch (Standard_Failure) {
        aStatus = Standard_False;
      }
    }
    aStatus = aStatus && !Dir.Failed();
    if (!aStatus) {
      if (myVerbose)
        cout << "Resource Manager: Error opening or creating directory \"" << aFilePath
             << "\". Permission denied. Cannot save resources." << endl;
      return Standard_False;
    }
  }

  if (!anOSDPath.Name().IsEmpty())
  {
    anOSDPath.DownTrek (anOSDPath.Name () + anOSDPath.Extension ());
  }
  anOSDPath.SetName(myName);
  anOSDPath.SetExtension("");
  anOSDPath.SystemName(aFilePath);

  OSD_File File = anOSDPath;
  OSD_Protection theProt;
  aStatus = Standard_True;
  {
    try {
      OCC_CATCH_SIGNALS
      File.Build(OSD_ReadWrite, theProt);
    }
    catch (Standard_Failure) {
      aStatus = Standard_False;
    }
  }
  aStatus = aStatus && !File.Failed();
  if (!aStatus) {
    if (myVerbose)
      cout << "Resource Manager: Error opening or creating file \"" << aFilePath
           << "\". Permission denied. Cannot save resources." << endl;
    return Standard_False;
  }

  const Standard_Integer NbKey = myUserMap.Extent();
  if (NbKey)
  {
    TColStd_Array1OfAsciiString KeyArray(1,NbKey);
    Resource_DataMapIteratorOfDataMapOfAsciiStringAsciiString Iter(myUserMap);

    Standard_Integer Index;
    for ( Index = 1; Iter.More(); Iter.Next())
      KeyArray(Index++)= Iter.Key();

  std::sort (KeyArray.begin(), KeyArray.end());

    TCollection_AsciiString Line, Value;
    for (Index = 1 ; Index <= NbKey ; Index++) {
      Value = myUserMap(KeyArray(Index));
      if (!Value.IsEmpty())
        switch(Value.Value(1)) {
        case '\\' :
        case ' ' :
        case '\t' :
          Value.Insert(1,'\\');
          break;
        }
      Line = KeyArray(Index) + ":\t" + Value + "\n";

      if (Debug)
        cout << "Line = '" << Line << "'" << endl;

      File.Write(Line, Line.Length());
    }
    if (myVerbose)
      cout << "Resource Manager: Resources saved in file " << aFilePath << endl;
  }
  File.Close();
  return Standard_True;
}

//=======================================================================
//function : Integer
//purpose  : Gets the value of an integer resource
//=======================================================================

Standard_Integer Resource_Manager::Integer(const Standard_CString aResourceName) const
{
  TCollection_AsciiString Result = Value(aResourceName) ;
  if (!Result.IsIntegerValue()) {
    TCollection_AsciiString n("Value of resource `");
    n+= aResourceName;
    n+= "` is not an integer";
    throw Standard_TypeMismatch(n.ToCString());
  }
  return Result.IntegerValue();
}

//=======================================================================
//function : Real
//purpose  : Gets the value of a real resource
//=======================================================================

Standard_Real Resource_Manager::Real(const Standard_CString  aResourceName) const
{
  TCollection_AsciiString Result = Value(aResourceName) ;
  if (!Result.IsRealValue()) {
    TCollection_AsciiString n("Value of resource `");
    n+= aResourceName;
    n+= "` is not a real";
    throw Standard_TypeMismatch(n.ToCString());
  }
  return Result.RealValue();
}

//=======================================================================
//function : Value
//purpose  : Gets the value of a CString resource
//=======================================================================

Standard_CString Resource_Manager::Value(const Standard_CString aResource) const
{
  TCollection_AsciiString  Resource(aResource);
  if (myUserMap.IsBound(Resource))
    return myUserMap(Resource).ToCString();
  if (myRefMap.IsBound(Resource))
    return myRefMap(Resource).ToCString();
  throw Resource_NoSuchResource(aResource);
}

//=======================================================================
//function : ExtValue
//purpose  : Gets the value of a ExtString resource
//=======================================================================

Standard_ExtString Resource_Manager::ExtValue(const Standard_CString aResource)
{
  TCollection_AsciiString  Resource(aResource);
  if (myExtStrMap.IsBound(Resource))
    return myExtStrMap(Resource).ToExtString();

  TCollection_AsciiString Result = Value(aResource);
  TCollection_ExtendedString ExtResult;

  Resource_Unicode::ConvertFormatToUnicode(Result.ToCString(),ExtResult);

  myExtStrMap.Bind(Resource, ExtResult);
  return  myExtStrMap(Resource).ToExtString();
}

//=======================================================================
//function : SetResource
//purpose  : Sets the new value of an integer resource.
//           If the resource does not exist, it is created.
//=======================================================================
void Resource_Manager::SetResource(const Standard_CString aResourceName,
				   const Standard_Integer aValue)
{
  SetResource(aResourceName,TCollection_AsciiString(aValue).ToCString());
}

//=======================================================================
//function : SetResource
//purpose  : Sets the new value of a real resource.
//           If the resource does not exist, it is created.
//=======================================================================
void Resource_Manager::SetResource(const Standard_CString aResourceName,
				   const Standard_Real    aValue)
{
  SetResource(aResourceName,TCollection_AsciiString(aValue).ToCString());
}

//=======================================================================
//function : SetResource
//purpose  : Sets the new value of ExtString resource.
//           If the resource does not exist, it is created.
//=======================================================================
void Resource_Manager::SetResource(const Standard_CString aResource,
				   const Standard_ExtString aValue)
{
  Standard_PCharacter pStr;
  TCollection_AsciiString Resource = aResource;
  TCollection_ExtendedString ExtValue = aValue;
  TCollection_AsciiString FormatStr(ExtValue.Length()*3+10, ' ');

  if (!myExtStrMap.Bind(Resource,ExtValue)) {
    myExtStrMap(Resource) = ExtValue;
  }
  //
  pStr=(Standard_PCharacter)FormatStr.ToCString();
  //
  Resource_Unicode::ConvertUnicodeToFormat(ExtValue,
					   pStr,//FormatStr.ToCString(),
					   FormatStr.Length()) ;
  SetResource(aResource,FormatStr.ToCString());
}

//=======================================================================
//function : SetResource
//purpose  : Sets the new value of an enum resource.
//           If the resource does not exist, it is created.
//=======================================================================
void Resource_Manager::SetResource(const Standard_CString aResource,
				   const Standard_CString aValue)
{
  TCollection_AsciiString Resource = aResource;
  TCollection_AsciiString Value = aValue;
  if (!myUserMap.Bind(Resource, Value))
    myUserMap(Resource) = Value;
}

//=======================================================================
//function : Find
//purpose  : Tells if a resource exits.
//=======================================================================
Standard_Boolean Resource_Manager::Find(const Standard_CString aResource) const
{
  TCollection_AsciiString  Resource(aResource);
  if (myUserMap.IsBound(Resource) || myRefMap.IsBound(Resource))
    return Standard_True;
  return Standard_False;
}

//=======================================================================
//function : GetResourcePath
//purpose  : 
//=======================================================================

void Resource_Manager::GetResourcePath (TCollection_AsciiString& aPath, const Standard_CString aName, const Standard_Boolean isUserDefaults)
{
  aPath.Clear();

  TCollection_AsciiString anEnvVar("CSF_");
  anEnvVar += aName;
  anEnvVar += isUserDefaults?"UserDefaults":"Defaults";

  TCollection_AsciiString dir;
  OSD_Environment anEnv(anEnvVar);
  dir = anEnv.Value();
  if (dir.IsEmpty())
    return;

  TCollection_AsciiString aResPath(dir);

  OSD_Path anOSDPath(aResPath);

  if (!anOSDPath.Name().IsEmpty())
  {
    anOSDPath.DownTrek (anOSDPath.Name () + anOSDPath.Extension ());
  }
  anOSDPath.SetName (aName);
  anOSDPath.SetExtension ("");

  anOSDPath.SystemName(aPath);
}
