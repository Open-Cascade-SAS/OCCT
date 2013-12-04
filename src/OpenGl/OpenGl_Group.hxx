// Created on: 2011-08-01
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _OpenGl_Group_Header
#define _OpenGl_Group_Header

#include <NCollection_List.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>

#include <OpenGl_Element.hxx>

#include <OpenGl_AspectLine.hxx>
#include <OpenGl_AspectFace.hxx>
#include <OpenGl_AspectMarker.hxx>
#include <OpenGl_AspectText.hxx>

#include <OpenGl_tsm.hxx>

class OpenGl_Group;
class OpenGl_Structure;

typedef NCollection_List<const OpenGl_Group*     > OpenGl_ListOfGroup;

struct OpenGl_ElementNode
{
  TelType type;
  OpenGl_Element *elem;
  OpenGl_ElementNode *next;
  DEFINE_STANDARD_ALLOC
};

class OpenGl_Group : public OpenGl_Element
{
public:

#ifndef HAVE_OPENCL
  OpenGl_Group();
#else
  OpenGl_Group (const OpenGl_Structure* theAncestorStructure);
#endif

  void SetAspectLine   (const CALL_DEF_CONTEXTLINE&     theAspect, const Standard_Boolean IsGlobal = Standard_True);
  void SetAspectFace   (const CALL_DEF_CONTEXTFILLAREA& theAspect, const Standard_Boolean IsGlobal = Standard_True);
  void SetAspectMarker (const CALL_DEF_CONTEXTMARKER&   theAspect, const Standard_Boolean IsGlobal = Standard_True);
  void SetAspectText   (const CALL_DEF_CONTEXTTEXT&     theAspect, const Standard_Boolean IsGlobal = Standard_True);

  void AddElement (const TelType, OpenGl_Element * );

  virtual void Render  (const Handle(OpenGl_Workspace)& theWorkspace) const;
  virtual void Release (const Handle(OpenGl_Context)&   theGlCtx);

  //! Returns first OpenGL element node of the group.
  const OpenGl_ElementNode* FirstNode() const { return myFirst; }

  //! Returns OpenGL face aspect.
  const OpenGl_AspectFace* AspectFace() const { return myAspectFace; }

#ifdef HAVE_OPENCL

  //! Returns modification state for ray-tracing.
  Standard_Size ModificationState() const { return myModificationState; }

  //! Is the group ray-tracable (contains ray-tracable elements)?
  Standard_Boolean IsRaytracable() const { return myIsRaytracable; }

#endif

protected:

  virtual ~OpenGl_Group();

protected:

  OpenGl_AspectLine*     myAspectLine;
  OpenGl_AspectFace*     myAspectFace;
  OpenGl_AspectMarker*   myAspectMarker;
  OpenGl_AspectText*     myAspectText;

  OpenGl_ElementNode*    myFirst;
  OpenGl_ElementNode*    myLast;
  
#ifdef HAVE_OPENCL
  const OpenGl_Structure*  myAncestorStructure;
  Standard_Boolean         myIsRaytracable;
  Standard_Size            myModificationState;
#endif

public:

  DEFINE_STANDARD_ALLOC

};

#endif //_OpenGl_Group_Header
