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

#ifndef _IGESSelect_SetVersion5_HeaderFile
#define _IGESSelect_SetVersion5_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESSelect_ModelModifier.hxx>
class IFSelect_ContextModif;
class IGESData_IGESModel;
class Interface_CopyTool;
class TCollection_AsciiString;

class IGESSelect_SetVersion5;
DEFINE_STANDARD_HANDLE(IGESSelect_SetVersion5, IGESSelect_ModelModifier)

//! Sets IGES Version (coded in global parameter 23) to be at least
//! IGES 5.1 . If it is older, it is set to IGES 5.1, and
//! LastChangeDate (new Global n0 25) is added (current time)
//! Else, it does nothing (i.e. changes neither IGES Version nor
//! LastChangeDate)
class IGESSelect_SetVersion5 : public IGESSelect_ModelModifier
{

public:
  //! Creates an SetVersion5, which uses the system Date for Last
  //! Change Date
  Standard_EXPORT IGESSelect_SetVersion5();

  //! Specific action : only <target> is used : IGES Version (coded)
  //! is upgraded to 5.1 if it is older, and it this case the new
  //! global parameter 25 (LastChangeDate) is set to current time
  Standard_EXPORT void Performing(IFSelect_ContextModif&            ctx,
                                  const Handle(IGESData_IGESModel)& target,
                                  Interface_CopyTool&               TC) const Standard_OVERRIDE;

  //! Returns a text which is
  //! "Update IGES Version to 5.1"
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESSelect_SetVersion5, IGESSelect_ModelModifier)

protected:
private:
};

#endif // _IGESSelect_SetVersion5_HeaderFile
