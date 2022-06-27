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

#ifndef _Units_UnitsDictionary_HeaderFile
#define _Units_UnitsDictionary_HeaderFile

#include <Units_QuantitiesSequence.hxx>
class TCollection_AsciiString;
class Units_Dimensions;


class Units_UnitsDictionary;
DEFINE_STANDARD_HANDLE(Units_UnitsDictionary, Standard_Transient)

//! This class creates  a dictionary of all  the units
//! you want to know.
class Units_UnitsDictionary : public Standard_Transient
{

public:

  
  //! Returns an empty instance of UnitsDictionary.
  Standard_EXPORT Units_UnitsDictionary();
  
  //! Returns a  UnitsDictionary object  which  contains the
  //! sequence  of all   the  units  you want to   consider,
  //! physical quantity by physical quantity.
  Standard_EXPORT void Creates ();
  
  //! Returns   the  head   of   the  sequence  of  physical
  //! quantities.
  Handle(Units_QuantitiesSequence) Sequence() const;

  //! Returns for <aquantity> the active unit.
  Standard_EXPORT TCollection_AsciiString ActiveUnit (const Standard_CString aquantity) const;
  
  //! Dumps only  the sequence   of  quantities without  the
  //! units  if  <alevel> is  equal  to zero,  and  for each
  //! quantity all the units stored if <alevel>  is equal to
  //! one.
    void Dump (const Standard_Integer alevel) const;
  
  //! Dumps  for a     designated  physical       dimensions
  //! <adimensions> all the previously stored units.
    void Dump (const Handle(Units_Dimensions)& adimensions) const;




  DEFINE_STANDARD_RTTIEXT(Units_UnitsDictionary,Standard_Transient)

protected:




private:


  Handle(Units_QuantitiesSequence) thequantitiessequence;


};


#include <Units_UnitsDictionary.lxx>





#endif // _Units_UnitsDictionary_HeaderFile
