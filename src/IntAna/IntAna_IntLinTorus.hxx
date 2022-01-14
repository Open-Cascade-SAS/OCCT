// Created on: 1991-05-16
// Created by: Isabelle GRIGNON
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _IntAna_IntLinTorus_HeaderFile
#define _IntAna_IntLinTorus_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Standard_Integer.hxx>
#include <gp_Pnt.hxx>
class gp_Lin;
class gp_Torus;


//! Intersection between a line and a torus.
class IntAna_IntLinTorus 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT IntAna_IntLinTorus();
  
  //! Creates the intersection between a line and a torus.
  Standard_EXPORT IntAna_IntLinTorus(const gp_Lin& L, const gp_Torus& T);
  
  //! Intersects a line and a torus.
  Standard_EXPORT void Perform (const gp_Lin& L, const gp_Torus& T);
  
  //! Returns True if the computation was successful.
    Standard_Boolean IsDone() const;
  
  //! Returns the number of intersection points.
    Standard_Integer NbPoints() const;
  
  //! Returns the intersection point of range Index.
    const gp_Pnt& Value (const Standard_Integer Index) const;
  
  //! Returns the parameter on the line of the intersection
  //! point of range Index.
    Standard_Real ParamOnLine (const Standard_Integer Index) const;
  
  //! Returns the parameters on the torus of the intersection
  //! point of range Index.
    void ParamOnTorus (const Standard_Integer Index, Standard_Real& FI, Standard_Real& THETA) const;




protected:





private:



  Standard_Boolean done;
  Standard_Integer nbpt;
  gp_Pnt thePoint[4];
  Standard_Real theParam[4];
  Standard_Real theFi[4];
  Standard_Real theTheta[4];


};


#include <IntAna_IntLinTorus.lxx>





#endif // _IntAna_IntLinTorus_HeaderFile
