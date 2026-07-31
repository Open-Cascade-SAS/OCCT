// Created on: 1993-11-18
// Created by: Isabelle GRIGNON
// Copyright (c) 1993-1999 Matra Datavision
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

//=================================================================================================

inline void ChFiDS_FaceInterference::SetInterference(const int                        LineIndex,
                                                     const TopAbs_Orientation         Trans,
                                                     const occ::handle<Geom2d_Curve>& PCurv1,
                                                     const occ::handle<Geom2d_Curve>& PCurv2)
{
  lineindex      = LineIndex;
  LineTransition = Trans;
  pCurveOnFace   = PCurv1;
  pCurveOnSurf   = PCurv2;
}

//=================================================================================================

inline void ChFiDS_FaceInterference::SetLineIndex(const int I)
{
  lineindex = I;
}

//=================================================================================================

inline void ChFiDS_FaceInterference::SetFirstParameter(const double U1)
{
  firstParam = U1;
}

//=================================================================================================

inline void ChFiDS_FaceInterference::SetLastParameter(const double U1)
{
  lastParam = U1;
}

//=================================================================================================

inline int ChFiDS_FaceInterference::LineIndex() const
{
  return lineindex;
}

//=================================================================================================

inline TopAbs_Orientation ChFiDS_FaceInterference::Transition() const
{
  return LineTransition;
}

//=================================================================================================

inline const occ::handle<Geom2d_Curve>& ChFiDS_FaceInterference::PCurveOnFace() const
{
  return pCurveOnFace;
}

//=================================================================================================

inline const occ::handle<Geom2d_Curve>& ChFiDS_FaceInterference::PCurveOnSurf() const
{
  return pCurveOnSurf;
}

//=================================================================================================

inline occ::handle<Geom2d_Curve>& ChFiDS_FaceInterference::ChangePCurveOnFace()
{
  return pCurveOnFace;
}

//=================================================================================================

inline occ::handle<Geom2d_Curve>& ChFiDS_FaceInterference::ChangePCurveOnSurf()
{
  return pCurveOnSurf;
}

//=================================================================================================

inline double ChFiDS_FaceInterference::FirstParameter() const
{
  return firstParam;
}

//=================================================================================================

inline double ChFiDS_FaceInterference::LastParameter() const
{
  return lastParam;
}
