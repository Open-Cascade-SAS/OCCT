// Created on: 1991-10-10
// Created by: Jean Claude VAUTHIER
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Convert_ElementarySurfaceToBSplineSurface_HeaderFile
#define _Convert_ElementarySurfaceToBSplineSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Macro.hxx>

#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>

//! Root class for algorithms which convert an elementary
//! surface (cylinder, cone, sphere or torus) into a BSpline surface.
//! These algorithms all work on elementary surfaces from
//! the gp package and compute all the data needed to
//! construct a BSpline surface equivalent to the cylinder,
//! cone, sphere or torus.
class Convert_ElementarySurfaceToBSplineSurface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns the degree in the U parametric direction.
  [[nodiscard]] Standard_EXPORT int UDegree() const;

  //! Returns the degree in the V parametric direction.
  [[nodiscard]] Standard_EXPORT int VDegree() const;

  //! Returns the number of poles in the U parametric direction.
  [[nodiscard]] Standard_EXPORT int NbUPoles() const;

  //! Returns the number of poles in the V parametric direction.
  [[nodiscard]] Standard_EXPORT int NbVPoles() const;

  //! Returns the number of knots in the U parametric direction.
  [[nodiscard]] Standard_EXPORT int NbUKnots() const;

  //! Returns the number of knots in the V parametric direction.
  [[nodiscard]] Standard_EXPORT int NbVKnots() const;

  //! Returns true if the surface is periodic in the U parametric direction.
  [[nodiscard]] Standard_EXPORT bool IsUPeriodic() const;

  //! Returns true if the surface is periodic in the V parametric direction.
  [[nodiscard]] Standard_EXPORT bool IsVPeriodic() const;

  //! Returns the pole of index (UIndex, VIndex).
  //! @throws Standard_OutOfRange if indices are out of bounds
  Standard_DEPRECATED("Use Poles() batch accessor instead")
  Standard_EXPORT gp_Pnt Pole(const int UIndex, const int VIndex) const;

  //! Returns the weight of the pole of index (UIndex, VIndex).
  //! @throws Standard_OutOfRange if indices are out of bounds
  Standard_DEPRECATED("Use Weights() batch accessor instead")
  Standard_EXPORT double Weight(const int UIndex, const int VIndex) const;

  //! Returns the U-knot of range UIndex.
  //! @throws Standard_OutOfRange if UIndex is out of bounds
  Standard_DEPRECATED("Use UKnots() batch accessor instead")
  Standard_EXPORT double UKnot(const int UIndex) const;

  //! Returns the V-knot of range VIndex.
  //! @throws Standard_OutOfRange if VIndex is out of bounds
  Standard_DEPRECATED("Use VKnots() batch accessor instead")
  Standard_EXPORT double VKnot(const int VIndex) const;

  //! Returns the multiplicity of the U-knot of range UIndex.
  //! @throws Standard_OutOfRange if UIndex is out of bounds
  Standard_DEPRECATED("Use UMultiplicities() batch accessor instead")
  Standard_EXPORT int UMultiplicity(const int UIndex) const;

  //! Returns the multiplicity of the V-knot of range VIndex.
  //! @throws Standard_OutOfRange if VIndex is out of bounds
  Standard_DEPRECATED("Use VMultiplicities() batch accessor instead")
  Standard_EXPORT int VMultiplicity(const int VIndex) const;

  //! Returns the poles of the BSpline surface.
  [[nodiscard]] Standard_EXPORT const NCollection_Array2<gp_Pnt>& Poles() const;

  //! Returns the weights of the BSpline surface.
  [[nodiscard]] Standard_EXPORT const NCollection_Array2<double>& Weights() const;

  //! Returns the U-knots of the BSpline surface.
  [[nodiscard]] Standard_EXPORT const NCollection_Array1<double>& UKnots() const;

  //! Returns the V-knots of the BSpline surface.
  [[nodiscard]] Standard_EXPORT const NCollection_Array1<double>& VKnots() const;

  //! Returns the U-multiplicities of the BSpline surface.
  [[nodiscard]] Standard_EXPORT const NCollection_Array1<int>& UMultiplicities() const;

  //! Returns the V-multiplicities of the BSpline surface.
  [[nodiscard]] Standard_EXPORT const NCollection_Array1<int>& VMultiplicities() const;

protected:
  Standard_EXPORT Convert_ElementarySurfaceToBSplineSurface(const int theNbUPoles,
                                                            const int theNbVPoles,
                                                            const int theNbUKnots,
                                                            const int theNbVKnots,
                                                            const int theUDegree,
                                                            const int theVDegree);

  //! Resizes internal arrays (poles, weights, knots, multiplicities)
  //! to match the actual sizes stored in myNbUPoles, myNbVPoles,
  //! myNbUKnots, and myNbVKnots. This is intended to be called at the
  //! end of derived class constructors when the base class constructor
  //! allocates arrays with maximum possible sizes but the derived
  //! constructor uses fewer elements.
  Standard_EXPORT void Finalize();

protected:
  NCollection_Array2<gp_Pnt> myPoles;
  NCollection_Array2<double> myWeights;
  NCollection_Array1<double> myUKnots;
  NCollection_Array1<double> myVKnots;
  NCollection_Array1<int>    myUMults;
  NCollection_Array1<int>    myVMults;
  int                        myUDegree     = 0;
  int                        myVDegree     = 0;
  bool                       myIsUPeriodic = false;
  bool                       myIsVPeriodic = false;
  int                        myNbUPoles;
  int                        myNbVPoles;
  int                        myNbUKnots;
  int                        myNbVKnots;
};

#endif // _Convert_ElementarySurfaceToBSplineSurface_HeaderFile
