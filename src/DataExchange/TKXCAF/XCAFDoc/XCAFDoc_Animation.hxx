// Copyright (c) 2025 OPEN CASCADE SAS
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

#ifndef _XCAFDoc_Animation_HeaderFile
#define _XCAFDoc_Animation_HeaderFile

#include <TDataStd_GenericEmpty.hxx>

class Standard_GUID;
class TDF_Label;
class XCAFAnimObjects_Object;

// resolve name collisions with WinAPI headers
#ifdef GetObject
  #undef GetObject
#endif

//! Implementation of a TDF_Attribute for animation data representation.
//! This attribute is attached to a label that identifies an animation within XCAF document.
//! It stores animation operations such as translation, rotation, scaling, etc.
//! Animation data is organized in a hierarchical structure of labels in the XCAF document.
class XCAFDoc_Animation : public TDataStd_GenericEmpty
{
public:
  //! Constructor - creates an empty animation attribute.
  Standard_EXPORT XCAFDoc_Animation();

  //! Returns the GUID for animation attributes.
  //! @return Standard_GUID identifying the animation attribute
  Standard_EXPORT static const Standard_GUID& GetID();

  //! Returns the GUID for shape references in animations.
  //! This GUID is used to store references to shapes that are animated.
  //! @return Standard_GUID for shape references
  Standard_EXPORT static const Standard_GUID& AnimRefShapeGUID();

  //! Creates or retrieves an animation attribute on the specified label.
  //! If the attribute does not exist, it will be created.
  //! @param theLabel Label where animation attribute should be stored
  //! @return Handle to the animation attribute
  Standard_EXPORT static Handle(XCAFDoc_Animation) Set(const TDF_Label& theLabel);

  //! Returns the GUID identifying this attribute.
  //! @return Standard_GUID of this animation attribute
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;

  //! Updates parent's label and its sub-labels with data taken from theAnimationObject.
  //! Old data associated with the label will be lost.
  //! This method stores all animation details including operation types and parameters.
  //! @param theAnimationObject Animation object containing all animation data
  Standard_EXPORT void SetObject(const Handle(XCAFAnimObjects_Object)& theAnimationObject);

  //! Returns animation object data taken from the parent's label and its sub-labels.
  //! The method reconstructs the animation object from the data stored in the XCAF document.
  //! @return Handle to the reconstructed animation object
  Standard_EXPORT Handle(XCAFAnimObjects_Object) GetObject() const;

  //! Dumps the content of this animation attribute into the JSON stream
  //! @param theOStream Output stream where JSON data will be written
  //! @param theDepth Recursion depth for JSON structure (-1 for maximum depth)
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

  DEFINE_DERIVED_ATTRIBUTE(XCAFDoc_Animation, TDataStd_GenericEmpty)
};

#endif
