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
#include <NCollection_List.hxx>
#include <IntPolyh_Couple.hxx>
#include <IntPolyh_PMaillageAffinage.hxx>
#include <NCollection_Array1.hxx>

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
  Standard_EXPORT IntPolyh_Intersection(const occ::handle<Adaptor3d_Surface>& theS1,
                                        const occ::handle<Adaptor3d_Surface>& theS2);

  //! Constructor for intersection of two surfaces with the given
  //! size of the sampling nets:
  //! - <theNbSU1> x <theNbSV1> - for the first surface <theS1>;
  //! - <theNbSU2> x <theNbSV2> - for the second surface <theS2>.
  //! Performs intersection.
  Standard_EXPORT IntPolyh_Intersection(const occ::handle<Adaptor3d_Surface>& theS1,
                                        const int           theNbSU1,
                                        const int           theNbSV1,
                                        const occ::handle<Adaptor3d_Surface>& theS2,
                                        const int           theNbSU2,
                                        const int           theNbSV2);

  //! Constructor for intersection of two surfaces with the precomputed sampling.
  //! Performs intersection.
  Standard_EXPORT IntPolyh_Intersection(const occ::handle<Adaptor3d_Surface>& theS1,
                                        const NCollection_Array1<double>&      theUPars1,
                                        const NCollection_Array1<double>&      theVPars1,
                                        const occ::handle<Adaptor3d_Surface>& theS2,
                                        const NCollection_Array1<double>&      theUPars2,
                                        const NCollection_Array1<double>&      theVPars2);

public: //! @name Getting the results
  //! Returns state of the operation
  bool IsDone() const { return myIsDone; }

  //! Returns state of the operation
  bool IsParallel() const { return myIsParallel; }

  //! Returns the number of section lines
  int NbSectionLines() const { return mySectionLines.NbItems(); }

  //! Returns the number of points in the given line
  int NbPointsInLine(const int IndexLine) const
  {
    return mySectionLines[IndexLine - 1].NbStartPoints();
  }

  // Returns number of tangent zones
  int NbTangentZones() const { return myTangentZones.NbItems(); }

  //! Returns number of points in tangent zone
  int NbPointsInTangentZone(const int) const { return 1; }

  //! Gets the parameters of the point in section line
  Standard_EXPORT void GetLinePoint(const int IndexLine,
                                    const int IndexPoint,
                                    double&         x,
                                    double&         y,
                                    double&         z,
                                    double&         u1,
                                    double&         v1,
                                    double&         u2,
                                    double&         v2,
                                    double&         incidence) const;

  //! Gets the parameters of the point in tangent zone
  Standard_EXPORT void GetTangentZonePoint(const int IndexLine,
                                           const int IndexPoint,
                                           double&         x,
                                           double&         y,
                                           double&         z,
                                           double&         u1,
                                           double&         v1,
                                           double&         u2,
                                           double&         v2) const;

private: //! @name Performing the intersection
  //! Compute the intersection by first making the sampling of the surfaces.
  Standard_EXPORT void Perform();

  //! Compute the intersection on the precomputed sampling.
  Standard_EXPORT void Perform(const NCollection_Array1<double>& theUPars1,
                               const NCollection_Array1<double>& theVPars1,
                               const NCollection_Array1<double>& theUPars2,
                               const NCollection_Array1<double>& theVPars2);

  //! Performs the default (standard) intersection of the triangles
  Standard_EXPORT bool PerformStd(const NCollection_Array1<double>& theUPars1,
                                              const NCollection_Array1<double>& theVPars1,
                                              const NCollection_Array1<double>& theUPars2,
                                              const NCollection_Array1<double>& theVPars2,
                                              const double         theDeflTol1,
                                              const double         theDeflTol2,
                                              IntPolyh_PMaillageAffinage& theMaillageS,
                                              int&           theNbCouples);

  //! Performs the advanced intersection of the triangles - four intersection with
  //! different shifts of the sampling points.
  Standard_EXPORT bool PerformAdv(const NCollection_Array1<double>& theUPars1,
                                              const NCollection_Array1<double>& theVPars1,
                                              const NCollection_Array1<double>& theUPars2,
                                              const NCollection_Array1<double>& theVPars2,
                                              const double         theDeflTol1,
                                              const double         theDeflTol2,
                                              IntPolyh_PMaillageAffinage& theMaillageFF,
                                              IntPolyh_PMaillageAffinage& theMaillageFR,
                                              IntPolyh_PMaillageAffinage& theMaillageRF,
                                              IntPolyh_PMaillageAffinage& theMaillageRR,
                                              int&           theNbCouples);

  //! Performs the advanced intersection of the triangles.
  Standard_EXPORT bool PerformMaillage(const NCollection_Array1<double>& theUPars1,
                                                   const NCollection_Array1<double>& theVPars1,
                                                   const NCollection_Array1<double>& theUPars2,
                                                   const NCollection_Array1<double>& theVPars2,
                                                   const double         theDeflTol1,
                                                   const double         theDeflTol2,
                                                   IntPolyh_PMaillageAffinage& theMaillage);

  //! Performs the advanced intersection of the triangles.
  Standard_EXPORT bool PerformMaillage(const NCollection_Array1<double>&        theUPars1,
                                                   const NCollection_Array1<double>&        theVPars1,
                                                   const NCollection_Array1<double>&        theUPars2,
                                                   const NCollection_Array1<double>&        theVPars2,
                                                   const double                theDeflTol1,
                                                   const double                theDeflTol2,
                                                   const IntPolyh_ArrayOfPointNormal& thePoints1,
                                                   const IntPolyh_ArrayOfPointNormal& thePoints2,
                                                   const bool             theIsFirstFwd,
                                                   const bool      theIsSecondFwd,
                                                   IntPolyh_PMaillageAffinage& theMaillage);

  //! Clears the arrays from the duplicate couples, keeping only one instance of it.
  Standard_EXPORT void MergeCouples(NCollection_List<IntPolyh_Couple>& theArrayFF,
                                    NCollection_List<IntPolyh_Couple>& theArrayFR,
                                    NCollection_List<IntPolyh_Couple>& theArrayRF,
                                    NCollection_List<IntPolyh_Couple>& theArrayRR) const;

  bool AnalyzeIntersection(IntPolyh_PMaillageAffinage& theMaillage);
  bool IsAdvRequired(IntPolyh_PMaillageAffinage& theMaillage);

private: //! @name Fields
  // Inputs
  occ::handle<Adaptor3d_Surface> mySurf1;           //!< First surface
  occ::handle<Adaptor3d_Surface> mySurf2;           //!< Second surface
                                               // clang-format off
  int myNbSU1;                    //!< Number of samples in U direction for first surface
  int myNbSV1;                    //!< Number of samples in V direction for first surface
  int myNbSU2;                    //!< Number of samples in U direction for second surface
  int myNbSV2;                    //!< Number of samples in V direction for second surface
  // Results
                                               // clang-format on
  bool             myIsDone;       //!< State of the operation
  IntPolyh_ArrayOfSectionLines mySectionLines; //!< Section lines
  IntPolyh_ArrayOfTangentZones myTangentZones; //!< Tangent zones
  bool             myIsParallel;
};

#endif // _IntPolyh_Intersection_HeaderFile
