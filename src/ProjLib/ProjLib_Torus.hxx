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

#ifndef _ProjLib_Torus_HeaderFile
#define _ProjLib_Torus_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <gp_Torus.hxx>
#include <ProjLib_Projector.hxx>
class gp_Circ;
class gp_Lin;
class gp_Elips;
class gp_Parab;
class gp_Hypr;

//! Projects elementary curves on a torus.
class ProjLib_Torus : public ProjLib_Projector
{
public:
  DEFINE_STANDARD_ALLOC

  //! Undefined projection.
  Standard_EXPORT ProjLib_Torus();

  //! Projection on the torus <To>.
  Standard_EXPORT ProjLib_Torus(const gp_Torus& To);

  //! Projection of the circle <C> on the torus <To>.
  Standard_EXPORT ProjLib_Torus(const gp_Torus& To, const gp_Circ& C);

  Standard_EXPORT void Init(const gp_Torus& To);

  Standard_EXPORT virtual void Project(const gp_Lin& L) Standard_OVERRIDE;

  Standard_EXPORT virtual void Project(const gp_Circ& C) Standard_OVERRIDE;

  Standard_EXPORT virtual void Project(const gp_Elips& E) Standard_OVERRIDE;

  Standard_EXPORT virtual void Project(const gp_Parab& P) Standard_OVERRIDE;

  Standard_EXPORT virtual void Project(const gp_Hypr& H) Standard_OVERRIDE;

protected:
private:
  gp_Torus myTorus;
};

#endif // _ProjLib_Torus_HeaderFile
