// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
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

#ifndef _StepRepr_RepresentationItem_HeaderFile
#define _StepRepr_RepresentationItem_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <MMgt_TShared.hxx>
class TCollection_HAsciiString;


class StepRepr_RepresentationItem;
DEFINE_STANDARD_HANDLE(StepRepr_RepresentationItem, MMgt_TShared)


class StepRepr_RepresentationItem : public MMgt_TShared
{

public:

  
  //! Returns a RepresentationItem
  Standard_EXPORT StepRepr_RepresentationItem();
  
  Standard_EXPORT virtual void Init (const Handle(TCollection_HAsciiString)& aName);
  
  Standard_EXPORT void SetName (const Handle(TCollection_HAsciiString)& aName);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) Name() const;




  DEFINE_STANDARD_RTTI(StepRepr_RepresentationItem,MMgt_TShared)

protected:




private:


  Handle(TCollection_HAsciiString) name;


};







#endif // _StepRepr_RepresentationItem_HeaderFile
