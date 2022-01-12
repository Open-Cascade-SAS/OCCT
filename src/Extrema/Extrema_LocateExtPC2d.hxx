// Created on: 1991-02-26
// Created by: Isabelle GRIGNON
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

#ifndef _Extrema_LocateExtPC2d_HeaderFile
#define _Extrema_LocateExtPC2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Extrema_POnCurv2d.hxx>
#include <Standard_Boolean.hxx>
#include <Extrema_LocEPCOfLocateExtPC2d.hxx>
#include <Extrema_ELPCOfLocateExtPC2d.hxx>
#include <GeomAbs_CurveType.hxx>
class Standard_DomainError;
class StdFail_NotDone;
class Adaptor2d_Curve2d;
class Extrema_Curve2dTool;
class Extrema_ExtPElC2d;
class gp_Pnt2d;
class gp_Vec2d;
class Extrema_POnCurv2d;
class Extrema_ELPCOfLocateExtPC2d;
class Extrema_EPCOfELPCOfLocateExtPC2d;
class Extrema_LocEPCOfLocateExtPC2d;
class Extrema_PCLocFOfLocEPCOfLocateExtPC2d;



class Extrema_LocateExtPC2d 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Extrema_LocateExtPC2d();
  
  //! Calculates the distance with a close point.
  //! The close point is defined by the parameter value
  //! U0.
  //! The function F(u)=distance(P,C(u)) has an extremum
  //! when g(u)=dF/du=0. The algorithm searches a zero
  //! near the close point.
  //! TolF is used to decide to stop the iterations.
  //! At the nth iteration, the criteria is:
  //! abs(Un - Un-1) < TolF.
  Standard_EXPORT Extrema_LocateExtPC2d(const gp_Pnt2d& P, const Adaptor2d_Curve2d& C, const Standard_Real U0, const Standard_Real TolF);
  
  //! Calculates the distance with a close point.
  //! The close point is defined by the parameter value
  //! U0.
  //! The function F(u)=distance(P,C(u)) has an extremum
  //! when g(u)=dF/du=0. The algorithm searches a zero
  //! near the close point.
  //! Zeros are searched between Umin et Usup.
  //! TolF is used to decide to stop the iterations.
  //! At the nth iteration, the criteria is:
  //! abs(Un - Un-1) < TolF.
  Standard_EXPORT Extrema_LocateExtPC2d(const gp_Pnt2d& P, const Adaptor2d_Curve2d& C, const Standard_Real U0, const Standard_Real Umin, const Standard_Real Usup, const Standard_Real TolF);
  
  //! sets the fields of the algorithm.
  Standard_EXPORT void Initialize (const Adaptor2d_Curve2d& C, const Standard_Real Umin, const Standard_Real Usup, const Standard_Real TolF);
  
  Standard_EXPORT void Perform (const gp_Pnt2d& P, const Standard_Real U0);
  
  //! Returns True if the distance is found.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns the value of the extremum square distance.
  Standard_EXPORT Standard_Real SquareDistance() const;
  
  //! Returns True if the extremum distance is a minimum.
  Standard_EXPORT Standard_Boolean IsMin() const;
  
  //! Returns the point of the extremum distance.
  Standard_EXPORT const Extrema_POnCurv2d& Point() const;




protected:





private:



  Extrema_POnCurv2d mypp;
  Standard_Address myC;
  Standard_Real mydist2;
  Standard_Boolean myismin;
  Standard_Boolean myDone;
  Standard_Real myumin;
  Standard_Real myusup;
  Standard_Real mytol;
  Extrema_LocEPCOfLocateExtPC2d myLocExtPC;
  Extrema_ELPCOfLocateExtPC2d myExtremPC;
  GeomAbs_CurveType type;
  Standard_Integer numberext;


};







#endif // _Extrema_LocateExtPC2d_HeaderFile
