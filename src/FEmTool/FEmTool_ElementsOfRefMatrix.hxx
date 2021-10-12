// Created on: 1998-11-10
// Created by: Igor FEOKTISTOV
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

#ifndef _FEmTool_ElementsOfRefMatrix_HeaderFile
#define _FEmTool_ElementsOfRefMatrix_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <math_FunctionSet.hxx>
#include <Standard_Boolean.hxx>
#include <math_Vector.hxx>
class PLib_Base;


//! this  class  describes  the  functions  needed  for
//! calculating  matrix  elements  of  RefMatrix  for  linear
//! criteriums  (Tension,  Flexsion  and  Jerk) by  Gauss  integration.
//! Each  function  from  set  gives  value  Pi(u)'*Pj(u)'  or
//! Pi(u)''*Pj(u)''  or  Pi(u)'''*Pj(u)'''  for  each  i  and  j,
//! where  Pi(u)  is  i-th  basis  function  of  expansion  and
//! (')  means  derivative.
class FEmTool_ElementsOfRefMatrix  : public math_FunctionSet
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT FEmTool_ElementsOfRefMatrix(const Handle(PLib_Base)& TheBase, const Standard_Integer DerOrder);
  
  //! returns the number of variables of the function.
  //! It  is  supposed  that  NbVariables  =  1.
  Standard_EXPORT Standard_Integer NbVariables() const;
  
  //! returns the number of equations of the function.
  Standard_EXPORT Standard_Integer NbEquations() const;
  
  //! computes the values <F> of the functions for the
  //! variable <X>.
  //! returns True if the computation was done successfully,
  //! False otherwise.
  //! F  contains  results  only  for  i<=j  in  following  order:
  //! P0*P0,  P0*P1,  P0*P2...  P1*P1,  P1*P2,...  (upper  triangle of
  //! matrix  {PiPj})
  Standard_EXPORT Standard_Boolean Value (const math_Vector& X, math_Vector& F);




protected:





private:



  Handle(PLib_Base) myBase;
  Standard_Integer myDerOrder;
  Standard_Integer myNbEquations;


};







#endif // _FEmTool_ElementsOfRefMatrix_HeaderFile
