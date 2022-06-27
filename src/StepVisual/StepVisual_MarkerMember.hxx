// Created on: 1997-04-01
// Created by: Christian CAILLET
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _StepVisual_MarkerMember_HeaderFile
#define _StepVisual_MarkerMember_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepData_SelectInt.hxx>
#include <Standard_Integer.hxx>
#include <StepVisual_MarkerType.hxx>


class StepVisual_MarkerMember;
DEFINE_STANDARD_HANDLE(StepVisual_MarkerMember, StepData_SelectInt)

//! Defines MarkerType as unique member of MarkerSelect
//! Works with an EnumTool
class StepVisual_MarkerMember : public StepData_SelectInt
{

public:

  
  Standard_EXPORT StepVisual_MarkerMember();
  
  Standard_EXPORT virtual Standard_Boolean HasName() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_CString Name() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Boolean SetName (const Standard_CString name) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_CString EnumText() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void SetEnumText (const Standard_Integer val, const Standard_CString text) Standard_OVERRIDE;
  
  Standard_EXPORT void SetValue (const StepVisual_MarkerType val);
  
  Standard_EXPORT StepVisual_MarkerType Value() const;




  DEFINE_STANDARD_RTTIEXT(StepVisual_MarkerMember,StepData_SelectInt)

protected:




private:




};







#endif // _StepVisual_MarkerMember_HeaderFile
