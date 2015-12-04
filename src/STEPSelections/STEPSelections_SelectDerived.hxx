// Created on: 1999-02-18
// Created by: Pavel DURANDIN
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

#ifndef _STEPSelections_SelectDerived_HeaderFile
#define _STEPSelections_SelectDerived_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepSelect_StepType.hxx>
#include <Standard_Boolean.hxx>
class Standard_Transient;
class Interface_InterfaceModel;
class TCollection_AsciiString;


class STEPSelections_SelectDerived;
DEFINE_STANDARD_HANDLE(STEPSelections_SelectDerived, StepSelect_StepType)


class STEPSelections_SelectDerived : public StepSelect_StepType
{

public:

  
  Standard_EXPORT STEPSelections_SelectDerived();
  
  Standard_EXPORT virtual Standard_Boolean Matches (const Handle(Standard_Transient)& ent, const Handle(Interface_InterfaceModel)& model, const TCollection_AsciiString& text, const Standard_Boolean exact) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(STEPSelections_SelectDerived,StepSelect_StepType)

protected:




private:




};







#endif // _STEPSelections_SelectDerived_HeaderFile
