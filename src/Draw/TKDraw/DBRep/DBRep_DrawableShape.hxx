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

#include <DBRep_Edge.hxx>
#include <NCollection_List.hxx>
#include <DBRep_Face.hxx>
#include <NCollection_List.hxx>
#include <DBRep_HideData.hxx>
#include <NCollection_List.hxx>
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

//! Drawable structure to display a shape. Contains a
//! list of edges and a list of faces.
class DBRep_DrawableShape : public Draw_Drawable3D
{
  DEFINE_STANDARD_RTTIEXT(DBRep_DrawableShape, Draw_Drawable3D)
  Draw_Drawable3D_FACTORY
public:
  Standard_EXPORT DBRep_DrawableShape(const TopoDS_Shape&    C,
                                      const Draw_Color&      FreeCol,
                                      const Draw_Color&      ConnCol,
                                      const Draw_Color&      EdgeCol,
                                      const Draw_Color&      IsosCol,
                                      const double    size,
                                      const int nbisos,
                                      const int discret);

  //! Changes the number of isoparametric curves in a shape.
  Standard_EXPORT void ChangeNbIsos(const int NbIsos);

  //! Returns the number of isoparametric curves in a shape.
  Standard_EXPORT int NbIsos() const;

  //! Changes the number of isoparametric curves in a shape.
  Standard_EXPORT void ChangeDiscret(const int Discret);

  //! Returns the discretisation value of curve
  Standard_EXPORT int Discret() const;

  //! Return const &
  Standard_EXPORT TopoDS_Shape Shape() const;

  //! When True the orientations of the edges and free
  //! vertices are displayed.
  Standard_EXPORT void DisplayOrientation(const bool D);

  //! When True the triangulations of the faces
  //! are displayed even if there is a surface.
  Standard_EXPORT void DisplayTriangulation(const bool D);

  //! When True the polygons of the edges
  //! are displayed even if there is a geometric curve.
  Standard_EXPORT void DisplayPolygons(const bool D);

  //! Performs Hidden lines.
  Standard_EXPORT void DisplayHLR(const bool withHLR,
                                  const bool withRg1,
                                  const bool withRgN,
                                  const bool withHid,
                                  const double    ang);

  Standard_EXPORT bool DisplayTriangulation() const;

  Standard_EXPORT bool DisplayPolygons() const;

  Standard_EXPORT void GetDisplayHLR(bool& withHLR,
                                     bool& withRg1,
                                     bool& withRgN,
                                     bool& withHid,
                                     double&    ang) const;

  Standard_EXPORT void DrawOn(Draw_Display& dis) const override;

  Standard_EXPORT void DisplayHiddenLines(Draw_Display& dis);

  //! For variable copy.
  Standard_EXPORT virtual occ::handle<Draw_Drawable3D> Copy() const override;

  //! For variable dump.
  Standard_EXPORT virtual void Dump(Standard_OStream& S) const override;

  //! Save drawable into stream.
  Standard_EXPORT virtual void Save(Standard_OStream& theStream) const override;

  //! For variable whatis command.
  Standard_EXPORT virtual void Whatis(Draw_Interpretor& I) const override;

  //! Returns the subshape touched by the last pick.
  //! u,v are the parameters of the closest point.
  Standard_EXPORT static void LastPick(TopoDS_Shape& S, double& u, double& v);

public:
  //! Auxiliary method computing nodal normals for presentation purposes.
  //! @param[out] theNormals  vector of computed normals (pair of points [from, to])
  //! @param[in] theFace      input face
  //! @param[in] theLength    normal length
  //! @return FALSE if normals can not be computed
  Standard_EXPORT static bool addMeshNormals(
    NCollection_Vector<std::pair<gp_Pnt, gp_Pnt>>& theNormals,
    const TopoDS_Face&                             theFace,
    const double                            theLength);

  //! Auxiliary method computing nodal normals for presentation purposes.
  //! @param[out] theNormals  map of computed normals (grouped per Face)
  //! @param[in] theShape     input shape which will be exploded into Faces
  //! @param[in] theLength    normal length
  Standard_EXPORT static void addMeshNormals(
    NCollection_DataMap<TopoDS_Face, NCollection_Vector<std::pair<gp_Pnt, gp_Pnt>>>& theNormals,
    const TopoDS_Shape&                                                              theShape,
    const double                                                              theLength);

  //! Auxiliary method computing surface normals distributed within the Face for presentation
  //! purposes.
  //! @param[out] theNormals   vector of computed normals (pair of points [from, to])
  //! @param[in] theFace       input face
  //! @param[in] theLength     normal length
  //! @param[in] theNbAlongU   number along U
  //! @param[in] theNbAlongV   number along V
  //! @return FALSE if normals can not be computed
  Standard_EXPORT static bool addSurfaceNormals(
    NCollection_Vector<std::pair<gp_Pnt, gp_Pnt>>& theNormals,
    const TopoDS_Face&                             theFace,
    const double                            theLength,
    const int                         theNbAlongU,
    const int                         theNbAlongV);

  //! Auxiliary method computing surface normals distributed within the Face for presentation
  //! purposes.
  //! @param[out] theNormals   map of computed normals (grouped per Face)
  //! @param[in] theShape      input shape which will be exploded into Faces
  //! @param[in] theLength     normal length
  //! @param[in] theNbAlongU   number along U
  //! @param[in] theNbAlongV   number along V
  //! @return FALSE if normals can not be computed
  Standard_EXPORT static void addSurfaceNormals(
    NCollection_DataMap<TopoDS_Face, NCollection_Vector<std::pair<gp_Pnt, gp_Pnt>>>& theNormals,
    const TopoDS_Shape&                                                              theShape,
    const double                                                              theLength,
    const int                                                           theNbAlongU,
    const int                                                           theNbAlongV);

private:
  void display(const occ::handle<Poly_Triangulation>& T, const gp_Trsf& tr, Draw_Display& dis) const;

  //! Updates internal data necessary for display
  void updateDisplayData() const;

private:
  TopoDS_Shape myShape;

  mutable NCollection_List<occ::handle<DBRep_Edge>> myEdges;
  mutable NCollection_List<occ::handle<DBRep_Face>> myFaces;
  NCollection_List<DBRep_HideData>     myHidData;

  double    mySize;
  int myDiscret;
  Draw_Color       myFreeCol;
  Draw_Color       myConnCol;
  Draw_Color       myEdgeCol;
  Draw_Color       myIsosCol;
  int myNbIsos;
  bool myDispOr;
  bool mytriangulations;
  bool mypolygons;
  bool myHLR;
  bool myRg1;
  bool myRgN;
  bool myHid;
  double    myAng;
};

#endif // _DBRep_DrawableShape_HeaderFile
