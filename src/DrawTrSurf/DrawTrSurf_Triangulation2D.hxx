// Created on: 1997-07-22
// Created by: Laurent PAINNOT
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _DrawTrSurf_Triangulation2D_HeaderFile
#define _DrawTrSurf_Triangulation2D_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_HArray1OfInteger.hxx>
#include <Draw_Drawable2D.hxx>
#include <Standard_OStream.hxx>
#include <Draw_Interpretor.hxx>
class Poly_Triangulation;
class Draw_Display;
class Draw_Drawable3D;


class DrawTrSurf_Triangulation2D;
DEFINE_STANDARD_HANDLE(DrawTrSurf_Triangulation2D, Draw_Drawable2D)

//! Used to display a 2d triangulation.
//!
//! Display internal edges in blue
//! Display boundary edges in red
//! Optional display of triangles and nodes indices.
class DrawTrSurf_Triangulation2D : public Draw_Drawable2D
{

public:

  
  Standard_EXPORT DrawTrSurf_Triangulation2D(const Handle(Poly_Triangulation)& T);
  
  Standard_EXPORT Handle(Poly_Triangulation) Triangulation() const;
  
  Standard_EXPORT void DrawOn (Draw_Display& dis) const Standard_OVERRIDE;
  
  //! For variable copy.
  Standard_EXPORT virtual Handle(Draw_Drawable3D) Copy() const Standard_OVERRIDE;
  
  //! For variable dump.
  Standard_EXPORT virtual void Dump (Standard_OStream& S) const Standard_OVERRIDE;
  
  //! For variable whatis command. Set  as a result  the
  //! type of the variable.
  Standard_EXPORT virtual void Whatis (Draw_Interpretor& I) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(DrawTrSurf_Triangulation2D,Draw_Drawable2D)

protected:




private:


  Handle(Poly_Triangulation) myTriangulation;
  Handle(TColStd_HArray1OfInteger) myInternals;
  Handle(TColStd_HArray1OfInteger) myFree;


};







#endif // _DrawTrSurf_Triangulation2D_HeaderFile
