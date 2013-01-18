// Created on: 1995-03-15
// Created by: Robert COUBLANC
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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


#include <StdSelect_ViewerSelector3d.ixx>
#include <StdSelect.hxx>
#include <SelectBasics_SensitiveEntity.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax3.hxx>
#include <gp_GTrsf.hxx>
#include <V3d_PerspectiveView.hxx>
#include <V3d_Plane.hxx>
#include <Select3D_SensitiveEntity.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <SelectMgr_DataMapIteratorOfDataMapOfIntegerSensitive.hxx>
#include <SelectBasics_ListOfBox2d.hxx>
#include <Visual3d_TransientManager.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <Select3D_SensitiveCurve.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <Select3D_SensitiveFace.hxx>
#include <Select3D_SensitiveCircle.hxx>
#include <Select3D_SensitivePoint.hxx>
#include <Select3D_SensitiveTriangulation.hxx>
#include <Select3D_SensitiveTriangle.hxx>
#include <Select3D_SensitiveWire.hxx>
#include <Select3D_SensitiveEntitySequence.hxx>
#include <Select3D_ListOfSensitiveTriangle.hxx>
#include <Select3D_SensitiveBox.hxx>
#include <Select3D_ListIteratorOfListOfSensitiveTriangle.hxx>

#include <SelectMgr_DataMapIteratorOfDataMapOfSelectionActivation.hxx>
#include <Aspect_TypeOfMarker.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <SelectBasics_ListIteratorOfListOfBox2d.hxx>
#include <Poly_Connect.hxx>
#include <TColStd_HArray1OfInteger.hxx>

#include <Poly_Array1OfTriangle.hxx>
#include <Poly_Triangulation.hxx>
#include <OSD_Environment.hxx>
#include <V3d.hxx>
#include <V3d_View.hxx>
#include <TColgp_SequenceOfPnt.hxx>


static Standard_Integer StdSel_NumberOfFreeEdges (const Handle(Poly_Triangulation)& Trg)
{
  Standard_Integer nFree = 0;
  Poly_Connect pc(Trg);
  Standard_Integer t[3];
  Standard_Integer i, j;
  for (i = 1; i <= Trg->NbTriangles(); i++)
  {
    pc.Triangles (i, t[0], t[1], t[2]);
    for (j = 0; j < 3; j++)
      if (t[j] == 0) nFree++;
  }
  return nFree;
}

static Standard_Boolean ReadIsDebugMode()
{
  OSD_Environment StdSelectdb ("SELDEBUGMODE");
  return !StdSelectdb.Value().IsEmpty();
}

static Standard_Boolean StdSelectDebugModeOn()
{
  static const Standard_Boolean isDebugMode = ReadIsDebugMode();
  return isDebugMode;
}

//==================================================
// Function:
// Purpose :
//==================================================

StdSelect_ViewerSelector3d
::StdSelect_ViewerSelector3d():
myprj(new Select3D_Projector()),
mylastzoom(0.0),
mysensmode(StdSelect_SM_WINDOW),
mypixtol(2),
myupdatetol(Standard_True)
{
  for (Standard_Integer i=0;i<=13;i++) {mycoeff [i] = 0.;myprevcoeff[i]=0.0;}
  for (Standard_Integer j=0;j<2;j++) {mycenter [j] = 0.;myprevcenter[j]=0.0;}
}


//==================================================
// Function:
// Purpose :
//==================================================

StdSelect_ViewerSelector3d
::StdSelect_ViewerSelector3d(const Handle(Select3D_Projector)& aProj):
myprj(aProj),
mylastzoom(0.0),
mysensmode(StdSelect_SM_WINDOW),
mypixtol(2),
myupdatetol(Standard_True)
{
  for (Standard_Integer i=0;i<=13;i++) {mycoeff [i] = 0.;myprevcoeff[i]=0.0;}
  for (Standard_Integer j=0;j<2;j++) {mycenter [j] = 0.;myprevcenter[j]=0.0;}
}

//==================================================
// Function: Convert
// Purpose :
//==================================================

void StdSelect_ViewerSelector3d::Convert(const Handle(SelectMgr_Selection)& aSel)
{
  for(aSel->Init();aSel->More();aSel->Next())
  {
    if(aSel->Sensitive()->NeedsConversion())
    {
      Handle(Select3D_SensitiveEntity) SE = *((Handle(Select3D_SensitiveEntity)*) &(aSel->Sensitive()));
      SE->Project(myprj);
      if(!tosort) tosort=Standard_True;
    }
  }
}

//==================================================
// Function: Set
// Purpose :
//==================================================

void StdSelect_ViewerSelector3d
::Set(const Handle(Select3D_Projector)& aProj)
{
  myprj = aProj;
  toupdate=Standard_True;
}

//==================================================
// Function: SetSensitivityMode
// Purpose :
//==================================================

void StdSelect_ViewerSelector3d
::SetSensitivityMode(const StdSelect_SensitivityMode aMode)
{
  mysensmode = aMode;
  toupdate = Standard_True;
}

//==================================================
// Function: SetPixelTolerance
// Purpose :
//==================================================

void StdSelect_ViewerSelector3d
::SetPixelTolerance(const Standard_Integer aTolerance)
{
  if(mypixtol!=aTolerance)
  {
    mypixtol    = aTolerance;
    myupdatetol = Standard_True;
  }
}

//==================================================
// Function: SelectPix
// Purpose :
//==================================================

void StdSelect_ViewerSelector3d
::Pick(const Standard_Integer XPix,
       const Standard_Integer YPix,
       const Handle(V3d_View)& aView)
{
  UpdateProj(aView);
  Standard_Real Xr3d,Yr3d,Zr3d;
  gp_Pnt2d P2d;
  aView->Convert(XPix,YPix,Xr3d,Yr3d,Zr3d);
  myprj->Project(gp_Pnt(Xr3d,Yr3d,Zr3d),P2d);

  // compute depth limits if clipping plane(s) enabled
  gp_Lin anEyeLine = myprj->Shoot (P2d.X(), P2d.Y());
  Standard_Real aPlaneA, aPlaneB, aPlaneC, aPlaneD;
  Standard_Real aDepthFrom = ShortRealFirst();
  Standard_Real aDepthTo   = ShortRealLast();
  for (aView->InitActivePlanes(); aView->MoreActivePlanes(); aView->NextActivePlanes())
  {
    aView->ActivePlane()->Plane (aPlaneA, aPlaneB, aPlaneC, aPlaneD);
    const gp_Dir& anEyeLineDir  = anEyeLine.Direction();
    gp_Dir aPlaneNormal (aPlaneA, aPlaneB, aPlaneC);

    Standard_Real aDotProduct = anEyeLineDir.Dot (aPlaneNormal);
    Standard_Real aDirection = -(aPlaneD + anEyeLine.Location().XYZ().Dot (aPlaneNormal.XYZ()));
    if (Abs (aDotProduct) < Precision::Angular())
    {
      // eyeline parallel to the clipping plane
      if (aDirection > 0.0)
      {
        // invalidate the interval
        aDepthTo   = ShortRealFirst();
        aDepthFrom = ShortRealFirst();
        break;
      }
      // just ignore this plane
      continue;
    }

    // compute distance along the eyeline from eyeline location to intersection with clipping plane
    Standard_Real aDepth = aDirection / aDotProduct;

    // reduce depth limits
    if (aDotProduct < 0.0)
    {
      if (aDepth < aDepthTo)
      {
        aDepthTo = aDepth;
      }
    }
    else if (aDepth > aDepthFrom)
    {
      aDepthFrom = aDepth;
    }
  }
  myprj->DepthMinMax (aDepthFrom, aDepthTo);

  InitSelect(P2d.X(),P2d.Y());
}


//==================================================
// Function: InitSelect
// Purpose :
//==================================================

void StdSelect_ViewerSelector3d
::Pick(const Standard_Integer XPMin,
       const Standard_Integer YPMin,
       const Standard_Integer XPMax,
       const Standard_Integer YPMax,
       const Handle(V3d_View)& aView)
{
  if (myupdatetol && SensitivityMode() == StdSelect_SM_WINDOW)
  {
    SetSensitivity (aView->Convert (mypixtol));
    myupdatetol = Standard_False;
  }
  UpdateProj (aView);

  Standard_Real x1,y1,z1,x2,y2,z2;
  gp_Pnt2d P2d_1,P2d_2;
  aView->Convert(XPMin,YPMin,x1,y1,z1);
  aView->Convert(XPMax,YPMax,x2,y2,z2);
  myprj->Project(gp_Pnt(x1,y1,z1),P2d_1);
  myprj->Project(gp_Pnt(x2,y2,z2),P2d_2);

  InitSelect (Min(P2d_1.X(),P2d_2.X()),
              Min(P2d_1.Y(),P2d_2.Y()),
              Max(P2d_1.X(),P2d_2.X()),
              Max(P2d_1.Y(),P2d_2.Y()));
}

//==================================================
// Function: Pick
// Purpose : Selection using a polyline
//==================================================

void StdSelect_ViewerSelector3d::Pick(const TColgp_Array1OfPnt2d& aPolyline, const Handle(V3d_View)& aView)
{
  if (myupdatetol && SensitivityMode() == StdSelect_SM_WINDOW)
  {
    SetSensitivity (aView->Convert (mypixtol));
    myupdatetol = Standard_False;
  }

  UpdateProj (aView);

  Standard_Integer NbPix = aPolyline.Length();
  Standard_Integer i;

  // Convert pixel
  Handle(TColgp_HArray1OfPnt2d) P2d = new TColgp_HArray1OfPnt2d(1,NbPix);

  for (i = 1; i <= NbPix; ++i)
  {
    Standard_Real x,y,z;
    Standard_Integer XP = (Standard_Integer)(aPolyline(i).X());
    Standard_Integer YP = (Standard_Integer)(aPolyline(i).Y());
    gp_Pnt2d Pnt2d;

    aView->Convert (XP, YP, x, y, z);
    myprj->Project (gp_Pnt (x, y, z), Pnt2d);

    P2d->SetValue (i, Pnt2d);
  }

  const TColgp_Array1OfPnt2d& aPolyConvert = P2d->Array1();

  InitSelect(aPolyConvert);
}

//==================================================
// Function: DisplayAreas
// Purpose : display the activated areas...
//==================================================

void StdSelect_ViewerSelector3d::DisplayAreas(const Handle(V3d_View)& aView)
{
  if (myupdatetol && SensitivityMode() == StdSelect_SM_WINDOW)
  {
    SetSensitivity (aView->Convert (mypixtol));
    myupdatetol = Standard_False;
  }
  UpdateProj(aView);
  UpdateSort(); // Updates the activated areas

  if(mystruct.IsNull())
    mystruct = new Graphic3d_Structure(aView->Viewer()->Viewer());

  if(myareagroup.IsNull())
    myareagroup  = new Graphic3d_Group(mystruct);

  SelectMgr_DataMapIteratorOfDataMapOfIntegerSensitive It(myentities);
  Handle(Select3D_Projector) prj = StdSelect::GetProjector(aView);
  prj->SetView(aView);

  Standard_Real xmin,ymin,xmax,ymax;
  gp_Pnt Pbid;
  SelectBasics_ListOfBox2d BoxList;

  TColgp_SequenceOfPnt aSeqLines;
  for (; It.More(); It.Next())
  {
    It.Value()->Areas(BoxList);
    for (SelectBasics_ListIteratorOfListOfBox2d itb (BoxList); itb.More(); itb.Next())
    {
      itb.Value().Get (xmin, ymin, xmax, ymax);

      Pbid.SetCoord (xmin - mytolerance, ymin - mytolerance, 0.0);
      prj->Transform (Pbid, prj->InvertedTransformation());
	  aSeqLines.Append(Pbid);

      Pbid.SetCoord (xmax + mytolerance, ymin - mytolerance, 0.0);
      prj->Transform (Pbid, prj->InvertedTransformation());
	  aSeqLines.Append(Pbid);

      Pbid.SetCoord (xmax + mytolerance, ymax + mytolerance, 0.0);
      prj->Transform (Pbid, prj->InvertedTransformation());
	  aSeqLines.Append(Pbid);

      Pbid.SetCoord (xmin - mytolerance, ymax + mytolerance, 0.0);
      prj->Transform (Pbid, prj->InvertedTransformation());
	  aSeqLines.Append(Pbid);
    }
  }

  if (aSeqLines.Length())
  {
    Standard_Integer n, np;
    const Standard_Integer nbl = aSeqLines.Length() / 4;
    Handle(Graphic3d_ArrayOfPolylines) aPrims = new Graphic3d_ArrayOfPolylines(5*nbl,nbl);
    for (np = 1, n=0; n<nbl; n++) {
      aPrims->AddBound(5);
      const gp_Pnt &p1 = aSeqLines(np++);
      aPrims->AddVertex(p1);
      aPrims->AddVertex(aSeqLines(np++));
      aPrims->AddVertex(aSeqLines(np++));
      aPrims->AddVertex(aSeqLines(np++));
      aPrims->AddVertex(p1);
    }
    myareagroup->AddPrimitiveArray(aPrims);
  }

  myareagroup->SetGroupPrimitivesAspect (new Graphic3d_AspectLine3d (Quantity_NOC_AQUAMARINE1, Aspect_TOL_DASH, 1.0));
  myareagroup->Structure()->SetDisplayPriority(10);
  myareagroup->Structure()->Display();

  if(aView->TransientManagerBeginDraw())
  {
    Visual3d_TransientManager::DrawStructure(mystruct);
    Visual3d_TransientManager::EndDraw();
  }
  else
  {
    aView->Update();
  }
}

//==================================================
// Function: ClearAreas
// Purpose :
//==================================================

void StdSelect_ViewerSelector3d::ClearAreas(const Handle(V3d_View)& aView)
{
  if(myareagroup.IsNull()) return;
  myareagroup->Clear();
  if(aView.IsNull()) return;
  if(aView->TransientManagerBeginDraw())
    Visual3d_TransientManager::EndDraw();
  else
    aView->Update();
}

//==================================================
// Function: updateproj
// Purpose : at any time verifies that
//           the view coefficients did not change :
// store current view coeffts
//        in static array cf [ 0->2 At coordinates XAT YAT ZAT
//                            3->5 Up coordinates XUP YUP ZUP
//                            6->8 ProjVect coordinates DX DY DZ
//                             9   focale
//                            10   1. if pers 0. else
//==================================================

Standard_Boolean StdSelect_ViewerSelector3d::UpdateProj(const Handle(V3d_View)& aView)
{
  myprevcoeff[ 9] = 0.0;
  myprevcoeff[10] = 0.0;
  Standard_Boolean Pers = Standard_False;
  if (aView->Type() == V3d_PERSPECTIVE)
  {
    Pers = Standard_True;
    myprevcoeff[10] = 1.0;
    myprevcoeff[ 9] = aView->Focale();
  }
  aView->At (myprevcoeff[0], myprevcoeff[1], myprevcoeff[2]);
  aView->Up (myprevcoeff[3], myprevcoeff[4], myprevcoeff[5]);
  aView->Proj (myprevcoeff[6], myprevcoeff[7], myprevcoeff[8]);
  aView->AxialScale (myprevcoeff[11], myprevcoeff[12], myprevcoeff[13]);
  aView->Center (myprevcenter[0], myprevcenter[1]);
  Standard_Integer ii;

  for (ii = 0; ii <= 13 && (myprevcoeff[ii] == mycoeff[ii]); ++ii) {}
  if (ii <= 13 || (myprevcenter[0] != mycenter[0]) || (myprevcenter[1] != mycenter[1]))
  {
    if (StdSelectDebugModeOn())
    {
      cout<<"\t\t\t\t\t VS3d::UpdateProj====> coefficients changes on reprojette"<<endl;
      cout<<"\t\t\t\t\t";
      for (Standard_Integer i = 1; i <= 9; ++i)
      {
        cout<<mycoeff[i-1]<<"  ";
        if (i%3==0)
          cout<<"\n\t\t\t\t\t";
      }
      cout<<"focale :"<<mycoeff[9]<<" persp :"<<mycoeff[10]<<endl;
      cout<<"center :"<<mycenter[0]<<"  "<<mycenter[1]<<endl;
    }
    toupdate = Standard_True;
    myupdatetol = Standard_True;
    for (Standard_Integer imod = ii; imod <= 13; ++imod)
    {
      mycoeff[imod] = myprevcoeff[imod];
    }
    for (Standard_Integer jmod = 0; jmod < 2; ++jmod)
    {
      mycenter[jmod] = myprevcenter[jmod];
    }

    gp_Dir Zpers (mycoeff[6], mycoeff[7], mycoeff[8]);
    gp_Dir Ypers (mycoeff[3], mycoeff[4], mycoeff[5]);
    gp_Dir Xpers = Ypers.Crossed (Zpers);
    gp_XYZ loc (mycoeff[0], mycoeff[1], mycoeff[2]);
    gp_Mat matrix;
    matrix.SetCols (Xpers.XYZ(), Ypers.XYZ(), Zpers.XYZ());
    gp_Mat matScale (mycoeff[11], 0, 0, 0, mycoeff[12], 0, 0, 0, mycoeff[13]);
    matrix.Transpose();
    loc.Multiply (matrix);
    loc.Reverse ();
    matrix.Multiply (matScale);
    gp_GTrsf GT;
    GT.SetTranslationPart (loc);
    GT.SetVectorialPart (matrix);

    myprj = new Select3D_Projector (GT, Pers, mycoeff[9]);

    // SAV 08/05/02 : fix for detection problem in a perspective view
    if (aView->Type() == V3d_PERSPECTIVE)
      myprj->SetView (aView);
    // NKV 31/07/07 : fix for detection problem in case of custom matrix
    else if (aView->ViewOrientation().IsCustomMatrix())
      myprj->SetView (aView);
  }

  if (Abs (aView->Scale() - mylastzoom) > 1.e-3)
  {
    myupdatetol = Standard_True;
    mylastzoom = aView->Scale();
  }

  if (myupdatetol && SensitivityMode() == StdSelect_SM_WINDOW)
  {
    SetSensitivity (aView->Convert (mypixtol));
    myupdatetol = Standard_False;
  }

  if (toupdate) UpdateConversion();
  if (tosort) UpdateSort();

  return Standard_True;
}


//=============================
// Function: DisplaySensitive.
// Purpose : Display active primitives.
//=============================
void StdSelect_ViewerSelector3d::DisplaySensitive(const Handle(V3d_View)& aViou)
{
  if (myupdatetol && SensitivityMode() == StdSelect_SM_WINDOW)
  {
    SetSensitivity (aViou->Convert (mypixtol));
		myupdatetol = Standard_False;
  }
  if(toupdate) UpdateProj(aViou);
  if(tosort) UpdateSort(); // Updates the activated areas

  // Preparation des structures
  if(mystruct.IsNull())
    mystruct = new Graphic3d_Structure(aViou->Viewer()->Viewer());

  if(mysensgroup.IsNull())
    mysensgroup = new Graphic3d_Group(mystruct);

  Quantity_Color Col(Quantity_NOC_INDIANRED3);
  Handle(Graphic3d_AspectMarker3d) AM =
    new Graphic3d_AspectMarker3d(Aspect_TOM_O_PLUS,Col,2.);
  mysensgroup-> SetPrimitivesAspect (AM);
  mysensgroup->SetPrimitivesAspect (
    new Graphic3d_AspectLine3d (Quantity_NOC_GRAY40, Aspect_TOL_SOLID, 2.0));

  // Remplissage de la structure...

  SelectMgr_DataMapIteratorOfDataMapOfSelectionActivation It(myselections);

  for (; It.More(); It.Next())
  {
    if (It.Value()==0)
    {
      const Handle(SelectMgr_Selection)& Sel = It.Key();
      ComputeSensitivePrs(Sel);
    }
  }

  mysensgroup->Structure()->SetDisplayPriority(10);
  mystruct->Display();
  if (aViou->TransientManagerBeginDraw())
  {
    Visual3d_TransientManager::DrawStructure(mystruct);
    Visual3d_TransientManager::EndDraw();
  }
  else if (!aViou.IsNull())
  {
    aViou->Update();
  }
}

//=============================
// Function: ClearSensitive
// Purpose :
//=============================
void StdSelect_ViewerSelector3d::ClearSensitive(const Handle(V3d_View)& aViou)
{
  if(mysensgroup.IsNull()) return;
  mysensgroup->Clear();
  if(aViou.IsNull()) return;

  if(aViou->TransientManagerBeginDraw())
    Visual3d_TransientManager::EndDraw();
  else
    aViou->Update();
}

//=======================================================================
//function : DisplaySenstive
//purpose  :
//=======================================================================
void StdSelect_ViewerSelector3d::
DisplaySensitive (const Handle(SelectMgr_Selection)& Sel,
                  const Handle(V3d_View)& aViou,
                  const Standard_Boolean ClearOthers)
{
  if (mystruct.IsNull())
    mystruct = new Graphic3d_Structure (aViou->Viewer()->Viewer());
  if (mysensgroup.IsNull())
  {
    mysensgroup = new Graphic3d_Group (mystruct);
    Quantity_Color Col (Quantity_NOC_INDIANRED3);
    Handle(Graphic3d_AspectMarker3d) AM =
      new Graphic3d_AspectMarker3d (Aspect_TOM_O_PLUS, Col, 2.0);
    mysensgroup-> SetPrimitivesAspect (AM);
    mysensgroup->SetPrimitivesAspect (
      new Graphic3d_AspectLine3d (Quantity_NOC_GRAY40, Aspect_TOL_SOLID, 2.0));
  }

  if(ClearOthers) mysensgroup->Clear();

  ComputeSensitivePrs(Sel);

  mystruct->SetDisplayPriority(10);
  mystruct->Display();
  if(aViou->TransientManagerBeginDraw())
  {
    Visual3d_TransientManager::DrawStructure(mystruct);
    Visual3d_TransientManager::EndDraw();
  }
  else if(!aViou.IsNull())
  {
    aViou->Update();
  }
}

//=======================================================================
//function : DisplayAreas
//purpose  :
//=======================================================================

void StdSelect_ViewerSelector3d::
DisplayAreas (const Handle(SelectMgr_Selection)& Sel,
              const Handle(V3d_View)& aViou,
              const Standard_Boolean ClearOthers)
{
  if (mystruct.IsNull())
    mystruct = new Graphic3d_Structure (aViou->Viewer()->Viewer());

  if (mysensgroup.IsNull())
  {
    myareagroup = new Graphic3d_Group (mystruct);
    myareagroup->SetGroupPrimitivesAspect(new Graphic3d_AspectLine3d (Quantity_NOC_AQUAMARINE1, Aspect_TOL_DASH, 1.0));
  }

  if(ClearOthers) myareagroup->Clear();

  ComputeAreasPrs(Sel);

  mystruct->SetDisplayPriority(10);
  mystruct->Display();

  if(aViou->TransientManagerBeginDraw())
  {
    Visual3d_TransientManager::DrawStructure(mystruct);
    Visual3d_TransientManager::EndDraw();
  }
  else
  {
    aViou->Update();
  }
}

//=======================================================================
//function : ComputeSensitivePrs
//purpose  :
//=======================================================================

void StdSelect_ViewerSelector3d::ComputeSensitivePrs(const Handle(SelectMgr_Selection)& Sel)
{
  TColgp_SequenceOfPnt aSeqLines, aSeqFree;
  TColStd_SequenceOfInteger aSeqBnds;

  for(Sel->Init();Sel->More();Sel->Next())
  {
    Handle(Select3D_SensitiveEntity) Ent = Handle(Select3D_SensitiveEntity)::DownCast(Sel->Sensitive());
    const Standard_Boolean hasloc = (Ent.IsNull()? Standard_False : Ent->HasLocation());

    TopLoc_Location theloc;
    if(hasloc)
      theloc = Ent->Location();

    //==============
    // Box
    //=============

    if (Ent->DynamicType()==STANDARD_TYPE(Select3D_SensitiveBox))
    {
      const Bnd_Box& B = Handle(Select3D_SensitiveBox)::DownCast (Ent)->Box();
      Standard_Real xmin, ymin, zmin, xmax, ymax, zmax;
      B.Get (xmin, ymin, zmin, xmax, ymax, zmax);
      Standard_Integer i;
      gp_Pnt theboxpoint[8] =
      {
        gp_Pnt(xmin,ymin,zmin),
        gp_Pnt(xmax,ymin,zmin),
        gp_Pnt(xmax,ymax,zmin),
        gp_Pnt(xmin,ymax,zmin),
        gp_Pnt(xmin,ymin,zmax),
        gp_Pnt(xmax,ymin,zmax),
        gp_Pnt(xmax,ymax,zmax),
        gp_Pnt(xmin,ymax,zmax)
      };
      if(hasloc)
      {
        for (i = 0; i <= 7; i++)
          theboxpoint[i].Transform (theloc.Transformation());
      }

      aSeqBnds.Append(5);
      for (i = 0; i < 4; i++)
        aSeqLines.Append(theboxpoint[i]);
      aSeqLines.Append(theboxpoint[0]);

      aSeqBnds.Append(5);
      for (i = 4; i < 8; i++)
        aSeqLines.Append(theboxpoint[i]);
      aSeqLines.Append(theboxpoint[4]);

      for (i = 0; i < 4; i++)
      {
        aSeqBnds.Append(2);
        aSeqLines.Append(theboxpoint[i]);
        aSeqLines.Append(theboxpoint[i+4]);
      }
    }
    //==============
    // Face
    //=============
    else if (Ent->DynamicType()==STANDARD_TYPE(Select3D_SensitiveFace))
    {
      Handle(Select3D_SensitiveFace) aFace = Handle(Select3D_SensitiveFace)::DownCast(Ent);
      Handle(TColgp_HArray1OfPnt) TheHPts;
      aFace->Points3D(TheHPts);
      const TColgp_Array1OfPnt& ThePts = TheHPts->Array1();

      aSeqBnds.Append(ThePts.Length());
      for (Standard_Integer I = ThePts.Lower(); I <= ThePts.Upper(); I++)
      {
        if (hasloc)
          aSeqLines.Append(ThePts(I).Transformed (theloc.Transformation()));
        else
          aSeqLines.Append(ThePts(I));
      }
    }
    //==============
    // Curve
    //=============
    else if (Ent->DynamicType()==STANDARD_TYPE(Select3D_SensitiveCurve))
    {
      Handle(Select3D_SensitiveCurve) aCurve = Handle(Select3D_SensitiveCurve)::DownCast(Ent);
      Handle(TColgp_HArray1OfPnt) TheHPts;
      aCurve->Points3D(TheHPts);
      const TColgp_Array1OfPnt& ThePts = TheHPts->Array1();

      aSeqBnds.Append(ThePts.Length());
      for (Standard_Integer I = ThePts.Lower(); I <= ThePts.Upper(); I++)
      {
        if (hasloc)
          aSeqLines.Append(ThePts(I).Transformed (theloc.Transformation()));
        else
          aSeqLines.Append(ThePts(I));
      }
    }
    //==============
    // Wire
    //=============
    else if (Ent->DynamicType()==STANDARD_TYPE(Select3D_SensitiveWire))
    {
      Handle(Select3D_SensitiveWire) aWire = Handle(Select3D_SensitiveWire)::DownCast(Ent);
      Select3D_SensitiveEntitySequence EntitySeq;
      aWire->GetEdges (EntitySeq);

      for (int i = 1; i <= EntitySeq.Length(); i++)
      {
        Handle(Select3D_SensitiveEntity) SubEnt = Handle(Select3D_SensitiveEntity)::DownCast(EntitySeq.Value(i));

        //Segment
        if (SubEnt->DynamicType()==STANDARD_TYPE(Select3D_SensitiveSegment))
        {
          gp_Pnt P1 (Handle(Select3D_SensitiveSegment)::DownCast(SubEnt)->StartPoint().XYZ());
          gp_Pnt P2 (Handle(Select3D_SensitiveSegment)::DownCast(SubEnt)->EndPoint().XYZ());
          if (hasloc)
          {
            P1.Transform(theloc.Transformation());
            P2.Transform(theloc.Transformation());
          }
          aSeqBnds.Append(2);
          aSeqLines.Append(P1);
          aSeqLines.Append(P2);
        }

        //circle
        if (SubEnt->DynamicType()==STANDARD_TYPE(Select3D_SensitiveCircle))
        {
          Handle(Select3D_SensitiveCircle) C = Handle(Select3D_SensitiveCircle)::DownCast(SubEnt);
          Standard_Integer Lo, Up;
          C->ArrayBounds (Lo, Up);
          Standard_Integer II = Lo;
          while (II <= Up - 2)
          {
            gp_Pnt ThePts[3] =
            {
              gp_Pnt (C->GetPoint3d (II).XYZ()),
              gp_Pnt (C->GetPoint3d (++II).XYZ()),
              gp_Pnt (C->GetPoint3d (++II).XYZ())
            };

            if (hasloc)
            {
              for (Standard_Integer jj = 0; jj <= 2; jj++)
                ThePts[jj].Transform (theloc.Transformation());
            }

            aSeqBnds.Append(4);
            aSeqLines.Append(ThePts[0]);
            aSeqLines.Append(ThePts[1]);
            aSeqLines.Append(ThePts[2]);
            aSeqLines.Append(ThePts[0]);
          }
        }

        //curve
        if (SubEnt->DynamicType()==STANDARD_TYPE(Select3D_SensitiveCurve))
        {
          Handle(Select3D_SensitiveCurve) aCurve = Handle(Select3D_SensitiveCurve)::DownCast(SubEnt);
          Handle(TColgp_HArray1OfPnt) TheHPts;
          aCurve->Points3D (TheHPts);
          const TColgp_Array1OfPnt& ThePts = TheHPts->Array1();

          aSeqBnds.Append(ThePts.Length());
          for (Standard_Integer I = ThePts.Lower(); I <= ThePts.Upper(); I++)
          {
            if (hasloc)
              aSeqLines.Append(ThePts(I).Transformed (theloc.Transformation()));
            else
              aSeqLines.Append(ThePts(I));
          }
        }
      }
    }
    //==============
    // Segment
    //=============
    else if (Ent->DynamicType()==STANDARD_TYPE(Select3D_SensitiveSegment))
    {
      gp_Pnt P1 (Handle(Select3D_SensitiveSegment)::DownCast(Ent)->StartPoint().XYZ());
      gp_Pnt P2 (Handle(Select3D_SensitiveSegment)::DownCast(Ent)->EndPoint().XYZ());
      if (hasloc)
      {
        P1.Transform (theloc.Transformation());
        P2.Transform (theloc.Transformation());
      }
      aSeqBnds.Append(2);
      aSeqLines.Append(P1);
      aSeqLines.Append(P2);
    }
    //==============
    // Circle
    //=============
    else if (Ent->DynamicType()==STANDARD_TYPE(Select3D_SensitiveCircle))
    {
      Handle(Select3D_SensitiveCircle) C = Handle(Select3D_SensitiveCircle)::DownCast(Ent);
      Standard_Integer Lo, Up;
      C->ArrayBounds (Lo, Up);
      Standard_Integer II = Lo;
      while (II <= Up - 2)
      {
        gp_Pnt ThePts[3] =
        {
          gp_Pnt (C->GetPoint3d (II).XYZ()),
          gp_Pnt (C->GetPoint3d (++II).XYZ()),
          gp_Pnt (C->GetPoint3d (++II).XYZ())
        };

        if (hasloc)
        {
          for (Standard_Integer jj = 0; jj <= 2; jj++)
            ThePts[jj].Transform (theloc.Transformation());
        }

        aSeqBnds.Append(4);
        aSeqLines.Append(ThePts[0]);
        aSeqLines.Append(ThePts[1]);
        aSeqLines.Append(ThePts[2]);
        aSeqLines.Append(ThePts[0]);
      }
    }
    //==============
    // Point
    //=============
    else if (Ent->DynamicType()==STANDARD_TYPE(Select3D_SensitivePoint))
    {
      gp_Pnt P = hasloc ?
        Handle(Select3D_SensitivePoint)::DownCast(Ent)->Point() :
        Handle(Select3D_SensitivePoint)::DownCast(Ent)->Point().Transformed (theloc.Transformation());
      Graphic3d_Vertex V (P.X(), P.Y(), P.Z());
      mysensgroup->Marker (V);
    }
    //============================================================
    // Triangulation : On met un petit offset ves l'interieur...
    //==========================================================
    else if (Ent->DynamicType()==STANDARD_TYPE(Select3D_SensitiveTriangulation))
    {
      const Handle(Poly_Triangulation)& PT =
        (*((Handle(Select3D_SensitiveTriangulation)*) &Ent))->Triangulation();

      const Poly_Array1OfTriangle& triangles = PT->Triangles();
      const TColgp_Array1OfPnt& Nodes = PT->Nodes();
      Standard_Integer n[3];

      TopLoc_Location iloc, bidloc;
      if ((*((Handle(Select3D_SensitiveTriangulation)*) &Ent))->HasInitLocation())
        bidloc = (*((Handle(Select3D_SensitiveTriangulation)*) &Ent))->GetInitLocation();

      if (bidloc.IsIdentity())
        iloc = theloc;
      else
        iloc = theloc * bidloc;

      Standard_Integer i;
      for (i = 1; i <= PT->NbTriangles(); i++)
      {
        triangles (i).Get (n[0], n[1], n[2]);
        gp_Pnt P1 (Nodes (n[0]).Transformed (iloc));
        gp_Pnt P2 (Nodes (n[1]).Transformed (iloc));
        gp_Pnt P3 (Nodes (n[2]).Transformed (iloc));
        gp_XYZ V1 (P1.XYZ());
        gp_XYZ V2 (P2.XYZ());
        gp_XYZ V3 (P3.XYZ());
        gp_XYZ CDG (P1.XYZ()); CDG += (P2.XYZ()); CDG += (P3.XYZ()); CDG /= 3.0;
        V1 -= CDG; V2 -= CDG; V3 -= CDG;
        V1 *= 0.9; V2 *= 0.9; V3 *= 0.9;
        V1 += CDG; V2 += CDG; V3 += CDG;

        aSeqBnds.Append(4);
        aSeqLines.Append(gp_Pnt(V1));
        aSeqLines.Append(gp_Pnt(V2));
        aSeqLines.Append(gp_Pnt(V3));
        aSeqLines.Append(gp_Pnt(V1));
      }

      // recherche des bords libres...

      Handle(TColStd_HArray1OfInteger) FreeEdges = new TColStd_HArray1OfInteger (1, 2 * StdSel_NumberOfFreeEdges (PT));
      TColStd_Array1OfInteger& FreeE = FreeEdges->ChangeArray1();
      Poly_Connect pc (PT);
      Standard_Integer t[3];
      Standard_Integer j;
      Standard_Integer fr (1);
      for (i = 1; i <= PT->NbTriangles(); i++)
      {
        pc.Triangles (i, t[0], t[1], t[2]);
        triangles (i).Get (n[0], n[1], n[2]);
        for (j = 0; j < 3; j++)
        {
          Standard_Integer k = (j + 1) % 3;
          if (t[j] == 0)
          {
            FreeE (fr)    = n[j];
            FreeE (fr + 1)= n[k];
            fr += 2;
          }
        }
      }
      for (Standard_Integer ifri = 1; ifri <= FreeE.Length(); ifri += 2)
      {
        gp_Pnt pe1 (Nodes (FreeE (ifri)).Transformed (iloc)), pe2 (Nodes (FreeE (ifri + 1)).Transformed (iloc));
        aSeqFree.Append(pe1);
        aSeqFree.Append(pe2);
      }
    }
    else if (Ent->DynamicType()==STANDARD_TYPE(Select3D_SensitiveTriangle))
    {
      Handle(Select3D_SensitiveTriangle) Str = Handle(Select3D_SensitiveTriangle)::DownCast(Ent);
      gp_Pnt P1, P2, P3;
      Str->Points3D (P1, P2, P3);
      gp_Pnt CDG = Str->Center3D();

      gp_XYZ V1 (P1.XYZ()); V1 -= (CDG.XYZ());
      gp_XYZ V2 (P2.XYZ()); V2 -= (CDG.XYZ());
      gp_XYZ V3 (P3.XYZ()); V3 -= (CDG.XYZ());
      V1 *= 0.9; V2 *= 0.9; V3 *= 0.9;
      V1 += CDG.XYZ(); V2 += CDG.XYZ(); V3 += CDG.XYZ();

      aSeqBnds.Append(4);
      aSeqLines.Append(gp_Pnt(V1));
      aSeqLines.Append(gp_Pnt(V2));
      aSeqLines.Append(gp_Pnt(V3));
      aSeqLines.Append(gp_Pnt(V1));
    }
  }

  Standard_Integer i;

  if (aSeqLines.Length())
  {
    Handle(Graphic3d_ArrayOfPolylines) aPrims = new Graphic3d_ArrayOfPolylines(aSeqLines.Length(),aSeqBnds.Length());
    for (i = 1; i <= aSeqLines.Length(); i++)
      aPrims->AddVertex(aSeqLines(i));
    for (i = 1; i <= aSeqBnds.Length(); i++)
      aPrims->AddBound(aSeqBnds(i));
    myareagroup->AddPrimitiveArray(aPrims);
  }

  if (aSeqFree.Length())
  {
    mysensgroup->SetPrimitivesAspect (new Graphic3d_AspectLine3d (Quantity_NOC_GREEN, Aspect_TOL_SOLID, 2.0));
    Handle(Graphic3d_ArrayOfPolylines) aPrims = new Graphic3d_ArrayOfPolylines(aSeqFree.Length(),aSeqFree.Length()/2);
    for (i = 1; i <= aSeqFree.Length(); i++)
	{
      aPrims->AddBound(2);
      aPrims->AddVertex(aSeqLines(i++));
      aPrims->AddVertex(aSeqLines(i));
	}
    mysensgroup->AddPrimitiveArray(aPrims);
    mysensgroup->SetPrimitivesAspect (new Graphic3d_AspectLine3d (Quantity_NOC_GRAY40, Aspect_TOL_SOLID, 2.0));
  }
}

//=======================================================================
//function : ComputeAreaPrs
//purpose  :
//=======================================================================

void StdSelect_ViewerSelector3d::ComputeAreasPrs (const Handle(SelectMgr_Selection)& Sel)
{
  Standard_Real xmin, ymin, xmax, ymax;
  gp_Pnt Pbid;
  SelectBasics_ListOfBox2d BoxList;

  TColgp_SequenceOfPnt aSeqLines;
  for (Sel->Init(); Sel->More(); Sel->Next())
  {
    Sel->Sensitive()->Areas (BoxList);
    for (SelectBasics_ListIteratorOfListOfBox2d itb (BoxList); itb.More(); itb.Next())
    {
      itb.Value().Get (xmin, ymin, xmax, ymax);

      Pbid.SetCoord (xmin - mytolerance, ymin - mytolerance, 0.0);
      myprj->Transform (Pbid, myprj->InvertedTransformation());
      aSeqLines.Append(Pbid);

      Pbid.SetCoord (xmax + mytolerance, ymin - mytolerance, 0.0);
      myprj->Transform (Pbid, myprj->InvertedTransformation());
      aSeqLines.Append(Pbid);

      Pbid.SetCoord (xmax + mytolerance, ymax + mytolerance, 0.0);
      myprj->Transform (Pbid, myprj->InvertedTransformation());
      aSeqLines.Append(Pbid);

      Pbid.SetCoord (xmin - mytolerance, ymax + mytolerance, 0.0);
      myprj->Transform (Pbid, myprj->InvertedTransformation());
      aSeqLines.Append(Pbid);
    }
  }

  if (aSeqLines.Length())
  {
    Standard_Integer n, np;
    const Standard_Integer nbl = aSeqLines.Length() / 4;
    Handle(Graphic3d_ArrayOfPolylines) aPrims = new Graphic3d_ArrayOfPolylines(5*nbl,nbl);
    for (np = 1, n=0; n<nbl; n++) {
      aPrims->AddBound(5);
      const gp_Pnt &p1 = aSeqLines(np++);
      aPrims->AddVertex(p1);
      aPrims->AddVertex(aSeqLines(np++));
      aPrims->AddVertex(aSeqLines(np++));
      aPrims->AddVertex(aSeqLines(np++));
      aPrims->AddVertex(p1);
    }
    myareagroup->AddPrimitiveArray(aPrims);
  }
}

//=======================================================================
//function : ReactivateProjector
//purpose  :
//=======================================================================
void StdSelect_ViewerSelector3d::ReactivateProjector()
{
  Handle(SelectBasics_SensitiveEntity) BS;
  for (SelectMgr_DataMapIteratorOfDataMapOfIntegerSensitive it (myentities); it.More(); it.Next())
  {
    BS = it.Value();
    if (BS->Is3D())
    {
      (*((Handle(Select3D_SensitiveEntity)*) &BS))->SetLastPrj (myprj);
    }
  }
}
