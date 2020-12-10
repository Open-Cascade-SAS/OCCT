// Created on: 1994-05-09
// Created by: Gilles DEBARBOUILLE
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

#ifndef _Units_Explorer_HeaderFile
#define _Units_Explorer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Units_QuantitiesSequence.hxx>
#include <Units_UnitsSequence.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
#include <Standard_CString.hxx>
#include <Standard_Boolean.hxx>
class Units_UnitsSystem;
class Units_UnitsDictionary;
class TCollection_AsciiString;


//! This class provides all the services to explore
//! UnitsSystem or UnitsDictionary.
class Units_Explorer 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor of the class.
  Standard_EXPORT Units_Explorer();
  
  //! Creates a new instance of the class, initialized with
  //! the UnitsSystem <aunitssystem>.
  Standard_EXPORT Units_Explorer(const Handle(Units_UnitsSystem)& aunitssystem);
  
  //! Creates a new instance of the class, initialized with
  //! the UnitsDictionary <aunitsdictionary>.
  Standard_EXPORT Units_Explorer(const Handle(Units_UnitsDictionary)& aunitsdictionary);
  
  //! Creates a new instance of the class, initialized with
  //! the UnitsSystem <aunitssystem> and positioned at the
  //! quantity <aquantity>.
  Standard_EXPORT Units_Explorer(const Handle(Units_UnitsSystem)& aunitssystem, const Standard_CString aquantity);
  
  //! Creates a  new instance of the class,  initialized with
  //! the  UnitsDictionary <aunitsdictionary> and positioned
  //! at the quantity <aquantity>.
  Standard_EXPORT Units_Explorer(const Handle(Units_UnitsDictionary)& aunitsdictionary, const Standard_CString aquantity);
  
  //! Initializes  the  instance  of  the  class  with  the
  //! UnitsSystem <aunitssystem>.
  Standard_EXPORT void Init (const Handle(Units_UnitsSystem)& aunitssystem);
  
  //! Initializes  the  instance  of  the  class  with  the
  //! UnitsDictionary <aunitsdictionary>.
  Standard_EXPORT void Init (const Handle(Units_UnitsDictionary)& aunitsdictionary);
  
  //! Initializes  the  instance  of  the   class  with  the
  //! UnitsSystem  <aunitssystem>  and   positioned  at  the
  //! quantity <aquantity>.
  Standard_EXPORT void Init (const Handle(Units_UnitsSystem)& aunitssystem, const Standard_CString aquantity);
  
  //! Initializes  the  instance   of  the  class  with  the
  //! UnitsDictionary  <aunitsdictionary> and positioned  at
  //! the quantity <aquantity>.
  Standard_EXPORT void Init (const Handle(Units_UnitsDictionary)& aunitsdictionary, const Standard_CString aquantity);
  
  //! Returns True if there is another Quantity to explore,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean MoreQuantity() const;
  
  //! Sets the next Quantity current.
  Standard_EXPORT void NextQuantity();
  
  //! Returns the name of the current Quantity.
  Standard_EXPORT TCollection_AsciiString Quantity() const;
  
  //! Returns True if there is another Unit to explore,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean MoreUnit() const;
  
  //! Sets the next Unit current.
  Standard_EXPORT void NextUnit();
  
  //! Returns the name of the current unit.
  Standard_EXPORT TCollection_AsciiString Unit() const;
  
  //! If the  units system  to  explore  is  a user  system,
  //! returns True  if  the  current unit  is  active, False
  //! otherwise.
  //!
  //! If   the   units  system  to  explore  is   the  units
  //! dictionary,  returns True if the  current unit is  the
  //! S.I. unit.
  Standard_EXPORT Standard_Boolean IsActive() const;




protected:





private:



  Standard_Integer thecurrentquantity;
  Handle(Units_QuantitiesSequence) thequantitiessequence;
  Standard_Integer thecurrentunit;
  Handle(Units_UnitsSequence) theunitssequence;
  Handle(TColStd_HSequenceOfInteger) theactiveunitssequence;


};







#endif // _Units_Explorer_HeaderFile
