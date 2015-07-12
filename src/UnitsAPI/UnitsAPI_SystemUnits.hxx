// Created on: 1995-11-15
// Created by: Jean-Louis Frenkel <rmi@pernox>
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

#ifndef _UnitsAPI_SystemUnits_HeaderFile
#define _UnitsAPI_SystemUnits_HeaderFile

//! Identifies unit systems which may be defined as a
//! basis system in the user's session:
//! -   UnitsAPI_DEFAULT : default system (this is the SI system)
//! -   UnitsAPI_SI : the SI unit system
//! -   UnitsAPI_MDTV : the MDTV unit system; it
//! is equivalent to the SI unit system but the
//! length unit and all its derivatives use
//! millimeters instead of meters.
//! Use the function SetLocalSystem to set up one
//! of these unit systems as working environment.
enum UnitsAPI_SystemUnits
{
UnitsAPI_DEFAULT,
UnitsAPI_SI,
UnitsAPI_MDTV
};

#endif // _UnitsAPI_SystemUnits_HeaderFile
