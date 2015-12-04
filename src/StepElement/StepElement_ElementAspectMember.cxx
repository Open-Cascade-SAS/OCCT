// Created on: 2002-12-10
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V2.0

#include <Standard_Type.hxx>
#include <StepElement_ElementAspectMember.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepElement_ElementAspectMember,StepData_SelectNamed)

static Standard_CString aElementVolume = "ELEMENT_VOLUME";
static Standard_CString aVolume3dFace = "VOLUME_3D_FACE";
static Standard_CString aVolume2dFace = "VOLUME_2D_FACE";
static Standard_CString aVolume3dEdge = "VOLUME_3D_EDGE";
static Standard_CString aVolume2dEdge = "VOLUME_2D_EDGE";
static Standard_CString aSurface3dFace = "SURFACE_3D_FACE";
static Standard_CString aSurface2dFace = "SURFACE_2D_FACE";
static Standard_CString aSurface3dEdge = "SURFACE_3D_EDGE";
static Standard_CString aSurface2dEdge = "SURFACE_2D_EDGE";
static Standard_CString aCurveEdge = "CURVE_EDGE";

//=======================================================================
//function : StepElement_ElementAspectMember
//purpose  : 
//=======================================================================

StepElement_ElementAspectMember::StepElement_ElementAspectMember () : mycase(0) 
{
}

//=======================================================================
//function : HasName
//purpose  : 
//=======================================================================

Standard_Boolean StepElement_ElementAspectMember::HasName() const
{
 return mycase >0;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Standard_CString StepElement_ElementAspectMember::Name() const
{
  switch(mycase)  {
    case 1  : return aElementVolume;
    case 2  : return aVolume3dFace;
    case 3  : return aVolume2dFace;
    case 4  : return aVolume3dEdge;
    case 5  : return aVolume2dEdge;
    case 6  : return aSurface3dFace;
    case 7  : return aSurface2dFace;
    case 8  : return aSurface3dEdge;
    case 9  : return aSurface2dEdge;
    case 10  : return aCurveEdge;
    default : break;
  }
  return "";
}

//=======================================================================
//function : CompareNames
//purpose  : 
//=======================================================================

static Standard_Integer CompareNames(const Standard_CString name,Standard_Integer &numen) 
{
  Standard_Integer thecase =0;
  if (!name || name[0] == '\0') thecase = 0;
   else if(!strcmp (name,aElementVolume)) { 
     thecase = 1;
     numen =  1;
   }
   else if(!strcmp (name,aVolume3dFace)) thecase = 2;
   else if(!strcmp (name,aVolume2dFace)) thecase = 3;
   else if(!strcmp (name,aVolume3dEdge)) thecase = 4;
   else if(!strcmp (name,aVolume2dEdge)) thecase = 5;
   else if(!strcmp (name,aSurface3dFace)) thecase = 6;
   else if(!strcmp (name,aSurface2dFace)) thecase = 7;
   else if(!strcmp (name,aSurface3dEdge)) thecase = 8;
   else if(!strcmp (name,aSurface2dEdge)) thecase = 9;
   else if(!strcmp (name,aCurveEdge)) { 
     thecase = 10;
     numen =  1;
   }
  return thecase;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

Standard_Boolean StepElement_ElementAspectMember::SetName(const Standard_CString name) 
{
  Standard_Integer numit =0;
  mycase = CompareNames(name,numit);
  if(numit) SetInteger(numit);
  return (mycase >0);
}

//=======================================================================
//function : Matches
//purpose  : 
//=======================================================================

Standard_Boolean StepElement_ElementAspectMember::Matches(const Standard_CString name) const
{
  Standard_Integer numit =0;
  Standard_Integer thecase = CompareNames(name,numit);
  return (mycase==thecase);
}
