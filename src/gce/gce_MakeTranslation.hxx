// Created on: 1992-08-26
// Created by: Remi GILET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _gce_MakeTranslation_HeaderFile
#define _gce_MakeTranslation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Trsf.hxx>
class gp_Vec;
class gp_Pnt;


//! This class implements elementary construction algorithms for a
//! translation in 3D space. The result is a gp_Trsf transformation.
//! A MakeTranslation object provides a framework for:
//! -   defining the construction of the transformation,
//! -   implementing the construction algorithm, and
//! -   consulting the result.
class gce_MakeTranslation 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs a translation along the vector " Vect"
  Standard_EXPORT gce_MakeTranslation(const gp_Vec& Vect);
  
  //! Constructs a translation along the vector
  //! (Point1,Point2) defined from the point Point1 to the point Point2.
  Standard_EXPORT gce_MakeTranslation(const gp_Pnt& Point1, const gp_Pnt& Point2);
  

  //! Returns the constructed transformation.
  Standard_EXPORT const gp_Trsf& Value() const;
  
  Standard_EXPORT const gp_Trsf& Operator() const;
Standard_EXPORT operator gp_Trsf() const;




protected:





private:



  gp_Trsf TheTranslation;


};







#endif // _gce_MakeTranslation_HeaderFile
