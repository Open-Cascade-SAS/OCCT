// Created on: 1993-05-05
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

#ifndef _IGESGraph_Protocol_HeaderFile
#define _IGESGraph_Protocol_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESData_Protocol.hxx>
#include <Standard_Integer.hxx>
class Interface_Protocol;

class IGESGraph_Protocol;
DEFINE_STANDARD_HANDLE(IGESGraph_Protocol, IGESData_Protocol)

//! Description of Protocol for IGESGraph
class IGESGraph_Protocol : public IGESData_Protocol
{

public:
  Standard_EXPORT IGESGraph_Protocol();

  //! Gives the count of Resource Protocol. Here, one
  //! (Protocol from IGESBasic)
  Standard_EXPORT virtual Standard_Integer NbResources() const Standard_OVERRIDE;

  //! Returns a Resource, given a rank.
  Standard_EXPORT virtual Handle(Interface_Protocol) Resource(const Standard_Integer num) const
    Standard_OVERRIDE;

  //! Returns a Case Number, specific of each recognized Type
  //! This Case Number is then used in Libraries : the various
  //! Modules attached to this class of Protocol must use them
  //! in accordance (for a given value of TypeNumber, they must
  //! consider the same Type as the Protocol defines)
  Standard_EXPORT virtual Standard_Integer TypeNumber(const Handle(Standard_Type)& atype) const
    Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESGraph_Protocol, IGESData_Protocol)

protected:
private:
};

#endif // _IGESGraph_Protocol_HeaderFile
