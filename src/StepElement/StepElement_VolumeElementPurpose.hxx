// Created on: 2002-12-10
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _StepElement_VolumeElementPurpose_HeaderFile
#define _StepElement_VolumeElementPurpose_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
#include <StepElement_EnumeratedVolumeElementPurpose.hxx>
class Standard_Transient;
class StepData_SelectMember;
class TCollection_HAsciiString;


//! Representation of STEP SELECT type VolumeElementPurpose
class StepElement_VolumeElementPurpose  : public StepData_SelectType
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor
  Standard_EXPORT StepElement_VolumeElementPurpose();
  
  //! Recognizes a kind of VolumeElementPurpose select type
  //! return 0
  Standard_EXPORT Standard_Integer CaseNum (const Handle(Standard_Transient)& ent) const Standard_OVERRIDE;
  
  //! Recognizes a items of select member VolumeElementPurposeMember
  //! 1 -> EnumeratedVolumeElementPurpose
  //! 2 -> ApplicationDefinedElementPurpose
  //! 0 else
  Standard_EXPORT virtual Standard_Integer CaseMem (const Handle(StepData_SelectMember)& ent) const Standard_OVERRIDE;
  
  //! Returns a new select member the type VolumeElementPurposeMember
  Standard_EXPORT virtual Handle(StepData_SelectMember) NewMember() const Standard_OVERRIDE;
  
  //! Set Value for EnumeratedVolumeElementPurpose
  Standard_EXPORT void SetEnumeratedVolumeElementPurpose (const StepElement_EnumeratedVolumeElementPurpose aVal);
  
  //! Returns Value as EnumeratedVolumeElementPurpose (or Null if another type)
  Standard_EXPORT StepElement_EnumeratedVolumeElementPurpose EnumeratedVolumeElementPurpose() const;
  
  //! Set Value for ApplicationDefinedElementPurpose
  Standard_EXPORT void SetApplicationDefinedElementPurpose (const Handle(TCollection_HAsciiString)& aVal);
  
  //! Returns Value as ApplicationDefinedElementPurpose (or Null if another type)
  Standard_EXPORT Handle(TCollection_HAsciiString) ApplicationDefinedElementPurpose() const;




protected:





private:





};







#endif // _StepElement_VolumeElementPurpose_HeaderFile
