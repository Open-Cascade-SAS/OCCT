// Created on: 1996-09-09
// Created by: Herve LOUESSARD
// Copyright (c) 1996-1999 Matra Datavision
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

#include <LocalAnalysis.hxx>
#include <LocalAnalysis_CurveContinuity.hxx>
#include <LocalAnalysis_SurfaceContinuity.hxx>

/*********************************************************************************/
/*********************************************************************************/
void LocalAnalysis::Dump(const LocalAnalysis_SurfaceContinuity& surfconti, Standard_OStream& o)
{
  if (!surfconti.IsDone())
  {
    o << "Problem in the computation " << std::endl;
    if (surfconti.StatusError() == LocalAnalysis_NullFirstDerivative)
      o << "one of the first derivatives is null" << std::endl;
    else if (surfconti.StatusError() == LocalAnalysis_NullSecondDerivative)
      o << "one of the  second derivatives is null" << std::endl;
    else if (surfconti.StatusError() == LocalAnalysis_NormalNotDefined)
      o << "one (or both) normal is undefined" << std::endl;
    else if (surfconti.StatusError() == LocalAnalysis_CurvatureNotDefined)
      o << "one of the mean curvatures is undefined" << std::endl;
  }
  else
    switch (surfconti.ContinuityStatus())
    {
      case GeomAbs_C0: {
        if (surfconti.IsC0())
          o << " Continuity Status : C0 " << std::endl;
        else
          o << " Continuity Status : No C0 " << std::endl;
        o << " C0Value = " << surfconti.C0Value() << std::endl;
      }
      break;
      case GeomAbs_C1: {
        if (surfconti.IsC1())
          o << " Continuity Status : C1 " << std::endl;
        else
        {
          if (surfconti.IsC0())
            o << " Continuity Status : C0 " << std::endl;
          else
            o << " Continuity Status : NoC0 " << std::endl;
        }
        o << " C0Value = " << surfconti.C0Value() << std::endl;
        o << " C1UAngle = " << surfconti.C1UAngle() << std::endl;
        o << " C1URatio = " << surfconti.C1URatio() << std::endl;
        o << " C1VAngle = " << surfconti.C1VAngle() << std::endl;
        o << " C1VRatio = " << surfconti.C1VRatio() << std::endl;
      }
      break;
      case GeomAbs_C2: {
        if (surfconti.IsC2())
          o << " Continuity Status : C2 " << std::endl;
        else
        {
          if (surfconti.IsC1())
            o << " Continuity Status : C1 " << std::endl;
          else
          {
            if (surfconti.IsC0())
              o << " Continuity Status : C0 " << std::endl;
            else
              o << " Continuity Status : NoC0 " << std::endl;
          }
        }

        o << " C0Value = " << surfconti.C0Value() << std::endl;
        o << " C1UAngle = " << surfconti.C1UAngle() << std::endl;
        o << " C1VAngle = " << surfconti.C1VAngle() << std::endl;
        o << " C2UAngle = " << surfconti.C2UAngle() << std::endl;
        o << " C2VAngle = " << surfconti.C2VAngle() << std::endl;
        o << " C1URatio = " << surfconti.C1URatio() << std::endl;
        o << " C1VRatio = " << surfconti.C1VRatio() << std::endl;
        o << " C2URatio = " << surfconti.C2URatio() << std::endl;
        o << " C2VRatio = " << surfconti.C2VRatio() << std::endl;
      }
      break;
      case GeomAbs_G1: {
        if (surfconti.IsG1())
          o << " Continuity Status : G1 " << std::endl;
        else
        {
          if (surfconti.IsC0())
            o << " Continuity Status : G0 " << std::endl;
          else
            o << " Continuity Status : NoG0 " << std::endl;
        }
        o << " G0Value = " << surfconti.C0Value() << std::endl;
        o << " G1Angle = " << surfconti.G1Angle() << std::endl << std::endl;
      }
      break;
      case GeomAbs_G2: {
        if (surfconti.IsG2())
          o << " Continuity Status : G2 " << std::endl;
        else
        {
          if (surfconti.IsG1())
            o << " Continuity Status : G1 " << std::endl;
          else
          {
            if (surfconti.IsC0())
              o << " Continuity Status : G0 " << std::endl;
            else
              o << " Continuity Status : NoG0 " << std::endl;
          }
        }
        o << " G0Value = " << surfconti.C0Value() << std::endl;
        o << " G1Value = " << surfconti.G1Angle() << std::endl;
        o << " G2CurvatureGap = " << surfconti.G2CurvatureGap() << std::endl;
      }
      break;

      default: {
      }
    }
}

/*********************************************************************************/

void LocalAnalysis::Dump(const LocalAnalysis_CurveContinuity& curvconti, Standard_OStream& o)
{
  if (!curvconti.IsDone())
  {
    o << "Problem in the computation " << std::endl;
    if (curvconti.StatusError() == LocalAnalysis_NullFirstDerivative)
      o << "one (or both) first derivative is null" << std::endl;
    else if (curvconti.StatusError() == LocalAnalysis_NullSecondDerivative)
      o << "one (or both) second derivative is null" << std::endl;
    else if (curvconti.StatusError() == LocalAnalysis_TangentNotDefined)
      o << "one (or both) tangent is undefined " << std::endl;
    else if (curvconti.StatusError() == LocalAnalysis_NormalNotDefined)
      o << "one (or both) normal is undefined" << std::endl;
  }
  else
    switch (curvconti.ContinuityStatus())
    {
      case GeomAbs_C0: {
        if (curvconti.IsC0())
          o << " Continuity Status : C0 " << std::endl;
        else
          o << " Continuity Status : No C0 " << std::endl;
        o << " C0Value = " << curvconti.C0Value() << std::endl;
      }
      break;
      case GeomAbs_C1: {
        if (curvconti.IsC1())
          o << " Continuity Status : C1 " << std::endl;
        else
        {
          if (curvconti.IsC0())
            o << " Continuity Status : C0 " << std::endl;
          else
            o << " Continuity Status : NoC0 " << std::endl;
        }
        o << " C0Value = " << curvconti.C0Value() << std::endl;
        o << " C1Angle = " << curvconti.C1Angle() << std::endl;
        o << " C1Ratio = " << curvconti.C1Ratio() << std::endl;
      }
      break;
      case GeomAbs_C2: {
        if (curvconti.IsC2())
          o << " Continuity Status : C2 " << std::endl;
        else
        {
          if (curvconti.IsC1())
            o << " Continuity Status : C1 " << std::endl;
          else
          {
            if (curvconti.IsC0())
              o << " Continuity Status : C0 " << std::endl;
            else
              o << " Continuity Status : NoC0 " << std::endl;
          }
        }
        o << " C0Value = " << curvconti.C0Value() << std::endl;
        o << " C1Angle = " << curvconti.C1Angle() << std::endl;
        o << " C2Angle = " << curvconti.C2Angle() << std::endl;
        o << " C1Ratio = " << curvconti.C1Ratio() << std::endl;
        o << " C2Ratio = " << curvconti.C2Ratio() << std::endl;
      }
      break;

      case GeomAbs_G1: {
        if (curvconti.IsG1())
          o << " Continuity Status : G1 " << std::endl;
        else
        {
          if (curvconti.IsC0())
            o << " Continuity Status : G0 " << std::endl;
          else
            o << " Continuity Status : NoG0 " << std::endl;
        }
        o << " G0Value = " << curvconti.C0Value() << std::endl;
        o << " G1Angle = " << curvconti.G1Angle() << std::endl;
      }
      break;
      case GeomAbs_G2: {
        if (curvconti.IsG2())
          o << " Continuity Status : G2 " << std::endl;
        else
        {
          if (curvconti.IsG1())
            o << " Continuity Status : G1 " << std::endl;
          else
          {
            if (curvconti.IsC0())
              o << " Continuity Status : G0 " << std::endl;
            else
              o << " Continuity Status : NoG0 " << std::endl;
          }
        }
        o << " G0Value = " << curvconti.C0Value() << std::endl;
        o << " G1Angle = " << curvconti.G1Angle() << std::endl;
        o << " G2Angle = " << curvconti.G2Angle() << std::endl;
        o << " Relative curvature variation = " << curvconti.G2CurvatureVariation() << std::endl;
      }
      break;

      default: {
      }
    }
}

/*********************************************************************************/
/*********************************************************************************/
