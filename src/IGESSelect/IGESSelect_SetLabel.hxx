// Created on: 1999-02-26
// Created by: Christian CAILLET
// Copyright (c) 1999 Matra Datavision
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

#ifndef _IGESSelect_SetLabel_HeaderFile
#define _IGESSelect_SetLabel_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <IGESSelect_ModelModifier.hxx>
class IFSelect_ContextModif;
class IGESData_IGESModel;
class Interface_CopyTool;
class TCollection_AsciiString;


class IGESSelect_SetLabel;
DEFINE_STANDARD_HANDLE(IGESSelect_SetLabel, IGESSelect_ModelModifier)

//! Sets/Clears Short Label of Entities, those designated by the
//! Selection. No Selection means all the file
//!
//! May enforce, else it sets only if no label is yet set
//! Mode : 0 to clear (always enforced)
//! 1 to set label to DE number (changes it if already set)
class IGESSelect_SetLabel : public IGESSelect_ModelModifier
{

public:

  
  //! Creates a SetLabel for IGESEntity
  //! Mode : see Purpose of the class
  Standard_EXPORT IGESSelect_SetLabel(const Standard_Integer mode, const Standard_Boolean enforce);
  
  //! Specific action : Sets or Clears the Label
  Standard_EXPORT void Performing (IFSelect_ContextModif& ctx, const Handle(IGESData_IGESModel)& target, Interface_CopyTool& TC) const Standard_OVERRIDE;
  
  //! Returns a text which is
  //! "Clear Short Label"  or  "Set Label to DE"
  //! With possible additional information " (enforced)"
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IGESSelect_SetLabel,IGESSelect_ModelModifier)

protected:




private:


  Standard_Integer themode;
  Standard_Boolean theforce;


};







#endif // _IGESSelect_SetLabel_HeaderFile
