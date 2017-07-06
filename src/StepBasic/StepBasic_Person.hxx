// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _StepBasic_Person_HeaderFile
#define _StepBasic_Person_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Interface_HArray1OfHAsciiString.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;


class StepBasic_Person;
DEFINE_STANDARD_HANDLE(StepBasic_Person, Standard_Transient)


class StepBasic_Person : public Standard_Transient
{

public:

  
  //! Returns a Person
  Standard_EXPORT StepBasic_Person();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aId, const Standard_Boolean hasAlastName, const Handle(TCollection_HAsciiString)& aLastName, const Standard_Boolean hasAfirstName, const Handle(TCollection_HAsciiString)& aFirstName, const Standard_Boolean hasAmiddleNames, const Handle(Interface_HArray1OfHAsciiString)& aMiddleNames, const Standard_Boolean hasAprefixTitles, const Handle(Interface_HArray1OfHAsciiString)& aPrefixTitles, const Standard_Boolean hasAsuffixTitles, const Handle(Interface_HArray1OfHAsciiString)& aSuffixTitles);
  
  Standard_EXPORT void SetId (const Handle(TCollection_HAsciiString)& aId);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) Id() const;
  
  Standard_EXPORT void SetLastName (const Handle(TCollection_HAsciiString)& aLastName);
  
  Standard_EXPORT void UnSetLastName();
  
  Standard_EXPORT Handle(TCollection_HAsciiString) LastName() const;
  
  Standard_EXPORT Standard_Boolean HasLastName() const;
  
  Standard_EXPORT void SetFirstName (const Handle(TCollection_HAsciiString)& aFirstName);
  
  Standard_EXPORT void UnSetFirstName();
  
  Standard_EXPORT Handle(TCollection_HAsciiString) FirstName() const;
  
  Standard_EXPORT Standard_Boolean HasFirstName() const;
  
  Standard_EXPORT void SetMiddleNames (const Handle(Interface_HArray1OfHAsciiString)& aMiddleNames);
  
  Standard_EXPORT void UnSetMiddleNames();
  
  Standard_EXPORT Handle(Interface_HArray1OfHAsciiString) MiddleNames() const;
  
  Standard_EXPORT Standard_Boolean HasMiddleNames() const;
  
  Standard_EXPORT Handle(TCollection_HAsciiString) MiddleNamesValue (const Standard_Integer num) const;
  
  Standard_EXPORT Standard_Integer NbMiddleNames() const;
  
  Standard_EXPORT void SetPrefixTitles (const Handle(Interface_HArray1OfHAsciiString)& aPrefixTitles);
  
  Standard_EXPORT void UnSetPrefixTitles();
  
  Standard_EXPORT Handle(Interface_HArray1OfHAsciiString) PrefixTitles() const;
  
  Standard_EXPORT Standard_Boolean HasPrefixTitles() const;
  
  Standard_EXPORT Handle(TCollection_HAsciiString) PrefixTitlesValue (const Standard_Integer num) const;
  
  Standard_EXPORT Standard_Integer NbPrefixTitles() const;
  
  Standard_EXPORT void SetSuffixTitles (const Handle(Interface_HArray1OfHAsciiString)& aSuffixTitles);
  
  Standard_EXPORT void UnSetSuffixTitles();
  
  Standard_EXPORT Handle(Interface_HArray1OfHAsciiString) SuffixTitles() const;
  
  Standard_EXPORT Standard_Boolean HasSuffixTitles() const;
  
  Standard_EXPORT Handle(TCollection_HAsciiString) SuffixTitlesValue (const Standard_Integer num) const;
  
  Standard_EXPORT Standard_Integer NbSuffixTitles() const;




  DEFINE_STANDARD_RTTIEXT(StepBasic_Person,Standard_Transient)

protected:




private:


  Handle(TCollection_HAsciiString) id;
  Handle(TCollection_HAsciiString) lastName;
  Handle(TCollection_HAsciiString) firstName;
  Handle(Interface_HArray1OfHAsciiString) middleNames;
  Handle(Interface_HArray1OfHAsciiString) prefixTitles;
  Handle(Interface_HArray1OfHAsciiString) suffixTitles;
  Standard_Boolean hasLastName;
  Standard_Boolean hasFirstName;
  Standard_Boolean hasMiddleNames;
  Standard_Boolean hasPrefixTitles;
  Standard_Boolean hasSuffixTitles;


};







#endif // _StepBasic_Person_HeaderFile
