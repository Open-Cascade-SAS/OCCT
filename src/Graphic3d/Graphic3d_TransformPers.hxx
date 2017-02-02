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

#include <Aspect_TypeOfTriedronPosition.hxx>
#include <Bnd_Box.hxx>
#include <BVH_Box.hxx>
#include <Graphic3d_Camera.hxx>
#include <Graphic3d_TransformUtils.hxx>
#include <Graphic3d_TransModeFlags.hxx>
#include <Graphic3d_Vec.hxx>
#include <NCollection_Mat4.hxx>

DEFINE_STANDARD_HANDLE(Graphic3d_TransformPers, Standard_Transient)

//! Class for keeping and computing transformation persistence.
//! Note that instance of this class can not define
//! no transformation persistence Graphic3d_TMF_None - NULL handle should be used for this purpose.
class Graphic3d_TransformPers : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_TransformPers, Standard_Transient)
public:

  //! Return true if specified mode is zoom/rotate transformation persistence.
  static Standard_Boolean IsZoomOrRotate (Graphic3d_TransModeFlags theMode)
  {
    return (theMode & (Graphic3d_TMF_ZoomPers | Graphic3d_TMF_RotatePers)) != 0;
  }

  //! Return true if specified mode is 2d/trihedron transformation persistence.
  static Standard_Boolean IsTrihedronOr2d (Graphic3d_TransModeFlags theMode)
  {
    return (theMode & (Graphic3d_TMF_TriedronPers | Graphic3d_TMF_2d)) != 0;
  }

  //! Create Graphic3d_TransformPers instance from deprecated parameters set
  //! decoding 2D corner + offset parameters from 3D point.
  Standard_EXPORT static Handle(Graphic3d_TransformPers) FromDeprecatedParams (Graphic3d_TransModeFlags theFlag,
                                                                               const gp_Pnt&            thePoint);

public:

  //! Set transformation persistence.
  Graphic3d_TransformPers (const Graphic3d_TransModeFlags theMode)
  : myMode (theMode)
  {
    if (IsZoomOrRotate (theMode))
    {
      SetPersistence (theMode, gp_Pnt(0.0, 0.0, 0.0));
    }
    else if (IsTrihedronOr2d (theMode))
    {
      SetPersistence (theMode, Aspect_TOTP_LEFT_LOWER, Graphic3d_Vec2i (0, 0));
    }
    else
    {
      throw Standard_ProgramError("Graphic3d_TransformPers::SetPersistence(), wrong persistence mode.");
    }
  }

  //! Set Zoom/Rotate transformation persistence with an anchor 3D point.
  //! Throws an exception if persistence mode is not Graphic3d_TMF_ZoomPers, Graphic3d_TMF_ZoomRotatePers or Graphic3d_TMF_RotatePers.
  Graphic3d_TransformPers (const Graphic3d_TransModeFlags theMode,
                           const gp_Pnt& thePnt)
  : myMode (Graphic3d_TMF_None)
  {
    SetPersistence (theMode, thePnt);
  }

  //! Set 2d/trihedron transformation persistence with a corner and 2D offset.
  //! Throws an exception if persistence mode is not Graphic3d_TMF_TriedronPers or Graphic3d_TMF_2d.
  //! The offset is a positive displacement from the view corner in pixels.
  Graphic3d_TransformPers (const Graphic3d_TransModeFlags theMode,
                           const Aspect_TypeOfTriedronPosition theCorner,
                           const Graphic3d_Vec2i& theOffset = Graphic3d_Vec2i (0, 0))
  : myMode (Graphic3d_TMF_None)
  {
    SetPersistence (theMode, theCorner, theOffset);
  }

  //! Return true for Graphic3d_TMF_ZoomPers, Graphic3d_TMF_ZoomRotatePers or Graphic3d_TMF_RotatePers modes.
  Standard_Boolean IsZoomOrRotate() const { return IsZoomOrRotate (myMode); }

  //! Return true for Graphic3d_TMF_TriedronPers and Graphic3d_TMF_2d modes.
  Standard_Boolean IsTrihedronOr2d() const { return IsTrihedronOr2d (myMode); }

  //! Transformation persistence mode flags.
  Graphic3d_TransModeFlags Mode() const { return myMode; }

  //! Transformation persistence mode flags.
  Graphic3d_TransModeFlags Flags() const { return myMode; }

  //! Set Zoom/Rotate transformation persistence with an anchor 3D point.
  //! Throws an exception if persistence mode is not Graphic3d_TMF_ZoomPers, Graphic3d_TMF_ZoomRotatePers or Graphic3d_TMF_RotatePers.
  void SetPersistence (const Graphic3d_TransModeFlags theMode,
                       const gp_Pnt& thePnt)
  {
    if (!IsZoomOrRotate (theMode))
    {
      throw Standard_ProgramError("Graphic3d_TransformPers::SetPersistence(), wrong persistence mode.");
    }

    myMode = theMode;
    myParams.Params3d.PntX = thePnt.X();
    myParams.Params3d.PntY = thePnt.Y();
    myParams.Params3d.PntZ = thePnt.Z();
  }

  //! Set 2d/trihedron transformation persistence with a corner and 2D offset.
  //! Throws an exception if persistence mode is not Graphic3d_TMF_TriedronPers or Graphic3d_TMF_2d.
  void SetPersistence (const Graphic3d_TransModeFlags theMode,
                       const Aspect_TypeOfTriedronPosition theCorner,
                       const Graphic3d_Vec2i& theOffset)
  {
    if (!IsTrihedronOr2d (theMode))
    {
      throw Standard_ProgramError("Graphic3d_TransformPers::SetPersistence(), wrong persistence mode.");
    }

    myMode = theMode;
    myParams.Params2d.Corner  = theCorner;
    myParams.Params2d.OffsetX = theOffset.x();
    myParams.Params2d.OffsetY = theOffset.y();
  }

public:

  //! Return the anchor point for zoom/rotate transformation persistence.
  gp_Pnt AnchorPoint() const
  {
    if (!IsZoomOrRotate())
    {
      throw Standard_ProgramError("Graphic3d_TransformPers::AnchorPoint(), wrong persistence mode.");
    }

    return gp_Pnt (myParams.Params3d.PntX, myParams.Params3d.PntY, myParams.Params3d.PntZ);
  }

  //! Set the anchor point for zoom/rotate transformation persistence.
  void SetAnchorPoint (const gp_Pnt& thePnt)
  {
    if (!IsZoomOrRotate())
    {
      throw Standard_ProgramError("Graphic3d_TransformPers::SetAnchorPoint(), wrong persistence mode.");
    }

    myParams.Params3d.PntX = thePnt.X();
    myParams.Params3d.PntY = thePnt.Y();
    myParams.Params3d.PntZ = thePnt.Z();
  }

  //! Return the corner for 2d/trihedron transformation persistence.
  Aspect_TypeOfTriedronPosition Corner2d() const
  {
    if (!IsTrihedronOr2d())
    {
      throw Standard_ProgramError("Graphic3d_TransformPers::Corner2d(), wrong persistence mode.");
    }

    return myParams.Params2d.Corner;
  }

  //! Set the corner for 2d/trihedron transformation persistence.
  void SetCorner2d (const Aspect_TypeOfTriedronPosition thePos)
  {
    if (!IsTrihedronOr2d())
    {
      throw Standard_ProgramError("Graphic3d_TransformPers::SetCorner2d(), wrong persistence mode.");
    }

    myParams.Params2d.Corner = thePos;
  }

  //! Return the offset from the corner for 2d/trihedron transformation persistence.
  Graphic3d_Vec2i Offset2d() const
  {
    if (!IsTrihedronOr2d())
    {
      throw Standard_ProgramError("Graphic3d_TransformPers::Offset2d(), wrong persistence mode.");
    }

    return Graphic3d_Vec2i (myParams.Params2d.OffsetX, myParams.Params2d.OffsetY);
  }

  //! Set the offset from the corner for 2d/trihedron transformation persistence.
  void SetOffset2d (const Graphic3d_Vec2i& theOffset)
  {
    if (!IsTrihedronOr2d())
    {
      throw Standard_ProgramError("Graphic3d_TransformPers::SetOffset2d(), wrong persistence mode.");
    }

    myParams.Params2d.OffsetX = theOffset.x();
    myParams.Params2d.OffsetY = theOffset.y();
  }

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
              BVH_Box<T, 3>& theBoundingBox) const;

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

  //! Apply transformation persistence on specified matrices.
  //! @param theCamera camera definition
  //! @param theProjection projection matrix to modify
  //! @param theWorldView  world-view matrix to modify
  //! @param theViewportWidth  viewport width
  //! @param theViewportHeight viewport height
  template<class T>
  void Apply (const Handle(Graphic3d_Camera)& theCamera,
              const NCollection_Mat4<T>& theProjection,
              NCollection_Mat4<T>& theWorldView,
              const Standard_Integer theViewportWidth,
              const Standard_Integer theViewportHeight) const;

private:

  //! 3D anchor point for zoom/rotate transformation persistence.
  struct PersParams3d
  {
    Standard_Real PntX;
    Standard_Real PntY;
    Standard_Real PntZ;
  };

  //! 2d/trihedron transformation persistence parameters.
  struct PersParams2d
  {
    Standard_Integer OffsetX;
    Standard_Integer OffsetY;
    Aspect_TypeOfTriedronPosition Corner;
  };

private:

  Graphic3d_TransModeFlags myMode;  //!< Transformation persistence mode flags
  union
  {
    PersParams3d Params3d;
    PersParams2d Params2d;
  } myParams;

};

// =======================================================================
// function : Apply
// purpose  : Apply transformation to world view and projection matrices.
// =======================================================================
template<class T>
void Graphic3d_TransformPers::Apply (const Handle(Graphic3d_Camera)& theCamera,
                                     const NCollection_Mat4<T>& theProjection,
                                     NCollection_Mat4<T>& theWorldView,
                                     const Standard_Integer theViewportWidth,
                                     const Standard_Integer theViewportHeight) const
{
  (void )theViewportWidth;
  (void )theProjection;
  if (myMode == Graphic3d_TMF_None
   || theViewportHeight == 0)
  {
    return;
  }

  // use total size when tiling is active
  const Standard_Integer aVPSizeY = theCamera->Tile().IsValid() ? theCamera->Tile().TotalSize.y() : theViewportHeight;

  // a small enough jitter compensation offset
  // to avoid image dragging within single pixel in corner cases
  const Standard_Real aJitterComp = 0.001;
  if (myMode == Graphic3d_TMF_TriedronPers)
  {
    // reset Z focus for trihedron persistence
    const Standard_Real aFocus = theCamera->IsOrthographic()
                               ? theCamera->Distance()
                               : (theCamera->ZFocusType() == Graphic3d_Camera::FocusType_Relative
                                ? Standard_Real(theCamera->ZFocus() * theCamera->Distance())
                                : Standard_Real(theCamera->ZFocus()));

    // scale factor to pixels
    const gp_XYZ aViewDim = theCamera->ViewDimensions (aFocus);
    const Standard_Real aScale = Abs(aViewDim.Y()) / Standard_Real(aVPSizeY);
    const gp_Dir aForward (theCamera->Center().XYZ() - theCamera->Eye().XYZ());
    gp_XYZ aCenter = theCamera->Center().XYZ() + aForward.XYZ() * (aFocus - theCamera->Distance());
    if ((myParams.Params2d.Corner & (Aspect_TOTP_LEFT | Aspect_TOTP_RIGHT)) != 0)
    {
      const Standard_Real anOffsetX = (Standard_Real(myParams.Params2d.OffsetX) + aJitterComp) * aScale;
      const gp_Dir aSide   = aForward.Crossed (theCamera->Up());
      const gp_XYZ aDeltaX = aSide.XYZ() * (Abs(aViewDim.X()) * 0.5 - anOffsetX);
      if ((myParams.Params2d.Corner & Aspect_TOTP_RIGHT) != 0)
      {
        aCenter += aDeltaX;
      }
      else
      {
        aCenter -= aDeltaX;
      }
    }
    if ((myParams.Params2d.Corner & (Aspect_TOTP_TOP | Aspect_TOTP_BOTTOM)) != 0)
    {
      const Standard_Real anOffsetY = (Standard_Real(myParams.Params2d.OffsetY) + aJitterComp) * aScale;
      const gp_XYZ aDeltaY = theCamera->Up().XYZ() * (Abs(aViewDim.Y()) * 0.5 - anOffsetY);
      if ((myParams.Params2d.Corner & Aspect_TOTP_TOP) != 0)
      {
        aCenter += aDeltaY;
      }
      else
      {
        aCenter -= aDeltaY;
      }
    }

    NCollection_Mat4<Standard_Real> aWorldView = theCamera->OrientationMatrix();
    Graphic3d_TransformUtils::Translate (aWorldView, aCenter.X(), aCenter.Y(), aCenter.Z());
    Graphic3d_TransformUtils::Scale     (aWorldView, aScale,      aScale,      aScale);
    theWorldView.ConvertFrom (aWorldView);
    return;
  }
  else if (myMode == Graphic3d_TMF_2d)
  {
    const Standard_Real aFocus = theCamera->IsOrthographic()
                               ? theCamera->Distance()
                               : (theCamera->ZFocusType() == Graphic3d_Camera::FocusType_Relative
                                ? Standard_Real(theCamera->ZFocus() * theCamera->Distance())
                                : Standard_Real(theCamera->ZFocus()));

    // scale factor to pixels
    const gp_XYZ        aViewDim = theCamera->ViewDimensions (aFocus);
    const Standard_Real aScale   = Abs(aViewDim.Y()) / Standard_Real(aVPSizeY);
    gp_XYZ aCenter (0.0, 0.0, -aFocus);
    if ((myParams.Params2d.Corner & (Aspect_TOTP_LEFT | Aspect_TOTP_RIGHT)) != 0)
    {
      aCenter.SetX (-aViewDim.X() * 0.5 + (Standard_Real(myParams.Params2d.OffsetX) + aJitterComp) * aScale);
      if ((myParams.Params2d.Corner & Aspect_TOTP_RIGHT) != 0)
      {
        aCenter.SetX (-aCenter.X());
      }
    }
    if ((myParams.Params2d.Corner & (Aspect_TOTP_TOP | Aspect_TOTP_BOTTOM)) != 0)
    {
      aCenter.SetY (-aViewDim.Y() * 0.5 + (Standard_Real(myParams.Params2d.OffsetY) + aJitterComp) * aScale);
      if ((myParams.Params2d.Corner & Aspect_TOTP_TOP) != 0)
      {
        aCenter.SetY (-aCenter.Y());
      }
    }

    theWorldView.InitIdentity();
    Graphic3d_TransformUtils::Translate (theWorldView, T(aCenter.X()), T(aCenter.Y()), T(aCenter.Z()));
    Graphic3d_TransformUtils::Scale     (theWorldView, T(aScale),      T(aScale),      T(aScale));
    return;
  }
  else
  {
    // Compute reference point for transformation in untransformed projection space.
    NCollection_Mat4<Standard_Real> aWorldView = theCamera->OrientationMatrix();
    Graphic3d_TransformUtils::Translate (aWorldView, myParams.Params3d.PntX, myParams.Params3d.PntY, myParams.Params3d.PntZ);
    if ((myMode & Graphic3d_TMF_RotatePers) != 0)
    {
      // lock rotation by nullifying rotation component
      aWorldView.SetValue (0, 0, 1.0);
      aWorldView.SetValue (1, 0, 0.0);
      aWorldView.SetValue (2, 0, 0.0);

      aWorldView.SetValue (0, 1, 0.0);
      aWorldView.SetValue (1, 1, 1.0);
      aWorldView.SetValue (2, 1, 0.0);

      aWorldView.SetValue (0, 2, 0.0);
      aWorldView.SetValue (1, 2, 0.0);
      aWorldView.SetValue (2, 2, 1.0);
    }

    if ((myMode & Graphic3d_TMF_ZoomPers) != 0)
    {
      // lock zooming
      gp_Vec aVecToEye (theCamera->Direction());
      gp_Vec aVecToObj (theCamera->Eye(), gp_Pnt (myParams.Params3d.PntX, myParams.Params3d.PntY, myParams.Params3d.PntZ));
      const Standard_Real aFocus = aVecToObj.Dot (aVecToEye);
      const gp_XYZ aViewDim = theCamera->ViewDimensions (aFocus);
      const Standard_Real aScale = Abs(aViewDim.Y()) / Standard_Real(aVPSizeY);
      Graphic3d_TransformUtils::Scale (aWorldView, aScale, aScale, aScale);
    }
    theWorldView.ConvertFrom (aWorldView);
    return;
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

  typename BVH_Box<T, 3>::BVH_VecNt aMin (aXmin, aYmin, aZmin);
  typename BVH_Box<T, 3>::BVH_VecNt aMax (aXmax, aYmax, aZmax);
  BVH_Box<T, 3> aBBox (aMin, aMax);

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
                                     BVH_Box<T, 3>& theBoundingBox) const
{
  NCollection_Mat4<T> aTPers = Compute (theCamera, theProjection, theWorldView, theViewportWidth, theViewportHeight);
  if (aTPers.IsIdentity()
  || !theBoundingBox.IsValid())
  {
    return;
  }

  const typename BVH_Box<T, 3>::BVH_VecNt& aMin = theBoundingBox.CornerMin();
  const typename BVH_Box<T, 3>::BVH_VecNt& aMax = theBoundingBox.CornerMax();

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
    aCorner = aCorner / aCorner.w();
    theBoundingBox.Add (typename BVH_Box<T, 3>::BVH_VecNt (aCorner.x(), aCorner.y(), aCorner.z()));
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
  if (myMode == Graphic3d_TMF_None)
  {
    return NCollection_Mat4<T>();
  }

  NCollection_Mat4<T> aWorldView (theWorldView);
  NCollection_Mat4<T> anUnviewMat;
  if (!theWorldView.Inverted (anUnviewMat))
  {
    return NCollection_Mat4<T>();
  }

  // compute only world-view matrix difference to avoid floating point instability
  // caused by projection matrix modifications outside of this algorithm (e.g. by Z-fit)
  Apply (theCamera, theProjection, aWorldView, theViewportWidth, theViewportHeight);
  return anUnviewMat * aWorldView;
}

#endif // _Graphic3d_TransformPers_HeaderFile
