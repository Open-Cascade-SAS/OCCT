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

DEFINE_STANDARD_HANDLE(DrawTrSurf_Curve, DrawTrSurf_Drawable)

//! This class defines a drawable curve in 3d space.
class DrawTrSurf_Curve : public DrawTrSurf_Drawable
{
  DEFINE_STANDARD_RTTIEXT(DrawTrSurf_Curve, DrawTrSurf_Drawable)
  Draw_Drawable3D_FACTORY public :

      //! creates a drawable curve from a curve of package Geom.
      Standard_EXPORT
      DrawTrSurf_Curve(const Handle(Geom_Curve)& C,
                       const Standard_Boolean    DispOrigin = Standard_True);

  Standard_EXPORT DrawTrSurf_Curve(const Handle(Geom_Curve)& C,
                                   const Draw_Color&         aColor,
                                   const Standard_Integer    Discret,
                                   const Standard_Real       Deflection,
                                   const Standard_Integer    DrawMode,
                                   const Standard_Boolean    DispOrigin     = Standard_True,
                                   const Standard_Boolean    DispCurvRadius = Standard_False,
                                   const Standard_Real       RadiusMax      = 1.0e3,
                                   const Standard_Real       RatioOfRadius  = 0.1);

  Standard_EXPORT virtual void DrawOn(Draw_Display& dis) const Standard_OVERRIDE;

  Handle(Geom_Curve) GetCurve() const { return curv; }

  void SetColor(const Draw_Color& theColor) { look = theColor; }

  Standard_Boolean DisplayOrigin() const { return disporigin; }

  void DisplayOrigin(const Standard_Boolean V) { disporigin = V; }

  void ShowCurvature() { dispcurvradius = Standard_True; }

  void ClearCurvature() { dispcurvradius = Standard_False; }

  void SetRadiusMax(const Standard_Real theRadius) { radiusmax = theRadius; }

  void SetRadiusRatio(const Standard_Real theRatio) { radiusratio = theRatio; }

  Draw_Color Color() const { return look; }

  Standard_Real RadiusMax() const { return radiusmax; }

  Standard_Real RadiusRatio() const { return radiusratio; }

  //! For variable copy.
  Standard_EXPORT virtual Handle(Draw_Drawable3D) Copy() const Standard_OVERRIDE;

  //! For variable dump.
  Standard_EXPORT virtual void Dump(Standard_OStream& S) const Standard_OVERRIDE;

  //! Save drawable into stream.
  Standard_EXPORT virtual void Save(Standard_OStream& theStream) const Standard_OVERRIDE;

  //! For variable whatis command. Set as a result the type of the variable.
  Standard_EXPORT virtual void Whatis(Draw_Interpretor& I) const Standard_OVERRIDE;

protected:
  Handle(Geom_Curve) curv;
  Draw_Color         look;
  Standard_Boolean   disporigin;
  Standard_Boolean   dispcurvradius;
  Standard_Real      radiusmax;
  Standard_Real      radiusratio;
};

#endif // _DrawTrSurf_Curve_HeaderFile
