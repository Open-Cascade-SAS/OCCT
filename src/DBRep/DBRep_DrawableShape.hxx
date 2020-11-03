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

#include <DBRep_ListOfEdge.hxx>
#include <DBRep_ListOfFace.hxx>
#include <DBRep_ListOfHideData.hxx>
#include <Draw_Color.hxx>
#include <Draw_Drawable3D.hxx>
#include <Draw_Interpretor.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_Vector.hxx>
#include <Standard_OStream.hxx>
#include <TopoDS_Shape.hxx>

class Draw_Display;
class Poly_Triangulation;
class gp_Trsf;

//! Drawable structure to display a  shape. Contains a
//! list of edges and a list of faces.
class DBRep_DrawableShape : public Draw_Drawable3D
{
  DEFINE_STANDARD_RTTIEXT(DBRep_DrawableShape, Draw_Drawable3D)
  Draw_Drawable3D_FACTORY
public:

  Standard_EXPORT DBRep_DrawableShape(const TopoDS_Shape& C, const Draw_Color& FreeCol, const Draw_Color& ConnCol, const Draw_Color& EdgeCol, const Draw_Color& IsosCol, const Standard_Real size, const Standard_Integer nbisos, const Standard_Integer discret);
  
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
  
  Standard_EXPORT void DrawOn (Draw_Display& dis) const Standard_OVERRIDE;
  
  Standard_EXPORT void DisplayHiddenLines (Draw_Display& dis);
  
  //! For variable copy.
  Standard_EXPORT virtual Handle(Draw_Drawable3D) Copy() const Standard_OVERRIDE;
  
  //! For variable dump.
  Standard_EXPORT virtual void Dump (Standard_OStream& S) const Standard_OVERRIDE;

  //! Save drawable into stream.
  Standard_EXPORT virtual void Save (Standard_OStream& theStream) const Standard_OVERRIDE;

  //! For variable whatis command.
  Standard_EXPORT virtual void Whatis (Draw_Interpretor& I) const Standard_OVERRIDE;
  
  //! Returns the subshape touched by the last pick.
  //! u,v are the parameters of the closest point.
  Standard_EXPORT static void LastPick (TopoDS_Shape& S, Standard_Real& u, Standard_Real& v);

public:

  //! Auxiliary method computing nodal normals for presentation purposes.
  //! @param theNormals [out] vector of computed normals (pair of points [from, to])
  //! @param theFace    [in]  input face
  //! @param theLength  [in]  normal length
  //! @return FALSE if normals can not be computed
  Standard_EXPORT static Standard_Boolean addMeshNormals (NCollection_Vector<std::pair<gp_Pnt, gp_Pnt> >& theNormals,
                                                          const TopoDS_Face&  theFace,
                                                          const Standard_Real theLength);

  //! Auxiliary method computing nodal normals for presentation purposes.
  //! @param theNormals [out] map of computed normals (grouped per Face)
  //! @param theShape   [in]  input shape which will be exploded into Faces
  //! @param theLength  [in]  normal length
  Standard_EXPORT static void addMeshNormals (NCollection_DataMap<TopoDS_Face, NCollection_Vector<std::pair<gp_Pnt, gp_Pnt> > > & theNormals,
                                              const TopoDS_Shape& theShape,
                                              const Standard_Real theLength);

  //! Auxiliary method computing surface normals distributed within the Face for presentation purposes.
  //! @param theNormals  [out] vector of computed normals (pair of points [from, to])
  //! @param theFace     [in]  input face
  //! @param theLength   [in]  normal length
  //! @param theNbAlongU [in]  number along U
  //! @param theNbAlongV [in]  number along V
  //! @return FALSE if normals can not be computed
  Standard_EXPORT static Standard_Boolean addSurfaceNormals (NCollection_Vector<std::pair<gp_Pnt, gp_Pnt> >& theNormals,
                                                             const TopoDS_Face&     theFace,
                                                             const Standard_Real    theLength,
                                                             const Standard_Integer theNbAlongU,
                                                             const Standard_Integer theNbAlongV);

  //! Auxiliary method computing surface normals distributed within the Face for presentation purposes.
  //! @param theNormals  [out] map of computed normals (grouped per Face)
  //! @param theShape    [in]  input shape which will be exploded into Faces
  //! @param theLength   [in]  normal length
  //! @param theNbAlongU [in]  number along U
  //! @param theNbAlongV [in]  number along V
  //! @return FALSE if normals can not be computed
  Standard_EXPORT static void addSurfaceNormals (NCollection_DataMap<TopoDS_Face, NCollection_Vector<std::pair<gp_Pnt, gp_Pnt> > >& theNormals,
                                                 const TopoDS_Shape&    theShape,
                                                 const Standard_Real    theLength,
                                                 const Standard_Integer theNbAlongU,
                                                 const Standard_Integer theNbAlongV);

private:

  void display (const Handle(Poly_Triangulation)& T, const gp_Trsf& tr, Draw_Display& dis) const;

  //! Updates internal data necessary for display
  void updateDisplayData () const;

private:

  TopoDS_Shape myShape;

  mutable DBRep_ListOfEdge myEdges;
  mutable DBRep_ListOfFace myFaces;
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

DEFINE_STANDARD_HANDLE(DBRep_DrawableShape, Draw_Drawable3D)

#endif // _DBRep_DrawableShape_HeaderFile
