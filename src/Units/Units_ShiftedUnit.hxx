// Created on: 1992-11-04
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

#ifndef _Units_ShiftedUnit_HeaderFile
#define _Units_ShiftedUnit_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Units_Unit.hxx>
#include <Standard_Integer.hxx>
class Units_Quantity;
class Units_Token;


class Units_ShiftedUnit;
DEFINE_STANDARD_HANDLE(Units_ShiftedUnit, Units_Unit)

//! This class is useful   to describe  units  with  a
//! shifted origin in relation to another unit. A well
//! known example  is the  Celsius degrees in relation
//! to Kelvin degrees. The shift of the Celsius origin
//! is 273.15 Kelvin degrees.
class Units_ShiftedUnit : public Units_Unit
{

public:

  
  //! Creates  and  returns a  shifted unit.   <aname> is the
  //! name of the unit,  <asymbol> is the usual abbreviation
  //! of the unit, <avalue> is the  value in relation to the
  //! International System of Units, and <amove>  is the gap
  //! in relation to another unit.
  //!
  //! For  example Celsius   degree   of temperature  is  an
  //! instance of ShiftedUnit  with <avalue> equal to 1. and
  //! <amove> equal to 273.15.
  Standard_EXPORT Units_ShiftedUnit(const Standard_CString aname, const Standard_CString asymbol, const Standard_Real avalue, const Standard_Real amove, const Handle(Units_Quantity)& aquantity);
  
  //! Creates  and returns a  unit.  <aname> is  the name of
  //! the  unit, <asymbol> is the  usual abbreviation of the
  //! unit.
  Standard_EXPORT Units_ShiftedUnit(const Standard_CString aname, const Standard_CString asymbol);
  
  //! Creates  and returns a  unit.  <aname> is  the name of
  //! the  unit.
  Standard_EXPORT Units_ShiftedUnit(const Standard_CString aname);
  
  //! Sets the field <themove> to <amove>
  Standard_EXPORT void Move (const Standard_Real amove);
  
  //! Returns the shifted value <themove>.
  Standard_EXPORT Standard_Real Move() const;
  
  //! This redefined method returns a ShiftedToken object.
  Standard_EXPORT virtual Handle(Units_Token) Token() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Dump (const Standard_Integer ashift, const Standard_Integer alevel) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(Units_ShiftedUnit,Units_Unit)

protected:




private:


  Standard_Real themove;


};







#endif // _Units_ShiftedUnit_HeaderFile
