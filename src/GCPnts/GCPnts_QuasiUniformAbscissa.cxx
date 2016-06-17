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

//#include <GCPnts_QuasiUniformAbscissa.ixx>

#include <StdFail_NotDone.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_ConstructionError.hxx>
#include <GCPnts_QuasiUniformAbscissa.hxx>
#include <GCPnts_UniformAbscissa.hxx>
#include <Adaptor3d_Curve.hxx>
#include <Adaptor2d_Curve2d.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <gp_Pnt2d.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <gp_Pnt.hxx>

#ifdef OCCT_DEBUG
//#include <DrawTrSurf.hxx>

//static Standard_Integer compteur = 0;
#endif

//=======================================================================
//function : GCPnts_QuasiUniformAbscissa
//purpose  : 
//=======================================================================

GCPnts_QuasiUniformAbscissa::GCPnts_QuasiUniformAbscissa ()
     :myDone(Standard_False),
	 myNbPoints(0)
{
} 

#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>

#define TheCurve                 Adaptor3d_Curve
#define Handle_TheBezierCurve   Handle(Geom_BezierCurve)
#define Handle_TheBSplineCurve  Handle(Geom_BSplineCurve)
#define TheArray1OfPnt          TColgp_Array1OfPnt
#define ThePnt                  gp_Pnt

#include "GCPnts_QuasiUniformAbscissa.pxx"

#undef TheCurve
#undef Handle_TheBezierCurve
#undef Handle_TheBSplineCurve
#undef TheArray1OfPnt     
#undef ThePnt

#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>

#define TheCurve                 Adaptor2d_Curve2d
#define Handle_TheBezierCurve   Handle(Geom2d_BezierCurve)
#define Handle_TheBSplineCurve  Handle(Geom2d_BSplineCurve)
#define TheArray1OfPnt          TColgp_Array1OfPnt2d
#define ThePnt                  gp_Pnt2d

#include "GCPnts_QuasiUniformAbscissa.pxx"



