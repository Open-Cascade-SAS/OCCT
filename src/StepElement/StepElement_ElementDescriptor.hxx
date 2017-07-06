// Created on: 2002-12-12
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

#ifndef _StepElement_ElementDescriptor_HeaderFile
#define _StepElement_ElementDescriptor_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepElement_ElementOrder.hxx>
#include <Standard_Transient.hxx>
class TCollection_HAsciiString;


class StepElement_ElementDescriptor;
DEFINE_STANDARD_HANDLE(StepElement_ElementDescriptor, Standard_Transient)

//! Representation of STEP entity ElementDescriptor
class StepElement_ElementDescriptor : public Standard_Transient
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepElement_ElementDescriptor();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const StepElement_ElementOrder aTopologyOrder, const Handle(TCollection_HAsciiString)& aDescription);
  
  //! Returns field TopologyOrder
  Standard_EXPORT StepElement_ElementOrder TopologyOrder() const;
  
  //! Set field TopologyOrder
  Standard_EXPORT void SetTopologyOrder (const StepElement_ElementOrder TopologyOrder);
  
  //! Returns field Description
  Standard_EXPORT Handle(TCollection_HAsciiString) Description() const;
  
  //! Set field Description
  Standard_EXPORT void SetDescription (const Handle(TCollection_HAsciiString)& Description);




  DEFINE_STANDARD_RTTIEXT(StepElement_ElementDescriptor,Standard_Transient)

protected:




private:


  StepElement_ElementOrder theTopologyOrder;
  Handle(TCollection_HAsciiString) theDescription;


};







#endif // _StepElement_ElementDescriptor_HeaderFile
