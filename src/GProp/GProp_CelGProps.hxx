// Created on: 1992-12-02
// Created by: Isabelle GRIGNON
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

#ifndef _GProp_CelGProps_HeaderFile
#define _GProp_CelGProps_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <GProp_GProps.hxx>
class gp_Circ;
class gp_Pnt;
class gp_Lin;



//! Computes the  global properties of bounded curves
//! in 3D space.
//! It can be an elementary curve from package gp such as
//! Lin, Circ, Elips, Parab .
class GProp_CelGProps  : public GProp_GProps
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT GProp_CelGProps();
  
  Standard_EXPORT GProp_CelGProps(const gp_Circ& C, const gp_Pnt& CLocation);
  
  Standard_EXPORT GProp_CelGProps(const gp_Circ& C, const Standard_Real U1, const Standard_Real U2, const gp_Pnt& CLocation);
  
  Standard_EXPORT GProp_CelGProps(const gp_Lin& C, const Standard_Real U1, const Standard_Real U2, const gp_Pnt& CLocation);
  
  Standard_EXPORT void SetLocation (const gp_Pnt& CLocation);
  
  Standard_EXPORT void Perform (const gp_Circ& C, const Standard_Real U1, const Standard_Real U2);
  
  Standard_EXPORT void Perform (const gp_Lin& C, const Standard_Real U1, const Standard_Real U2);




protected:





private:





};







#endif // _GProp_CelGProps_HeaderFile
