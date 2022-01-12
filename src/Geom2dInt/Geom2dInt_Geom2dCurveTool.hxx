// Created on: 1992-10-22
// Created by: Laurent BUCHARD
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _Geom2dInt_Geom2dCurveTool_HeaderFile
#define _Geom2dInt_Geom2dCurveTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <GeomAbs_CurveType.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Hypr2d.hxx>
#include <Standard_Integer.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <TColStd_Array1OfReal.hxx>
class Adaptor2d_Curve2d;
class gp_Pnt2d;
class gp_Vec2d;


//! This class provides a Geom2dCurveTool as < Geom2dCurveTool from IntCurve >
//! from a Tool as < Geom2dCurveTool from Adaptor3d > .
class Geom2dInt_Geom2dCurveTool 
{
public:

  DEFINE_STANDARD_ALLOC

  
    static GeomAbs_CurveType GetType (const Adaptor2d_Curve2d& C);
  
    static Standard_Boolean IsComposite (const Adaptor2d_Curve2d& C);
  
  //! Returns the Lin2d from gp corresponding to the curve C.
  //! This method is called only when TheType returns
  //! GeomAbs_Line.
    static gp_Lin2d Line (const Adaptor2d_Curve2d& C);
  
  //! Returns the Circ2d from gp corresponding to the curve C.
  //! This method is called only when TheType returns
  //! GeomAbs_Circle.
    static gp_Circ2d Circle (const Adaptor2d_Curve2d& C);
  
  //! Returns the Elips2d from gp corresponding to the curve C.
  //! This method is called only when TheType returns
  //! GeomAbs_Ellipse.
    static gp_Elips2d Ellipse (const Adaptor2d_Curve2d& C);
  
  //! Returns the Parab2d from gp corresponding to the curve C.
  //! This method is called only when TheType returns
  //! GeomAbs_Parabola.
    static gp_Parab2d Parabola (const Adaptor2d_Curve2d& C);
  
  //! Returns the Hypr2d from gp corresponding to the curve C.
  //! This method is called only when TheType returns
  //! GeomAbs_Hyperbola.
    static gp_Hypr2d Hyperbola (const Adaptor2d_Curve2d& C);
  
    static Standard_Real EpsX (const Adaptor2d_Curve2d& C);
  
    static Standard_Real EpsX (const Adaptor2d_Curve2d& C, const Standard_Real Eps_XYZ);
  
  Standard_EXPORT static Standard_Integer NbSamples (const Adaptor2d_Curve2d& C);
  
  Standard_EXPORT static Standard_Integer NbSamples (const Adaptor2d_Curve2d& C, const Standard_Real U0, const Standard_Real U1);
  
    static Standard_Real FirstParameter (const Adaptor2d_Curve2d& C);
  
    static Standard_Real LastParameter (const Adaptor2d_Curve2d& C);
  
    static gp_Pnt2d Value (const Adaptor2d_Curve2d& C, const Standard_Real X);
  
    static void D0 (const Adaptor2d_Curve2d& C, const Standard_Real U, gp_Pnt2d& P);
  
    static void D1 (const Adaptor2d_Curve2d& C, const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& T);
  
    static void D2 (const Adaptor2d_Curve2d& C, const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& T, gp_Vec2d& N);
  
    static void D3 (const Adaptor2d_Curve2d& C, const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& T, gp_Vec2d& N, gp_Vec2d& V);
  
    static gp_Vec2d DN (const Adaptor2d_Curve2d& C, const Standard_Real U, const Standard_Integer N);
  
  //! output the number of interval of continuity C2 of
  //! the curve
    static Standard_Integer NbIntervals (const Adaptor2d_Curve2d& C);
  
  //! compute Tab.
    static void Intervals (const Adaptor2d_Curve2d& C, TColStd_Array1OfReal& Tab);
  
  //! output the bounds of interval of index <Index>
  //! used if Type == Composite.
    static void GetInterval (const Adaptor2d_Curve2d& C, const Standard_Integer Index, const TColStd_Array1OfReal& Tab, Standard_Real& U1, Standard_Real& U2);
  
    static Standard_Integer Degree (const Adaptor2d_Curve2d& C);




protected:





private:





};


#include <Geom2dInt_Geom2dCurveTool.lxx>





#endif // _Geom2dInt_Geom2dCurveTool_HeaderFile
