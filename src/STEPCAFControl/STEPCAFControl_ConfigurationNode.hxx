// Copyright (c) 2022 OPEN CASCADE SAS
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

#ifndef _STEPCAFControl_ConfigurationNode_HeaderFile
#define _STEPCAFControl_ConfigurationNode_HeaderFile

#include <DE_ConfigurationNode.hxx>
#include <STEPControl_StepModelType.hxx>
#include <Resource_FormatType.hxx>
#include <UnitsMethods_LengthUnit.hxx>

//! The purpose of this class is to configure the transfer process for STEP format
//! Stores the necessary settings for STEPCAFControl_Provider.
//! Configures and creates special provider to transfer STEP files.
//!
//! Nodes grouped by Vendor name and Format type.
//! The Vendor name is "OCC"
//! The Format type is "STEP"
//! The supported CAD extensions are ".stp", ".step", ".stpz"
//! The import process is supported.
//! The export process is supported.
class STEPCAFControl_ConfigurationNode : public DE_ConfigurationNode
{
  DEFINE_STANDARD_RTTIEXT(STEPCAFControl_ConfigurationNode, DE_ConfigurationNode)
public:

  //! Initializes all field by default
  Standard_EXPORT STEPCAFControl_ConfigurationNode();

  //! Copies values of all fields
  //! @param[in] theNode object to copy
  Standard_EXPORT STEPCAFControl_ConfigurationNode(const Handle(STEPCAFControl_ConfigurationNode)& theNode);

  //! Updates values according the resource
  //! @param[in] theResource input resource to use
  //! @return true if theResource loading has ended correctly
  Standard_EXPORT virtual bool Load(const Handle(DE_ConfigurationContext)& theResource) Standard_OVERRIDE;

  //! Writes configuration to the string
  //! @return result resource string
  Standard_EXPORT virtual TCollection_AsciiString Save() const Standard_OVERRIDE;

  //! Copies values of all fields
  //! @return new object with the same field values
  Standard_EXPORT virtual Handle(DE_ConfigurationNode) Copy() const Standard_OVERRIDE;


  //! Creates new provider for the own format
  //! @return new created provider
  Standard_EXPORT virtual Handle(DE_Provider) BuildProvider() Standard_OVERRIDE;

public:

  //! Checks the import supporting
  //! @return true if import is supported
  Standard_EXPORT virtual bool IsImportSupported() const Standard_OVERRIDE;

  //! Checks the export supporting
  //! @return true if export is supported
  Standard_EXPORT virtual bool IsExportSupported() const Standard_OVERRIDE;

  //! Gets CAD format name of associated provider
  //! @return provider CAD format
  Standard_EXPORT virtual TCollection_AsciiString GetFormat() const Standard_OVERRIDE;

  //! Gets provider's vendor name of associated provider
  //! @return provider's vendor name
  Standard_EXPORT virtual TCollection_AsciiString GetVendor() const Standard_OVERRIDE;

  //! Gets list of supported file extensions
  //! @return list of extensions
  Standard_EXPORT virtual TColStd_ListOfAsciiString GetExtensions() const Standard_OVERRIDE;

  //! Checks the file content to verify a format
  //! @param[in] theBuffer read stream buffer to check content
  //! @return Standard_True if file is supported by a current provider
  Standard_EXPORT virtual bool CheckContent(const Handle(NCollection_Buffer)& theBuffer) const Standard_OVERRIDE;

public:

  enum ReadMode_BSplineContinuity
  {
    ReadMode_BSplineContinuity_C0 = 0,
    ReadMode_BSplineContinuity_C1,
    ReadMode_BSplineContinuity_C2
  };
  enum ReadMode_Precision
  {
    ReadMode_Precision_File = 0,
    ReadMode_Precision_User
  };
  enum ReadMode_MaxPrecision
  {
    ReadMode_MaxPrecision_Preferred = 0,
    ReadMode_MaxPrecision_Forced
  };
  enum ReadMode_SurfaceCurve
  {
    ReadMode_SurfaceCurve_Default = 0,
    ReadMode_SurfaceCurve_2DUse_Preferred = 2,
    ReadMode_SurfaceCurve_2DUse_Forced = -2,
    ReadMode_SurfaceCurve_3DUse_Preferred = 3,
    ReadMode_SurfaceCurve_3DUse_Forced = -3
  };
  enum AngleUnitMode
  {
    AngleUnitMode_File = 0,
    AngleUnitMode_Rad,
    AngleUnitMode_Deg
  };
  enum ReadMode_ProductContext
  {
    ReadMode_ProductContext_All = 1,
    ReadMode_ProductContext_Design,
    ReadMode_ProductContext_Analysis
  };
  enum ReadMode_ShapeRepr
  {
    ReadMode_ShapeRepr_All = 1,
    ReadMode_ShapeRepr_ABSR,
    ReadMode_ShapeRepr_MSSR,
    ReadMode_ShapeRepr_GBSSR,
    ReadMode_ShapeRepr_FBSR,
    ReadMode_ShapeRepr_EBWSR,
    ReadMode_ShapeRepr_GBWSR
  };
  enum ReadMode_AssemblyLevel
  {
    ReadMode_AssemblyLevel_All = 1,
    ReadMode_AssemblyLevel_Assembly,
    ReadMode_AssemblyLevel_Structure,
    ReadMode_AssemblyLevel_Shape
  };
  enum RWMode_Tessellated
  {
    RWMode_Tessellated_Off = 0,
    RWMode_Tessellated_On,
    RWMode_Tessellated_OnNoBRep
  };
  enum WriteMode_PrecisionMode
  {
    WriteMode_PrecisionMode_Least = -1,
    WriteMode_PrecisionMode_Average = 0,
    WriteMode_PrecisionMode_Greatest = 1,
    WriteMode_PrecisionMode_Session = 2
  };
  enum WriteMode_Assembly
  {
    WriteMode_Assembly_Off = 0,
    WriteMode_Assembly_On,
    WriteMode_Assembly_Auto
  };
  enum WriteMode_StepSchema
  {
    WriteMode_StepSchema_AP214CD = 1,
    WriteMode_StepSchema_AP214DIS,
    WriteMode_StepSchema_AP203,
    WriteMode_StepSchema_AP214IS,
    WriteMode_StepSchema_AP242DIS
  };
  enum WriteMode_VertexMode
  {
    WriteMode_VertexMode_OneCompound = 0,
    WriteMode_VertexMode_SingleVertex
  };
  struct STEPCAFControl_InternalSection
  {
    // Common 
    ReadMode_BSplineContinuity ReadBSplineContinuity = ReadMode_BSplineContinuity_C1; //<! Manages the continuity of BSpline curves
    ReadMode_Precision ReadPrecisionMode = ReadMode_Precision_File; //<! Reads the precision mode value
    double ReadPrecisionVal = 0.0001; //<! ReadMode_Precision for shape construction (if enabled user mode)
    ReadMode_MaxPrecision ReadMaxPrecisionMode = ReadMode_MaxPrecision_Preferred; //<! Defines the mode of applying the maximum allowed tolerance
    double ReadMaxPrecisionVal = 1; //<! Defines the maximum allowable tolerance
    bool ReadSameParamMode = false; //<! Defines the using of BRepLib::SameParameter
    ReadMode_SurfaceCurve ReadSurfaceCurveMode = ReadMode_SurfaceCurve_Default; //<! Reference for the computation of curves in case of 2D/3D
    double EncodeRegAngle = 0.57295779513; //<! Continuity which these two faces are connected with at that edge
    AngleUnitMode AngleUnit = AngleUnitMode_File; //<! Indicates what angle units should be used when a STEP file is read

    // Read
    TCollection_AsciiString ReadResourceName = "STEP"; //<! Defines the name of the resource file to read
    TCollection_AsciiString ReadSequence = "FromSTEP"; //<! Defines the name of the sequence of operators to read
    bool ReadProductMode = true; //<! Defines the approach used for selection of top-level STEP entities for translation, and for recognition of assembly structures
    ReadMode_ProductContext ReadProductContext = ReadMode_ProductContext_All; //<! When reading AP 209 STEP files, allows selecting either only 'design' or 'analysis', or both types of products for translation
    ReadMode_ShapeRepr ReadShapeRepr = ReadMode_ShapeRepr_All; //<! Specifies preferred type of representation of the shape of the product
    RWMode_Tessellated ReadTessellated = RWMode_Tessellated_On; //!< Defines whether tessellated shapes should be translated
    ReadMode_AssemblyLevel ReadAssemblyLevel = ReadMode_AssemblyLevel_All; //<! Specifies which data should be read for the products found in the STEP file
    bool ReadRelationship = true; //<! Defines whether shapes associated with the main SHAPE_DEFINITION_REPRESENTATION entity of the product via SHAPE_REPRESENTATIONSHIP_RELATION should be translated
    bool ReadShapeAspect = true; //<! Defines whether shapes associated with the PRODUCT_DEFINITION_SHAPE entity of the product via SHAPE_ASPECT should be translated
    bool ReadConstrRelation = false; //<! Flag regulating translation of "CONSTRUCTIVE_GEOMETRY_REPRESENTATION_RELATIONSHIP" entities
    bool ReadSubshapeNames = false; //<! Indicates whether to read sub-shape names from 'Name' attributes of STEP Representation Items
    Resource_FormatType ReadCodePage = Resource_FormatType_UTF8; //<! STEP file encoding for names translation
    bool ReadNonmanifold = false; //<! Defines non-manifold topology reading
    bool ReadIdeas = false; //<! Defines !I-Deas-like STEP processing
    bool ReadAllShapes = false; //<! Parameter to read all top level solids and shells
    bool ReadRootTransformation = true; ///<!/ Mode to variate apply or not transformation placed in the root shape representation
    bool ReadColor = true; //<! ColorMode is used to indicate read Colors or not
    bool ReadName = true; //<! NameMode is used to indicate read Name or not
    bool ReadLayer = true; //<! LayerMode is used to indicate read Layers or not
    bool ReadProps = true; //<! PropsMode is used to indicate read Validation properties or not

    // Write
    WriteMode_PrecisionMode WritePrecisionMode = WriteMode_PrecisionMode_Average; //<! Specifies the mode of writing the resolution value into the STEP file
    double WritePrecisionVal = 0.0001; //<! Resolution value for an STEP file when WriteMode_PrecisionMode is Greatest
    WriteMode_Assembly WriteAssembly = WriteMode_Assembly_Off; //<! Writing assembly mode
    WriteMode_StepSchema WriteSchema = WriteMode_StepSchema_AP214CD; //<! Defines the version of schema used for the output STEP file
    RWMode_Tessellated WriteTessellated = RWMode_Tessellated_OnNoBRep; //!< Defines whether tessellated shapes should be translated
    TCollection_AsciiString WriteProductName; //<! Defines the text string that will be used for field 'name' of PRODUCT entities written to the STEP file
    bool WriteSurfaceCurMode = true; //<! Indicates whether parametric curves (curves in parametric space of surface) should be written into the STEP file
    UnitsMethods_LengthUnit WriteUnit = UnitsMethods_LengthUnit_Millimeter; //<! Defines a unit in which the STEP file should be written
    TCollection_AsciiString WriteResourceName = "STEP"; //<! Defines the name of the resource file to write
    TCollection_AsciiString WriteSequence = "ToSTEP"; //<! Defines the name of the sequence of operators to write
    WriteMode_VertexMode WriteVertexMode = WriteMode_VertexMode_OneCompound; //<! Indicates which of free vertices writing mode is switch on
    bool WriteSubshapeNames = false; //<! Indicates whether to write sub-shape names to 'Name' attributes of STEP Representation Items
    bool WriteColor = true; //<! ColorMode is used to indicate write Colors or not
    bool WriteName = true; //<! NameMode is used to indicate write Name or not
    bool WriteLayer = true; //<! LayerMode is used to indicate write Layers or not
    bool WriteProps = true; //<! PropsMode is used to indicate write Validation properties or not
    STEPControl_StepModelType WriteModelType = STEPControl_AsIs; //<! Gives you the choice of translation mode for an Open CASCADE shape that is being translated to STEP

  } InternalParameters;

};

#endif // _STEPCAFControl_ConfigurationNode_HeaderFile
