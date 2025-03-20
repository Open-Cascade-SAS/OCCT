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

#ifndef OpenGl_Group_HeaderFile
#define OpenGl_Group_HeaderFile

#include <Graphic3d_Group.hxx>
#include <Graphic3d_Structure.hxx>

#include <OpenGl_Aspects.hxx>
#include <OpenGl_Element.hxx>

class OpenGl_Structure;

struct OpenGl_ElementNode
{
  OpenGl_Element*     elem;
  OpenGl_ElementNode* next;
  DEFINE_STANDARD_ALLOC
};

//! Implementation of low-level graphic group.
class OpenGl_Group : public Graphic3d_Group
{
public:
  //! Create empty group.
  //! Will throw exception if not created by OpenGl_Structure.
  Standard_EXPORT OpenGl_Group(const Handle(Graphic3d_Structure)& theStruct);

  Standard_EXPORT virtual void Clear(const Standard_Boolean theToUpdateStructureMgr)
    Standard_OVERRIDE;

  //! Return line aspect.
  virtual Handle(Graphic3d_Aspects) Aspects() const Standard_OVERRIDE
  {
    return myAspects != NULL ? myAspects->Aspect() : Handle(Graphic3d_Aspects)();
  }

  //! Return TRUE if group contains primitives with transform persistence.
  bool HasPersistence() const
  {
    return !myTrsfPers.IsNull()
           || (myStructure != NULL && !myStructure->TransformPersistence().IsNull());
  }

  //! Update aspect.
  Standard_EXPORT virtual void SetGroupPrimitivesAspect(const Handle(Graphic3d_Aspects)& theAspect)
    Standard_OVERRIDE;

  //! Append aspect as an element.
  Standard_EXPORT virtual void SetPrimitivesAspect(const Handle(Graphic3d_Aspects)& theAspect)
    Standard_OVERRIDE;

  //! Update presentation aspects after their modification.
  Standard_EXPORT virtual void SynchronizeAspects() Standard_OVERRIDE;

  //! Replace aspects specified in the replacement map.
  Standard_EXPORT virtual void ReplaceAspects(const Graphic3d_MapOfAspectsToAspects& theMap)
    Standard_OVERRIDE;

  //! Add primitive array element
  Standard_EXPORT virtual void AddPrimitiveArray(const Graphic3d_TypeOfPrimitiveArray theType,
                                                 const Handle(Graphic3d_IndexBuffer)& theIndices,
                                                 const Handle(Graphic3d_Buffer)&      theAttribs,
                                                 const Handle(Graphic3d_BoundBuffer)& theBounds,
                                                 const Standard_Boolean theToEvalMinMax)
    Standard_OVERRIDE;

  //! Adds a text for display
  Standard_EXPORT virtual void AddText(const Handle(Graphic3d_Text)& theTextParams,
                                       const Standard_Boolean theToEvalMinMax) Standard_OVERRIDE;
  //! Add flipping element
  Standard_EXPORT virtual void SetFlippingOptions(const Standard_Boolean theIsEnabled,
                                                  const gp_Ax2& theRefPlane) Standard_OVERRIDE;

  //! Add stencil test element
  Standard_EXPORT virtual void SetStencilTestOptions(const Standard_Boolean theIsEnabled)
    Standard_OVERRIDE;

public:
  OpenGl_Structure* GlStruct() const
  {
    return (OpenGl_Structure*)(myStructure->CStructure().operator->());
  }

  Standard_EXPORT void AddElement(OpenGl_Element* theElem);

  Standard_EXPORT virtual void Render(const Handle(OpenGl_Workspace)& theWorkspace) const;
  Standard_EXPORT virtual void Release(const Handle(OpenGl_Context)& theGlCtx);

  //! Returns first OpenGL element node of the group.
  const OpenGl_ElementNode* FirstNode() const { return myFirst; }

  //! Returns OpenGL aspect.
  const OpenGl_Aspects* GlAspects() const { return myAspects; }

  //! Is the group ray-tracable (contains ray-tracable elements)?
  Standard_Boolean IsRaytracable() const { return myIsRaytracable; }

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

protected:
  Standard_EXPORT virtual ~OpenGl_Group();

private:
  //! Render element if it passes the filtering procedure.
  //! This method should be used for elements which can be used in scope of rendering algorithms.
  //! E.g. elements of groups during recursive rendering.
  //! If render filter is null, pure rendering is performed.
  //! @param[in] theWorkspace  the rendering workspace
  //! @param[in] theFilter     the rendering filter to check whether the element should be rendered
  //! or not
  //! @return True if element passes the check and renders
  Standard_EXPORT bool renderFiltered(const Handle(OpenGl_Workspace)& theWorkspace,
                                      OpenGl_Element*                 theElement) const;

protected:
  OpenGl_Aspects*     myAspects;
  OpenGl_ElementNode* myFirst;
  OpenGl_ElementNode* myLast;
  Standard_Boolean    myIsRaytracable;

public:
  DEFINE_STANDARD_RTTIEXT(OpenGl_Group, Graphic3d_Group) // Type definition
};

DEFINE_STANDARD_HANDLE(OpenGl_Group, Graphic3d_Group)

#endif // _OpenGl_Group_Header
