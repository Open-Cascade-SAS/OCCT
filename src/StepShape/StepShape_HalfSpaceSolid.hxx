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

#ifndef _StepShape_HalfSpaceSolid_HeaderFile
#define _StepShape_HalfSpaceSolid_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>
class StepGeom_Surface;
class TCollection_HAsciiString;


class StepShape_HalfSpaceSolid;
DEFINE_STANDARD_HANDLE(StepShape_HalfSpaceSolid, StepGeom_GeometricRepresentationItem)


class StepShape_HalfSpaceSolid : public StepGeom_GeometricRepresentationItem
{

public:

  
  //! Returns a HalfSpaceSolid
  Standard_EXPORT StepShape_HalfSpaceSolid();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(StepGeom_Surface)& aBaseSurface, const Standard_Boolean aAgreementFlag);
  
  Standard_EXPORT void SetBaseSurface (const Handle(StepGeom_Surface)& aBaseSurface);
  
  Standard_EXPORT Handle(StepGeom_Surface) BaseSurface() const;
  
  Standard_EXPORT void SetAgreementFlag (const Standard_Boolean aAgreementFlag);
  
  Standard_EXPORT Standard_Boolean AgreementFlag() const;




  DEFINE_STANDARD_RTTIEXT(StepShape_HalfSpaceSolid,StepGeom_GeometricRepresentationItem)

protected:




private:


  Handle(StepGeom_Surface) baseSurface;
  Standard_Boolean agreementFlag;


};







#endif // _StepShape_HalfSpaceSolid_HeaderFile
