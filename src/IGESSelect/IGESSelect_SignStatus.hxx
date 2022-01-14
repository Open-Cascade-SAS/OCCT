// Created on: 1998-01-28
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

#ifndef _IGESSelect_SignStatus_HeaderFile
#define _IGESSelect_SignStatus_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_Signature.hxx>
#include <Standard_CString.hxx>
class Standard_Transient;
class Interface_InterfaceModel;
class TCollection_AsciiString;


class IGESSelect_SignStatus;
DEFINE_STANDARD_HANDLE(IGESSelect_SignStatus, IFSelect_Signature)

//! Gives D.E. Status under the form i,j,k,l (4 figures)
//! i for BlankStatus
//! j for SubordinateStatus
//! k for UseFlag
//! l for Hierarchy
//!
//! For matching, allowed shortcuts
//! B(Blanked) or V(Visible) are allowed instead of  i
//! I(Independant=0), P(Physically Dep.=1), L(Logically Dep.=2) or
//! D(Dependant=3) are allowed instead of  j
//! These letters must be given in their good position
//! For non-exact matching :
//! a letter (see above), no comma : only this status is checked
//! nothing or a star between commas : this status is OK
class IGESSelect_SignStatus : public IFSelect_Signature
{

public:

  
  Standard_EXPORT IGESSelect_SignStatus();
  
  //! Returns the value (see above)
  Standard_EXPORT Standard_CString Value (const Handle(Standard_Transient)& ent, const Handle(Interface_InterfaceModel)& model) const Standard_OVERRIDE;
  
  //! Performs the match rule (see above)
  Standard_EXPORT virtual Standard_Boolean Matches (const Handle(Standard_Transient)& ent, const Handle(Interface_InterfaceModel)& model, const TCollection_AsciiString& text, const Standard_Boolean exact) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IGESSelect_SignStatus,IFSelect_Signature)

protected:




private:




};







#endif // _IGESSelect_SignStatus_HeaderFile
