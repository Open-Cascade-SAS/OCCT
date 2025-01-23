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

#include <DESTEP_ConfigurationNode.hxx>

#include <DESTEP_Provider.hxx>
#include <DE_ConfigurationContext.hxx>
#include <DE_PluginHolder.hxx>
#include <NCollection_Buffer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DESTEP_ConfigurationNode, DE_ShapeFixConfigurationNode)

namespace
{
static const TCollection_AsciiString& THE_CONFIGURATION_SCOPE()
{
  static const TCollection_AsciiString aScope = "provider";
  return aScope;
}

// Wrapper to auto-load DE component
DE_PluginHolder<DESTEP_ConfigurationNode> THE_OCCT_STEP_COMPONENT_PLUGIN;
} // namespace

//=================================================================================================

DESTEP_ConfigurationNode::DESTEP_ConfigurationNode()
    : DE_ShapeFixConfigurationNode()
{
  DE_ShapeFixConfigurationNode::ShapeFixParameters =
    DESTEP_Parameters::GetDefaultShapeFixParameters();
}

//=================================================================================================

DESTEP_ConfigurationNode::DESTEP_ConfigurationNode(const Handle(DESTEP_ConfigurationNode)& theNode)
    : DE_ShapeFixConfigurationNode(theNode),
      InternalParameters(theNode->InternalParameters)
{
}

//=================================================================================================

bool DESTEP_ConfigurationNode::Load(const Handle(DE_ConfigurationContext)& theResource)
{
  TCollection_AsciiString aScope =
    THE_CONFIGURATION_SCOPE() + "." + GetFormat() + "." + GetVendor();

  InternalParameters.ReadBSplineContinuity =
    (DESTEP_Parameters::ReadMode_BSplineContinuity)theResource->IntegerVal(
      "read.iges.bspline.continuity",
      InternalParameters.ReadBSplineContinuity,
      aScope);
  InternalParameters.ReadPrecisionMode =
    (DESTEP_Parameters::ReadMode_Precision)
      theResource->IntegerVal("read.precision.mode", InternalParameters.ReadPrecisionMode, aScope);
  InternalParameters.ReadPrecisionVal =
    theResource->RealVal("read.precision.val", InternalParameters.ReadPrecisionVal, aScope);
  InternalParameters.ReadMaxPrecisionMode =
    (DESTEP_Parameters::ReadMode_MaxPrecision)theResource->IntegerVal(
      "read.maxprecision.mode",
      InternalParameters.ReadMaxPrecisionMode,
      aScope);
  InternalParameters.ReadMaxPrecisionVal =
    theResource->RealVal("read.maxprecision.val", InternalParameters.ReadMaxPrecisionVal, aScope);
  InternalParameters.ReadSameParamMode =
    theResource->BooleanVal("read.stdsameparameter.mode",
                            InternalParameters.ReadSameParamMode,
                            aScope);
  InternalParameters.ReadSurfaceCurveMode =
    (DESTEP_Parameters::ReadMode_SurfaceCurve)theResource->IntegerVal(
      "read.surfacecurve.mode",
      InternalParameters.ReadSurfaceCurveMode,
      aScope);
  InternalParameters.EncodeRegAngle =
    theResource->RealVal("read.encoderegularity.angle", InternalParameters.EncodeRegAngle, aScope);
  InternalParameters.AngleUnit =
    (DESTEP_Parameters::AngleUnitMode)theResource->IntegerVal("angleunit.mode",
                                                              InternalParameters.AngleUnit,
                                                              aScope);

  InternalParameters.ReadProductMode =
    theResource->BooleanVal("read.product.mode", InternalParameters.ReadProductMode, aScope);
  InternalParameters.ReadProductContext =
    (DESTEP_Parameters::ReadMode_ProductContext)theResource->IntegerVal(
      "read.product.context",
      InternalParameters.ReadProductContext,
      aScope);
  InternalParameters.ReadShapeRepr =
    (DESTEP_Parameters::ReadMode_ShapeRepr)theResource->IntegerVal("read.shape.repr",
                                                                   InternalParameters.ReadShapeRepr,
                                                                   aScope);
  InternalParameters.ReadTessellated =
    (DESTEP_Parameters::RWMode_Tessellated)
      theResource->IntegerVal("read.tessellated", InternalParameters.ReadTessellated, aScope);
  InternalParameters.ReadAssemblyLevel =
    (DESTEP_Parameters::ReadMode_AssemblyLevel)
      theResource->IntegerVal("read.assembly.level", InternalParameters.ReadAssemblyLevel, aScope);
  InternalParameters.ReadRelationship =
    theResource->BooleanVal("read.shape.relationship", InternalParameters.ReadRelationship, aScope);
  InternalParameters.ReadShapeAspect =
    theResource->BooleanVal("read.shape.aspect", InternalParameters.ReadShapeAspect, aScope);
  InternalParameters.ReadConstrRelation =
    theResource->BooleanVal("read.constructivegeom.relationship",
                            InternalParameters.ReadConstrRelation,
                            aScope);
  InternalParameters.ReadSubshapeNames =
    theResource->BooleanVal("read.stepcaf.subshapes.name",
                            InternalParameters.ReadSubshapeNames,
                            aScope);
  InternalParameters.ReadCodePage =
    (Resource_FormatType)theResource->IntegerVal("read.codepage",
                                                 InternalParameters.ReadCodePage,
                                                 aScope);
  InternalParameters.ReadNonmanifold =
    theResource->BooleanVal("read.nonmanifold", InternalParameters.ReadNonmanifold, aScope);
  InternalParameters.ReadIdeas =
    theResource->BooleanVal("read.ideas", InternalParameters.ReadIdeas, aScope);
  InternalParameters.ReadAllShapes =
    theResource->BooleanVal("read.all.shapes", InternalParameters.ReadAllShapes, aScope);
  InternalParameters.ReadRootTransformation =
    theResource->BooleanVal("read.root.transformation",
                            InternalParameters.ReadRootTransformation,
                            aScope);
  InternalParameters.ReadColor =
    theResource->BooleanVal("read.color", InternalParameters.ReadColor, aScope);
  InternalParameters.ReadName =
    theResource->BooleanVal("read.name", InternalParameters.ReadName, aScope);
  InternalParameters.ReadLayer =
    theResource->BooleanVal("read.layer", InternalParameters.ReadLayer, aScope);
  InternalParameters.ReadProps =
    theResource->BooleanVal("read.props", InternalParameters.ReadProps, aScope);
  InternalParameters.ReadMetadata =
    theResource->BooleanVal("read.metadata", InternalParameters.ReadMetadata, aScope);

  InternalParameters.WritePrecisionMode =
    (DESTEP_Parameters::WriteMode_PrecisionMode)theResource->IntegerVal(
      "write.precision.mode",
      InternalParameters.WritePrecisionMode,
      aScope);
  InternalParameters.WritePrecisionVal =
    theResource->RealVal("write.precision.val", InternalParameters.WritePrecisionVal, aScope);
  InternalParameters.WriteAssembly =
    (DESTEP_Parameters::WriteMode_Assembly)theResource->IntegerVal("write.assembly",
                                                                   InternalParameters.WriteAssembly,
                                                                   aScope);
  InternalParameters.WriteSchema =
    (DESTEP_Parameters::WriteMode_StepSchema)theResource->IntegerVal("write.schema",
                                                                     InternalParameters.WriteSchema,
                                                                     aScope);
  InternalParameters.WriteTessellated =
    (DESTEP_Parameters::RWMode_Tessellated)
      theResource->IntegerVal("write.tessellated", InternalParameters.WriteTessellated, aScope);
  InternalParameters.WriteProductName =
    theResource->StringVal("write.product.name", InternalParameters.WriteProductName, aScope);
  InternalParameters.WriteSurfaceCurMode =
    theResource->BooleanVal("write.surfacecurve.mode",
                            InternalParameters.WriteSurfaceCurMode,
                            aScope);
  InternalParameters.WriteUnit =
    (UnitsMethods_LengthUnit)theResource->IntegerVal("write.unit",
                                                     InternalParameters.WriteUnit,
                                                     aScope);
  InternalParameters.WriteVertexMode =
    (DESTEP_Parameters::WriteMode_VertexMode)
      theResource->IntegerVal("write.vertex.mode", InternalParameters.WriteVertexMode, aScope);
  InternalParameters.WriteSubshapeNames =
    theResource->BooleanVal("write.stepcaf.subshapes.name",
                            InternalParameters.WriteSubshapeNames,
                            aScope);
  InternalParameters.WriteColor =
    theResource->BooleanVal("write.color", InternalParameters.WriteColor, aScope);
  InternalParameters.WriteName =
    theResource->BooleanVal("write.name", InternalParameters.WriteName, aScope);
  InternalParameters.WriteLayer =
    theResource->BooleanVal("write.layer", InternalParameters.WriteLayer, aScope);
  InternalParameters.WriteProps =
    theResource->BooleanVal("write.props", InternalParameters.WriteProps, aScope);
  InternalParameters.WriteModelType =
    (STEPControl_StepModelType)theResource->IntegerVal("write.model.type",
                                                       InternalParameters.WriteModelType,
                                                       aScope);

  return DE_ShapeFixConfigurationNode::Load(theResource);
}

//=================================================================================================

TCollection_AsciiString DESTEP_ConfigurationNode::Save() const
{
  TCollection_AsciiString aResult;
  aResult += "!*****************************************************************************\n";
  aResult =
    aResult + "!Configuration Node " + " Vendor: " + GetVendor() + " Format: " + GetFormat() + "\n";
  TCollection_AsciiString aScope =
    THE_CONFIGURATION_SCOPE() + "." + GetFormat() + "." + GetVendor() + ".";

  aResult += "!\n";
  aResult += "!Common parameters:\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Manages the continuity of BSpline curves (IGES entities 106, 112 and 126) ";
  aResult += "after translation to Open CASCADE Technology\n";
  aResult += "!Default value: 1. Available values: 0, 1, 2\n";
  aResult +=
    aScope + "read.iges.bspline.continuity :\t " + InternalParameters.ReadBSplineContinuity + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Reads the precision mode value\n";
  aResult += "!Default value: \"File\"(0). Available values: \"File\"(0), \"User\"(1)\n";
  aResult += aScope + "read.precision.mode :\t " + InternalParameters.ReadPrecisionMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!This parameter gives the precision for shape construction when the ";
  aResult += "read.precision.mode parameter value is 1\n";
  aResult += "!Default value: 0.0001. Available values: any real positive (non null) value\n";
  aResult += aScope + "read.precision.val :\t " + InternalParameters.ReadPrecisionVal + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the mode of applying the maximum allowed tolerance\n";
  aResult +=
    "!Default value: \"Preferred\"(0). Available values: \"Preferred\"(0), \"Forced\"(1)\n";
  aResult +=
    aScope + "read.maxprecision.mode :\t " + InternalParameters.ReadMaxPrecisionMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the maximum allowable tolerance (in internal units, which are specified in "
             "xstep.cascade.unit)";
  aResult += " of the shape\n";
  aResult += "!Default value: 1. Available values: any real positive (non null) value\n";
  aResult += aScope + "read.maxprecision.val :\t " + InternalParameters.ReadMaxPrecisionVal + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the using of BRepLib::SameParameter\n";
  aResult += "!Default value: \"Off\"(0). Available values: \"Off\"(0), \"On\"(1)\n";
  aResult +=
    aScope + "read.stdsameparameter.mode :\t " + InternalParameters.ReadSameParamMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult +=
    "!Preference for the computation of curves in case of 2D/3D inconsistency in an entity ";
  aResult += "which has both 2D and 3D representations.\n";
  aResult +=
    "!Default value: \"Default\"(0). Available values: \"Default\"(0), \"2DUse_Preferred\"(2), ";
  aResult += "\"2DUse_Forced\"(-2), \"3DUse_Preferred\"(3), \"3DUse_Forced\"(-3)\n";
  aResult +=
    aScope + "read.surfacecurve.mode :\t " + InternalParameters.ReadSurfaceCurveMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!This parameter is used within the BRepLib::EncodeRegularity() function ";
  aResult += "which is called for a shape read ";
  aResult += "from an IGES or a STEP file at the end of translation process.This function sets the "
             "regularity flag of";
  aResult +=
    " an edge in a shell when this edge is shared by two faces.This flag shows the continuity, ";
  aResult += "which these two faces are connected with at that edge.\n";
  aResult += "!Default value (in degrees): 0.57295779513. Available values: <double>\n";
  aResult += aScope + "read.encoderegularity.angle :\t " + InternalParameters.EncodeRegAngle + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Indicates what angle units should be used when a STEP file is read\n";
  aResult += "!Default value: \"File\"(0). Available values: \"File\"(0), \"Rad\"(1), \"Deg\"(2)\n";
  aResult += aScope + "angleunit.mode :\t " + InternalParameters.AngleUnit + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Read Parameters:\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult +=
    "!Defines the approach used for selection of top-level STEP entities for translation, ";
  aResult += "and for recognition of assembly structures\n";
  aResult += "!Default value: 1(\"ON\"). Available values: 0(\"OFF\"), 1(\"ON\")\n";
  aResult += aScope + "read.product.mode :\t " + InternalParameters.ReadProductMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult +=
    "!When reading AP 209 STEP files, allows selecting either only 'design' or 'analysis', ";
  aResult += "or both types of products for translation\n";
  aResult +=
    "!Default value: 1(\"all\"). Available values: 1(\"all\"), 2(\"design\"), 3(\"analysis\")\n";
  aResult += aScope + "read.product.context :\t " + InternalParameters.ReadProductContext + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Specifies preferred type of representation of the shape of the product, in case if "
             "a STEP file contains";
  aResult += " more than one representation(i.e.multiple PRODUCT_DEFINITION_SHAPE entities) for a "
             "single product\n";
  aResult += "!Default value: 1(\"All\"). Available values: 1(\"All\"), 2(\"ABSR\"), 3(\"MSSR\"), "
             "4(\"GBSSR\"), ";
  aResult += "5(\"FBSR\"), 6(\"EBWSR\"), 7(\"GBWSR\")\n";
  aResult += aScope + "read.shape.repr :\t " + InternalParameters.ReadShapeRepr + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines whether tessellated shapes should be translated";
  aResult += "!Default value: 1(\"On\"). Available values: 0(\"OFF\"), 2(\"OnNoBRep\")\n";
  aResult += aScope + "read.tessellated :\t " + InternalParameters.ReadTessellated + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Specifies which data should be read for the products found in the STEP file\n";
  aResult += "!Default value: 1(\"All\"). Available values: 1(\"All\"), 2(\"assembly\"),";
  aResult += "3(\"structure\"), 4(\"shape\")\n";
  aResult += aScope + "read.assembly.level :\t " + InternalParameters.ReadAssemblyLevel + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult +=
    "!Defines whether shapes associated with the main SHAPE_DEFINITION_REPRESENTATION entity";
  aResult += "of the product via SHAPE_REPRESENTATIONSHIP_RELATION should be translated\n";
  aResult += "!Default value: 1(\"ON\"). Available values: 0(\"OFF\"), 1(\"ON\")\n";
  aResult += aScope + "read.shape.relationship :\t " + InternalParameters.ReadRelationship + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult +=
    "!Defines whether shapes associated with the PRODUCT_DEFINITION_SHAPE entity of the product ";
  aResult += "via SHAPE_ASPECT should be translated.\n";
  aResult += "!Default value: 1(\"ON\"). Available values: 0(\"OFF\"), 1(\"ON\")\n";
  aResult += aScope + "read.shape.aspect :\t " + InternalParameters.ReadShapeAspect + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Boolean flag regulating translation of "
             "\"CONSTRUCTIVE_GEOMETRY_REPRESENTATION_RELATIONSHIP\" ";
  aResult += "entities that define position of constructive geometry entities contained in ";
  aResult += "\"CONSTRUCTIVE_GEOMETRY_REPRESENTATION\" with respect to the main representation of "
             "the shape (product).\n";
  aResult += "!Default value: 0(\"OFF\"). Available values: 0(\"OFF\"), 1(\"ON\")\n";
  aResult += aScope + "read.constructivegeom.relationship :\t "
             + InternalParameters.ReadConstrRelation + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Indicates whether to read sub-shape names from 'Name' attributes of STEP "
             "Representation Items\n";
  aResult += "!Default value: 0(\"OFF\"). Available values: 0(\"OFF\"), 1(\"ON\")\n";
  aResult +=
    aScope + "read.stepcaf.subshapes.name :\t " + InternalParameters.ReadSubshapeNames + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!STEP file encoding for names translation\n";
  aResult +=
    "!Default value: 4(\"UTF8\"). Available values: 0(\"SJIS\"), 1(\"EUC\"), 2(\"NoConversion\"), ";
  aResult +=
    "3(\"GB\"), 4(\"UTF8\"), 5(\"SystemLocale\"), 6(\"CP1250\"), 7(\"CP1251\"), 8(\"CP1252\"), ";
  aResult += "9(\"CP1253\"), 10(\"CP1254\"), 11(\"CP1255\"), 12(\"CP1256\"), 13(\"CP1257\"), "
             "14(\"CP1258\"), ";
  aResult += "15(\"iso8859-1\"), 16(\"iso8859-2\"), 17(\"iso8859-3\"), 18(\"iso8859-4\"), "
             "19(\"iso8859-5\"), ";
  aResult +=
    "20(\"iso8859-6\"), 21(\"iso8859-7\"), 22(\"iso8859-8\"), 23(\"iso8859-9\"), 24(\"CP850\")\n";
  aResult += aScope + "read.codepage :\t " + InternalParameters.ReadCodePage + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Non-manifold topology reading\n";
  aResult += "!Default value: 0(\"OFF\"). Available values: 0(\"OFF\"), 1(\"ON\")\n";
  aResult += aScope + "read.nonmanifold :\t " + InternalParameters.ReadNonmanifold + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!I-Deas-like STEP processing\n";
  aResult += "!Default value: 0(\"OFF\"). Available values: 0(\"OFF\"), 1(\"ON\")\n";
  aResult += aScope + "read.ideas :\t " + InternalParameters.ReadIdeas + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Parameter to read all top level solids and shells\n";
  aResult += "!Default value: 0(\"OFF\"). Available values: 0(\"OFF\"), 1(\"ON\")\n";
  aResult += aScope + "read.all.shapes :\t " + InternalParameters.ReadAllShapes + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult +=
    "!Mode to variate apply or not transformation placed in the root shape representation.\n";
  aResult += "!Default value: 1(\"ON\"). Available values: 0(\"OFF\"), 1(\"ON\")\n";
  aResult +=
    aScope + "read.root.transformation :\t " + InternalParameters.ReadRootTransformation + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Setting up the read.colo parameter which is used to indicate read Colors or not\n";
  aResult += "!Default value: +. Available values: \"-\", \"+\"\n";
  aResult += aScope + "read.color :\t " + InternalParameters.ReadColor + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Setting up the read.name parameter which is used to indicate read Names or not\n";
  aResult += "!Default value: +. Available values: \"-\", \"+\"\n";
  aResult += aScope + "read.name :\t " + InternalParameters.ReadName + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Setting up the read.layer parameter which is used to indicate read Layers or not\n";
  aResult += "!Default value: +. Available values: \"-\", \"+\"\n";
  aResult += aScope + "read.layer :\t " + InternalParameters.ReadLayer + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Setting up the read.props parameter which is used to indicate read Validation "
             "properties or not\n";
  aResult += "!Default value: +. Available values: \"-\", \"+\"\n";
  aResult += aScope + "read.props :\t " + InternalParameters.ReadProps + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult +=
    "!Setting up the read.metadata parameter which is used to indicate read Metadata or not\n";
  aResult += "!Default value: 1(\"ON\"). Available values: 0(\"OFF\"), 1(\"ON\")\n";
  aResult += aScope + "read.metadata :\t " + InternalParameters.ReadMetadata + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Write Parameters:\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Writes the precision value\n";
  aResult += "!Default value: \"Average\"(0). Available values: \"Least\"(-1), \"Average\"(0), ";
  aResult += "\"Greatest\"(1), \"Session\"(2)\n";
  aResult += aScope + "write.precision.mode :\t " + InternalParameters.WritePrecisionMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!A user-defined precision value\n";
  aResult += "!Default value: 0.0001. Available values: any real positive (non null) value\n";
  aResult += aScope + "write.precision.val :\t " + InternalParameters.WritePrecisionVal + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Writing assembly mode\n";
  aResult += "!Default value: 2(\"Auto\"). Available values: 0(\"Off\"), 1(\"On\"), 2(\"Auto\")\n";
  aResult += aScope + "write.assembly :\t " + InternalParameters.WriteAssembly + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the version of schema used for the output STEP file\n";
  aResult +=
    "!Default value: 4 or AP214IS. Available values: 1 or AP214CD, 2 or AP214DIS, 3 or AP203, ";
  aResult += "4 or AP214IS, 5 or AP242DIS\n";
  aResult += aScope + "write.schema :\t " + InternalParameters.WriteSchema + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines whether tessellated shapes should be translated";
  aResult += "!Default value: 2(\"OnNoBRep\"). Available values: 0(\"OFF\"), 1(\"On\")\n";
  aResult += aScope + "write.tessellated :\t " + InternalParameters.WriteTessellated + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the text string that will be used for field 'name' of PRODUCT entities "
             "written to the STEP file\n";
  aResult += "!Default value: OCCT STEP translator (current OCCT version number). Available "
             "values: <string>\n";
  aResult += aScope + "write.product.name :\t " + InternalParameters.WriteProductName + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult +=
    "!This parameter indicates whether parametric curves should be written into the STEP file\n";
  aResult += "!Default value: 1(\"ON\"). Available values: 0(\"OFF\"), 1(\"ON\")\n";
  aResult +=
    aScope + "write.surfacecurve.mode :\t " + InternalParameters.WriteSurfaceCurMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines a unit in which the STEP file should be written.\n";
  aResult += "!Default value: MM(2). Available values: \"INCH\"(1), \"MM\"(2), \"??\"(3), "
             "\"FT\"(4), \"MI\"(5), ";
  aResult += "\"M\"(6), \"KM\"(7), \"MIL\"(8), \"UM\"(9), \"CM\"(10), \"UIN\"(11)\n";
  aResult += aScope + "write.unit :\t " + InternalParameters.WriteUnit + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!This parameter indicates which of free vertices writing mode is switch on\n";
  aResult += "!Default value: 0(\"One Compound\"). Available values: 0(\"One Compound\"), "
             "1(\"Signle Vertex\")\n";
  aResult += aScope + "write.vertex.mode :\t " + InternalParameters.WriteVertexMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Indicates whether to write sub-shape names to 'Name' attributes of STEP "
             "Representation Items\n";
  aResult += "!Default value: 0(\"OFF\"). Available values: 0(\"OFF\"), 1(\"ON\")\n";
  aResult +=
    aScope + "write.stepcaf.subshapes.name :\t " + InternalParameters.WriteSubshapeNames + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Setting up the write.colo parameter which is used to indicate write Colors or not\n";
  aResult += "!Default value: +. Available values: \"-\", \"+\"\n";
  aResult += aScope + "write.color :\t " + InternalParameters.WriteColor + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Setting up the write.name parameter which is used to indicate write Names or not\n";
  aResult += "!Default value: +. Available values: \"-\", \"+\"\n";
  aResult += aScope + "write.name :\t " + InternalParameters.WriteName + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult +=
    "!Setting up the write.layer parameter which is used to indicate write Layers or not\n";
  aResult += "!Default value: +. Available values: \"-\", \"+\"\n";
  aResult += aScope + "write.layer :\t " + InternalParameters.WriteLayer + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Setting up the write.props parameter which is used to indicate write Validation "
             "properties or not\n";
  aResult += "!Default value: +. Available values: \"-\", \"+\"\n";
  aResult += aScope + "write.props :\t " + InternalParameters.WriteProps + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Setting up the Model Type which gives you the choice of translation mode for an "
             "Open CASCADE shape that ";
  aResult += "is being translated to STEP\n";
  aResult += "!Default value: 0. Available values: 0, 1, 2, 3, 4\n";
  aResult += aScope + "write.model.type :\t " + InternalParameters.WriteModelType + "\n";
  aResult += "!\n";

  aResult += DE_ShapeFixConfigurationNode::Save();

  aResult += "!*****************************************************************************\n";

  return aResult;
}

//=================================================================================================

Handle(DE_ConfigurationNode) DESTEP_ConfigurationNode::Copy() const
{
  return new DESTEP_ConfigurationNode(*this);
}

//=================================================================================================

Handle(DE_Provider) DESTEP_ConfigurationNode::BuildProvider()
{
  return new DESTEP_Provider(this);
}

//=================================================================================================

bool DESTEP_ConfigurationNode::IsImportSupported() const
{
  return true;
}

//=================================================================================================

bool DESTEP_ConfigurationNode::IsExportSupported() const
{
  return true;
}

//=================================================================================================

TCollection_AsciiString DESTEP_ConfigurationNode::GetFormat() const
{
  return TCollection_AsciiString("STEP");
}

//=================================================================================================

TCollection_AsciiString DESTEP_ConfigurationNode::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}

//=================================================================================================

TColStd_ListOfAsciiString DESTEP_ConfigurationNode::GetExtensions() const
{
  TColStd_ListOfAsciiString anExt;
  anExt.Append("stp");
  anExt.Append("step");
  anExt.Append("stpz");
  return anExt;
}

//=================================================================================================

bool DESTEP_ConfigurationNode::CheckContent(const Handle(NCollection_Buffer)& theBuffer) const
{
  if (theBuffer.IsNull() || theBuffer->Size() < 100)
  {
    return false;
  }
  const char* aBytes = (const char*)theBuffer->Data();
  if (::strstr(aBytes, "IFC"))
  {
    return false;
  }
  if (::strstr(aBytes, "ISO-10303-21"))
  {
    // Double-check by presence of "FILE_SHEMA" statement
    const char* aPtr = ::strstr(aBytes, "FILE_SCHEMA");
    if (aPtr)
    {
      return true;
    }
  }
  return false;
}
