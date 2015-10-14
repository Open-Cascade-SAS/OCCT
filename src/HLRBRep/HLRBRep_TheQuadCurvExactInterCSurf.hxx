// Created on: 1992-10-14
// Created by: Christophe MARION
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

#ifndef _HLRBRep_TheQuadCurvExactInterCSurf_HeaderFile
#define _HLRBRep_TheQuadCurvExactInterCSurf_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <Standard_Address.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class HLRBRep_SurfaceTool;
class gp_Lin;
class HLRBRep_LineTool;
class HLRBRep_TheQuadCurvFuncOfTheQuadCurvExactInterCSurf;



class HLRBRep_TheQuadCurvExactInterCSurf 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Provides the signed distance function : Q(w)
  //! and its first derivative dQ(w)/dw
  Standard_EXPORT HLRBRep_TheQuadCurvExactInterCSurf(const Standard_Address& S, const gp_Lin& C);
  
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  Standard_EXPORT Standard_Integer NbRoots() const;
  
  Standard_EXPORT Standard_Real Root (const Standard_Integer Index) const;
  
  Standard_EXPORT Standard_Integer NbIntervals() const;
  
  //! U1 and U2 are the parameters of
  //! a segment on the curve.
  Standard_EXPORT void Intervals (const Standard_Integer Index, Standard_Real& U1, Standard_Real& U2) const;




protected:





private:



  Standard_Integer nbpnts;
  TColStd_SequenceOfReal pnts;
  Standard_Integer nbintv;
  TColStd_SequenceOfReal intv;


};







#endif // _HLRBRep_TheQuadCurvExactInterCSurf_HeaderFile
