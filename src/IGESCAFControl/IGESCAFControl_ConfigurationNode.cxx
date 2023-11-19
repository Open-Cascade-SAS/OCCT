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

#include <IGESCAFControl_ConfigurationNode.hxx>

#include <DE_ConfigurationContext.hxx>
#include <DE_PluginHolder.hxx>
#include <IGESCAFControl_Provider.hxx>
#include <NCollection_Buffer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESCAFControl_ConfigurationNode, DE_ConfigurationNode)

namespace
{
  static const TCollection_AsciiString& THE_CONFIGURATION_SCOPE()
  {
    static const TCollection_AsciiString aScope = "provider";
    return aScope;
  }

  // Wrapper to auto-load DE component
  DE_PluginHolder<IGESCAFControl_ConfigurationNode> THE_OCCT_IGES_COMPONENT_PLUGIN;
}

//=======================================================================
// function : IGESCAFControl_ConfigurationNode
// purpose  : 
//=======================================================================
IGESCAFControl_ConfigurationNode::IGESCAFControl_ConfigurationNode() :
  DE_ConfigurationNode()
{}

//=======================================================================
// function : IGESCAFControl_ConfigurationNode
// purpose  : 
//=======================================================================
IGESCAFControl_ConfigurationNode::IGESCAFControl_ConfigurationNode(const Handle(IGESCAFControl_ConfigurationNode)& theNode)
  :DE_ConfigurationNode(theNode)
{
  InternalParameters = theNode->InternalParameters;
}

//=======================================================================
// function : Load
// purpose  : 
//=======================================================================
bool IGESCAFControl_ConfigurationNode::Load(const Handle(DE_ConfigurationContext)& theResource)
{
  TCollection_AsciiString aScope = THE_CONFIGURATION_SCOPE() + "." + GetFormat() + "." + GetVendor();

  InternalParameters.ReadBSplineContinuity = (ReadMode_BSplineContinuity)
    theResource->IntegerVal("read.iges.bspline.continuity", InternalParameters.ReadBSplineContinuity, aScope);
  InternalParameters.ReadPrecisionMode = (ReadMode_Precision)
    theResource->IntegerVal("read.precision.mode", InternalParameters.ReadPrecisionMode, aScope);
  InternalParameters.ReadPrecisionVal =
    theResource->RealVal("read.precision.val", InternalParameters.ReadPrecisionVal, aScope);
  InternalParameters.ReadMaxPrecisionMode = (ReadMode_MaxPrecision)
    theResource->IntegerVal("read.maxprecision.mode", InternalParameters.ReadMaxPrecisionMode, aScope);
  InternalParameters.ReadMaxPrecisionVal =
    theResource->RealVal("read.maxprecision.val", InternalParameters.ReadMaxPrecisionVal, aScope);
  InternalParameters.ReadSameParamMode = 
    theResource->BooleanVal("read.stdsameparameter.mode", InternalParameters.ReadSameParamMode, aScope);
  InternalParameters.ReadSurfaceCurveMode = (ReadMode_SurfaceCurve)
    theResource->IntegerVal("read.surfacecurve.mode", InternalParameters.ReadSurfaceCurveMode, aScope);
  InternalParameters.EncodeRegAngle =
    theResource->RealVal("read.encoderegularity.angle", InternalParameters.EncodeRegAngle, aScope);

  InternalParameters.ReadApproxd1 = 
    theResource->BooleanVal("read.bspline.approxd1.mode", InternalParameters.ReadApproxd1, aScope);
  InternalParameters.ReadResourceName =
    theResource->StringVal("read.resource.name", InternalParameters.ReadResourceName, aScope);
  InternalParameters.ReadSequence =
    theResource->StringVal("read.sequence", InternalParameters.ReadSequence, aScope);
  InternalParameters.ReadFaultyEntities = 
    theResource->BooleanVal("read.fau_lty.entities", InternalParameters.ReadFaultyEntities, aScope);
  InternalParameters.ReadOnlyVisible = 
    theResource->BooleanVal("read.onlyvisible", InternalParameters.ReadOnlyVisible, aScope);
  InternalParameters.ReadColor = 
    theResource->BooleanVal("read.color", InternalParameters.ReadColor, aScope);
  InternalParameters.ReadName = 
    theResource->BooleanVal("read.name", InternalParameters.ReadName, aScope);
  InternalParameters.ReadLayer = 
    theResource->BooleanVal("read.layer", InternalParameters.ReadLayer, aScope);

  InternalParameters.WriteBRepMode = (WriteMode_BRep)
    theResource->IntegerVal("write.brep.mode", InternalParameters.WriteBRepMode, aScope);
  InternalParameters.WriteConvertSurfaceMode = (WriteMode_ConvertSurface)
    theResource->IntegerVal("write.convertsurface.mode", InternalParameters.WriteConvertSurfaceMode, aScope);
  InternalParameters.WriteUnit = (UnitsMethods_LengthUnit)
    theResource->IntegerVal("write.unit", InternalParameters.WriteUnit, aScope);
  InternalParameters.WriteHeaderAuthor =
    theResource->StringVal("write.header.author", InternalParameters.WriteHeaderAuthor, aScope);
  InternalParameters.WriteHeaderCompany =
    theResource->StringVal("write.header.company", InternalParameters.WriteHeaderCompany, aScope);
  InternalParameters.WriteHeaderProduct =
    theResource->StringVal("write.header.product", InternalParameters.WriteHeaderProduct, aScope);
  InternalParameters.WriteHeaderReciever =
    theResource->StringVal("write.header.receiver", InternalParameters.WriteHeaderReciever, aScope);
  InternalParameters.WriteResourceName =
    theResource->StringVal("write.resource.name", InternalParameters.WriteResourceName, aScope);
  InternalParameters.WriteSequence =
    theResource->StringVal("write.sequence", InternalParameters.WriteSequence, aScope);
  InternalParameters.WritePrecisionMode = (WriteMode_PrecisionMode)
    theResource->IntegerVal("write.precision.mode", InternalParameters.WritePrecisionMode, aScope);
  InternalParameters.WritePrecisionVal =
    theResource->RealVal("write.precision.val", InternalParameters.WritePrecisionVal, aScope);
  InternalParameters.WritePlaneMode = (WriteMode_PlaneMode)
    theResource->IntegerVal("write.plane.mode", InternalParameters.WritePlaneMode, aScope);
  InternalParameters.WriteOffsetMode = 
    theResource->BooleanVal("write.offset", InternalParameters.WriteOffsetMode, aScope);
  InternalParameters.WriteColor = 
    theResource->BooleanVal("write.color", InternalParameters.WriteColor, aScope);
  InternalParameters.WriteName = 
    theResource->BooleanVal("write.name", InternalParameters.WriteName, aScope);
  InternalParameters.WriteLayer = 
    theResource->BooleanVal("write.layer", InternalParameters.WriteLayer, aScope);

  return true;
}

//=======================================================================
// function : Save
// purpose  :
//=======================================================================
TCollection_AsciiString IGESCAFControl_ConfigurationNode::Save() const
{
  TCollection_AsciiString aResult;
  aResult += "!*****************************************************************************\n";
  aResult = aResult + "!Configuration Node " + " Vendor: " + GetVendor() + " Format: " + GetFormat() + "\n";
  TCollection_AsciiString aScope = THE_CONFIGURATION_SCOPE() + "." + GetFormat() + "." + GetVendor() + ".";

  aResult += "!\n";
  aResult += "!Common parameters:\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Manages the continuity of BSpline curves (IGES entities 106, 112 and 126) ";
  aResult += "after translation to Open CASCADE Technology\n";
  aResult += "!Default value: 1. Available values: 0, 1, 2\n";
  aResult += aScope + "read.iges.bspline.continuity :\t " + InternalParameters.ReadBSplineContinuity + "\n";
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
  aResult += "!Default value: \"Preferred\"(0). Available values: \"Preferred\"(0), \"Forced\"(1)\n";
  aResult += aScope + "read.maxprecision.mode :\t " + InternalParameters.ReadMaxPrecisionMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the maximum allowable tolerance (in internal units, which are specified in xstep.cascade.unit)";
  aResult += " of the shape\n";
  aResult += "!Default value: 1. Available values: any real positive (non null) value\n";
  aResult += aScope + "read.maxprecision.val :\t " + InternalParameters.ReadMaxPrecisionVal + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the using of BRepLib::SameParameter\n";
  aResult += "!Default value: \"Off\"(0). Available values: \"Off\"(0), \"On\"(0)\n";
  aResult += aScope + "read.stdsameparameter.mode :\t " + InternalParameters.ReadSameParamMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Preference for the computation of curves in case of 2D/3D inconsistency in an entity ";
  aResult += "which has both 2D and 3D representations.\n";
  aResult += "!Default value: \"Default\"(0). Available values: \"Default\"(0), \"2DUse_Preferred\"(2), ";
  aResult += "\"2DUse_Forced\"(-2), \"3DUse_Preferred\"(3), \"3DUse_Forced\"(-3)\n";
  aResult += aScope + "read.surfacecurve.mode :\t " + InternalParameters.ReadSurfaceCurveMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!This parameter is used within the BRepLib::EncodeRegularity() function ";
  aResult += "which is called for a shape read ";
  aResult += "from an IGES or a STEP file at the end of translation process.This function sets the regularity flag of";
  aResult += " an edge in a shell when this edge is shared by two faces.This flag shows the continuity, ";
  aResult += "which these two faces are connected with at that edge.\n";
  aResult += "!Default value (in degrees): 0.57295779513. Available values: <double>\n";
  aResult += aScope + "read.encoderegularity.angle :\t " + InternalParameters.EncodeRegAngle + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Read parameters:\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!If set to True, it affects the translation of bspline curves of degree 1 from IGES: ";
  aResult += "these curves(which geometrically are polylines) are split by duplicated points, and the translator ";
  aResult += "attempts to convert each of the obtained parts to a bspline of a higher continuity\n";
  aResult += "!Default value: \"Off\"(0). Available values: \"Off\"(0), \"On\"(1)\n";
  aResult += aScope + "read.bspline.approxd1.mode :\t " + InternalParameters.ReadApproxd1 + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the name of the resource file\n";
  aResult += "!Default value: \"IGES\". Available values: <string>\n";
  aResult += aScope + "read.resource.name :\t " + InternalParameters.ReadResourceName + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the name of the sequence of operators\n";
  aResult += "!Default value: \"FromIGES\". Available values: <string>\n";
  aResult += aScope + "read.sequence :\t " + InternalParameters.ReadSequence + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Parameter for reading fa-iled entities\n";
  aResult += "!Default value: \"Off\"(0). Available values: \"Off\"(0), \"On\"(1)\n";
  aResult += aScope + "read.fau_lty.entities :\t " + InternalParameters.ReadFaultyEntities + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Controls transferring invisible sub entities which logically depend on the grouping entities\n";
  aResult += "!Default value: \"Off\"(0). Available values: \"Off\"(0), \"On\"(1)\n";
  aResult += aScope + "read.onlyvisible :\t " + InternalParameters.ReadOnlyVisible + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Setting up the ColorMode parameter which is used to indicate read Colors or not\n";
  aResult += "!Default value: 1. Available values: 0, 1\n";
  aResult += aScope + "read.color :\t " + InternalParameters.ReadColor + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Setting up the NameMode parameter which is used to indicate read Names or not\n";
  aResult += "!Default value: 1. Available values: 0, 1\n";
  aResult += aScope + "read.name :\t " + InternalParameters.ReadName + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Setting up the LayerMode parameter which is used to indicate read Layers or not\n";
  aResult += "!Default value: 1. Available values: 0, 1\n";
  aResult += aScope + "read.layer :\t " + InternalParameters.ReadLayer + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Write parameters:\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Flag to define entities type to write\n";
  aResult += "!Default value: \"Faces\"(0). Available values: \"Faces\"(0), \"BRep\"(1)\n";
  aResult += aScope + "write.brep.mode :\t " + InternalParameters.WriteBRepMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!When writing to IGES in the BRep mode, this parameter indicates whether elementary surfaces";
  aResult += "(cylindrical, conical, spherical, and toroidal) are converted into corresponding IGES 5.3 entities";
  aResult += "(if the value of a parameter value is On), or written as surfaces of revolution(by default)\n";
  aResult += "!Default value: \"Off\"(0). Available values: \"Off\"(0), \"On\"(1)\n";
  aResult += aScope + "write.convertsurface.mode :\t " + InternalParameters.WriteConvertSurfaceMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Allows choosing the unit. The default unit for Open CASCADE Technology is \"MM\" (millimeter).";
  aResult += "You can choose to write a file into any unit accepted by IGES\n";
  aResult += "!Default value: MM(2). Available values: \"INCH\"(1), \"MM\"(2), \"??\"(3), \"FT\"(4), \"MI\"(5), ";
  aResult += "\"M\"(6), \"KM\"(7), \"MIL\"(8), \"UM\"(9), \"CM\"(10), \"UIN\"(11)\n";
  aResult += aScope + "write.unit :\t " + InternalParameters.WriteUnit + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Gives the name of the author of the file\n";
  aResult += "!Default value: {System name of the user}. Available values: <string>\n";
  aResult += aScope + "write.header.author :\t " + InternalParameters.WriteHeaderAuthor + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Gives the name of the sending company\n";
  aResult += "!Default value: ""(empty). Available values: <string>\n";
  aResult += aScope + "write.header.company :\t " + InternalParameters.WriteHeaderCompany + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Gives the name of the sending product\n";
  aResult += "!Default value: \"CAS.CADE IGES processor Vx.x\"";
  aResult += "(where x.x means the current version of Open CASCADE Technology)";
  aResult += "Available values : <string>\n";
  aResult += aScope + "write.header.product :\t " + InternalParameters.WriteHeaderProduct + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Gives the name of the receiving company\n";
  aResult += "!Default value: ""(empty). Available values: <string>\n";
  aResult += aScope + "write.header.receiver :\t " + InternalParameters.WriteHeaderReciever + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the name of the resource file\n";
  aResult += "!Default value: \"IGES\". Available values: <string>\n";
  aResult += aScope + "write.resource.name :\t " + InternalParameters.WriteResourceName + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Defines the name of the sequence of operators\n";
  aResult += "!Default value: \"To\". Available values: <string>\n";
  aResult += aScope + "write.sequence :\t " + InternalParameters.WriteSequence + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Specifies the mode of writing the resolution value into the IGES file\n";
  aResult += "!Default value: Average(0). Available values: \"Least\"(-1), \"Average\"(0), ";
  aResult += "\"Greatest\"(1), \"Session\"(2)\n";
  aResult += aScope + "write.precision.mode :\t " + InternalParameters.WritePrecisionMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!This parameter gives the resolution value for an IGES file when the write.precision.mode parameter value is 1\n";
  aResult += "!Default value: 0.0001. Available values: any real positive (non null) value\n";
  aResult += aScope + "write.precision.val :\t " + InternalParameters.WritePrecisionVal + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Writing planes mode\n";
  aResult += "!Default value: \"Plane\"(0). Available values: \"Plane\"(0), \"BSpline\"(1)\n";
  aResult += aScope + "write.plane.mode :\t " + InternalParameters.WritePlaneMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Writing offset curves like BSplines\n";
  aResult += "!Default value: \"Off\"(0). Available values: \"Off\"(0), \"On\"(1)\n";
  aResult += aScope + "write.offset :\t " + InternalParameters.WriteOffsetMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Setting up the ColorMode parameter which is used to indicate write Colors or not\n";
  aResult += "!Default value: 1. Available values: 0, 1\n";
  aResult += aScope + "write.color :\t " + InternalParameters.WriteColor + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Setting up the NameMode parameter which is used to indicate write Names or not\n";
  aResult += "!Default value: 1. Available values: 0, 1\n";
  aResult += aScope + "write.name :\t " + InternalParameters.WriteName + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Setting up the LayerMode parameter which is used to indicate write Layers or not\n";
  aResult += "!Default value: 1. Available values: 0, 1\n";
  aResult += aScope + "write.layer :\t " + InternalParameters.WriteLayer + "\n";
  aResult += "!\n";

  aResult += "!*****************************************************************************\n";
  return aResult;
}

//=======================================================================
// function : Copy
// purpose  :
//=======================================================================
Handle(DE_ConfigurationNode) IGESCAFControl_ConfigurationNode::Copy() const
{
  return new IGESCAFControl_ConfigurationNode(*this);
}

//=======================================================================
// function : BuildProvider
// purpose  :
//=======================================================================
Handle(DE_Provider) IGESCAFControl_ConfigurationNode::BuildProvider()
{
  return new IGESCAFControl_Provider(this);
}

//=======================================================================
// function : IsImportSupported
// purpose  :
//=======================================================================
bool IGESCAFControl_ConfigurationNode::IsImportSupported() const
{
  return true;
}

//=======================================================================
// function : IsExportSupported
// purpose  :
//=======================================================================
bool IGESCAFControl_ConfigurationNode::IsExportSupported() const
{
  return true;
}

//=======================================================================
// function : GetFormat
// purpose  :
//=======================================================================
TCollection_AsciiString IGESCAFControl_ConfigurationNode::GetFormat() const
{
  return TCollection_AsciiString("IGES");
}

//=======================================================================
// function : GetVendor
// purpose  :
//=======================================================================
TCollection_AsciiString IGESCAFControl_ConfigurationNode::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}

//=======================================================================
// function : GetExtensions
// purpose  :
//=======================================================================
TColStd_ListOfAsciiString IGESCAFControl_ConfigurationNode::GetExtensions() const
{
  TColStd_ListOfAsciiString anExt;
  anExt.Append("igs");
  anExt.Append("iges");
  return anExt;
}

//=======================================================================
// function : CheckContent
// purpose  :
//=======================================================================
bool IGESCAFControl_ConfigurationNode::CheckContent(const Handle(NCollection_Buffer)& theBuffer) const
{
  if (theBuffer.IsNull() || theBuffer->Size() < 83)
  {
    return false;
  }
  const char* aBytes = (const char*)theBuffer->Data();
  if (aBytes[72] == 'S')
  {
    const char* aPtr = aBytes + 73;
    while (aPtr < aBytes + 80 && (*aPtr == ' ' || *aPtr == '0'))
    {
      aPtr++;
    }
    if (*aPtr == '1' && !::isalnum((unsigned char)*++aPtr))
    {
      return true;
    }
  }
  return false;
}
