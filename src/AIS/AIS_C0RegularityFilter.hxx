// Created on: 1998-02-04
// Created by: Julia GERASIMOVA
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

#ifndef _AIS_C0RegularityFilter_HeaderFile
#define _AIS_C0RegularityFilter_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopTools_MapOfShape.hxx>
#include <SelectMgr_Filter.hxx>
#include <Standard_Boolean.hxx>
#include <TopAbs_ShapeEnum.hxx>
class TopoDS_Shape;
class SelectMgr_EntityOwner;


class AIS_C0RegularityFilter;
DEFINE_STANDARD_HANDLE(AIS_C0RegularityFilter, SelectMgr_Filter)


class AIS_C0RegularityFilter : public SelectMgr_Filter
{

public:

  
  Standard_EXPORT AIS_C0RegularityFilter(const TopoDS_Shape& aShape);
  
  Standard_EXPORT virtual Standard_Boolean ActsOn (const TopAbs_ShapeEnum aType) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Boolean IsOk (const Handle(SelectMgr_EntityOwner)& EO) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(AIS_C0RegularityFilter,SelectMgr_Filter)

protected:




private:


  TopTools_MapOfShape myMapOfEdges;


};







#endif // _AIS_C0RegularityFilter_HeaderFile
