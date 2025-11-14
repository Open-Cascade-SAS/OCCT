// Created on: 1992-08-13
// Created by: Remi LEQUETTE
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

#ifndef _TopBas_TestInterference_HeaderFile
#define _TopBas_TestInterference_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <TopAbs_Orientation.hxx>

class TopBas_TestInterference
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopBas_TestInterference();

  Standard_EXPORT TopBas_TestInterference(const Standard_Real&     Inters,
                                          const Standard_Integer&  Bound,
                                          const TopAbs_Orientation Orient,
                                          const TopAbs_Orientation Trans,
                                          const TopAbs_Orientation BTrans);

  void Intersection(const Standard_Real& I);

  void Boundary(const Standard_Integer& B);

  void Orientation(const TopAbs_Orientation O);

  void Transition(const TopAbs_Orientation Tr);

  void BoundaryTransition(const TopAbs_Orientation BTr);

  const Standard_Real& Intersection() const;

  Standard_Real& ChangeIntersection();

  const Standard_Integer& Boundary() const;

  Standard_Integer& ChangeBoundary();

  TopAbs_Orientation Orientation() const;

  TopAbs_Orientation Transition() const;

  TopAbs_Orientation BoundaryTransition() const;

protected:
private:
  Standard_Real      myIntersection;
  Standard_Integer   myBoundary;
  TopAbs_Orientation myOrientation;
  TopAbs_Orientation myTransition;
  TopAbs_Orientation myBTransition;
};

//=================================================================================================
// Inline implementations
//=================================================================================================

inline void TopBas_TestInterference::Intersection(const Standard_Real& I)
{
  myIntersection = I;
}

//=================================================================================================

inline void TopBas_TestInterference::Boundary(const Standard_Integer& B)
{
  myBoundary = B;
}

//=================================================================================================

inline void TopBas_TestInterference::Orientation(const TopAbs_Orientation Or)
{
  myOrientation = Or;
}

//=================================================================================================

inline void TopBas_TestInterference::Transition(const TopAbs_Orientation Or)
{
  myTransition = Or;
}

//=================================================================================================

inline void TopBas_TestInterference::BoundaryTransition(const TopAbs_Orientation Or)
{
  myBTransition = Or;
}

//=================================================================================================

inline const Standard_Real& TopBas_TestInterference::Intersection() const
{
  return myIntersection;
}

//=================================================================================================

inline Standard_Real& TopBas_TestInterference::ChangeIntersection()
{
  return myIntersection;
}

//=================================================================================================

inline const Standard_Integer& TopBas_TestInterference::Boundary() const
{
  return myBoundary;
}

//=================================================================================================

inline Standard_Integer& TopBas_TestInterference::ChangeBoundary()
{
  return myBoundary;
}

//=================================================================================================

inline TopAbs_Orientation TopBas_TestInterference::Orientation() const
{
  return myOrientation;
}

//=================================================================================================

inline TopAbs_Orientation TopBas_TestInterference::Transition() const
{
  return myTransition;
}

//=================================================================================================

inline TopAbs_Orientation TopBas_TestInterference::BoundaryTransition() const
{
  return myBTransition;
}

#endif // _TopBas_TestInterference_HeaderFile
