// Created on: 2014-05-22
// Created by: Varvara POSKONINA
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#ifndef _SelectMgr_TriangularFrustumSet_HeaderFile
#define _SelectMgr_TriangularFrustumSet_HeaderFile

#include <NCollection_Handle.hxx>
#include <NCollection_List.hxx>

#include <SelectMgr_BaseFrustum.hxx>
#include <SelectMgr_TriangularFrustum.hxx>

typedef NCollection_Handle<SelectMgr_TriangularFrustum> SelectMgr_HTriangularFrustum;
typedef NCollection_List<SelectMgr_HTriangularFrustum> SelectMgr_TriangFrustums;
typedef NCollection_List<SelectMgr_HTriangularFrustum>::Iterator SelectMgr_TriangFrustumsIter;

//! This class is used to handle polyline selection. The main principle of polyline selection
//! algorithm is to split the polygon defined by polyline onto triangles. Than each of
//! them is considered as a base for triangular frustum building. In other
//! words, each triangle vertiex will be projected from 2d screen space to 3d world space
//! onto near and far view frustum planes. Thus, the projected triangles make up the bases of
//! selecting frustum. When the set of such frustums is created, the function determining
//! selection iterates through triangular frustum set and searches for overlap with any
//! frustum.
class SelectMgr_TriangularFrustumSet : public SelectMgr_BaseFrustum
{
public:

  SelectMgr_TriangularFrustumSet() {};

  ~SelectMgr_TriangularFrustumSet() {};

  //! Meshes polygon bounded by polyline. Than organizes a set of triangular frustums,
  //! where each triangle's projection onto near and far view frustum planes is
  //! considered as a frustum base
  virtual void Build (const TColgp_Array1OfPnt2d& thePoints) Standard_OVERRIDE;

  //! Returns a copy of the frustum with all sub-volumes transformed according to the matrix given
  virtual NCollection_Handle<SelectMgr_BaseFrustum> ScaleAndTransform (const Standard_Integer theScale,
                                                                       const gp_Trsf& theTrsf) Standard_OVERRIDE;

  virtual Standard_Boolean Overlaps (const SelectMgr_Vec3& theMinPnt,
                                     const SelectMgr_Vec3& theMaxPnt,
                                     Standard_Real& theDepth) Standard_OVERRIDE;

  virtual Standard_Boolean Overlaps (const SelectMgr_Vec3& theMinPnt,
                                     const SelectMgr_Vec3& theMaxPnt,
                                     Standard_Boolean* theInside) Standard_OVERRIDE;

  virtual Standard_Boolean Overlaps (const gp_Pnt& thePnt,
                                     Standard_Real& theDepth) Standard_OVERRIDE;

  virtual Standard_Boolean Overlaps (const Handle(TColgp_HArray1OfPnt)& theArrayOfPnts,
                                     Select3D_TypeOfSensitivity theSensType,
                                     Standard_Real& theDepth) Standard_OVERRIDE;

  virtual Standard_Boolean Overlaps (const gp_Pnt& thePnt1,
                                     const gp_Pnt& thePnt2,
                                     Standard_Real& theDepth) Standard_OVERRIDE;

  virtual Standard_Boolean Overlaps (const gp_Pnt& thePnt1,
                                     const gp_Pnt& thePnt2,
                                     const gp_Pnt& thePnt3,
                                     Select3D_TypeOfSensitivity theSensType,
                                     Standard_Real& theDepth) Standard_OVERRIDE;

private:

    SelectMgr_TriangFrustums myFrustums;
};

#endif // _SelectMgr_TriangularFrustumSet_HeaderFile
