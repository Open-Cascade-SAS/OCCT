// Created: 1998-06-03 
// 
// Copyright (c) 1999-2013 OPEN CASCADE SAS 
// 
// This file is part of commercial software by OPEN CASCADE SAS, 
// furnished in accordance with the terms and conditions of the contract 
// and with the inclusion of this copyright notice. 
// This file or any part thereof may not be provided or otherwise 
// made available to any third party. 
// 
// No ownership title to the software is transferred hereby. 
// 
// OPEN CASCADE SAS makes no representation or warranties with respect to the 
// performance of this software, and specifically disclaims any responsibility 
// for any damages, special or consequential, connected with its use. 

#ifndef _GeomConvert_SurfToAnaSurf_HeaderFile
#define _GeomConvert_SurfToAnaSurf_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <GeomConvert_ConvType.hxx>
#include <GeomAbs_SurfaceType.hxx>
class Geom_Surface;


//! Converts a surface to the analitical form with given
//! precision. Conversion is done only the surface is bspline
//! of bezier and this can be approximed by some analytical
//! surface with that precision.
class GeomConvert_SurfToAnaSurf 
{
public:

  DEFINE_STANDARD_ALLOC
  
  Standard_EXPORT GeomConvert_SurfToAnaSurf();
  
  Standard_EXPORT GeomConvert_SurfToAnaSurf(const Handle(Geom_Surface)& S);
  
  Standard_EXPORT void Init (const Handle(Geom_Surface)& S);

  void SetConvType(const GeomConvert_ConvType theConvType = GeomConvert_Simplest)
  {
    myConvType = theConvType;
  }
  void SetTarget(const GeomAbs_SurfaceType theSurfType = GeomAbs_Plane)
  {
    myTarget = theSurfType;
  }

  //! Returns maximal deviation of converted surface from the original
  //! one computed by last call to ConvertToAnalytical
  Standard_Real Gap() const
  {
    return myGap;
  }
  
  //! Tries to convert the Surface to an Analytic form
  //! Returns the result
  //! In case of failure, returns a Null Handle
  //!
  Standard_EXPORT Handle(Geom_Surface) ConvertToAnalytical (const Standard_Real InitialToler);
  Standard_EXPORT Handle(Geom_Surface) ConvertToAnalytical (const Standard_Real InitialToler,
                                                            const Standard_Real Umin, const Standard_Real Umax,
                                                            const Standard_Real Vmin, const Standard_Real Vmax);
 
  //! Returns true if surfaces is same with the given tolerance
  Standard_EXPORT static Standard_Boolean IsSame (const Handle(Geom_Surface)& S1, const Handle(Geom_Surface)& S2, const Standard_Real tol);
  
  //! Returns true, if surface is canonical
  Standard_EXPORT static Standard_Boolean IsCanonical (const Handle(Geom_Surface)& S);


protected:

private:

  Handle(Geom_Surface) mySurf;
  Standard_Real myGap;
  GeomConvert_ConvType myConvType;
  GeomAbs_SurfaceType myTarget;

};


#endif // _GeomConvert_SurfToAnaSurf_HeaderFile
