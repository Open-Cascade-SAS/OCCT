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

#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Draw_Drawable3D.hxx>
#include <Draw_Interpretor.hxx>

class Poly_Triangulation;

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
  Standard_EXPORT DrawTrSurf_Triangulation(const occ::handle<Poly_Triangulation>& T);

  occ::handle<Poly_Triangulation> Triangulation() const { return myTriangulation; }

  void ShowNodes(const bool theB) { myNodes = theB; }

  bool ShowNodes() const { return myNodes; }

  void ShowTriangles(const bool theB) { myTriangles = theB; }

  bool ShowTriangles() const { return myTriangles; }

  Standard_EXPORT virtual void DrawOn(Draw_Display& dis) const override;

  //! For variable copy.
  Standard_EXPORT virtual occ::handle<Draw_Drawable3D> Copy() const override;

  //! For variable dump.
  Standard_EXPORT virtual void Dump(Standard_OStream& S) const override;

  //! Save drawable into stream.
  Standard_EXPORT virtual void Save(Standard_OStream& theStream) const override;

  //! For variable whatis command. Set as a result the type of the variable.
  Standard_EXPORT virtual void Whatis(Draw_Interpretor& I) const override;

private:
  occ::handle<Poly_Triangulation>       myTriangulation;
  occ::handle<NCollection_HArray1<int>> myInternals;
  occ::handle<NCollection_HArray1<int>> myFree;
  bool                                  myNodes;
  bool                                  myTriangles;
};

#endif // _DrawTrSurf_Triangulation_HeaderFile
