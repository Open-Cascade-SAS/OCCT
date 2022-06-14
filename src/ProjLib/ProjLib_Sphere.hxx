// Created on: 1993-08-24
// Created by: Bruno DUMORTIER
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

#ifndef _ProjLib_Sphere_HeaderFile
#define _ProjLib_Sphere_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <gp_Sphere.hxx>
#include <ProjLib_Projector.hxx>
class gp_Circ;
class gp_Lin;
class gp_Elips;
class gp_Parab;
class gp_Hypr;


//! Projects elementary curves on a sphere.
class ProjLib_Sphere  : public ProjLib_Projector
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Undefined projection.
  Standard_EXPORT ProjLib_Sphere();
  
  //! Projection on the sphere <Sp>.
  Standard_EXPORT ProjLib_Sphere(const gp_Sphere& Sp);
  
  //! Projection of the circle <C> on the sphere <Sp>.
  Standard_EXPORT ProjLib_Sphere(const gp_Sphere& Sp, const gp_Circ& C);
  
  Standard_EXPORT void Init (const gp_Sphere& Sp);
  
  Standard_EXPORT virtual void Project (const gp_Lin& L) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Project (const gp_Circ& C) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Project (const gp_Elips& E) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Project (const gp_Parab& P) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Project (const gp_Hypr& H) Standard_OVERRIDE;
  
  //! Set the point of parameter U on C in the natural
  //! restrictions of the sphere.
  Standard_EXPORT void SetInBounds (const Standard_Real U);




protected:





private:



  gp_Sphere mySphere;


};







#endif // _ProjLib_Sphere_HeaderFile
