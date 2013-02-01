// Created on: 2007-07-17
// Created by: Alexander GRIGORIEV
// Copyright (c) 2007-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <VrmlData_Material.hxx>
#include <Precision.hxx>
#include <VrmlData_InBuffer.hxx>
#include <VrmlData_Scene.hxx>
#include <gp_XYZ.hxx>

#ifdef WNT
#define _CRT_SECURE_NO_DEPRECATE
#pragma warning (disable:4996)
#endif

IMPLEMENT_STANDARD_HANDLE  (VrmlData_Material, VrmlData_Node)
IMPLEMENT_STANDARD_RTTIEXT (VrmlData_Material, VrmlData_Node)

//=======================================================================
//function : VrmlData_Material()
//purpose  : Empty Constructor
//=======================================================================

VrmlData_Material::VrmlData_Material ()
  : myAmbientIntensity        (0.2),
    myShininess               (0.2),
    myTransparency            (0.),
    myDiffuseColor            (0.8, 0.8, 0.8, Quantity_TOC_RGB),
    myEmissiveColor           (0., 0., 0., Quantity_TOC_RGB),
    mySpecularColor           (0., 0., 0., Quantity_TOC_RGB)
{}

//=======================================================================
//function : VrmlData_Material
//purpose  : Constructor
//=======================================================================

VrmlData_Material::VrmlData_Material (const VrmlData_Scene&  theScene,
                                      const char             * theName,
                                      const Standard_Real    theAmbientIntens,
                                      const Standard_Real    theShininess,
                                      const Standard_Real    theTransparency)
  : VrmlData_Node             (theScene, theName),
    myAmbientIntensity        (theAmbientIntens < 0. ? 0.2 : theAmbientIntens),
    myShininess               (theShininess     < 0. ? 0.2 : theShininess),
    myTransparency            (theTransparency  < 0  ? 0.  : theTransparency),
    myDiffuseColor            (0.8, 0.8, 0.8, Quantity_TOC_RGB),
    myEmissiveColor           (0., 0., 0., Quantity_TOC_RGB),
    mySpecularColor           (0., 0., 0., Quantity_TOC_RGB)
{}


//=======================================================================
//function : VrmlData_Material::Clone
//purpose  : 
//=======================================================================

Handle(VrmlData_Node) VrmlData_Material::Clone
                                (const Handle(VrmlData_Node)& theOther) const
{
  Handle(VrmlData_Material) aResult =
    Handle(VrmlData_Material)::DownCast (VrmlData_Node::Clone(theOther));
  if (aResult.IsNull())
    aResult =
      new VrmlData_Material (theOther.IsNull() ? Scene() : theOther->Scene(),
                             Name());

  aResult->SetAmbientIntensity (myAmbientIntensity);
  aResult->SetShininess        (myShininess);
  aResult->SetTransparency     (myTransparency);
  aResult->SetDiffuseColor     (myDiffuseColor);
  aResult->SetEmissiveColor    (myEmissiveColor);
  aResult->SetSpecularColor    (mySpecularColor);
  return aResult;
}

//=======================================================================
//function : VrmlData_Material::Read
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_Material::Read (VrmlData_InBuffer& theBuffer)
{
  VrmlData_ErrorStatus aStatus;
  const Standard_Real aConf = 0.001 * Precision::Confusion();
  Standard_Real anIntensity[3] = { 0.2, 0.2, 0. };
  gp_XYZ        aColor[3] = {
    gp_XYZ (0.8, 0.8, 0.8),
    gp_XYZ (0.0, 0.0, 0.0),
    gp_XYZ (0.0, 0.0, 0.0)
  };
  while (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer))) {
    if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "ambientIntensity")) {
      if (OK(aStatus, Scene().ReadReal (theBuffer, anIntensity[0],
                                        Standard_False, Standard_False)))
        if (anIntensity[0] < -aConf || anIntensity[0] > 1.+aConf) {
          aStatus = VrmlData_IrrelevantNumber;
          break;
        }
    } else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "shininess")) {
      if (OK(aStatus, Scene().ReadReal (theBuffer, anIntensity[1],
                                        Standard_False, Standard_False)))
        if (anIntensity[1] < -aConf || anIntensity[1] > 1.+aConf) {
          aStatus = VrmlData_IrrelevantNumber;
          break;
        }
    } else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "transparency")) {
      if (OK(aStatus, Scene().ReadReal (theBuffer, anIntensity[2],
                                        Standard_False, Standard_False)))
        if (anIntensity[2] < -aConf || anIntensity[2] > 1.+aConf) {
          aStatus = VrmlData_IrrelevantNumber;
          break;
        }
    } else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "diffuseColor")) {
      if (OK(aStatus, Scene().ReadXYZ (theBuffer, aColor[0],
                                       Standard_False, Standard_False)))
        if (aColor[0].X() < -aConf || aColor[0].X() > 1.+aConf ||
            aColor[0].Y() < -aConf || aColor[0].Y() > 1.+aConf ||
            aColor[0].Z() < -aConf || aColor[0].Z() > 1.+aConf)
        {
          aStatus = VrmlData_IrrelevantNumber;
          break;
        }
    } else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "emissiveColor")) {
      if (OK(aStatus, Scene().ReadXYZ (theBuffer, aColor[1],
                                       Standard_False, Standard_False)))
        if (aColor[1].X() < -aConf || aColor[1].X() > 1.+aConf ||
            aColor[1].Y() < -aConf || aColor[1].Y() > 1.+aConf ||
            aColor[1].Z() < -aConf || aColor[1].Z() > 1.+aConf)
        {
          aStatus = VrmlData_IrrelevantNumber;
          break;
        }
    } else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "specularColor")) {
      if (OK(aStatus, Scene().ReadXYZ (theBuffer, aColor[2],
                                       Standard_False, Standard_False)))
        if (aColor[2].X() < -aConf || aColor[2].X() > 1.+aConf ||
            aColor[2].Y() < -aConf || aColor[2].Y() > 1.+aConf ||
            aColor[2].Z() < -aConf || aColor[2].Z() > 1.+aConf)
        {
          aStatus = VrmlData_IrrelevantNumber;
          break;
        }
    } else
      break;

    if (!OK(aStatus))
      break;
  }
          
  // Read the terminating (closing) brace
  if (OK(aStatus))
    aStatus = readBrace (theBuffer);

  // Store the values in the Material node instance
  if (OK(aStatus)) {
    myAmbientIntensity  = anIntensity[0];
    myShininess         = anIntensity[1];
    myTransparency      = anIntensity[2];
    myDiffuseColor.SetValues  (aColor[0].X(), aColor[0].Y(), aColor[0].Z(),
                               Quantity_TOC_RGB);
    myEmissiveColor.SetValues (aColor[1].X(), aColor[1].Y(), aColor[1].Z(),
                               Quantity_TOC_RGB);
    mySpecularColor.SetValues (aColor[2].X(), aColor[2].Y(), aColor[2].Z(),
                               Quantity_TOC_RGB);
  }
  return aStatus;
}

//=======================================================================
//function : VrmlData_Material::Write
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_Material::Write (const char * thePrefix) const
{
  VrmlData_ErrorStatus aStatus;
  const VrmlData_Scene& aScene = Scene();
  static char header[] = "Material {";
  if (aScene.IsDummyWrite() == Standard_False &&
      OK (aStatus, aScene.WriteLine (thePrefix, header, GlobalIndent())))
  {
    char buf[128];
    Standard_Real val[3];
    Quantity_TypeOfColor bidType (Quantity_TOC_RGB);
    const Standard_Real aConf (0.001 * Precision::Confusion());

    if (OK(aStatus) && fabs(myAmbientIntensity - 0.2) > aConf) {
      Sprintf (buf, "%.6g", myAmbientIntensity);
      aStatus = aScene.WriteLine ("ambientIntensity ", buf);
    }
    if (OK(aStatus)) {
      myDiffuseColor.Values  (val[0], val[1], val[2], bidType);
      if ((val[0] - 0.8) * (val[0] - 0.8) +
          (val[1] - 0.8) * (val[1] - 0.8) +
          (val[2] - 0.8) * (val[2] - 0.8) > 1e-7)
      {
        Sprintf (buf, "%.6g %.6g %.6g", val[0], val[1], val[2]);
        aStatus = aScene.WriteLine ("diffuseColor     ", buf);
      }
    }
    if (OK(aStatus)) {
      myEmissiveColor.Values  (val[0], val[1], val[2], bidType);
      if (val[0] * val[0] + val[1] * val[1] + val[2] * val[2] > 1e-7) {      
        Sprintf (buf, "%.6g %.6g %.6g", val[0], val[1], val[2]);
        aStatus = aScene.WriteLine ("emissiveColor    ", buf);
      }
    }
    if (OK(aStatus) && fabs(myShininess - 0.2) > aConf) {
      Sprintf (buf, "%.6g", myShininess);
      aStatus = aScene.WriteLine ("shininess        ", buf);
    }
    if (OK(aStatus)) {
      mySpecularColor.Values  (val[0], val[1], val[2], bidType);
      if (val[0] * val[0] + val[1] * val[1] + val[2] * val[2] > 1e-7) {      
        Sprintf (buf, "%.6g %.6g %.6g", val[0], val[1], val[2]);
        aStatus = aScene.WriteLine ("specularColor    ", buf);
      }
    }
    if (OK(aStatus) && myTransparency > aConf) {
      Sprintf (buf, "%.6g", myTransparency);
      aStatus = aScene.WriteLine ("transparency     ", buf);
    }

    aStatus = WriteClosing();
  }
  return aStatus;
}

//=======================================================================
//function : IsDefault
//purpose  : 
//=======================================================================

Standard_Boolean VrmlData_Material::IsDefault () const
{
  const Standard_Real aConf (0.001 * Precision::Confusion());
  Standard_Boolean aResult (Standard_False);
  if (fabs(myAmbientIntensity - 0.2) < aConf &&
      fabs(myShininess - 0.2)        < aConf &&
      myTransparency                 < aConf)
  {
    Standard_Real val[3][3];
    Quantity_TypeOfColor bidType (Quantity_TOC_RGB);
    myDiffuseColor.Values  (val[0][0], val[0][1], val[0][2], bidType);
    myEmissiveColor.Values (val[1][0], val[1][1], val[1][2], bidType);
    mySpecularColor.Values (val[2][0], val[2][1], val[2][2], bidType);
    aResult = (((val[0][0] - 0.8)*(val[0][0] - 0.8) +
                (val[0][1] - 0.8)*(val[0][1] - 0.8) +
                (val[0][2] - 0.8)*(val[0][2] - 0.8) < 1e-7) &&
               (val[1][0] * val[1][0] +
                val[1][1] * val[1][0] +
                val[1][2] * val[1][0] < 1e-7) &&
               (val[2][0] * val[2][0] +
                val[2][1] * val[2][0] +
                val[2][2] * val[2][0] < 1e-7));
  }
  return aResult;
}


