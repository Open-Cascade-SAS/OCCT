// Created on: 1996-07-01
// Created by: Jacques GOUSSARD
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _LocOpe_HBuilder_HeaderFile
#define _LocOpe_HBuilder_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopOpeBRepBuild_HBuilder.hxx>
#include <Standard_Boolean.hxx>
class TopOpeBRepDS_BuildTool;


class LocOpe_HBuilder;
DEFINE_STANDARD_HANDLE(LocOpe_HBuilder, TopOpeBRepBuild_HBuilder)


class LocOpe_HBuilder : public TopOpeBRepBuild_HBuilder
{

public:

  
    LocOpe_HBuilder(const TopOpeBRepDS_BuildTool& BT);
  
    Standard_Boolean Classify() const;
  
    void Classify (const Standard_Boolean B);




  DEFINE_STANDARD_RTTI(LocOpe_HBuilder,TopOpeBRepBuild_HBuilder)

protected:




private:




};


#include <LocOpe_HBuilder.lxx>





#endif // _LocOpe_HBuilder_HeaderFile
