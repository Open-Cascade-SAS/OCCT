// Created on: 1999-11-26
// Created by: Andrey BETENEV
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

#ifndef _StepRepr_ConfigurationDesign_HeaderFile
#define _StepRepr_ConfigurationDesign_HeaderFile

#include <Standard.hxx>

#include <StepRepr_ConfigurationDesignItem.hxx>
#include <Standard_Transient.hxx>
class StepRepr_ConfigurationItem;


class StepRepr_ConfigurationDesign;
DEFINE_STANDARD_HANDLE(StepRepr_ConfigurationDesign, Standard_Transient)

//! Representation of STEP entity ConfigurationDesign
class StepRepr_ConfigurationDesign : public Standard_Transient
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepRepr_ConfigurationDesign();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const Handle(StepRepr_ConfigurationItem)& aConfiguration, const StepRepr_ConfigurationDesignItem& aDesign);
  
  //! Returns field Configuration
  Standard_EXPORT Handle(StepRepr_ConfigurationItem) Configuration() const;
  
  //! Set field Configuration
  Standard_EXPORT void SetConfiguration (const Handle(StepRepr_ConfigurationItem)& Configuration);
  
  //! Returns field Design
  Standard_EXPORT StepRepr_ConfigurationDesignItem Design() const;
  
  //! Set field Design
  Standard_EXPORT void SetDesign (const StepRepr_ConfigurationDesignItem& Design);




  DEFINE_STANDARD_RTTIEXT(StepRepr_ConfigurationDesign,Standard_Transient)

protected:




private:


  Handle(StepRepr_ConfigurationItem) theConfiguration;
  StepRepr_ConfigurationDesignItem theDesign;


};







#endif // _StepRepr_ConfigurationDesign_HeaderFile
