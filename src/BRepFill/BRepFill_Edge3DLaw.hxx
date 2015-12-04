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

#ifndef _BRepFill_Edge3DLaw_HeaderFile
#define _BRepFill_Edge3DLaw_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <BRepFill_LocationLaw.hxx>
class TopoDS_Wire;
class GeomFill_LocationLaw;


class BRepFill_Edge3DLaw;
DEFINE_STANDARD_HANDLE(BRepFill_Edge3DLaw, BRepFill_LocationLaw)

//! Build Location Law, with a  Wire.
class BRepFill_Edge3DLaw : public BRepFill_LocationLaw
{

public:

  
  Standard_EXPORT BRepFill_Edge3DLaw(const TopoDS_Wire& Path, const Handle(GeomFill_LocationLaw)& Law);




  DEFINE_STANDARD_RTTIEXT(BRepFill_Edge3DLaw,BRepFill_LocationLaw)

protected:




private:




};







#endif // _BRepFill_Edge3DLaw_HeaderFile
