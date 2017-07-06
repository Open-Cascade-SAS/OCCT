// Created on: 1994-06-16
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _HeaderSection_FileName_HeaderFile
#define _HeaderSection_FileName_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Interface_HArray1OfHAsciiString.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;


class HeaderSection_FileName;
DEFINE_STANDARD_HANDLE(HeaderSection_FileName, Standard_Transient)


class HeaderSection_FileName : public Standard_Transient
{

public:

  
  //! Returns a FileName
  Standard_EXPORT HeaderSection_FileName();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(TCollection_HAsciiString)& aTimeStamp, const Handle(Interface_HArray1OfHAsciiString)& aAuthor, const Handle(Interface_HArray1OfHAsciiString)& aOrganization, const Handle(TCollection_HAsciiString)& aPreprocessorVersion, const Handle(TCollection_HAsciiString)& aOriginatingSystem, const Handle(TCollection_HAsciiString)& aAuthorisation);
  
  Standard_EXPORT void SetName (const Handle(TCollection_HAsciiString)& aName);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) Name() const;
  
  Standard_EXPORT void SetTimeStamp (const Handle(TCollection_HAsciiString)& aTimeStamp);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) TimeStamp() const;
  
  Standard_EXPORT void SetAuthor (const Handle(Interface_HArray1OfHAsciiString)& aAuthor);
  
  Standard_EXPORT Handle(Interface_HArray1OfHAsciiString) Author() const;
  
  Standard_EXPORT Handle(TCollection_HAsciiString) AuthorValue (const Standard_Integer num) const;
  
  Standard_EXPORT Standard_Integer NbAuthor() const;
  
  Standard_EXPORT void SetOrganization (const Handle(Interface_HArray1OfHAsciiString)& aOrganization);
  
  Standard_EXPORT Handle(Interface_HArray1OfHAsciiString) Organization() const;
  
  Standard_EXPORT Handle(TCollection_HAsciiString) OrganizationValue (const Standard_Integer num) const;
  
  Standard_EXPORT Standard_Integer NbOrganization() const;
  
  Standard_EXPORT void SetPreprocessorVersion (const Handle(TCollection_HAsciiString)& aPreprocessorVersion);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) PreprocessorVersion() const;
  
  Standard_EXPORT void SetOriginatingSystem (const Handle(TCollection_HAsciiString)& aOriginatingSystem);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) OriginatingSystem() const;
  
  Standard_EXPORT void SetAuthorisation (const Handle(TCollection_HAsciiString)& aAuthorisation);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) Authorisation() const;




  DEFINE_STANDARD_RTTIEXT(HeaderSection_FileName,Standard_Transient)

protected:




private:


  Handle(TCollection_HAsciiString) name;
  Handle(TCollection_HAsciiString) timeStamp;
  Handle(Interface_HArray1OfHAsciiString) author;
  Handle(Interface_HArray1OfHAsciiString) organization;
  Handle(TCollection_HAsciiString) preprocessorVersion;
  Handle(TCollection_HAsciiString) originatingSystem;
  Handle(TCollection_HAsciiString) authorisation;


};







#endif // _HeaderSection_FileName_HeaderFile
