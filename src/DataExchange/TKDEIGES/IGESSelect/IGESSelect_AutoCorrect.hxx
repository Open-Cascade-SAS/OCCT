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

#ifndef _IGESSelect_AutoCorrect_HeaderFile
#define _IGESSelect_AutoCorrect_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESSelect_ModelModifier.hxx>
class IFSelect_ContextModif;
class IGESData_IGESModel;
class Interface_CopyTool;
class TCollection_AsciiString;

class IGESSelect_AutoCorrect;
DEFINE_STANDARD_HANDLE(IGESSelect_AutoCorrect, IGESSelect_ModelModifier)

//! Does the absolutely effective corrections on IGES Entity.
//! That is to say : regarding the norm in details, some values
//! have mandatory values, or set of values with constraints.
//! When such values/constraints are univoque, they can be forced.
//! Also nullifies items of Directory Part, Associativities, and
//! Properties, which are not (or not longer) in <target> Model.
//!
//! Works by calling a BasicEditor from IGESData
//! Works with the specific IGES Services : DirChecker which
//! allows to correct data in "Directory Part" of Entities (such
//! as required values for status, or references to be null), and
//! the specific IGES service OwnCorrect, which is specialised for
//! each type of entity.
//!
//! Remark : this does not comprise the computation of use flag or
//! subordinate status according references, which is made by
//! the ModelModifier class ComputeStatus.
//!
//! The Input Selection, when present, designates the entities to
//! be corrected. If it is not present, all the entities of the
//! model are corrected.
class IGESSelect_AutoCorrect : public IGESSelect_ModelModifier
{

public:
  //! Creates an AutoCorrect.
  Standard_EXPORT IGESSelect_AutoCorrect();

  //! Specific action : corrects entities when it is absolutely
  //! obvious, i.e. non equivoque (by DirChecker and specific
  //! service OwnCorrect) : works with a protocol.
  Standard_EXPORT void Performing(IFSelect_ContextModif&            ctx,
                                  const Handle(IGESData_IGESModel)& target,
                                  Interface_CopyTool&               TC) const Standard_OVERRIDE;

  //! Returns a text which is
  //! "Auto-correction of IGES Entities"
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESSelect_AutoCorrect, IGESSelect_ModelModifier)

protected:
private:
};

#endif // _IGESSelect_AutoCorrect_HeaderFile
