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

#ifndef _DrawTrSurf_Surface_HeaderFile
#define _DrawTrSurf_Surface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Draw_Color.hxx>
#include <Standard_Integer.hxx>
#include <DrawTrSurf_Drawable.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_OStream.hxx>
#include <Draw_Interpretor.hxx>
class Geom_Surface;
class Draw_Color;
class Draw_Display;
class Draw_Drawable3D;


class DrawTrSurf_Surface;
DEFINE_STANDARD_HANDLE(DrawTrSurf_Surface, DrawTrSurf_Drawable)

//! This class defines a drawable surface.
//! With this class you can draw a general surface from
//! package Geom.
class DrawTrSurf_Surface : public DrawTrSurf_Drawable
{

public:

  
  //! default drawing mode
  //! Just the middle isoparametric curves are drawn.
  //! The boundaries are yellow, the isoparametric curves are blues.
  //! For the discretisation 50 points are computed in each parametric
  //! direction.
  Standard_EXPORT DrawTrSurf_Surface(const Handle(Geom_Surface)& S);
  
  Standard_EXPORT DrawTrSurf_Surface(const Handle(Geom_Surface)& S, const Standard_Integer Nu, const Standard_Integer Nv, const Draw_Color& BoundsColor, const Draw_Color& IsosColor, const Standard_Integer Discret, const Standard_Real Deflection, const Standard_Integer DrawMode);
  
    Draw_Color BoundsColor() const;
  
  //! rub out all the isoparametric curves.
  Standard_EXPORT virtual void ClearIsos();
  
  Standard_EXPORT void DrawOn (Draw_Display& dis) const Standard_OVERRIDE;
  
  //! Iso = True : Draw the isos, the boundaries, the UVMarker.
  //! Iso = False: Only Draw the boundary and the UVMarker.
  Standard_EXPORT void DrawOn (Draw_Display& dis, const Standard_Boolean Iso) const;
  
    Handle(Geom_Surface) GetSurface() const;
  
    Draw_Color IsosColor() const;
  
    void NbIsos (Standard_Integer& Nu, Standard_Integer& Nb) const;
  
    void SetBoundsColor (const Draw_Color& aColor);
  
    void SetIsosColor (const Draw_Color& aColor);
  
  //! change the number of isoparametric curves to be drawn.
  Standard_EXPORT virtual void ShowIsos (const Standard_Integer Nu, const Standard_Integer Nv);
  
  //! For variable copy.
  Standard_EXPORT virtual Handle(Draw_Drawable3D) Copy() const Standard_OVERRIDE;
  
  //! For variable dump.
  Standard_EXPORT virtual void Dump (Standard_OStream& S) const Standard_OVERRIDE;
  
  //! For variable whatis command.
  Standard_EXPORT virtual void Whatis (Draw_Interpretor& I) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(DrawTrSurf_Surface,DrawTrSurf_Drawable)

protected:


  Handle(Geom_Surface) surf;
  Draw_Color boundsLook;
  Draw_Color isosLook;
  Standard_Integer nbUIsos;
  Standard_Integer nbVIsos;


private:




};


#include <DrawTrSurf_Surface.lxx>





#endif // _DrawTrSurf_Surface_HeaderFile
