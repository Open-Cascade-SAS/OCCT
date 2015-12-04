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

#ifndef _CDM_MessageDriver_HeaderFile
#define _CDM_MessageDriver_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <Standard_ExtString.hxx>


class CDM_MessageDriver;
DEFINE_STANDARD_HANDLE(CDM_MessageDriver, Standard_Transient)


class CDM_MessageDriver : public Standard_Transient
{

public:

  
  Standard_EXPORT virtual void Write (const Standard_ExtString aString) = 0;




  DEFINE_STANDARD_RTTIEXT(CDM_MessageDriver,Standard_Transient)

protected:




private:




};







#endif // _CDM_MessageDriver_HeaderFile
