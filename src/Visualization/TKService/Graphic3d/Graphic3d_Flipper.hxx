// Copyright (c) 2024 OPEN CASCADE SAS
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

#ifndef _Graphic3d_Flipper_HeaderFile
#define _Graphic3d_Flipper_HeaderFile

#include <BVH_Box.hxx>
#include <Bnd_Box.hxx>
#include <NCollection_Mat4.hxx>
#include <Standard_Transient.hxx>
#include <gp_Ax2.hxx>

//! CPU-side analogue of OpenGl_Flipper used by the selection pipeline.
//! Describes a reference coordinate system used at draw time to mirror
//! group contents so that they remain upright relative to the camera.
//! The same logic is used here to compute the flipping matrix applied
//! to bounding boxes and sensitive frustums, so that selection matches
//! the rendered geometry.
class Graphic3d_Flipper : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_Flipper, Standard_Transient)

public:
  //! Constructor.
  Graphic3d_Flipper(const gp_Ax2& theRefPlane)
      : myRefPlane(theRefPlane)
  {
  }

  //! Return reference plane used for flipping.
  gp_Ax2 RefPlane() const { return myRefPlane; }

  //! Set reference plane used for flipping.
  void SetRefPlane(const gp_Ax2& theValue) { myRefPlane = theValue; }

  //! Apply flipping transformation to a Bnd_Box of presentation.
  //! @param theWorldView   [in]     the world view transformation matrix
  //! @param theBoundingBox [in,out] the bounding box to transform
  template <class T>
  void Apply(const NCollection_Mat4<T>& theWorldView, Bnd_Box& theBoundingBox) const;

  //! Apply flipping transformation to a BVH_Box of presentation.
  //! @param theWorldView   [in]     the world view transformation matrix
  //! @param theBoundingBox [in,out] the bounding box to transform
  template <class T>
  void Apply(const NCollection_Mat4<T>& theWorldView, BVH_Box<T, 3>& theBoundingBox) const;

  //! Compute the flipping transformation matrix.
  //! The returned matrix can be composed with the model-world transformation
  //! of an object to reproduce the render-time flip. Note: each branch of
  //! the flip is a 180 degree rotation (self-inverse), so the returned matrix
  //! is its own inverse.
  //! @param theWorldView [in] the composed World-View * ModelWorld matrix that
  //!                          the caller would use at draw time. To agree with
  //!                          OpenGl_Flipper::Render() the caller must include
  //!                          the owning object's Transformation() and, when
  //!                          available, the flipping group's own
  //!                          Transformation(). Passing the bare WorldView
  //!                          matrix is only correct for objects/groups with
  //!                          identity transformations.
  //! TODO: group-level Graphic3d_Group::Transformation() is not folded in by
  //! the selection pipeline yet (the sensitive entity does not carry a
  //! reference to its host group). This only affects consumers that give the
  //! flipping group its own gp_Trsf - uncommon in stock OCCT.
  template <class T>
  NCollection_Mat4<T> Compute(const NCollection_Mat4<T>& theWorldView) const;

  //! Dumps the content of me into the stream.
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const;

private:
  gp_Ax2 myRefPlane;
};

//=================================================================================================

template <class T>
void Graphic3d_Flipper::Apply(const NCollection_Mat4<T>& theWorldView,
                              Bnd_Box&                   theBoundingBox) const
{
  if (theBoundingBox.IsVoid())
  {
    return;
  }

  T aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  theBoundingBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  typename BVH_Box<T, 3>::BVH_VecNt aMin(aXmin, aYmin, aZmin);
  typename BVH_Box<T, 3>::BVH_VecNt aMax(aXmax, aYmax, aZmax);
  BVH_Box<T, 3>                     aBBox(aMin, aMax);

  Apply(theWorldView, aBBox);

  theBoundingBox = Bnd_Box();
  theBoundingBox.Update(aBBox.CornerMin().x(),
                        aBBox.CornerMin().y(),
                        aBBox.CornerMin().z(),
                        aBBox.CornerMax().x(),
                        aBBox.CornerMax().y(),
                        aBBox.CornerMax().z());
}

//=================================================================================================

template <class T>
void Graphic3d_Flipper::Apply(const NCollection_Mat4<T>& theWorldView,
                              BVH_Box<T, 3>&             theBoundingBox) const
{
  NCollection_Mat4<T> aTPers = Compute(theWorldView);
  if (aTPers.IsIdentity() || !theBoundingBox.IsValid())
  {
    return;
  }

  const typename BVH_Box<T, 3>::BVH_VecNt& aMin = theBoundingBox.CornerMin();
  const typename BVH_Box<T, 3>::BVH_VecNt& aMax = theBoundingBox.CornerMax();

  typename BVH_Box<T, 4>::BVH_VecNt anArrayOfCorners[8];
  anArrayOfCorners[0] =
    typename BVH_Box<T, 4>::BVH_VecNt(aMin.x(), aMin.y(), aMin.z(), static_cast<T>(1.0));
  anArrayOfCorners[1] =
    typename BVH_Box<T, 4>::BVH_VecNt(aMin.x(), aMin.y(), aMax.z(), static_cast<T>(1.0));
  anArrayOfCorners[2] =
    typename BVH_Box<T, 4>::BVH_VecNt(aMin.x(), aMax.y(), aMin.z(), static_cast<T>(1.0));
  anArrayOfCorners[3] =
    typename BVH_Box<T, 4>::BVH_VecNt(aMin.x(), aMax.y(), aMax.z(), static_cast<T>(1.0));
  anArrayOfCorners[4] =
    typename BVH_Box<T, 4>::BVH_VecNt(aMax.x(), aMin.y(), aMin.z(), static_cast<T>(1.0));
  anArrayOfCorners[5] =
    typename BVH_Box<T, 4>::BVH_VecNt(aMax.x(), aMin.y(), aMax.z(), static_cast<T>(1.0));
  anArrayOfCorners[6] =
    typename BVH_Box<T, 4>::BVH_VecNt(aMax.x(), aMax.y(), aMin.z(), static_cast<T>(1.0));
  anArrayOfCorners[7] =
    typename BVH_Box<T, 4>::BVH_VecNt(aMax.x(), aMax.y(), aMax.z(), static_cast<T>(1.0));

  theBoundingBox.Clear();
  for (int anIt = 0; anIt < 8; ++anIt)
  {
    typename BVH_Box<T, 4>::BVH_VecNt& aCorner = anArrayOfCorners[anIt];
    aCorner                                    = aTPers * aCorner;
    aCorner                                    = aCorner / aCorner.w();
    theBoundingBox.Add(typename BVH_Box<T, 3>::BVH_VecNt(aCorner.x(), aCorner.y(), aCorner.z()));
  }
}

//=================================================================================================

template <class T>
NCollection_Mat4<T> Graphic3d_Flipper::Compute(const NCollection_Mat4<T>& theWorldView) const
{
  NCollection_Vec4<T> aReferenceOrigin(static_cast<T>(myRefPlane.Location().X()),
                                       static_cast<T>(myRefPlane.Location().Y()),
                                       static_cast<T>(myRefPlane.Location().Z()),
                                       static_cast<T>(1));
  NCollection_Vec4<T> aReferenceX(static_cast<T>(myRefPlane.XDirection().X()),
                                  static_cast<T>(myRefPlane.XDirection().Y()),
                                  static_cast<T>(myRefPlane.XDirection().Z()),
                                  static_cast<T>(1));
  NCollection_Vec4<T> aReferenceY(static_cast<T>(myRefPlane.YDirection().X()),
                                  static_cast<T>(myRefPlane.YDirection().Y()),
                                  static_cast<T>(myRefPlane.YDirection().Z()),
                                  static_cast<T>(1));
  NCollection_Vec4<T> aReferenceZ(static_cast<T>(myRefPlane.Axis().Direction().X()),
                                  static_cast<T>(myRefPlane.Axis().Direction().Y()),
                                  static_cast<T>(myRefPlane.Axis().Direction().Z()),
                                  static_cast<T>(1));

  NCollection_Mat4<T> aMatrixMV;
  aMatrixMV.Convert(theWorldView);

  const NCollection_Vec4<T> aMVReferenceOrigin = aMatrixMV * aReferenceOrigin;
  const NCollection_Vec4<T> aMVReferenceX =
    aMatrixMV * NCollection_Vec4<T>(aReferenceX.xyz() + aReferenceOrigin.xyz(), static_cast<T>(1));
  const NCollection_Vec4<T> aMVReferenceY =
    aMatrixMV * NCollection_Vec4<T>(aReferenceY.xyz() + aReferenceOrigin.xyz(), static_cast<T>(1));
  const NCollection_Vec4<T> aMVReferenceZ =
    aMatrixMV * NCollection_Vec4<T>(aReferenceZ.xyz() + aReferenceOrigin.xyz(), static_cast<T>(1));

  const NCollection_Vec4<T> aDirX = aMVReferenceX - aMVReferenceOrigin;
  const NCollection_Vec4<T> aDirY = aMVReferenceY - aMVReferenceOrigin;
  const NCollection_Vec4<T> aDirZ = aMVReferenceZ - aMVReferenceOrigin;

  const bool isReversedX = aDirX.xyz().Dot(NCollection_Vec3<T>::DX()) < static_cast<T>(0);
  const bool isReversedY = aDirY.xyz().Dot(NCollection_Vec3<T>::DY()) < static_cast<T>(0);
  const bool isReversedZ = aDirZ.xyz().Dot(NCollection_Vec3<T>::DZ()) < static_cast<T>(0);

  NCollection_Mat4<T> aTransform;
  if ((isReversedX || isReversedY) && !isReversedZ)
  {
    // invert by Z axis: left, up vectors mirrored
    aTransform.SetColumn(0, -aTransform.GetColumn(0).xyz());
    aTransform.SetColumn(1, -aTransform.GetColumn(1).xyz());
  }
  else if (isReversedY && isReversedZ)
  {
    // rotate by X axis: up, forward vectors mirrored
    aTransform.SetColumn(1, -aTransform.GetColumn(1).xyz());
    aTransform.SetColumn(2, -aTransform.GetColumn(2).xyz());
  }
  else if (isReversedZ)
  {
    // rotate by Y axis: left, forward vectors mirrored
    aTransform.SetColumn(0, -aTransform.GetColumn(0).xyz());
    aTransform.SetColumn(2, -aTransform.GetColumn(2).xyz());
  }
  else
  {
    return NCollection_Mat4<T>();
  }

  NCollection_Mat4<T> aRefAxes;
  NCollection_Mat4<T> aRefInv;
  aRefAxes.SetColumn(0, aReferenceX.xyz());
  aRefAxes.SetColumn(1, aReferenceY.xyz());
  aRefAxes.SetColumn(2, aReferenceZ.xyz());
  aRefAxes.SetColumn(3, aReferenceOrigin.xyz());
  aRefAxes.Inverted(aRefInv);

  aTransform = aRefAxes * aTransform * aRefInv;
  return aTransform;
}

#endif // _Graphic3d_Flipper_HeaderFile
