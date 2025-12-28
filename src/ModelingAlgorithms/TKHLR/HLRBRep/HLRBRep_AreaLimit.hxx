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

#ifndef _HLRBRep_AreaLimit_HeaderFile
#define _HLRBRep_AreaLimit_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <HLRAlgo_Intersection.hxx>
#include <TopAbs_State.hxx>
#include <Standard_Transient.hxx>

//! The private nested class AreaLimit represents a
//! vertex on the Edge with the state on the left and
//! the right.
class HLRBRep_AreaLimit : public Standard_Transient
{

public:
  //! The previous and next field are set to NULL.
  Standard_EXPORT HLRBRep_AreaLimit(const HLRAlgo_Intersection& V,
                                    const bool      Boundary,
                                    const bool      Interference,
                                    const TopAbs_State          StateBefore,
                                    const TopAbs_State          StateAfter,
                                    const TopAbs_State          EdgeBefore,
                                    const TopAbs_State          EdgeAfter);

  Standard_EXPORT void StateBefore(const TopAbs_State St);

  Standard_EXPORT void StateAfter(const TopAbs_State St);

  Standard_EXPORT void EdgeBefore(const TopAbs_State St);

  Standard_EXPORT void EdgeAfter(const TopAbs_State St);

  Standard_EXPORT void Previous(const occ::handle<HLRBRep_AreaLimit>& P);

  Standard_EXPORT void Next(const occ::handle<HLRBRep_AreaLimit>& N);

  Standard_EXPORT const HLRAlgo_Intersection& Vertex() const;

  Standard_EXPORT bool IsBoundary() const;

  Standard_EXPORT bool IsInterference() const;

  Standard_EXPORT TopAbs_State StateBefore() const;

  Standard_EXPORT TopAbs_State StateAfter() const;

  Standard_EXPORT TopAbs_State EdgeBefore() const;

  Standard_EXPORT TopAbs_State EdgeAfter() const;

  Standard_EXPORT occ::handle<HLRBRep_AreaLimit> Previous() const;

  Standard_EXPORT occ::handle<HLRBRep_AreaLimit> Next() const;

  Standard_EXPORT void Clear();

  DEFINE_STANDARD_RTTIEXT(HLRBRep_AreaLimit, Standard_Transient)

private:
  HLRAlgo_Intersection      myVertex;
  bool          myBoundary;
  bool          myInterference;
  TopAbs_State              myStateBefore;
  TopAbs_State              myStateAfter;
  TopAbs_State              myEdgeBefore;
  TopAbs_State              myEdgeAfter;
  occ::handle<HLRBRep_AreaLimit> myPrevious;
  occ::handle<HLRBRep_AreaLimit> myNext;
};

#endif // _HLRBRep_AreaLimit_HeaderFile
