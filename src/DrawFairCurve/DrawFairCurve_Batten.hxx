// Created on: 1996-02-16
// Created by: Philippe MANGIN
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _DrawFairCurve_Batten_HeaderFile
#define _DrawFairCurve_Batten_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Address.hxx>
#include <DrawTrSurf_BSplineCurve2d.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_OStream.hxx>
class gp_Pnt2d;


class DrawFairCurve_Batten;
DEFINE_STANDARD_HANDLE(DrawFairCurve_Batten, DrawTrSurf_BSplineCurve2d)

//! Interactive Draw object of type "Batten"
class DrawFairCurve_Batten : public DrawTrSurf_BSplineCurve2d
{

public:

  
  Standard_EXPORT DrawFairCurve_Batten(const Standard_Address TheBatten);
  
  Standard_EXPORT void Compute();
  
  Standard_EXPORT void SetPoint (const Standard_Integer Side, const gp_Pnt2d& Point);
  
  Standard_EXPORT void SetAngle (const Standard_Integer Side, const Standard_Real Angle);
  
  Standard_EXPORT void SetSliding (const Standard_Real Length);
  
  Standard_EXPORT void SetHeight (const Standard_Real Heigth);
  
  Standard_EXPORT void SetSlope (const Standard_Real Slope);
  
  Standard_EXPORT Standard_Real GetAngle (const Standard_Integer Side) const;
  
  Standard_EXPORT Standard_Real GetSliding() const;
  
  Standard_EXPORT void FreeSliding();
  
  Standard_EXPORT void FreeAngle (const Standard_Integer Side);
  
  //! For variable dump.
  Standard_EXPORT virtual void Dump (Standard_OStream& S) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(DrawFairCurve_Batten,DrawTrSurf_BSplineCurve2d)

protected:


  Standard_Address MyBatten;


private:




};







#endif // _DrawFairCurve_Batten_HeaderFile
