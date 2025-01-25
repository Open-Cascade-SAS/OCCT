// Created on: 1994-06-01
// Created by: Christian CAILLET
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

#ifndef _IGESSelect_UpdateCreationDate_HeaderFile
#define _IGESSelect_UpdateCreationDate_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESSelect_ModelModifier.hxx>
class IFSelect_ContextModif;
class IGESData_IGESModel;
class Interface_CopyTool;
class TCollection_AsciiString;

class IGESSelect_UpdateCreationDate;
DEFINE_STANDARD_HANDLE(IGESSelect_UpdateCreationDate, IGESSelect_ModelModifier)

//! Allows to Change the Creation Date indication in the Header
//! (Global Section) of IGES File. It is taken from the operating
//! system (time of application of the Modifier).
//! The Selection of the Modifier is not used : it simply acts as
//! a criterium to select IGES Files to touch up
class IGESSelect_UpdateCreationDate : public IGESSelect_ModelModifier
{

public:
  //! Creates an UpdateCreationDate, which uses the system Date
  Standard_EXPORT IGESSelect_UpdateCreationDate();

  //! Specific action : only <target> is used : the system Date
  //! is set to Global Section Item n0 18.
  Standard_EXPORT void Performing(IFSelect_ContextModif&            ctx,
                                  const Handle(IGESData_IGESModel)& target,
                                  Interface_CopyTool&               TC) const Standard_OVERRIDE;

  //! Returns a text which is
  //! "Update IGES Header Creation Date"
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESSelect_UpdateCreationDate, IGESSelect_ModelModifier)

protected:
private:
};

#endif // _IGESSelect_UpdateCreationDate_HeaderFile
