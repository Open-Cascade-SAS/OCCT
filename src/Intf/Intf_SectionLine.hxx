// Created on: 1991-06-18
// Created by: Didier PIFFAULT
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Intf_SectionLine_HeaderFile
#define _Intf_SectionLine_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Intf_SeqOfSectionPoint.hxx>
#include <Standard_Boolean.hxx>
class Intf_SectionPoint;


//! Describe    a  polyline  of   intersection  between two
//! polyhedra as a sequence of points of intersection.
class Intf_SectionLine 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns number of points in this SectionLine.
    Standard_Integer NumberOfPoints() const;
  
  //! Gives the point of intersection of  address <Index>  in the
  //! SectionLine.
  Standard_EXPORT const Intf_SectionPoint& GetPoint (const Standard_Integer Index) const;
  
  //! Returns True if the SectionLine is closed.
  Standard_EXPORT Standard_Boolean IsClosed() const;
  
  //! Returns True if ThePI is in the SectionLine <me>.
  Standard_EXPORT Standard_Boolean Contains (const Intf_SectionPoint& ThePI) const;
  
  //! Checks if <ThePI>  is an end of  the SectionLine. Returns 1
  //! for the beginning, 2 for the end, otherwise 0.
  Standard_EXPORT Standard_Integer IsEnd (const Intf_SectionPoint& ThePI) const;
  
  //! Compares two SectionLines.
  Standard_EXPORT Standard_Boolean IsEqual (const Intf_SectionLine& Other) const;
Standard_Boolean operator == (const Intf_SectionLine& Other) const
{
  return IsEqual(Other);
}
  
  //! Constructs an empty SectionLine.
  Standard_EXPORT Intf_SectionLine();
  
  //! Copies a SectionLine.
  Standard_EXPORT Intf_SectionLine(const Intf_SectionLine& Other);
  
  //! Assignment
  Intf_SectionLine& operator= (const Intf_SectionLine& theOther)
  {
    //closed = theOther.closed; // not copied as in copy constructor
    myPoints = theOther.myPoints;
    return *this;
  }

  //! Adds a point at the end of the SectionLine.
  Standard_EXPORT void Append (const Intf_SectionPoint& Pi);
  
  //! Concatenates   the SectionLine  <LS>  at  the  end  of  the
  //! SectionLine <me>.
  Standard_EXPORT void Append (Intf_SectionLine& LS);
  
  //! Adds a point to the beginning of the SectionLine <me>.
  Standard_EXPORT void Prepend (const Intf_SectionPoint& Pi);
  
  //! Concatenates a SectionLine  <LS>  at the  beginning  of the
  //! SectionLine <me>.
  Standard_EXPORT void Prepend (Intf_SectionLine& LS);
  
  //! Reverses the order of the elements of the SectionLine.
  Standard_EXPORT void Reverse();
  
  //! Closes the SectionLine.
  Standard_EXPORT void Close();
  
  Standard_EXPORT void Dump (const Standard_Integer Indent) const;




protected:





private:



  Intf_SeqOfSectionPoint myPoints;
  Standard_Boolean closed;


};


#include <Intf_SectionLine.lxx>





#endif // _Intf_SectionLine_HeaderFile
