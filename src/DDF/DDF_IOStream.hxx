// Created on: 1997-08-22
// Created by: DAUTRY Philippe
// Copyright (c) 1997-1999 Matra Datavision
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

// This file has been written using FSD_File.hxx as template.
// This is a specific adaptation for Draw use (save & restore commands).
// It is not sure at all this code is portable on any other plateform than
// SUN OS. Don't use it anywhere else.
// Thanks for comprehension. (22 august 97)


#ifndef DDF_IOStream_HeaderFile
#define DDF_IOStream_HeaderFile

#include <stdio.h>

#ifndef _Storage_BaseDriver_HeaderFile
#include <Storage_BaseDriver.hxx>
#endif
#ifndef _Storage_Error_HeaderFile
#include <Storage_Error.hxx>
#endif
#ifndef _Storage_OpenMode_HeaderFile
#include <Storage_OpenMode.hxx>
#endif
#ifndef _Standard_Boolean_HeaderFile
#include <Standard_Boolean.hxx>
#endif
#ifndef _Standard_Integer_HeaderFile
#include <Standard_Integer.hxx>
#endif
#ifndef _Standard_Character_HeaderFile
#include <Standard_Character.hxx>
#endif
#ifndef _Standard_ExtCharacter_HeaderFile
#include <Standard_ExtCharacter.hxx>
#endif
#ifndef _Standard_Real_HeaderFile
#include <Standard_Real.hxx>
#endif
#ifndef _Standard_ShortReal_HeaderFile
#include <Standard_ShortReal.hxx>
#endif
#ifndef _Standard_CString_HeaderFile
#include <Standard_CString.hxx>
#endif
#include <Storage_Position.hxx>

class Storage_StreamTypeMismatchError;
class Storage_StreamFormatError;
class Storage_StreamWriteError;
class Storage_StreamExtCharParityError;

class TColStd_SequenceOfExtendedString;

#ifndef Standard_EXPORT
#ifdef WNT
#define Standard_EXPORT __declspec(dllexport)
#else
#define Standard_EXPORT
#endif
#endif
#ifdef WNT
#pragma warning (disable : 4275)
#endif

class Standard_EXPORT DDF_IOStream  : public Storage_BaseDriver {

public:

 // Methods PUBLIC
 // 
DDF_IOStream();
  Storage_Error Open(const TCollection_AsciiString& aName,const Storage_OpenMode aMode) ;
  Storage_Error Open(istream* anIStream) ;
  Storage_Error Open(ostream* anOStream) ;
  Standard_Boolean IsEnd() ;
  Storage_Position Tell() { return -1; }
static  Storage_Error IsGoodFileType(istream* anIStream) ;
  Storage_Error BeginWriteInfoSection() ;
  void WriteInfo(const Standard_Integer nbObj,const TCollection_AsciiString& dbVersion,const TCollection_AsciiString& date,const TCollection_AsciiString& schemaName,const TCollection_AsciiString& schemaVersion,const TCollection_ExtendedString& appName,const TCollection_AsciiString& appVersion,const TCollection_ExtendedString& objectType,const TColStd_SequenceOfAsciiString& userInfo) ;
  Storage_Error EndWriteInfoSection() ;
  Storage_Error BeginReadInfoSection() ;
  void ReadInfo(Standard_Integer& nbObj,TCollection_AsciiString& dbVersion,TCollection_AsciiString& date,TCollection_AsciiString& schemaName,TCollection_AsciiString& schemaVersion,TCollection_ExtendedString& appName,TCollection_AsciiString& appVersion,TCollection_ExtendedString& objectType,TColStd_SequenceOfAsciiString& userInfo) ;
  Storage_Error EndReadInfoSection() ;
  Storage_Error BeginWriteCommentSection() ;
  void WriteComment(const TColStd_SequenceOfExtendedString& userComments) ;
  Storage_Error EndWriteCommentSection() ;
  Storage_Error BeginReadCommentSection() ;
  void ReadComment(TColStd_SequenceOfExtendedString& userComments) ;
  Storage_Error EndReadCommentSection() ;
  Storage_Error BeginWriteTypeSection() ;
  void SetTypeSectionSize(const Standard_Integer aSize) ;
  void WriteTypeInformations(const Standard_Integer typeNum,const TCollection_AsciiString& typeName) ;
  Storage_Error EndWriteTypeSection() ;
  Storage_Error BeginReadTypeSection() ;
  Standard_Integer TypeSectionSize() ;
  void ReadTypeInformations(Standard_Integer& typeNum,TCollection_AsciiString& typeName) ;
  Storage_Error EndReadTypeSection() ;
  Storage_Error BeginWriteRootSection() ;
  void SetRootSectionSize(const Standard_Integer aSize) ;
  void WriteRoot(const TCollection_AsciiString& rootName,const Standard_Integer aRef,const TCollection_AsciiString& aType) ;
  Storage_Error EndWriteRootSection() ;
  Storage_Error BeginReadRootSection() ;
  Standard_Integer RootSectionSize() ;
  void ReadRoot(TCollection_AsciiString& rootName,Standard_Integer& aRef,TCollection_AsciiString& aType) ;
  Storage_Error EndReadRootSection() ;
  Storage_Error BeginWriteRefSection() ;
  void SetRefSectionSize(const Standard_Integer aSize) ;
  void WriteReferenceType(const Standard_Integer reference,const Standard_Integer typeNum) ;
  Storage_Error EndWriteRefSection() ;
  Storage_Error BeginReadRefSection() ;
  Standard_Integer RefSectionSize() ;
  void ReadReferenceType(Standard_Integer& reference,Standard_Integer& typeNum) ;
  Storage_Error EndReadRefSection() ;
  Storage_Error BeginWriteDataSection() ;
  void WritePersistentObjectHeader(const Standard_Integer aRef,const Standard_Integer aType) ;
  void BeginWritePersistentObjectData() ;
  void BeginWriteObjectData() ;
  void EndWriteObjectData() ;
  void EndWritePersistentObjectData() ;
  Storage_Error EndWriteDataSection() ;
  Storage_Error BeginReadDataSection() ;
  void ReadPersistentObjectHeader(Standard_Integer& aRef,Standard_Integer& aType) ;
  void BeginReadPersistentObjectData() ;
  void BeginReadObjectData() ;
  void EndReadObjectData() ;
  void EndReadPersistentObjectData() ;
  Storage_Error EndReadDataSection() ;
  void SkipObject() ;
  Storage_BaseDriver& PutReference(const Standard_Integer aValue) ;
  Storage_BaseDriver& PutCharacter(const Standard_Character aValue) ;
  Storage_BaseDriver& operator <<(const Standard_Character aValue) 
{
  return PutCharacter(aValue);
}

  Storage_BaseDriver& PutExtCharacter(const Standard_ExtCharacter aValue) ;
  Storage_BaseDriver& operator <<(const Standard_ExtCharacter aValue) 
{
  return PutExtCharacter(aValue);
}

  Storage_BaseDriver& PutInteger(const Standard_Integer aValue) ;
  Storage_BaseDriver& operator <<(const Standard_Integer aValue) 
{
  return PutInteger(aValue);
}

  Storage_BaseDriver& PutBoolean(const Standard_Boolean aValue) ;
  Storage_BaseDriver& operator <<(const Standard_Boolean aValue) 
{
  return PutBoolean(aValue);
}

  Storage_BaseDriver& PutReal(const Standard_Real aValue) ;
  Storage_BaseDriver& operator <<(const Standard_Real aValue) 
{
  return PutReal(aValue);
}

  Storage_BaseDriver& PutShortReal(const Standard_ShortReal aValue) ;
  Storage_BaseDriver& operator <<(const Standard_ShortReal aValue) 
{
  return PutShortReal(aValue);
}

  Storage_BaseDriver& GetReference(Standard_Integer& aValue) ;
  Storage_BaseDriver& GetCharacter(Standard_Character& aValue) ;
  Storage_BaseDriver& operator >>(Standard_Character& aValue) 
{
  return GetCharacter(aValue);
}

  Storage_BaseDriver& GetExtCharacter(Standard_ExtCharacter& aValue) ;
  Storage_BaseDriver& operator >>(Standard_ExtCharacter& aValue) 
{
  return GetExtCharacter(aValue);
}

  Storage_BaseDriver& GetInteger(Standard_Integer& aValue) ;
  Storage_BaseDriver& operator >>(Standard_Integer& aValue) 
{
  return GetInteger(aValue);
}

  Storage_BaseDriver& GetBoolean(Standard_Boolean& aValue) ;
  Storage_BaseDriver& operator >>(Standard_Boolean& aValue) 
{
  return GetBoolean(aValue);
}

  Storage_BaseDriver& GetReal(Standard_Real& aValue) ;
  Storage_BaseDriver& operator >>(Standard_Real& aValue) 
{
  return GetReal(aValue);
}

  Storage_BaseDriver& GetShortReal(Standard_ShortReal& aValue) ;
  Storage_BaseDriver& operator >>(Standard_ShortReal& aValue) 
{
  return GetShortReal(aValue);
}

  Storage_Error Close() ;
  void Destroy() ;
~DDF_IOStream()
{
  Destroy();
}






protected:

 // Methods PROTECTED
 // 
  void ReadLine(TCollection_AsciiString& buffer) ;
  void ReadWord(TCollection_AsciiString& buffer) ;
  void ReadExtendedLine(TCollection_ExtendedString& buffer) ;
  void WriteExtendedLine(const TCollection_ExtendedString& buffer) ;
  void ReadChar(TCollection_AsciiString& buffer,const Standard_Integer rsize) ;
  void ReadString(TCollection_AsciiString& buffer) ;
  void FlushEndOfLine() ;
  Storage_Error FindTag(const Standard_CString aTag) ;


 // Fields PROTECTED
 //


private: 

 // Methods PRIVATE
 // 
static const Standard_CString MagicNumber() ;


 // Fields PRIVATE
 //
//FSD_FStream myStream;
istream* myIStream;
ostream* myOStream;


};

#ifdef WNT
#pragma warning (default : 4275)
#endif


// other inline functions and methods (like "C++: function call" methods)
//
#endif
