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

#ifndef _IGESSelect_ComputeStatus_HeaderFile
#define _IGESSelect_ComputeStatus_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESSelect_ModelModifier.hxx>
class IFSelect_ContextModif;
class IGESData_IGESModel;
class Interface_CopyTool;
class TCollection_AsciiString;

class IGESSelect_ComputeStatus;
DEFINE_STANDARD_HANDLE(IGESSelect_ComputeStatus, IGESSelect_ModelModifier)

//! Computes Status of IGES Entities for a whole IGESModel.
//! This concerns SubordinateStatus and UseFlag, which must have
//! some definite values according the way they are referenced.
//! (see definitions of Logical use, Physical use, etc...)
//!
//! Works by calling a BasicEditor from IGESData. Works on the
//! whole produced (target) model, because computation is global.
class IGESSelect_ComputeStatus : public IGESSelect_ModelModifier
{

public:
  //! Creates an ComputeStatus, which uses the system Date
  Standard_EXPORT IGESSelect_ComputeStatus();

  //! Specific action : it first evaluates the required values for
  //! Subordinate Status and Use Flag (in Directory Part of each
  //! IGES Entity). Then it corrects them, for the whole target.
  //! Works with a Protocol. Implementation uses BasicEditor
  Standard_EXPORT void Performing(IFSelect_ContextModif&            ctx,
                                  const Handle(IGESData_IGESModel)& target,
                                  Interface_CopyTool&               TC) const Standard_OVERRIDE;

  //! Returns a text which is
  //! "Compute Subordinate Status and Use Flag"
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESSelect_ComputeStatus, IGESSelect_ModelModifier)

protected:
private:
};

#endif // _IGESSelect_ComputeStatus_HeaderFile
