// Created on: 1994-03-24
// Created by: model
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _Adaptor3d_TopolTool_HeaderFile
#define _Adaptor3d_TopolTool_HeaderFile

#include <Adaptor2d_Line2d.hxx>
#include <Adaptor3d_HVertex.hxx>
#include <Adaptor3d_Surface.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TopAbs_State.hxx>
#include <TopAbs_Orientation.hxx>

class Adaptor3d_HVertex;

DEFINE_STANDARD_HANDLE(Adaptor3d_TopolTool, Standard_Transient)

//! This class provides a default topological tool,
//! based on the Umin,Vmin,Umax,Vmax of an HSurface from Adaptor3d.
//! All methods and fields may be redefined when inheriting from this class.
//! This class is used to instantiate algorithms as Intersection, outlines,...
class Adaptor3d_TopolTool : public Standard_Transient
{

public:
  Standard_EXPORT Adaptor3d_TopolTool();

  Standard_EXPORT Adaptor3d_TopolTool(const Handle(Adaptor3d_Surface)& Surface);

  Standard_EXPORT virtual void Initialize();

  Standard_EXPORT virtual void Initialize(const Handle(Adaptor3d_Surface)& S);

  Standard_EXPORT virtual void Initialize(const Handle(Adaptor2d_Curve2d)& Curve);

  Standard_EXPORT virtual void Init();

  Standard_EXPORT virtual Standard_Boolean More();

  Standard_EXPORT virtual Handle(Adaptor2d_Curve2d) Value();

  Standard_EXPORT virtual void Next();

  Standard_EXPORT virtual void InitVertexIterator();

  Standard_EXPORT virtual Standard_Boolean MoreVertex();

  Standard_EXPORT virtual Handle(Adaptor3d_HVertex) Vertex();

  Standard_EXPORT virtual void NextVertex();

  Standard_EXPORT virtual TopAbs_State Classify(
    const gp_Pnt2d&        P,
    const Standard_Real    Tol,
    const Standard_Boolean ReacdreOnPeriodic = Standard_True);

  Standard_EXPORT virtual Standard_Boolean IsThePointOn(
    const gp_Pnt2d&        P,
    const Standard_Real    Tol,
    const Standard_Boolean ReacdreOnPeriodic = Standard_True);

  //! If the function returns the orientation of the arc.
  //! If the orientation is FORWARD or REVERSED, the arc is
  //! a "real" limit of the surface.
  //! If the orientation is INTERNAL or EXTERNAL, the arc is
  //! considered as an arc on the surface.
  Standard_EXPORT virtual TopAbs_Orientation Orientation(const Handle(Adaptor2d_Curve2d)& C);

  //! Returns the orientation of the vertex V.
  //! The vertex has been found with an exploration on
  //! a given arc. The orientation is the orientation
  //! of the vertex on this arc.
  Standard_EXPORT virtual TopAbs_Orientation Orientation(const Handle(Adaptor3d_HVertex)& V);

  //! Returns True if the vertices V1 and V2 are identical.
  //! This method does not take the orientation of the
  //! vertices in account.
  Standard_EXPORT virtual Standard_Boolean Identical(const Handle(Adaptor3d_HVertex)& V1,
                                                     const Handle(Adaptor3d_HVertex)& V2);

  //! answers if arcs and vertices may have 3d representations,
  //! so that we could use Tol3d and Pnt methods.
  Standard_EXPORT virtual Standard_Boolean Has3d() const;

  //! returns 3d tolerance of the arc C
  Standard_EXPORT virtual Standard_Real Tol3d(const Handle(Adaptor2d_Curve2d)& C) const;

  //! returns 3d tolerance of the vertex V
  Standard_EXPORT virtual Standard_Real Tol3d(const Handle(Adaptor3d_HVertex)& V) const;

  //! returns 3d point of the vertex V
  Standard_EXPORT virtual gp_Pnt Pnt(const Handle(Adaptor3d_HVertex)& V) const;

  Standard_EXPORT virtual void ComputeSamplePoints();

  //! compute the sample-points for the intersections algorithms
  Standard_EXPORT virtual Standard_Integer NbSamplesU();

  //! compute the sample-points for the intersections algorithms
  Standard_EXPORT virtual Standard_Integer NbSamplesV();

  //! compute the sample-points for the intersections algorithms
  Standard_EXPORT virtual Standard_Integer NbSamples();

  //! return the set of U parameters on the surface
  //! obtained by the method SamplePnts
  Standard_EXPORT void UParameters(TColStd_Array1OfReal& theArray) const;

  //! return the set of V parameters on the surface
  //! obtained by the method SamplePnts
  Standard_EXPORT void VParameters(TColStd_Array1OfReal& theArray) const;

  Standard_EXPORT virtual void SamplePoint(const Standard_Integer Index,
                                           gp_Pnt2d&              P2d,
                                           gp_Pnt&                P3d);

  Standard_EXPORT virtual Standard_Boolean DomainIsInfinite();

  Standard_EXPORT virtual Standard_Address Edge() const;

  //! Compute the sample-points for the intersections algorithms by adaptive algorithm for BSpline
  //! surfaces. For other surfaces algorithm is the same as in method ComputeSamplePoints(), but
  //! only fill arrays of U and V sample parameters;
  //! @param[in] theDefl   a required deflection
  //! @param[in] theNUmin  minimal nb points for U
  //! @param[in] theNVmin  minimal nb points for V
  Standard_EXPORT virtual void SamplePnts(const Standard_Real    theDefl,
                                          const Standard_Integer theNUmin,
                                          const Standard_Integer theNVmin);

  //! Compute the sample-points for the intersections algorithms
  //! by adaptive algorithm for BSpline surfaces - is used in SamplePnts
  //! @param[in] theDefl   required deflection
  //! @param[in] theNUmin  minimal nb points for U
  //! @param[in] theNVmin  minimal nb points for V
  Standard_EXPORT virtual void BSplSamplePnts(const Standard_Real    theDefl,
                                              const Standard_Integer theNUmin,
                                              const Standard_Integer theNVmin);

  //! Returns true if provide uniform sampling of points.
  Standard_EXPORT virtual Standard_Boolean IsUniformSampling() const;

  //! Computes the cone's apex parameters.
  //! @param[in] theC conical surface
  //! @param[in] theU U parameter of cone's apex
  //! @param[in] theV V parameter of cone's apex
  Standard_EXPORT static void GetConeApexParam(const gp_Cone& theC,
                                               Standard_Real& theU,
                                               Standard_Real& theV);

  DEFINE_STANDARD_RTTIEXT(Adaptor3d_TopolTool, Standard_Transient)

protected:
  Handle(Adaptor3d_Surface)     myS;
  Standard_Integer              myNbSamplesU;
  Standard_Integer              myNbSamplesV;
  Handle(TColStd_HArray1OfReal) myUPars;
  Handle(TColStd_HArray1OfReal) myVPars;

private:
  Standard_Integer          nbRestr;
  Standard_Integer          idRestr;
  Standard_Real             Uinf;
  Standard_Real             Usup;
  Standard_Real             Vinf;
  Standard_Real             Vsup;
  Handle(Adaptor2d_Line2d)  myRestr[4];
  Standard_Integer          nbVtx;
  Standard_Integer          idVtx;
  Handle(Adaptor3d_HVertex) myVtx[2];
};

#endif // _Adaptor3d_TopolTool_HeaderFile
