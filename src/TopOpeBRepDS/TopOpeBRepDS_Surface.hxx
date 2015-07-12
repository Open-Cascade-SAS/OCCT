// Created on: 1993-06-23
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _TopOpeBRepDS_Surface_HeaderFile
#define _TopOpeBRepDS_Surface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
class Geom_Surface;


//! A Geom surface and a tolerance.
class TopOpeBRepDS_Surface 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRepDS_Surface();
  
  Standard_EXPORT TopOpeBRepDS_Surface(const Handle(Geom_Surface)& P, const Standard_Real T);
  
  Standard_EXPORT TopOpeBRepDS_Surface(const TopOpeBRepDS_Surface& Other);
  
  Standard_EXPORT void Assign (const TopOpeBRepDS_Surface& Other);
void operator= (const TopOpeBRepDS_Surface& Other)
{
  Assign(Other);
}
  
  Standard_EXPORT const Handle(Geom_Surface)& Surface() const;
  
  Standard_EXPORT Standard_Real Tolerance() const;
  
  //! Update the tolerance
  Standard_EXPORT void Tolerance (const Standard_Real tol);
  
  Standard_EXPORT Standard_Boolean Keep() const;
  
  Standard_EXPORT void ChangeKeep (const Standard_Boolean B);




protected:





private:



  Handle(Geom_Surface) mySurface;
  Standard_Real myTolerance;
  Standard_Boolean myKeep;


};







#endif // _TopOpeBRepDS_Surface_HeaderFile
