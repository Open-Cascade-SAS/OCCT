// Created on: 2015-06-18
// Created by: Anton POLETAEV
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _Graphic3d_TransformPers_HeaderFile
#define _Graphic3d_TransformPers_HeaderFile

#include <Bnd_Box.hxx>
#include <BVH_Box.hxx>
#include <Graphic3d_Camera.hxx>
#include <Graphic3d_TransformUtils.hxx>
#include <Graphic3d_TransModeFlags.hxx>
#include <NCollection_Mat4.hxx>
#include <NCollection_Vec4.hxx>

//! Class for keeping and computing transformation persistence.
class Graphic3d_TransformPers
{
public:

  DEFINE_STANDARD_ALLOC

  //! Default constructor.
  Graphic3d_TransformPers()
  : Flags (Graphic3d_TMF_None),
    Point (0.0, 0.0, 0.0) {}

  //! Transformation persistence mode flags.
  Graphic3d_TransModeFlags Flags;

  //! Reference point for transformation.
  Graphic3d_Vec3d Point;

public:

  //! Apply transformation to bounding box of presentation.
  //! @param theCamera [in] camera definition
  //! @param theProjection [in] the projection transformation matrix.
  //! @param theWorldView [in] the world view transformation matrix.
  //! @param theViewportWidth [in] the width of viewport (for 2d persistence).
  //! @param theViewportHeight [in] the height of viewport (for 2d persistence).
  //! @param theBoundingBox [in/out] the bounding box to transform.
  template<class T>
  void Apply (const Handle(Graphic3d_Camera)& theCamera,
              const NCollection_Mat4<T>& theProjection,
              const NCollection_Mat4<T>& theWorldView,
              const Standard_Integer theViewportWidth,
              const Standard_Integer theViewportHeight,
              Bnd_Box& theBoundingBox) const;

  //! Apply transformation to bounding box of presentation
  //! @param theCamera [in] camera definition
  //! @param theProjection [in] the projection transformation matrix.
  //! @param theWorldView [in] the world view transformation matrix.
  //! @param theViewportWidth [in] the width of viewport (for 2d persistence).
  //! @param theViewportHeight [in] the height of viewport (for 2d persistence).
  //! @param theBoundingBox [in/out] the bounding box to transform.
  template<class T>
  void Apply (const Handle(Graphic3d_Camera)& theCamera,
              const NCollection_Mat4<T>& theProjection,
              const NCollection_Mat4<T>& theWorldView,
              const Standard_Integer theViewportWidth,
              const Standard_Integer theViewportHeight,
              BVH_Box<T, 4>& theBoundingBox) const;

  //! Compute transformation.
  //! Computed matrix can be applied to model world transformation
  //! of an object to implement effect of transformation persistence.
  //! @param theCamera [in] camera definition
  //! @param theProjection [in] the projection transformation matrix.
  //! @param theWorldView [in] the world view transformation matrix.
  //! @param theViewportWidth [in] the width of viewport (for 2d persistence).
  //! @param theViewportHeight [in] the height of viewport (for 2d persistence).
  //! @return transformation matrix to be applied to model world transformation of an object.
  template<class T>
  NCollection_Mat4<T> Compute (const Handle(Graphic3d_Camera)& theCamera,
                               const NCollection_Mat4<T>& theProjection,
                               const NCollection_Mat4<T>& theWorldView,
                               const Standard_Integer theViewportWidth,
                               const Standard_Integer theViewportHeight) const;

  template<class T>
  void Apply (const Handle(Graphic3d_Camera)& theCamera,
              NCollection_Mat4<T>& theProjection,
              NCollection_Mat4<T>& theWorldView,
              const Standard_Integer theViewportWidth,
              const Standard_Integer theViewportHeight) const;
};

// =======================================================================
// function : Apply
// purpose  : Apply transformation to world view and projection matrices.
// =======================================================================
template<class T>
void Graphic3d_TransformPers::Apply (const Handle(Graphic3d_Camera)& theCamera,
                                     NCollection_Mat4<T>& theProjection,
                                     NCollection_Mat4<T>& theWorldView,
                                     const Standard_Integer theViewportWidth,
                                     const Standard_Integer theViewportHeight) const
{
  (void )theViewportWidth;
  if (!Flags)
  {
    return;
  }

  if (Flags == Graphic3d_TMF_TriedronPers)
  {
    // reset Z focus for trihedron persistence
    const Standard_Real aFocus = theCamera->IsOrthographic()
                               ? theCamera->Distance()
                               : (theCamera->ZFocusType() == Graphic3d_Camera::FocusType_Relative
                                ? Standard_Real(theCamera->ZFocus() * theCamera->Distance())
                                : Standard_Real(theCamera->ZFocus()));

    // scale factor to pixels
    const gp_XYZ aViewDim = theCamera->ViewDimensions (aFocus);
    const Standard_Real aScale = Abs(aViewDim.Y()) / Standard_Real(theViewportHeight);

    // offset from the corner
    const Standard_Real anOffset = Point.z() * aScale;

    const gp_Dir aForward (theCamera->Center().XYZ() - theCamera->Eye().XYZ());
    gp_XYZ aCenter = theCamera->Center().XYZ() + aForward.XYZ() * (aFocus - theCamera->Distance());
    if (Point.x() != 0.0)
    {
      const gp_Dir aSide = aForward.Crossed (theCamera->Up());
      if (Point.x() > 0.0)
      {
        aCenter += aSide.XYZ() * (Abs(aViewDim.X()) * 0.5 - anOffset);
      }
      else
      {
        aCenter -= aSide.XYZ() * (Abs(aViewDim.X()) * 0.5 - anOffset);
      }
    }
    if (Point.y() != 0.0)
    {
      if (Point.y() > 0.0)
      {
        aCenter += theCamera->Up().XYZ() * (Abs(aViewDim.Y()) * 0.5 - anOffset);
      }
      else
      {
        aCenter -= theCamera->Up().XYZ() * (Abs(aViewDim.Y()) * 0.5 - anOffset);
      }
    }

    Graphic3d_TransformUtils::Translate (theWorldView, T(aCenter.X()), T(aCenter.Y()), T(aCenter.Z()));
    Graphic3d_TransformUtils::Scale     (theWorldView, T(aScale),      T(aScale),      T(aScale));
    return;
  }
  else if (Flags == Graphic3d_TMF_2d)
  {
    const Standard_Real aFocus = theCamera->IsOrthographic()
                               ? theCamera->Distance()
                               : (theCamera->ZFocusType() == Graphic3d_Camera::FocusType_Relative
                                ? Standard_Real(theCamera->ZFocus() * theCamera->Distance())
                                : Standard_Real(theCamera->ZFocus()));

    // scale factor to pixels
    const gp_XYZ        aViewDim = theCamera->ViewDimensions (aFocus);
    const Standard_Real aScale   = Abs(aViewDim.Y()) / Standard_Real(theViewportHeight);
    gp_XYZ aCenter (0.0, 0.0, -aFocus);
    if (Point.x() != 0.0)
    {
      aCenter.SetX (-aViewDim.X() * 0.5 + Point.z() * aScale);
      if (Point.x() > 0.0)
      {
        aCenter.SetX (-aCenter.X());
      }
    }
    if (Point.y() != 0.0)
    {
      aCenter.SetY (-aViewDim.Y() * 0.5 + Point.z() * aScale);
      if (Point.y() > 0.0)
      {
        aCenter.SetY (-aCenter.Y());
      }
    }

    theWorldView.InitIdentity();
    Graphic3d_TransformUtils::Translate (theWorldView, T(aCenter.X()), T(aCenter.Y()), T(aCenter.Z()));
    Graphic3d_TransformUtils::Scale     (theWorldView, T(aScale),      T(aScale),      T(aScale));
    return;
  }

  {
    // Compute reference point for transformation in untransformed projection space.
    NCollection_Vec4<T> aRefPoint (static_cast<T> (Point.x()),
                                   static_cast<T> (Point.y()),
                                   static_cast<T> (Point.z()),
                                   static_cast<T> (1.0));
    NCollection_Vec4<T> aRefPointProj;
    if ((Flags & Graphic3d_TMF_PanPers) != Graphic3d_TMF_PanPers)
    {
      aRefPointProj  = theProjection * (theWorldView * aRefPoint);
      aRefPointProj /= aRefPointProj.w();
    }

    // Prevent zooming.
    if ((Flags & Graphic3d_TMF_ZoomPers) != 0)
    {
      const T aSize = static_cast<T> (1.0);
      const Standard_Integer aViewport[4] = { 0, 0, theViewportHeight, theViewportHeight };
      NCollection_Mat4<T> aWorldView;
      aWorldView.InitIdentity();

      NCollection_Vec3<T> aWinCoordsRefPoint;
      Graphic3d_TransformUtils::Project (static_cast<T> (Point.x()),
                                         static_cast<T> (Point.y()),
                                         static_cast<T> (Point.z()),
                                         theWorldView, theProjection, aViewport,
                                         aWinCoordsRefPoint.x(), aWinCoordsRefPoint.y(), aWinCoordsRefPoint.z());

      NCollection_Vec3<T> anUnProj1;
      Graphic3d_TransformUtils::UnProject (aWinCoordsRefPoint.x(), aWinCoordsRefPoint.y(), aWinCoordsRefPoint.z(),
                                           aWorldView, theProjection, aViewport,
                                           anUnProj1.x(), anUnProj1.y(), anUnProj1.z());

      NCollection_Vec3<T> anUnProj2;
      Graphic3d_TransformUtils::UnProject (aWinCoordsRefPoint.x(), aWinCoordsRefPoint.y() + aSize, aWinCoordsRefPoint.z(),
                                           aWorldView, theProjection, aViewport,
                                           anUnProj2.x(), anUnProj2.y(), anUnProj2.z());

      const T aScale = (anUnProj2.y() - anUnProj1.y()) / aSize;

      Graphic3d_TransformUtils::Scale (theWorldView, aScale, aScale, aScale);
    }

    // Prevent translation by nullifying translation component.
    if ((Flags & Graphic3d_TMF_PanPers) != 0)
    {
      theWorldView .SetValue (0, 3, static_cast<T> (0.0));
      theWorldView .SetValue (1, 3, static_cast<T> (0.0));
      theWorldView .SetValue (2, 3, static_cast<T> (0.0));
      theProjection.SetValue (0, 3, static_cast<T> (0.0));
      theProjection.SetValue (1, 3, static_cast<T> (0.0));
      theProjection.SetValue (2, 3, static_cast<T> (0.0));
    }

    // Prevent rotation by nullifying rotation component.
    if (Flags & Graphic3d_TMF_RotatePers)
    {
      theWorldView.SetValue (0, 0, static_cast<T> (1.0));
      theWorldView.SetValue (1, 0, static_cast<T> (0.0));
      theWorldView.SetValue (2, 0, static_cast<T> (0.0));

      theWorldView.SetValue (0, 1, static_cast<T> (0.0));
      theWorldView.SetValue (1, 1, static_cast<T> (1.0));
      theWorldView.SetValue (2, 1, static_cast<T> (0.0));

      theWorldView.SetValue (0, 2, static_cast<T> (0.0));
      theWorldView.SetValue (1, 2, static_cast<T> (0.0));
      theWorldView.SetValue (2, 2, static_cast<T> (1.0));
    }

    if ((Flags & Graphic3d_TMF_PanPers) != Graphic3d_TMF_PanPers)
    {
      NCollection_Mat4<T> anUnviewMat;

      if (!(theProjection * theWorldView).Inverted (anUnviewMat))
      {
        Standard_ProgramError::Raise ("Graphic3d_TransformPers::Apply, can not inverse world view projection matrix.");
      }

      // Move to reference point location in transformed view projection space.
      aRefPoint  = anUnviewMat * aRefPointProj;
      aRefPoint /= aRefPoint.w();

      Graphic3d_TransformUtils::Translate<T> (theWorldView, aRefPoint.x(), aRefPoint.y(), aRefPoint.z());
    }
  }
}

// =======================================================================
// function : Apply
// purpose  : Apply transformation to bounding box of presentation.
// =======================================================================
template<class T>
void Graphic3d_TransformPers::Apply (const Handle(Graphic3d_Camera)& theCamera,
                                     const NCollection_Mat4<T>& theProjection,
                                     const NCollection_Mat4<T>& theWorldView,
                                     const Standard_Integer theViewportWidth,
                                     const Standard_Integer theViewportHeight,
                                     Bnd_Box& theBoundingBox) const
{
  if (theBoundingBox.IsVoid())
  {
    return;
  }

  T aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;

  theBoundingBox.Get (aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  typename BVH_Box<T, 4>::BVH_VecNt aMin (aXmin, aYmin, aZmin, static_cast<T> (1.0));
  typename BVH_Box<T, 4>::BVH_VecNt aMax (aXmax, aYmax, aZmax, static_cast<T> (1.0));
  BVH_Box<T, 4> aBBox (aMin, aMax);

  Apply (theCamera, theProjection, theWorldView, theViewportWidth, theViewportHeight, aBBox);

  theBoundingBox = Bnd_Box();
  theBoundingBox.Update (aBBox.CornerMin().x(), aBBox.CornerMin().y(), aBBox.CornerMin().z(),
                         aBBox.CornerMax().x(), aBBox.CornerMax().y(), aBBox.CornerMax().z());
}

// =======================================================================
// function : Apply
// purpose  : Apply transformation to bounding box of presentation.
// =======================================================================
template<class T>
void Graphic3d_TransformPers::Apply (const Handle(Graphic3d_Camera)& theCamera,
                                     const NCollection_Mat4<T>& theProjection,
                                     const NCollection_Mat4<T>& theWorldView,
                                     const Standard_Integer theViewportWidth,
                                     const Standard_Integer theViewportHeight,
                                     BVH_Box<T, 4>& theBoundingBox) const
{
  NCollection_Mat4<T> aTPers = Compute (theCamera, theProjection, theWorldView, theViewportWidth, theViewportHeight);
  if (aTPers.IsIdentity()
  || !theBoundingBox.IsValid())
  {
    return;
  }

  const typename BVH_Box<T, 4>::BVH_VecNt& aMin = theBoundingBox.CornerMin();
  const typename BVH_Box<T, 4>::BVH_VecNt& aMax = theBoundingBox.CornerMax();

  typename BVH_Box<T, 4>::BVH_VecNt anArrayOfCorners[8];
  anArrayOfCorners[0] = typename BVH_Box<T, 4>::BVH_VecNt (aMin.x(), aMin.y(), aMin.z(), static_cast<T> (1.0));
  anArrayOfCorners[1] = typename BVH_Box<T, 4>::BVH_VecNt (aMin.x(), aMin.y(), aMax.z(), static_cast<T> (1.0));
  anArrayOfCorners[2] = typename BVH_Box<T, 4>::BVH_VecNt (aMin.x(), aMax.y(), aMin.z(), static_cast<T> (1.0));
  anArrayOfCorners[3] = typename BVH_Box<T, 4>::BVH_VecNt (aMin.x(), aMax.y(), aMax.z(), static_cast<T> (1.0));
  anArrayOfCorners[4] = typename BVH_Box<T, 4>::BVH_VecNt (aMax.x(), aMin.y(), aMin.z(), static_cast<T> (1.0));
  anArrayOfCorners[5] = typename BVH_Box<T, 4>::BVH_VecNt (aMax.x(), aMin.y(), aMax.z(), static_cast<T> (1.0));
  anArrayOfCorners[6] = typename BVH_Box<T, 4>::BVH_VecNt (aMax.x(), aMax.y(), aMin.z(), static_cast<T> (1.0));
  anArrayOfCorners[7] = typename BVH_Box<T, 4>::BVH_VecNt (aMax.x(), aMax.y(), aMax.z(), static_cast<T> (1.0));

  theBoundingBox.Clear();
  for (Standard_Integer anIt = 0; anIt < 8; ++anIt)
  {
    typename BVH_Box<T, 4>::BVH_VecNt& aCorner = anArrayOfCorners[anIt];
    aCorner  = aTPers * aCorner;
    aCorner /= aCorner.w();
    theBoundingBox.Add (aCorner);
  }
}

// =======================================================================
// function : Compute
// purpose  : Compute transformation.
// =======================================================================
template<class T>
NCollection_Mat4<T> Graphic3d_TransformPers::Compute (const Handle(Graphic3d_Camera)& theCamera,
                                                      const NCollection_Mat4<T>& theProjection,
                                                      const NCollection_Mat4<T>& theWorldView,
                                                      const Standard_Integer theViewportWidth,
                                                      const Standard_Integer theViewportHeight) const
{
  if (Flags == Graphic3d_TMF_None)
  {
    return NCollection_Mat4<T>();
  }

  NCollection_Mat4<T> anUnviewMat;

  if (!(theProjection * theWorldView).Inverted (anUnviewMat))
  {
    return NCollection_Mat4<T>();
  }

  NCollection_Mat4<T> aProjection (theProjection);
  NCollection_Mat4<T> aWorldView (theWorldView);

  Apply (theCamera, aProjection, aWorldView, theViewportWidth, theViewportHeight);

  return anUnviewMat * (aProjection * aWorldView);
}

#endif // _Graphic3d_TransformPers_HeaderFile
