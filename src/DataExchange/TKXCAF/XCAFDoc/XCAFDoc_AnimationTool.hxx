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

#ifndef _XCAFDoc_AnimationTool_HeaderFile
#define _XCAFDoc_AnimationTool_HeaderFile

#include <TDF_LabelSequence.hxx>
#include <TDataStd_GenericEmpty.hxx>

//! Tool to work with animations in XCAF documents.
//! This tool provides methods to manage animations, their parameters and references to shapes.
//! The tool is responsible for storing and retrieving animation data such as frame rates,
//! time codes, axis information, and associations between shapes and animations.
class XCAFDoc_AnimationTool : public TDataStd_GenericEmpty
{

public:
  //! Returns the GUID for animation tool attributes.
  //! @return Standard_GUID identifying the animation tool
  Standard_EXPORT static const Standard_GUID& GetID();

  //! Creates (if does not exist) or retrieves an animation tool from the XCAFDoc at label L.
  //! @param theLabel Label where animation tool should be created or found
  //! @return Handle to the animation tool
  Standard_EXPORT static Handle(XCAFDoc_AnimationTool) Set(const TDF_Label& theLabel);

  //! Creates an empty animation tool.
  //! Creates a tool to work with animations in a document.
  //! Attaches to the provided label.
  Standard_EXPORT XCAFDoc_AnimationTool();

  //! Returns the base label under which animations are stored.
  //! @return TDF_Label that serves as the root for all animations
  Standard_EXPORT TDF_Label BaseLabel() const;

  //! Checks if the given label represents an animation.
  //! @param theLabel Label to check
  //! @return true if the label contains animation data
  Standard_EXPORT bool IsAnimation(const TDF_Label& theLabel) const;

  //! Associates a shape with an animation by creating a reference between them.
  //! @param theShLabel Label of the shape to be animated
  //! @param theAnimLabel Label of the animation to apply to the shape
  Standard_EXPORT void SetAnimation(const TDF_Label& theShLabel,
                                    const TDF_Label& theAnimLabel) const;

  //! Retrieves the global frames per second (FPS) setting.
  //! @param theFPS Variable to store the FPS value
  //! @return true if FPS value exists, false otherwise
  Standard_EXPORT bool GetGlobalFPS(double& theFPS) const;

  //! Sets the global frames per second (FPS) setting.
  //! @param theFPS FPS value to set
  Standard_EXPORT void SetGlobalFPS(const double theFPS) const;

  //! Retrieves the start time code for animations.
  //! @param theCode Variable to store the time code
  //! @return true if time code exists, false otherwise
  Standard_EXPORT bool GetStartTimeCode(double& theCode) const;

  //! Sets the start time code for animations.
  //! @param theCode Time code value to set
  Standard_EXPORT void SetStartTimeCode(const double theCode) const;

  //! Retrieves the end time code for animations.
  //! @param theCode Variable to store the time code
  //! @return true if time code exists, false otherwise
  Standard_EXPORT bool GetEndTimeCode(double& theCode) const;

  //! Sets the end time code for animations.
  //! @param theCode Time code value to set
  Standard_EXPORT void SetEndTimeCode(const double theCode) const;

  //! Retrieves the length unit used in the file.
  //! @param theLengthUnit Variable to store the length unit
  //! @return true if length unit exists, false otherwise
  Standard_EXPORT bool GetFileLengthUnit(double& theLengthUnit) const;

  //! Sets the length unit used in the file.
  //! @param theLengthUnit Length unit value to set
  Standard_EXPORT void SetFileLengthUnit(const double theLengthUnit) const;

  //! Retrieves the up axis used in the coordinate system (typically "X", "Y", or "Z").
  //! @param theAxis Variable to store the axis string
  //! @return true if axis information exists, false otherwise
  Standard_EXPORT bool GetUpAxis(TCollection_AsciiString& theAxis) const;

  //! Sets the up axis used in the coordinate system.
  //! @param theAxis Axis string to set (typically "X", "Y", or "Z")
  Standard_EXPORT void SetUpAxis(const TCollection_AsciiString theAxis) const;

  //! Returns a sequence of all animation labels currently stored in the animation table.
  //! @param theLabels Sequence to store animation labels
  Standard_EXPORT void GetAnimationLabels(TDF_LabelSequence& theLabels) const;

  //! Returns the animation label defined for a specific shape.
  //! @param theShLabel Label of the shape to query
  //! @param theAnimLabel Label of the animation associated with the shape
  //! @return true if an animation is associated with the shape, false otherwise
  Standard_EXPORT bool GetRefAnimationLabel(const TDF_Label& theShLabel,
                                            TDF_Label&       theAnimLabel) const;

  //! Returns the shape label for which the animation is defined.
  //! @param theAnimLabel Label of the animation to query
  //! @param theShLabel Label of the shape associated with the animation
  //! @return true if the animation is associated with a shape, false otherwise
  Standard_EXPORT bool GetRefShapeLabel(const TDF_Label& theAnimLabel, TDF_Label& theShLabel) const;

  //! Adds a new animation definition to the animation table and returns its label.
  //! @return TDF_Label of the newly created animation
  Standard_EXPORT TDF_Label AddAnimation() const;

  //! Gets non-uniform scale values from the label.
  //! @param theShLabel Label containing shape with scale information
  //! @param theDX Scale factor along X axis
  //! @param theDY Scale factor along Y axis
  //! @param theDZ Scale factor along Z axis
  //! @return true if attribute exists
  Standard_EXPORT static Standard_Boolean GetShapeNonUniformScale(const TDF_Label& theShLabel,
                                                                  double&          theDX,
                                                                  double&          theDY,
                                                                  double&          theDZ);

  //! Sets non-uniform scale values on the label.
  //! @param theShLabel Label containing shape to be scaled
  //! @param theDX Scale factor along X axis
  //! @param theDY Scale factor along Y axis
  //! @param theDZ Scale factor along Z axis
  //! @return true if operation succeeded
  Standard_EXPORT static Standard_Boolean SetShapeNonUniformScale(const TDF_Label& theShLabel,
                                                                  const double     theDX,
                                                                  const double     theDY,
                                                                  const double     theDZ);

  //! Checks if the given animation is marked as locked.
  //! Locked animations cannot be modified.
  //! @param theAnimLabel Label of the animation to check
  //! @return true if the animation is locked, false otherwise
  Standard_EXPORT bool IsLocked(const TDF_Label& theAnimLabel) const;

  //! Marks the given animation as locked.
  //! This prevents the animation from being modified.
  //! @param theAnimLabel Label of the animation to lock
  Standard_EXPORT void Lock(const TDF_Label& theAnimLabel) const;

  //! Removes the lock from the given animation.
  //! This allows the animation to be modified.
  //! @param theAnimLabel Label of the animation to unlock
  Standard_EXPORT void Unlock(const TDF_Label& theAnimLabel) const;

  //! Dumps information about the animation tool to stream.
  //! @param theOStream Stream to output information
  //! @param theDepth If true, dumps detailed information
  //! @return Modified output stream
  Standard_EXPORT Standard_OStream& Dump(Standard_OStream& theOStream, const bool theDepth) const;

  //! Dumps information about the animation tool to stream (overridden from TDF_Attribute).
  //! @param theOStream Stream to output information
  //! @return Modified output stream
  Standard_EXPORT virtual Standard_OStream& Dump(Standard_OStream& theOStream) const
    Standard_OVERRIDE;

  //! Returns the GUID identifying this attribute.
  //! @return Standard_GUID of the animation tool
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;

  //! Dumps the content of this animation tool into the JSON stream.
  //! @param theOStream Output stream where JSON data will be written
  //! @param theDepth Recursion depth for JSON structure (-1 for maximum depth)
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

  DEFINE_DERIVED_ATTRIBUTE(XCAFDoc_AnimationTool, TDataStd_GenericEmpty)

private:
};

#endif // _XCAFDoc_AnimationTool_HeaderFile
