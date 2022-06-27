// Created on: 1998-07-28
// Created by: LECLERE Florence
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

#ifndef _TopOpeBRepBuild_FuseFace_HeaderFile
#define _TopOpeBRepBuild_FuseFace_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopTools_ListOfShape.hxx>
#include <Standard_Boolean.hxx>


class TopOpeBRepBuild_FuseFace 
{
public:

  DEFINE_STANDARD_ALLOC

  
    TopOpeBRepBuild_FuseFace();
  
    TopOpeBRepBuild_FuseFace(const TopTools_ListOfShape& LIF, const TopTools_ListOfShape& LRF, const Standard_Integer CXM);
  
  Standard_EXPORT void Init (const TopTools_ListOfShape& LIF, const TopTools_ListOfShape& LRF, const Standard_Integer CXM);
  
  Standard_EXPORT void PerformFace();
  
  Standard_EXPORT void PerformEdge();
  
  Standard_EXPORT void ClearEdge();
  
  Standard_EXPORT void ClearVertex();
  
    Standard_Boolean IsDone() const;
  
    Standard_Boolean IsModified() const;
  
  const TopTools_ListOfShape& LFuseFace() const;
  
  const TopTools_ListOfShape& LInternEdge() const;
  
  const TopTools_ListOfShape& LExternEdge() const;
  
  const TopTools_ListOfShape& LModifEdge() const;
  
  const TopTools_ListOfShape& LInternVertex() const;
  
  const TopTools_ListOfShape& LExternVertex() const;
  
  const TopTools_ListOfShape& LModifVertex() const;




protected:



  TopTools_ListOfShape myLIE;
  TopTools_ListOfShape myLEE;
  TopTools_ListOfShape myLME;
  TopTools_ListOfShape myLIV;
  TopTools_ListOfShape myLEV;
  TopTools_ListOfShape myLMV;


private:



  TopTools_ListOfShape myLIF;
  TopTools_ListOfShape myLRF;
  TopTools_ListOfShape myLFF;
  Standard_Boolean myInternal;
  Standard_Boolean myModified;
  Standard_Boolean myDone;


};


#include <TopOpeBRepBuild_FuseFace.lxx>





#endif // _TopOpeBRepBuild_FuseFace_HeaderFile
