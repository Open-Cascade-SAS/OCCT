// Created on: 1995-03-06
// Created by: Laurent PAINNOT
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _DrawTrSurf_Triangulation_HeaderFile
#define _DrawTrSurf_Triangulation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_HArray1OfInteger.hxx>
#include <Standard_Boolean.hxx>
#include <Draw_Drawable3D.hxx>
#include <Standard_OStream.hxx>
#include <Draw_Interpretor.hxx>
class Poly_Triangulation;
class Draw_Display;
class Draw_Drawable3D;


class DrawTrSurf_Triangulation;
DEFINE_STANDARD_HANDLE(DrawTrSurf_Triangulation, Draw_Drawable3D)

//! Used to display a triangulation.
//!
//! Display internal edges in blue
//! Display boundary edges in red
//! Optional display of triangles and nodes indices.
class DrawTrSurf_Triangulation : public Draw_Drawable3D
{

public:

  
  Standard_EXPORT DrawTrSurf_Triangulation(const Handle(Poly_Triangulation)& T);
  
  Standard_EXPORT Handle(Poly_Triangulation) Triangulation() const;
  
  Standard_EXPORT void ShowNodes (const Standard_Boolean B);
  
  Standard_EXPORT Standard_Boolean ShowNodes() const;
  
  Standard_EXPORT void ShowTriangles (const Standard_Boolean B);
  
  Standard_EXPORT Standard_Boolean ShowTriangles() const;
  
  Standard_EXPORT void DrawOn (Draw_Display& dis) const Standard_OVERRIDE;
  
  //! For variable copy.
  Standard_EXPORT virtual Handle(Draw_Drawable3D) Copy() const Standard_OVERRIDE;
  
  //! For variable dump.
  Standard_EXPORT virtual void Dump (Standard_OStream& S) const Standard_OVERRIDE;
  
  //! For variable whatis command. Set  as a result  the
  //! type of the variable.
  Standard_EXPORT virtual void Whatis (Draw_Interpretor& I) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(DrawTrSurf_Triangulation,Draw_Drawable3D)

protected:




private:


  Handle(Poly_Triangulation) myTriangulation;
  Handle(TColStd_HArray1OfInteger) myInternals;
  Handle(TColStd_HArray1OfInteger) myFree;
  Standard_Boolean myNodes;
  Standard_Boolean myTriangles;


};







#endif // _DrawTrSurf_Triangulation_HeaderFile
