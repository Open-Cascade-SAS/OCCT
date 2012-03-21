// Created on: 1996-02-05
// Created by: Jea Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
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


#include <TestTopOpe_BOOP.hxx>

#include <TopOpeBRepTool.hxx>
#include <TopOpeBRepDS.hxx>
#include <TopOpeBRep_DSFiller.hxx>
#include <TopOpeBRep_define.hxx>

Standard_Integer TestTopOpe_BOOP::DSF_SetInterTolerances(TopOpeBRep_DSFiller& DSF)
{
  if (myVarsTopo.GetForceToli()) {
    TopOpeBRep_ShapeIntersector& tobsi = DSF.ChangeShapeIntersector();
    Standard_Real tola,tolt; myVarsTopo.GetToli(tola,tolt);
    TopOpeBRep_FacesIntersector& tobfi = tobsi.ChangeFacesIntersector();
    tobfi.ForceTolerances(tola,tolt);
    TopOpeBRep_EdgesIntersector& tobei = tobsi.ChangeEdgesIntersector();
    tobei.ForceTolerances(tola,tolt);
  }
  return 0;
}

Standard_Integer TestTopOpe_BOOP::DSF_Insert(TopOpeBRep_DSFiller& DSF)
{
//  Standard_Integer err = 0;
  DSF_Insert_InsertIntersection(DSF);
  DSF_Insert_Complete_CompleteDS(DSF);
  DSF_Insert_Complete_Filter(DSF);
  DSF_Insert_Complete_Reducer(DSF);
  DSF_Insert_Complete_RemoveUnsharedGeometry(DSF);
  DSF_Insert_Complete_Checker(DSF);
  mylastPREP = 190;
  return 0;
}

Standard_Integer TestTopOpe_BOOP::DSF_Insert_InsertIntersection(TopOpeBRep_DSFiller& DSF)
{
//  Standard_Integer err = 0;
  DSF_SetInterTolerances(DSF);
  DSF.InsertIntersection(myS1,myS2,myHDS);
  mylastPREP = 110;
  return 0;
}

Standard_Integer TestTopOpe_BOOP::DSF_Insert_Complete_GapFiller(TopOpeBRep_DSFiller& DSF)
{
//  Standard_Integer err = 0;
  DSF.GapFiller(myHDS);
  mylastPREP = 115;
  return 0;
}

Standard_Integer TestTopOpe_BOOP::DSF_Insert_Complete_CompleteDS(TopOpeBRep_DSFiller& DSF)
{
//  Standard_Integer err = 0;
  DSF.CompleteDS(myHDS);
  mylastPREP = 120;
  return 0;
}

Standard_Integer TestTopOpe_BOOP::DSF_Insert_Complete_Filter(TopOpeBRep_DSFiller& DSF)
{
//  Standard_Integer err = 0;
  DSF.Filter(myHDS);
  mylastPREP = 130;
  return 0;
}

Standard_Integer TestTopOpe_BOOP::DSF_Insert_Complete_Reducer(TopOpeBRep_DSFiller& DSF)
{
//  Standard_Integer err = 0;
  DSF.Reducer(myHDS);
  mylastPREP = 140;
  return 0;
}

Standard_Integer TestTopOpe_BOOP::DSF_Insert_Complete_RemoveUnsharedGeometry(TopOpeBRep_DSFiller& DSF)
{
//  Standard_Integer err = 0;
  DSF.RemoveUnsharedGeometry(myHDS);
  mylastPREP = 150;
  return 0;
}

Standard_Integer TestTopOpe_BOOP::DSF_Insert_Complete_Checker(TopOpeBRep_DSFiller& DSF)
{
//  Standard_Integer err = 0;
  DSF.Checker(myHDS);
  mylastPREP = 160;
  return 0;
}
