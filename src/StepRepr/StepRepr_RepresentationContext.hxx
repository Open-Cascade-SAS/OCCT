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

#ifndef _StepRepr_RepresentationContext_HeaderFile
#define _StepRepr_RepresentationContext_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <MMgt_TShared.hxx>
class TCollection_HAsciiString;


class StepRepr_RepresentationContext;
DEFINE_STANDARD_HANDLE(StepRepr_RepresentationContext, MMgt_TShared)


class StepRepr_RepresentationContext : public MMgt_TShared
{

public:

  
  //! Returns a RepresentationContext
  Standard_EXPORT StepRepr_RepresentationContext();
  
  Standard_EXPORT virtual void Init (const Handle(TCollection_HAsciiString)& aContextIdentifier, const Handle(TCollection_HAsciiString)& aContextType);
  
  Standard_EXPORT void SetContextIdentifier (const Handle(TCollection_HAsciiString)& aContextIdentifier);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) ContextIdentifier() const;
  
  Standard_EXPORT void SetContextType (const Handle(TCollection_HAsciiString)& aContextType);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) ContextType() const;




  DEFINE_STANDARD_RTTI(StepRepr_RepresentationContext,MMgt_TShared)

protected:




private:


  Handle(TCollection_HAsciiString) contextIdentifier;
  Handle(TCollection_HAsciiString) contextType;


};







#endif // _StepRepr_RepresentationContext_HeaderFile
