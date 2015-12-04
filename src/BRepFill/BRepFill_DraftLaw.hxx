// Created on: 1998-01-14
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

#ifndef _BRepFill_DraftLaw_HeaderFile
#define _BRepFill_DraftLaw_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <BRepFill_Edge3DLaw.hxx>
#include <Standard_Real.hxx>
class TopoDS_Wire;
class GeomFill_LocationDraft;


class BRepFill_DraftLaw;
DEFINE_STANDARD_HANDLE(BRepFill_DraftLaw, BRepFill_Edge3DLaw)

//! Build Location Law, with a  Wire.
class BRepFill_DraftLaw : public BRepFill_Edge3DLaw
{

public:

  
  Standard_EXPORT BRepFill_DraftLaw(const TopoDS_Wire& Path, const Handle(GeomFill_LocationDraft)& Law);
  
  //! To clean the little discontinuities.
  Standard_EXPORT void CleanLaw (const Standard_Real TolAngular);




  DEFINE_STANDARD_RTTIEXT(BRepFill_DraftLaw,BRepFill_Edge3DLaw)

protected:




private:




};







#endif // _BRepFill_DraftLaw_HeaderFile
