// Created on: 1992-05-21
// Created by: Jean Claude VAUTHIER
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

#ifndef _DrawTrSurf_Curve_HeaderFile
#define _DrawTrSurf_Curve_HeaderFile

#include <Draw_Color.hxx>
#include <DrawTrSurf_Drawable.hxx>
#include <Draw_Interpretor.hxx>

class Geom_Curve;
class Draw_Display;

//! This class defines a drawable curve in 3d space.
class DrawTrSurf_Curve : public DrawTrSurf_Drawable
{
  DEFINE_STANDARD_RTTIEXT(DrawTrSurf_Curve, DrawTrSurf_Drawable)
  Draw_Drawable3D_FACTORY
public:
  //! creates a drawable curve from a curve of package Geom.
  Standard_EXPORT DrawTrSurf_Curve(const occ::handle<Geom_Curve>& C,
                                   const bool    DispOrigin = true);

  Standard_EXPORT DrawTrSurf_Curve(const occ::handle<Geom_Curve>& C,
                                   const Draw_Color&         aColor,
                                   const int    Discret,
                                   const double       Deflection,
                                   const int    DrawMode,
                                   const bool    DispOrigin     = true,
                                   const bool    DispCurvRadius = false,
                                   const double       RadiusMax      = 1.0e3,
                                   const double       RatioOfRadius  = 0.1);

  Standard_EXPORT virtual void DrawOn(Draw_Display& dis) const override;

  occ::handle<Geom_Curve> GetCurve() const { return curv; }

  void SetColor(const Draw_Color& theColor) { look = theColor; }

  bool DisplayOrigin() const { return disporigin; }

  void DisplayOrigin(const bool V) { disporigin = V; }

  void ShowCurvature() { dispcurvradius = true; }

  void ClearCurvature() { dispcurvradius = false; }

  void SetRadiusMax(const double theRadius) { radiusmax = theRadius; }

  void SetRadiusRatio(const double theRatio) { radiusratio = theRatio; }

  Draw_Color Color() const { return look; }

  double RadiusMax() const { return radiusmax; }

  double RadiusRatio() const { return radiusratio; }

  //! For variable copy.
  Standard_EXPORT virtual occ::handle<Draw_Drawable3D> Copy() const override;

  //! For variable dump.
  Standard_EXPORT virtual void Dump(Standard_OStream& S) const override;

  //! Save drawable into stream.
  Standard_EXPORT virtual void Save(Standard_OStream& theStream) const override;

  //! For variable whatis command. Set as a result the type of the variable.
  Standard_EXPORT virtual void Whatis(Draw_Interpretor& I) const override;

protected:
  occ::handle<Geom_Curve> curv;
  Draw_Color         look;
  bool   disporigin;
  bool   dispcurvradius;
  double      radiusmax;
  double      radiusratio;
};

#endif // _DrawTrSurf_Curve_HeaderFile
