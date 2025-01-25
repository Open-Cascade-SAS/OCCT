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

#ifndef _IGESSelect_RebuildGroups_HeaderFile
#define _IGESSelect_RebuildGroups_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESSelect_ModelModifier.hxx>
class IFSelect_ContextModif;
class IGESData_IGESModel;
class Interface_CopyTool;
class TCollection_AsciiString;

class IGESSelect_RebuildGroups;
DEFINE_STANDARD_HANDLE(IGESSelect_RebuildGroups, IGESSelect_ModelModifier)

//! Rebuilds Groups which were bypassed to produce new models.
//! If a set of entities, all put into a same IGESModel, were
//! part of a same Group in the starting Model, this Modifier
//! rebuilds the original group, but only with the transferred
//! entities. The distinctions (Ordered or not, "WithoutBackP"
//! or not) are renewed, also the name of the group.
//!
//! If the Input Selection is present, tries to rebuild groups
//! only for the selected entities. Else, tries to rebuild
//! groups for all the transferred entities.
class IGESSelect_RebuildGroups : public IGESSelect_ModelModifier
{

public:
  //! Creates an RebuildGroups, which uses the system Date
  Standard_EXPORT IGESSelect_RebuildGroups();

  //! Specific action : Rebuilds the original groups
  Standard_EXPORT void Performing(IFSelect_ContextModif&            ctx,
                                  const Handle(IGESData_IGESModel)& target,
                                  Interface_CopyTool&               TC) const Standard_OVERRIDE;

  //! Returns a text which is
  //! "Rebuild Groups"
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESSelect_RebuildGroups, IGESSelect_ModelModifier)

protected:
private:
};

#endif // _IGESSelect_RebuildGroups_HeaderFile
