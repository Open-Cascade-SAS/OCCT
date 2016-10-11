// Created on: 1996-12-11
// Created by: Robert COUBLANC
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _AIS_TypeOfAngle_HeaderFile
#define _AIS_TypeOfAngle_HeaderFile

//! Declares the type of angle.
enum AIS_TypeOfAngle
{
AIS_TOA_Interior, //!< the angle between two lines built on geometry parameters
AIS_TOA_Exterior  //!< the angle equal 2 PI minus the interior angle
};

#endif // _AIS_TypeOfAngle_HeaderFile
