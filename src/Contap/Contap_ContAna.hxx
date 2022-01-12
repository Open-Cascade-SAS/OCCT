// Created on: 1993-03-04
// Created by: Jacques GOUSSARD
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

#ifndef _Contap_ContAna_HeaderFile
#define _Contap_ContAna_HeaderFile

#include <GeomAbs_CurveType.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <Standard.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class gp_Sphere;
class gp_Cylinder;
class gp_Cone;
class gp_Lin;

//! This class provides the computation of the contours
//! for quadric surfaces.
class Contap_ContAna 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Contap_ContAna();
  
  Standard_EXPORT void Perform (const gp_Sphere& S, const gp_Dir& D);
  
  Standard_EXPORT void Perform (const gp_Sphere& S, const gp_Dir& D, const Standard_Real Ang);
  
  Standard_EXPORT void Perform (const gp_Sphere& S, const gp_Pnt& Eye);
  
  Standard_EXPORT void Perform (const gp_Cylinder& C, const gp_Dir& D);
  
  Standard_EXPORT void Perform (const gp_Cylinder& C, const gp_Dir& D, const Standard_Real Ang);
  
  Standard_EXPORT void Perform (const gp_Cylinder& C, const gp_Pnt& Eye);
  
  Standard_EXPORT void Perform (const gp_Cone& C, const gp_Dir& D);
  
  Standard_EXPORT void Perform (const gp_Cone& C, const gp_Dir& D, const Standard_Real Ang);
  
  Standard_EXPORT void Perform (const gp_Cone& C, const gp_Pnt& Eye);
  
    Standard_Boolean IsDone() const;
  
    Standard_Integer NbContours() const;
  
  //! Returns GeomAbs_Line or GeomAbs_Circle, when
  //! IsDone() returns True.
    GeomAbs_CurveType TypeContour() const;
  
    gp_Circ Circle() const;
  
  Standard_EXPORT gp_Lin Line (const Standard_Integer Index) const;




protected:





private:



  Standard_Boolean done;
  Standard_Integer nbSol;
  GeomAbs_CurveType typL;
  gp_Pnt pt1;
  gp_Pnt pt2;
  gp_Pnt pt3;
  gp_Pnt pt4;
  gp_Dir dir1;
  gp_Dir dir2;
  gp_Dir dir3;
  gp_Dir dir4;
  Standard_Real prm;


};


#include <Contap_ContAna.lxx>





#endif // _Contap_ContAna_HeaderFile
