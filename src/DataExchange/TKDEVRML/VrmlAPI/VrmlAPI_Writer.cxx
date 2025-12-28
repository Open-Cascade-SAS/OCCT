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
#include <OSD_FileSystem.hxx>
#include <OSD_Path.hxx>
#include <Poly_Triangulation.hxx>
#include <Standard_Macro.hxx>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
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
  myDrawer                             = new VrmlConverter_Drawer;
  myDeflection                         = -1;
  occ::handle<NCollection_HArray1<Quantity_Color>> Col1 = new NCollection_HArray1<Quantity_Color>(1, 1, Quantity_NOC_BLACK);
  occ::handle<NCollection_HArray1<double>>   kik1 = new NCollection_HArray1<double>(1, 1, 0.0);
  occ::handle<NCollection_HArray1<double>>   kik2 = new NCollection_HArray1<double>(1, 1, 0.1);
  myFrontMaterial                      = new Vrml_Material(Col1, Col1, Col1, Col1, kik1, kik2);
  myPointsMaterial                     = new Vrml_Material(Col1, Col1, Col1, Col1, kik1, kik2);
  myUisoMaterial                       = new Vrml_Material(Col1, Col1, Col1, Col1, kik1, kik2);
  myVisoMaterial                       = new Vrml_Material(Col1, Col1, Col1, Col1, kik1, kik2);
  myLineMaterial                       = new Vrml_Material(Col1, Col1, Col1, Col1, kik1, kik2);
  myWireMaterial                       = new Vrml_Material(Col1, Col1, Col1, Col1, kik1, kik2);
  myFreeBoundsMaterial                 = new Vrml_Material(Col1, Col1, Col1, Col1, kik1, kik2);
  myUnfreeBoundsMaterial               = new Vrml_Material(Col1, Col1, Col1, Col1, kik1, kik2);
  DX                                   = 1;
  DY                                   = -1;
  DZ                                   = 1;
  XUp                                  = 0;
  YUp                                  = 0;
  ZUp                                  = 1;
  Focus                                = 6;
  ResetToDefaults();
}

void VrmlAPI_Writer::ResetToDefaults()
{
  myTransparency                       = 0.0;
  myShininess                          = 0.1;
  occ::handle<NCollection_HArray1<double>>   kik1 = new NCollection_HArray1<double>(1, 1, myTransparency);
  occ::handle<NCollection_HArray1<double>>   kik2 = new NCollection_HArray1<double>(1, 1, myShininess);
  occ::handle<NCollection_HArray1<Quantity_Color>> Col  = new NCollection_HArray1<Quantity_Color>(1, 1, Quantity_NOC_BLACK);
  //
  myFrontMaterial->SetAmbientColor(Col);
  myFrontMaterial->SetTransparency(kik1);
  myFrontMaterial->SetShininess(kik2);
  myPointsMaterial->SetAmbientColor(Col);
  myPointsMaterial->SetTransparency(kik1);
  myPointsMaterial->SetShininess(kik2);
  myUisoMaterial->SetAmbientColor(Col);
  myUisoMaterial->SetTransparency(kik1);
  myUisoMaterial->SetShininess(kik2);
  myVisoMaterial->SetAmbientColor(Col);
  myVisoMaterial->SetTransparency(kik1);
  myVisoMaterial->SetShininess(kik2);
  myLineMaterial->SetAmbientColor(Col);
  myLineMaterial->SetTransparency(kik1);
  myLineMaterial->SetShininess(kik2);
  myWireMaterial->SetAmbientColor(Col);
  myWireMaterial->SetTransparency(kik1);
  myWireMaterial->SetShininess(kik2);
  myFreeBoundsMaterial->SetAmbientColor(Col);
  myFreeBoundsMaterial->SetTransparency(kik1);
  myFreeBoundsMaterial->SetShininess(kik2);
  myUnfreeBoundsMaterial->SetAmbientColor(Col);
  myUnfreeBoundsMaterial->SetTransparency(kik1);
  myUnfreeBoundsMaterial->SetShininess(kik2);
  //
  //
  occ::handle<NCollection_HArray1<Quantity_Color>> Col2 =
    new NCollection_HArray1<Quantity_Color>(1, 1, Quantity_Color(0.75, 0.75, 0.75, Quantity_TOC_sRGB));
  occ::handle<NCollection_HArray1<Quantity_Color>> Col3 =
    new NCollection_HArray1<Quantity_Color>(1, 1, Quantity_Color(0.82, 0.79, 0.42, Quantity_TOC_sRGB));
  myUisoMaterial->SetDiffuseColor(Col2);
  myVisoMaterial->SetDiffuseColor(Col2);
  myFreeBoundsMaterial->SetDiffuseColor(Col2);
  myUnfreeBoundsMaterial->SetDiffuseColor(Col2);

  //
  //  occ::handle<NCollection_HArray1<Quantity_Color>> Col3 = new NCollection_HArray1<Quantity_Color>(1,1);
  //  color.SetValues(Quantity_NOC_GOLD);
  //  Col3->SetValue(1,color);
  myLineMaterial->SetDiffuseColor(Col2);
  myWireMaterial->SetDiffuseColor(Col2);
  //
  //  occ::handle<NCollection_HArray1<Quantity_Color>> Col4 = new NCollection_HArray1<Quantity_Color>(1,1);
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

occ::handle<VrmlConverter_Drawer> VrmlAPI_Writer::Drawer() const
{
  return myDrawer;
}

void VrmlAPI_Writer::SetDeflection(const double aDef)
{
  myDeflection = aDef;
  if (myDeflection > 0)
  {
    myDrawer->SetMaximalChordialDeviation(myDeflection);
    myDrawer->SetTypeOfDeflection(Aspect_TOD_ABSOLUTE);
  }
  else
    myDrawer->SetTypeOfDeflection(Aspect_TOD_RELATIVE);
}

void VrmlAPI_Writer::SetRepresentation(const VrmlAPI_RepresentationOfShape aRep)
{
  myRepresentation = aRep;
}

void VrmlAPI_Writer::SetTransparencyToMaterial(occ::handle<Vrml_Material>& aMaterial,
                                               const double    aTransparency)
{
  occ::handle<NCollection_HArray1<double>> t = new NCollection_HArray1<double>(1, 1, aTransparency);
  aMaterial->SetTransparency(t);
}

void VrmlAPI_Writer::SetShininessToMaterial(occ::handle<Vrml_Material>& aMaterial,
                                            const double    aShininess)
{
  occ::handle<NCollection_HArray1<double>> s = new NCollection_HArray1<double>(1, 1, aShininess);
  aMaterial->SetShininess(s);
}

void VrmlAPI_Writer::SetAmbientColorToMaterial(occ::handle<Vrml_Material>&                 aMaterial,
                                               const occ::handle<NCollection_HArray1<Quantity_Color>>& Color)
{
  aMaterial->SetAmbientColor(Color);
}

void VrmlAPI_Writer::SetDiffuseColorToMaterial(occ::handle<Vrml_Material>&                 aMaterial,
                                               const occ::handle<NCollection_HArray1<Quantity_Color>>& Color)
{
  aMaterial->SetDiffuseColor(Color);
}

void VrmlAPI_Writer::SetSpecularColorToMaterial(occ::handle<Vrml_Material>&                 aMaterial,
                                                const occ::handle<NCollection_HArray1<Quantity_Color>>& Color)
{
  aMaterial->SetSpecularColor(Color);
}

void VrmlAPI_Writer::SetEmissiveColorToMaterial(occ::handle<Vrml_Material>&                 aMaterial,
                                                const occ::handle<NCollection_HArray1<Quantity_Color>>& Color)
{
  aMaterial->SetEmissiveColor(Color);
}

VrmlAPI_RepresentationOfShape VrmlAPI_Writer::GetRepresentation() const
{
  return myRepresentation;
}

occ::handle<Vrml_Material> VrmlAPI_Writer::GetFrontMaterial() const
{
  return myFrontMaterial;
}

occ::handle<Vrml_Material> VrmlAPI_Writer::GetPointsMaterial() const
{
  return myPointsMaterial;
}

occ::handle<Vrml_Material> VrmlAPI_Writer::GetUisoMaterial() const
{
  return myUisoMaterial;
}

occ::handle<Vrml_Material> VrmlAPI_Writer::GetVisoMaterial() const
{
  return myVisoMaterial;
}

occ::handle<Vrml_Material> VrmlAPI_Writer::GetLineMaterial() const
{
  return myLineMaterial;
}

occ::handle<Vrml_Material> VrmlAPI_Writer::GetWireMaterial() const
{
  return myWireMaterial;
}

occ::handle<Vrml_Material> VrmlAPI_Writer::GetFreeBoundsMaterial() const
{
  return myFreeBoundsMaterial;
}

occ::handle<Vrml_Material> VrmlAPI_Writer::GetUnfreeBoundsMaterial() const
{
  return myUnfreeBoundsMaterial;
}

bool VrmlAPI_Writer::Write(const TopoDS_Shape&    aShape,
                                       const char* aFile,
                                       const int aVersion) const
{
  const occ::handle<OSD_FileSystem>& aFileSystem = OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::ostream> anOutStream =
    aFileSystem->OpenOStream(aFile, std::ios::out | std::ios::binary);
  if (anOutStream.get() == NULL)
  {
    return false;
  }

  return Write(aShape, *anOutStream, aVersion);
}

bool VrmlAPI_Writer::WriteDoc(const occ::handle<TDocStd_Document>& theDoc,
                                          const char*          theFile,
                                          const double             theScale) const
{
  const occ::handle<OSD_FileSystem>& aFileSystem = OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::ostream> anOutStream =
    aFileSystem->OpenOStream(theFile, std::ios::out | std::ios::binary);
  if (anOutStream.get() == NULL)
  {
    return false;
  }

  return WriteDoc(theDoc, *anOutStream, theScale);
}

//=================================================================================================

bool VrmlAPI_Writer::Write(const TopoDS_Shape&    aShape,
                                       Standard_OStream&      theOStream,
                                       const int aVersion) const
{
  if (aVersion == 1)
    return write_v1(aShape, theOStream);
  else if (aVersion == 2)
    return write_v2(aShape, theOStream);

  return false;
}

//=================================================================================================

bool VrmlAPI_Writer::WriteDoc(const occ::handle<TDocStd_Document>& theDoc,
                                          Standard_OStream&               theOStream,
                                          const double             theScale) const
{
  VrmlData_Scene        aScene;
  VrmlData_ShapeConvert aConv(aScene, theScale);
  aConv.ConvertDocument(theDoc);

  theOStream << aScene;
  theOStream.flush();
  return theOStream.good();
}

bool VrmlAPI_Writer::write_v1(const TopoDS_Shape& aShape,
                                          Standard_OStream&   theOStream) const
{
  occ::handle<VrmlConverter_IsoAspect> ia  = new VrmlConverter_IsoAspect;
  occ::handle<VrmlConverter_IsoAspect> ia1 = new VrmlConverter_IsoAspect;
  ia->SetMaterial(myUisoMaterial);
  ia->SetHasMaterial(true);
  myDrawer->SetUIsoAspect(ia);
  ia1->SetMaterial(myVisoMaterial);
  ia1->SetHasMaterial(true);
  myDrawer->SetVIsoAspect(ia1);
  // default Number of iso lines is 10
  //----  Definition of LineAspect (default - without own material)
  occ::handle<VrmlConverter_LineAspect> la = new VrmlConverter_LineAspect;
  la->SetMaterial(myLineMaterial);
  la->SetHasMaterial(true);
  myDrawer->SetLineAspect(la);
  //----  Definition of Wire (without any neighbour)
  occ::handle<VrmlConverter_LineAspect> lw = new VrmlConverter_LineAspect;
  lw->SetMaterial(myWireMaterial);
  lw->SetHasMaterial(true);
  myDrawer->SetWireAspect(lw);
  //----  Definition of Free boundaries
  occ::handle<VrmlConverter_LineAspect> lf = new VrmlConverter_LineAspect;
  lf->SetMaterial(myFreeBoundsMaterial);
  lf->SetHasMaterial(true);
  myDrawer->SetFreeBoundaryAspect(lf);
  //----  Definition of Unfree boundaries
  occ::handle<VrmlConverter_LineAspect> lun = new VrmlConverter_LineAspect;
  lun->SetMaterial(myUnfreeBoundsMaterial);
  lun->SetHasMaterial(true);
  myDrawer->SetUnFreeBoundaryAspect(lun);
  //----  Definition of Points (default - without own material)
  occ::handle<VrmlConverter_PointAspect> pa = new VrmlConverter_PointAspect;
  pa->SetMaterial(myPointsMaterial);
  pa->SetHasMaterial(true);
  myDrawer->SetPointAspect(pa);
  //-----------------------------------------
  occ::handle<VrmlConverter_ShadingAspect> sa = new VrmlConverter_ShadingAspect;
  sa->SetFrontMaterial(myFrontMaterial);
  sa->SetHasMaterial(true);
  Vrml_ShapeHints sh;
  sa->SetShapeHints(sh);
  myDrawer->SetShadingAspect(sa);
  //-------- Shape --------------------------
  NCollection_Array1<TopoDS_Shape> Shapes(1, 1);
  Shapes.SetValue(1, aShape);

  // Check shape tessellation
  TopExp_Explorer  anExp(aShape, TopAbs_FACE);
  TopLoc_Location  aLoc;
  bool hasTriangles = false;
  for (; anExp.More(); anExp.Next())
  {
    const TopoDS_Face& aFace = TopoDS::Face(anExp.Current());
    if (!aFace.IsNull())
    {
      occ::handle<Poly_Triangulation> aTri = BRep_Tool::Triangulation(aFace, aLoc);
      if (!aTri.IsNull())
      {
        hasTriangles = true;
        break;
      }
    }
  }

  //=========================================
  //----  Definition of data for Projector
  //=========================================

  VrmlConverter_TypeOfLight       Light  = VrmlConverter_NoLight;
  VrmlConverter_TypeOfCamera      Camera = VrmlConverter_PerspectiveCamera;
  occ::handle<VrmlConverter_Projector> projector =
    new VrmlConverter_Projector(Shapes, Focus, DX, DY, DZ, XUp, YUp, ZUp, Camera, Light);

  Vrml::VrmlHeaderWriter(theOStream);
  if (myRepresentation == VrmlAPI_BothRepresentation)
    Vrml::CommentWriter(
      " This file contents both Shaded and Wire Frame representation of selected Shape ",
      theOStream);
  if (myRepresentation == VrmlAPI_ShadedRepresentation)
    Vrml::CommentWriter(" This file contents only Shaded representation of selected Shape ",
                        theOStream);
  if (myRepresentation == VrmlAPI_WireFrameRepresentation)
    Vrml::CommentWriter(" This file contents only Wire Frame representation of selected Shape ",
                        theOStream);

  Vrml_Separator S1;
  S1.Print(theOStream);
  projector->Add(theOStream);

  Light  = VrmlConverter_DirectionLight;
  Camera = VrmlConverter_OrthographicCamera;
  occ::handle<VrmlConverter_Projector> projector1 =
    new VrmlConverter_Projector(Shapes, Focus, DX, DY, DZ, XUp, YUp, ZUp, Camera, Light);
  projector1->Add(theOStream);

  Vrml_Separator S2;
  S2.Print(theOStream);
  if ((myRepresentation == VrmlAPI_ShadedRepresentation
       || myRepresentation == VrmlAPI_BothRepresentation)
      && hasTriangles)
  {
    Vrml_Group Group1;
    Group1.Print(theOStream);
    Vrml_Instancing I2("Shaded representation of shape");
    I2.DEF(theOStream);
    VrmlConverter_ShadedShape::Add(theOStream, aShape, myDrawer);
    Group1.Print(theOStream);
  }
  if (myRepresentation == VrmlAPI_WireFrameRepresentation
      || myRepresentation == VrmlAPI_BothRepresentation)
  {
    Vrml_Group Group2;
    Group2.Print(theOStream);
    Vrml_Instancing I3("Wire Frame representation of shape");
    I3.DEF(theOStream);
    VrmlConverter_WFDeflectionShape::Add(theOStream, aShape, myDrawer);
    Group2.Print(theOStream);
  }
  S2.Print(theOStream);
  S1.Print(theOStream);

  theOStream.flush();
  return theOStream.good();
}

bool VrmlAPI_Writer::write_v2(const TopoDS_Shape& aShape,
                                          Standard_OStream&   theOStream) const
{
  bool anExtFace = false;
  if (myRepresentation == VrmlAPI_ShadedRepresentation
      || myRepresentation == VrmlAPI_BothRepresentation)
    anExtFace = true;

  bool anExtEdge = false;
  if (myRepresentation == VrmlAPI_WireFrameRepresentation
      || myRepresentation == VrmlAPI_BothRepresentation)
    anExtEdge = true;

  VrmlData_Scene        aScene;
  VrmlData_ShapeConvert aConv(aScene);
  aConv.AddShape(aShape);
  aConv.Convert(anExtFace, anExtEdge);

  theOStream << aScene;
  theOStream.flush();
  return theOStream.good();
}
