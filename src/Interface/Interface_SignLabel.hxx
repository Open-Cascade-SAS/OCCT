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
#include <Standard_CString.hxx>
class TCollection_AsciiString;
class Standard_Transient;


class Interface_SignLabel;
DEFINE_STANDARD_HANDLE(Interface_SignLabel, MoniTool_SignText)

//! Signature to give the Label from the Model
class Interface_SignLabel : public MoniTool_SignText
{

public:

  
  Standard_EXPORT Interface_SignLabel();
  
  //! Returns "Entity Label"
  Standard_EXPORT Standard_CString Name() const Standard_OVERRIDE;
  
  //! Considers context as an InterfaceModel and returns the Label
  //! computed by it
  Standard_EXPORT TCollection_AsciiString Text (const Handle(Standard_Transient)& ent, const Handle(Standard_Transient)& context) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(Interface_SignLabel,MoniTool_SignText)

protected:




private:




};







#endif // _Interface_SignLabel_HeaderFile
