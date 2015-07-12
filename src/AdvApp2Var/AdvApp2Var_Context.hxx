// Created on: 1996-04-16
// Created by: Joelle CHAUVET
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _AdvApp2Var_Context_HeaderFile
#define _AdvApp2Var_Context_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>



//! contains  all the  parameters  for approximation
//! ( tolerancy, computing option, ...)
class AdvApp2Var_Context 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT AdvApp2Var_Context();
  
  Standard_EXPORT AdvApp2Var_Context(const Standard_Integer ifav, const Standard_Integer iu, const Standard_Integer iv, const Standard_Integer nlimu, const Standard_Integer nlimv, const Standard_Integer iprecis, const Standard_Integer nb1Dss, const Standard_Integer nb2Dss, const Standard_Integer nb3Dss, const Handle(TColStd_HArray1OfReal)& tol1D, const Handle(TColStd_HArray1OfReal)& tol2D, const Handle(TColStd_HArray1OfReal)& tol3D, const Handle(TColStd_HArray2OfReal)& tof1D, const Handle(TColStd_HArray2OfReal)& tof2D, const Handle(TColStd_HArray2OfReal)& tof3D);
  
  Standard_EXPORT Standard_Integer TotalDimension() const;
  
  Standard_EXPORT Standard_Integer TotalNumberSSP() const;
  
  Standard_EXPORT Standard_Integer FavorIso() const;
  
  Standard_EXPORT Standard_Integer UOrder() const;
  
  Standard_EXPORT Standard_Integer VOrder() const;
  
  Standard_EXPORT Standard_Integer ULimit() const;
  
  Standard_EXPORT Standard_Integer VLimit() const;
  
  Standard_EXPORT Standard_Integer UJacDeg() const;
  
  Standard_EXPORT Standard_Integer VJacDeg() const;
  
  Standard_EXPORT Handle(TColStd_HArray1OfReal) UJacMax() const;
  
  Standard_EXPORT Handle(TColStd_HArray1OfReal) VJacMax() const;
  
  Standard_EXPORT Handle(TColStd_HArray1OfReal) URoots() const;
  
  Standard_EXPORT Handle(TColStd_HArray1OfReal) VRoots() const;
  
  Standard_EXPORT Handle(TColStd_HArray1OfReal) UGauss() const;
  
  Standard_EXPORT Handle(TColStd_HArray1OfReal) VGauss() const;
  
  Standard_EXPORT Handle(TColStd_HArray1OfReal) IToler() const;
  
  Standard_EXPORT Handle(TColStd_HArray2OfReal) FToler() const;
  
  Standard_EXPORT Handle(TColStd_HArray2OfReal) CToler() const;




protected:





private:



  Standard_Integer myFav;
  Standard_Integer myOrdU;
  Standard_Integer myOrdV;
  Standard_Integer myLimU;
  Standard_Integer myLimV;
  Standard_Integer myNb1DSS;
  Standard_Integer myNb2DSS;
  Standard_Integer myNb3DSS;
  Standard_Integer myNbURoot;
  Standard_Integer myNbVRoot;
  Standard_Integer myJDegU;
  Standard_Integer myJDegV;
  Handle(TColStd_HArray1OfReal) myJMaxU;
  Handle(TColStd_HArray1OfReal) myJMaxV;
  Handle(TColStd_HArray1OfReal) myURoots;
  Handle(TColStd_HArray1OfReal) myVRoots;
  Handle(TColStd_HArray1OfReal) myUGauss;
  Handle(TColStd_HArray1OfReal) myVGauss;
  Handle(TColStd_HArray1OfReal) myInternalTol;
  Handle(TColStd_HArray2OfReal) myFrontierTol;
  Handle(TColStd_HArray2OfReal) myCuttingTol;


};







#endif // _AdvApp2Var_Context_HeaderFile
