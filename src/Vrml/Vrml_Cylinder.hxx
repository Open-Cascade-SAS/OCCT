// Created on: 1996-12-25
// Created by: Alexander BRIVIN
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _Vrml_Cylinder_HeaderFile
#define _Vrml_Cylinder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Vrml_CylinderParts.hxx>
#include <Standard_Real.hxx>
#include <Standard_OStream.hxx>

//! defines a Cylinder node of VRML specifying geometry shapes.
//! This  node  represents  a  simple  capped  cylinder  centred  around the  y-axis.
//! By  default ,  the  cylinder  is  centred  at  (0,0,0)
//! and  has  size  of  -1  to  +1  in  the  all  three  dimensions.
//! The  cylinder  has  three  parts:
//! the  sides,  the  top  (y=+1)  and  the  bottom (y=-1)
class Vrml_Cylinder
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT Vrml_Cylinder(const Vrml_CylinderParts aParts  = Vrml_CylinderALL,
                                const Standard_Real      aRadius = 1,
                                const Standard_Real      aHeight = 2);

  Standard_EXPORT void SetParts(const Vrml_CylinderParts aParts);

  Standard_EXPORT Vrml_CylinderParts Parts() const;

  Standard_EXPORT void SetRadius(const Standard_Real aRadius);

  Standard_EXPORT Standard_Real Radius() const;

  Standard_EXPORT void SetHeight(const Standard_Real aHeight);

  Standard_EXPORT Standard_Real Height() const;

  Standard_EXPORT Standard_OStream& Print(Standard_OStream& anOStream) const;

protected:
private:
  Vrml_CylinderParts myParts;
  Standard_Real      myRadius;
  Standard_Real      myHeight;
};

#endif // _Vrml_Cylinder_HeaderFile
