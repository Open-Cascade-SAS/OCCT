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

#ifndef _HLRBRep_EdgeInterferenceTool_HeaderFile
#define _HLRBRep_EdgeInterferenceTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <HLRAlgo_Intersection.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <TopAbs_Orientation.hxx>
#include <Standard_Real.hxx>
class HLRBRep_Data;
class HLRAlgo_Intersection;
class gp_Dir;
class HLRAlgo_Interference;


//! Implements the  methods required  to  instantiates
//! the EdgeInterferenceList from HLRAlgo.
class HLRBRep_EdgeInterferenceTool 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT HLRBRep_EdgeInterferenceTool(const Handle(HLRBRep_Data)& DS);
  
  Standard_EXPORT void LoadEdge();
  
    void InitVertices();
  
    Standard_Boolean MoreVertices() const;
  
    void NextVertex();
  
    const HLRAlgo_Intersection& CurrentVertex() const;
  
    TopAbs_Orientation CurrentOrientation() const;
  
    Standard_Real CurrentParameter() const;
  
    Standard_Boolean IsPeriodic() const;
  
  //! Returns local geometric description of the Edge at
  //! parameter   <Para>.  See  method  Reset  of  class
  //! EdgeFaceTransition from TopCnx for other arguments.
  Standard_EXPORT void EdgeGeometry (const Standard_Real Param, gp_Dir& Tgt, gp_Dir& Nrm, Standard_Real& Curv) const;
  
    Standard_Real ParameterOfInterference (const HLRAlgo_Interference& I) const;
  
  //! True if   the  two interferences are on   the same
  //! geometric locus.
  Standard_EXPORT Standard_Boolean SameInterferences (const HLRAlgo_Interference& I1, const HLRAlgo_Interference& I2) const;
  
  //! True if the  Interference and the  current  Vertex
  //! are on the same geometric locus.
  Standard_EXPORT Standard_Boolean SameVertexAndInterference (const HLRAlgo_Interference& I) const;
  
  //! Returns   the  geometry of  the   boundary  at the
  //! interference  <I>.  See the AddInterference method
  //! of the class  EdgeFaceTransition  from  TopCnx for
  //! the other arguments.
  Standard_EXPORT void InterferenceBoundaryGeometry (const HLRAlgo_Interference& I, gp_Dir& Tang, gp_Dir& Norm, Standard_Real& Curv) const;




protected:





private:



  Handle(HLRBRep_Data) myDS;
  HLRAlgo_Intersection inter[2];
  Standard_Integer cur;


};


#include <HLRBRep_EdgeInterferenceTool.lxx>





#endif // _HLRBRep_EdgeInterferenceTool_HeaderFile
