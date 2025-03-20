// Created on: 1998-07-21
// Created by: data exchange team
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

#ifndef _ShapeExtend_Status_HeaderFile
#define _ShapeExtend_Status_HeaderFile

//! This enumeration is used in
//! ShapeHealing toolkit for representing flags in the
//! return statuses of class methods.
//! The status is a field of the class which is set by one or
//! several methods of that class.
//! It is used for reporting about errors and other situations
//! encountered during execution of the method.
//! There are defined 8 values for DONE and 8 for FAIL flags:
//! ShapeExtend_DONE1 ...      ShapeExtend_DONE8,
//! ShapeExtend_FAIL1 ...      ShapeExtend_FAIL8
//! and also enumerations for representing combinations of flags:
//! ShapeExtend_OK - no flags at all,
//! ShapeExtend_DONE - any of flags DONEi,
//! ShapeExtend_FAIL - any of flags FAILi.
//! The class that uses statuses provides a method(s) which
//! answers whether the flag corresponding to a given
//! enumerative value is (are) set:
//! Standard_Boolean Status(const ShapeExtend_Status test);
//! Note that status can have several flags set simultaneously.
//! Status(ShapeExtend_OK) gives True when no flags are set.
//! Nothing done, everything OK
//! Something was done, case 1
//! Something was done, case 2
//! Something was done, case 3
//! Something was done, case 4
//! Something was done, case 5
//! Something was done, case 6
//! Something was done, case 7
//! Something was done, case 8
//! Something was done (any of DONE#)
//! The method failed, case 1
//! The method failed, case 2
//! The method failed, case 3
//! The method failed, case 4
//! The method failed, case 5
//! The method failed, case 6
//! The method failed, case 7
//! The method failed, case 8
//! The method failed (any of FAIL# occurred)
enum ShapeExtend_Status
{
  ShapeExtend_OK,
  ShapeExtend_DONE1,
  ShapeExtend_DONE2,
  ShapeExtend_DONE3,
  ShapeExtend_DONE4,
  ShapeExtend_DONE5,
  ShapeExtend_DONE6,
  ShapeExtend_DONE7,
  ShapeExtend_DONE8,
  ShapeExtend_DONE,
  ShapeExtend_FAIL1,
  ShapeExtend_FAIL2,
  ShapeExtend_FAIL3,
  ShapeExtend_FAIL4,
  ShapeExtend_FAIL5,
  ShapeExtend_FAIL6,
  ShapeExtend_FAIL7,
  ShapeExtend_FAIL8,
  ShapeExtend_FAIL
};

#endif // _ShapeExtend_Status_HeaderFile
