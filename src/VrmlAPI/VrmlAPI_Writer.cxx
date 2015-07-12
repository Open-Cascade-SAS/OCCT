// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <BRep_Tool.hxx>
#include <OSD_OpenFile.hxx>
#include <OSD_Path.hxx>
#include <Poly_Triangulation.hxx>
#include <Quantity_HArray1OfColor.hxx>
#include <Standard_Stream.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_Array1OfShape.hxx>
#include <Vrml.hxx>
#include <Vrml_Group.hxx>
#include <Vrml_Instancing.hxx>
#include <Vrml_Material.hxx>
#include <Vrml_Separator.hxx>
#include <VrmlAPI_Writer.hxx>
#include <VrmlConverter_Drawer.hxx>
#include <VrmlConverter_IsoAspect.hxx>
#include <VrmlConverter_LineAspect.hxx>
#include <VrmlConverter_PointAspect.hxx>
#include <VrmlConverter_Projector.hxx>
#include <VrmlConverter_ShadedShape.hxx>
#include <VrmlConverter_ShadingAspect.hxx>
#include <VrmlConverter_WFDeflectionShape.hxx>
#include <VrmlData_Scene.hxx>
#include <VrmlData_ShapeConvert.hxx>

VrmlAPI_Writer::VrmlAPI_Writer()
{
  myDrawer = new VrmlConverter_Drawer;
  myDeflection = -1;
  Quantity_Color color;
  color.SetValues(0, 0, 0, Quantity_TOC_RGB);
  Handle(Quantity_HArray1OfColor) Col1 = new Quantity_HArray1OfColor(1,1);
  Col1->SetValue(1,color);
  Handle(TColStd_HArray1OfReal) kik1 = new TColStd_HArray1OfReal(1,1,0.0);
  Handle(TColStd_HArray1OfReal) kik2 = new TColStd_HArray1OfReal(1,1,0.1);
  myFrontMaterial = new Vrml_Material(Col1,Col1, Col1, Col1, kik1, kik2);
  myPointsMaterial = new Vrml_Material(Col1,Col1, Col1, Col1, kik1, kik2);
  myUisoMaterial = new Vrml_Material(Col1,Col1, Col1, Col1, kik1, kik2);
  myVisoMaterial = new Vrml_Material(Col1,Col1, Col1, Col1, kik1, kik2);
  myLineMaterial = new Vrml_Material(Col1,Col1, Col1, Col1, kik1, kik2);
  myWireMaterial = new Vrml_Material(Col1,Col1, Col1, Col1, kik1, kik2);
  myFreeBoundsMaterial = new Vrml_Material(Col1,Col1, Col1, Col1, kik1, kik2);
  myUnfreeBoundsMaterial = new Vrml_Material(Col1,Col1, Col1, Col1, kik1, kik2);
  DX = 1;      
  DY = -1;  
  DZ = 1;
  XUp = 0;
  YUp = 0;
  ZUp = 1;
  Focus = 6;
  ResetToDefaults();
}

void VrmlAPI_Writer::ResetToDefaults() 
{
  myTransparency = 0.0;
  myShininess = 0.1;
  Handle(TColStd_HArray1OfReal) kik1 = new TColStd_HArray1OfReal(1,1,myTransparency);
  Handle(TColStd_HArray1OfReal) kik2 = new TColStd_HArray1OfReal(1,1,myShininess);
  Handle(Quantity_HArray1OfColor) Col = new Quantity_HArray1OfColor(1,1);
  Quantity_Color color; 
  color.SetValues(0, 0, 0, Quantity_TOC_RGB);
  Col->SetValue(1,color);
  //
  myFrontMaterial->SetAmbientColor(Col); myFrontMaterial->SetTransparency(kik1);myFrontMaterial->SetShininess(kik2);
  myPointsMaterial->SetAmbientColor(Col); myPointsMaterial->SetTransparency(kik1);myPointsMaterial->SetShininess(kik2);
  myUisoMaterial->SetAmbientColor(Col); myUisoMaterial->SetTransparency(kik1);myUisoMaterial->SetShininess(kik2);
  myVisoMaterial->SetAmbientColor(Col); myVisoMaterial->SetTransparency(kik1);myVisoMaterial->SetShininess(kik2);
  myLineMaterial->SetAmbientColor(Col); myLineMaterial->SetTransparency(kik1);myLineMaterial->SetShininess(kik2);
  myWireMaterial->SetAmbientColor(Col); myWireMaterial->SetTransparency(kik1); myWireMaterial->SetShininess(kik2);
  myFreeBoundsMaterial->SetAmbientColor(Col); myFreeBoundsMaterial->SetTransparency(kik1);myFreeBoundsMaterial->SetShininess(kik2);
  myUnfreeBoundsMaterial->SetAmbientColor(Col); myUnfreeBoundsMaterial->SetTransparency(kik1);myUnfreeBoundsMaterial->SetShininess(kik2);
  //
  //
  Handle(Quantity_HArray1OfColor) Col2 = new Quantity_HArray1OfColor(1,1);
  color.SetValues(0.75, 0.75, 0.75, Quantity_TOC_RGB);
  Col2->SetValue(1,color);
  Handle(Quantity_HArray1OfColor) Col3 = new Quantity_HArray1OfColor(1,1);
  color.SetValues(0.82, 0.79, 0.42, Quantity_TOC_RGB);
  Col3->SetValue(1,color);
  
  myUisoMaterial->SetDiffuseColor(Col2);
  myVisoMaterial->SetDiffuseColor(Col2);
  myFreeBoundsMaterial->SetDiffuseColor(Col2);
  myUnfreeBoundsMaterial->SetDiffuseColor(Col2);
  
  //
//  Handle(Quantity_HArray1OfColor) Col3 = new Quantity_HArray1OfColor(1,1);
//  color.SetValues(Quantity_NOC_GOLD);
//  Col3->SetValue(1,color);
  myLineMaterial->SetDiffuseColor(Col2);
  myWireMaterial->SetDiffuseColor(Col2);
  //
//  Handle(Quantity_HArray1OfColor) Col4 = new Quantity_HArray1OfColor(1,1);
//  color.SetValues(Quantity_NOC_GOLD);
//  Col4->SetValue(1,color);
  myFrontMaterial->SetDiffuseColor(Col2);
  myPointsMaterial->SetDiffuseColor(Col2);
  //
  
  myUisoMaterial->SetSpecularColor(Col3);
  myVisoMaterial->SetSpecularColor(Col3);
  myFreeBoundsMaterial->SetSpecularColor(Col3);
  myUnfreeBoundsMaterial->SetSpecularColor(Col3);
  myLineMaterial->SetSpecularColor(Col3);
  myWireMaterial->SetSpecularColor(Col3);
  myFrontMaterial->SetSpecularColor(Col3);
  myPointsMaterial->SetSpecularColor(Col3);
  
  myRepresentation = VrmlAPI_BothRepresentation;
}
Handle(VrmlConverter_Drawer) VrmlAPI_Writer::Drawer() const
{
  return myDrawer;
}
void VrmlAPI_Writer::SetDeflection(const Standard_Real aDef)
{
  myDeflection = aDef;
  if (myDeflection > 0) {
    myDrawer->SetMaximalChordialDeviation(myDeflection);
    myDrawer->SetTypeOfDeflection(Aspect_TOD_ABSOLUTE);
  }
  else myDrawer->SetTypeOfDeflection(Aspect_TOD_RELATIVE);
}
void VrmlAPI_Writer::SetRepresentation(const VrmlAPI_RepresentationOfShape aRep)
{
  myRepresentation = aRep;
}
void VrmlAPI_Writer::SetTransparencyToMaterial(Handle(Vrml_Material)& aMaterial,const Standard_Real aTransparency) 
{
  Handle(TColStd_HArray1OfReal) t = new TColStd_HArray1OfReal(1,1,aTransparency);
  aMaterial->SetTransparency(t);
}

void VrmlAPI_Writer::SetShininessToMaterial(Handle(Vrml_Material)& aMaterial,const Standard_Real aShininess) 
{  
  Handle(TColStd_HArray1OfReal) s = new TColStd_HArray1OfReal(1,1,aShininess);
  aMaterial->SetShininess(s);
}

void VrmlAPI_Writer::SetAmbientColorToMaterial(Handle(Vrml_Material)& aMaterial,const Handle(Quantity_HArray1OfColor)& Color) 
{
  aMaterial->SetAmbientColor(Color);
}

void VrmlAPI_Writer::SetDiffuseColorToMaterial(Handle(Vrml_Material)& aMaterial,const Handle(Quantity_HArray1OfColor)& Color) 
{
  aMaterial->SetDiffuseColor(Color);
}

void VrmlAPI_Writer::SetSpecularColorToMaterial(Handle(Vrml_Material)& aMaterial,const Handle(Quantity_HArray1OfColor)& Color) 
{
  aMaterial->SetSpecularColor(Color);
}

void VrmlAPI_Writer::SetEmissiveColorToMaterial(Handle(Vrml_Material)& aMaterial,const Handle(Quantity_HArray1OfColor)& Color) 
{
  aMaterial->SetEmissiveColor(Color);
}

VrmlAPI_RepresentationOfShape VrmlAPI_Writer::GetRepresentation() const
{
  return myRepresentation;  
}

Handle(Vrml_Material) VrmlAPI_Writer::GetFrontMaterial() const
{
  return myFrontMaterial;
}

Handle(Vrml_Material) VrmlAPI_Writer::GetPointsMaterial() const
{
  return myPointsMaterial;
}

Handle(Vrml_Material) VrmlAPI_Writer::GetUisoMaterial() const
{
  return myUisoMaterial;
}

Handle(Vrml_Material) VrmlAPI_Writer::GetVisoMaterial() const
{
  return myVisoMaterial;
}

Handle(Vrml_Material) VrmlAPI_Writer::GetLineMaterial() const
{
  return myLineMaterial;
}

Handle(Vrml_Material) VrmlAPI_Writer::GetWireMaterial() const
{
  return myWireMaterial;
}

Handle(Vrml_Material) VrmlAPI_Writer::GetFreeBoundsMaterial() const
{
  return myFreeBoundsMaterial;
}

Handle(Vrml_Material) VrmlAPI_Writer::GetUnfreeBoundsMaterial() const
{
  return myUnfreeBoundsMaterial;
}

void VrmlAPI_Writer::Write(const TopoDS_Shape& aShape,const Standard_CString aFile, const Standard_Integer aVersion) const
{
  if (aVersion == 1)
    write_v1(aShape, aFile);
  else if (aVersion == 2)
    write_v2(aShape, aFile);
}

void VrmlAPI_Writer::write_v1(const TopoDS_Shape& aShape,const Standard_CString aFile) const
{
  OSD_Path thePath(aFile);
  TCollection_AsciiString theFile;thePath.SystemName(theFile);
  ofstream outfile;
  OSD_OpenStream(outfile, theFile.ToCString(), ios::out);
  Handle(VrmlConverter_IsoAspect) ia = new VrmlConverter_IsoAspect;  // UIso
  Handle(VrmlConverter_IsoAspect) ia1 = new VrmlConverter_IsoAspect; //VIso
  ia->SetMaterial(myUisoMaterial);
  ia->SetHasMaterial(Standard_True);
  myDrawer->SetUIsoAspect(ia);
  ia1->SetMaterial(myVisoMaterial);
  ia1->SetHasMaterial(Standard_True);
  myDrawer->SetVIsoAspect(ia1);
// default Number of iso lines is 10
//----  Definition of LineAspect (default - without own material)
  Handle(VrmlConverter_LineAspect) la = new VrmlConverter_LineAspect;
  la->SetMaterial(myLineMaterial);
  la->SetHasMaterial(Standard_True);
  myDrawer->SetLineAspect(la);
//----  Definition of Wire (without any neighbour)
  Handle(VrmlConverter_LineAspect) lw = new VrmlConverter_LineAspect;
  lw->SetMaterial(myWireMaterial);
  lw->SetHasMaterial(Standard_True);
  myDrawer->SetWireAspect(lw);
//----  Definition of Free boundaries
  Handle(VrmlConverter_LineAspect) lf = new VrmlConverter_LineAspect;
  lf->SetMaterial(myFreeBoundsMaterial);
  lf->SetHasMaterial(Standard_True);
  myDrawer->SetFreeBoundaryAspect(lf);
//----  Definition of Unfree boundaries
  Handle(VrmlConverter_LineAspect) lun = new VrmlConverter_LineAspect;
  lun->SetMaterial(myUnfreeBoundsMaterial);
  lun->SetHasMaterial(Standard_True);
  myDrawer->SetUnFreeBoundaryAspect(lun);
//----  Definition of Points (default - without own material)
  Handle(VrmlConverter_PointAspect) pa = new VrmlConverter_PointAspect;
  pa->SetMaterial(myPointsMaterial);
  pa->SetHasMaterial(Standard_True);
  myDrawer->SetPointAspect(pa);
//-----------------------------------------
  Handle(VrmlConverter_ShadingAspect) sa = new VrmlConverter_ShadingAspect;
  sa->SetFrontMaterial(myFrontMaterial);
  sa->SetHasMaterial(Standard_True);
  Vrml_ShapeHints  sh;
  sa->SetShapeHints(sh);
  myDrawer->SetShadingAspect(sa);
//-------- Shape --------------------------
  TopTools_Array1OfShape Shapes(1,1);
  Shapes.SetValue(1,aShape);

  // Check shape tesselation
  TopExp_Explorer anExp (aShape, TopAbs_FACE);
  TopLoc_Location aLoc;
  Standard_Boolean hasTriangles = Standard_False;
  for (; anExp.More(); anExp.Next())
  {
    const TopoDS_Face& aFace = TopoDS::Face (anExp.Current());
    if (!aFace.IsNull()) 
    {
      Handle(Poly_Triangulation) aTri =
        BRep_Tool::Triangulation (aFace, aLoc);

      if (!aTri.IsNull())
      {
        hasTriangles = Standard_True;
        break;
      }
    }
  }

//=========================================
//----  Definition of data for Projector
//=========================================

  VrmlConverter_TypeOfLight Light = VrmlConverter_NoLight;
  VrmlConverter_TypeOfCamera Camera = VrmlConverter_PerspectiveCamera;
  Handle(VrmlConverter_Projector) projector = new VrmlConverter_Projector (Shapes, 
									   Focus, 
									   DX,  DY,  DZ, 
									   XUp, YUp, ZUp,
									   Camera,
									   Light);
  Vrml::VrmlHeaderWriter(outfile);
  if (myRepresentation == VrmlAPI_BothRepresentation)
    Vrml::CommentWriter(" This file contents both Shaded and Wire Frame representation of selected Shape ",outfile);   
  if (myRepresentation == VrmlAPI_ShadedRepresentation)
    Vrml::CommentWriter(" This file contents only Shaded representation of selected Shape ",outfile);   
  if (myRepresentation == VrmlAPI_WireFrameRepresentation)
    Vrml::CommentWriter(" This file contents only Wire Frame representation of selected Shape ",outfile);   
  Vrml_Separator S1;
  S1.Print(outfile); 
  projector->Add(outfile);
  Light = VrmlConverter_DirectionLight;
  Camera = VrmlConverter_OrthographicCamera;
  Handle(VrmlConverter_Projector) projector1 = new VrmlConverter_Projector (Shapes, 
									   Focus, 
									   DX,  DY,  DZ, 
									   XUp, YUp, ZUp,
									   Camera,
									   Light);
  projector1->Add(outfile);
  Vrml_Separator S2;
  S2.Print(outfile); 
  if ( (myRepresentation == VrmlAPI_ShadedRepresentation || myRepresentation == VrmlAPI_BothRepresentation) && hasTriangles)
    {
      Vrml_Group Group1;
      Group1.Print(outfile);
      Vrml_Instancing I2 ("Shaded representation of shape");
      I2.DEF(outfile);
      VrmlConverter_ShadedShape::Add(outfile,aShape,myDrawer);
      Group1.Print(outfile);
    }
  if (myRepresentation == VrmlAPI_WireFrameRepresentation || myRepresentation == VrmlAPI_BothRepresentation)
    {
      Vrml_Group Group2;
      Group2.Print(outfile);
      Vrml_Instancing I3 ("Wire Frame representation of shape");
      I3.DEF(outfile);
      VrmlConverter_WFDeflectionShape::Add(outfile,aShape,myDrawer);
      Group2.Print(outfile);
    }
  S2.Print(outfile); 
  S1.Print(outfile); 
}

void VrmlAPI_Writer::write_v2(const TopoDS_Shape& aShape,const Standard_CString aFile) const
{
  Standard_Boolean anExtFace = Standard_False;
  if(myRepresentation == VrmlAPI_ShadedRepresentation || myRepresentation == VrmlAPI_BothRepresentation) 
    anExtFace = Standard_True;

  Standard_Boolean anExtEdge = Standard_False;
  if(myRepresentation == VrmlAPI_WireFrameRepresentation|| myRepresentation == VrmlAPI_BothRepresentation)
    anExtEdge = Standard_True;

  VrmlData_Scene aScene;
  VrmlData_ShapeConvert aConv(aScene);
  aConv.AddShape(aShape);
  aConv.Convert(anExtFace, anExtEdge);

  filebuf aFoc;
  ostream outStream (&aFoc);
  if (aFoc.open (aFile, ios::out))
    outStream << aScene;
}
