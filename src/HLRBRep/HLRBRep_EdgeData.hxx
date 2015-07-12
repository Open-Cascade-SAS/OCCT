// Created on: 1997-04-17
// Created by: Christophe MARION
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

#ifndef _HLRBRep_EdgeData_HeaderFile
#define _HLRBRep_EdgeData_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <HLRAlgo_EdgeStatus.hxx>
#include <HLRBRep_Curve.hxx>
#include <Standard_ShortReal.hxx>
#include <Standard_Real.hxx>
#include <Standard_Address.hxx>
class TopoDS_Edge;
class HLRAlgo_EdgeStatus;
class HLRBRep_Curve;



class HLRBRep_EdgeData 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT HLRBRep_EdgeData();
  
  Standard_EXPORT void Set (const Standard_Boolean Reg1, const Standard_Boolean RegN, const TopoDS_Edge& EG, const Standard_Integer V1, const Standard_Integer V2, const Standard_Boolean Out1, const Standard_Boolean Out2, const Standard_Boolean Cut1, const Standard_Boolean Cut2, const Standard_Real Start, const Standard_ShortReal TolStart, const Standard_Real End, const Standard_ShortReal TolEnd);
  
    Standard_Boolean Selected() const;
  
    void Selected (const Standard_Boolean B);
  
    Standard_Boolean Rg1Line() const;
  
    void Rg1Line (const Standard_Boolean B);
  
    Standard_Boolean RgNLine() const;
  
    void RgNLine (const Standard_Boolean B);
  
    Standard_Boolean Vertical() const;
  
    void Vertical (const Standard_Boolean B);
  
    Standard_Boolean Simple() const;
  
    void Simple (const Standard_Boolean B);
  
    Standard_Boolean OutLVSta() const;
  
    void OutLVSta (const Standard_Boolean B);
  
    Standard_Boolean OutLVEnd() const;
  
    void OutLVEnd (const Standard_Boolean B);
  
    Standard_Boolean CutAtSta() const;
  
    void CutAtSta (const Standard_Boolean B);
  
    Standard_Boolean CutAtEnd() const;
  
    void CutAtEnd (const Standard_Boolean B);
  
    Standard_Boolean VerAtSta() const;
  
    void VerAtSta (const Standard_Boolean B);
  
    Standard_Boolean VerAtEnd() const;
  
    void VerAtEnd (const Standard_Boolean B);
  
    Standard_Boolean AutoIntersectionDone() const;
  
    void AutoIntersectionDone (const Standard_Boolean B);
  
    Standard_Boolean Used() const;
  
    void Used (const Standard_Boolean B);
  
    Standard_Integer HideCount() const;
  
    void HideCount (const Standard_Integer I);
  
    Standard_Integer VSta() const;
  
    void VSta (const Standard_Integer I);
  
    Standard_Integer VEnd() const;
  
    void VEnd (const Standard_Integer I);
  
  Standard_EXPORT void UpdateMinMax (const Standard_Address TotMinMax);
  
    Standard_Address MinMax() const;
  
    HLRAlgo_EdgeStatus& Status();
  
    HLRBRep_Curve& ChangeGeometry();
  
    const HLRBRep_Curve& Geometry() const;
  
    Standard_Address Curve();
  
    Standard_ShortReal Tolerance() const;




protected:





private:



  Standard_Boolean myFlags;
  Standard_Integer myHideCount;
  Standard_Integer myVSta;
  Standard_Integer myVEnd;
  Standard_Integer myMinMax[16];
  HLRAlgo_EdgeStatus myStatus;
  HLRBRep_Curve myGeometry;
  Standard_ShortReal myTolerance;


};


#include <HLRBRep_EdgeData.lxx>





#endif // _HLRBRep_EdgeData_HeaderFile
