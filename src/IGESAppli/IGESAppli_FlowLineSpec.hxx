// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen ( Anand NATRAJAN )
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _IGESAppli_FlowLineSpec_HeaderFile
#define _IGESAppli_FlowLineSpec_HeaderFile

#include <Standard.hxx>

#include <Interface_HArray1OfHAsciiString.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;


class IGESAppli_FlowLineSpec;
DEFINE_STANDARD_HANDLE(IGESAppli_FlowLineSpec, IGESData_IGESEntity)

//! defines FlowLineSpec, Type <406> Form <14>
//! in package IGESAppli
//! Attaches one or more text strings to entities being
//! used to represent a flow line
class IGESAppli_FlowLineSpec : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESAppli_FlowLineSpec();
  
  //! This method is used to set the fields of the class
  //! FlowLineSpec
  //! - allProperties : primary flow line specification and modifiers
  Standard_EXPORT void Init (const Handle(Interface_HArray1OfHAsciiString)& allProperties);
  
  //! returns the number of property values
  Standard_EXPORT Standard_Integer NbPropertyValues() const;
  
  //! returns primary flow line specification name
  Standard_EXPORT Handle(TCollection_HAsciiString) FlowLineName() const;
  
  //! returns specified modifier element
  //! raises exception if Index <= 1 or Index > NbPropertyValues
  Standard_EXPORT Handle(TCollection_HAsciiString) Modifier (const Standard_Integer Index) const;




  DEFINE_STANDARD_RTTIEXT(IGESAppli_FlowLineSpec,IGESData_IGESEntity)

protected:




private:


  Handle(Interface_HArray1OfHAsciiString) theNameAndModifiers;


};







#endif // _IGESAppli_FlowLineSpec_HeaderFile
