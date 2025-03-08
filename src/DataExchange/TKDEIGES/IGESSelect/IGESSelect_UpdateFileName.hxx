// Created on: 1995-02-23
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

#ifndef _IGESSelect_UpdateFileName_HeaderFile
#define _IGESSelect_UpdateFileName_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESSelect_ModelModifier.hxx>
class IFSelect_ContextModif;
class IGESData_IGESModel;
class Interface_CopyTool;
class TCollection_AsciiString;

class IGESSelect_UpdateFileName;
DEFINE_STANDARD_HANDLE(IGESSelect_UpdateFileName, IGESSelect_ModelModifier)

//! Sets the File Name in Header to be the actual name of the file
//! If new file name is unknown, the former one is kept
//! Remark : this works well only when it is Applied and send time
//! If it is run immediately, new file name is unknown and nothing
//! is done
//! The Selection of the Modifier is not used : it simply acts as
//! a criterium to select IGES Files to touch up
class IGESSelect_UpdateFileName : public IGESSelect_ModelModifier
{

public:
  //! Creates an UpdateFileName, which uses the system Date
  Standard_EXPORT IGESSelect_UpdateFileName();

  //! Specific action : only <target> is used : the system Date
  //! is set to Global Section Item n0 18.
  Standard_EXPORT void Performing(IFSelect_ContextModif&            ctx,
                                  const Handle(IGESData_IGESModel)& target,
                                  Interface_CopyTool&               TC) const Standard_OVERRIDE;

  //! Returns a text which is
  //! "Updates IGES File Name to new current one"
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESSelect_UpdateFileName, IGESSelect_ModelModifier)

protected:
private:
};

#endif // _IGESSelect_UpdateFileName_HeaderFile
