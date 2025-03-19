// Created on: 1994-06-01
// Created by: Modelistation
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

#ifndef _IGESSelect_SetGlobalParameter_HeaderFile
#define _IGESSelect_SetGlobalParameter_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <IGESSelect_ModelModifier.hxx>
class TCollection_HAsciiString;
class IFSelect_ContextModif;
class IGESData_IGESModel;
class Interface_CopyTool;
class TCollection_AsciiString;

class IGESSelect_SetGlobalParameter;
DEFINE_STANDARD_HANDLE(IGESSelect_SetGlobalParameter, IGESSelect_ModelModifier)

//! Sets a Global (Header) Parameter to a new value, directly given
//! Controls the form of the parameter (Integer, Real, String
//! with such or such form), but not the consistence of the new
//! value regarding the rest of the file.
//!
//! The new value is given under the form of a HAsciiString, even
//! for Integer or Real values. For String values, Hollerith forms
//! are accepted but not mandatory
//! Warning : a Null (not set) value is not accepted. For an empty string,
//! give a Text Parameter which is empty
class IGESSelect_SetGlobalParameter : public IGESSelect_ModelModifier
{

public:
  //! Creates an SetGlobalParameter, to be applied on Global
  //! Parameter <numpar>
  Standard_EXPORT IGESSelect_SetGlobalParameter(const Standard_Integer numpar);

  //! Returns the global parameter number to which this modifiers
  //! applies
  Standard_EXPORT Standard_Integer GlobalNumber() const;

  //! Sets a Text Parameter for the new value
  Standard_EXPORT void SetValue(const Handle(TCollection_HAsciiString)& text);

  //! Returns the value to set to the global parameter (Text Param)
  Standard_EXPORT Handle(TCollection_HAsciiString) Value() const;

  //! Specific action : only <target> is used : the form of the new
  //! value is checked regarding the parameter number (given at
  //! creation time).
  Standard_EXPORT void Performing(IFSelect_ContextModif&            ctx,
                                  const Handle(IGESData_IGESModel)& target,
                                  Interface_CopyTool&               TC) const Standard_OVERRIDE;

  //! Returns a text which is
  //! "Sets Global Parameter <numpar> to <new value>"
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESSelect_SetGlobalParameter, IGESSelect_ModelModifier)

protected:
private:
  Standard_Integer                 thenum;
  Handle(TCollection_HAsciiString) theval;
};

#endif // _IGESSelect_SetGlobalParameter_HeaderFile
