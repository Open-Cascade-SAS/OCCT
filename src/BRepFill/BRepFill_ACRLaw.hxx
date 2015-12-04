// Created on: 1998-09-01
// Created by: Stephanie HUMEAU
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

#ifndef _BRepFill_ACRLaw_HeaderFile
#define _BRepFill_ACRLaw_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_HArray1OfReal.hxx>
#include <BRepFill_LocationLaw.hxx>
class TopoDS_Wire;
class GeomFill_LocationGuide;


class BRepFill_ACRLaw;
DEFINE_STANDARD_HANDLE(BRepFill_ACRLaw, BRepFill_LocationLaw)

//! Build Location Law,  with a Wire.   In the case
//! of guided contour and trihedron by reduced
//! curvilinear abscissa
class BRepFill_ACRLaw : public BRepFill_LocationLaw
{

public:

  
  Standard_EXPORT BRepFill_ACRLaw(const TopoDS_Wire& Path, const Handle(GeomFill_LocationGuide)& Law);




  DEFINE_STANDARD_RTTIEXT(BRepFill_ACRLaw,BRepFill_LocationLaw)

protected:




private:


  Handle(TColStd_HArray1OfReal) OrigParam;


};







#endif // _BRepFill_ACRLaw_HeaderFile
