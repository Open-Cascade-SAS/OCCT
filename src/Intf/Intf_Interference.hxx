// Created on: 1991-06-24
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

#ifndef _Intf_Interference_HeaderFile
#define _Intf_Interference_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Intf_SeqOfSectionPoint.hxx>
#include <Intf_SeqOfSectionLine.hxx>
#include <Intf_SeqOfTangentZone.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
class Intf_SectionPoint;
class Intf_SectionLine;
class Intf_TangentZone;


//! Describes the   Interference  computation    result
//! between polygon2d or polygon3d or polyhedron
//! (as  three sequences   of  points  of  intersection,
//! polylines of intersection and zones de tangence).
class Intf_Interference 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Gives the number   of  points of  intersection  in the
  //! interference.
    Standard_Integer NbSectionPoints() const;
  
  //! Gives the point of  intersection of address  Index in
  //! the interference.
    const Intf_SectionPoint& PntValue (const Standard_Integer Index) const;
  
  //! Gives the number  of polylines of  intersection in the
  //! interference.
    Standard_Integer NbSectionLines() const;
  
  //! Gives the polyline of intersection at address <Index> in
  //! the interference.
    const Intf_SectionLine& LineValue (const Standard_Integer Index) const;
  
  //! Gives the number of zones of tangence in the interference.
    Standard_Integer NbTangentZones() const;
  
  //! Gives  the zone of  tangence at address   Index in the
  //! interference.
    const Intf_TangentZone& ZoneValue (const Standard_Integer Index) const;
  
  //! Gives the tolerance used for the calculation.
    Standard_Real GetTolerance() const;
  
  //! Tests if the polylines of  intersection or the zones of
  //! tangence contain the point of intersection <ThePnt>.
  Standard_EXPORT Standard_Boolean Contains (const Intf_SectionPoint& ThePnt) const;
  
  //! Inserts a new zone of tangence in  the current list of
  //! tangent zones of  the interference  and  returns  True
  //! when done.
  Standard_EXPORT Standard_Boolean Insert (const Intf_TangentZone& TheZone);
  
  //! Insert a new segment of intersection in the current  list of
  //! polylines of intersection of the interference.
  Standard_EXPORT void Insert (const Intf_SectionPoint& pdeb, const Intf_SectionPoint& pfin);
  
  Standard_EXPORT void Dump() const;




protected:

  //! Empty constructor
  Standard_EXPORT Intf_Interference(const Standard_Boolean Self);
  
  //! Destructor is protected, for safer inheritance
  ~Intf_Interference () {}

  //! Only one argument for the intersection.
  Standard_EXPORT void SelfInterference (const Standard_Boolean Self);


  Intf_SeqOfSectionPoint mySPoins;
  Intf_SeqOfSectionLine mySLines;
  Intf_SeqOfTangentZone myTZones;
  Standard_Boolean SelfIntf;
  Standard_Real Tolerance;


private:





};


#include <Intf_Interference.lxx>





#endif // _Intf_Interference_HeaderFile
