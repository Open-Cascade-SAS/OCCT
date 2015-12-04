// Created on: 1997-05-21
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

#ifndef _StepData_DescrProtocol_HeaderFile
#define _StepData_DescrProtocol_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_AsciiString.hxx>
#include <StepData_FileProtocol.hxx>
#include <Standard_CString.hxx>


class StepData_DescrProtocol;
DEFINE_STANDARD_HANDLE(StepData_DescrProtocol, StepData_FileProtocol)

//! A DescrProtocol is a protocol dynamically (at execution time)
//! defined with :
//! - a list of resources (inherits FileProtocol)
//! - a list of entity descriptions
//! i.e. it can be defined with only C++ writing to initialize it
//! Its initialization must :
//! - set its schema name
//! - define its resources (which can also be other DescrProtocol)
//! - define its entity descriptions
//! - record it in the system by calling RecordLib
class StepData_DescrProtocol : public StepData_FileProtocol
{

public:

  
  Standard_EXPORT StepData_DescrProtocol();
  
  //! Defines a specific Schema Name for this Protocol
  Standard_EXPORT void SetSchemaName (const Standard_CString name);
  
  //! Records this Protocol in the service libraries, with a
  //! DescrGeneral and a DescrReadWrite
  //! Does nothing if the Protocol brings no proper description
  Standard_EXPORT void LibRecord() const;
  
  //! Returns the Schema Name attached to each class of Protocol
  //! here, returns the SchemaName set by SetSchemaName
  //! was C++ : return const
  Standard_EXPORT virtual Standard_CString SchemaName() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(StepData_DescrProtocol,StepData_FileProtocol)

protected:




private:


  TCollection_AsciiString thename;


};







#endif // _StepData_DescrProtocol_HeaderFile
