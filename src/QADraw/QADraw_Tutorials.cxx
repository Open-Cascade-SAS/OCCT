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

#include <QADraw.hxx>

#include <ViewerTest.hxx>
#include <ViewerTest_EventManager.hxx>

#include <AIS_Animation.hxx>
#include <AIS_AnimationObject.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepBndLib.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_BndBox.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_ToolCylinder.hxx>
#include <Prs3d_ToolDisk.hxx>
#include <Prs3d_PresentationShadow.hxx>
#include <StdPrs_ShadedShape.hxx>
#include <StdPrs_WFShape.hxx>
#include <StdSelect_BRepSelectionTool.hxx>
#include <StdPrs_ToolTriangulatedShape.hxx>
#include <Select3D_SensitiveBox.hxx>
#include <Select3D_SensitivePrimitiveArray.hxx>
#include <V3d_Viewer.hxx>
#include <math_BullardGenerator.hxx>

namespace
{

//! Custom AIS object from dox/samples/ais_object.md tutorial.
//! Make sure to update tutorial after modifications in this code!
class MyAisObject : public AIS_InteractiveObject
{
  DEFINE_STANDARD_RTTI_INLINE(MyAisObject, AIS_InteractiveObject)
public:
  enum MyDispMode { MyDispMode_Main = 0, MyDispMode_Highlight = 1 };
public:
  MyAisObject();
  void SetAnimation (const Handle(AIS_Animation)& theAnim) { myAnim = theAnim; }
public:
  virtual void Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                        const Handle(Prs3d_Presentation)& thePrs,
                        const Standard_Integer theMode) override;

  virtual void ComputeSelection (const Handle(SelectMgr_Selection)& theSel,
                                 const Standard_Integer theMode) override;

  virtual bool AcceptDisplayMode (const Standard_Integer theMode) const override
  {
    return theMode == MyDispMode_Main || theMode == MyDispMode_Highlight;
  }
protected:
  Handle(AIS_Animation) myAnim;
  gp_Pnt myDragPntFrom;
};

MyAisObject::MyAisObject()
{
  // highlighting might use different display mode (see tutorial)
  //SetHilightMode (MyDispMode_Highlight);

  myDrawer->SetupOwnShadingAspect();
  myDrawer->ShadingAspect()->SetMaterial (Graphic3d_NameOfMaterial_Silver);
  myDrawer->SetWireAspect (new Prs3d_LineAspect (Quantity_NOC_GREEN, Aspect_TOL_SOLID, 2.0));
}

void MyAisObject::Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                           const Handle(Prs3d_Presentation)& thePrs,
                           const Standard_Integer theMode)
{
  (void )thePrsMgr;
  const double aRadius = 100.0, aHeight = 100.0;
  TopoDS_Shape aShape = BRepPrimAPI_MakeCylinder (aRadius, aHeight);
  if (theMode == MyDispMode_Main)
  {
    // use standard shape builders
    //StdPrs_ShadedShape::Add (thePrs, aShape, myDrawer);
    //StdPrs_WFShape::Add (thePrs, aShape, myDrawer); // add wireframe

    // use quadric builders for cylinder surface and disks
    const int aNbSlices = 25;
    Prs3d_ToolCylinder aCyl (aRadius, aRadius, aHeight, aNbSlices, aNbSlices);
    Prs3d_ToolDisk aDisk (0.0, aRadius, 25, 1);

    Handle(Graphic3d_ArrayOfTriangles) aTris =
      new Graphic3d_ArrayOfTriangles (aCyl.VerticesNb() + 2 * aDisk.VerticesNb(),
                                      3 * (aCyl.TrianglesNb() + 2 * aDisk.TrianglesNb()),
                                      Graphic3d_ArrayFlags_VertexNormal);
    aCyl .FillArray (aTris, gp_Trsf());
    aDisk.FillArray (aTris, gp_Trsf());

    gp_Trsf aDisk2Trsf;
    aDisk2Trsf.SetTransformation (gp_Ax3 (gp_Pnt (0.0, 0.0, aHeight), -gp::DZ(), gp::DX()), gp::XOY());
    aDisk.FillArray (aTris, aDisk2Trsf);

    Handle(Graphic3d_Group) aGroupTris = thePrs->NewGroup();
    aGroupTris->SetGroupPrimitivesAspect (myDrawer->ShadingAspect()->Aspect());
    aGroupTris->AddPrimitiveArray (aTris);
    aGroupTris->SetClosed (true); // will allow backface culling / capping for our solid object

    // manually tessellated disk
    /*Handle(Graphic3d_ArrayOfTriangles) aTris2 =
      new Graphic3d_ArrayOfTriangles (aNbSlices + 1, aNbSlices * 3, Graphic3d_ArrayFlags_VertexNormal);
    aTris2->AddVertex (gp_Pnt (0.0, 0.0, aHeight), gp::DZ());
    for (int aSliceIter = 0; aSliceIter < aNbSlices; ++aSliceIter)
    {
      double anAngle = M_PI * 2.0 * double(aSliceIter) / double(aNbSlices);
      aTris2->AddVertex (gp_Pnt (Cos (anAngle) * aRadius, Sin (anAngle) * aRadius, aHeight), gp::DZ());
    }
    for (int aSliceIter = 0; aSliceIter < aNbSlices; ++aSliceIter)
    {
      aTris2->AddEdges (1, aSliceIter + 2, aSliceIter + 1 < aNbSlices ? (aSliceIter + 3) : 2);
    }
    aGroupTris->AddPrimitiveArray (aTris2);*/

    // manually tessellate cylinder section as a polyline
    Handle(Graphic3d_ArrayOfSegments) aSegs = new Graphic3d_ArrayOfSegments (4, 4 * 2, Graphic3d_ArrayFlags_None);
    aSegs->AddVertex (gp_Pnt (0.0, -aRadius, 0.0));
    aSegs->AddVertex (gp_Pnt (0.0, -aRadius, aHeight));
    aSegs->AddVertex (gp_Pnt (0.0,  aRadius, aHeight));
    aSegs->AddVertex (gp_Pnt (0.0,  aRadius, 0.0));
    aSegs->AddEdges (1, 2);
    aSegs->AddEdges (2, 3);
    aSegs->AddEdges (3, 4);
    aSegs->AddEdges (4, 1);

    Handle(Graphic3d_Group) aGroupSegs = thePrs->NewGroup();
    aGroupSegs->SetGroupPrimitivesAspect (myDrawer->WireAspect()->Aspect());
    aGroupSegs->AddPrimitiveArray (aSegs);
  }
  else if (theMode == MyDispMode_Highlight)
  {
    Bnd_Box aBox;
    BRepBndLib::Add (aShape, aBox);
    Prs3d_BndBox::Add (thePrs, aBox, myDrawer);
  }
}

//! Custom AIS owner.
class MyAisOwner : public SelectMgr_EntityOwner
{
  DEFINE_STANDARD_RTTI_INLINE(MyAisOwner, SelectMgr_EntityOwner)
public:
  MyAisOwner (const Handle(MyAisObject)& theObj, int thePriority = 0)
    : SelectMgr_EntityOwner (theObj, thePriority) {}

  void SetAnimation (const Handle(AIS_Animation)& theAnim) { myAnim = theAnim; }

  virtual void HilightWithColor (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                 const Handle(Prs3d_Drawer)& theStyle,
                                 const Standard_Integer theMode) override;
  virtual void Unhilight (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                          const Standard_Integer theMode) override;

  virtual bool IsForcedHilight() const override { return true; }
  virtual bool HandleMouseClick (const Graphic3d_Vec2i& thePoint,
                                 Aspect_VKeyMouse theButton,
                                 Aspect_VKeyFlags theModifiers,
                                 bool theIsDoubleClick) override;
  virtual void SetLocation (const TopLoc_Location& theLocation) override
  {
    if (!myPrs.IsNull()) { myPrs->SetTransformation (new TopLoc_Datum3D (theLocation.Transformation())); }
  }
protected:
  Handle(Prs3d_Presentation) myPrs;
  Handle(AIS_Animation) myAnim;
};

void MyAisOwner::HilightWithColor (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                   const Handle(Prs3d_Drawer)& theStyle,
                                   const Standard_Integer theMode)
{
  (void )theMode;
  MyAisObject* anObj = dynamic_cast<MyAisObject*> (mySelectable);
  if (myPrs.IsNull())
  {
    myPrs = new Prs3d_Presentation (thePrsMgr->StructureManager());
    anObj->Compute (thePrsMgr, myPrs, MyAisObject::MyDispMode_Highlight);
  }
  if (thePrsMgr->IsImmediateModeOn())
  {
    Handle(StdSelect_ViewerSelector3d) aSelector = anObj->InteractiveContext()->MainSelector();
    SelectMgr_SortCriterion aPickPnt;
    for (int aPickIter = 1; aPickIter <= aSelector->NbPicked(); ++aPickIter)
    {
      if (aSelector->Picked (aPickIter) == this)
      {
        aPickPnt = aSelector->PickedData (aPickIter);
        break;
      }
    }

    Handle(Prs3d_Presentation) aPrs = mySelectable->GetHilightPresentation (thePrsMgr);
    aPrs->Clear();
    Handle(Graphic3d_Group) aGroupPnt = aPrs->NewGroup();
    aGroupPnt->SetGroupPrimitivesAspect (theStyle->ArrowAspect()->Aspect());

    gp_Trsf aTrsfInv (mySelectable->InversedTransformation().Trsf());
    gp_Dir  aNorm (aPickPnt.Normal.x(), aPickPnt.Normal.y(), aPickPnt.Normal.z());
    Handle(Graphic3d_ArrayOfTriangles) aTris =
      Prs3d_Arrow::DrawShaded (gp_Ax1(aPickPnt.Point, aNorm).Transformed (aTrsfInv),
                               1.0, 15.0,
                               3.0, 4.0, 10);
    aGroupPnt->AddPrimitiveArray (aTris);

    aPrs->SetZLayer (Graphic3d_ZLayerId_Top);
    thePrsMgr->AddToImmediateList (aPrs);

    //Handle(Prs3d_PresentationShadow) aShadow = new Prs3d_PresentationShadow (thePrsMgr->StructureManager(), myPrs);
    //aShadow->SetZLayer (Graphic3d_ZLayerId_Top);
    //aShadow->Highlight (theStyle);
    //thePrsMgr->AddToImmediateList (aShadow);
  }
  else
  {
    myPrs->SetTransformation (mySelectable->TransformationGeom());
    myPrs->Display();
  }
}

void MyAisOwner::Unhilight (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                            const Standard_Integer theMode)
{
  (void )thePrsMgr;
  (void )theMode;
  if (!myPrs.IsNull())
  {
    myPrs->Erase();
  }
}

bool MyAisOwner::HandleMouseClick (const Graphic3d_Vec2i& thePoint,
                                   Aspect_VKeyMouse theButton,
                                   Aspect_VKeyFlags theModifiers,
                                   bool theIsDoubleClick)
{
  (void )thePoint;
  (void )theButton;
  (void )theModifiers;
  (void )theIsDoubleClick;
  {
    static math_BullardGenerator aRandGen;
    Quantity_Color aRandColor (float(aRandGen.NextInt() % 256) / 255.0f,
                               float(aRandGen.NextInt() % 256) / 255.0f,
                               float(aRandGen.NextInt() % 256) / 255.0f,
                               Quantity_TOC_sRGB);
    mySelectable->Attributes()->ShadingAspect()->SetColor(aRandColor);
    mySelectable->SynchronizeAspects();
  }

  if (!myAnim.IsNull())
  {
    static bool isFirst = true;
    isFirst = !isFirst;
    MyAisObject* anObj = dynamic_cast<MyAisObject*> (mySelectable);

    gp_Trsf aTrsfTo;
    aTrsfTo.SetRotation (gp_Ax1 (gp::Origin(), gp::DX()), isFirst ? M_PI * 0.5 : -M_PI * 0.5);
    gp_Trsf aTrsfFrom = anObj->LocalTransformation();
    Handle(AIS_AnimationObject) anAnim = new AIS_AnimationObject ("MyAnim", anObj->InteractiveContext(), anObj, aTrsfFrom, aTrsfTo);
    anAnim->SetOwnDuration (2.0);

    myAnim->Clear();
    myAnim->Add (anAnim);
    myAnim->StartTimer (0.0, 1.0, true);
  }

  return true;
}

void MyAisObject::ComputeSelection (const Handle(SelectMgr_Selection)& theSel,
                                    const Standard_Integer theMode)
{
  if (theMode != 0)
  {
    return;
  }

  const double aRadius = 100.0, aHeight = 100.0;
  TopoDS_Shape aShape = BRepPrimAPI_MakeCylinder (aRadius, aHeight);
  Bnd_Box aBox;
  BRepBndLib::Add (aShape, aBox);
  Handle(MyAisOwner) anOwner = new MyAisOwner (this);
  anOwner->SetAnimation (myAnim);

  Handle(Graphic3d_ArrayOfTriangles) aTris = Prs3d_ToolCylinder::Create (aRadius, aRadius, aHeight, 25, 25, gp_Trsf());
  Handle(Select3D_SensitivePrimitiveArray) aSensTri = new Select3D_SensitivePrimitiveArray (anOwner);
  aSensTri->InitTriangulation (aTris->Attributes(), aTris->Indices(), TopLoc_Location());
  theSel->Add (aSensTri);

  //Handle(SelectMgr_EntityOwner) anOwner = new SelectMgr_EntityOwner (this);
  //Handle(Select3D_SensitiveBox) aSensBox = new Select3D_SensitiveBox (anOwner, aBox);
  //theSel->Add (aSensBox);
}

}

//=======================================================================
//function : QATutorialAisObject
//purpose  :
//=======================================================================
static Standard_Integer QATutorialAisObject (Draw_Interpretor& theDi,
                                             Standard_Integer  theNbArgs,
                                             const char**      theArgVec)
{
  if (theNbArgs != 2)
  {
    theDi << "Syntax error: wrong number of arguments";
    return 1;
  }
  if (ViewerTest::GetAISContext().IsNull())
  {
    theDi << "Syntax error: no active viewer";
    return 1;
  }

  const TCollection_AsciiString aPrsName (theArgVec[1]);

  Handle(MyAisObject) aPrs = new MyAisObject();
  aPrs->SetAnimation (ViewerTest::CurrentEventManager()->ObjectsAnimation());
  ViewerTest::Display (aPrsName, aPrs);
  return 0;
}

//=======================================================================
//function : TutorialCommands
//purpose  :
//=======================================================================
void QADraw::TutorialCommands (Draw_Interpretor& theCommands)
{
  const char* aGroup = "QA_Commands";

  theCommands.Add ("QATutorialAisObject",
                   "QATutorialAisObject name",
                   __FILE__, QATutorialAisObject, aGroup);
}
