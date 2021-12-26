// Created on: 1993-12-02
// Created by: Jacques GOUSSARD
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

#ifndef _Blend_Point_HeaderFile
#define _Blend_Point_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_Boolean.hxx>
#include <gp_Vec2d.hxx>
class gp_Vec2d;



class Blend_Point 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Blend_Point();
  
  //! Creates a point on 2 surfaces, with tangents.
  Standard_EXPORT Blend_Point(const gp_Pnt& Pt1, const gp_Pnt& Pt2, const Standard_Real Param, const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2, const gp_Vec& Tg1, const gp_Vec& Tg2, const gp_Vec2d& Tg12d, const gp_Vec2d& Tg22d);
  
  //! Creates a point on 2 surfaces, without tangents.
  Standard_EXPORT Blend_Point(const gp_Pnt& Pt1, const gp_Pnt& Pt2, const Standard_Real Param, const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2);
  
  //! Creates a point on a surface and a curve, with tangents.
  Standard_EXPORT Blend_Point(const gp_Pnt& Pts, const gp_Pnt& Ptc, const Standard_Real Param, const Standard_Real U, const Standard_Real V, const Standard_Real W, const gp_Vec& Tgs, const gp_Vec& Tgc, const gp_Vec2d& Tg2d);
  
  //! Creates a point on a surface and a curve, without tangents.
  Standard_EXPORT Blend_Point(const gp_Pnt& Pts, const gp_Pnt& Ptc, const Standard_Real Param, const Standard_Real U, const Standard_Real V, const Standard_Real W);
  
  //! Creates a point on a surface and a curve on surface,
  //! with tangents.
  Standard_EXPORT Blend_Point(const gp_Pnt& Pt1, const gp_Pnt& Pt2, const Standard_Real Param, const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2, const Standard_Real PC, const gp_Vec& Tg1, const gp_Vec& Tg2, const gp_Vec2d& Tg12d, const gp_Vec2d& Tg22d);
  
  //! Creates a point on a surface and a curve on surface,
  //! without tangents.
  Standard_EXPORT Blend_Point(const gp_Pnt& Pt1, const gp_Pnt& Pt2, const Standard_Real Param, const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2, const Standard_Real PC);
  
  //! Creates a point on two curves on surfaces, with tangents.
  Standard_EXPORT Blend_Point(const gp_Pnt& Pt1, const gp_Pnt& Pt2, const Standard_Real Param, const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2, const Standard_Real PC1, const Standard_Real PC2, const gp_Vec& Tg1, const gp_Vec& Tg2, const gp_Vec2d& Tg12d, const gp_Vec2d& Tg22d);
  
  //! Creates a point on two curves on surfaces, with tangents.
  Standard_EXPORT Blend_Point(const gp_Pnt& Pt1, const gp_Pnt& Pt2, const Standard_Real Param, const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2, const Standard_Real PC1, const Standard_Real PC2);
  
  //! Set the values for a point on 2 surfaces, with tangents.
  Standard_EXPORT void SetValue (const gp_Pnt& Pt1, const gp_Pnt& Pt2, const Standard_Real Param, const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2, const gp_Vec& Tg1, const gp_Vec& Tg2, const gp_Vec2d& Tg12d, const gp_Vec2d& Tg22d);
  
  //! Set the values for a point on 2 surfaces, without tangents.
  Standard_EXPORT void SetValue (const gp_Pnt& Pt1, const gp_Pnt& Pt2, const Standard_Real Param, const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2);
  
  //! Set the values for a point on a surface and a curve,
  //! with tangents.
  Standard_EXPORT void SetValue (const gp_Pnt& Pts, const gp_Pnt& Ptc, const Standard_Real Param, const Standard_Real U, const Standard_Real V, const Standard_Real W, const gp_Vec& Tgs, const gp_Vec& Tgc, const gp_Vec2d& Tg2d);
  
  //! Set the values for a point on a surface and a curve,
  //! without tangents.
  Standard_EXPORT void SetValue (const gp_Pnt& Pts, const gp_Pnt& Ptc, const Standard_Real Param, const Standard_Real U, const Standard_Real V, const Standard_Real W);
  
  //! Creates a point on a surface and a curve on surface,
  //! with tangents.
  Standard_EXPORT void SetValue (const gp_Pnt& Pt1, const gp_Pnt& Pt2, const Standard_Real Param, const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2, const Standard_Real PC, const gp_Vec& Tg1, const gp_Vec& Tg2, const gp_Vec2d& Tg12d, const gp_Vec2d& Tg22d);
  
  //! Creates a point on a surface and a curve on surface,
  //! without tangents.
  Standard_EXPORT void SetValue (const gp_Pnt& Pt1, const gp_Pnt& Pt2, const Standard_Real Param, const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2, const Standard_Real PC);
  
  //! Creates a point on two curves on surfaces, with tangents.
  Standard_EXPORT void SetValue (const gp_Pnt& Pt1, const gp_Pnt& Pt2, const Standard_Real Param, const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2, const Standard_Real PC1, const Standard_Real PC2, const gp_Vec& Tg1, const gp_Vec& Tg2, const gp_Vec2d& Tg12d, const gp_Vec2d& Tg22d);
  
  //! Creates a point on two curves on surfaces, without tangents.
  Standard_EXPORT void SetValue (const gp_Pnt& Pt1, const gp_Pnt& Pt2, const Standard_Real Param, const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2, const Standard_Real PC1, const Standard_Real PC2);
  
  //! Creates a point on two curves.
  Standard_EXPORT void SetValue (const gp_Pnt& Pt1, const gp_Pnt& Pt2, const Standard_Real Param, const Standard_Real PC1, const Standard_Real PC2);
  
  //! Changes parameter on existing point
    void SetParameter (const Standard_Real Param);
  
    Standard_Real Parameter() const;
  
  //! Returns Standard_True if it was not possible to compute
  //! the tangent vectors at PointOnS1 and/or PointOnS2.
    Standard_Boolean IsTangencyPoint() const;
  
    const gp_Pnt& PointOnS1() const;
  
    const gp_Pnt& PointOnS2() const;
  
    void ParametersOnS1 (Standard_Real& U, Standard_Real& V) const;
  
    void ParametersOnS2 (Standard_Real& U, Standard_Real& V) const;
  
    const gp_Vec& TangentOnS1() const;
  
    const gp_Vec& TangentOnS2() const;
  
    gp_Vec2d Tangent2dOnS1() const;
  
    gp_Vec2d Tangent2dOnS2() const;
  
    const gp_Pnt& PointOnS() const;
  
    const gp_Pnt& PointOnC() const;
  
    void ParametersOnS (Standard_Real& U, Standard_Real& V) const;
  
    Standard_Real ParameterOnC() const;
  
    const gp_Vec& TangentOnS() const;
  
    const gp_Vec& TangentOnC() const;
  
    gp_Vec2d Tangent2d() const;
  
    const gp_Pnt& PointOnC1() const;
  
    const gp_Pnt& PointOnC2() const;
  
    Standard_Real ParameterOnC1() const;
  
    Standard_Real ParameterOnC2() const;
  
    const gp_Vec& TangentOnC1() const;
  
    const gp_Vec& TangentOnC2() const;




protected:





private:



  gp_Pnt pt1;
  gp_Pnt pt2;
  gp_Vec tg1;
  gp_Vec tg2;
  Standard_Real prm;
  Standard_Real u1;
  Standard_Real v1;
  Standard_Real u2;
  Standard_Real v2;
  Standard_Real pc1;
  Standard_Real pc2;
  Standard_Real utg12d;
  Standard_Real vtg12d;
  Standard_Real utg22d;
  Standard_Real vtg22d;
  Standard_Boolean hass1;
  Standard_Boolean hass2;
  Standard_Boolean hasc1;
  Standard_Boolean hasc2;
  Standard_Boolean istgt;


};


#include <Blend_Point.lxx>





#endif // _Blend_Point_HeaderFile
