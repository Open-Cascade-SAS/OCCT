// Created on: 1994-02-18
// Created by: Bruno DUMORTIER
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

#ifndef _GeomFill_SectionGenerator_HeaderFile
#define _GeomFill_SectionGenerator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <GeomFill_Profiler.hxx>
#include <Standard_Integer.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>

//! Provides section data for surface approximation through the profiler curves.
//! Section parameters are copied and indexed from one; section evaluation supplies
//! poles, weights and endpoint derivatives to the AppSurf approximation algorithm.
class GeomFill_SectionGenerator : public GeomFill_Profiler
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a section generator without curves or section parameters.
  Standard_EXPORT GeomFill_SectionGenerator();

  //! Copies section parameters into independent one-based storage.
  //! @param Params nonempty array of finite, strictly increasing parameters
  //! @throws Standard_NullObject if Params is null
  //! @throws Standard_ConstructionError if values are invalid; stored parameters remain unchanged
  Standard_EXPORT void SetParam(const occ::handle<NCollection_HArray1<double>>& Params);

  //! Returns the pole counts, knot count and degree of the section curves.
  Standard_EXPORT void GetShape(int& NbPoles, int& NbKnots, int& Degree, int& NbPoles2d) const;

  //! Fills TKnots with the common section knot sequence.
  Standard_EXPORT void Knots(NCollection_Array1<double>& TKnots) const;

  //! Fills TMults with the common section knot multiplicities.
  Standard_EXPORT void Mults(NCollection_Array1<int>& TMults) const;

  //! Used for the first and last section
  //! The method returns true if the derivatives
  //! are computed, otherwise it returns false.
  Standard_EXPORT bool Section(const int                     P,
                               NCollection_Array1<gp_Pnt>&   Poles,
                               NCollection_Array1<gp_Vec>&   DPoles,
                               NCollection_Array1<gp_Pnt2d>& Poles2d,
                               NCollection_Array1<gp_Vec2d>& DPoles2d,
                               NCollection_Array1<double>&   Weigths,
                               NCollection_Array1<double>&   DWeigths) const;

  //! Returns poles and weights for section P.
  Standard_EXPORT void Section(const int                     P,
                               NCollection_Array1<gp_Pnt>&   Poles,
                               NCollection_Array1<gp_Pnt2d>& Poles2d,
                               NCollection_Array1<double>&   Weigths) const;

  //! Returns the parameter of Section<P>, to impose it for the
  //! approximation.
  Standard_EXPORT double Parameter(const int P) const;

protected:
  occ::handle<NCollection_HArray1<double>> myParams;
};

#endif // _GeomFill_SectionGenerator_HeaderFile
