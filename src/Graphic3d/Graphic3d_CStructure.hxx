// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

#ifndef _Graphic3d_CStructure_HeaderFile
#define _Graphic3d_CStructure_HeaderFile

#include <Graphic3d_CGroup.hxx>

class Graphic3d_CStructure
{

public:

  int   Id;
  void* ptrStructure;

  int   Priority;
  int   PreviousPriority;

  CALL_DEF_CONTEXTLINE     ContextLine;
  CALL_DEF_CONTEXTFILLAREA ContextFillArea;
  CALL_DEF_CONTEXTMARKER   ContextMarker;
  CALL_DEF_CONTEXTTEXT     ContextText;

  CALL_DEF_BOUNDBOX BoundBox;

  float Transformation[4][4];
  int   Composition;

  int   ContainsFacet;

  unsigned IsDeleted     : 1;
  unsigned IsOpen        : 1;
  unsigned IsInfinite    : 1;
  unsigned stick         : 1;
  unsigned highlight     : 1;
  unsigned visible       : 1;
  unsigned pick          : 1;
  unsigned HLRValidation : 1;

  CALL_DEF_TRANSFORM_PERSISTENCE TransformPersistence;

};

///typedef Graphic3d_CStructure CALL_DEF_STRUCTURE;

const Handle(Standard_Type)& TYPE(Graphic3d_CStructure);

#endif // Graphic3d_CStructure_HeaderFile
