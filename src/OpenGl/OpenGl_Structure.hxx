// File:      OpenGl_Structure.hxx
// Created:   1 August 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

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
  OpenGl_Structure ();
  virtual ~OpenGl_Structure ();

  void SetTransformation (const float *AMatrix);

  void SetTransformPersistence (const CALL_DEF_TRANSFORM_PERSISTENCE &ATransPers);

  void SetDegenerateModel (const Standard_Integer AMode, const float ASkipRatio);

  void SetAspectLine (const CALL_DEF_CONTEXTLINE &AContext);
  void SetAspectFace (const CALL_DEF_CONTEXTFILLAREA &AContext);
  void SetAspectMarker (const CALL_DEF_CONTEXTMARKER &AContext);
  void SetAspectText (const CALL_DEF_CONTEXTTEXT &AContext);

  void SetHighlightBox (const CALL_DEF_BOUNDBOX &ABoundBox);
  void ClearHighlightBox ();

  void SetHighlightColor (const Standard_ShortReal R, const Standard_ShortReal G, const Standard_ShortReal B);
  void ClearHighlightColor ();

  void SetNamedStatus (const Standard_Integer aStatus) { myNamedStatus = aStatus; }

  void Connect (const OpenGl_Structure *astructure);
  void Disconnect (const OpenGl_Structure *astructure);

  OpenGl_Group * AddGroup ();
  void RemoveGroup (const OpenGl_Group *);
  void Clear ();

  //! Set z layer ID to display the structure in specified layer
  void SetZLayer (const Standard_Integer theLayerIndex);

  //! Get z layer ID
  Standard_Integer GetZLayer () const;

  virtual void Render (const Handle(OpenGl_Workspace) &AWorkspace) const;
  
 protected:

  //Structure_LABBegin
  OpenGl_Matrix *myTransformation;
  TEL_TRANSFORM_PERSISTENCE *myTransPers;
  DEGENERATION *myDegenerateModel;
  OpenGl_AspectLine *myAspectLine;
  OpenGl_AspectFace *myAspectFace;
  OpenGl_AspectMarker *myAspectMarker;
  OpenGl_AspectText *myAspectText;
  //Structure_LABHighlight
  OpenGl_Group *myHighlightBox;
  TEL_COLOUR *myHighlightColor;
  //Structure_LABVisibility
  //Structure_LABPick
  int myNamedStatus; //Structure_LABNameSet
  int myZLayer;

  OpenGl_ListOfStructure myConnected;

  OpenGl_ListOfGroup myGroups;

 public:
  IMPLEMENT_MEMORY_OPERATORS
};

#endif //OpenGl_Structure_Header
