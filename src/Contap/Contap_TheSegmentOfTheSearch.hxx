// Created on: 1993-02-05
// Created by: Jacques GOUSSARD
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

#ifndef _Contap_TheSegmentOfTheSearch_HeaderFile
#define _Contap_TheSegmentOfTheSearch_HeaderFile

#include <Adaptor2d_Curve2d.hxx>
#include <Contap_ThePathPointOfTheSearch.hxx>

class Standard_DomainError;
class Adaptor3d_HVertex;
class Contap_ThePathPointOfTheSearch;

class Contap_TheSegmentOfTheSearch
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor.
  Standard_EXPORT Contap_TheSegmentOfTheSearch();

  //! Defines the concerned arc.
  void SetValue(const Handle(Adaptor2d_Curve2d)& A);

  //! Defines the first point or the last point,
  //! depending on the value of the boolean First.
  Standard_EXPORT void SetLimitPoint(const Contap_ThePathPointOfTheSearch& V,
                                     const Standard_Boolean                First);

  //! Returns the geometric curve on the surface 's domain
  //! which is solution.
  const Handle(Adaptor2d_Curve2d)& Curve() const;

  //! Returns True if there is a vertex (ThePathPoint) defining
  //! the lowest valid parameter on the arc.
  Standard_Boolean HasFirstPoint() const;

  //! Returns the first point.
  const Contap_ThePathPointOfTheSearch& FirstPoint() const;

  //! Returns True if there is a vertex (ThePathPoint) defining
  //! the greatest valid parameter on the arc.
  Standard_Boolean HasLastPoint() const;

  //! Returns the last point.
  const Contap_ThePathPointOfTheSearch& LastPoint() const;

protected:
private:
  Handle(Adaptor2d_Curve2d)      arc;
  Standard_Boolean               hasfp;
  Contap_ThePathPointOfTheSearch thefp;
  Standard_Boolean               haslp;
  Contap_ThePathPointOfTheSearch thelp;
};

#define TheVertex Handle(Adaptor3d_HVertex)
#define TheVertex_hxx <Adaptor3d_HVertex.hxx>
#define TheArc Handle(Adaptor2d_Curve2d)
#define TheArc_hxx <Adaptor2d_Curve2d.hxx>
#define ThePathPoint Contap_ThePathPointOfTheSearch
#define ThePathPoint_hxx <Contap_ThePathPointOfTheSearch.hxx>
#define IntStart_Segment Contap_TheSegmentOfTheSearch
#define IntStart_Segment_hxx <Contap_TheSegmentOfTheSearch.hxx>

#include <IntStart_Segment.lxx>

#undef TheVertex
#undef TheVertex_hxx
#undef TheArc
#undef TheArc_hxx
#undef ThePathPoint
#undef ThePathPoint_hxx
#undef IntStart_Segment
#undef IntStart_Segment_hxx

#endif // _Contap_TheSegmentOfTheSearch_HeaderFile
