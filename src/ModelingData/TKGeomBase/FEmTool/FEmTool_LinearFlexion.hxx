// Created on: 1997-09-18
// Created by: Philippe MANGIN
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _FEmTool_LinearFlexion_HeaderFile
#define _FEmTool_LinearFlexion_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <math_Matrix.hxx>
#include <Standard_Integer.hxx>
#include <FEmTool_ElementaryCriterion.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <math_Vector.hxx>

//! Criterium of LinearFlexion To Hermit-Jacobi elements
class FEmTool_LinearFlexion : public FEmTool_ElementaryCriterion
{

public:
  Standard_EXPORT FEmTool_LinearFlexion(const int WorkDegree,
                                        const GeomAbs_Shape    ConstraintOrder);

  Standard_EXPORT virtual occ::handle<NCollection_HArray2<int>> DependenceTable() const
    override;

  Standard_EXPORT virtual double Value() override;

  Standard_EXPORT virtual void Hessian(const int Dimension1,
                                       const int Dimension2,
                                       math_Matrix&           H) override;

  Standard_EXPORT virtual void Gradient(const int Dimension,
                                        math_Vector&           G) override;

  DEFINE_STANDARD_RTTIEXT(FEmTool_LinearFlexion, FEmTool_ElementaryCriterion)

private:
  math_Matrix      RefMatrix;
  int myOrder;
};

#endif // _FEmTool_LinearFlexion_HeaderFile
