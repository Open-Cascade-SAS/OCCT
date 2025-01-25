// Created on: 1993-06-24
// Created by: Jean Yves LEBEY
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

#ifndef _TopOpeBRepTool_GeomTool_HeaderFile
#define _TopOpeBRepTool_GeomTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopOpeBRepTool_OutCurveType.hxx>
#include <Standard_Integer.hxx>

class TopOpeBRepTool_GeomTool
{
public:
  DEFINE_STANDARD_ALLOC

  //! Boolean flags <CompC3D>, <CompPC1>, <CompPC2>
  //! indicate whether  the  corresponding result curves
  //! <C3D>, <PC1>, <PC2> of MakeCurves method  must or not
  //! be computed from an intersection line <L>.
  //! When  the line <L> is a walking one, <TypeC3D> is the
  //! kind  of the 3D curve <C3D>  to  compute  :
  //! - BSPLINE1 to compute  a BSpline of  degree 1 on the
  //! walking   points  of  <L>,
  //! - APPROX  to build  an  approximation curve on the
  //! walking points of <L>.
  Standard_EXPORT TopOpeBRepTool_GeomTool(
    const TopOpeBRepTool_OutCurveType TypeC3D = TopOpeBRepTool_BSPLINE1,
    const Standard_Boolean            CompC3D = Standard_True,
    const Standard_Boolean            CompPC1 = Standard_True,
    const Standard_Boolean            CompPC2 = Standard_True);

  Standard_EXPORT void Define(const TopOpeBRepTool_OutCurveType TypeC3D,
                              const Standard_Boolean            CompC3D,
                              const Standard_Boolean            CompPC1,
                              const Standard_Boolean            CompPC2);

  Standard_EXPORT void Define(const TopOpeBRepTool_OutCurveType TypeC3D);

  Standard_EXPORT void DefineCurves(const Standard_Boolean CompC3D);

  Standard_EXPORT void DefinePCurves1(const Standard_Boolean CompPC1);

  Standard_EXPORT void DefinePCurves2(const Standard_Boolean CompPC2);

  Standard_EXPORT void Define(const TopOpeBRepTool_GeomTool& GT);

  Standard_EXPORT void GetTolerances(Standard_Real& tol3d, Standard_Real& tol2d) const;

  Standard_EXPORT void SetTolerances(const Standard_Real tol3d, const Standard_Real tol2d);

  Standard_EXPORT Standard_Integer NbPntMax() const;

  Standard_EXPORT void SetNbPntMax(const Standard_Integer NbPntMax);

  Standard_EXPORT TopOpeBRepTool_OutCurveType TypeC3D() const;

  Standard_EXPORT Standard_Boolean CompC3D() const;

  Standard_EXPORT Standard_Boolean CompPC1() const;

  Standard_EXPORT Standard_Boolean CompPC2() const;

protected:
  TopOpeBRepTool_OutCurveType myTypeC3D;
  Standard_Boolean            myCompC3D;
  Standard_Boolean            myCompPC1;
  Standard_Boolean            myCompPC2;

private:
  Standard_Real    myTol3d;
  Standard_Real    myTol2d;
  Standard_Integer myNbPntMax;
};

#endif // _TopOpeBRepTool_GeomTool_HeaderFile
