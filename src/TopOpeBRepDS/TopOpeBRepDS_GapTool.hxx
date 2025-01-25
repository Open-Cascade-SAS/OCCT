// Created on: 1998-08-20
// Created by: Yves FRICAUD
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

#ifndef _TopOpeBRepDS_GapTool_HeaderFile
#define _TopOpeBRepDS_GapTool_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopOpeBRepDS_DataMapOfIntegerListOfInterference.hxx>
#include <TopOpeBRepDS_DataMapOfInterferenceShape.hxx>
#include <Standard_Transient.hxx>
#include <TopOpeBRepDS_ListOfInterference.hxx>
#include <Standard_Integer.hxx>
class TopOpeBRepDS_HDataStructure;
class TopOpeBRepDS_Interference;
class TopOpeBRepDS_Curve;
class TopoDS_Shape;

class TopOpeBRepDS_GapTool;
DEFINE_STANDARD_HANDLE(TopOpeBRepDS_GapTool, Standard_Transient)

class TopOpeBRepDS_GapTool : public Standard_Transient
{

public:
  Standard_EXPORT TopOpeBRepDS_GapTool();

  Standard_EXPORT TopOpeBRepDS_GapTool(const Handle(TopOpeBRepDS_HDataStructure)& HDS);

  Standard_EXPORT void Init(const Handle(TopOpeBRepDS_HDataStructure)& HDS);

  Standard_EXPORT const TopOpeBRepDS_ListOfInterference& Interferences(
    const Standard_Integer IndexPoint) const;

  Standard_EXPORT const TopOpeBRepDS_ListOfInterference& SameInterferences(
    const Handle(TopOpeBRepDS_Interference)& I) const;

  Standard_EXPORT TopOpeBRepDS_ListOfInterference& ChangeSameInterferences(
    const Handle(TopOpeBRepDS_Interference)& I);

  Standard_EXPORT Standard_Boolean Curve(const Handle(TopOpeBRepDS_Interference)& I,
                                         TopOpeBRepDS_Curve&                      C) const;

  Standard_EXPORT Standard_Boolean EdgeSupport(const Handle(TopOpeBRepDS_Interference)& I,
                                               TopoDS_Shape&                            E) const;

  //! Return les faces qui  ont genere la section origine
  //! de I
  Standard_EXPORT Standard_Boolean FacesSupport(const Handle(TopOpeBRepDS_Interference)& I,
                                                TopoDS_Shape&                            F1,
                                                TopoDS_Shape&                            F2) const;

  Standard_EXPORT Standard_Boolean ParameterOnEdge(const Handle(TopOpeBRepDS_Interference)& I,
                                                   const TopoDS_Shape&                      E,
                                                   Standard_Real& U) const;

  Standard_EXPORT void SetPoint(const Handle(TopOpeBRepDS_Interference)& I,
                                const Standard_Integer                   IndexPoint);

  Standard_EXPORT void SetParameterOnEdge(const Handle(TopOpeBRepDS_Interference)& I,
                                          const TopoDS_Shape&                      E,
                                          const Standard_Real                      U);

  DEFINE_STANDARD_RTTIEXT(TopOpeBRepDS_GapTool, Standard_Transient)

protected:
private:
  Handle(TopOpeBRepDS_HDataStructure)             myHDS;
  TopOpeBRepDS_DataMapOfIntegerListOfInterference myGToI;
  TopOpeBRepDS_DataMapOfInterferenceShape         myInterToShape;
};

#endif // _TopOpeBRepDS_GapTool_HeaderFile
