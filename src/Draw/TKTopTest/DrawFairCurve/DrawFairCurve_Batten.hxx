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

#include <DrawTrSurf_BSplineCurve2d.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_OStream.hxx>
class gp_Pnt2d;

//! Interactive Draw object of type "Batten"
class DrawFairCurve_Batten : public DrawTrSurf_BSplineCurve2d
{

public:
  Standard_EXPORT DrawFairCurve_Batten(void* const TheBatten);

  Standard_EXPORT void Compute();

  Standard_EXPORT void SetPoint(const int Side, const gp_Pnt2d& Point);

  Standard_EXPORT void SetAngle(const int Side, const double Angle);

  Standard_EXPORT void SetSliding(const double Length);

  Standard_EXPORT void SetHeight(const double Heigth);

  Standard_EXPORT void SetSlope(const double Slope);

  Standard_EXPORT double GetAngle(const int Side) const;

  Standard_EXPORT double GetSliding() const;

  Standard_EXPORT void FreeSliding();

  Standard_EXPORT void FreeAngle(const int Side);

  //! For variable dump.
  Standard_EXPORT virtual void Dump(Standard_OStream& S) const override;

  DEFINE_STANDARD_RTTIEXT(DrawFairCurve_Batten, DrawTrSurf_BSplineCurve2d)

protected:
  void* MyBatten;

};

#endif // _DrawFairCurve_Batten_HeaderFile
