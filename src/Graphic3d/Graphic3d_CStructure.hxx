// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _Graphic3d_CStructure_HeaderFile
#define _Graphic3d_CStructure_HeaderFile

#include <Graphic3d_CGroup.hxx>
#include <Graphic3d_SequenceOfHClipPlane.hxx>

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

  Graphic3d_SequenceOfHClipPlane ClipPlanes;
};

///typedef Graphic3d_CStructure CALL_DEF_STRUCTURE;

const Handle(Standard_Type)& TYPE(Graphic3d_CStructure);

#endif // Graphic3d_CStructure_HeaderFile
