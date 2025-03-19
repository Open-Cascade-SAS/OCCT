// Created on: 1993-06-17
// Created by: Jean Yves LEBEY
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

#ifndef _TopOpeBRepDS_CurveIterator_HeaderFile
#define _TopOpeBRepDS_CurveIterator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopOpeBRepDS_InterferenceIterator.hxx>
#include <TopOpeBRepDS_ListOfInterference.hxx>
#include <Standard_Integer.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_State.hxx>
class TopOpeBRepDS_Interference;
class Geom2d_Curve;

class TopOpeBRepDS_CurveIterator : public TopOpeBRepDS_InterferenceIterator
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an  iterator on the  curves on surface
  //! described by the interferences in <L>.
  Standard_EXPORT TopOpeBRepDS_CurveIterator(const TopOpeBRepDS_ListOfInterference& L);

  //! Returns  True if the Interference <I>  has a
  //! GeometryType() TopOpeBRepDS_CURVE
  //! returns False else.
  Standard_EXPORT virtual Standard_Boolean MatchInterference(
    const Handle(TopOpeBRepDS_Interference)& I) const Standard_OVERRIDE;

  //! Index of the curve in the data structure.
  Standard_EXPORT Standard_Integer Current() const;

  Standard_EXPORT TopAbs_Orientation Orientation(const TopAbs_State S) const;

  Standard_EXPORT const Handle(Geom2d_Curve)& PCurve() const;

protected:
private:
};

#endif // _TopOpeBRepDS_CurveIterator_HeaderFile
