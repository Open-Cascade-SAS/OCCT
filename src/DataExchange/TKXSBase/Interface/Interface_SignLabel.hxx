// Created on: 1998-05-20
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

#ifndef _Interface_SignLabel_HeaderFile
#define _Interface_SignLabel_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <MoniTool_SignText.hxx>
class TCollection_AsciiString;
class Standard_Transient;

//! Signature to give the Label from the Model
class Interface_SignLabel : public MoniTool_SignText
{

public:
  Standard_EXPORT Interface_SignLabel();

  //! Returns "Entity Label"
  Standard_EXPORT const char* Name() const override;

  //! Considers context as an InterfaceModel and returns the Label
  //! computed by it
  Standard_EXPORT TCollection_AsciiString
    Text(const occ::handle<Standard_Transient>& ent,
         const occ::handle<Standard_Transient>& context) const override;

  DEFINE_STANDARD_RTTIEXT(Interface_SignLabel, MoniTool_SignText)
};

#endif // _Interface_SignLabel_HeaderFile
