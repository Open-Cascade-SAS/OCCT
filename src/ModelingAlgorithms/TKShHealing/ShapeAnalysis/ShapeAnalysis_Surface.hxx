// Created on: 1998-06-03
// Created by: data exchange team
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _ShapeAnalysis_Surface_HeaderFile
#define _ShapeAnalysis_Surface_HeaderFile

#include <Extrema_ExtPS.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Bnd_Box.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TColgp_SequenceOfPnt2d.hxx>

class Geom_Surface;
class Geom_Curve;

DEFINE_STANDARD_HANDLE(ShapeAnalysis_Surface, Standard_Transient)

//! Complements standard tool Geom_Surface by providing additional
//! functionality for detection surface singularities, checking
//! spatial surface closure and computing projections of 3D points
//! onto a surface.
//!
//! * The singularities
//! Each singularity stores the precision with which corresponding
//! surface iso-line is considered as degenerated.
//! The number of singularities is determined by specifying precision
//! and always not greater than 4.
//!
//! * The spatial closure
//! The check for spatial closure is performed with given precision
//! (default value is Precision::Confusion).
//! If Geom_Surface says that the surface is closed, this class
//! also says this. Otherwise additional analysis is performed.
//!
//! * The parameters of 3D point on the surface
//! The projection of the point is performed with given precision.
//! This class tries to find a solution taking into account possible
//! singularities.
//! Additional method for searching the solution from already built
//! one is also provided.
//!
//! This tool is optimised: computes most information only once
class ShapeAnalysis_Surface : public Standard_Transient
{

public:
  //! Creates an analyzer object on the basis of existing surface
  Standard_EXPORT ShapeAnalysis_Surface(const Handle(Geom_Surface)& S);

  //! Loads existing surface
  Standard_EXPORT void Init(const Handle(Geom_Surface)& S);

  //! Reads all the data from another Surface, without recomputing
  Standard_EXPORT void Init(const Handle(ShapeAnalysis_Surface)& other);

  Standard_EXPORT void SetDomain(const Standard_Real U1,
                                 const Standard_Real U2,
                                 const Standard_Real V1,
                                 const Standard_Real V2);

  //! Returns a surface being analyzed
  const Handle(Geom_Surface)& Surface() const;

  //! Returns the Adaptor.
  //! Creates it if not yet done.
  Standard_EXPORT const Handle(GeomAdaptor_Surface)& Adaptor3d();

  //! Returns the Adaptor (may be Null if method Adaptor() was not called)
  const Handle(GeomAdaptor_Surface)& TrueAdaptor3d() const;

  //! Returns 3D distance found by one of the following methods.
  //! IsDegenerated, DegeneratedValues, ProjectDegenerated
  //! (distance between 3D point and found or last (if not found)
  //! singularity),
  //! IsUClosed, IsVClosed (minimum value of precision to consider
  //! the surface to be closed),
  //! ValueOfUV (distance between 3D point and found solution).
  Standard_Real Gap() const;

  //! Returns a 3D point specified by parameters in surface
  //! parametrical space
  gp_Pnt Value(const Standard_Real u, const Standard_Real v);

  //! Returns a 3d point specified by a point in surface
  //! parametrical space
  gp_Pnt Value(const gp_Pnt2d& p2d);

  //! Returns True if the surface has singularities for the given
  //! precision (i.e. if there are surface singularities with sizes
  //! not greater than precision).
  Standard_EXPORT Standard_Boolean HasSingularities(const Standard_Real preci);

  //! Returns the number of singularities for the given precision
  //! (i.e. number of surface singularities with sizes not greater
  //! than precision).
  Standard_EXPORT Standard_Integer NbSingularities(const Standard_Real preci);

  //! Returns the characteristics of the singularity specified by
  //! its rank number <num>.
  //! That means, that it is not necessary for <num> to be in the
  //! range [1, NbSingularities] but must be not greater than
  //! possible (see ComputeSingularities).
  //! The returned characteristics are:
  //! preci: the smallest precision with which the iso-line is
  //! considered as degenerated,
  //! P3d: 3D point of singularity (middle point of the surface
  //! iso-line),
  //! firstP2d and lastP2d: first and last 2D points of the
  //! iso-line in parametrical surface,
  //! firstpar and lastpar: first and last parameters of the
  //! iso-line in parametrical surface,
  //! uisodeg: if the degenerated iso-line is U-iso (True) or
  //! V-iso (False).
  //! Returns False if <num> is out of range, else returns True.
  Standard_EXPORT Standard_Boolean Singularity(const Standard_Integer num,
                                               Standard_Real&         preci,
                                               gp_Pnt&                P3d,
                                               gp_Pnt2d&              firstP2d,
                                               gp_Pnt2d&              lastP2d,
                                               Standard_Real&         firstpar,
                                               Standard_Real&         lastpar,
                                               Standard_Boolean&      uisodeg);

  //! Returns True if there is at least one surface boundary which
  //! is considered as degenerated with <preci> and distance
  //! between P3d and corresponding singular point is less than
  //! <preci>
  Standard_EXPORT Standard_Boolean IsDegenerated(const gp_Pnt& P3d, const Standard_Real preci);

  //! Returns True if there is at least one surface iso-line which
  //! is considered as degenerated with <preci> and distance
  //! between P3d and corresponding singular point is less than
  //! <preci> (like IsDegenerated).
  //! Returns characteristics of the first found boundary matching
  //! those criteria.
  Standard_EXPORT Standard_Boolean
    DegeneratedValues(const gp_Pnt&          P3d,
                      const Standard_Real    preci,
                      gp_Pnt2d&              firstP2d,
                      gp_Pnt2d&              lastP2d,
                      Standard_Real&         firstpar,
                      Standard_Real&         lastpar,
                      const Standard_Boolean forward = Standard_True);

  //! Projects a point <P3d> on a singularity by computing
  //! one of the coordinates of preliminary computed <result>.
  //!
  //! Finds the iso-line which is considered as degenerated with
  //! <preci> and
  //! a. distance between P3d and corresponding singular point is
  //! less than <preci> (like IsDegenerated) or
  //! b. difference between already computed <result>'s coordinate
  //! and iso-coordinate of the boundary is less than 2D
  //! resolution (computed from <preci> by Geom_Adaptor).
  //! Then sets not yet computed <result>'s coordinate taking it
  //! from <neighbour> and returns True.
  Standard_EXPORT Standard_Boolean ProjectDegenerated(const gp_Pnt&       P3d,
                                                      const Standard_Real preci,
                                                      const gp_Pnt2d&     neighbour,
                                                      gp_Pnt2d&           result);

  //! Checks points at the beginning (direct is True) or end
  //! (direct is False) of array <points> to lie in singularity of
  //! surface, and if yes, adjusts the indeterminate 2d coordinate
  //! of these points by nearest point which is not in singularity.
  //! Returns True if some points were adjusted.
  Standard_EXPORT Standard_Boolean ProjectDegenerated(const Standard_Integer      nbrPnt,
                                                      const TColgp_SequenceOfPnt& points,
                                                      TColgp_SequenceOfPnt2d&     pnt2d,
                                                      const Standard_Real         preci,
                                                      const Standard_Boolean      direct);

  //! Returns True if straight pcurve going from point p2d1 to p2d2
  //! is degenerate, i.e. lies in the singularity of the surface.
  //! NOTE: it uses another method of detecting singularity than
  //! used by ComputeSingularities() et al.!
  //! For that, maximums of distances between points p2d1, p2d2
  //! and 0.5*(p2d1+p2d2) and between corresponding 3d points are
  //! computed.
  //! The pcurve (p2d1, p2d2) is considered as degenerate if:
  //! - max distance in 3d is less than <tol>
  //! - max distance in 2d is at least <ratio> times greater than
  //! the Resolution computed from max distance in 3d
  //! (max3d < tol && max2d > ratio * Resolution(max3d))
  //! NOTE: <ratio> should be >1 (e.g. 10)
  Standard_EXPORT Standard_Boolean IsDegenerated(const gp_Pnt2d&     p2d1,
                                                 const gp_Pnt2d&     p2d2,
                                                 const Standard_Real tol,
                                                 const Standard_Real ratio);

  //! Returns the bounds of the surface
  //! (from Bounds from Surface, but buffered)
  void Bounds(Standard_Real& ufirst,
              Standard_Real& ulast,
              Standard_Real& vfirst,
              Standard_Real& vlast) const;

  //! Computes bound isos (protected against exceptions)
  Standard_EXPORT void ComputeBoundIsos();

  //! Returns a U-Iso. Null if not possible or failed
  //! Remark : bound isos are buffered
  Standard_EXPORT Handle(Geom_Curve) UIso(const Standard_Real U);

  //! Returns a V-Iso. Null if not possible or failed
  //! Remark : bound isos are buffered
  Standard_EXPORT Handle(Geom_Curve) VIso(const Standard_Real V);

  //! Tells if the Surface is spatially closed in U with given
  //! precision. If <preci> < 0 then Precision::Confusion is used.
  //! If Geom_Surface says that the surface is U-closed, this method
  //! also says this. Otherwise additional analysis is performed,
  //! comparing given precision with the following distances:
  //! - periodic B-Splines are closed,
  //! - polynomial B-Spline with boundary multiplicities degree+1
  //! and Bezier - maximum distance between poles,
  //! - rational B-Spline or one with boundary multiplicities not
  //! degree+1 - maximum distance computed at knots and their
  //! middles,
  //! - surface of extrusion - distance between ends of basis
  //! curve,
  //! - other (RectangularTrimmed and Offset) - maximum distance
  //! computed at 100 equi-distanted points.
  Standard_EXPORT Standard_Boolean IsUClosed(const Standard_Real preci = -1);

  //! Tells if the Surface is spatially closed in V with given
  //! precision. If <preci> < 0 then Precision::Confusion is used.
  //! If Geom_Surface says that the surface is V-closed, this method
  //! also says this. Otherwise additional analysis is performed,
  //! comparing given precision with the following distances:
  //! - periodic B-Splines are closed,
  //! - polynomial B-Spline with boundary multiplicities degree+1
  //! and Bezier - maximum distance between poles,
  //! - rational B-Spline or one with boundary multiplicities not
  //! degree+1 - maximum distance computed at knots and their
  //! middles,
  //! - surface of revolution - distance between ends of basis
  //! curve,
  //! - other (RectangularTrimmed and Offset) - maximum distance
  //! computed at 100 equi-distanted points.
  Standard_EXPORT Standard_Boolean IsVClosed(const Standard_Real preci = -1);

  //! Computes the parameters in the surface parametrical space of
  //! 3D point.
  //! The result is parameters of the point projected onto the
  //! surface.
  //! This method enhances functionality provided by the standard
  //! tool GeomAPI_ProjectPointOnSurface by treatment of cases when
  //! the projected point is near to the surface boundaries and
  //! when this standard tool fails.
  Standard_EXPORT gp_Pnt2d ValueOfUV(const gp_Pnt& P3D, const Standard_Real preci);

  //! Projects a point P3D on the surface.
  //! Does the same thing as ValueOfUV but tries to optimize
  //! computations by taking into account previous point <p2dPrev>:
  //! makes a step by UV and tries Newton algorithm.
  //! If <maxpreci> >0. and distance between solution and
  //! P3D is greater than <maxpreci>, that solution is considered
  //! as bad, and ValueOfUV() is used.
  //! If not succeeded, calls ValueOfUV()
  Standard_EXPORT gp_Pnt2d NextValueOfUV(const gp_Pnt2d&     p2dPrev,
                                         const gp_Pnt&       P3D,
                                         const Standard_Real preci,
                                         const Standard_Real maxpreci = -1.0);

  //! Tries a refinement of an already computed couple (U,V) by
  //! using projecting 3D point on iso-lines:
  //! 1. boundaries of the surface,
  //! 2. iso-lines passing through (U,V)
  //! 3. iteratively received iso-lines passing through new U and
  //! new V (number of iterations is limited by 5 in each
  //! direction)
  //! Returns the best resulting distance between P3D and Value(U,V)
  //! in the case of success. Else, returns a very great value
  Standard_EXPORT Standard_Real UVFromIso(const gp_Pnt&       P3D,
                                          const Standard_Real preci,
                                          Standard_Real&      U,
                                          Standard_Real&      V);

  //! Returns minimum value to consider the surface as U-closed
  Standard_Real UCloseVal() const;

  //! Returns minimum value to consider the surface as V-closed
  Standard_Real VCloseVal() const;

  Standard_EXPORT const Bnd_Box& GetBoxUF();

  Standard_EXPORT const Bnd_Box& GetBoxUL();

  Standard_EXPORT const Bnd_Box& GetBoxVF();

  Standard_EXPORT const Bnd_Box& GetBoxVL();

  DEFINE_STANDARD_RTTIEXT(ShapeAnalysis_Surface, Standard_Transient)

protected:
  Handle(Geom_Surface)        mySurf;
  Handle(GeomAdaptor_Surface) myAdSur;
  Extrema_ExtPS               myExtPS;
  Standard_Boolean            myExtOK;
  Standard_Integer            myNbDeg;
  Standard_Real               myPreci[4];
  gp_Pnt                      myP3d[4];
  gp_Pnt2d                    myFirstP2d[4];
  gp_Pnt2d                    myLastP2d[4];
  Standard_Real               myFirstPar[4];
  Standard_Real               myLastPar[4];
  Standard_Boolean            myUIsoDeg[4];
  Standard_Boolean            myIsos;
  Standard_Real               myUF;
  Standard_Real               myUL;
  Standard_Real               myVF;
  Standard_Real               myVL;
  Handle(Geom_Curve)          myIsoUF;
  Handle(Geom_Curve)          myIsoUL;
  Handle(Geom_Curve)          myIsoVF;
  Handle(Geom_Curve)          myIsoVL;
  Standard_Boolean            myIsoBoxes;
  Bnd_Box                     myBndUF;
  Bnd_Box                     myBndUL;
  Bnd_Box                     myBndVF;
  Bnd_Box                     myBndVL;
  Standard_Real               myGap;
  Standard_Real               myUDelt;
  Standard_Real               myVDelt;
  Standard_Real               myUCloseVal;
  Standard_Real               myVCloseVal;

private:
  //! Computes singularities on the surface.
  //! Computes the sizes of boundaries or singular ares of the
  //! surface. Then each boundary or area is considered as
  //! degenerated with precision not less than its size.
  //!
  //! The singularities and corresponding precisions are the
  //! following:
  //! - ConicalSurface -  one degenerated point (apex of the cone),
  //! precision is 0.,
  //! - ToroidalSurface - two degenerated points, precision is
  //! Max (0, majorR-minorR),
  //! - SphericalSurface - two degenerated points (poles),
  //! precision is 0.
  //! - Bounded, Surface Of Revolution, Offset - four degenerated
  //! points, precisions are maximum distance between corners
  //! and middle point on the boundary
  Standard_EXPORT void ComputeSingularities();

  Standard_EXPORT void ComputeBoxes();

  //! @return 0, 1 or 2.
  Standard_EXPORT Standard_Integer SurfaceNewton(const gp_Pnt2d&     p2dPrev,
                                                 const gp_Pnt&       P3D,
                                                 const Standard_Real preci,
                                                 gp_Pnt2d&           sol);

  Standard_EXPORT void SortSingularities();
};

#include <ShapeAnalysis_Surface.lxx>

#endif // _ShapeAnalysis_Surface_HeaderFile
