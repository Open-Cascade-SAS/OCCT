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

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <IntPolyh_ArrayOfSectionLines.hxx>
#include <IntPolyh_ArrayOfTangentZones.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Standard_Real.hxx>
#include <IntPolyh_PMaillageAffinage.hxx>
#include <IntPolyh_ListOfCouples.hxx>
class Adaptor3d_HSurface;


//! the main   algorithm.  Algorithm   outputs are
//! lines  and  points like   describe   in the last
//! paragraph.  The Algorithm provides direct access to
//! the elements of those   lines  and points. Other
//! classes  of this package  are for internal use and
//! only concern the algorithmic part.
class IntPolyh_Intersection 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructor
  Standard_EXPORT IntPolyh_Intersection(const Handle(Adaptor3d_HSurface)& S1, const Handle(Adaptor3d_HSurface)& S2);
  
  //! NbSU1 ... NbSV2 are used to compute the initial
  //! samples of  the  iso parametric  meshes  on the
  //! surfaces.
  Standard_EXPORT IntPolyh_Intersection(const Handle(Adaptor3d_HSurface)& S1, const Standard_Integer NbSU1, const Standard_Integer NbSV1, const Handle(Adaptor3d_HSurface)& S2, const Standard_Integer NbSU2, const Standard_Integer NbSV2);
  
  //! D1,  D2 are used to compute the initial
  //! samples of  the  iso parametric  meshes  on the
  //! surfaces.
  Standard_EXPORT IntPolyh_Intersection(const Handle(Adaptor3d_HSurface)& S1, const TColStd_Array1OfReal& anUpars1, const TColStd_Array1OfReal& aVpars1, const Handle(Adaptor3d_HSurface)& S2, const TColStd_Array1OfReal& anUpars2, const TColStd_Array1OfReal& aVpars2);
  
  //! Compute the intersection.
  Standard_EXPORT void Perform();
  
  //! Compute the intersection.
  Standard_EXPORT void Perform (const TColStd_Array1OfReal& Upars1, const TColStd_Array1OfReal& Vpars1, const TColStd_Array1OfReal& Upars2, const TColStd_Array1OfReal& Vpars2);
  
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  Standard_EXPORT Standard_Integer NbSectionLines() const;
  
  Standard_EXPORT Standard_Integer NbPointsInLine (const Standard_Integer IndexLine) const;
  
  Standard_EXPORT void GetLinePoint (const Standard_Integer IndexLine, const Standard_Integer IndexPoint, Standard_Real& x, Standard_Real& y, Standard_Real& z, Standard_Real& u1, Standard_Real& v1, Standard_Real& u2, Standard_Real& v2, Standard_Real& incidence) const;
  
  Standard_EXPORT Standard_Integer NbTangentZones() const;
  
  Standard_EXPORT Standard_Integer NbPointsInTangentZone (const Standard_Integer IndexLine) const;
  
  Standard_EXPORT void GetTangentZonePoint (const Standard_Integer IndexLine, const Standard_Integer IndexPoint, Standard_Real& x, Standard_Real& y, Standard_Real& z, Standard_Real& u1, Standard_Real& v1, Standard_Real& u2, Standard_Real& v2) const;




protected:





private:

  
  //! Computes MaillageAffinage
  Standard_EXPORT Standard_Boolean PerformMaillage (const Standard_Boolean isFirstFwd, const Standard_Boolean isSecondFwd, IntPolyh_PMaillageAffinage& MaillageS);
  
  //! The method PerformMaillage(..) is used to compute MaillageAffinage. It is
  //! called four times (two times for each surface) for creation of inscribed
  //! and circumscribed mesh for each surface.
  Standard_EXPORT Standard_Boolean PerformMaillage (IntPolyh_PMaillageAffinage& MaillageS);
  
  //! Computes MaillageAffinage
  Standard_EXPORT Standard_Boolean PerformMaillage (const Standard_Boolean isFirstFwd, const Standard_Boolean isSecondFwd, const TColStd_Array1OfReal& Upars1, const TColStd_Array1OfReal& Vpars1, const TColStd_Array1OfReal& Upars2, const TColStd_Array1OfReal& Vpars2, IntPolyh_PMaillageAffinage& MaillageS);
  
  //! The method PerformMaillage(..) is used to compute MaillageAffinage. It is
  //! called four times (two times for each surface) for creation of inscribed
  //! and circumscribed mesh for each surface.
  Standard_EXPORT Standard_Boolean PerformMaillage (const TColStd_Array1OfReal& Upars1, const TColStd_Array1OfReal& Vpars1, const TColStd_Array1OfReal& Upars2, const TColStd_Array1OfReal& Vpars2, IntPolyh_PMaillageAffinage& MaillageS);
  
  //! This method analyzes arrays to find same couples. If some
  //! are detected it leaves the couple in only one array
  //! deleting from others.
  Standard_EXPORT void MergeCouples (IntPolyh_ListOfCouples& anArrayFF, IntPolyh_ListOfCouples& anArrayFR, IntPolyh_ListOfCouples& anArrayRF, IntPolyh_ListOfCouples& anArrayRR) const;
  
  //! Process default interference
  Standard_EXPORT Standard_Boolean PerformStd (IntPolyh_PMaillageAffinage& MaillageS, Standard_Integer& NbCouples);
  
  //! Process advanced interference
  Standard_EXPORT Standard_Boolean PerformAdv (IntPolyh_PMaillageAffinage& MaillageFF, IntPolyh_PMaillageAffinage& MaillageFR, IntPolyh_PMaillageAffinage& MaillageRF, IntPolyh_PMaillageAffinage& MaillageRR, Standard_Integer& NbCouples);
  
  //! Process default interference
  Standard_EXPORT Standard_Boolean PerformStd (const TColStd_Array1OfReal& Upars1, const TColStd_Array1OfReal& Vpars1, const TColStd_Array1OfReal& Upars2, const TColStd_Array1OfReal& Vpars2, IntPolyh_PMaillageAffinage& MaillageS, Standard_Integer& NbCouples);
  
  //! Process advanced interference
  Standard_EXPORT Standard_Boolean PerformAdv (const TColStd_Array1OfReal& Upars1, const TColStd_Array1OfReal& Vpars1, const TColStd_Array1OfReal& Upars2, const TColStd_Array1OfReal& Vpars2, IntPolyh_PMaillageAffinage& MaillageFF, IntPolyh_PMaillageAffinage& MaillageFR, IntPolyh_PMaillageAffinage& MaillageRF, IntPolyh_PMaillageAffinage& MaillageRR, Standard_Integer& NbCouples);


  Standard_Boolean done;
  Standard_Integer nbsectionlines;
  Standard_Integer nbtangentzones;
  IntPolyh_ArrayOfSectionLines TSectionLines;
  IntPolyh_ArrayOfTangentZones TTangentZones;
  Standard_Integer myNbSU1;
  Standard_Integer myNbSV1;
  Standard_Integer myNbSU2;
  Standard_Integer myNbSV2;
  Handle(Adaptor3d_HSurface) mySurf1;
  Handle(Adaptor3d_HSurface) mySurf2;


};







#endif // _IntPolyh_Intersection_HeaderFile
