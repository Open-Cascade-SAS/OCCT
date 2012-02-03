// File:      OpenGl_Group.hxx
// Created:   1 August 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#ifndef _OpenGl_Group_Header
#define _OpenGl_Group_Header

#include <InterfaceGraphic_Graphic3d.hxx>

#include <OpenGl_Element.hxx>

#include <OpenGl_AspectLine.hxx>
#include <OpenGl_AspectFace.hxx>
#include <OpenGl_AspectMarker.hxx>
#include <OpenGl_AspectText.hxx>

#include <OpenGl_tsm.hxx>

struct OpenGl_ElementNode
{
  TelType type;
  OpenGl_Element *elem;
  OpenGl_ElementNode *next;
  IMPLEMENT_MEMORY_OPERATORS
};

class OpenGl_Group : public OpenGl_Element
{
 public:
  OpenGl_Group ();
  virtual ~OpenGl_Group();

  void SetAspectLine (const CALL_DEF_CONTEXTLINE &AContext, const Standard_Boolean IsGlobal = Standard_True);
  void SetAspectFace (const CALL_DEF_CONTEXTFILLAREA &AContext, const Standard_Boolean IsGlobal = Standard_True);
  void SetAspectMarker (const CALL_DEF_CONTEXTMARKER &AContext, const Standard_Boolean IsGlobal = Standard_True);
  void SetAspectText (const CALL_DEF_CONTEXTTEXT &AContext, const Standard_Boolean IsGlobal = Standard_True);

  void AddElement (const TelType, OpenGl_Element * );
  void Clear ();

  void RemovePrimitiveArray (CALL_DEF_PARRAY *APArray);

  virtual void Render (const Handle(OpenGl_Workspace) &AWorkspace) const;

 protected:

  OpenGl_AspectLine *myAspectLine;
  OpenGl_AspectFace *myAspectFace;
  OpenGl_AspectMarker *myAspectMarker;
  OpenGl_AspectText *myAspectText;

  OpenGl_ElementNode *myFirst, *myLast;

 public:
  IMPLEMENT_MEMORY_OPERATORS
};

#endif //_OpenGl_Group_Header
