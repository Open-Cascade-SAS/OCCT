// Created on: 1995-12-05
// Created by: Laurent BOURESCHE
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _GeomFill_DegeneratedBound_HeaderFile
#define _GeomFill_DegeneratedBound_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Pnt.hxx>
#include <Standard_Real.hxx>
#include <GeomFill_Boundary.hxx>
#include <Standard_Boolean.hxx>
class gp_Pnt;
class gp_Vec;


class GeomFill_DegeneratedBound;
DEFINE_STANDARD_HANDLE(GeomFill_DegeneratedBound, GeomFill_Boundary)

//! Description of a degenerated boundary (a point).
//! Class defining  a degenerated  boundary   for   a
//! constrained filling   with  a   point  and  no   other
//! constraint. Only used to  simulate an  ordinary bound,
//! may not be usefull and desapear soon.
class GeomFill_DegeneratedBound : public GeomFill_Boundary
{

public:

  
  Standard_EXPORT GeomFill_DegeneratedBound(const gp_Pnt& Point, const Standard_Real First, const Standard_Real Last, const Standard_Real Tol3d, const Standard_Real Tolang);
  
  Standard_EXPORT gp_Pnt Value (const Standard_Real U) const Standard_OVERRIDE;
  
  Standard_EXPORT void D1 (const Standard_Real U, gp_Pnt& P, gp_Vec& V) const Standard_OVERRIDE;
  
  Standard_EXPORT void Reparametrize (const Standard_Real First, const Standard_Real Last, const Standard_Boolean HasDF, const Standard_Boolean HasDL, const Standard_Real DF, const Standard_Real DL, const Standard_Boolean Rev) Standard_OVERRIDE;
  
  Standard_EXPORT void Bounds (Standard_Real& First, Standard_Real& Last) const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean IsDegenerated() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(GeomFill_DegeneratedBound,GeomFill_Boundary)

protected:




private:


  gp_Pnt myPoint;
  Standard_Real myFirst;
  Standard_Real myLast;


};







#endif // _GeomFill_DegeneratedBound_HeaderFile
