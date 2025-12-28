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
  Standard_EXPORT OpenGl_Group(const occ::handle<Graphic3d_Structure>& theStruct);

  Standard_EXPORT void Clear(const bool theToUpdateStructureMgr) override;

  //! Return line aspect.
  occ::handle<Graphic3d_Aspects> Aspects() const override
  {
    return myAspects != nullptr ? myAspects->Aspect() : occ::handle<Graphic3d_Aspects>();
  }

  //! Return TRUE if group contains primitives with transform persistence.
  bool HasPersistence() const
  {
    return !myTrsfPers.IsNull()
           || (myStructure != nullptr && !myStructure->TransformPersistence().IsNull());
  }

  //! Update aspect.
  Standard_EXPORT void SetGroupPrimitivesAspect(
    const occ::handle<Graphic3d_Aspects>& theAspect) override;

  //! Append aspect as an element.
  Standard_EXPORT void SetPrimitivesAspect(
    const occ::handle<Graphic3d_Aspects>& theAspect) override;

  //! Update presentation aspects after their modification.
  Standard_EXPORT void SynchronizeAspects() override;

  //! Replace aspects specified in the replacement map.
  Standard_EXPORT void ReplaceAspects(
    const NCollection_DataMap<occ::handle<Graphic3d_Aspects>, occ::handle<Graphic3d_Aspects>>&
      theMap) override;

  //! Add primitive array element
  Standard_EXPORT void AddPrimitiveArray(
    const Graphic3d_TypeOfPrimitiveArray      theType,
    const occ::handle<Graphic3d_IndexBuffer>& theIndices,
    const occ::handle<Graphic3d_Buffer>&      theAttribs,
    const occ::handle<Graphic3d_BoundBuffer>& theBounds,
    const bool                                theToEvalMinMax) override;

  //! Adds a text for display
  Standard_EXPORT void AddText(const occ::handle<Graphic3d_Text>& theTextParams,
                                       const bool                         theToEvalMinMax) override;
  //! Add flipping element
  Standard_EXPORT void SetFlippingOptions(const bool    theIsEnabled,
                                                  const gp_Ax2& theRefPlane) override;

  //! Add stencil test element
  Standard_EXPORT void SetStencilTestOptions(const bool theIsEnabled) override;

public:
  OpenGl_Structure* GlStruct() const
  {
    return (OpenGl_Structure*)(myStructure->CStructure().operator->());
  }

  Standard_EXPORT void AddElement(OpenGl_Element* theElem);

  Standard_EXPORT virtual void Render(const occ::handle<OpenGl_Workspace>& theWorkspace) const;
  Standard_EXPORT virtual void Release(const occ::handle<OpenGl_Context>& theGlCtx);

  //! Returns first OpenGL element node of the group.
  const OpenGl_ElementNode* FirstNode() const { return myFirst; }

  //! Returns OpenGL aspect.
  const OpenGl_Aspects* GlAspects() const { return myAspects; }

  //! Is the group ray-tracable (contains ray-tracable elements)?
  bool IsRaytracable() const { return myIsRaytracable; }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream,
                                        int               theDepth = -1) const override;

protected:
  Standard_EXPORT ~OpenGl_Group() override;

private:
  //! Render element if it passes the filtering procedure.
  //! This method should be used for elements which can be used in scope of rendering algorithms.
  //! E.g. elements of groups during recursive rendering.
  //! If render filter is null, pure rendering is performed.
  //! @param[in] theWorkspace  the rendering workspace
  //! @param[in] theFilter     the rendering filter to check whether the element should be rendered
  //! or not
  //! @return True if element passes the check and renders
  Standard_EXPORT bool renderFiltered(const occ::handle<OpenGl_Workspace>& theWorkspace,
                                      OpenGl_Element*                      theElement) const;

protected:
  OpenGl_Aspects*     myAspects;
  OpenGl_ElementNode* myFirst;
  OpenGl_ElementNode* myLast;
  bool                myIsRaytracable;

public:
  DEFINE_STANDARD_RTTIEXT(OpenGl_Group, Graphic3d_Group) // Type definition
};

#endif // _OpenGl_Group_Header
