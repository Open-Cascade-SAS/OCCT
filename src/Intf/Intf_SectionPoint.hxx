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

#ifndef _Intf_SectionPoint_HeaderFile
#define _Intf_SectionPoint_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <gp_Pnt.hxx>
#include <Intf_PIType.hxx>
#include <Standard_Integer.hxx>
class gp_Pnt2d;


//! Describes an intersection  point between  polygons  and
//! polyedra.
class Intf_SectionPoint 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Returns the location of the SectionPoint.
  Standard_EXPORT const gp_Pnt& Pnt() const;
  
  //! Returns the cumulated  Parameter of the SectionPoint on the
  //! first element.
    Standard_Real ParamOnFirst() const;
  
  //! Returns the cumulated Parameter of the section point on the
  //! second element.
    Standard_Real ParamOnSecond() const;
  
  //! Returns the type of the section point on the first element.
    Intf_PIType TypeOnFirst() const;
  
  //! Returns  the  type  of the  section  point  on  the  second
  //! element.
    Intf_PIType TypeOnSecond() const;
  
  Standard_EXPORT void InfoFirst (Intf_PIType& Dim, Standard_Integer& Add1, Standard_Integer& Add2, Standard_Real& Param) const;

  //! Gives the data about the first argument of the Interference.
  Standard_EXPORT void InfoFirst (Intf_PIType& Dim, Standard_Integer& Addr, Standard_Real& Param) const;

  Standard_EXPORT void InfoSecond (Intf_PIType& Dim, Standard_Integer& Add1, Standard_Integer& Add2, Standard_Real& Param) const;

  //! Gives the data about the second argument of the Interference.
  Standard_EXPORT void InfoSecond (Intf_PIType& Dim, Standard_Integer& Addr, Standard_Real& Param) const;

  //! Gives the incidence at  this  section point. The  incidence
  //! between the two triangles is given by the cosine.  The best
  //! incidence is 0. (PI/2).  The worst is 1. (null angle).
  Standard_EXPORT Standard_Real Incidence() const;
  
  //! Returns True if the two SectionPoint have the  same logical
  //! information.
    Standard_Boolean IsEqual (const Intf_SectionPoint& Other) const;
  Standard_Boolean operator== (const Intf_SectionPoint& Other) const
{
  return IsEqual(Other);
}
  
  //! Returns True if the two SectionPoints are  on the  same edge
  //! of the first or the second element.
  Standard_EXPORT Standard_Boolean IsOnSameEdge (const Intf_SectionPoint& Other) const;
  
  Standard_EXPORT Intf_SectionPoint();
  
  //! Builds  a  SectionPoint    with the  respective  dimensions
  //! (vertex edge or face)  of the concerned arguments and their
  //! addresses in the Topological structure.
  Standard_EXPORT Intf_SectionPoint(const gp_Pnt& Where, const Intf_PIType DimeO, const Standard_Integer AddrO1, const Standard_Integer AddrO2, const Standard_Real ParamO, const Intf_PIType DimeT, const Standard_Integer AddrT1, const Standard_Integer AddrT2, const Standard_Real ParamT, const Standard_Real Incid);
  
  //! Builds  a  SectionPoint 2d   with the respective dimensions
  //! (vertex or  edge)  of  the concerned arguments   and  their
  //! addresses in the Topological structure.
  Standard_EXPORT Intf_SectionPoint(const gp_Pnt2d& Where, const Intf_PIType DimeO, const Standard_Integer AddrO1, const Standard_Real ParamO, const Intf_PIType DimeT, const Standard_Integer AddrT1, const Standard_Real ParamT, const Standard_Real Incid);
  
  //! Merges two SectionPoints.
  Standard_EXPORT void Merge (Intf_SectionPoint& Other);
  
  Standard_EXPORT void Dump (const Standard_Integer Indent) const;

private:

  gp_Pnt myPnt;
  Intf_PIType DimenObje;
  Standard_Integer IndexO1;
  Standard_Integer IndexO2;
  Standard_Real ParamObje;
  Intf_PIType DimenTool;
  Standard_Integer IndexT1;
  Standard_Integer IndexT2;
  Standard_Real ParamTool;
  Standard_Real Incide;

};

#include <Intf_SectionPoint.lxx>

#endif // _Intf_SectionPoint_HeaderFile
