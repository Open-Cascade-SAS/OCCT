// Created on: 1996-03-06
// Created by: Laurent BOURESCHE
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

#ifndef _ChFiDS_CircSection_HeaderFile
#define _ChFiDS_CircSection_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Circ.hxx>
#include <gp_Lin.hxx>


//! A Section of fillet.
class ChFiDS_CircSection 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT ChFiDS_CircSection();
  
  Standard_EXPORT void Set (const gp_Circ& C, const Standard_Real F, const Standard_Real L);
  
  Standard_EXPORT void Set (const gp_Lin& C, const Standard_Real F, const Standard_Real L);
  
  Standard_EXPORT void Get (gp_Circ& C, Standard_Real& F, Standard_Real& L) const;
  
  Standard_EXPORT void Get (gp_Lin& C, Standard_Real& F, Standard_Real& L) const;




protected:





private:



  gp_Circ myCirc;
  gp_Lin myLin;
  Standard_Real myF;
  Standard_Real myL;


};







#endif // _ChFiDS_CircSection_HeaderFile
