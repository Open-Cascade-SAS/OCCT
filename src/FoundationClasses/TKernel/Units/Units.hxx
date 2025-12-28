// Created on: 1992-06-22
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _Units_HeaderFile
#define _Units_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_CString.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class Units_UnitsDictionary;
class Units_Quantity;
class Units_Lexicon;
class Units_Dimensions;

//! This package provides all the facilities to create
//! and question a dictionary of units, and also to
//! manipulate measurements which are real values with
//! units.
class Units
{
public:
  DEFINE_STANDARD_ALLOC

  //! Defines the location of the file containing all the
  //! information useful in creating the dictionary of all
  //! the units known to the system.
  Standard_EXPORT static void UnitsFile(const char* afile);

  //! Defines the location of the file containing the lexicon
  //! useful in manipulating composite units.
  Standard_EXPORT static void LexiconFile(const char* afile);

  //! Returns a unique instance of the dictionary of units.
  //! If <amode> is True, then it forces the recomputation of
  //! the dictionary of units.
  Standard_EXPORT static occ::handle<Units_UnitsDictionary> DictionaryOfUnits(
    const bool amode = false);

  //! Returns a unique quantity instance corresponding to <aquantity>.
  Standard_EXPORT static occ::handle<Units_Quantity> Quantity(const char* aquantity);

  //! Returns the first quantity string founded from the unit <aUnit>.
  Standard_EXPORT static const char* FirstQuantity(const char* aunit);

  //! Returns a unique instance of the Units_Lexicon.
  //! If <amode> is True, it forces the recomputation of
  //! the dictionary of units, and by consequence the
  //! completion of the Units_Lexicon.
  Standard_EXPORT static occ::handle<Units_Lexicon> LexiconUnits(
    const bool amode = true);

  //! Return a unique instance of LexiconFormula.
  Standard_EXPORT static occ::handle<Units_Lexicon> LexiconFormula();

  //! Returns always the same instance of Dimensions.
  Standard_EXPORT static occ::handle<Units_Dimensions> NullDimensions();

  //! Converts <avalue> expressed in <afirstunit> into the <asecondunit>.
  Standard_EXPORT static double Convert(const double    avalue,
                                               const char* afirstunit,
                                               const char* asecondunit);

  Standard_EXPORT static double ToSI(const double    aData,
                                            const char* aUnit);

  Standard_EXPORT static double ToSI(const double       aData,
                                            const char*    aUnit,
                                            occ::handle<Units_Dimensions>& aDim);

  Standard_EXPORT static double FromSI(const double    aData,
                                              const char* aUnit);

  Standard_EXPORT static double FromSI(const double       aData,
                                              const char*    aUnit,
                                              occ::handle<Units_Dimensions>& aDim);

  //! return the dimension associated to the Type
  Standard_EXPORT static occ::handle<Units_Dimensions> Dimensions(const char* aType);
};

#endif // _Units_HeaderFile
