// Created on: 1993-11-04
// Created by: Christian CAILLET
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _IFSelect_BasicDumper_HeaderFile
#define _IFSelect_BasicDumper_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_SessionDumper.hxx>
class IFSelect_SessionFile;
class Standard_Transient;
class TCollection_AsciiString;

class IFSelect_BasicDumper;
DEFINE_STANDARD_HANDLE(IFSelect_BasicDumper, IFSelect_SessionDumper)

//! BasicDumper takes into account, for SessionFile, all the
//! classes defined in the package IFSelect : Selections,
//! Dispatches (there is no Modifier)
class IFSelect_BasicDumper : public IFSelect_SessionDumper
{

public:
  //! Creates a BasicDumper and puts it into the Library of Dumper
  Standard_EXPORT IFSelect_BasicDumper();

  //! Write the Own Parameters of Types defined in package IFSelect
  //! Returns True if <item> has been processed, False else
  Standard_EXPORT Standard_Boolean
    WriteOwn(IFSelect_SessionFile&             file,
             const Handle(Standard_Transient)& item) const Standard_OVERRIDE;

  //! Recognizes and Read Own Parameters for Types of package
  //! IFSelect. Returns True if done and <item> created, False else
  Standard_EXPORT Standard_Boolean
    ReadOwn(IFSelect_SessionFile&          file,
            const TCollection_AsciiString& type,
            Handle(Standard_Transient)&    item) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IFSelect_BasicDumper, IFSelect_SessionDumper)

protected:
private:
};

#endif // _IFSelect_BasicDumper_HeaderFile
