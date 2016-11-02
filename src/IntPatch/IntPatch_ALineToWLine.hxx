// Created on: 1993-11-26
// Created by: Modelistation
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

#ifndef _IntPatch_ALineToWLine_HeaderFile
#define _IntPatch_ALineToWLine_HeaderFile

#include <IntPatch_SequenceOfLine.hxx>
#include <IntSurf_Quadric.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Macro.hxx>

class Adaptor3d_HSurface;
class IntPatch_ALine;
class IntSurf_PntOn2S;

class IntPatch_ALineToWLine 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Constructor
  Standard_EXPORT IntPatch_ALineToWLine(const Handle(Adaptor3d_HSurface)& theS1,
                                        const Handle(Adaptor3d_HSurface)& theS2,
                                        const Standard_Integer theNbPoints = 200);
  
  Standard_EXPORT void SetTolOpenDomain (const Standard_Real aT);
  
  Standard_EXPORT Standard_Real TolOpenDomain() const;
  
  Standard_EXPORT void SetTolTransition (const Standard_Real aT);
  
  Standard_EXPORT Standard_Real TolTransition() const;
  
  Standard_EXPORT void SetTol3D (const Standard_Real aT);
  
  Standard_EXPORT Standard_Real Tol3D() const;
  
  //! Converts aline to the set of Walking-lines and adds
  //! them in theLines.
  Standard_EXPORT void MakeWLine (const Handle(IntPatch_ALine)& aline,
                                  IntPatch_SequenceOfLine& theLines) const;
  
  //! Converts aline (limitted by paraminf and paramsup) to the set of 
  //! Walking-lines and adds them in theLines.
  Standard_EXPORT void MakeWLine (const Handle(IntPatch_ALine)& aline,
                                  const Standard_Real paraminf,
                                  const Standard_Real paramsup,
                                  IntPatch_SequenceOfLine& theLines) const;

protected:
  //! Computes step value to construct point-line. The step depends on
  //! the local curvature of the intersection line computed in thePOn2S.
  //! theTgMagnitude is the magnitude of tangent vector to the intersection
  //! line (in the point thePOn2S).
  //! Computed step is always in the range [theStepMin, theStepMax].
  //! Returns FALSE if the step cannot be computed. In this case, its value
  //! will not be changed.
  Standard_EXPORT Standard_Boolean StepComputing(const Handle(IntPatch_ALine)& theALine,
                                                 const IntSurf_PntOn2S& thePOn2S,
                                                 const Standard_Real theLastParOfAline,
                                                 const Standard_Real theCurParam,
                                                 const Standard_Real theTgMagnitude,
                                                 const Standard_Real theStepMin,
                                                 const Standard_Real theStepMax,
                                                 const Standard_Real theMaxDeflection,
                                                 Standard_Real& theStep) const;


  Standard_EXPORT Standard_Integer CheckDeflection(const gp_XYZ& theMidPt,
                                                   const Standard_Real theMaxDeflection) const;



private:


  Handle(Adaptor3d_HSurface) myS1;
  Handle(Adaptor3d_HSurface) myS2;
  IntSurf_Quadric myQuad1;
  IntSurf_Quadric myQuad2;

  //! Approximate number of points in resulting
  //! WLine (precise number of points is computed
  //! by the algorithms)
  Standard_Integer myNbPointsInWline;
  Standard_Real myTolOpenDomain;
  Standard_Real myTolTransition;
  Standard_Real myTol3D;
};

#endif // _IntPatch_ALineToWLine_HeaderFile
