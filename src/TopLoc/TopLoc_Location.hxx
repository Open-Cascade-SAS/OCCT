// Created on: 1990-12-19
// Created by: Christophe MARION
// Copyright (c) 1990-1999 Matra Datavision
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

#ifndef _TopLoc_Location_HeaderFile
#define _TopLoc_Location_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopLoc_SListOfItemLocation.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>

class gp_Trsf;
class TopLoc_Datum3D;

//! A Location is a composite transition. It comprises a
//! series of elementary reference coordinates, i.e.
//! objects of type TopLoc_Datum3D, and the powers to
//! which these objects are raised.
class TopLoc_Location 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs an empty local coordinate system object.
  //! Note: A Location constructed from a default datum is said to be "empty".
  Standard_EXPORT TopLoc_Location();
  
  //! Constructs the local coordinate system object defined
  //! by the transformation T. T invokes in turn, a TopLoc_Datum3D object.
  Standard_EXPORT TopLoc_Location(const gp_Trsf& T);
  
  //! Constructs the local coordinate system object defined by the 3D datum D.
  //! Exceptions
  //! Standard_ConstructionError if the transformation
  //! T does not represent a 3D coordinate system.
  Standard_EXPORT TopLoc_Location(const Handle(TopLoc_Datum3D)& D);
  
  //! Returns true if this location is equal to the Identity transformation.
    Standard_Boolean IsIdentity() const;
  
  //! Resets this location to the Identity transformation.
    void Identity();
  
  //! Returns    the  first   elementary  datum  of  the
  //! Location.  Use the NextLocation function recursively to access
  //! the other data comprising this location.
  //! Exceptions
  //! Standard_NoSuchObject if this location is empty.
    const Handle(TopLoc_Datum3D)& FirstDatum() const;
  
  //! Returns   the  power  elevation  of    the   first
  //! elementary datum.
  //! Exceptions
  //! Standard_NoSuchObject if this location is empty.
    Standard_Integer FirstPower() const;
  
  //! Returns  a Location representing  <me> without the
  //! first datum. We have the relation :
  //!
  //! <me> = NextLocation() * FirstDatum() ^ FirstPower()
  //! Exceptions
  //! Standard_NoSuchObject if this location is empty.
    const TopLoc_Location& NextLocation() const;
  
  //! Returns  the transformation    associated  to  the
  //! coordinate system.
  Standard_EXPORT const gp_Trsf& Transformation() const;
Standard_EXPORT operator gp_Trsf() const;
  
  //! Returns the inverse of <me>.
  //!
  //! <me> * Inverted() is an Identity.
  Standard_NODISCARD Standard_EXPORT TopLoc_Location Inverted() const;
  
  //! Returns <me> * <Other>, the  elementary datums are
  //! concatenated.
  Standard_NODISCARD Standard_EXPORT TopLoc_Location Multiplied (const TopLoc_Location& Other) const;
Standard_NODISCARD TopLoc_Location operator* (const TopLoc_Location& Other) const
{
  return Multiplied(Other);
}
  
  //! Returns  <me> / <Other>.
  Standard_NODISCARD Standard_EXPORT TopLoc_Location Divided (const TopLoc_Location& Other) const;
Standard_NODISCARD TopLoc_Location operator/ (const TopLoc_Location& Other) const
{
  return Divided(Other);
}
  
  //! Returns <Other>.Inverted() * <me>.
  Standard_NODISCARD Standard_EXPORT TopLoc_Location Predivided (const TopLoc_Location& Other) const;
  
  //! Returns me at the power <pwr>.   If <pwr>  is zero
  //! returns  Identity.  <pwr> can  be lower  than zero
  //! (usual meaning for powers).
  Standard_NODISCARD Standard_EXPORT TopLoc_Location Powered (const Standard_Integer pwr) const;

  //! Returns a hashed value for this local coordinate system. This value is used, with map tables, to store and
  //! retrieve the object easily
  //! @return a computed hash code
  size_t HashCode () const;
  
  //! Returns true if this location and the location Other
  //! have the same elementary data, i.e. contain the same
  //! series of TopLoc_Datum3D and respective powers.
  //! This method is an alias for operator ==.
  Standard_EXPORT Standard_Boolean IsEqual (const TopLoc_Location& Other) const;
Standard_Boolean operator == (const TopLoc_Location& Other) const
{
  return IsEqual(Other);
}
  
  //! Returns true if this location and the location Other do
  //! not have the same elementary data, i.e. do not
  //! contain the same series of TopLoc_Datum3D and respective powers.
  //! This method is an alias for operator !=.
  Standard_EXPORT Standard_Boolean IsDifferent (const TopLoc_Location& Other) const;
Standard_Boolean operator != (const TopLoc_Location& Other) const
{
  return IsDifferent(Other);
}
  
  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

  //! Prints the contents of <me> on the stream <s>.
  Standard_EXPORT void ShallowDump (Standard_OStream& S) const;

  //! Clear myItems
  void Clear()
  {
    myItems.Clear();
  }


  static Standard_Real ScalePrec()
  {
    return  1.e-14;
  }

protected:





private:



  TopLoc_SListOfItemLocation myItems;


};

#include <TopLoc_Location.lxx>

#endif // _TopLoc_Location_HeaderFile
