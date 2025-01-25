// Created on: 1993-10-22
// Created by: Gilles DEBARBOUILLE
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

#ifndef _Units_UnitsSystem_HeaderFile
#define _Units_UnitsSystem_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Units_QuantitiesSequence.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
#include <Standard_Transient.hxx>
class TCollection_AsciiString;

class Units_UnitsSystem;
DEFINE_STANDARD_HANDLE(Units_UnitsSystem, Standard_Transient)

//! This class  allows  the  user  to  define his  own
//! system of units.
class Units_UnitsSystem : public Standard_Transient
{

public:
  //! Returns an instance of UnitsSystem initialized to the
  //! S.I. units system.
  Standard_EXPORT Units_UnitsSystem();

  //! Returns an instance of UnitsSystem initialized to the
  //! S.I. units system upgraded by the base system units description
  //! file.
  //! Attempts to find the four following files:
  //! $CSF_`aName`Defaults/.aName
  //! $CSF_`aName`SiteDefaults/.aName
  //! $CSF_`aName`GroupDefaults/.aName
  //! $CSF_`aName`UserDefaults/.aName
  //! See : Resource_Manager for the description of this file.
  Standard_EXPORT Units_UnitsSystem(const Standard_CString aName,
                                    const Standard_Boolean Verbose = Standard_False);

  //! Returns the sequence of refined quantities.
  Standard_EXPORT Handle(Units_QuantitiesSequence) QuantitiesSequence() const;

  //! Returns a sequence of integer in correspondence with
  //! the sequence of quantities, which indicates, for each
  //! redefined quantity, the index into the sequence of
  //! units, of the active unit.
  Standard_EXPORT Handle(TColStd_HSequenceOfInteger) ActiveUnitsSequence() const;

  //! Specifies for <aquantity> the unit <aunit> used.
  Standard_EXPORT void Specify(const Standard_CString aquantity, const Standard_CString aunit);

  //! Removes for <aquantity> the unit <aunit> used.
  Standard_EXPORT void Remove(const Standard_CString aquantity, const Standard_CString aunit);

  //! Specifies for <aquantity> the unit <aunit> used.
  Standard_EXPORT void Activate(const Standard_CString aquantity, const Standard_CString aunit);

  //! Activates the first unit of all defined system quantities
  Standard_EXPORT void Activates();

  //! Returns for <aquantity> the active unit.
  Standard_EXPORT TCollection_AsciiString ActiveUnit(const Standard_CString aquantity) const;

  //! Converts a real value <avalue> from the unit <aunit>
  //! belonging to the physical dimensions <aquantity> to
  //! the corresponding unit of the user system.
  Standard_EXPORT Standard_Real ConvertValueToUserSystem(const Standard_CString aquantity,
                                                         const Standard_Real    avalue,
                                                         const Standard_CString aunit) const;

  //! Converts the real value <avalue> from the S.I. system
  //! of units to the user system of units. <aquantity> is
  //! the physical dimensions of the measurement.
  Standard_EXPORT Standard_Real ConvertSIValueToUserSystem(const Standard_CString aquantity,
                                                           const Standard_Real    avalue) const;

  //! Converts the real value <avalue> from the user system
  //! of units to the S.I. system of units. <aquantity> is
  //! the physical dimensions of the measurement.
  Standard_EXPORT Standard_Real ConvertUserSystemValueToSI(const Standard_CString aquantity,
                                                           const Standard_Real    avalue) const;

  Standard_EXPORT void Dump() const;

  //! Returns TRUE if no units has been defined in the system.
  Standard_EXPORT Standard_Boolean IsEmpty() const;

  DEFINE_STANDARD_RTTIEXT(Units_UnitsSystem, Standard_Transient)

protected:
private:
  Handle(Units_QuantitiesSequence)   thequantitiessequence;
  Handle(TColStd_HSequenceOfInteger) theactiveunitssequence;
};

#endif // _Units_UnitsSystem_HeaderFile
