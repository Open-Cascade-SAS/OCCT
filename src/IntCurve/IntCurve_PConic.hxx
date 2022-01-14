// Created on: 1992-03-04
// Created by: Laurent BUCHARD
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

#ifndef _IntCurve_PConic_HeaderFile
#define _IntCurve_PConic_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <gp_Ax22d.hxx>
#include <Standard_Integer.hxx>
#include <GeomAbs_CurveType.hxx>
class gp_Elips2d;
class gp_Circ2d;
class gp_Parab2d;
class gp_Hypr2d;
class gp_Lin2d;


//! This class represents a conic from gp as a
//! parametric curve ( in order to be used by the
//! class PConicTool from IntCurve).
class IntCurve_PConic 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT IntCurve_PConic(const IntCurve_PConic& PC);
  
  Standard_EXPORT IntCurve_PConic(const gp_Elips2d& E);
  
  Standard_EXPORT IntCurve_PConic(const gp_Circ2d& C);
  
  Standard_EXPORT IntCurve_PConic(const gp_Parab2d& P);
  
  Standard_EXPORT IntCurve_PConic(const gp_Hypr2d& H);
  
  Standard_EXPORT IntCurve_PConic(const gp_Lin2d& L);
  
  //! EpsX is a internal tolerance used in math
  //! algorithms, usually about 1e-10
  //! (See FunctionAllRoots for more details)
  Standard_EXPORT void SetEpsX (const Standard_Real EpsDist);
  
  //! Accuracy is the number of samples used to
  //! approximate the parametric curve on its domain.
  Standard_EXPORT void SetAccuracy (const Standard_Integer Nb);
  
    Standard_Integer Accuracy() const;
  
    Standard_Real EpsX() const;
  
  //! The Conics are manipulated as objects which only
  //! depend on three parameters : Axis and two Real from Standards.
  //! Type Curve is used to select the correct Conic.
    GeomAbs_CurveType TypeCurve() const;
  
    const gp_Ax22d& Axis2() const;
  
    Standard_Real Param1() const;
  
    Standard_Real Param2() const;




protected:





private:



  gp_Ax22d axe;
  Standard_Real prm1;
  Standard_Real prm2;
  Standard_Real TheEpsX;
  Standard_Integer TheAccuracy;
  GeomAbs_CurveType type;


};


#include <IntCurve_PConic.lxx>





#endif // _IntCurve_PConic_HeaderFile
