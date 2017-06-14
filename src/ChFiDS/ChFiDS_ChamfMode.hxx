// Created by: Julia GERASIMOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _ChFiDS_ChamfMode_HeaderFile
#define _ChFiDS_ChamfMode_HeaderFile

//! this enumeration defines several modes of chamfer
enum ChFiDS_ChamfMode
{
  //! chamfer with constant distance from spine to one of the two surfaces
  ChFiDS_ClassicChamfer,
  
  //! symmetric chamfer with constant throat
  //! that is the height of isosceles triangle in section
  ChFiDS_ConstThroatChamfer,
  
  //! chamfer with constant throat: the section of chamfer is right-angled triangle,
  //! the first of two surfaces (where is the top of the chamfer)
  //! is virtually moved inside the solid by offset operation,
  //! the apex of the section is on the intersection curve between moved surface and second surface,
  //! right angle is at the top of the chamfer,
  //! the length of the leg from apex to top is constant - it is throat
  ChFiDS_ConstThroatWithPenetrationChamfer
};

#endif // _ChFiDS_ChamfMode_HeaderFile
