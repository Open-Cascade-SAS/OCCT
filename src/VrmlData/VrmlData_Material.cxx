// Created on: 2007-07-17
// Created by: Alexander GRIGORIEV
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#include <VrmlData_Material.hxx>
#include <Precision.hxx>
#include <VrmlData_InBuffer.hxx>
#include <VrmlData_Scene.hxx>
#include <gp_XYZ.hxx>

IMPLEMENT_STANDARD_RTTIEXT(VrmlData_Material, VrmlData_Node)

#ifdef _MSC_VER
  #define _CRT_SECURE_NO_DEPRECATE
  #pragma warning(disable : 4996)
#endif

namespace
{
static const Standard_Real THE_MAT_PREC = 0.001 * Precision::Confusion();

//=================================================================================================

static bool isValidValue(Standard_Real theVal)
{
  return theVal >= -THE_MAT_PREC && theVal <= 1.0 + THE_MAT_PREC;
}

//=================================================================================================

static bool isValidColor(const gp_XYZ& theVec3)
{
  return isValidValue(theVec3.X()) && isValidValue(theVec3.Y()) && isValidValue(theVec3.Z());
}

//=================================================================================================

static bool parseColor(VrmlData_ErrorStatus& theStatus,
                       VrmlData_InBuffer&    theBuffer,
                       gp_XYZ&               theColor,
                       const VrmlData_Scene& theScene)
{
  if (!VrmlData_Node::OK(theStatus, VrmlData_Scene::ReadLine(theBuffer)))
  {
    return false;
  }

  bool isArray = *theBuffer.LinePtr == '[';
  if (isArray)
  {
    ++theBuffer.LinePtr;
  }
  theStatus = theScene.ReadXYZ(theBuffer, theColor, Standard_False, Standard_False);
  if (isArray)
  {
    if (VrmlData_Node::OK(theStatus, VrmlData_Scene::ReadLine(theBuffer))
        && *theBuffer.LinePtr == ']')
    {
      ++theBuffer.LinePtr;
    }
    else
    {
      theStatus = VrmlData_VrmlFormatError;
    }
  }
  if (!isValidColor(theColor))
  {
    theStatus = VrmlData_IrrelevantNumber;
    return false;
  }
  return true;
}

//=================================================================================================

static bool parseScalar(VrmlData_ErrorStatus& theStatus,
                        VrmlData_InBuffer&    theBuffer,
                        Standard_Real&        theValue,
                        const VrmlData_Scene& theScene)
{
  if (!VrmlData_Node::OK(theStatus, VrmlData_Scene::ReadLine(theBuffer)))
  {
    return false;
  }

  bool isArray = *theBuffer.LinePtr == '[';
  if (isArray)
  {
    ++theBuffer.LinePtr;
  }

  theStatus = theScene.ReadReal(theBuffer, theValue, Standard_False, Standard_False);
  if (isArray)
  {
    if (VrmlData_Node::OK(theStatus, VrmlData_Scene::ReadLine(theBuffer))
        && *theBuffer.LinePtr == ']')
    {
      ++theBuffer.LinePtr;
    }
    else
    {
      theStatus = VrmlData_VrmlFormatError;
    }
  }
  if (!isValidValue(theValue))
  {
    theStatus = VrmlData_IrrelevantNumber;
    return false;
  }
  return true;
}
} // namespace

//=======================================================================
// function : VrmlData_Material()
// purpose  : Empty Constructor
//=======================================================================

VrmlData_Material::VrmlData_Material()
    : myAmbientIntensity(0.2),
      myShininess(0.2),
      myTransparency(0.),
      myAmbientColor(0., 0., 0., Quantity_TOC_RGB),
      myDiffuseColor(0.8, 0.8, 0.8, Quantity_TOC_sRGB),
      myEmissiveColor(Quantity_NOC_BLACK),
      mySpecularColor(Quantity_NOC_BLACK)
{
}

//=================================================================================================

VrmlData_Material::VrmlData_Material(const VrmlData_Scene& theScene,
                                     const char*           theName,
                                     const Standard_Real   theAmbientIntens,
                                     const Standard_Real   theShininess,
                                     const Standard_Real   theTransparency)
    : VrmlData_Node(theScene, theName),
      myAmbientIntensity(theAmbientIntens < 0. ? 0.2 : theAmbientIntens),
      myShininess(theShininess < 0. ? 0.2 : theShininess),
      myTransparency(theTransparency < 0 ? 0. : theTransparency),
      myAmbientColor(0., 0., 0., Quantity_TOC_RGB),
      myDiffuseColor(0.8, 0.8, 0.8, Quantity_TOC_sRGB),
      myEmissiveColor(Quantity_NOC_BLACK),
      mySpecularColor(Quantity_NOC_BLACK)
{
}

//=================================================================================================

Handle(VrmlData_Node) VrmlData_Material::Clone(const Handle(VrmlData_Node)& theOther) const
{
  Handle(VrmlData_Material) aResult =
    Handle(VrmlData_Material)::DownCast(VrmlData_Node::Clone(theOther));
  if (aResult.IsNull())
    aResult = new VrmlData_Material(theOther.IsNull() ? Scene() : theOther->Scene(), Name());

  aResult->SetAmbientIntensity(myAmbientIntensity);
  aResult->SetShininess(myShininess);
  aResult->SetTransparency(myTransparency);
  aResult->SetAmbientColor(myAmbientColor);
  aResult->SetDiffuseColor(myDiffuseColor);
  aResult->SetEmissiveColor(myEmissiveColor);
  aResult->SetSpecularColor(mySpecularColor);
  return aResult;
}

//=================================================================================================

VrmlData_ErrorStatus VrmlData_Material::Read(VrmlData_InBuffer& theBuffer)
{
  VrmlData_ErrorStatus aStatus;
  Standard_Real        anIntensity[3] = {0.2, 0.2, 0.};
  gp_XYZ               aColor[4]      = {gp_XYZ(0.0, 0.0, 0.0),
                                         gp_XYZ(0.8, 0.8, 0.8),
                                         gp_XYZ(0.0, 0.0, 0.0),
                                         gp_XYZ(0.0, 0.0, 0.0)};
  while (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
  {
    if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "ambientIntensity"))
    {
      parseScalar(aStatus, theBuffer, anIntensity[0], Scene());
    }
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "shininess"))
    {
      parseScalar(aStatus, theBuffer, anIntensity[1], Scene());
    }
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "transparency"))
    {
      parseScalar(aStatus, theBuffer, anIntensity[2], Scene());
    }
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "ambientColor"))
    {
      parseColor(aStatus, theBuffer, aColor[0], Scene());
    }
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "diffuseColor"))
    {
      parseColor(aStatus, theBuffer, aColor[1], Scene());
    }
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "emissiveColor"))
    {
      parseColor(aStatus, theBuffer, aColor[2], Scene());
    }
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "specularColor"))
    {
      parseColor(aStatus, theBuffer, aColor[3], Scene());
    }
    else
      break;

    if (!OK(aStatus))
      break;
  }

  // Read the terminating (closing) brace
  if (OK(aStatus))
    aStatus = readBrace(theBuffer);

  // Store the values in the Material node instance
  if (OK(aStatus))
  {
    myAmbientIntensity = anIntensity[0];
    myShininess        = anIntensity[1];
    myTransparency     = anIntensity[2];
    myAmbientColor.SetValues(aColor[0].X(), aColor[0].Y(), aColor[0].Z(), Quantity_TOC_sRGB);
    myDiffuseColor.SetValues(aColor[1].X(), aColor[1].Y(), aColor[1].Z(), Quantity_TOC_sRGB);
    myEmissiveColor.SetValues(aColor[2].X(), aColor[2].Y(), aColor[2].Z(), Quantity_TOC_sRGB);
    mySpecularColor.SetValues(aColor[3].X(), aColor[3].Y(), aColor[3].Z(), Quantity_TOC_sRGB);
  }
  return aStatus;
}

//=================================================================================================

VrmlData_ErrorStatus VrmlData_Material::Write(const char* thePrefix) const
{
  VrmlData_ErrorStatus  aStatus  = VrmlData_StatusOK;
  const VrmlData_Scene& aScene   = Scene();
  static char           header[] = "Material {";
  if (aScene.IsDummyWrite() == Standard_False
      && OK(aStatus, aScene.WriteLine(thePrefix, header, GlobalIndent())))
  {
    char                       buf[128];
    Standard_Real              val[3];
    const Quantity_TypeOfColor bidType = Quantity_TOC_sRGB;
    constexpr Standard_Real    aConf(0.001 * Precision::Confusion());

    if (OK(aStatus) && fabs(myAmbientIntensity - 0.2) > aConf)
    {
      Sprintf(buf, "%.6g", myAmbientIntensity);
      aStatus = aScene.WriteLine("ambientIntensity ", buf);
    }
    if (OK(aStatus))
    {
      myDiffuseColor.Values(val[0], val[1], val[2], bidType);
      if ((val[0] - 0.8) * (val[0] - 0.8) + (val[1] - 0.8) * (val[1] - 0.8)
            + (val[2] - 0.8) * (val[2] - 0.8)
          > 1e-7)
      {
        Sprintf(buf, "%.6g %.6g %.6g", val[0], val[1], val[2]);
        aStatus = aScene.WriteLine("diffuseColor     ", buf);
      }
    }
    if (OK(aStatus))
    {
      myEmissiveColor.Values(val[0], val[1], val[2], bidType);
      if (val[0] * val[0] + val[1] * val[1] + val[2] * val[2] > 1e-7)
      {
        Sprintf(buf, "%.6g %.6g %.6g", val[0], val[1], val[2]);
        aStatus = aScene.WriteLine("emissiveColor    ", buf);
      }
    }
    if (OK(aStatus) && fabs(myShininess - 0.2) > aConf)
    {
      Sprintf(buf, "%.6g", myShininess);
      aStatus = aScene.WriteLine("shininess        ", buf);
    }
    if (OK(aStatus))
    {
      mySpecularColor.Values(val[0], val[1], val[2], bidType);
      if (val[0] * val[0] + val[1] * val[1] + val[2] * val[2] > 1e-7)
      {
        Sprintf(buf, "%.6g %.6g %.6g", val[0], val[1], val[2]);
        aStatus = aScene.WriteLine("specularColor    ", buf);
      }
    }
    if (OK(aStatus) && myTransparency > aConf)
    {
      Sprintf(buf, "%.6g", myTransparency);
      aStatus = aScene.WriteLine("transparency     ", buf);
    }

    aStatus = WriteClosing();
  }
  return aStatus;
}

//=================================================================================================

Standard_Boolean VrmlData_Material::IsDefault() const
{
  constexpr Standard_Real aConf(0.001 * Precision::Confusion());
  Standard_Boolean        aResult(Standard_False);
  if (fabs(myAmbientIntensity - 0.2) < aConf && fabs(myShininess - 0.2) < aConf
      && myTransparency < aConf)
  {
    Standard_Real              val[3][3];
    const Quantity_TypeOfColor bidType = Quantity_TOC_sRGB;
    myDiffuseColor.Values(val[0][0], val[0][1], val[0][2], bidType);
    myEmissiveColor.Values(val[1][0], val[1][1], val[1][2], bidType);
    mySpecularColor.Values(val[2][0], val[2][1], val[2][2], bidType);
    aResult = (((val[0][0] - 0.8) * (val[0][0] - 0.8) + (val[0][1] - 0.8) * (val[0][1] - 0.8)
                  + (val[0][2] - 0.8) * (val[0][2] - 0.8)
                < 1e-7)
               && (val[1][0] * val[1][0] + val[1][1] * val[1][0] + val[1][2] * val[1][0] < 1e-7)
               && (val[2][0] * val[2][0] + val[2][1] * val[2][0] + val[2][2] * val[2][0] < 1e-7));
  }
  return aResult;
}
