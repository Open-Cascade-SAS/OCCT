// Created on: 2002-12-10
// Created by: data exchange team
// Copyright (c) 2002-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V2.0

#include <StepElement_ElementAspectMember.ixx>
#include <TCollection_HAsciiString.hxx>

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
