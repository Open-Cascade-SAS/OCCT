// Created on: 1993-11-09
// Created by: Laurent BOURESCHE
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

#ifndef _ChFi3d_FilletShape_HeaderFile
#define _ChFi3d_FilletShape_HeaderFile


//! Lists the types of fillet shapes. These include the following:
//! -   ChFi3d_Rational (default value), which is the
//! standard NURBS representation of circles,
//! -   ChFi3d_QuasiAngular, which is a NURBS
//! representation of circles where the parameters
//! match those of the circle,
//! -   ChFi3d_Polynomial, which corresponds to a
//! polynomial approximation of circles. This type
//! facilitates the implementation of the construction algorithm.
enum ChFi3d_FilletShape
{
ChFi3d_Rational,
ChFi3d_QuasiAngular,
ChFi3d_Polynomial
};

#endif // _ChFi3d_FilletShape_HeaderFile
