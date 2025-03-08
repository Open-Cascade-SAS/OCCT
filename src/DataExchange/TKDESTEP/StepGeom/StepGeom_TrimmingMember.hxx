// Created on: 1997-03-27
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

#ifndef _StepGeom_TrimmingMember_HeaderFile
#define _StepGeom_TrimmingMember_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepData_SelectReal.hxx>

class StepGeom_TrimmingMember;
DEFINE_STANDARD_HANDLE(StepGeom_TrimmingMember, StepData_SelectReal)

//! For immediate members of TrimmingSelect, i.e. :
//! ParameterValue (a Real)
class StepGeom_TrimmingMember : public StepData_SelectReal
{

public:
  Standard_EXPORT StepGeom_TrimmingMember();

  Standard_EXPORT virtual Standard_Boolean HasName() const Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_CString Name() const Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Boolean SetName(const Standard_CString name) Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(StepGeom_TrimmingMember, StepData_SelectReal)

protected:
private:
};

#endif // _StepGeom_TrimmingMember_HeaderFile
