// Copyright (c) 2020 OPEN CASCADE SAS
//
// This file is part of the examples of the Open CASCADE Technology software library.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

#include "Viewer3dSamples.h"

#include "MakeBottle.h"

#include <AIS_Shape.hxx>
#include <AIS_ViewCube.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <V3d_SpotLight.hxx>
#include <V3d_PositionalLight.hxx>
#include <V3d_DirectionalLight.hxx>
#include <V3d_AmbientLight.hxx>

void Viewer3dSamples::ExecuteSample (const TCollection_AsciiString& theSampleName)
{
  Standard_Boolean anIsSamplePresent = Standard_True;
  FindSourceCode(theSampleName);
  if (theSampleName == "SpotLight3dSample")
    SpotLight3dSample();
  else if (theSampleName == "PositionalLight3dSample")
    PositionalLight3dSample();
  else if (theSampleName == "DirectionalLight3dSample")
    DirectionalLight3dSample();
  else if (theSampleName == "AmbientLight3dSample")
    AmbientLight3dSample();
  else if (theSampleName == "ClearLight3dSample")
    ClearLight3dSample();
  else if (theSampleName == "VerticesSelect3dSample")
    VerticesSelect3dSample();
  else if (theSampleName == "EdgesSelect3dSample")
    EdgesSelect3dSample();
  else if (theSampleName == "FacesSelect3dSample")
    FacesSelect3dSample();
  else if (theSampleName == "NeutralPointSelect3dSample")
    NeutralPointSelect3dSample();
  else if (theSampleName == "WireFramePresentation3dSample")
    WireFramePresentation3dSample();
  else if (theSampleName == "ShadingPresentation3dSample")
    ShadingPresentation3dSample();
  else if (theSampleName == "RedColorPresentation3dSample")
    RedColorPresentation3dSample();
  else if (theSampleName == "GrayColorPresentation3dSample")
    GrayColorPresentation3dSample();
  else if (theSampleName == "PlasticPresentation3dSample")
    PlasticPresentation3dSample();
  else if (theSampleName == "BronzePresentation3dSample")
    BronzePresentation3dSample();
  else if (theSampleName == "OpaquePresentation3dSample")
    OpaquePresentation3dSample();
  else if (theSampleName == "HalfTransparencyPresentation3dSample")
    HalfTransparencyPresentation3dSample();
  else if (theSampleName == "VboOn3dSample")
    VboOn3dSample();
  else if (theSampleName == "VboOff3dSample")
    VboOff3dSample();
  else
  {
    myResult << "No function found: " << theSampleName;
    myCode += TCollection_AsciiString("No function found: ") + theSampleName;
    anIsSamplePresent = Standard_False;
  }
  myIsProcessed = anIsSamplePresent;
}

void Viewer3dSamples::AppendBottle()
{
  TopoDS_Shape aBottle = MakeBottle(50, 70, 30);
  Handle(AIS_InteractiveObject) aShape = new AIS_Shape(aBottle);
  myObject3d.Append(aShape);
  Handle(AIS_ViewCube) aViewCube = new AIS_ViewCube();
  myObject3d.Append(aViewCube);
  myResult << "A bottle shape was created." << std::endl;
}

void Viewer3dSamples::ClearExtra()
{
  NeutralPointSelect3dSample();
  VboOff3dSample();
  ClearLight3dSample();
  // Delete Lights
  V3d_ListOfLight aLights;
  for(V3d_ListOfLightIterator anIter = myView->Viewer()->DefinedLightIterator(); 
      anIter.More(); anIter.Next())
  {
    aLights.Append(anIter.Value());
  }

  for(V3d_ListOfLightIterator aLightIter (aLights);
      aLightIter.More(); aLightIter.Next())
  {
    myView->Viewer()->DelLight (aLightIter.Value());
  }
  myView->Viewer()->SetDefaultLights(); // Setting the default lights on
  myView->Update();

  myContext->RemoveAll (Standard_True);
}

void Viewer3dSamples::SpotLight3dSample()
{
  // Spot light source creation
  Handle(V3d_SpotLight) aSpotLight = 
    new V3d_SpotLight(gp_Pnt(100.0, 0.0, 0.0), gp_Dir(-1.0, 0.0, 0.0), Quantity_NOC_RED);
  aSpotLight->SetIntensity(5000);
  myView->SetLightOn(aSpotLight);
}

void Viewer3dSamples::PositionalLight3dSample()
{
  Handle(V3d_PositionalLight) aPositionalLight = 
    new V3d_PositionalLight(gp_Pnt(0.0, -100.0, 5.0), Quantity_NOC_GREEN);
  aPositionalLight->SetAttenuation(1, 0);
  myView->SetLightOn(aPositionalLight);
}

void Viewer3dSamples::DirectionalLight3dSample()
{
  Handle(V3d_DirectionalLight) aDirectionalLight = 
    new V3d_DirectionalLight(gp_Dir(-1.0, 0.0, -1.0), Quantity_NOC_BLUE1);
  myView->SetLightOn(aDirectionalLight);
}

void Viewer3dSamples::AmbientLight3dSample()
{
  Handle(V3d_AmbientLight) aAmbientLight = 
    new V3d_AmbientLight(Quantity_NOC_MAGENTA1);
  myView->SetLightOn(aAmbientLight);
}

void Viewer3dSamples::ClearLight3dSample()
{
  // Setting Off all viewer active lights
  V3d_ListOfLight aLights;
  for(V3d_ListOfLightIterator anIter = myView->Viewer()->ActiveLightIterator();
      anIter.More(); anIter.Next())
  {
    aLights.Append(anIter.Value());
  }

  for(V3d_ListOfLightIterator aLightIter(aLights); 
      aLightIter.More(); aLightIter.Next())
  {
    myView->Viewer()->SetLightOff (aLightIter.Value());
  }
  // Setting Off all view active lights
  aLights.Clear();
  for(V3d_ListOfLightIterator anIter = myView->ActiveLightIterator(); 
      anIter.More(); anIter.Next())
  {
    aLights.Append(anIter.Value());
  }

  for(V3d_ListOfLightIterator aLightIter (aLights);
      aLightIter.More(); aLightIter.Next())
  {
    myView->SetLightOff (aLightIter.Value());
  }
  myView->Viewer()->SetDefaultLights(); // Setting the default lights on
  myView->Update();
}

void Viewer3dSamples::VerticesSelect3dSample()
{
  myContext->Deactivate();
  myContext->Activate(AIS_Shape::SelectionMode(TopAbs_VERTEX));
}

void Viewer3dSamples::EdgesSelect3dSample()
{
  myContext->Deactivate();
  myContext->Activate(AIS_Shape::SelectionMode(TopAbs_EDGE));
}

void Viewer3dSamples::FacesSelect3dSample()
{
  myContext->Deactivate();
  myContext->Activate(AIS_Shape::SelectionMode(TopAbs_FACE));
}

void Viewer3dSamples::NeutralPointSelect3dSample()
{
  myContext->Deactivate();
  myContext->Activate(0);
}

void Viewer3dSamples::WireFramePresentation3dSample()
{
  AIS_ListOfInteractive anAisObjectsList;
  myContext->DisplayedObjects(anAisObjectsList);
  for(AIS_ListOfInteractive::Iterator anIter(anAisObjectsList); 
      anIter.More(); anIter.Next())
  {
    const Handle(AIS_InteractiveObject)& anAisObject = anIter.Value();
    myContext->SetDisplayMode(anAisObject, 0, false); // set wireframe
  }
  myContext->UpdateCurrentViewer();
}

void Viewer3dSamples::ShadingPresentation3dSample()
{
  AIS_ListOfInteractive anAisObjectsList;
  myContext->DisplayedObjects(anAisObjectsList);
  for(AIS_ListOfInteractive::Iterator anIter(anAisObjectsList);
      anIter.More(); anIter.Next())
  {
    const Handle(AIS_InteractiveObject)& anAisObject = anIter.Value();
    myContext->SetDisplayMode(anAisObject, 1, false); // set shading
  }
  myContext->UpdateCurrentViewer();
}

void Viewer3dSamples::RedColorPresentation3dSample()
{
  AIS_ListOfInteractive anAisObjectsList;
  myContext->DisplayedObjects(anAisObjectsList);
  for(AIS_ListOfInteractive::Iterator anIter(anAisObjectsList); 
      anIter.More(); anIter.Next())
  {
    const Handle(AIS_InteractiveObject)& anAisObject = anIter.Value();
    Quantity_Color aShapeColor;
    myContext->Color(anAisObject, aShapeColor);
    myResult << "A Current shape color: Red = " << aShapeColor.Red()
             << " Green = " << aShapeColor.Green() << " Blue = " << aShapeColor.Blue() << std::endl;
    aShapeColor.SetValues(0.8, 0.1, 0.1, Quantity_TOC_RGB);
    myContext->SetColor(anAisObject, aShapeColor, Standard_False);
    myResult << "A New shape color: Red = " << aShapeColor.Red()
             << " Green = " << aShapeColor.Green() << " Blue = " << aShapeColor.Blue() << std::endl;
  }
}

void Viewer3dSamples::GrayColorPresentation3dSample()
{
  AIS_ListOfInteractive anAisObjectsList;
  myContext->DisplayedObjects(anAisObjectsList);
  for(AIS_ListOfInteractive::Iterator anIter(anAisObjectsList);
      anIter.More(); anIter.Next())
  {
    const Handle(AIS_InteractiveObject)& anAisObject = anIter.Value();
    Quantity_Color aShapeColor;
    myContext->Color(anAisObject, aShapeColor);
    myResult << "A Current shape color: Hue = " << aShapeColor.Hue()
             << " Light = " << aShapeColor.Light() 
             << " Saturation = " << aShapeColor.Saturation() << std::endl;
    aShapeColor.SetValues(0.0, 0.3, 0.1, Quantity_TOC_HLS);
    myContext->SetColor(anAisObject, aShapeColor, Standard_False);
    myResult << "A New shape color: Hue = " << aShapeColor.Hue()
             << " Light = " << aShapeColor.Light() 
             << " Saturation = " << aShapeColor.Saturation() << std::endl;
  }
}

void Viewer3dSamples::PlasticPresentation3dSample()
{
  AIS_ListOfInteractive anAisObjectsList;
  myContext->DisplayedObjects(anAisObjectsList);
  Graphic3d_NameOfMaterial aMaterial = Graphic3d_NOM_PLASTIC;
  for(AIS_ListOfInteractive::Iterator anIter (anAisObjectsList); 
      anIter.More(); anIter.Next())
  {
    const Handle(AIS_InteractiveObject)& anAisObject = anIter.Value();
    myContext->SetMaterial(anAisObject, aMaterial, Standard_False);
  }
  myContext->UpdateCurrentViewer();
}

void Viewer3dSamples::BronzePresentation3dSample()
{
  AIS_ListOfInteractive anAisObjectsList;
  myContext->DisplayedObjects(anAisObjectsList);
  Graphic3d_NameOfMaterial aMaterial = Graphic3d_NOM_BRONZE;
  for(AIS_ListOfInteractive::Iterator anIter (anAisObjectsList); 
      anIter.More(); anIter.Next())
  {
    const Handle(AIS_InteractiveObject)& anAisObject = anIter.Value();
    myContext->SetMaterial(anAisObject, aMaterial, Standard_False);
  }
  myContext->UpdateCurrentViewer();
}

void Viewer3dSamples::OpaquePresentation3dSample()
{
  AIS_ListOfInteractive anAisObjectsList;
  myContext->DisplayedObjects(anAisObjectsList);
  for(AIS_ListOfInteractive::Iterator anIter (anAisObjectsList);
      anIter.More(); anIter.Next())
  {
    const Handle(AIS_InteractiveObject)& anAisObject = anIter.Value();
    myContext->SetTransparency(anAisObject, 0.0, Standard_False);
  }
  myContext->UpdateCurrentViewer();
}

void Viewer3dSamples::HalfTransparencyPresentation3dSample()
{
  AIS_ListOfInteractive anAisObjectsList;
  myContext->DisplayedObjects(anAisObjectsList);
  for(AIS_ListOfInteractive::Iterator anIter (anAisObjectsList);
      anIter.More(); anIter.Next())
  {
    const Handle(AIS_InteractiveObject)& anAisObject = anIter.Value();
    myContext->SetTransparency(anAisObject, 0.5, Standard_False);
  }
  myContext->UpdateCurrentViewer();
}

void Viewer3dSamples::VboOn3dSample()
{
  if(Handle(OpenGl_GraphicDriver) aDriver = 
     Handle(OpenGl_GraphicDriver)::DownCast(myContext->CurrentViewer()->Driver()))
  {
    aDriver->ChangeOptions().vboDisable = Standard_False;
  }
}

void Viewer3dSamples::VboOff3dSample()
{
  if(Handle(OpenGl_GraphicDriver) aDriver = 
    Handle(OpenGl_GraphicDriver)::DownCast(myContext->CurrentViewer()->Driver()))
  {
    aDriver->ChangeOptions().vboDisable = Standard_True;
  }
}
