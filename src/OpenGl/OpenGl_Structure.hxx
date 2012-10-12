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


#ifndef OpenGl_Structure_Header
#define OpenGl_Structure_Header

#include <NCollection_List.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>

#include <OpenGl_AspectLine.hxx>
#include <OpenGl_AspectFace.hxx>
#include <OpenGl_AspectMarker.hxx>
#include <OpenGl_AspectText.hxx>

#include <OpenGl_Group.hxx>
#include <OpenGl_Matrix.hxx>

typedef NCollection_List<const OpenGl_Structure *> OpenGl_ListOfStructure;
typedef NCollection_List<const OpenGl_Group *> OpenGl_ListOfGroup;

class OpenGl_Structure : public OpenGl_Element
{

public:

  OpenGl_Structure();

  void SetTransformation (const float *AMatrix);

  void SetTransformPersistence (const CALL_DEF_TRANSFORM_PERSISTENCE &ATransPers);

  void SetDegenerateModel (const Standard_Integer AMode, const float ASkipRatio);

  void SetAspectLine (const CALL_DEF_CONTEXTLINE &AContext);
  void SetAspectFace (const CALL_DEF_CONTEXTFILLAREA &AContext);
  void SetAspectMarker (const CALL_DEF_CONTEXTMARKER &AContext);
  void SetAspectText (const CALL_DEF_CONTEXTTEXT &AContext);

  void SetHighlightBox (const Handle(OpenGl_Context)& theGlCtx,
                        const CALL_DEF_BOUNDBOX&      theBoundBox);

  void ClearHighlightBox (const Handle(OpenGl_Context)& theGlCtx);

  void SetHighlightColor (const Handle(OpenGl_Context)& theGlCtx,
                          const Standard_ShortReal R,
                          const Standard_ShortReal G,
                          const Standard_ShortReal B);

  void ClearHighlightColor (const Handle(OpenGl_Context)& theGlCtx);

  void SetNamedStatus (const Standard_Integer aStatus) { myNamedStatus = aStatus; }

  void Connect (const OpenGl_Structure *astructure);
  void Disconnect (const OpenGl_Structure *astructure);

  OpenGl_Group* AddGroup();
  void RemoveGroup (const Handle(OpenGl_Context)& theGlCtx,
                    const OpenGl_Group*           theGroup);
  void Clear (const Handle(OpenGl_Context)& theGlCtx);

  //! Set z layer ID to display the structure in specified layer
  void SetZLayer (const Standard_Integer theLayerIndex);

  //! Get z layer ID
  Standard_Integer GetZLayer () const;

  virtual void Render  (const Handle(OpenGl_Workspace)& theWorkspace) const;
  virtual void Release (const Handle(OpenGl_Context)&   theGlCtx);

  //! This method releases GL resources without actual elements destruction.
  //! As result structure could be correctly destroyed layter without GL context
  //! (after last window was closed for example).
  //!
  //! Notice however that reusage of this structure after calling this method is incorrect
  //! and will lead to broken visualization due to loosed data.
  void ReleaseGlResources (const Handle(OpenGl_Context)& theGlCtx);

protected:

  virtual ~OpenGl_Structure();

protected:

  //Structure_LABBegin
  OpenGl_Matrix*             myTransformation;
  TEL_TRANSFORM_PERSISTENCE* myTransPers;
  DEGENERATION*              myDegenerateModel;
  OpenGl_AspectLine*         myAspectLine;
  OpenGl_AspectFace*         myAspectFace;
  OpenGl_AspectMarker*       myAspectMarker;
  OpenGl_AspectText*         myAspectText;
  //Structure_LABHighlight
  OpenGl_Group*              myHighlightBox;
  TEL_COLOUR*                myHighlightColor;
  //Structure_LABVisibility
  //Structure_LABPick
  int                        myNamedStatus; //Structure_LABNameSet
  int                        myZLayer;

  OpenGl_ListOfStructure     myConnected;
  OpenGl_ListOfGroup         myGroups;

public:

  DEFINE_STANDARD_ALLOC

};

#endif //OpenGl_Structure_Header
