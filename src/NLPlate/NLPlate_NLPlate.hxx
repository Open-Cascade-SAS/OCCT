// Created on: 1998-04-09
// Created by: Andre LIEUTIER
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

#ifndef _NLPlate_NLPlate_HeaderFile
#define _NLPlate_NLPlate_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <NLPlate_SequenceOfHGPPConstraint.hxx>
#include <NLPlate_StackOfPlate.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
class Geom_Surface;
class NLPlate_HGPPConstraint;
class gp_XYZ;
class gp_XY;



class NLPlate_NLPlate 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT NLPlate_NLPlate(const Handle(Geom_Surface)& InitialSurface);
  
  Standard_EXPORT void Load (const Handle(NLPlate_HGPPConstraint)& GConst);
  
  Standard_EXPORT void Solve (const Standard_Integer ord = 2, const Standard_Integer InitialConsraintOrder = 1);
  
  Standard_EXPORT void Solve2 (const Standard_Integer ord = 2, const Standard_Integer InitialConsraintOrder = 1);
  
  Standard_EXPORT void IncrementalSolve (const Standard_Integer ord = 2, const Standard_Integer InitialConsraintOrder = 1, const Standard_Integer NbIncrements = 4, const Standard_Boolean UVSliding = Standard_False);
  
  //! returns True if all has been correctly done.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  Standard_EXPORT void destroy();
~NLPlate_NLPlate()
{
  destroy();
}
  
  //! reset the Plate in the initial state
  //! ( same as after Create((Surface))
  Standard_EXPORT void Init();
  
  Standard_EXPORT gp_XYZ Evaluate (const gp_XY& point2d) const;
  
  Standard_EXPORT gp_XYZ EvaluateDerivative (const gp_XY& point2d, const Standard_Integer iu, const Standard_Integer iv) const;
  
  Standard_EXPORT Standard_Integer Continuity() const;
  
  Standard_EXPORT void ConstraintsSliding (const Standard_Integer NbIterations = 3);
  
  Standard_EXPORT Standard_Integer MaxActiveConstraintOrder() const;




protected:





private:

  
  Standard_EXPORT Standard_Boolean Iterate (const Standard_Integer ConstraintOrder, const Standard_Integer ResolutionOrder, const Standard_Real IncrementalLoading = 1.0);


  Handle(Geom_Surface) myInitialSurface;
  NLPlate_SequenceOfHGPPConstraint myHGPPConstraints;
  NLPlate_StackOfPlate mySOP;
  Standard_Boolean OK;


};







#endif // _NLPlate_NLPlate_HeaderFile
