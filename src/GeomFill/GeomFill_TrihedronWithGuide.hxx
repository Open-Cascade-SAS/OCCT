// Created on: 1998-07-08
// Created by: Stephanie HUMEAU
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

#ifndef _GeomFill_TrihedronWithGuide_HeaderFile
#define _GeomFill_TrihedronWithGuide_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Pnt.hxx>
#include <GeomFill_TrihedronLaw.hxx>
#include <Standard_Real.hxx>
class Adaptor3d_HCurve;
class Standard_OutOfRange;
class Standard_NotImplemented;
class gp_Pnt;


class GeomFill_TrihedronWithGuide;
DEFINE_STANDARD_HANDLE(GeomFill_TrihedronWithGuide, GeomFill_TrihedronLaw)

//! To define Trihedron along one Curve with a guide
class GeomFill_TrihedronWithGuide : public GeomFill_TrihedronLaw
{

public:

  
  Standard_EXPORT virtual Handle(Adaptor3d_HCurve) Guide() const = 0;
  
  Standard_EXPORT virtual void Origine (const Standard_Real Param1, const Standard_Real Param2) = 0;
  
  //! Returns the current point on guide
  //! found by D0, D1 or D2.
  Standard_EXPORT gp_Pnt CurrentPointOnGuide() const;




  DEFINE_STANDARD_RTTIEXT(GeomFill_TrihedronWithGuide,GeomFill_TrihedronLaw)

protected:


  Handle(Adaptor3d_HCurve) myGuide;
  Handle(Adaptor3d_HCurve) myTrimG;
  gp_Pnt myCurPointOnGuide;


private:




};







#endif // _GeomFill_TrihedronWithGuide_HeaderFile
