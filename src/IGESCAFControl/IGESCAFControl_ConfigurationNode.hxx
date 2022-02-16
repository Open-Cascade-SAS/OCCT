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

#ifndef _IGESCAFControl_ConfigurationNode_HeaderFile
#define _IGESCAFControl_ConfigurationNode_HeaderFile

#include <DE_ConfigurationNode.hxx>
#include <UnitsMethods_LengthUnit.hxx>

//! The purpose of this class is to configure the transfer process for IGES format
//! Stores the necessary settings for IGESCAFControl_Provider.
//! Configures and creates special provider to transfer IGES files.
//!
//! Nodes grouped by Vendor name and Format type.
//! The Vendor name is "OCC"
//! The Format type is "IGES"
//! The supported CAD extensions are ".igs", ".iges"
//! The import process is supported.
//! The export process is supported.
class IGESCAFControl_ConfigurationNode : public DE_ConfigurationNode
{
  DEFINE_STANDARD_RTTIEXT(IGESCAFControl_ConfigurationNode, DE_ConfigurationNode)
public:

  //! Initializes all fields by default
  Standard_EXPORT IGESCAFControl_ConfigurationNode();

  //! Copies values of all fields
  //! @param[in] theNode object to copy
  Standard_EXPORT IGESCAFControl_ConfigurationNode(const Handle(IGESCAFControl_ConfigurationNode)& theNode);

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
  enum WriteMode_BRep
  {
    WriteMode_BRep_Faces = 0,
    WriteMode_BRep_BRep
  };
  enum WriteMode_ConvertSurface
  {
    WriteMode_ConvertSurface_Off = 0,
    WriteMode_ConvertSurface_On
  };
  enum WriteMode_PrecisionMode
  {
    WriteMode_PrecisionMode_Least = -1,
    WriteMode_PrecisionMode_Average = 0,
    WriteMode_PrecisionMode_Greatest = 1,
    WriteMode_PrecisionMode_Session = 2
  };
  enum WriteMode_PlaneMode
  {
    WriteMode_PlaneMode_Plane = 0,
    WriteMode_PlaneMode_BSpline
  };
  struct IGESCAFControl_InternalSection
  {
    // Common
    ReadMode_BSplineContinuity ReadBSplineContinuity = ReadMode_BSplineContinuity_C1; //<! Manages the continuity of BSpline curves
    ReadMode_Precision ReadPrecisionMode = ReadMode_Precision_File; //<! Reads the precision mode value
    double ReadPrecisionVal = 0.0001; //<! ReadMode_Precision for shape construction (if enabled user mode)
    ReadMode_MaxPrecision ReadMaxPrecisionMode = ReadMode_MaxPrecision_Preferred; //<! Defines the mode of applying the maximum allowed tolerance
    double ReadMaxPrecisionVal = 1; //<! Defines the maximum allowable tolerance
    bool ReadSameParamMode = false; //<! Defines the using of BRepLib::SameParameter
    ReadMode_SurfaceCurve ReadSurfaceCurveMode = ReadMode_SurfaceCurve_Default; //<! reference for the computation of curves in case of 2D/3D
    double EncodeRegAngle = 0.57295779513; //<! Continuity which these two faces are connected with at that edge

    //Read
    bool ReadApproxd1 = false; //<! Flag to split bspline curves of degree 1
    TCollection_AsciiString ReadResourceName = "IGES"; //<! Defines the name of the resource file to read
    TCollection_AsciiString ReadSequence = "FromIGES"; //<! Defines the name of the sequence of operators to read
    bool ReadFaultyEntities = false; //<! Parameter for reading failed entities
    bool ReadOnlyVisible = false; //<! Parameter for reading invisible entities
    bool ReadColor = true; //<! ColorMode is used to indicate read Colors or not
    bool ReadName = true; //<! NameMode is used to indicate read Name or not
    bool ReadLayer = true; //<! LayerMode is used to indicate read Layers or not

    // Write
    WriteMode_BRep WriteBRepMode = WriteMode_BRep_Faces; //<! Flag to define entities type to write
    WriteMode_ConvertSurface WriteConvertSurfaceMode = WriteMode_ConvertSurface_Off; //<! Flag to convert surface to elementary
    UnitsMethods_LengthUnit WriteUnit = UnitsMethods_LengthUnit_Millimeter; //<! Define unit to write IGES file
    TCollection_AsciiString WriteHeaderAuthor; //<! Name of the author of the file
    TCollection_AsciiString WriteHeaderCompany; //<! Name of the sending company
    TCollection_AsciiString WriteHeaderProduct; //<! Name of the sending product
    TCollection_AsciiString WriteHeaderReciever; //<! Name of the receiving company
    TCollection_AsciiString WriteResourceName = "IGES"; //<! Defines the name of the resource file to write
    TCollection_AsciiString WriteSequence = "ToIGES"; //<! Defines the name of the sequence of operators to write
    WriteMode_PrecisionMode WritePrecisionMode = WriteMode_PrecisionMode_Average; //<! Specifies the mode of writing the resolution value into the IGES file
    double WritePrecisionVal = 0.0001; //<! Resolution value for an IGES file when WriteMode_PrecisionMode is Greatest
    WriteMode_PlaneMode WritePlaneMode = WriteMode_PlaneMode_Plane; //<! Flag to convert plane to the BSline
    bool WriteOffsetMode = false; //<! Writing offset curves like BSplines
    bool WriteColor = true; //<! ColorMode is used to indicate write Colors or not
    bool WriteName = true; //<! NameMode is used to indicate write Name or not
    bool WriteLayer = true; //<! LayerMode is used to indicate write Layers or not
  } InternalParameters;

};

#endif // _IGESCAFControl_ConfigurationNode_HeaderFile
