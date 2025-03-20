// Created on: 1998-08-18
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

#ifndef _TopOpeBRepDS_Association_HeaderFile
#define _TopOpeBRepDS_Association_HeaderFile

#include <Standard.hxx>

#include <TopOpeBRepDS_DataMapOfInterferenceListOfInterference.hxx>
#include <Standard_Transient.hxx>
#include <TopOpeBRepDS_ListOfInterference.hxx>
#include <Standard_Boolean.hxx>
class TopOpeBRepDS_Interference;

class TopOpeBRepDS_Association;
DEFINE_STANDARD_HANDLE(TopOpeBRepDS_Association, Standard_Transient)

class TopOpeBRepDS_Association : public Standard_Transient
{

public:
  Standard_EXPORT TopOpeBRepDS_Association();

  Standard_EXPORT void Associate(const Handle(TopOpeBRepDS_Interference)& I,
                                 const Handle(TopOpeBRepDS_Interference)& K);

  Standard_EXPORT void Associate(const Handle(TopOpeBRepDS_Interference)& I,
                                 const TopOpeBRepDS_ListOfInterference&   LI);

  Standard_EXPORT Standard_Boolean HasAssociation(const Handle(TopOpeBRepDS_Interference)& I) const;

  Standard_EXPORT TopOpeBRepDS_ListOfInterference& Associated(
    const Handle(TopOpeBRepDS_Interference)& I);

  Standard_EXPORT Standard_Boolean AreAssociated(const Handle(TopOpeBRepDS_Interference)& I,
                                                 const Handle(TopOpeBRepDS_Interference)& K) const;

  DEFINE_STANDARD_RTTIEXT(TopOpeBRepDS_Association, Standard_Transient)

protected:
private:
  TopOpeBRepDS_DataMapOfInterferenceListOfInterference myMap;
};

#endif // _TopOpeBRepDS_Association_HeaderFile
