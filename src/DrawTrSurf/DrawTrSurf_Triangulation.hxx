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

#include <TColStd_HArray1OfInteger.hxx>
#include <Draw_Drawable3D.hxx>
#include <Draw_Interpretor.hxx>

class Poly_Triangulation;

DEFINE_STANDARD_HANDLE(DrawTrSurf_Triangulation, Draw_Drawable3D)

//! Used to display a triangulation.
//!
//! Display internal edges in blue
//! Display boundary edges in red
//! Optional display of triangles and nodes indices.
class DrawTrSurf_Triangulation : public Draw_Drawable3D
{
  DEFINE_STANDARD_RTTIEXT(DrawTrSurf_Triangulation, Draw_Drawable3D)
  Draw_Drawable3D_FACTORY
public:
  
  Standard_EXPORT DrawTrSurf_Triangulation (const Handle(Poly_Triangulation)& T);

  Handle(Poly_Triangulation) Triangulation() const { return myTriangulation; }

  void ShowNodes (const Standard_Boolean theB) { myNodes = theB; }

  Standard_Boolean ShowNodes() const { return myNodes; }

  void ShowTriangles (const Standard_Boolean theB) { myTriangles = theB; }

  Standard_Boolean ShowTriangles() const { return myTriangles; }

  Standard_EXPORT virtual void DrawOn (Draw_Display& dis) const Standard_OVERRIDE;

  //! For variable copy.
  Standard_EXPORT virtual Handle(Draw_Drawable3D) Copy() const Standard_OVERRIDE;

  //! For variable dump.
  Standard_EXPORT virtual void Dump (Standard_OStream& S) const Standard_OVERRIDE;

  //! Save drawable into stream.
  Standard_EXPORT virtual void Save (Standard_OStream& theStream) const Standard_OVERRIDE;

  //! For variable whatis command. Set as a result the type of the variable.
  Standard_EXPORT virtual void Whatis (Draw_Interpretor& I) const Standard_OVERRIDE;

private:

  Handle(Poly_Triangulation) myTriangulation;
  Handle(TColStd_HArray1OfInteger) myInternals;
  Handle(TColStd_HArray1OfInteger) myFree;
  Standard_Boolean myNodes;
  Standard_Boolean myTriangles;

};

#endif // _DrawTrSurf_Triangulation_HeaderFile
