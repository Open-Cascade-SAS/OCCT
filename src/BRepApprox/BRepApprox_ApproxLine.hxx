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
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class Geom_BSplineCurve;
class Geom2d_BSplineCurve;
class IntSurf_LineOn2S;
class IntSurf_PntOn2S;


class BRepApprox_ApproxLine;
DEFINE_STANDARD_HANDLE(BRepApprox_ApproxLine, Standard_Transient)


class BRepApprox_ApproxLine : public Standard_Transient
{

public:

  
  Standard_EXPORT BRepApprox_ApproxLine(const Handle(Geom_BSplineCurve)& CurveXYZ, const Handle(Geom2d_BSplineCurve)& CurveUV1, const Handle(Geom2d_BSplineCurve)& CurveUV2);
  
  //! theTang variable has been entered only for compatibility with 
  //! the alias IntPatch_WLine. They are not used in this class.
  Standard_EXPORT BRepApprox_ApproxLine(const Handle(IntSurf_LineOn2S)& lin, const Standard_Boolean theTang = Standard_False);
  
  Standard_EXPORT Standard_Integer NbPnts() const;
  
  Standard_EXPORT IntSurf_PntOn2S Point (const Standard_Integer Index);




  DEFINE_STANDARD_RTTIEXT(BRepApprox_ApproxLine,Standard_Transient)

protected:




private:


  Handle(Geom_BSplineCurve) myCurveXYZ;
  Handle(Geom2d_BSplineCurve) myCurveUV1;
  Handle(Geom2d_BSplineCurve) myCurveUV2;
  Handle(IntSurf_LineOn2S) myLineOn2S;


};







#endif // _BRepApprox_ApproxLine_HeaderFile
