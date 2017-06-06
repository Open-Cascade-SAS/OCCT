// Created on: 1994-07-19
// Created by: Guest Kernel
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _Quantity_Convert_HeaderFile
#define _Quantity_Convert_HeaderFile

#include <NCollection_Array1.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <Quantity_PhysicalQuantity.hxx>

class TCollection_AsciiString;

#ifndef __Quantity_API
  #if defined(_WIN32) && !defined(HAVE_NO_DLL)
    #ifdef __Quantity_DLL
      #define __Quantity_API __declspec(dllexport)
    #else
      #define __Quantity_API __declspec(dllimport)
    #endif
  #else
    #define __Quantity_API
  #endif
#endif

extern __Quantity_API NCollection_Array1<Standard_Real> ConvertTable;

//! Services to manage units conversion between Front-ends and Engines.
//! This conversion is managed by a table of correspondance between the quantities
//! and their "conversion coefficient".
//! This table is implemented like an external array (TCollection_Array1) regarding
//! to the quantities enumeration.
class Standard_DEPRECATED("This class is deprecated - UnitsAPI should be used instead") Quantity_Convert
{
public:

  DEFINE_STANDARD_ALLOC

  //! Creates an object;
  Standard_EXPORT Quantity_Convert();
  
  //! Updates the conversion table (correspondances between
  //! Quantities and conversion coefficients).
  static void SetQuantity (const Quantity_PhysicalQuantity theQuantity,
                           const Standard_Real theCoef)
  {
    if (theQuantity > 0 && theQuantity <= 68)
    {
      ConvertTable (theQuantity) = theCoef;
    }
  }

  //! Converts, from the conversion table, the value theVal from the user system to the SI system.
  static Standard_Real ConvertUserToSI (const Quantity_PhysicalQuantity theQuantity,
                                        const Standard_Real theVal)
  {
    return theQuantity > 0 && theQuantity <= 68
         ? theVal * ConvertTable (theQuantity)
         : theVal;
  }

  //! Converts, from the conversion table, the value theVal from the SI system to the user system.
  static Standard_Real ConvertSIToUser (const Quantity_PhysicalQuantity theQuantity, const Standard_Real theVal)
  {
    return theQuantity > 0 && theQuantity <= 68
         ? (theVal / ConvertTable (theQuantity))
         : theVal;
  }

  //! if (aType is a physical quantity)
  //! returns True and the name of the associated PhysicalQuantity .
  //! else
  //! returns False.
  Standard_EXPORT static Standard_Boolean IsPhysicalQuantity (const TCollection_AsciiString& aTypeName, TCollection_AsciiString& anEnum);

};

#endif // _Quantity_Convert_HeaderFile
