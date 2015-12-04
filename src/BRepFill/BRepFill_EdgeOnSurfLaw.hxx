// Created on: 1998-07-27
// Created by: Philippe MANGIN
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _BRepFill_EdgeOnSurfLaw_HeaderFile
#define _BRepFill_EdgeOnSurfLaw_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <BRepFill_LocationLaw.hxx>
class TopoDS_Wire;
class TopoDS_Shape;


class BRepFill_EdgeOnSurfLaw;
DEFINE_STANDARD_HANDLE(BRepFill_EdgeOnSurfLaw, BRepFill_LocationLaw)

//! Build Location Law, with a Wire and a  Surface.
class BRepFill_EdgeOnSurfLaw : public BRepFill_LocationLaw
{

public:

  
  Standard_EXPORT BRepFill_EdgeOnSurfLaw(const TopoDS_Wire& Path, const TopoDS_Shape& Surf);
  
  //! returns <False> if one  Edge of <Path> do not have
  //! representation on  <Surf>.   In this  case  it is
  //! impossible to use this object.
  Standard_EXPORT Standard_Boolean HasResult() const;




  DEFINE_STANDARD_RTTIEXT(BRepFill_EdgeOnSurfLaw,BRepFill_LocationLaw)

protected:




private:


  Standard_Boolean hasresult;


};







#endif // _BRepFill_EdgeOnSurfLaw_HeaderFile
