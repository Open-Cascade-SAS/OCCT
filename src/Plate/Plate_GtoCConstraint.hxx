// Created on: 1995-10-19
// Created by: Andre LIEUTIER
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

#ifndef _Plate_GtoCConstraint_HeaderFile
#define _Plate_GtoCConstraint_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Plate_PinpointConstraint.hxx>
#include <Plate_D1.hxx>
#include <gp_XY.hxx>
#include <Standard_Integer.hxx>
class gp_XYZ;
class Plate_D2;
class Plate_D3;


//! define a G1, G2  or G3 constraint on the Plate
class Plate_GtoCConstraint 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Plate_GtoCConstraint(const Plate_GtoCConstraint& ref);
  
  Standard_EXPORT Plate_GtoCConstraint(const gp_XY& point2d, const Plate_D1& D1S, const Plate_D1& D1T);
  
  Standard_EXPORT Plate_GtoCConstraint(const gp_XY& point2d, const Plate_D1& D1S, const Plate_D1& D1T, const gp_XYZ& nP);
  
  Standard_EXPORT Plate_GtoCConstraint(const gp_XY& point2d, const Plate_D1& D1S, const Plate_D1& D1T, const Plate_D2& D2S, const Plate_D2& D2T);
  
  Standard_EXPORT Plate_GtoCConstraint(const gp_XY& point2d, const Plate_D1& D1S, const Plate_D1& D1T, const Plate_D2& D2S, const Plate_D2& D2T, const gp_XYZ& nP);
  
  Standard_EXPORT Plate_GtoCConstraint(const gp_XY& point2d, const Plate_D1& D1S, const Plate_D1& D1T, const Plate_D2& D2S, const Plate_D2& D2T, const Plate_D3& D3S, const Plate_D3& D3T);
  
  Standard_EXPORT Plate_GtoCConstraint(const gp_XY& point2d, const Plate_D1& D1S, const Plate_D1& D1T, const Plate_D2& D2S, const Plate_D2& D2T, const Plate_D3& D3S, const Plate_D3& D3T, const gp_XYZ& nP);
  
    const Standard_Integer& nb_PPC() const;
  
    const Plate_PinpointConstraint& GetPPC (const Standard_Integer Index) const;
  
    const Plate_D1& D1SurfInit() const;




protected:





private:



  Plate_PinpointConstraint myPPC[9];
  Plate_D1 myD1SurfInit;
  gp_XY pnt2d;
  Standard_Integer nb_PPConstraints;


};


#include <Plate_GtoCConstraint.lxx>





#endif // _Plate_GtoCConstraint_HeaderFile
