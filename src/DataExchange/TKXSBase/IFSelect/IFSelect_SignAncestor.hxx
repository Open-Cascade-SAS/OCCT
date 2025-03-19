// Created on: 1999-02-17
// Created by: Pavel DURANDIN
// Copyright (c) 1999 Matra Datavision
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

#ifndef _IFSelect_SignAncestor_HeaderFile
#define _IFSelect_SignAncestor_HeaderFile

#include <Standard.hxx>

#include <IFSelect_SignType.hxx>
class Standard_Transient;
class Interface_InterfaceModel;
class TCollection_AsciiString;

class IFSelect_SignAncestor;
DEFINE_STANDARD_HANDLE(IFSelect_SignAncestor, IFSelect_SignType)

class IFSelect_SignAncestor : public IFSelect_SignType
{

public:
  Standard_EXPORT IFSelect_SignAncestor(const Standard_Boolean nopk = Standard_False);

  Standard_EXPORT virtual Standard_Boolean Matches(const Handle(Standard_Transient)&       ent,
                                                   const Handle(Interface_InterfaceModel)& model,
                                                   const TCollection_AsciiString&          text,
                                                   const Standard_Boolean exact) const
    Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IFSelect_SignAncestor, IFSelect_SignType)

protected:
private:
};

#endif // _IFSelect_SignAncestor_HeaderFile
