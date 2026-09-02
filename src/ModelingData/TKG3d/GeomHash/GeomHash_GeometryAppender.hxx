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

#ifndef _GeomHash_GeometryAppender_HeaderFile
#define _GeomHash_GeometryAppender_HeaderFile

#include <GeomHash_Accumulator.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_Circle.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Standard_NotImplemented.hxx>

template <class TheSinkType = GeomHash_Accumulator<uint64_t>>
class GeomHash_GeometryAppender
{
public:
  using result_type = typename TheSinkType::result_type;

  result_type operator()(const occ::handle<Geom_Curve>& theCurve) const
  {
    TheSinkType aSink;
    aSink.AppendUInt32(0x47433344u);
    aSink.AppendBool(!theCurve.IsNull());
    if (!theCurve.IsNull())
    {
      AppendCurve(aSink, theCurve);
    }
    return aSink.Finish();
  }

  result_type operator()(const occ::handle<Geom2d_Curve>& theCurve) const
  {
    TheSinkType aSink;
    aSink.AppendUInt32(0x47433244u);
    aSink.AppendBool(!theCurve.IsNull());
    if (!theCurve.IsNull())
    {
      AppendCurve(aSink, theCurve);
    }
    return aSink.Finish();
  }

  result_type operator()(const occ::handle<Geom_Surface>& theSurface) const
  {
    TheSinkType aSink;
    aSink.AppendUInt32(0x47535552u);
    aSink.AppendBool(!theSurface.IsNull());
    if (!theSurface.IsNull())
    {
      AppendSurface(aSink, theSurface);
    }
    return aSink.Finish();
  }

  static void AppendCurve(TheSinkType& theSink, const occ::handle<Geom_Curve>& theCurve)
  {
    const occ::handle<Standard_Type>& aType = theCurve->DynamicType();
    if (aType == STANDARD_TYPE(Geom_Line))
    {
      theSink.AppendByte(1);
      appendAxis(theSink, occ::down_cast<Geom_Line>(theCurve)->Position());
    }
    else if (aType == STANDARD_TYPE(Geom_Circle))
    {
      const occ::handle<Geom_Circle> aCircle = occ::down_cast<Geom_Circle>(theCurve);
      theSink.AppendByte(2);
      appendAxis(theSink, aCircle->Position());
      theSink.AppendDouble(aCircle->Radius());
    }
    else if (aType == STANDARD_TYPE(Geom_Ellipse))
    {
      const occ::handle<Geom_Ellipse> anEllipse = occ::down_cast<Geom_Ellipse>(theCurve);
      theSink.AppendByte(3);
      appendAxis(theSink, anEllipse->Position());
      theSink.AppendDouble(anEllipse->MajorRadius());
      theSink.AppendDouble(anEllipse->MinorRadius());
    }
    else if (aType == STANDARD_TYPE(Geom_Parabola))
    {
      const occ::handle<Geom_Parabola> aParabola = occ::down_cast<Geom_Parabola>(theCurve);
      theSink.AppendByte(4);
      appendAxis(theSink, aParabola->Position());
      theSink.AppendDouble(aParabola->Focal());
    }
    else if (aType == STANDARD_TYPE(Geom_Hyperbola))
    {
      const occ::handle<Geom_Hyperbola> aHyperbola = occ::down_cast<Geom_Hyperbola>(theCurve);
      theSink.AppendByte(5);
      appendAxis(theSink, aHyperbola->Position());
      theSink.AppendDouble(aHyperbola->MajorRadius());
      theSink.AppendDouble(aHyperbola->MinorRadius());
    }
    else if (aType == STANDARD_TYPE(Geom_BezierCurve))
    {
      appendBezierCurve(theSink, occ::down_cast<Geom_BezierCurve>(theCurve));
    }
    else if (aType == STANDARD_TYPE(Geom_BSplineCurve))
    {
      appendBSplineCurve(theSink, occ::down_cast<Geom_BSplineCurve>(theCurve));
    }
    else if (aType == STANDARD_TYPE(Geom_TrimmedCurve))
    {
      const occ::handle<Geom_TrimmedCurve> aTrimmed = occ::down_cast<Geom_TrimmedCurve>(theCurve);
      theSink.AppendByte(8);
      theSink.AppendDouble(aTrimmed->FirstParameter());
      theSink.AppendDouble(aTrimmed->LastParameter());
      AppendCurve(theSink, aTrimmed->BasisCurve());
    }
    else if (aType == STANDARD_TYPE(Geom_OffsetCurve))
    {
      const occ::handle<Geom_OffsetCurve> anOffset = occ::down_cast<Geom_OffsetCurve>(theCurve);
      theSink.AppendByte(9);
      theSink.AppendDouble(anOffset->Offset());
      appendDirection(theSink, anOffset->Direction());
      AppendCurve(theSink, anOffset->BasisCurve());
    }
    else
    {
      throw Standard_NotImplemented("GeomHash_GeometryAppender: unsupported 3D curve");
    }
  }

  static void AppendCurve(TheSinkType& theSink, const occ::handle<Geom2d_Curve>& theCurve)
  {
    const occ::handle<Standard_Type>& aType = theCurve->DynamicType();
    if (aType == STANDARD_TYPE(Geom2d_Line))
    {
      theSink.AppendByte(1);
      appendAxis(theSink, occ::down_cast<Geom2d_Line>(theCurve)->Position());
    }
    else if (aType == STANDARD_TYPE(Geom2d_Circle))
    {
      const occ::handle<Geom2d_Circle> aCircle = occ::down_cast<Geom2d_Circle>(theCurve);
      theSink.AppendByte(2);
      appendAxis(theSink, aCircle->Position());
      theSink.AppendDouble(aCircle->Radius());
    }
    else if (aType == STANDARD_TYPE(Geom2d_Ellipse))
    {
      const occ::handle<Geom2d_Ellipse> anEllipse = occ::down_cast<Geom2d_Ellipse>(theCurve);
      theSink.AppendByte(3);
      appendAxis(theSink, anEllipse->Position());
      theSink.AppendDouble(anEllipse->MajorRadius());
      theSink.AppendDouble(anEllipse->MinorRadius());
    }
    else if (aType == STANDARD_TYPE(Geom2d_Parabola))
    {
      const occ::handle<Geom2d_Parabola> aParabola = occ::down_cast<Geom2d_Parabola>(theCurve);
      theSink.AppendByte(4);
      appendAxis(theSink, aParabola->Position());
      theSink.AppendDouble(aParabola->Focal());
    }
    else if (aType == STANDARD_TYPE(Geom2d_Hyperbola))
    {
      const occ::handle<Geom2d_Hyperbola> aHyperbola = occ::down_cast<Geom2d_Hyperbola>(theCurve);
      theSink.AppendByte(5);
      appendAxis(theSink, aHyperbola->Position());
      theSink.AppendDouble(aHyperbola->MajorRadius());
      theSink.AppendDouble(aHyperbola->MinorRadius());
    }
    else if (aType == STANDARD_TYPE(Geom2d_BezierCurve))
    {
      appendBezierCurve(theSink, occ::down_cast<Geom2d_BezierCurve>(theCurve));
    }
    else if (aType == STANDARD_TYPE(Geom2d_BSplineCurve))
    {
      appendBSplineCurve(theSink, occ::down_cast<Geom2d_BSplineCurve>(theCurve));
    }
    else if (aType == STANDARD_TYPE(Geom2d_TrimmedCurve))
    {
      const occ::handle<Geom2d_TrimmedCurve> aTrimmed =
        occ::down_cast<Geom2d_TrimmedCurve>(theCurve);
      theSink.AppendByte(8);
      theSink.AppendDouble(aTrimmed->FirstParameter());
      theSink.AppendDouble(aTrimmed->LastParameter());
      AppendCurve(theSink, aTrimmed->BasisCurve());
    }
    else if (aType == STANDARD_TYPE(Geom2d_OffsetCurve))
    {
      const occ::handle<Geom2d_OffsetCurve> anOffset = occ::down_cast<Geom2d_OffsetCurve>(theCurve);
      theSink.AppendByte(9);
      theSink.AppendDouble(anOffset->Offset());
      AppendCurve(theSink, anOffset->BasisCurve());
    }
    else
    {
      throw Standard_NotImplemented("GeomHash_GeometryAppender: unsupported 2D curve");
    }
  }

  static void AppendSurface(TheSinkType& theSink, const occ::handle<Geom_Surface>& theSurface)
  {
    const occ::handle<Standard_Type>& aType = theSurface->DynamicType();
    if (aType == STANDARD_TYPE(Geom_Plane))
    {
      theSink.AppendByte(1);
      appendAxis(theSink, occ::down_cast<Geom_Plane>(theSurface)->Position());
    }
    else if (aType == STANDARD_TYPE(Geom_CylindricalSurface))
    {
      const occ::handle<Geom_CylindricalSurface> aCylinder =
        occ::down_cast<Geom_CylindricalSurface>(theSurface);
      theSink.AppendByte(2);
      appendAxis(theSink, aCylinder->Position());
      theSink.AppendDouble(aCylinder->Radius());
    }
    else if (aType == STANDARD_TYPE(Geom_ConicalSurface))
    {
      const occ::handle<Geom_ConicalSurface> aCone =
        occ::down_cast<Geom_ConicalSurface>(theSurface);
      theSink.AppendByte(3);
      appendAxis(theSink, aCone->Position());
      theSink.AppendDouble(aCone->SemiAngle());
      theSink.AppendDouble(aCone->RefRadius());
    }
    else if (aType == STANDARD_TYPE(Geom_SphericalSurface))
    {
      const occ::handle<Geom_SphericalSurface> aSphere =
        occ::down_cast<Geom_SphericalSurface>(theSurface);
      theSink.AppendByte(4);
      appendAxis(theSink, aSphere->Position());
      theSink.AppendDouble(aSphere->Radius());
    }
    else if (aType == STANDARD_TYPE(Geom_ToroidalSurface))
    {
      const occ::handle<Geom_ToroidalSurface> aTorus =
        occ::down_cast<Geom_ToroidalSurface>(theSurface);
      theSink.AppendByte(5);
      appendAxis(theSink, aTorus->Position());
      theSink.AppendDouble(aTorus->MajorRadius());
      theSink.AppendDouble(aTorus->MinorRadius());
    }
    else if (aType == STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion))
    {
      const occ::handle<Geom_SurfaceOfLinearExtrusion> anExtrusion =
        occ::down_cast<Geom_SurfaceOfLinearExtrusion>(theSurface);
      theSink.AppendByte(6);
      appendDirection(theSink, anExtrusion->Direction());
      AppendCurve(theSink, anExtrusion->BasisCurve());
    }
    else if (aType == STANDARD_TYPE(Geom_SurfaceOfRevolution))
    {
      const occ::handle<Geom_SurfaceOfRevolution> aRevolution =
        occ::down_cast<Geom_SurfaceOfRevolution>(theSurface);
      theSink.AppendByte(7);
      appendAxis(theSink, aRevolution->Axis());
      AppendCurve(theSink, aRevolution->BasisCurve());
    }
    else if (aType == STANDARD_TYPE(Geom_BezierSurface))
    {
      appendBezierSurface(theSink, occ::down_cast<Geom_BezierSurface>(theSurface));
    }
    else if (aType == STANDARD_TYPE(Geom_BSplineSurface))
    {
      appendBSplineSurface(theSink, occ::down_cast<Geom_BSplineSurface>(theSurface));
    }
    else if (aType == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
    {
      const occ::handle<Geom_RectangularTrimmedSurface> aTrimmed =
        occ::down_cast<Geom_RectangularTrimmedSurface>(theSurface);
      double aFirstU = 0.0;
      double aLastU  = 0.0;
      double aFirstV = 0.0;
      double aLastV  = 0.0;
      aTrimmed->Bounds(aFirstU, aLastU, aFirstV, aLastV);
      theSink.AppendByte(10);
      theSink.AppendDouble(aFirstU);
      theSink.AppendDouble(aLastU);
      theSink.AppendDouble(aFirstV);
      theSink.AppendDouble(aLastV);
      AppendSurface(theSink, aTrimmed->BasisSurface());
    }
    else if (aType == STANDARD_TYPE(Geom_OffsetSurface))
    {
      const occ::handle<Geom_OffsetSurface> anOffset =
        occ::down_cast<Geom_OffsetSurface>(theSurface);
      theSink.AppendByte(11);
      theSink.AppendDouble(anOffset->Offset());
      AppendSurface(theSink, anOffset->BasisSurface());
    }
    else
    {
      throw Standard_NotImplemented("GeomHash_GeometryAppender: unsupported surface");
    }
  }

private:
  static void appendDirection(TheSinkType& theSink, const gp_Dir& theDirection)
  {
    theSink.AppendDouble(theDirection.X());
    theSink.AppendDouble(theDirection.Y());
    theSink.AppendDouble(theDirection.Z());
  }

  static void appendDirection(TheSinkType& theSink, const gp_Dir2d& theDirection)
  {
    theSink.AppendDouble(theDirection.X());
    theSink.AppendDouble(theDirection.Y());
  }

  static void appendAxis(TheSinkType& theSink, const gp_Ax1& theAxis)
  {
    theSink.AppendPoint(theAxis.Location());
    appendDirection(theSink, theAxis.Direction());
  }

  static void appendAxis(TheSinkType& theSink, const gp_Ax2& theAxis)
  {
    theSink.AppendPoint(theAxis.Location());
    appendDirection(theSink, theAxis.Direction());
    appendDirection(theSink, theAxis.XDirection());
  }

  static void appendAxis(TheSinkType& theSink, const gp_Ax3& theAxis)
  {
    theSink.AppendPoint(theAxis.Location());
    appendDirection(theSink, theAxis.Direction());
    appendDirection(theSink, theAxis.XDirection());
  }

  static void appendAxis(TheSinkType& theSink, const gp_Ax2d& theAxis)
  {
    theSink.AppendPoint2d(theAxis.Location());
    appendDirection(theSink, theAxis.Direction());
  }

  static void appendAxis(TheSinkType& theSink, const gp_Ax22d& theAxis)
  {
    theSink.AppendPoint2d(theAxis.Location());
    appendDirection(theSink, theAxis.XDirection());
    appendDirection(theSink, theAxis.YDirection());
  }

  template <class TheCurveType>
  static void appendBezierCurve(TheSinkType& theSink, const occ::handle<TheCurveType>& theCurve)
  {
    const bool isRational = theCurve->IsRational();
    const int  aNbPoles   = theCurve->NbPoles();
    theSink.AppendByte(6);
    theSink.AppendBool(isRational);
    theSink.AppendInt(aNbPoles);
    for (int anIndex = 1; anIndex <= aNbPoles; ++anIndex)
    {
      appendPoint(theSink, theCurve->Pole(anIndex));
      if (isRational)
      {
        theSink.AppendDouble(theCurve->Weight(anIndex));
      }
    }
  }

  template <class TheCurveType>
  static void appendBSplineCurve(TheSinkType& theSink, const occ::handle<TheCurveType>& theCurve)
  {
    const bool isRational = theCurve->IsRational();
    const int  aNbPoles   = theCurve->NbPoles();
    const int  aNbKnots   = theCurve->NbKnots();
    theSink.AppendByte(7);
    theSink.AppendBool(isRational);
    theSink.AppendBool(theCurve->IsPeriodic());
    theSink.AppendInt(theCurve->Degree());
    theSink.AppendInt(aNbPoles);
    theSink.AppendInt(aNbKnots);
    for (int anIndex = 1; anIndex <= aNbPoles; ++anIndex)
    {
      appendPoint(theSink, theCurve->Pole(anIndex));
      if (isRational)
      {
        theSink.AppendDouble(theCurve->Weight(anIndex));
      }
    }
    for (int anIndex = 1; anIndex <= aNbKnots; ++anIndex)
    {
      theSink.AppendDouble(theCurve->Knot(anIndex));
      theSink.AppendInt(theCurve->Multiplicity(anIndex));
    }
  }

  static void appendBezierSurface(TheSinkType&                           theSink,
                                  const occ::handle<Geom_BezierSurface>& theSurface)
  {
    const bool hasWeights = theSurface->IsURational() || theSurface->IsVRational();
    const int  aNbUPoles  = theSurface->NbUPoles();
    const int  aNbVPoles  = theSurface->NbVPoles();
    theSink.AppendByte(8);
    theSink.AppendBool(theSurface->IsURational());
    theSink.AppendBool(theSurface->IsVRational());
    theSink.AppendInt(aNbUPoles);
    theSink.AppendInt(aNbVPoles);
    for (int aUIndex = 1; aUIndex <= aNbUPoles; ++aUIndex)
    {
      for (int aVIndex = 1; aVIndex <= aNbVPoles; ++aVIndex)
      {
        theSink.AppendPoint(theSurface->Pole(aUIndex, aVIndex));
        if (hasWeights)
        {
          theSink.AppendDouble(theSurface->Weight(aUIndex, aVIndex));
        }
      }
    }
  }

  static void appendBSplineSurface(TheSinkType&                            theSink,
                                   const occ::handle<Geom_BSplineSurface>& theSurface)
  {
    const bool hasWeights = theSurface->IsURational() || theSurface->IsVRational();
    const int  aNbUPoles  = theSurface->NbUPoles();
    const int  aNbVPoles  = theSurface->NbVPoles();
    const int  aNbUKnots  = theSurface->NbUKnots();
    const int  aNbVKnots  = theSurface->NbVKnots();
    theSink.AppendByte(9);
    theSink.AppendBool(theSurface->IsURational());
    theSink.AppendBool(theSurface->IsVRational());
    theSink.AppendBool(theSurface->IsUPeriodic());
    theSink.AppendBool(theSurface->IsVPeriodic());
    theSink.AppendInt(theSurface->UDegree());
    theSink.AppendInt(theSurface->VDegree());
    theSink.AppendInt(aNbUPoles);
    theSink.AppendInt(aNbVPoles);
    theSink.AppendInt(aNbUKnots);
    theSink.AppendInt(aNbVKnots);
    for (int aUIndex = 1; aUIndex <= aNbUPoles; ++aUIndex)
    {
      for (int aVIndex = 1; aVIndex <= aNbVPoles; ++aVIndex)
      {
        theSink.AppendPoint(theSurface->Pole(aUIndex, aVIndex));
        if (hasWeights)
        {
          theSink.AppendDouble(theSurface->Weight(aUIndex, aVIndex));
        }
      }
    }
    for (int anIndex = 1; anIndex <= aNbUKnots; ++anIndex)
    {
      theSink.AppendDouble(theSurface->UKnot(anIndex));
      theSink.AppendInt(theSurface->UMultiplicity(anIndex));
    }
    for (int anIndex = 1; anIndex <= aNbVKnots; ++anIndex)
    {
      theSink.AppendDouble(theSurface->VKnot(anIndex));
      theSink.AppendInt(theSurface->VMultiplicity(anIndex));
    }
  }

  static void appendPoint(TheSinkType& theSink, const gp_Pnt& thePoint)
  {
    theSink.AppendPoint(thePoint);
  }

  static void appendPoint(TheSinkType& theSink, const gp_Pnt2d& thePoint)
  {
    theSink.AppendPoint2d(thePoint);
  }
};

#endif // _GeomHash_GeometryAppender_HeaderFile
