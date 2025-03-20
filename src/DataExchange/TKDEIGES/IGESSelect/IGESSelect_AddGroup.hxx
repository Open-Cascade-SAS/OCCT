// Created on: 1995-03-02
// Created by: Christian CAILLET
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

#ifndef _IGESSelect_AddGroup_HeaderFile
#define _IGESSelect_AddGroup_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESSelect_ModelModifier.hxx>
class IFSelect_ContextModif;
class IGESData_IGESModel;
class Interface_CopyTool;
class TCollection_AsciiString;

class IGESSelect_AddGroup;
DEFINE_STANDARD_HANDLE(IGESSelect_AddGroup, IGESSelect_ModelModifier)

//! Adds a Group to contain the entities designated by the
//! Selection. If no Selection is given, nothing is done
class IGESSelect_AddGroup : public IGESSelect_ModelModifier
{

public:
  //! Creates an AddGroup
  Standard_EXPORT IGESSelect_AddGroup();

  //! Specific action : Adds a new group
  Standard_EXPORT void Performing(IFSelect_ContextModif&            ctx,
                                  const Handle(IGESData_IGESModel)& target,
                                  Interface_CopyTool&               TC) const Standard_OVERRIDE;

  //! Returns a text which is
  //! "Add Group"
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESSelect_AddGroup, IGESSelect_ModelModifier)

protected:
private:
};

#endif // _IGESSelect_AddGroup_HeaderFile
