// Created on: 1999-03-03
// Created by: Fabrice SERVANT
// Copyright (c) 1999 Matra Datavision
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

#ifndef _IntPolyh_Intersection_HeaderFile
#define _IntPolyh_Intersection_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <IntPolyh_ArrayOfPointNormal.hxx>
#include <IntPolyh_ArrayOfSectionLines.hxx>
#include <IntPolyh_ArrayOfTangentZones.hxx>
#include <IntPolyh_ListOfCouples.hxx>
#include <IntPolyh_PMaillageAffinage.hxx>
#include <TColStd_Array1OfReal.hxx>

//! API algorithm for intersection of two surfaces by intersection
//! of their triangulations.
//!
//! Algorithm provides possibility to intersect surfaces as without
//! the precomputed sampling as with it.
//!
//! If the numbers of sampling points are not given, it will build the
//! net of 10x10 sampling points for each surface.
//!
//! The intersection is done inside constructors.
//! Before obtaining the results of intersection it is necessary to check
//! if intersection has been performed correctly. It can be done by calling
//! the *IsDone()* method.
//!
//! The results of intersection are the intersection lines and points.
class IntPolyh_Intersection
{
public:

  DEFINE_STANDARD_ALLOC

public: //! @name Constructors

  //! Constructor for intersection of two surfaces with default parameters.
  //! Performs intersection.
  Standard_EXPORT IntPolyh_Intersection(const Handle(Adaptor3d_Surface)& theS1,
                                        const Handle(Adaptor3d_Surface)& theS2);

  //! Constructor for intersection of two surfaces with the given
  //! size of the sampling nets:
  //! - <theNbSU1> x <theNbSV1> - for the first surface <theS1>;
  //! - <theNbSU2> x <theNbSV2> - for the second surface <theS2>.
  //! Performs intersection.
  Standard_EXPORT IntPolyh_Intersection(const Handle(Adaptor3d_Surface)& theS1,
                                        const Standard_Integer            theNbSU1,
                                        const Standard_Integer            theNbSV1,
                                        const Handle(Adaptor3d_Surface)& theS2,
                                        const Standard_Integer            theNbSU2,
                                        const Standard_Integer            theNbSV2);

  //! Constructor for intersection of two surfaces with the precomputed sampling.
  //! Performs intersection.
  Standard_EXPORT IntPolyh_Intersection(const Handle(Adaptor3d_Surface)& theS1,
                                        const TColStd_Array1OfReal&       theUPars1,
                                        const TColStd_Array1OfReal&       theVPars1,
                                        const Handle(Adaptor3d_Surface)& theS2,
                                        const TColStd_Array1OfReal&       theUPars2,
                                        const TColStd_Array1OfReal&       theVPars2);


public: //! @name Getting the results

  //! Returns state of the operation
  Standard_Boolean IsDone() const
  {
    return myIsDone;
  }

  //! Returns the number of section lines
  Standard_Integer NbSectionLines() const
  {
    return mySectionLines.NbItems();
  }

  //! Returns the number of points in the given line
  Standard_Integer NbPointsInLine(const Standard_Integer IndexLine) const
  {
    return mySectionLines[IndexLine-1].NbStartPoints();
  }

  // Returns number of tangent zones
  Standard_Integer NbTangentZones() const
  {
    return myTangentZones.NbItems();
  }

  //! Returns number of points in tangent zone
  Standard_Integer NbPointsInTangentZone(const Standard_Integer) const
  {
    return 1;
  }

  //! Gets the parameters of the point in section line
  Standard_EXPORT void GetLinePoint(const Standard_Integer IndexLine,
                                    const Standard_Integer IndexPoint,
                                    Standard_Real& x, Standard_Real& y, Standard_Real& z,
                                    Standard_Real& u1, Standard_Real& v1,
                                    Standard_Real& u2, Standard_Real& v2,
                                    Standard_Real& incidence) const;

  //! Gets the parameters of the point in tangent zone
  Standard_EXPORT void GetTangentZonePoint(const Standard_Integer IndexLine,
                                           const Standard_Integer IndexPoint,
                                           Standard_Real& x, Standard_Real& y, Standard_Real& z,
                                           Standard_Real& u1, Standard_Real& v1,
                                           Standard_Real& u2, Standard_Real& v2) const;


private: //! @name Performing the intersection

  //! Compute the intersection by first making the sampling of the surfaces.
  Standard_EXPORT void Perform();

  //! Compute the intersection on the precomputed sampling.
  Standard_EXPORT void Perform(const TColStd_Array1OfReal& theUPars1,
                               const TColStd_Array1OfReal& theVPars1,
                               const TColStd_Array1OfReal& theUPars2,
                               const TColStd_Array1OfReal& theVPars2);

  //! Performs the default (standard) intersection of the triangles
  Standard_EXPORT Standard_Boolean PerformStd(const TColStd_Array1OfReal& theUPars1,
                                              const TColStd_Array1OfReal& theVPars1,
                                              const TColStd_Array1OfReal& theUPars2,
                                              const TColStd_Array1OfReal& theVPars2,
                                              const Standard_Real         theDeflTol1,
                                              const Standard_Real         theDeflTol2,
                                              IntPolyh_PMaillageAffinage& theMaillageS,
                                              Standard_Integer&           theNbCouples);

  //! Performs the advanced intersection of the triangles - four intersection with
  //! different shifts of the sampling points.
  Standard_EXPORT Standard_Boolean PerformAdv(const TColStd_Array1OfReal& theUPars1,
                                              const TColStd_Array1OfReal& theVPars1,
                                              const TColStd_Array1OfReal& theUPars2,
                                              const TColStd_Array1OfReal& theVPars2,
                                              const Standard_Real         theDeflTol1,
                                              const Standard_Real         theDeflTol2,
                                              IntPolyh_PMaillageAffinage& theMaillageFF,
                                              IntPolyh_PMaillageAffinage& theMaillageFR,
                                              IntPolyh_PMaillageAffinage& theMaillageRF,
                                              IntPolyh_PMaillageAffinage& theMaillageRR,
                                              Standard_Integer&           theNbCouples);

  //! Performs the advanced intersection of the triangles.
  Standard_EXPORT Standard_Boolean PerformMaillage(const TColStd_Array1OfReal& theUPars1,
                                                   const TColStd_Array1OfReal& theVPars1,
                                                   const TColStd_Array1OfReal& theUPars2,
                                                   const TColStd_Array1OfReal& theVPars2,
                                                   const Standard_Real         theDeflTol1,
                                                   const Standard_Real         theDeflTol2,
                                                   IntPolyh_PMaillageAffinage& theMaillage);

  //! Performs the advanced intersection of the triangles.
  Standard_EXPORT Standard_Boolean PerformMaillage(const TColStd_Array1OfReal& theUPars1,
                                                   const TColStd_Array1OfReal& theVPars1,
                                                   const TColStd_Array1OfReal& theUPars2,
                                                   const TColStd_Array1OfReal& theVPars2,
                                                   const Standard_Real         theDeflTol1,
                                                   const Standard_Real         theDeflTol2,
                                                   const IntPolyh_ArrayOfPointNormal& thePoints1,
                                                   const IntPolyh_ArrayOfPointNormal& thePoints2,
                                                   const Standard_Boolean      theIsFirstFwd,
                                                   const Standard_Boolean      theIsSecondFwd,
                                                   IntPolyh_PMaillageAffinage& theMaillage);

  //! Clears the arrays from the duplicate couples, keeping only one instance of it.
  Standard_EXPORT void MergeCouples(IntPolyh_ListOfCouples& theArrayFF,
                                    IntPolyh_ListOfCouples& theArrayFR,
                                    IntPolyh_ListOfCouples& theArrayRF,
                                    IntPolyh_ListOfCouples& theArrayRR) const;


private: //! @name Fields

  // Inputs
  Handle(Adaptor3d_Surface) mySurf1;          //!< First surface
  Handle(Adaptor3d_Surface) mySurf2;          //!< Second surface
  Standard_Integer myNbSU1;                    //!< Number of samples in U direction for first surface
  Standard_Integer myNbSV1;                    //!< Number of samples in V direction for first surface
  Standard_Integer myNbSU2;                    //!< Number of samples in U direction for second surface
  Standard_Integer myNbSV2;                    //!< Number of samples in V direction for second surface
  // Results
  Standard_Boolean myIsDone;                   //!< State of the operation
  IntPolyh_ArrayOfSectionLines mySectionLines; //!< Section lines
  IntPolyh_ArrayOfTangentZones myTangentZones; //!< Tangent zones
};

#endif // _IntPolyh_Intersection_HeaderFile
