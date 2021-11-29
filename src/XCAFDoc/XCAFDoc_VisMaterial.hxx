// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _XCAFDoc_VisMaterial_HeaderFile
#define _XCAFDoc_VisMaterial_HeaderFile

#include <Graphic3d_AlphaMode.hxx>
#include <Graphic3d_TypeOfBackfacingModel.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TDF_Attribute.hxx>
#include <XCAFDoc_VisMaterialCommon.hxx>
#include <XCAFDoc_VisMaterialPBR.hxx>

class Graphic3d_Aspects;
class Graphic3d_MaterialAspect;

//! Attribute storing Material definition for visualization purposes.
//!
//! Visualization material provides extended information about how object should be displayed on the screen
//! (albedo, metalness, roughness - not just a single color as in case of XCAFDoc_Color).
//! It is expected to correlate with physical material properties (XCAFDoc_Material), but not necessarily (like painted/polished/rusty object).
//!
//! The document defines the list of visualization materials via global attribute XCAFDoc_VisMaterialTool,
//! while particular material assignment to the shape is done through tree-nodes links.
//! Therefore, XCAFDoc_VisMaterialTool methods should be used for managing XCAFDoc_VisMaterial attributes.
//!
//! Visualization material definition consists of two options: Common and PBR (for Physically Based Rendering).
//! Common material definition is an obsolete model defined by very first version of OpenGL graphics API
//! and having specific hardware-accelerated implementation in past (like T&L).
//! PBR metallic-roughness model is closer to physical material properties, and intended to be used within physically-based renderer.
//!
//! For compatibility reasons, this attribute allows defining both material models,
//! so that it is up-to Data Exchange and Application deciding which one to define and use for rendering (depending on viewer capabilities).
//! Automatic conversion from one model to another is possible, but lossy (converted material will not look the same).
//!
//! Within Data Exchange, different file formats have different capabilities for storing visualization material properties
//! from simple color (STEP, IGES), to common (OBJ, glTF 1.0) and PBR (glTF 2.0).
//! This should be taken into account while defining or converting document into one or another format - material definition might be lost or disturbed.
//!
//! @sa XCAFDoc_VisMaterialTool
class XCAFDoc_VisMaterial : public TDF_Attribute
{
  DEFINE_STANDARD_RTTIEXT(XCAFDoc_VisMaterial, TDF_Attribute)
public:

  //! Return attribute GUID.
  Standard_EXPORT static const Standard_GUID& GetID();

public:

  //! Empty constructor.
  Standard_EXPORT XCAFDoc_VisMaterial();

  //! Return TRUE if material definition is empty.
  bool IsEmpty() const { return !myPbrMat.IsDefined && !myCommonMat.IsDefined; }

  //! Fill in material aspect.
  Standard_EXPORT void FillMaterialAspect (Graphic3d_MaterialAspect& theAspect) const;

  //! Fill in graphic aspects.
  Standard_EXPORT void FillAspect (const Handle(Graphic3d_Aspects)& theAspect) const;

  //! Return TRUE if metal-roughness PBR material is defined; FALSE by default.
  Standard_Boolean HasPbrMaterial() const { return myPbrMat.IsDefined; }

  //! Return metal-roughness PBR material.
  //! Note that default constructor creates an empty material (@sa XCAFDoc_VisMaterialPBR::IsDefined).
  const XCAFDoc_VisMaterialPBR& PbrMaterial() const { return myPbrMat; }

  //! Setup metal-roughness PBR material.
  Standard_EXPORT void SetPbrMaterial (const XCAFDoc_VisMaterialPBR& theMaterial);

  //! Setup undefined metal-roughness PBR material.
  void UnsetPbrMaterial()
  {
    XCAFDoc_VisMaterialPBR anEmpty;
    anEmpty.IsDefined = false;
    SetPbrMaterial (anEmpty);
  }

  //! Return TRUE if common material is defined; FALSE by default.
  Standard_Boolean HasCommonMaterial() const { return myCommonMat.IsDefined; }

  //! Return common material.
  //! Note that default constructor creates an empty material (@sa XCAFDoc_VisMaterialCommon::IsDefined).
  const XCAFDoc_VisMaterialCommon& CommonMaterial() const { return myCommonMat; }

  //! Setup common material.
  Standard_EXPORT void SetCommonMaterial (const XCAFDoc_VisMaterialCommon& theMaterial);

  //! Setup undefined common material.
  void UnsetCommonMaterial()
  {
    XCAFDoc_VisMaterialCommon anEmpty;
    anEmpty.IsDefined = false;
    SetCommonMaterial (anEmpty);
  }

  //! Return base color.
  Standard_EXPORT Quantity_ColorRGBA BaseColor() const;

  //! Return alpha mode; Graphic3d_AlphaMode_BlendAuto by default.
  Graphic3d_AlphaMode AlphaMode() const { return myAlphaMode; }

  //! Return alpha cutoff value; 0.5 by default.
  Standard_ShortReal AlphaCutOff() const { return myAlphaCutOff; }

  //! Set alpha mode.
  Standard_EXPORT void SetAlphaMode (Graphic3d_AlphaMode theMode,
                                     Standard_ShortReal  theCutOff = 0.5f);

  //! Returns if the material is double or single sided; Graphic3d_TypeOfBackfacingModel_Auto by default.
  Graphic3d_TypeOfBackfacingModel FaceCulling() const { return myFaceCulling; }

  //! Specifies whether the material is double or single sided.
  Standard_EXPORT void SetFaceCulling (Graphic3d_TypeOfBackfacingModel theFaceCulling);

  Standard_DEPRECATED("Deprecated method, FaceCulling() should be used instead")
  Standard_Boolean IsDoubleSided() const { return myFaceCulling == Graphic3d_TypeOfBackfacingModel_DoubleSided; }

  Standard_DEPRECATED("Deprecated method, SetFaceCulling() should be used instead")
  void SetDoubleSided (Standard_Boolean theIsDoubleSided)
  {
    SetFaceCulling (theIsDoubleSided ? Graphic3d_TypeOfBackfacingModel_DoubleSided : Graphic3d_TypeOfBackfacingModel_Auto);
  }

  //! Return material name / tag (transient data, not stored in the document).
  const Handle(TCollection_HAsciiString)& RawName() const { return myRawName; }

  //! Set material name / tag (transient data, not stored in the document).
  void SetRawName (const Handle(TCollection_HAsciiString)& theName) { myRawName = theName; }

  //! Compare two materials.
  //! Performs deep comparison by actual values - e.g. can be useful for merging materials.
  Standard_Boolean IsEqual (const Handle(XCAFDoc_VisMaterial)& theOther) const
  {
    if (theOther.get() == this)
    {
      return true;
    }
    return theOther->myFaceCulling == myFaceCulling
        && theOther->myAlphaCutOff == myAlphaCutOff
        && theOther->myAlphaMode == myAlphaMode
        && theOther->myCommonMat.IsEqual (myCommonMat)
        && theOther->myPbrMat.IsEqual (myPbrMat);
  }

  //! Return Common material or convert PBR into Common material.
  Standard_EXPORT XCAFDoc_VisMaterialCommon ConvertToCommonMaterial();

  //! Return PBR material or convert Common into PBR material.
  Standard_EXPORT XCAFDoc_VisMaterialPBR ConvertToPbrMaterial();

public: //! @name interface implementation

  //! Return GUID of this attribute type.
  virtual const Standard_GUID& ID() const Standard_OVERRIDE { return GetID(); }

  //! Restore attribute from specified state.
  //! @param theWith [in] attribute state to restore (copy into this)
  Standard_EXPORT virtual void Restore (const Handle(TDF_Attribute)& theWith) Standard_OVERRIDE;

  //! Create a new empty attribute.
  Standard_EXPORT virtual Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;

  //! Paste this attribute into another one.
  //! @param theInto [in/out] target attribute to copy this into
  //! @param theRelTable [in] relocation table
  Standard_EXPORT virtual void Paste (const Handle(TDF_Attribute)& theInto,
                                      const Handle(TDF_RelocationTable)& theRelTable) const Standard_OVERRIDE;
  
  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;

private:

  Handle(TCollection_HAsciiString) myRawName;       //!< material name / tag (transient data)
  XCAFDoc_VisMaterialPBR           myPbrMat;        //!< metal-roughness material definition
  XCAFDoc_VisMaterialCommon        myCommonMat;     //!< common material definition
  Graphic3d_AlphaMode              myAlphaMode;     //!< alpha mode; Graphic3d_AlphaMode_BlendAuto by default
  Standard_ShortReal               myAlphaCutOff;   //!< alpha cutoff value; 0.5 by default
  Graphic3d_TypeOfBackfacingModel  myFaceCulling;   //!< specifies whether the material is double/single sided

};

DEFINE_STANDARD_HANDLE(XCAFDoc_VisMaterial, TDF_Attribute)

#endif // _XCAFDoc_VisMaterial_HeaderFile
