// Created on: 1998-10-29
// Created by: Jean Yves LEBEY
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _TopOpeBRep_Hctxff2d_HeaderFile
#define _TopOpeBRep_Hctxff2d_HeaderFile

#include <BRepAdaptor_Surface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <TopoDS_Face.hxx>

class TopOpeBRep_Hctxff2d : public Standard_Transient
{

public:
  Standard_EXPORT TopOpeBRep_Hctxff2d();

  Standard_EXPORT void SetFaces(const TopoDS_Face& F1, const TopoDS_Face& F2);

  Standard_EXPORT void SetHSurfaces(const occ::handle<BRepAdaptor_Surface>& S1,
                                    const occ::handle<BRepAdaptor_Surface>& S2);

  Standard_EXPORT void SetTolerances(const double Tol1, const double Tol2);

  Standard_EXPORT void GetTolerances(double& Tol1, double& Tol2) const;

  Standard_EXPORT double GetMaxTolerance() const;

  Standard_EXPORT const TopoDS_Face& Face(const int I) const;

  Standard_EXPORT occ::handle<BRepAdaptor_Surface> HSurface(const int I) const;

  Standard_EXPORT bool SurfacesSameOriented() const;

  Standard_EXPORT bool FacesSameOriented() const;

  Standard_EXPORT bool FaceSameOrientedWithRef(const int I) const;

  DEFINE_STANDARD_RTTIEXT(TopOpeBRep_Hctxff2d, Standard_Transient)

private:
  Standard_EXPORT void SetHSurfacesPrivate();

  TopoDS_Face                      myFace1;
  occ::handle<BRepAdaptor_Surface> mySurface1;
  GeomAbs_SurfaceType              mySurfaceType1;
  bool                             myf1surf1F_sameoriented;
  TopoDS_Face                      myFace2;
  occ::handle<BRepAdaptor_Surface> mySurface2;
  GeomAbs_SurfaceType              mySurfaceType2;
  bool                             myf2surf1F_sameoriented;
  bool                             mySurfacesSameOriented;
  bool                             myFacesSameOriented;
  double                           myTol1;
  double                           myTol2;
};

#endif // _TopOpeBRep_Hctxff2d_HeaderFile
