// Copyright (c) 2013 OPEN CASCADE SAS
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

#ifndef _StdPrs_WFDeflectionShape_H__
#define _StdPrs_WFDeflectionShape_H__

#include <Prs3d_Presentation.hxx>
#include <Prs3d_WFShape.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_Root.hxx>
#include <StdPrs_WFDeflectionRestrictedFace.hxx>
#include <StdPrs_DeflectionCurve.hxx>
#include <StdPrs_Vertex.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_HSequenceOfShape.hxx>

typedef Prs3d_WFShape <StdPrs_WFDeflectionRestrictedFace, StdPrs_DeflectionCurve, StdPrs_Vertex> StdPrs_WFDeflectionShape;
#endif
