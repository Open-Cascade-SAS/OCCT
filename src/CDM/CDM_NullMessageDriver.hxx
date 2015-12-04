// Created on: 1998-10-29
// Created by: Jean-Louis Frenkel
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

#ifndef _CDM_NullMessageDriver_HeaderFile
#define _CDM_NullMessageDriver_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <CDM_MessageDriver.hxx>
#include <Standard_ExtString.hxx>


class CDM_NullMessageDriver;
DEFINE_STANDARD_HANDLE(CDM_NullMessageDriver, CDM_MessageDriver)

//! a MessageDriver that writes nowhere.
class CDM_NullMessageDriver : public CDM_MessageDriver
{

public:

  
  Standard_EXPORT CDM_NullMessageDriver();
  
  Standard_EXPORT void Write (const Standard_ExtString aString) Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(CDM_NullMessageDriver,CDM_MessageDriver)

protected:




private:




};







#endif // _CDM_NullMessageDriver_HeaderFile
