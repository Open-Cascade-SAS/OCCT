// Created on: 2011-08-01
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2012 OPEN CASCADE SAS
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
