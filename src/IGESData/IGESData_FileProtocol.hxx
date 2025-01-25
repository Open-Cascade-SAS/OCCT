// Created on: 1993-10-26
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

#ifndef _IGESData_FileProtocol_HeaderFile
#define _IGESData_FileProtocol_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESData_Protocol.hxx>
#include <Standard_Integer.hxx>
class Interface_Protocol;

class IGESData_FileProtocol;
DEFINE_STANDARD_HANDLE(IGESData_FileProtocol, IGESData_Protocol)

//! This class allows to define complex protocols, in order to
//! treat various sub-sets (or the complete set) of the IGES Norm,
//! such as Solid + Draw (which are normally independent), etc...
//! While it inherits Protocol from IGESData, it admits UndefinedEntity too
class IGESData_FileProtocol : public IGESData_Protocol
{

public:
  //! Returns an empty FileProtocol
  Standard_EXPORT IGESData_FileProtocol();

  //! Adds a resource
  Standard_EXPORT void Add(const Handle(IGESData_Protocol)& protocol);

  //! Gives the count of Resources : the count of Added Protocols
  Standard_EXPORT virtual Standard_Integer NbResources() const Standard_OVERRIDE;

  //! Returns a Resource, given a rank (rank of call to Add)
  Standard_EXPORT virtual Handle(Interface_Protocol) Resource(const Standard_Integer num) const
    Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESData_FileProtocol, IGESData_Protocol)

private:
  Handle(IGESData_Protocol)     theresource;
  Handle(IGESData_FileProtocol) thenext;
};

#endif // _IGESData_FileProtocol_HeaderFile
