// Created on: 1998-06-30
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

#ifndef _StepBasic_DocumentType_HeaderFile
#define _StepBasic_DocumentType_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class TCollection_HAsciiString;


class StepBasic_DocumentType;
DEFINE_STANDARD_HANDLE(StepBasic_DocumentType, Standard_Transient)


class StepBasic_DocumentType : public Standard_Transient
{

public:

  
  Standard_EXPORT StepBasic_DocumentType();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& apdt);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) ProductDataType() const;
  
  Standard_EXPORT void SetProductDataType (const Handle(TCollection_HAsciiString)& apdt);




  DEFINE_STANDARD_RTTIEXT(StepBasic_DocumentType,Standard_Transient)

protected:




private:


  Handle(TCollection_HAsciiString) thepdt;


};







#endif // _StepBasic_DocumentType_HeaderFile
