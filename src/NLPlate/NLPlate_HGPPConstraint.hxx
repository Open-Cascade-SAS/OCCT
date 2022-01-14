// Created on: 1998-04-09
// Created by: Andre LIEUTIER
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _NLPlate_HGPPConstraint_HeaderFile
#define _NLPlate_HGPPConstraint_HeaderFile

#include <Standard.hxx>

#include <gp_XY.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Real.hxx>
class gp_XYZ;
class Plate_D1;
class Plate_D2;
class Plate_D3;


class NLPlate_HGPPConstraint;
DEFINE_STANDARD_HANDLE(NLPlate_HGPPConstraint, Standard_Transient)

//! define a PinPoint geometric Constraint used to load a Non Linear Plate
class NLPlate_HGPPConstraint : public Standard_Transient
{

public:

  
  NLPlate_HGPPConstraint() : myActiveOrder(0) {}
  
  Standard_EXPORT virtual void SetUVFreeSliding (const Standard_Boolean UVFree);
  
  Standard_EXPORT virtual void SetIncrementalLoadAllowed (const Standard_Boolean ILA);
  
  Standard_EXPORT virtual void SetActiveOrder (const Standard_Integer ActiveOrder);
  
  Standard_EXPORT virtual void SetUV (const gp_XY& UV);
  
  Standard_EXPORT virtual void SetOrientation (const Standard_Integer Orient = 0);
  
  Standard_EXPORT virtual void SetG0Criterion (const Standard_Real TolDist);
  
  Standard_EXPORT virtual void SetG1Criterion (const Standard_Real TolAng);
  
  Standard_EXPORT virtual void SetG2Criterion (const Standard_Real TolCurv);
  
  Standard_EXPORT virtual void SetG3Criterion (const Standard_Real TolG3);
  
  Standard_EXPORT virtual Standard_Boolean UVFreeSliding() const;
  
  Standard_EXPORT virtual Standard_Boolean IncrementalLoadAllowed() const;
  
  Standard_EXPORT virtual Standard_Integer ActiveOrder() const = 0;
  
  Standard_EXPORT virtual const gp_XY& UV() const;
  
  Standard_EXPORT virtual Standard_Integer Orientation();
  
  Standard_EXPORT virtual Standard_Boolean IsG0() const = 0;
  
  Standard_EXPORT virtual const gp_XYZ& G0Target() const;
  
  Standard_EXPORT virtual const Plate_D1& G1Target() const;
  
  Standard_EXPORT virtual const Plate_D2& G2Target() const;
  
  Standard_EXPORT virtual const Plate_D3& G3Target() const;
  
  Standard_EXPORT virtual Standard_Real G0Criterion() const;
  
  Standard_EXPORT virtual Standard_Real G1Criterion() const;
  
  Standard_EXPORT virtual Standard_Real G2Criterion() const;
  
  Standard_EXPORT virtual Standard_Real G3Criterion() const;




  DEFINE_STANDARD_RTTIEXT(NLPlate_HGPPConstraint,Standard_Transient)

protected:


  gp_XY myUV;
  Standard_Integer myActiveOrder;


private:




};







#endif // _NLPlate_HGPPConstraint_HeaderFile
