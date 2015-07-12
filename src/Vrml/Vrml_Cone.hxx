// Created on: 1996-12-24
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

#ifndef _Vrml_Cone_HeaderFile
#define _Vrml_Cone_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Vrml_ConeParts.hxx>
#include <Standard_Real.hxx>
#include <Standard_OStream.hxx>


//! defines a Cone node of VRML specifying geometry shapes.
//! This  node  represents  a  simple  cone,  whose  central  axis  is  aligned
//! with  the  y-axis.  By  default ,  the  cone  is  centred  at  (0,0,0)
//! and  has  size  of  -1  to  +1  in  the  all  three  directions.
//! the  cone  has  a  radius  of  1  at  the  bottom  and  height  of  2,
//! with  its  apex  at  1  and  its  bottom  at  -1.  The  cone  has  two  parts:
//! the  sides  and  the  bottom
class Vrml_Cone 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Vrml_Cone(const Vrml_ConeParts aParts = Vrml_ConeALL, const Standard_Real aBottomRadius = 1, const Standard_Real aHeight = 2);
  
  Standard_EXPORT void SetParts (const Vrml_ConeParts aParts);
  
  Standard_EXPORT Vrml_ConeParts Parts() const;
  
  Standard_EXPORT void SetBottomRadius (const Standard_Real aBottomRadius);
  
  Standard_EXPORT Standard_Real BottomRadius() const;
  
  Standard_EXPORT void SetHeight (const Standard_Real aHeight);
  
  Standard_EXPORT Standard_Real Height() const;
  
  Standard_EXPORT Standard_OStream& Print (Standard_OStream& anOStream) const;




protected:





private:



  Vrml_ConeParts myParts;
  Standard_Real myBottomRadius;
  Standard_Real myHeight;


};







#endif // _Vrml_Cone_HeaderFile
