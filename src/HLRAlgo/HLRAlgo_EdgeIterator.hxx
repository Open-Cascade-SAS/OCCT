// Created on: 1993-01-11
// Created by: Christophe MARION
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

#ifndef _HLRAlgo_EdgeIterator_HeaderFile
#define _HLRAlgo_EdgeIterator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_ShortReal.hxx>
#include <Standard_Boolean.hxx>
class HLRAlgo_EdgeStatus;



class HLRAlgo_EdgeIterator 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Iterator  on the  visible or  hidden  parts of  an
  //! edge.
  Standard_EXPORT HLRAlgo_EdgeIterator();
  
  Standard_EXPORT void InitHidden (HLRAlgo_EdgeStatus& status);
  
    Standard_Boolean MoreHidden() const;
  
  Standard_EXPORT void NextHidden();
  
  //! Returns the bounds and the tolerances
  //! of the current Hidden Interval
    void Hidden (Standard_Real& Start, Standard_ShortReal& TolStart, Standard_Real& End, Standard_ShortReal& TolEnd) const;
  
    void InitVisible (HLRAlgo_EdgeStatus& status);
  
    Standard_Boolean MoreVisible() const;
  
    void NextVisible();
  
  //! Returns the bounds and the tolerances
  //! of the current Visible Interval
    void Visible (Standard_Real& Start, Standard_ShortReal& TolStart, Standard_Real& End, Standard_ShortReal& TolEnd);




protected:





private:



  Standard_Integer myNbVis;
  Standard_Integer myNbHid;
  HLRAlgo_EdgeStatus* EVis;
  HLRAlgo_EdgeStatus* EHid;
  Standard_Integer iVis;
  Standard_Integer iHid;
  Standard_Real myHidStart;
  Standard_Real myHidEnd;
  Standard_ShortReal myHidTolStart;
  Standard_ShortReal myHidTolEnd;


};


#include <HLRAlgo_EdgeIterator.lxx>





#endif // _HLRAlgo_EdgeIterator_HeaderFile
