// Created on: 1998-03-26
// Created by: Robert COUBLANC
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

#ifndef _SelectMgr_SortCriterion_HeaderFile
#define _SelectMgr_SortCriterion_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>


//! This class provides data and criterion for sorting candidate
//! entities in the process of interactive selection by mouse click
class SelectMgr_SortCriterion 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT SelectMgr_SortCriterion();
  
  //! Defines parameters of selection criterion:
  //! - Priority: selection priority
  //! - Depth: distance from the view plane to the entity
  //! - MinDist: distance from the clicked point to the entity on the view plane
  //! - Tol: tolerance used for selecting candidates
  //! - PreferClosest: specify whether closest object is preferred even if
  //! if has less priority
  Standard_EXPORT SelectMgr_SortCriterion(const Standard_Integer thePriority, const Standard_Real theDepth, const Standard_Real theMinDist, const Standard_Real theTol, const Standard_Boolean PreferClosest);
  
    void SetPriority (const Standard_Integer P);
  
    void SetDepth (const Standard_Real D);
  
    void SetMinDist (const Standard_Real D);
  
    void SetTol (const Standard_Real T);
  
    Standard_Integer Priority() const;
  
    Standard_Real Depth() const;
  
    Standard_Real MinDist() const;
  
    Standard_Real Tol() const;
  
  Standard_EXPORT Standard_Boolean IsGreater (const SelectMgr_SortCriterion& anOtherCriterion) const;
Standard_Boolean operator > (const SelectMgr_SortCriterion& anOtherCriterion) const
{
  return IsGreater(anOtherCriterion);
}
  
  Standard_EXPORT Standard_Boolean IsLower (const SelectMgr_SortCriterion& anOtherCriterion) const;
Standard_Boolean operator < (const SelectMgr_SortCriterion& anOtherCriterion) const
{
  return IsLower(anOtherCriterion);
}




protected:





private:



  Standard_Integer myPrior;
  Standard_Real myDepth;
  Standard_Real myDist;
  Standard_Real myTol;
  Standard_Boolean myPreferClosest;


};


#include <SelectMgr_SortCriterion.lxx>





#endif // _SelectMgr_SortCriterion_HeaderFile
