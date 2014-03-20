// Created on: 2011-08-01
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#ifndef OpenGl_Structure_Header
#define OpenGl_Structure_Header

#include <Graphic3d_CStructure.hxx>
#include <Graphic3d_SequenceOfHClipPlane.hxx>

#include <OpenGl_AspectLine.hxx>
#include <OpenGl_AspectFace.hxx>
#include <OpenGl_AspectMarker.hxx>
#include <OpenGl_AspectText.hxx>

#include <OpenGl_Group.hxx>
#include <OpenGl_Matrix.hxx>
#include <OpenGl_NamedStatus.hxx>

#include <NCollection_List.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>

class OpenGl_Structure;
class OpenGl_GraphicDriver;

typedef NCollection_List<const OpenGl_Structure* > OpenGl_ListOfStructure;

class OpenGl_Structure : public Graphic3d_CStructure
{
  friend class OpenGl_Group;

public:

  //! Create empty structure
  OpenGl_Structure (const Handle(Graphic3d_StructureManager)& theManager);

  //! Setup structure graphic state
  virtual void UpdateNamedStatus();

  //! Clear graphic data
  virtual void Clear();

  //! Connect other structure to this one
  virtual void Connect    (Graphic3d_CStructure& theStructure);

  //! Disconnect other structure to this one
  virtual void Disconnect (Graphic3d_CStructure& theStructure);

  //! Synchronize structure aspects
  virtual void UpdateAspects();

  //! Synchronize structure transformation
  virtual void UpdateTransformation();

  //! Highlight entire structure with color
  virtual void HighlightWithColor  (const Graphic3d_Vec3&  theColor,
                                    const Standard_Boolean theToCreate);

  //! Highlight structure using boundary box
  virtual void HighlightWithBndBox (const Standard_Boolean theToCreate);

  //! Create shadow link to this structure
  virtual Handle(Graphic3d_CStructure) ShadowLink (const Handle(Graphic3d_StructureManager)& theManager) const;

public:

  //! Access graphic driver
  OpenGl_GraphicDriver* GlDriver() const
  {
    return (OpenGl_GraphicDriver* )myGraphicDriver.operator->();
  }

  void SetTransformPersistence (const CALL_DEF_TRANSFORM_PERSISTENCE &ATransPers);

  void SetAspectLine   (const CALL_DEF_CONTEXTLINE &theAspect);
  void SetAspectFace   (const CALL_DEF_CONTEXTFILLAREA& theAspect);
  void SetAspectMarker (const CALL_DEF_CONTEXTMARKER& theAspect);
  void SetAspectText   (const CALL_DEF_CONTEXTTEXT &theAspect);

  void SetHighlightBox (const Handle(OpenGl_Context)& theGlCtx,
                        const CALL_DEF_BOUNDBOX& theBoundBox);

  void ClearHighlightBox (const Handle(OpenGl_Context)& theGlCtx);

  void SetHighlightColor (const Handle(OpenGl_Context)& theGlCtx,
                          const Graphic3d_Vec3&         theColor);

  void ClearHighlightColor (const Handle(OpenGl_Context)& theGlCtx);

  Standard_Boolean IsVisible() const { return !(myNamedStatus & OPENGL_NS_HIDE); }

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
  //! As result structure could be correctly destroyed layer without GL context
  //! (after last window was closed for example).
  //!
  //! Notice however that reusage of this structure after calling this method is incorrect
  //! and will lead to broken visualization due to loosed data.
  void ReleaseGlResources (const Handle(OpenGl_Context)& theGlCtx);

  //! Returns list of OpenGL groups.
  virtual const OpenGl_ListOfGroup& Groups() const { return myGroups; }

  //! Returns list of connected OpenGL structures.
  const OpenGl_ListOfStructure& ConnectedStructures() const { return myConnected; }

  //! Returns OpenGL face aspect.
  const OpenGl_AspectFace* AspectFace() const { return myAspectFace; }

  //! Returns OpenGL transformation matrix.
  const OpenGl_Matrix* Transformation() const { return myTransformation; }
  
  //! Returns OpenGL persistent translation.
  const TEL_TRANSFORM_PERSISTENCE* PersistentTranslation() const { return myTransPers; }

#ifdef HAVE_OPENCL

  //! Returns structure modification state (for ray-tracing).
  Standard_Size ModificationState() const { return myModificationState; }

  //! Resets structure modification state (for ray-tracing)
  void ResetModificationState() const { myModificationState = 0; }

  //! Is the structure ray-tracable (contains ray-tracable elements)?
  Standard_Boolean IsRaytracable() const { return myIsRaytracable; }

#endif

protected:

  virtual ~OpenGl_Structure();

#ifdef HAVE_OPENCL

  //! Registers ancestor connected structure (for updating ray-tracing state).
  void RegisterAncestorStructure (const OpenGl_Structure* theStructure) const;

  //! Unregisters ancestor connected structure (for updating ray-tracing state).
  void UnregisterAncestorStructure (const OpenGl_Structure* theStructure) const;

  //! Unregisters structure from ancestor structure (for updating ray-tracing state).
  void UnregisterFromAncestorStructure() const;

  //! Updates modification state for structure and its parents.
  void UpdateStateWithAncestorStructures() const;

  //! Updates ray-tracable status for structure and its parents.
  void UpdateRaytracableWithAncestorStructures() const;

  //! Sets ray-tracable status for structure and its parents.
  void SetRaytracableWithAncestorStructures() const;

#endif

protected:

  OpenGl_Matrix*             myTransformation;
  TEL_TRANSFORM_PERSISTENCE* myTransPers;
  OpenGl_AspectLine*         myAspectLine;
  OpenGl_AspectFace*         myAspectFace;
  OpenGl_AspectMarker*       myAspectMarker;
  OpenGl_AspectText*         myAspectText;

  OpenGl_Group*              myHighlightBox;
  TEL_COLOUR*                myHighlightColor;

  int                        myNamedStatus;
  int                        myZLayer;

  OpenGl_ListOfStructure           myConnected;
  OpenGl_ListOfGroup               myGroups;

#ifdef HAVE_OPENCL
  mutable OpenGl_ListOfStructure   myAncestorStructures;
  mutable Standard_Boolean         myIsRaytracable;
  mutable Standard_Size            myModificationState;
#endif

public:

  DEFINE_STANDARD_RTTI(OpenGl_Structure) // Type definition

};

DEFINE_STANDARD_HANDLE(OpenGl_Structure, Graphic3d_CStructure)

#endif // OpenGl_Structure_Header
