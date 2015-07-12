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

#ifndef _Intf_TangentZone_HeaderFile
#define _Intf_TangentZone_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Intf_SeqOfSectionPoint.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Standard_OutOfRange;
class Intf_SectionPoint;


//! Describes   a  zone  of  tangence  between  polygons  or
//! polyhedra as a sequence of points of intersection.
class Intf_TangentZone 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns number of SectionPoint in this TangentZone.
    Standard_Integer NumberOfPoints() const;
  
  //! Gives   the   SectionPoint   of  address  <Index>  in  the
  //! TangentZone.
  Standard_EXPORT const Intf_SectionPoint& GetPoint (const Standard_Integer Index) const;
  
  //! Compares two TangentZones.
  Standard_EXPORT Standard_Boolean IsEqual (const Intf_TangentZone& Other) const;
Standard_Boolean operator == (const Intf_TangentZone& Other) const
{
  return IsEqual(Other);
}
  
  //! Checks if <ThePI> is in TangentZone.
  Standard_EXPORT Standard_Boolean Contains (const Intf_SectionPoint& ThePI) const;
  
  //! Gives  the parameter range of the  TangentZone on the first
  //! argument of the Interference. (Usable only for polygon)
    void ParamOnFirst (Standard_Real& paraMin, Standard_Real& paraMax) const;
  
  //! Gives the parameter range of the  TangentZone on the second
  //! argument of the Interference. (Usable only for polygon)
    void ParamOnSecond (Standard_Real& paraMin, Standard_Real& paraMax) const;
  
  //! Gives information  about  the    first argument   of   the
  //! Interference. (Usable only for polygon)
  Standard_EXPORT void InfoFirst (Standard_Integer& segMin, Standard_Real& paraMin, Standard_Integer& segMax, Standard_Real& paraMax) const;
  
  //! Gives   informations  about  the  second   argument of  the
  //! Interference. (Usable only for polygon)
  Standard_EXPORT void InfoSecond (Standard_Integer& segMin, Standard_Real& paraMin, Standard_Integer& segMax, Standard_Real& paraMax) const;
  
  //! Returns True if  <ThePI>  is in the parameter  range of the
  //! TangentZone.
  Standard_EXPORT Standard_Boolean RangeContains (const Intf_SectionPoint& ThePI) const;
  
  //! Returns True if the TangentZone  <Other> has  a common part
  //! with <me>.
  Standard_EXPORT Standard_Boolean HasCommonRange (const Intf_TangentZone& Other) const;
  
  //! Builds an empty tangent zone.
  Standard_EXPORT Intf_TangentZone();
  
  //! Copies a Tangent zone.
  Standard_EXPORT Intf_TangentZone(const Intf_TangentZone& Other);
  
  //! Adds a SectionPoint to the TangentZone.
  Standard_EXPORT void Append (const Intf_SectionPoint& Pi);
  
  //! Adds the TangentZone <Tzi> to <me>.
  Standard_EXPORT void Append (const Intf_TangentZone& Tzi);
  
  //! Inserts a SectionPoint in the TangentZone.
  Standard_EXPORT Standard_Boolean Insert (const Intf_SectionPoint& Pi);
  
  //! Inserts a point in the polygonal TangentZone.
  Standard_EXPORT void PolygonInsert (const Intf_SectionPoint& Pi);
  
  //! Inserts a SectionPoint before <Index> in the TangentZone.
  Standard_EXPORT void InsertBefore (const Standard_Integer Index, const Intf_SectionPoint& Pi);
  
  //! Inserts a SectionPoint after <Index> in the TangentZone.
  Standard_EXPORT void InsertAfter (const Standard_Integer Index, const Intf_SectionPoint& Pi);
  
  Standard_EXPORT void Dump (const Standard_Integer Indent) const;




protected:





private:



  Intf_SeqOfSectionPoint Result;
  Standard_Real ParamOnFirstMin;
  Standard_Real ParamOnFirstMax;
  Standard_Real ParamOnSecondMin;
  Standard_Real ParamOnSecondMax;


};


#include <Intf_TangentZone.lxx>





#endif // _Intf_TangentZone_HeaderFile
