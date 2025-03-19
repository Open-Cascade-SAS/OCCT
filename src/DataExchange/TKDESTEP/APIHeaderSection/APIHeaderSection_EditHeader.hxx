// Created on: 1998-07-08
// Created by: Christian CAILLET
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

#ifndef _APIHeaderSection_EditHeader_HeaderFile
#define _APIHeaderSection_EditHeader_HeaderFile

#include <Standard.hxx>

#include <IFSelect_Editor.hxx>
#include <Standard_Integer.hxx>
class TCollection_AsciiString;
class IFSelect_EditForm;
class TCollection_HAsciiString;
class Standard_Transient;
class Interface_InterfaceModel;

class APIHeaderSection_EditHeader;
DEFINE_STANDARD_HANDLE(APIHeaderSection_EditHeader, IFSelect_Editor)

class APIHeaderSection_EditHeader : public IFSelect_Editor
{

public:
  Standard_EXPORT APIHeaderSection_EditHeader();

  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;

  Standard_EXPORT Standard_Boolean
    Recognize(const Handle(IFSelect_EditForm)& form) const Standard_OVERRIDE;

  Standard_EXPORT Handle(TCollection_HAsciiString) StringValue(
    const Handle(IFSelect_EditForm)& form,
    const Standard_Integer           num) const Standard_OVERRIDE;

  Standard_EXPORT Standard_Boolean
    Apply(const Handle(IFSelect_EditForm)&        form,
          const Handle(Standard_Transient)&       ent,
          const Handle(Interface_InterfaceModel)& model) const Standard_OVERRIDE;

  Standard_EXPORT Standard_Boolean
    Load(const Handle(IFSelect_EditForm)&        form,
         const Handle(Standard_Transient)&       ent,
         const Handle(Interface_InterfaceModel)& model) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(APIHeaderSection_EditHeader, IFSelect_Editor)

protected:
private:
};

#endif // _APIHeaderSection_EditHeader_HeaderFile
