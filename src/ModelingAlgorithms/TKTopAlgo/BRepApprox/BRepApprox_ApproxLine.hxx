// Created on: 1995-07-20
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _BRepApprox_ApproxLine_HeaderFile
#define _BRepApprox_ApproxLine_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
class Geom_BSplineCurve;
class Geom2d_BSplineCurve;
class IntSurf_LineOn2S;
class IntSurf_PntOn2S;

class BRepApprox_ApproxLine : public Standard_Transient
{

public:
  Standard_EXPORT BRepApprox_ApproxLine(const occ::handle<Geom_BSplineCurve>&   CurveXYZ,
                                        const occ::handle<Geom2d_BSplineCurve>& CurveUV1,
                                        const occ::handle<Geom2d_BSplineCurve>& CurveUV2);

  //! theTang variable has been entered only for compatibility with
  //! the alias IntPatch_WLine. They are not used in this class.
  Standard_EXPORT BRepApprox_ApproxLine(const occ::handle<IntSurf_LineOn2S>& lin,
                                        const bool          theTang = false);

  Standard_EXPORT int NbPnts() const;

  Standard_EXPORT IntSurf_PntOn2S Point(const int Index);

  DEFINE_STANDARD_RTTIEXT(BRepApprox_ApproxLine, Standard_Transient)

private:
  occ::handle<Geom_BSplineCurve>   myCurveXYZ;
  occ::handle<Geom2d_BSplineCurve> myCurveUV1;
  occ::handle<Geom2d_BSplineCurve> myCurveUV2;
  occ::handle<IntSurf_LineOn2S>    myLineOn2S;
};

#endif // _BRepApprox_ApproxLine_HeaderFile
