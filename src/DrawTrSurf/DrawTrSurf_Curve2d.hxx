// Created on: 1992-05-22
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

#ifndef _DrawTrSurf_Curve2d_HeaderFile
#define _DrawTrSurf_Curve2d_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Draw_Color.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <DrawTrSurf_Drawable.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>
#include <Draw_Interpretor.hxx>
class Geom2d_Curve;
class Draw_Color;
class Draw_Display;
class Draw_Drawable3D;


class DrawTrSurf_Curve2d;
DEFINE_STANDARD_HANDLE(DrawTrSurf_Curve2d, DrawTrSurf_Drawable)

//! This class defines a drawable curve in 2d space.
//! The curve is drawned in the plane XOY.
class DrawTrSurf_Curve2d : public DrawTrSurf_Drawable
{

public:

  

  //! creates a drawable curve from a curve of package Geom2d.
  Standard_EXPORT DrawTrSurf_Curve2d(const Handle(Geom2d_Curve)& C, const Standard_Boolean DispOrigin = Standard_True);
  
  Standard_EXPORT DrawTrSurf_Curve2d(const Handle(Geom2d_Curve)& C, const Draw_Color& aColor, const Standard_Integer Discret, const Standard_Boolean DispOrigin = Standard_True, const Standard_Boolean DispCurvRadius = Standard_False, const Standard_Real RadiusMax = 1.0e3, const Standard_Real RatioOfRadius = 0.1);
  
  Standard_EXPORT void DrawOn (Draw_Display& dis) const Standard_OVERRIDE;
  
    Handle(Geom2d_Curve) GetCurve() const;
  
    void SetColor (const Draw_Color& aColor);
  
    void ShowCurvature();
  
    void ClearCurvature();
  
    void SetRadiusMax (const Standard_Real Radius);
  
    void SetRadiusRatio (const Standard_Real Ratio);
  
    Draw_Color Color() const;
  
    Standard_Real RadiusMax() const;
  
    Standard_Real RadiusRatio() const;
  
  //! For variable copy.
  Standard_EXPORT virtual Handle(Draw_Drawable3D) Copy() const Standard_OVERRIDE;
  
  //! For variable dump.
  Standard_EXPORT virtual void Dump (Standard_OStream& S) const Standard_OVERRIDE;
  
  //! Returns False.
  Standard_EXPORT virtual Standard_Boolean Is3D() const Standard_OVERRIDE;
  
  //! For variable whatis command. Set  as a result  the
  //! type of the variable.
  Standard_EXPORT virtual void Whatis (Draw_Interpretor& I) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(DrawTrSurf_Curve2d,DrawTrSurf_Drawable)

protected:


  Handle(Geom2d_Curve) curv;
  Draw_Color look;
  Standard_Boolean disporigin;
  Standard_Boolean dispcurvradius;
  Standard_Real radiusmax;
  Standard_Real radiusratio;


private:




};


#include <DrawTrSurf_Curve2d.lxx>





#endif // _DrawTrSurf_Curve2d_HeaderFile
