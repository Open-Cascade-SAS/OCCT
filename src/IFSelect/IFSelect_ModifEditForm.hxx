// Created on: 1998-02-27
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

#ifndef _IFSelect_ModifEditForm_HeaderFile
#define _IFSelect_ModifEditForm_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_Modifier.hxx>
class IFSelect_EditForm;
class IFSelect_ContextModif;
class Interface_InterfaceModel;
class Interface_Protocol;
class Interface_CopyTool;
class TCollection_AsciiString;

class IFSelect_ModifEditForm;
DEFINE_STANDARD_HANDLE(IFSelect_ModifEditForm, IFSelect_Modifier)

//! This modifier applies an EditForm on the entities selected
class IFSelect_ModifEditForm : public IFSelect_Modifier
{

public:
  //! Creates a ModifEditForm. It may not change the graph
  Standard_EXPORT IFSelect_ModifEditForm(const Handle(IFSelect_EditForm)& editform);

  //! Returns the EditForm
  Standard_EXPORT Handle(IFSelect_EditForm) EditForm() const;

  //! Acts by applying an EditForm to entities, selected or all model
  Standard_EXPORT void Perform(IFSelect_ContextModif&                  ctx,
                               const Handle(Interface_InterfaceModel)& target,
                               const Handle(Interface_Protocol)&       protocol,
                               Interface_CopyTool&                     TC) const Standard_OVERRIDE;

  //! Returns Label as "Apply EditForm <+ label of EditForm>"
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IFSelect_ModifEditForm, IFSelect_Modifier)

protected:
private:
  Handle(IFSelect_EditForm) theedit;
};

#endif // _IFSelect_ModifEditForm_HeaderFile
