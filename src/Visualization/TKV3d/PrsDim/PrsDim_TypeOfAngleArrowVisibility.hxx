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

#ifndef _PrsDim_TypeOfAngleArrowVisibility_HeaderFile
#define _PrsDim_TypeOfAngleArrowVisibility_HeaderFile

//! Declares what arrows are visible on angle presentation
enum PrsDim_TypeOfAngleArrowVisibility
{
  PrsDim_TypeOfAngleArrowVisibility_Both,   //!< both arrows of the first and the second angle tips
  PrsDim_TypeOfAngleArrowVisibility_First,  //!< only first point arrow
  PrsDim_TypeOfAngleArrowVisibility_Second, //!< only second point arrow
  PrsDim_TypeOfAngleArrowVisibility_None,   //!< arrows are not visible
};

#endif // _PrsDim_TypeOfAngleArrowVisibility_HeaderFile
