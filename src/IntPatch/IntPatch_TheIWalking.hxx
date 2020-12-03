// Created on: 1992-05-06
// Created by: Jacques GOUSSARD
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

#ifndef _IntPatch_TheIWalking_HeaderFile
#define _IntPatch_TheIWalking_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <IntSurf_SequenceOfPathPoint.hxx>
#include <math_Vector.hxx>
#include <IntWalk_VectorOfWalkingData.hxx>
#include <IntWalk_VectorOfInteger.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir2d.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TColStd_DataMapOfIntegerListOfInteger.hxx>
#include <IntPatch_SequenceOfIWLineOfTheIWalking.hxx>
#include <IntSurf_SequenceOfInteriorPoint.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <IntWalk_StatusDeflection.hxx>
#include <Bnd_Range.hxx>

class IntSurf_PathPoint;
class IntSurf_PathPointTool;
class IntSurf_InteriorPoint;
class IntSurf_InteriorPointTool;
class Adaptor3d_HSurfaceTool;
class IntPatch_TheSurfFunction;
class IntPatch_TheIWLineOfTheIWalking;
class IntSurf_PntOn2S;
class math_FunctionSetRoot;

class IntPatch_TheIWalking 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Deflection is the maximum deflection admitted between two
  //! consecutive points on a resulting polyline.
  //! Step is the maximum increment admitted between two
  //! consecutive points (in 2d space).
  //! Epsilon is the tolerance beyond which 2 points
  //! are confused.
  //! theToFillHoles is the flag defining whether possible holes
  //! between resulting curves are filled or not
  //! in case of IntPatch walking theToFillHoles is False
  Standard_EXPORT IntPatch_TheIWalking(const Standard_Real Epsilon, const Standard_Real Deflection, const Standard_Real Step,
                                       const Standard_Boolean theToFillHoles = Standard_False);
  
  //! Deflection is the maximum deflection admitted between two
  //! consecutive points on a resulting polyline.
  //! Step is the maximum increment admitted between two
  //! consecutive points (in 2d space).
  //! Epsilon is the tolerance beyond which 2 points
  //! are confused
    void SetTolerance (const Standard_Real Epsilon, const Standard_Real Deflection, const Standard_Real Step);
  
  //! Searches a set of polylines starting on a point of Pnts1
  //! or Pnts2.
  //! Each point on a resulting polyline verifies F(u,v)=0
  Standard_EXPORT void Perform (const IntSurf_SequenceOfPathPoint& Pnts1, const IntSurf_SequenceOfInteriorPoint& Pnts2, IntPatch_TheSurfFunction& Func, const Handle(Adaptor3d_Surface)& S, const Standard_Boolean Reversed = Standard_False);
  
  //! Searches a set of polylines starting on a point of Pnts1.
  //! Each point on a resulting polyline verifies F(u,v)=0
  Standard_EXPORT void Perform (const IntSurf_SequenceOfPathPoint& Pnts1, IntPatch_TheSurfFunction& Func, const Handle(Adaptor3d_Surface)& S, const Standard_Boolean Reversed = Standard_False);
  
  //! Returns true if the calculus was successful.
    Standard_Boolean IsDone() const;
  
  //! Returns the number of resulting polylines.
  //! An exception is raised if IsDone returns False.
    Standard_Integer NbLines() const;
  
  //! Returns the polyline of range Index.
  //! An exception is raised if IsDone is False.
  //! An exception is raised if Index<=0 or Index>NbLines.
    const Handle(IntPatch_TheIWLineOfTheIWalking)& Value (const Standard_Integer Index) const;
  
  //! Returns the number of points belonging to Pnts on which no
  //! line starts or ends.
  //! An exception is raised if IsDone returns False.
    Standard_Integer NbSinglePnts() const;
  
  //! Returns the point of range Index .
  //! An exception is raised if IsDone returns False.
  //! An exception is raised if Index<=0 or
  //! Index > NbSinglePnts.
    const IntSurf_PathPoint& SinglePnt (const Standard_Integer Index) const;




protected:

  
  Standard_EXPORT Standard_Boolean Cadrage (math_Vector& BornInf, math_Vector& BornSup, math_Vector& UVap, Standard_Real& Step, const Standard_Integer StepSign) const;
  
  Standard_EXPORT Standard_Boolean TestArretPassage (const TColStd_SequenceOfReal& Umult, const TColStd_SequenceOfReal& Vmult, IntPatch_TheSurfFunction& Section, math_Vector& UV, Standard_Integer& Irang);
  
  Standard_EXPORT Standard_Boolean TestArretPassage (const TColStd_SequenceOfReal& Umult, const TColStd_SequenceOfReal& Vmult, const math_Vector& UV, const Standard_Integer Index, Standard_Integer& Irang);
  
  Standard_EXPORT Standard_Boolean TestArretAjout (IntPatch_TheSurfFunction& Section, math_Vector& UV, Standard_Integer& Irang, IntSurf_PntOn2S& PSol);
  
  Standard_EXPORT void FillPntsInHoles (IntPatch_TheSurfFunction& Section,
                                        TColStd_SequenceOfInteger& CopySeqAlone,
                                        IntSurf_SequenceOfInteriorPoint& PntsInHoles);
  
  Standard_EXPORT void TestArretCadre (const TColStd_SequenceOfReal& Umult, const TColStd_SequenceOfReal& Vmult, const Handle(IntPatch_TheIWLineOfTheIWalking)& Line, IntPatch_TheSurfFunction& Section, math_Vector& UV, Standard_Integer& Irang);
  
  Standard_EXPORT IntWalk_StatusDeflection TestDeflection (IntPatch_TheSurfFunction& Section, const Standard_Boolean Finished, const math_Vector& UV, const IntWalk_StatusDeflection StatusPrecedent, Standard_Integer& NbDivision, Standard_Real& Step, const Standard_Integer StepSign);
  
  Standard_EXPORT void ComputeOpenLine (const TColStd_SequenceOfReal& Umult, const TColStd_SequenceOfReal& Vmult, const IntSurf_SequenceOfPathPoint& Pnts1, IntPatch_TheSurfFunction& Section, Standard_Boolean& Rajout);
  
  Standard_EXPORT void OpenLine (const Standard_Integer N, const IntSurf_PntOn2S& Psol, const IntSurf_SequenceOfPathPoint& Pnts1, IntPatch_TheSurfFunction& Section, const Handle(IntPatch_TheIWLineOfTheIWalking)& Line);
  
  Standard_EXPORT Standard_Boolean IsValidEndPoint (const Standard_Integer IndOfPoint, const Standard_Integer IndOfLine);
  
  Standard_EXPORT void RemoveTwoEndPoints (const Standard_Integer IndOfPoint);
  
  Standard_EXPORT Standard_Boolean IsPointOnLine (const gp_Pnt2d& theP2d, const Standard_Integer Irang);
  
  Standard_EXPORT void ComputeCloseLine (const TColStd_SequenceOfReal& Umult, const TColStd_SequenceOfReal& Vmult, const IntSurf_SequenceOfPathPoint& Pnts1, const IntSurf_SequenceOfInteriorPoint& Pnts2, IntPatch_TheSurfFunction& Section, Standard_Boolean& Rajout);
  
  Standard_EXPORT void AddPointInCurrentLine (const Standard_Integer N, const IntSurf_PathPoint& PathPnt, const Handle(IntPatch_TheIWLineOfTheIWalking)& CurrentLine) const;
  
  Standard_EXPORT void MakeWalkingPoint (const Standard_Integer Case, const Standard_Real U, const Standard_Real V, IntPatch_TheSurfFunction& Section, IntSurf_PntOn2S& Psol);
  
  //! Clears up internal containers
  Standard_EXPORT void Clear();

  //! Returns TRUE if thePOn2S is in one of existing lines.
  Standard_EXPORT Standard_Boolean IsPointOnLine(const IntSurf_PntOn2S& thePOn2S,
                                                 const math_Vector& theInfBounds,
                                                 const math_Vector& theSupBounds,
                                                 math_FunctionSetRoot& theSolver,
                                                 IntPatch_TheSurfFunction& theFunc);



private:



  Standard_Boolean done;
  IntSurf_SequenceOfPathPoint seqSingle;
  Standard_Real fleche;
  Standard_Real pas;
  math_Vector tolerance;
  Standard_Real epsilon;
  Standard_Boolean reversed;
  IntWalk_VectorOfWalkingData wd1;
  IntWalk_VectorOfWalkingData wd2;
  IntWalk_VectorOfInteger nbMultiplicities;
  Bnd_Range mySRangeU; // Estimated U-range for section curve
  Bnd_Range mySRangeV; // Estimated V-range for section curve
  Standard_Real Um;
  Standard_Real UM;
  Standard_Real Vm;
  Standard_Real VM;
  IntSurf_PntOn2S previousPoint;
  gp_Vec previousd3d;
  gp_Dir2d previousd2d;
  TColStd_SequenceOfInteger seqAjout;
  TColStd_SequenceOfInteger seqAlone;
  TColStd_DataMapOfIntegerListOfInteger PointLineLine;
  IntPatch_SequenceOfIWLineOfTheIWalking lines;
  Standard_Boolean ToFillHoles;


};

#define ThePointOfPath IntSurf_PathPoint
#define ThePointOfPath_hxx <IntSurf_PathPoint.hxx>
#define ThePointOfPathTool IntSurf_PathPointTool
#define ThePointOfPathTool_hxx <IntSurf_PathPointTool.hxx>
#define ThePOPIterator IntSurf_SequenceOfPathPoint
#define ThePOPIterator_hxx <IntSurf_SequenceOfPathPoint.hxx>
#define ThePointOfLoop IntSurf_InteriorPoint
#define ThePointOfLoop_hxx <IntSurf_InteriorPoint.hxx>
#define ThePointOfLoopTool IntSurf_InteriorPointTool
#define ThePointOfLoopTool_hxx <IntSurf_InteriorPointTool.hxx>
#define ThePOLIterator IntSurf_SequenceOfInteriorPoint
#define ThePOLIterator_hxx <IntSurf_SequenceOfInteriorPoint.hxx>
#define ThePSurface Handle(Adaptor3d_Surface)
#define ThePSurface_hxx <Adaptor3d_Surface.hxx>
#define ThePSurfaceTool Adaptor3d_HSurfaceTool
#define ThePSurfaceTool_hxx <Adaptor3d_HSurfaceTool.hxx>
#define TheIWFunction IntPatch_TheSurfFunction
#define TheIWFunction_hxx <IntPatch_TheSurfFunction.hxx>
#define IntWalk_TheIWLine IntPatch_TheIWLineOfTheIWalking
#define IntWalk_TheIWLine_hxx <IntPatch_TheIWLineOfTheIWalking.hxx>
#define IntWalk_SequenceOfIWLine IntPatch_SequenceOfIWLineOfTheIWalking
#define IntWalk_SequenceOfIWLine_hxx <IntPatch_SequenceOfIWLineOfTheIWalking.hxx>
#define Handle_IntWalk_TheIWLine Handle(IntPatch_TheIWLineOfTheIWalking)
#define IntWalk_IWalking IntPatch_TheIWalking
#define IntWalk_IWalking_hxx <IntPatch_TheIWalking.hxx>

#include <IntWalk_IWalking.lxx>

#undef ThePointOfPath
#undef ThePointOfPath_hxx
#undef ThePointOfPathTool
#undef ThePointOfPathTool_hxx
#undef ThePOPIterator
#undef ThePOPIterator_hxx
#undef ThePointOfLoop
#undef ThePointOfLoop_hxx
#undef ThePointOfLoopTool
#undef ThePointOfLoopTool_hxx
#undef ThePOLIterator
#undef ThePOLIterator_hxx
#undef ThePSurface
#undef ThePSurface_hxx
#undef ThePSurfaceTool
#undef ThePSurfaceTool_hxx
#undef TheIWFunction
#undef TheIWFunction_hxx
#undef IntWalk_TheIWLine
#undef IntWalk_TheIWLine_hxx
#undef IntWalk_SequenceOfIWLine
#undef IntWalk_SequenceOfIWLine_hxx
#undef Handle_IntWalk_TheIWLine
#undef IntWalk_IWalking
#undef IntWalk_IWalking_hxx




#endif // _IntPatch_TheIWalking_HeaderFile
