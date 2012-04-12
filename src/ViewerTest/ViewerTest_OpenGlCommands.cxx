// Created on: 2012-04-09
// Created by: Sergey ANIKIN
// Copyright (c) 2012 OPEN CASCADE SAS
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <ViewerTest.hxx>

#include <AIS_InteractiveObject.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <Graphic3d_Group.hxx>
#include <OpenGl_ArbVBO.hxx>
#include <OpenGl_AspectFace.hxx>
#include <OpenGl_AspectLine.hxx>
#include <OpenGl_AspectMarker.hxx>
#include <OpenGl_AspectText.hxx>
#include <OpenGl_Callback.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_Element.hxx>
#include <OpenGl_ExtFBO.hxx>
#include <OpenGl_GlCore20.hxx>
#include <OpenGl_ResourceCleaner.hxx>
#include <OpenGl_ResourceTexture.hxx>
#include <OpenGl_ResourceVBO.hxx>
#include <OpenGl_Workspace.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Root.hxx>
#include <Select3D_SensitiveCurve.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <TCollection_AsciiString.hxx>

extern Standard_Boolean VDisplayAISObject (const TCollection_AsciiString& theName,
                                           const Handle(AIS_InteractiveObject)& theAISObj,
                                           Standard_Boolean theReplaceIfExists = Standard_True);

//=======================================================================
//function : VUserDraw
//purpose  : Checks availability and operation of UserDraw feature
//=======================================================================
DEFINE_STANDARD_HANDLE(VUserDrawObj, AIS_InteractiveObject)

class VUserDrawObj : public AIS_InteractiveObject
{
public:
    // CASCADE RTTI
    DEFINE_STANDARD_RTTI(VUserDrawObj);

    VUserDrawObj()
    {
      myCoords[0] = -10.;
      myCoords[1] = -20.;
      myCoords[2] = -30.;
      myCoords[3] =  10.;
      myCoords[4] =  20.;
      myCoords[5] =  30.;
    }

public:
  class Element : public OpenGl_Element
  {
  private:
    Handle(VUserDrawObj) myIObj;

  public:
    Element (const Handle(VUserDrawObj)& theIObj,
             CALL_DEF_BOUNDS* theBounds)
    : myIObj( theIObj )
    {
      if (!myIObj.IsNull())
        myIObj->GetBounds(theBounds);
    }

    virtual ~Element ()
    {
    }

    virtual void Render (const Handle(OpenGl_Workspace)& theWorkspace) const
    {
      if (!myIObj.IsNull())
        myIObj->Render(theWorkspace);
    }

  public:
    DEFINE_STANDARD_ALLOC
  };

private:
    // Virtual methods implementation
    void Compute (const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                  const Handle(Prs3d_Presentation)& thePresentation,
                  const Standard_Integer theMode);

    void ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                           const Standard_Integer theMode);

    // Called by VUserDrawElement
    void Render(const Handle(OpenGl_Workspace)& theWorkspace) const;
    void GetBounds(CALL_DEF_BOUNDS* theBounds);

    GLfloat myCoords[6];

    friend class Element;
};
IMPLEMENT_STANDARD_HANDLE(VUserDrawObj, AIS_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT(VUserDrawObj, AIS_InteractiveObject)

void VUserDrawObj::Compute(const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                           const Handle(Prs3d_Presentation)& thePresentation,
                           const Standard_Integer theMode)
{
  thePresentation->Clear();

  Handle(Graphic3d_Group) aGrp = Prs3d_Root::CurrentGroup(thePresentation);
  aGrp->UserDraw(this, Standard_True, Standard_True);
}

void VUserDrawObj::ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                                     const Standard_Integer theMode)
{
  Handle(SelectMgr_EntityOwner) anEntityOwner = new SelectMgr_EntityOwner(this);
  Handle(TColgp_HArray1OfPnt) aPnts = new TColgp_HArray1OfPnt(1, 5);
  aPnts->SetValue(1, gp_Pnt(myCoords[0], myCoords[1], myCoords[2]));
  aPnts->SetValue(2, gp_Pnt(myCoords[3], myCoords[4], myCoords[2]));
  aPnts->SetValue(3, gp_Pnt(myCoords[3], myCoords[4], myCoords[5]));
  aPnts->SetValue(4, gp_Pnt(myCoords[0], myCoords[1], myCoords[5]));
  aPnts->SetValue(5, gp_Pnt(myCoords[0], myCoords[1], myCoords[2]));
  Handle(Select3D_SensitiveCurve) aSensitive = new Select3D_SensitiveCurve(anEntityOwner, aPnts);
  theSelection->Add(aSensitive);
}

void VUserDrawObj::GetBounds(CALL_DEF_BOUNDS* theBounds)
{
  if (theBounds)
  {
    theBounds->XMin = myCoords[0];
    theBounds->YMin = myCoords[1];
    theBounds->ZMin = myCoords[2];
    theBounds->XMax = myCoords[3];
    theBounds->YMax = myCoords[4];
    theBounds->ZMax = myCoords[5];
  }
}

void VUserDrawObj::Render(const Handle(OpenGl_Workspace)& theWorkspace) const
{
  // To test linking against OpenGl_Workspace and all aspect classes
  const OpenGl_AspectLine* aLA = theWorkspace->AspectLine(0);
  const OpenGl_AspectFace* aFA = theWorkspace->AspectFace(0);
  aFA->Context();
  const OpenGl_AspectMarker* aMA = theWorkspace->AspectMarker(0);
  aMA->Type();
  const OpenGl_AspectText* aTA = theWorkspace->AspectText(0);
  aTA->Font();
  TEL_COLOUR aColor = theWorkspace->NamedStatus & OPENGL_NS_HIGHLIGHT ?
    *(theWorkspace->HighlightColor) : aLA->Color();

  // To test OpenGl_Window
  Handle(OpenGl_Context) aCtx = theWorkspace->GetGlContext();
  GLCONTEXT aGlContext = theWorkspace->GetGContext();

  // To link against OpenGl_Context and extensions
  GLuint aVboId = -1, aTexId = -1;
  if (aCtx->arbVBO)
    aCtx->arbVBO->glGenBuffersARB(1, &aVboId);
  glGenTextures(1, &aTexId);

  // To link against OpenGl_ResourceCleaner, OpenGl_ResourceVBO, OpenGl_ResourceTexture
  OpenGl_ResourceCleaner* aResCleaner = OpenGl_ResourceCleaner::GetInstance();
  if (aVboId != (GLuint)-1)
    aResCleaner->AddResource(aGlContext, new OpenGl_ResourceVBO(aVboId));
  if (aTexId != (GLuint)-1)
    aResCleaner->AddResource(aGlContext, new OpenGl_ResourceTexture(aTexId));

  // Finally draw something to make sure UserDraw really works
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_LIGHTING);
  glColor4fv(aColor.rgb);
  glBegin(GL_LINE_LOOP);
  glVertex3f(myCoords[0], myCoords[1], myCoords[2]);
  glVertex3f(myCoords[3], myCoords[4], myCoords[2]);
  glVertex3f(myCoords[3], myCoords[4], myCoords[5]);
  glVertex3f(myCoords[0], myCoords[1], myCoords[5]);
  glEnd();
  glPopAttrib();
}



OpenGl_Element* VUserDrawCallback(const CALL_DEF_USERDRAW * theUserDraw)
{
  Handle(VUserDrawObj) anIObj = (VUserDrawObj*)theUserDraw->Data;
  if (anIObj.IsNull())
  {
    std::cout << "VUserDrawCallback error: null object passed, the custom scene element will not be rendered" << std::endl;
  }

  return new VUserDrawObj::Element(anIObj, theUserDraw->Bounds);
}

static Standard_Integer VUserDraw (Draw_Interpretor& di,
                                    Standard_Integer argc,
                                    const char ** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    di << argv[0] << "Call 'vinit' before!\n";
    return 1;
  }

  if (argc > 2)
  {
    di << argv[0] << "Wrong number of arguments, only the object name expected\n";
    return 1;
  }

  TCollection_AsciiString aName (argv[1]);
  VDisplayAISObject(aName, Handle(AIS_InteractiveObject)());

  // register the custom element factory function
  ::UserDrawCallback() = VUserDrawCallback;

  Handle(VUserDrawObj) anIObj = new VUserDrawObj();
  VDisplayAISObject(aName, anIObj);

  return 0;
}

//=======================================================================
//function : OpenGlCommands
//purpose  :
//=======================================================================

void ViewerTest::OpenGlCommands(Draw_Interpretor& theCommands)
{
  const char* aGroup ="Commands for low-level TKOpenGl features";

  theCommands.Add("vuserdraw",
    "vuserdraw : name - simulates drawing with help of UserDraw",
    __FILE__, VUserDraw, aGroup);
}
