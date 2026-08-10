// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <OpenGl_ShaderGrid.hxx>

#include <Precision.hxx>

#include <algorithm>
#include <cmath>

namespace
{
static constexpr double THE_TWO_PI = 2.0 * M_PI;

static bool isFiniteCoord(const double theValue)
{
  return std::isfinite(theValue) && !Precision::IsInfinite(theValue);
}

static bool isFinitePoint(const gp_Pnt& thePoint)
{
  return isFiniteCoord(thePoint.X()) && isFiniteCoord(thePoint.Y()) && isFiniteCoord(thePoint.Z());
}

static bool isFinitePoint(const gp_XYZ& thePoint)
{
  return isFiniteCoord(thePoint.X()) && isFiniteCoord(thePoint.Y()) && isFiniteCoord(thePoint.Z());
}

static double normalizedAngle(const double theAngle)
{
  double anAngle = std::fmod(theAngle, THE_TWO_PI);
  if (anAngle < 0.0)
  {
    anAngle += THE_TWO_PI;
  }
  return anAngle;
}

static double positiveAngleSpan(const double theStart, const double theEnd)
{
  double aSpan = std::fmod(theEnd - theStart, THE_TWO_PI);
  if (aSpan < 0.0)
  {
    aSpan += THE_TWO_PI;
  }

  if (std::abs(aSpan) <= Precision::Angular()
      && std::abs(theEnd - theStart) >= THE_TWO_PI - Precision::Angular())
  {
    return THE_TWO_PI;
  }
  return aSpan;
}

static bool isSamePoint(const gp_Pnt& theFirst, const gp_Pnt& theSecond)
{
  return theFirst.SquareDistance(theSecond) <= Precision::SquareConfusion();
}

static bool isSameDirection(const gp_Dir& theFirst, const gp_Dir& theSecond)
{
  return theFirst.Angle(theSecond) <= Precision::Angular();
}

static bool isSameScalar(const double theFirst, const double theSecond)
{
  return std::abs(theFirst - theSecond) <= Precision::Confusion();
}

static bool isSameAngle(const double theFirst, const double theSecond)
{
  double aDelta = std::abs(normalizedAngle(theFirst) - normalizedAngle(theSecond));
  aDelta        = std::min(aDelta, THE_TWO_PI - aDelta);
  return aDelta <= Precision::Angular();
}
} // namespace

//=================================================================================================

bool OpenGl_ShaderGrid::Display(const Aspect_GridParams&             theParams,
                                const gp_Ax3&                        thePlane,
                                const occ::handle<Graphic3d_Camera>& theCamera,
                                const occ::handle<OpenGl_Context>&   theContext)
{
  if (theParams.DrawMode() == Aspect_GDM_None)
  {
    Erase();
    return true;
  }

  const bool wasShown       = myIsShown;
  const bool wasBackground  = wasShown && myParams.IsBackground();
  const bool hasSameAnchor  = wasShown && hasSameAnchorFrame(theParams, thePlane);
  const bool toCaptureFrame = theParams.IsBackground() && !theContext.IsNull()
                              && (!wasShown || !wasBackground || !hasSameAnchor);

  myParams  = theParams;
  myPlane   = thePlane;
  myIsShown = true;

  if (theParams.IsViewAdaptive())
  {
    const double aReferenceScale =
      !theCamera.IsNull() && theCamera->Scale() > Precision::Confusion() ? theCamera->Scale() : 1.0;
    myParams.SetScale(theParams.Scale() * aReferenceScale);
    if (theParams.ScaleY() > 0.0)
    {
      myParams.SetScaleY(theParams.ScaleY() * aReferenceScale);
    }
  }

  if (toCaptureFrame)
  {
    myRefViewMatrix = theContext->WorldViewState.Current();
  }
  return true;
}

//=================================================================================================

void OpenGl_ShaderGrid::Erase()
{
  myIsShown = false;
}

//=================================================================================================

void OpenGl_ShaderGrid::frame(gp_Pnt& theOrigin, gp_XYZ& theX, gp_XYZ& theY, gp_XYZ& theN) const
{
  const double aCosA = std::cos(myParams.RotationAngle());
  const double aSinA = std::sin(myParams.RotationAngle());
  const gp_XYZ aRawX = myPlane.XDirection().XYZ();
  const gp_XYZ aRawY = myPlane.YDirection().XYZ();
  theX               = aRawX * aCosA - aRawY * aSinA;
  theY               = aRawX * aSinA + aRawY * aCosA;
  theN               = myPlane.Direction().XYZ();

  const gp_Pnt& anOriginLocal = myParams.Origin();
  theOrigin.SetXYZ(myPlane.Location().XYZ() + aRawX * anOriginLocal.X() + aRawY * anOriginLocal.Y()
                   + theN * (anOriginLocal.Z() + myParams.ZOffset()));
}

//=================================================================================================

void OpenGl_ShaderGrid::effectiveScale(const occ::handle<Graphic3d_Camera>& theCamera,
                                       double&                              theScaleX,
                                       double&                              theScaleY) const
{
  theScaleX = myParams.Scale();
  theScaleY = myParams.EffectiveScaleY();
  if (!myParams.IsViewAdaptive())
  {
    return;
  }

  const double aCurrentScale = !theCamera.IsNull() && theCamera->Scale() > Precision::Confusion()
                                 ? theCamera->Scale()
                                 : Precision::Confusion();
  theScaleX /= aCurrentScale;
  theScaleY /= aCurrentScale;
}

//=================================================================================================

bool OpenGl_ShaderGrid::angleInArc(const double theStart,
                                   const double theEnd,
                                   const double theAngle)
{
  const double aSpan = positiveAngleSpan(theStart, theEnd);
  if (aSpan >= THE_TWO_PI - Precision::Angular())
  {
    return true;
  }

  double aDelta = normalizedAngle(theAngle) - normalizedAngle(theStart);
  if (aDelta < 0.0)
  {
    aDelta += THE_TWO_PI;
  }
  return aDelta <= aSpan + Precision::Angular();
}

//=================================================================================================

bool OpenGl_ShaderGrid::isPointInBounds(const double theLocalX, const double theLocalY) const
{
  if (myParams.IsCircular())
  {
    const double aRadius = std::sqrt(theLocalX * theLocalX + theLocalY * theLocalY);
    if (myParams.Radius() > 0.0 && aRadius > myParams.Radius())
    {
      return false;
    }
    if (myParams.IsArc())
    {
      if (!angleInArc(myParams.AngleStart(), myParams.AngleEnd(), std::atan2(theLocalY, theLocalX)))
      {
        return false;
      }
    }
    return true;
  }

  return (myParams.SizeX() <= 0.0 || std::abs(theLocalX) <= myParams.SizeX() * 0.5)
         && (myParams.SizeY() <= 0.0 || std::abs(theLocalY) <= myParams.SizeY() * 0.5);
}

//=================================================================================================

void OpenGl_ShaderGrid::addLocalPoint(Bnd_Box&      theBox,
                                      const gp_Pnt& theOrigin,
                                      const gp_XYZ& theX,
                                      const gp_XYZ& theY,
                                      const double  theLocalX,
                                      const double  theLocalY)
{
  const gp_Pnt aPoint(theOrigin.XYZ() + theX * theLocalX + theY * theLocalY);
  if (isFinitePoint(aPoint))
  {
    theBox.Add(aPoint);
  }
}

//=================================================================================================

void OpenGl_ShaderGrid::addFiniteBounds(Bnd_Box& theBox) const
{
  gp_Pnt anOrigin;
  gp_XYZ aGridX, aGridY, aGridN;
  frame(anOrigin, aGridX, aGridY, aGridN);

  addLocalPoint(theBox, anOrigin, aGridX, aGridY, 0.0, 0.0);
  if (myParams.IsCircular())
  {
    const double aRadius = myParams.Radius();
    if (aRadius <= 0.0)
    {
      return;
    }

    if (!myParams.IsArc())
    {
      addLocalPoint(theBox, anOrigin, aGridX, aGridY, aRadius, 0.0);
      addLocalPoint(theBox, anOrigin, aGridX, aGridY, -aRadius, 0.0);
      addLocalPoint(theBox, anOrigin, aGridX, aGridY, 0.0, aRadius);
      addLocalPoint(theBox, anOrigin, aGridX, aGridY, 0.0, -aRadius);
      return;
    }

    auto addArcPoint = [&](const double theAngle) {
      addLocalPoint(theBox,
                    anOrigin,
                    aGridX,
                    aGridY,
                    aRadius * std::cos(theAngle),
                    aRadius * std::sin(theAngle));
    };

    addArcPoint(myParams.AngleStart());
    addArcPoint(myParams.AngleEnd());

    const double aCardinalAngles[] = {0.0, M_PI * 0.5, M_PI, -M_PI * 0.5};
    for (const double anAngle : aCardinalAngles)
    {
      if (angleInArc(myParams.AngleStart(), myParams.AngleEnd(), anAngle))
      {
        addArcPoint(anAngle);
      }
    }
    return;
  }

  const double aHalfX = myParams.SizeX() > 0.0 ? myParams.SizeX() * 0.5 : 0.0;
  const double aHalfY = myParams.SizeY() > 0.0 ? myParams.SizeY() * 0.5 : 0.0;
  if (aHalfX <= 0.0 && aHalfY <= 0.0)
  {
    return;
  }

  if (aHalfX > 0.0 && aHalfY > 0.0)
  {
    addLocalPoint(theBox, anOrigin, aGridX, aGridY, -aHalfX, -aHalfY);
    addLocalPoint(theBox, anOrigin, aGridX, aGridY, aHalfX, -aHalfY);
    addLocalPoint(theBox, anOrigin, aGridX, aGridY, aHalfX, aHalfY);
    addLocalPoint(theBox, anOrigin, aGridX, aGridY, -aHalfX, aHalfY);
    return;
  }

  if (aHalfX > 0.0)
  {
    addLocalPoint(theBox, anOrigin, aGridX, aGridY, -aHalfX, 0.0);
    addLocalPoint(theBox, anOrigin, aGridX, aGridY, aHalfX, 0.0);
  }
  if (aHalfY > 0.0)
  {
    addLocalPoint(theBox, anOrigin, aGridX, aGridY, 0.0, -aHalfY);
    addLocalPoint(theBox, anOrigin, aGridX, aGridY, 0.0, aHalfY);
  }
}

//=================================================================================================

bool OpenGl_ShaderGrid::planeLocalHit(const occ::handle<Graphic3d_Camera>& theCamera,
                                      const double                         theNdcX,
                                      const double                         theNdcY,
                                      double&                              theLocalX,
                                      double&                              theLocalY,
                                      gp_XYZ*                              theHit,
                                      const bool                           theToRejectBehind) const
{
  if (theCamera.IsNull())
  {
    return false;
  }

  gp_Pnt aPlaneOrigin;
  gp_XYZ aPlaneX, aPlaneY, aPlaneN;
  frame(aPlaneOrigin, aPlaneX, aPlaneY, aPlaneN);

  const double aNearZ        = theCamera->IsZeroToOneDepth() ? 0.0 : -1.0;
  const gp_Pnt aNearP        = theCamera->UnProject(gp_Pnt(theNdcX, theNdcY, aNearZ));
  const gp_Pnt aFarP         = theCamera->UnProject(gp_Pnt(theNdcX, theNdcY, 1.0));
  const bool   isPerspective = !theCamera->IsOrthographic();
  const gp_Pnt aRayOriginP   = isPerspective ? theCamera->Eye() : aNearP;
  const gp_XYZ aRay          = aFarP.XYZ() - aRayOriginP.XYZ();
  const double aRayMod       = aRay.Modulus();
  if (aRayMod <= Precision::Confusion())
  {
    return false;
  }

  const double aDenom = aPlaneN.Dot(aRay);
  if (std::abs(aDenom / aRayMod) <= Precision::Angular())
  {
    return false;
  }

  const double aT = aPlaneN.Dot(aPlaneOrigin.XYZ() - aRayOriginP.XYZ()) / aDenom;
  if (theToRejectBehind && isPerspective && aT < 0.0)
  {
    return false;
  }

  const gp_XYZ aHit    = aRayOriginP.XYZ() + aRay * aT;
  const gp_XYZ aLocal3 = aHit - aPlaneOrigin.XYZ();
  theLocalX            = aLocal3.Dot(aPlaneX);
  theLocalY            = aLocal3.Dot(aPlaneY);
  if (theHit != nullptr)
  {
    *theHit = aHit;
  }
  return true;
}

//=================================================================================================

void OpenGl_ShaderGrid::addViewFootprintBounds(Bnd_Box&                             theBox,
                                               const occ::handle<Graphic3d_Camera>& theCamera) const
{
  const double aSamples[][2] = {{-1.0, -1.0}, {1.0, -1.0}, {1.0, 1.0}, {-1.0, 1.0}, {0.0, 0.0}};
  for (const double* aSample : aSamples)
  {
    double aLocalX = 0.0;
    double aLocalY = 0.0;
    gp_XYZ aHit;
    if (!planeLocalHit(theCamera, aSample[0], aSample[1], aLocalX, aLocalY, &aHit))
    {
      continue;
    }
    if (!isPointInBounds(aLocalX, aLocalY))
    {
      continue;
    }
    if (isFinitePoint(aHit))
    {
      theBox.Add(gp_Pnt(aHit));
    }
  }
}

//=================================================================================================

void OpenGl_ShaderGrid::AddZFitBounds(Bnd_Box&                             theGraphicBox,
                                      const occ::handle<Graphic3d_Camera>& theCamera) const
{
  if (!myIsShown || myParams.DrawMode() == Aspect_GDM_None || myParams.IsBackground())
  {
    return;
  }

  Bnd_Box aGridBox;
  if (myParams.IsBounded())
  {
    addFiniteBounds(aGridBox);
  }
  else
  {
    addViewFootprintBounds(aGridBox, theCamera);
  }

  if (aGridBox.IsVoid())
  {
    addFiniteBounds(aGridBox);
  }
  if (!aGridBox.IsVoid())
  {
    theGraphicBox.Add(aGridBox);
  }
}

//=================================================================================================

bool OpenGl_ShaderGrid::hasSameAnchorFrame(const Aspect_GridParams& theParams,
                                           const gp_Ax3&            thePlane) const
{
  return isSamePoint(myPlane.Location(), thePlane.Location())
         && isSameDirection(myPlane.Direction(), thePlane.Direction())
         && isSameDirection(myPlane.XDirection(), thePlane.XDirection())
         && isSameDirection(myPlane.YDirection(), thePlane.YDirection())
         && isSamePoint(myParams.Origin(), theParams.Origin())
         && isSameAngle(myParams.RotationAngle(), theParams.RotationAngle())
         && isSameScalar(myParams.ZOffset(), theParams.ZOffset());
}

//=================================================================================================

bool OpenGl_ShaderGrid::referenceLocal(const occ::handle<Graphic3d_Camera>& theCamera,
                                       double&                              theLocalX,
                                       double&                              theLocalY) const
{
  const double aSamples[][2] = {{0.0, 0.0},
                                {-1.0, -1.0},
                                {1.0, -1.0},
                                {1.0, 1.0},
                                {-1.0, 1.0},
                                {0.0, -1.0},
                                {1.0, 0.0},
                                {0.0, 1.0},
                                {-1.0, 0.0}};
  for (const double* aSample : aSamples)
  {
    if (planeLocalHit(theCamera, aSample[0], aSample[1], theLocalX, theLocalY))
    {
      return true;
    }
  }
  return false;
}

//=================================================================================================

double OpenGl_ShaderGrid::snappedLocalShift(const double theLocal, const double theScale)
{
  if (theScale <= Precision::Confusion())
  {
    return 0.0;
  }

  const double aStep = 1.0 / theScale;
  return std::round(theLocal / aStep) * aStep;
}

//=================================================================================================

NCollection_Vec3<float> OpenGl_ShaderGrid::viewPoint(const NCollection_Mat4<float>& theWorldView,
                                                     const gp_Pnt&                  thePoint)
{
  const NCollection_Vec4<float> aPoint(float(thePoint.X()),
                                       float(thePoint.Y()),
                                       float(thePoint.Z()),
                                       1.0f);
  const NCollection_Vec4<float> aView = theWorldView * aPoint;
  return NCollection_Vec3<float>(aView.x(), aView.y(), aView.z());
}

//=================================================================================================

NCollection_Vec3<float> OpenGl_ShaderGrid::viewDirection(
  const NCollection_Mat4<float>& theWorldView,
  const gp_XYZ&                  theDirection)
{
  const NCollection_Vec4<float> aDir(float(theDirection.X()),
                                     float(theDirection.Y()),
                                     float(theDirection.Z()),
                                     0.0f);
  const NCollection_Vec4<float> aView = theWorldView * aDir;
  return NCollection_Vec3<float>(aView.x(), aView.y(), aView.z());
}

//=================================================================================================

bool OpenGl_ShaderGrid::acceptEchoCandidate(const occ::handle<Graphic3d_Camera>& theCamera,
                                            const int                            theWidth,
                                            const int                            theHeight,
                                            const int                            theX,
                                            const int                            theY,
                                            const gp_Pnt&                        theGridOrigin,
                                            const gp_XYZ&                        theGridX,
                                            const gp_XYZ&                        theGridY,
                                            const double                         theLocalX,
                                            const double                         theLocalY,
                                            gp_XYZ&                              theBestSnapped,
                                            double&                              theBestDist2,
                                            bool& theHasBestPoint) const
{
  if (!isPointInBounds(theLocalX, theLocalY))
  {
    return false;
  }

  const gp_XYZ aCandidate = theGridOrigin.XYZ() + theGridX * theLocalX + theGridY * theLocalY;
  if (!isFinitePoint(aCandidate))
  {
    return false;
  }
  if (!theCamera->IsOrthographic()
      && theCamera->Direction().XYZ().Dot(aCandidate - theCamera->Eye().XYZ())
           <= Precision::Confusion())
  {
    return false;
  }

  const gp_Pnt aProj = theCamera->Project(gp_Pnt(aCandidate));
  if (!isFinitePoint(aProj))
  {
    return false;
  }

  const double aPx    = (aProj.X() + 1.0) * 0.5 * double(theWidth);
  const double aPy    = double(theHeight - 1) - (aProj.Y() + 1.0) * 0.5 * double(theHeight);
  const double aDistX = aPx - double(theX);
  const double aDistY = aPy - double(theY);
  if (!isFiniteCoord(aDistX) || !isFiniteCoord(aDistY))
  {
    return false;
  }

  const double aDist2 = aDistX * aDistX + aDistY * aDistY;
  if (!isFiniteCoord(aDist2))
  {
    return false;
  }
  if (!theHasBestPoint || aDist2 < theBestDist2)
  {
    theHasBestPoint = true;
    theBestDist2    = aDist2;
    theBestSnapped  = aCandidate;
  }
  return true;
}

//=================================================================================================

bool OpenGl_ShaderGrid::echoDisplayPoint(const occ::handle<Graphic3d_Camera>& theCamera,
                                         const gp_XYZ&                        theSnapped,
                                         gp_Pnt&                              theDisplayPoint)
{
  const gp_Pnt aProjSnapped = theCamera->Project(gp_Pnt(theSnapped));
  if (!isFinitePoint(aProjSnapped))
  {
    return false;
  }
  const double aDisplayZ = theCamera->IsZeroToOneDepth() ? 0.5 : 0.0;
  theDisplayPoint = theCamera->UnProject(gp_Pnt(aProjSnapped.X(), aProjSnapped.Y(), aDisplayZ));
  return isFinitePoint(theDisplayPoint);
}

//=================================================================================================

bool OpenGl_ShaderGrid::Echo(const occ::handle<Graphic3d_Camera>& theCamera,
                             const int                            theWidth,
                             const int                            theHeight,
                             const int                            theX,
                             const int                            theY,
                             Graphic3d_Vertex&                    thePoint,
                             Graphic3d_Vertex&                    theDisplayPoint) const
{
  if (!myIsShown || myParams.DrawMode() == Aspect_GDM_None || myParams.IsBackground()
      || theCamera.IsNull() || theWidth <= 0 || theHeight <= 0)
  {
    return false;
  }

  const double aNdcX   = 2.0 * double(theX) / double(theWidth) - 1.0;
  const double aNdcY   = 2.0 * double(theHeight - 1 - theY) / double(theHeight) - 1.0;
  double       aLocalX = 0.0;
  double       aLocalY = 0.0;
  if (!planeLocalHit(theCamera, aNdcX, aNdcY, aLocalX, aLocalY))
  {
    return false;
  }
  if (!isPointInBounds(aLocalX, aLocalY))
  {
    return false;
  }

  double aScaleX = 0.0;
  double aScaleY = 0.0;
  effectiveScale(theCamera, aScaleX, aScaleY);
  if (aScaleX <= Precision::Confusion() || aScaleY <= Precision::Confusion())
  {
    return false;
  }

  gp_Pnt aGridOrigin;
  gp_XYZ aGridX, aGridY, aGridN;
  frame(aGridOrigin, aGridX, aGridY, aGridN);

  gp_XYZ     aSnapped       = aGridOrigin.XYZ();
  const bool toSnapByScreen = myParams.DrawMode() == Aspect_GDM_Points;
  double     aBestDist2     = RealLast();
  bool       hasBestPoint   = false;
  auto       addCandidate   = [&](const double theLocalX, const double theLocalY) {
    acceptEchoCandidate(theCamera,
                        theWidth,
                        theHeight,
                        theX,
                        theY,
                        aGridOrigin,
                        aGridX,
                        aGridY,
                        theLocalX,
                        theLocalY,
                        aSnapped,
                        aBestDist2,
                        hasBestPoint);
  };

  if (myParams.IsCircular())
  {
    const double aRadius      = std::sqrt(aLocalX * aLocalX + aLocalY * aLocalY);
    const double anAngle      = std::atan2(aLocalY, aLocalX);
    const double aRadiusStep  = 1.0 / aScaleX;
    const int    aNbDivisions = myParams.AngularDivisions();
    const double anAngleStep  = aNbDivisions > 0 ? M_PI / double(aNbDivisions) : M_PI;
    if (toSnapByScreen)
    {
      const double aRadiusIndex = std::floor(aRadius / aRadiusStep);
      const double anAngleIndex = std::floor(anAngle / anAngleStep);
      const double aRadii[2]    = {std::max(0.0, aRadiusIndex * aRadiusStep),
                                   std::max(0.0, (aRadiusIndex + 1.0) * aRadiusStep)};
      const double anAngles[2]  = {anAngleIndex * anAngleStep, (anAngleIndex + 1.0) * anAngleStep};
      for (double aCandidateRadius : aRadii)
      {
        for (double aCandidateAngle : anAngles)
        {
          addCandidate(aCandidateRadius * std::cos(aCandidateAngle),
                       aCandidateRadius * std::sin(aCandidateAngle));
        }
      }
      if (!hasBestPoint)
      {
        return false;
      }
    }
    else
    {
      const double aSnapRadius = std::round(aRadius / aRadiusStep) * aRadiusStep;
      const double aSnapAngle  = std::round(anAngle / anAngleStep) * anAngleStep;
      if (!acceptEchoCandidate(theCamera,
                               theWidth,
                               theHeight,
                               theX,
                               theY,
                               aGridOrigin,
                               aGridX,
                               aGridY,
                               aSnapRadius * std::cos(aSnapAngle),
                               aSnapRadius * std::sin(aSnapAngle),
                               aSnapped,
                               aBestDist2,
                               hasBestPoint))
      {
        return false;
      }
    }
  }
  else
  {
    const double aStepX = 1.0 / aScaleX;
    const double aStepY = 1.0 / aScaleY;
    if (toSnapByScreen)
    {
      const double anIndexX    = std::floor(aLocalX / aStepX);
      const double anIndexY    = std::floor(aLocalY / aStepY);
      const double aLocalXs[2] = {anIndexX * aStepX, (anIndexX + 1.0) * aStepX};
      const double aLocalYs[2] = {anIndexY * aStepY, (anIndexY + 1.0) * aStepY};
      for (double aCandidateX : aLocalXs)
      {
        for (double aCandidateY : aLocalYs)
        {
          addCandidate(aCandidateX, aCandidateY);
        }
      }
      if (!hasBestPoint)
      {
        return false;
      }
    }
    else
    {
      aLocalX = std::round(aLocalX / aStepX) * aStepX;
      aLocalY = std::round(aLocalY / aStepY) * aStepY;
      if (!acceptEchoCandidate(theCamera,
                               theWidth,
                               theHeight,
                               theX,
                               theY,
                               aGridOrigin,
                               aGridX,
                               aGridY,
                               aLocalX,
                               aLocalY,
                               aSnapped,
                               aBestDist2,
                               hasBestPoint))
      {
        return false;
      }
    }
  }

  if (!hasBestPoint)
  {
    return false;
  }

  thePoint.SetCoord(aSnapped.X(), aSnapped.Y(), aSnapped.Z());
  gp_Pnt aDisplayP;
  if (!echoDisplayPoint(theCamera, aSnapped, aDisplayP))
  {
    return false;
  }
  theDisplayPoint.SetCoord(aDisplayP.X(), aDisplayP.Y(), aDisplayP.Z());
  return true;
}

//=================================================================================================

bool OpenGl_ShaderGrid::SnapPoint(const occ::handle<Graphic3d_Camera>& theCamera,
                                  const Graphic3d_Vertex&              thePoint,
                                  Graphic3d_Vertex&                    theGridPoint) const
{
  if (!myIsShown || myParams.DrawMode() == Aspect_GDM_None || myParams.IsBackground())
  {
    return false;
  }

  double aScaleX = 0.0;
  double aScaleY = 0.0;
  effectiveScale(theCamera, aScaleX, aScaleY);
  if (aScaleX <= Precision::Confusion() || aScaleY <= Precision::Confusion())
  {
    return false;
  }

  gp_Pnt aGridOrigin;
  gp_XYZ aGridX, aGridY, aGridN;
  frame(aGridOrigin, aGridX, aGridY, aGridN);

  const gp_XYZ aPoint(thePoint.X(), thePoint.Y(), thePoint.Z());
  const gp_XYZ aLocal3 = aPoint - aGridOrigin.XYZ();
  double       aLocalX = aLocal3.Dot(aGridX);
  double       aLocalY = aLocal3.Dot(aGridY);

  gp_XYZ aSnapped = aGridOrigin.XYZ();
  if (myParams.IsCircular())
  {
    const double aRadius      = std::sqrt(aLocalX * aLocalX + aLocalY * aLocalY);
    const double anAngle      = std::atan2(aLocalY, aLocalX);
    const double aRadiusStep  = 1.0 / aScaleX;
    const int    aNbDivisions = myParams.AngularDivisions();
    const double anAngleStep  = aNbDivisions > 0 ? M_PI / double(aNbDivisions) : M_PI;
    const double aSnapRadius  = std::round(aRadius / aRadiusStep) * aRadiusStep;
    const double aSnapAngle   = std::round(anAngle / anAngleStep) * anAngleStep;
    aLocalX                   = aSnapRadius * std::cos(aSnapAngle);
    aLocalY                   = aSnapRadius * std::sin(aSnapAngle);
    if (!isPointInBounds(aLocalX, aLocalY))
    {
      return false;
    }
    aSnapped += aGridX * aLocalX + aGridY * aLocalY;
  }
  else
  {
    const double aStepX = 1.0 / aScaleX;
    const double aStepY = 1.0 / aScaleY;
    aLocalX             = std::round(aLocalX / aStepX) * aStepX;
    aLocalY             = std::round(aLocalY / aStepY) * aStepY;
    if (!isPointInBounds(aLocalX, aLocalY))
    {
      return false;
    }
    aSnapped += aGridX * aLocalX + aGridY * aLocalY;
  }

  theGridPoint.SetCoord(aSnapped.X(), aSnapped.Y(), aSnapped.Z());
  return true;
}

//=================================================================================================

NCollection_Mat4<float> OpenGl_ShaderGrid::DrawWorldView(
  const NCollection_Mat4<float>& theCurrentWorldView) const
{
  if (!myParams.IsBackground())
  {
    return theCurrentWorldView;
  }

  NCollection_Mat4<float> aRefInv;
  if (!myRefViewMatrix.Inverted(aRefInv))
  {
    aRefInv.InitIdentity();
  }
  return theCurrentWorldView * aRefInv;
}

//=================================================================================================

void OpenGl_ShaderGrid::SetUniforms(const occ::handle<OpenGl_Context>&       theContext,
                                    const occ::handle<OpenGl_ShaderProgram>& theProgram,
                                    const occ::handle<Graphic3d_Camera>&     theCamera,
                                    const NCollection_Mat4<float>&           theWorldView) const
{
  double aScaleX = 0.0;
  double aScaleY = 0.0;
  effectiveScale(theCamera, aScaleX, aScaleY);

  gp_Pnt aPlaneOrigin;
  gp_XYZ aXRotated, aYRotated, aNDir;
  frame(aPlaneOrigin, aXRotated, aYRotated, aNDir);

  theProgram->SetUniform(theContext, "uScaleX", GLfloat(aScaleX));
  theProgram->SetUniform(theContext, "uScaleY", GLfloat(aScaleY));
  theProgram->SetUniform(theContext, "uThickness", GLfloat(myParams.LineThickness()));
  theProgram->SetUniform(theContext,
                         "uColor",
                         NCollection_Vec3<float>((float)myParams.Color().Red(),
                                                 (float)myParams.Color().Green(),
                                                 (float)myParams.Color().Blue()));
  theProgram->SetUniform(theContext,
                         "uAccentColor",
                         NCollection_Vec3<float>((float)myParams.AccentColor().Red(),
                                                 (float)myParams.AccentColor().Green(),
                                                 (float)myParams.AccentColor().Blue()));
  theProgram->SetUniform(theContext, "uAccentScaleX", GLfloat(myParams.AccentScaleX()));
  theProgram->SetUniform(theContext, "uAccentScaleY", GLfloat(myParams.AccentScaleY()));
  theProgram->SetUniform(theContext, "uAccentAngularScale", GLfloat(myParams.AccentAngularScale()));
  theProgram->SetUniform(theContext, "uIsDrawAxis", myParams.IsDrawAxis() ? 1 : 0);
  theProgram->SetUniform(theContext, "uGridType", myParams.IsCircular() ? 1 : 0);
  theProgram->SetUniform(theContext, "uIsBackground", myParams.IsBackground() ? 1 : 0);

  const double aAngularScale =
    myParams.IsCircular() ? double(myParams.AngularDivisions()) / M_PI : 0.0;
  theProgram->SetUniform(theContext, "uAngularScale", GLfloat(aAngularScale));
  theProgram->SetUniform(theContext, "uDrawMode", myParams.DrawMode() == Aspect_GDM_Points ? 1 : 0);
  theProgram->SetUniform(theContext, "uIsPerspective", theCamera->IsOrthographic() ? 0 : 1);
  theProgram->SetUniform(theContext, "uIsZeroToOneDepth", theCamera->IsZeroToOneDepth() ? 1 : 0);
  theProgram->SetUniform(theContext, "uParallelTolerance", GLfloat(Precision::Angular()));

  NCollection_Vec2<float> aLocalOriginShift(0.0f, 0.0f);
  NCollection_Vec2<float> anAccentLocalOriginShift(0.0f, 0.0f);
  float                   aRadialOriginShift        = 0.0f;
  float                   anAccentRadialOriginShift = 0.0f;
  double                  aLocalRefX                = 0.0;
  double                  aLocalRefY                = 0.0;
  if (referenceLocal(theCamera, aLocalRefX, aLocalRefY))
  {
    if (myParams.IsCircular())
    {
      const double aRadiusRef   = std::sqrt(aLocalRefX * aLocalRefX + aLocalRefY * aLocalRefY);
      aRadialOriginShift        = float(snappedLocalShift(aRadiusRef, aScaleX));
      anAccentRadialOriginShift = myParams.AccentScaleX() > Precision::Confusion()
                                    ? float(snappedLocalShift(aRadiusRef, myParams.AccentScaleX()))
                                    : aRadialOriginShift;
    }
    else
    {
      aLocalOriginShift.SetValues(float(snappedLocalShift(aLocalRefX, aScaleX)),
                                  float(snappedLocalShift(aLocalRefY, aScaleY)));
      anAccentLocalOriginShift.SetValues(
        myParams.AccentScaleX() > Precision::Confusion()
          ? float(snappedLocalShift(aLocalRefX, myParams.AccentScaleX()))
          : aLocalOriginShift.x(),
        myParams.AccentScaleY() > Precision::Confusion()
          ? float(snappedLocalShift(aLocalRefY, myParams.AccentScaleY()))
          : aLocalOriginShift.y());
    }
  }

  const gp_Pnt aPlaneRef(aPlaneOrigin.XYZ() + aXRotated * double(aLocalOriginShift.x())
                         + aYRotated * double(aLocalOriginShift.y()));
  theProgram->SetUniform(theContext, "uPlaneOriginView", viewPoint(theWorldView, aPlaneOrigin));
  theProgram->SetUniform(theContext, "uPlaneRefView", viewPoint(theWorldView, aPlaneRef));
  theProgram->SetUniform(theContext, "uPlaneXView", viewDirection(theWorldView, aXRotated));
  theProgram->SetUniform(theContext, "uPlaneYView", viewDirection(theWorldView, aYRotated));
  theProgram->SetUniform(theContext, "uPlaneNView", viewDirection(theWorldView, aNDir));
  theProgram->SetUniform(theContext, "uLocalOriginShift", aLocalOriginShift);
  theProgram->SetUniform(theContext, "uAccentLocalOriginShift", anAccentLocalOriginShift);
  theProgram->SetUniform(theContext, "uRadialOriginShift", GLfloat(aRadialOriginShift));
  theProgram->SetUniform(theContext,
                         "uAccentRadialOriginShift",
                         GLfloat(anAccentRadialOriginShift));

  const float aHalfX  = myParams.SizeX() > 0.0 ? float(myParams.SizeX() * 0.5) : 0.0f;
  const float aHalfY  = myParams.SizeY() > 0.0 ? float(myParams.SizeY() * 0.5) : 0.0f;
  const float aRadius = myParams.Radius() > 0.0 ? float(myParams.Radius()) : 0.0f;
  theProgram->SetUniform(theContext, "uBounds", NCollection_Vec3<float>(aHalfX, aHalfY, aRadius));
  theProgram->SetUniform(theContext,
                         "uIsBoundFade",
                         myParams.IsBounded() && !myParams.IsViewAdaptive() ? 1 : 0);
  theProgram->SetUniform(
    theContext,
    "uArcRange",
    NCollection_Vec2<float>(float(myParams.AngleStart()), float(myParams.AngleEnd())));
  theProgram->SetUniform(theContext, "uArcBounded", myParams.IsArc() ? 1 : 0);
}
