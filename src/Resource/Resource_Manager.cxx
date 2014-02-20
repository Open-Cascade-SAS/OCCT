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

#include <Resource_Manager.hxx>
#include <Resource_Manager.ixx>
#include <Resource_DataMapIteratorOfDataMapOfAsciiStringAsciiString.hxx>
#include <Resource_QuickSortOfArray1.hxx>
#include <Resource_LexicalCompare.hxx>

#include <OSD_Path.hxx>
#include <OSD_File.hxx>
#include <OSD_Directory.hxx>
#include <OSD_Protection.hxx>

#include <Standard_ErrorHandler.hxx>
#include <TCollection_ExtendedString.hxx>
#include <Resource_Unicode.hxx>
#include <TColStd_Array1OfAsciiString.hxx>

#include <errno.h>

#define END      0
#define EMPTY    1
#define COMMENT  2
#define INCLUDE  3
#define RESOURCE 4
#define ERROR   -1

static Standard_Integer WhatKindOfLine(OSD_File& aFile,
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
    Load(aDefaultsDirectory,myName,myRefMap);
  }
  else
    if (myVerbose)
      cout << "Resource Manager Warning: aDefaultsDirectory is empty." << endl;

  if ( !anUserDefaultsDirectory.IsEmpty() ) {
    Load(anUserDefaultsDirectory,myName,myRefMap);
  }
  else
    if (myVerbose)
      cout << "Resource Manager Warning: anUserDefaultsDirectory is empty." << endl;
}

Resource_Manager::Resource_Manager(const Standard_CString aName,
				   const Standard_Boolean Verbose) : myName(aName), myVerbose(Verbose)
{
  Debug = (getenv("ResourceDebug") != NULL) ;

  TCollection_AsciiString EnvVar, CSF_ = "CSF_" ;
  TCollection_AsciiString Directory ;
  Standard_CString dir ;

  if ( getenv ("CSF_ResourceVerbose") != NULL )
    myVerbose = Standard_True;

  EnvVar = CSF_ + aName + "Defaults" ;
  if ((dir = getenv (EnvVar.ToCString())) != NULL) {
    Directory = dir;
    Load(Directory,myName,myRefMap);
  }
  else
    if (myVerbose)
      cout << "Resource Manager Warning: Environment variable \"" << EnvVar
	   << "\" not set." << endl;

  EnvVar = CSF_ + aName + "UserDefaults" ;
  if ((dir = getenv (EnvVar.ToCString())) != NULL) {
    Directory = dir;
    Load(Directory, myName, myUserMap);
  }
  else
    if (myVerbose)
      cout << "Resource Manager Warning: Environment variable \"" << EnvVar
	   << "\" not set." << endl;
}

void Resource_Manager::Load(TCollection_AsciiString& aDirectory,
			    TCollection_AsciiString& aName,
			    Resource_DataMapOfAsciiStringAsciiString& aMap)
{
  Standard_Integer Kind, Pos;
  TCollection_AsciiString Token1, Token2;
  TCollection_AsciiString Directory, Name;
  TCollection_AsciiString FileName;
  FileName =  aDirectory + "/" + aName;
  OSD_File File = OSD_Path(FileName);
  File.Open(OSD_ReadOnly,OSD_Protection());
  if (File.Failed()) {
    if (myVerbose)
      cout << "Resource Manager Warning: Cannot read file \"" << FileName
	   << "\". File not found or permission denied." << endl;
    return;
  }
  Standard_Integer LineNumber = 1;
  while ((Kind = WhatKindOfLine(File, Token1, Token2)) != END) {
    switch (Kind) {
    case COMMENT :
    case EMPTY :
      break ;
    case INCLUDE :
      Directory = OSD_Path::AbsolutePath(aDirectory,Token1);
      Pos = Directory.SearchFromEnd("/");
      if (Pos != 0) {
	Name = Directory.Split(Pos);
	Directory.Trunc(Pos-1);
	Load(Directory,Name,aMap);
      }
      break;
    case RESOURCE :
      if (!aMap.Bind(Token1,Token2))
        aMap(Token1) = Token2;
      break;
    case ERROR :
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

static Standard_Integer WhatKindOfLine(OSD_File& aFile,
				       TCollection_AsciiString& aToken1,
				       TCollection_AsciiString& aToken2)
{
  TCollection_AsciiString WhiteSpace = " \t" ;
  Standard_Integer Pos1,Pos2,Pos ;
  TCollection_AsciiString Line ;

  if (!GetLine(aFile,Line))
    return END;

  if (Line.Value(1) == '!')
    return COMMENT;

  if (Line.Value(1) == '#') {
    Line.Remove(1);
    if ((Line.Token(" \t")).IsDifferent("include"))
      return ERROR;

    aToken1 = Line.Token(" \t\n",2);
    return INCLUDE;
  }

  Pos1 = Line.FirstLocationNotInSet(WhiteSpace, 1, Line.Length());
  if (Line.Value(Pos1) == '\n')
    return EMPTY;

  Pos2 = Line.Location(1,':',Pos1,Line.Length());
  if (!Pos2 || Pos1 == Pos2)
    return ERROR;

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
  else
    aToken2 = Line.SubString(Pos,Line.Length()-1);

  if (Debug)
    cout << "'\t Value = '" << aToken2 << "'" << endl << flush;
  return RESOURCE;
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
  Standard_Integer Index;
  TCollection_AsciiString EnvVar, CSF_ = "CSF_";
  Standard_CString dir;

  EnvVar = CSF_ + myName + "UserDefaults";

  if ((dir = getenv (EnvVar.ToCString())) == NULL) {
    if (myVerbose)
      cout << "Resource Manager Warning: environment variable \""
	   << EnvVar << "\" not set.  Cannot save resources." << endl ;
    return Standard_False;
  }
  TCollection_AsciiString FilePath = dir;
  OSD_Directory Dir = OSD_Path(FilePath);
  Standard_Boolean Status = Standard_True;
  if ( !Dir.Exists() ) {
    {
      try {
        OCC_CATCH_SIGNALS
	Dir.Build(OSD_Protection(OSD_RX, OSD_RWX, OSD_RX, OSD_RX));
      }
      catch (Standard_Failure) {
	Status = Standard_False;
      }
    }
    Status = Status && !Dir.Failed();
    if (!Status) {
      if (myVerbose)
	cout << "Resource Manager: Error opening or creating directory \"" << FilePath
	     << "\". Permission denied. Cannot save resources." << endl;
      return Standard_False;
    }
  }

  FilePath += "/"; FilePath += myName;
  OSD_Path Path(FilePath);
  OSD_File File = Path;
  OSD_Protection theProt;
  Status = Standard_True;
  {
    try {
      OCC_CATCH_SIGNALS
      File.Build(OSD_ReadWrite, theProt);
    }
    catch (Standard_Failure) {
      Status = Standard_False;
    }
  }
  Status = Status && !File.Failed();
  if (!Status) {
    if (myVerbose)
      cout << "Resource Manager: Error opening or creating file \"" << FilePath
           << "\". Permission denied. Cannot save resources." << endl;
    return Standard_False;
  }

  Resource_LexicalCompare Comp;
  Standard_Integer NbKey = myUserMap.Extent();
  TColStd_Array1OfAsciiString KeyArray(1,NbKey+1); // 1 more item is added to allow saving empty resource
  Resource_DataMapIteratorOfDataMapOfAsciiStringAsciiString Iter(myUserMap);

  for ( Index = 1; Iter.More(); Iter.Next())
    KeyArray(Index++)= Iter.Key();

  Resource_QuickSortOfArray1::Sort(KeyArray, Comp);

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
    cout << "Resource Manager: Resources saved in file " << FilePath << endl;
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
    Standard_TypeMismatch::Raise(n.ToCString());
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
    Standard_TypeMismatch::Raise(n.ToCString());
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
  Resource_NoSuchResource::Raise(aResource);
  return ("");
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
