// Created on: 1991-07-16
// Created by: Christophe MARION
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _DrawTrSurf_Drawable_HeaderFile
#define _DrawTrSurf_Drawable_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Draw_Drawable3D.hxx>
#include <GeomAbs_IsoType.hxx>
class Adaptor2d_Curve2d;
class Draw_Display;
class Adaptor3d_Curve;
class Adaptor3d_IsoCurve;


class DrawTrSurf_Drawable;
DEFINE_STANDARD_HANDLE(DrawTrSurf_Drawable, Draw_Drawable3D)

//! this  class adds   to   the Drawable3D methods  to
//! display Curves and Curves on Surface.
//!
//! The discretisation,  number  of points on a Curve,
//! is stored in this class.
class DrawTrSurf_Drawable : public Draw_Drawable3D
{

public:

  
  //! Draw a polygon of the curve on the Display
  Standard_EXPORT void DrawCurve2dOn (Adaptor2d_Curve2d& C, Draw_Display& D) const;
  
  //! Draw a polygon of the curve on the Display
  Standard_EXPORT void DrawCurveOn (Adaptor3d_Curve& C, Draw_Display& D) const;
  
  //! Load C  with the specified iso  and Draw a polygon
  //! of the curve on the Display
  Standard_EXPORT void DrawIsoCurveOn (Adaptor3d_IsoCurve& C, const GeomAbs_IsoType T, const Standard_Real P, const Standard_Real F, const Standard_Real L, Draw_Display& D) const;
  
  //! this is defined only to tell C++ not to complain
  //! about inheriting a pure virtual method.
  Standard_EXPORT virtual void DrawOn (Draw_Display& dis) const Standard_OVERRIDE = 0;
  
    void SetDiscretisation (const Standard_Integer Discret);
  
    Standard_Integer GetDiscretisation() const;
  
    void SetDeflection (const Standard_Real Deflection);
  
    Standard_Real GetDeflection() const;
  
    void SetDrawMode (const Standard_Integer DrawMode);
  
    Standard_Integer GetDrawMode() const;




  DEFINE_STANDARD_RTTIEXT(DrawTrSurf_Drawable,Draw_Drawable3D)

protected:

  
  //! set the number of points on a curve at creation.
  Standard_EXPORT DrawTrSurf_Drawable(const Standard_Integer discret, const Standard_Real deflection = 0.01, const Standard_Integer DrawMode = 0);



private:


  Standard_Integer myDrawMode;
  Standard_Integer myDiscret;
  Standard_Real myDeflection;


};


#include <DrawTrSurf_Drawable.lxx>





#endif // _DrawTrSurf_Drawable_HeaderFile
