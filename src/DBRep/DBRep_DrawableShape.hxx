// Created on: 1991-07-04
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

#ifndef _DBRep_DrawableShape_HeaderFile
#define _DBRep_DrawableShape_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Shape.hxx>
#include <DBRep_ListOfEdge.hxx>
#include <DBRep_ListOfFace.hxx>
#include <DBRep_ListOfHideData.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Draw_Color.hxx>
#include <Standard_Boolean.hxx>
#include <Draw_Drawable3D.hxx>
#include <Standard_OStream.hxx>
#include <Draw_Interpretor.hxx>
class Standard_DomainError;
class TopoDS_Shape;
class Draw_Color;
class Draw_Display;
class Poly_Triangulation;
class gp_Trsf;
class Draw_Drawable3D;


class DBRep_DrawableShape;
DEFINE_STANDARD_HANDLE(DBRep_DrawableShape, Draw_Drawable3D)

//! Drawable structure to display a  shape. Contains a
//! list of edges and a list of faces.
class DBRep_DrawableShape : public Draw_Drawable3D
{

public:

  
  Standard_EXPORT DBRep_DrawableShape(const TopoDS_Shape& C, const Draw_Color& FreeCol, const Draw_Color& ConnCol, const Draw_Color& EdgeCol, const Draw_Color& IsosCol, const Standard_Real size, const Standard_Integer nbisos, const Standard_Integer discret);
  
  Standard_EXPORT void Set (const TopoDS_Shape& C);
  
  //! Changes the number of isoparametric curves in a shape.
  Standard_EXPORT void ChangeNbIsos (const Standard_Integer NbIsos);
  
  //! Returns the number of isoparametric curves in a shape.
  Standard_EXPORT Standard_Integer NbIsos() const;
  
  //! Changes the number of isoparametric curves in a shape.
  Standard_EXPORT void ChangeDiscret (const Standard_Integer Discret);
  
  //! Returns the discretisation value of curve
  Standard_EXPORT Standard_Integer Discret() const;
  
  //! Return const &
  Standard_EXPORT TopoDS_Shape Shape() const;
  
  //! When True  the orientations  of the edges and free
  //! vertices  are displayed.
  Standard_EXPORT void DisplayOrientation (const Standard_Boolean D);
  
  //! When True  the triangulations  of the faces
  //! are displayed even if there is a surface.
  Standard_EXPORT void DisplayTriangulation (const Standard_Boolean D);
  
  //! When True  the polygons  of the edges
  //! are displayed even if there is a geometric curve.
  Standard_EXPORT void DisplayPolygons (const Standard_Boolean D);
  
  //! Performs Hidden lines.
  Standard_EXPORT void DisplayHLR (const Standard_Boolean withHLR, const Standard_Boolean withRg1, const Standard_Boolean withRgN, const Standard_Boolean withHid, const Standard_Real ang);
  
  Standard_EXPORT Standard_Boolean DisplayTriangulation() const;
  
  Standard_EXPORT Standard_Boolean DisplayPolygons() const;
  
  Standard_EXPORT void GetDisplayHLR (Standard_Boolean& withHLR, Standard_Boolean& withRg1, Standard_Boolean& withRgN, Standard_Boolean& withHid, Standard_Real& ang) const;
  
  Standard_EXPORT void DrawOn (Draw_Display& dis) const;
  
  Standard_EXPORT void DisplayHiddenLines (Draw_Display& dis);
  
  //! For variable copy.
  Standard_EXPORT virtual Handle(Draw_Drawable3D) Copy() const Standard_OVERRIDE;
  
  //! For variable dump.
  Standard_EXPORT virtual void Dump (Standard_OStream& S) const Standard_OVERRIDE;
  
  //! For variable whatis command.
  Standard_EXPORT virtual void Whatis (Draw_Interpretor& I) const Standard_OVERRIDE;
  
  //! Returns the subshape touched by the last pick.
  //! u,v are the parameters of the closest point.
  Standard_EXPORT static void LastPick (TopoDS_Shape& S, Standard_Real& u, Standard_Real& v);




  DEFINE_STANDARD_RTTI(DBRep_DrawableShape,Draw_Drawable3D)

protected:




private:

  
  Standard_EXPORT void Display (const Handle(Poly_Triangulation)& T, const gp_Trsf& tr, Draw_Display& dis) const;

  TopoDS_Shape myShape;
  DBRep_ListOfEdge myEdges;
  DBRep_ListOfFace myFaces;
  DBRep_ListOfHideData myHidData;
  Standard_Real mySize;
  Standard_Integer myDiscret;
  Draw_Color myFreeCol;
  Draw_Color myConnCol;
  Draw_Color myEdgeCol;
  Draw_Color myIsosCol;
  Standard_Integer myNbIsos;
  Standard_Boolean myDispOr;
  Standard_Boolean mytriangulations;
  Standard_Boolean mypolygons;
  Standard_Boolean myHLR;
  Standard_Boolean myRg1;
  Standard_Boolean myRgN;
  Standard_Boolean myHid;
  Standard_Real myAng;


};







#endif // _DBRep_DrawableShape_HeaderFile
