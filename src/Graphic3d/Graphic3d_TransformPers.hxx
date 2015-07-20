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
  //! @param theProjection [in] the projection transformation matrix.
  //! @param theWorldView [in] the world view transformation matrix.
  //! @param theViewportWidth [in] the width of viewport (for 2d persistence).
  //! @param theViewportHeight [in] the height of viewport (for 2d persistence).
  //! @param theBoundingBox [in/out] the bounding box to transform.
  template<class T>
  void Apply (const NCollection_Mat4<T>& theProjection,
              const NCollection_Mat4<T>& theWorldView,
              const Standard_Integer theViewportWidth,
              const Standard_Integer theViewportHeight,
              Bnd_Box& theBoundingBox) const;

  //! Apply transformation to bounding box of presentation
  //! @param theProjection [in] the projection transformation matrix.
  //! @param theWorldView [in] the world view transformation matrix.
  //! @param theViewportWidth [in] the width of viewport (for 2d persistence).
  //! @param theViewportHeight [in] the height of viewport (for 2d persistence).
  //! @param theBoundingBox [in/out] the bounding box to transform.
  template<class T>
  void Apply (const NCollection_Mat4<T>& theProjection,
              const NCollection_Mat4<T>& theWorldView,
              const Standard_Integer theViewportWidth,
              const Standard_Integer theViewportHeight,
              BVH_Box<T, 4>& theBoundingBox) const;

  //! Compute transformation.
  //! Computed matrix can be applied to model world transformation
  //! of an object to implement effect of transformation persistence.
  //! @param theProjection [in] the projection transformation matrix.
  //! @param theWorldView [in] the world view transformation matrix.
  //! @param theViewportWidth [in] the width of viewport (for 2d persistence).
  //! @param theViewportHeight [in] the height of viewport (for 2d persistence).
  //! @return transformation matrix to be applied to model world transformation of an object.
  template<class T>
  NCollection_Mat4<T> Compute (const NCollection_Mat4<T>& theProjection,
                               const NCollection_Mat4<T>& theWorldView,
                               const Standard_Integer theViewportWidth,
                               const Standard_Integer theViewportHeight) const;

  template<class T>
  void Apply (NCollection_Mat4<T>& theProjection,
              NCollection_Mat4<T>& theWorldView,
              const Standard_Integer theViewportWidth,
              const Standard_Integer theViewportHeight) const;
};

// =======================================================================
// function : Apply
// purpose  : Apply transformation to world view and projection matrices.
// =======================================================================
template<class T>
void Graphic3d_TransformPers::Apply (NCollection_Mat4<T>& theProjection,
                                     NCollection_Mat4<T>& theWorldView,
                                     const Standard_Integer theViewportWidth,
                                     const Standard_Integer theViewportHeight) const
{
  if (!Flags)
  {
    return;
  }

  if (Flags & Graphic3d_TMF_2d)
  {
    T aLeft   = -static_cast<T> (theViewportWidth  / 2);
    T aRight  =  static_cast<T> (theViewportWidth  / 2);
    T aBottom = -static_cast<T> (theViewportHeight / 2);
    T aTop    =  static_cast<T> (theViewportHeight / 2);
    T aGap    =  static_cast<T> (Point.z());
    if (Point.x() > 0)
    {
      aLeft  -= static_cast<T> (theViewportWidth / 2) - aGap;
      aRight -= static_cast<T> (theViewportWidth / 2) - aGap;
    }
    else if (Point.x() < 0)
    {
      aLeft  += static_cast<T> (theViewportWidth / 2) - aGap;
      aRight += static_cast<T> (theViewportWidth / 2) - aGap;
    }
    if (Point.y() > 0)
    {
      aBottom -= static_cast<T> (theViewportHeight / 2) - aGap;
      aTop    -= static_cast<T> (theViewportHeight / 2) - aGap;
    }
    else if (Point.y() < 0)
    {
      aBottom += static_cast<T> (theViewportHeight / 2) - aGap;
      aTop    += static_cast<T> (theViewportHeight / 2) - aGap;
    }
    if (Flags == Graphic3d_TMF_2d_IsTopDown)
    {
      const T aTemp = aTop;
      aTop    = aBottom;
      aBottom = aTemp;
    }

    Graphic3d_TransformUtils::Ortho2D<T> (theProjection, aLeft, aRight, aBottom, aTop);

    theWorldView.InitIdentity();
  }
  else
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
    if ((Flags & Graphic3d_TMF_ZoomPers) || (Flags == Graphic3d_TMF_TriedronPers))
    {
      // Compute fixed-zoom multiplier. Actually function works ugly with TelPerspective!
      const T aDet2 = static_cast<T> (0.002) / Max (theProjection.GetValue (1, 1), theProjection.GetValue (0, 0));
      theProjection.ChangeValue (0, 0) *= aDet2;
      theProjection.ChangeValue (1, 1) *= aDet2;
      theProjection.ChangeValue (2, 2) *= aDet2;
    }

    // Prevent translation by nullifying translation component.
    if ((Flags & Graphic3d_TMF_PanPers) || Flags == Graphic3d_TMF_TriedronPers)
    {
      theWorldView .SetValue (0, 3, static_cast<T> (0.0));
      theWorldView .SetValue (1, 3, static_cast<T> (0.0));
      theWorldView .SetValue (2, 3, static_cast<T> (0.0));
      theProjection.SetValue (0, 3, static_cast<T> (0.0));
      theProjection.SetValue (1, 3, static_cast<T> (0.0));
      theProjection.SetValue (2, 3, static_cast<T> (0.0));
    }

    // Prevent scaling-on-axis.
    if (Flags & Graphic3d_TMF_ZoomPers)
    {
      NCollection_Vec3<T> aVecX = theWorldView.GetColumn (0).xyz();
      NCollection_Vec3<T> aVecY = theWorldView.GetColumn (1).xyz();
      NCollection_Vec3<T> aVecZ = theWorldView.GetColumn (2).xyz();
      T aScaleX = aVecX.Modulus();
      T aScaleY = aVecY.Modulus();
      T aScaleZ = aVecZ.Modulus();
      for (Standard_Integer anI = 0; anI < 3; ++anI)
      {
        theWorldView.ChangeValue (0, anI) /= aScaleX;
        theWorldView.ChangeValue (1, anI) /= aScaleY;
        theWorldView.ChangeValue (2, anI) /= aScaleZ;
      }
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

    if (Flags == Graphic3d_TMF_TriedronPers)
    {
      if (Point.x() != 0.0 && Point.y() != 0.0)
      {
        NCollection_Mat4<T> anUnviewMat;

        if (!(theProjection).Inverted (anUnviewMat))
        {
          Standard_ProgramError::Raise ("Graphic3d_TransformPers::Apply, can not inverse projection matrix.");
        }

        NCollection_Vec4<T> aProjMax (static_cast<T> ( 1.0), static_cast<T> ( 1.0), static_cast<T> (0.0), static_cast<T> (1.0));
        NCollection_Vec4<T> aProjMin (static_cast<T> (-1.0), static_cast<T> (-1.0), static_cast<T> (0.0), static_cast<T> (1.0));
        NCollection_Vec4<T> aViewMax = anUnviewMat * aProjMax;
        NCollection_Vec4<T> aViewMin = anUnviewMat * aProjMin;

        aViewMax /= aViewMax.w();
        aViewMin /= aViewMin.w();

        T aMoveX = static_cast<T> (0.5) * (aViewMax.x() - aViewMin.x() - static_cast<T> (Point.z()));
        T aMoveY = static_cast<T> (0.5) * (aViewMax.y() - aViewMin.y() - static_cast<T> (Point.z()));

        aMoveX = (Point.x() > 0.0) ? aMoveX : -aMoveX;
        aMoveY = (Point.y() > 0.0) ? aMoveY : -aMoveY;

        Graphic3d_TransformUtils::Translate<T> (theProjection, aMoveX, aMoveY, static_cast<T> (0.0));
      }
    }
    else if ((Flags & Graphic3d_TMF_PanPers) != Graphic3d_TMF_PanPers)
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
void Graphic3d_TransformPers::Apply (const NCollection_Mat4<T>& theProjection,
                                     const NCollection_Mat4<T>& theWorldView,
                                     const Standard_Integer theViewportWidth,
                                     const Standard_Integer theViewportHeight,
                                     Bnd_Box& theBoundingBox) const
{
  T aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;

  theBoundingBox.Get (aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  typename BVH_Box<T, 4>::BVH_VecNt aMin (aXmin, aYmin, aZmin, static_cast<T> (1.0));
  typename BVH_Box<T, 4>::BVH_VecNt aMax (aXmax, aYmax, aZmax, static_cast<T> (1.0));
  BVH_Box<T, 4> aBBox (aMin, aMax);

  Apply (theProjection, theWorldView, theViewportWidth, theViewportHeight, aBBox);

  theBoundingBox = Bnd_Box();
  theBoundingBox.Update (aBBox.CornerMin().x(), aBBox.CornerMin().y(), aBBox.CornerMin().z(),
                         aBBox.CornerMax().x(), aBBox.CornerMax().y(), aBBox.CornerMax().z());
}

// =======================================================================
// function : Apply
// purpose  : Apply transformation to bounding box of presentation.
// =======================================================================
template<class T>
void Graphic3d_TransformPers::Apply (const NCollection_Mat4<T>& theProjection,
                                     const NCollection_Mat4<T>& theWorldView,
                                     const Standard_Integer theViewportWidth,
                                     const Standard_Integer theViewportHeight,
                                     BVH_Box<T, 4>& theBoundingBox) const
{
  NCollection_Mat4<T> aTPers = Compute (theProjection, theWorldView, theViewportWidth, theViewportHeight);

  if (aTPers.IsIdentity())
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
NCollection_Mat4<T> Graphic3d_TransformPers::Compute (const NCollection_Mat4<T>& theProjection,
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

  Apply (aProjection, aWorldView, theViewportWidth, theViewportHeight);

  return anUnviewMat * (aProjection * aWorldView);
}

#endif // _Graphic3d_TransformPers_HeaderFile
