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

#ifndef _HeaderSection_FileDescription_HeaderFile
#define _HeaderSection_FileDescription_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Interface_HArray1OfHAsciiString.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;


class HeaderSection_FileDescription;
DEFINE_STANDARD_HANDLE(HeaderSection_FileDescription, Standard_Transient)


class HeaderSection_FileDescription : public Standard_Transient
{

public:

  
  //! Returns a FileDescription
  Standard_EXPORT HeaderSection_FileDescription();
  
  Standard_EXPORT void Init (const Handle(Interface_HArray1OfHAsciiString)& aDescription, const Handle(TCollection_HAsciiString)& aImplementationLevel);
  
  Standard_EXPORT void SetDescription (const Handle(Interface_HArray1OfHAsciiString)& aDescription);
  
  Standard_EXPORT Handle(Interface_HArray1OfHAsciiString) Description() const;
  
  Standard_EXPORT Handle(TCollection_HAsciiString) DescriptionValue (const Standard_Integer num) const;
  
  Standard_EXPORT Standard_Integer NbDescription() const;
  
  Standard_EXPORT void SetImplementationLevel (const Handle(TCollection_HAsciiString)& aImplementationLevel);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) ImplementationLevel() const;




  DEFINE_STANDARD_RTTIEXT(HeaderSection_FileDescription,Standard_Transient)

protected:




private:


  Handle(Interface_HArray1OfHAsciiString) description;
  Handle(TCollection_HAsciiString) implementationLevel;


};







#endif // _HeaderSection_FileDescription_HeaderFile
