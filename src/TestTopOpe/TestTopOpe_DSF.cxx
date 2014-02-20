// Created on: 1996-02-05
// Created by: Jea Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
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
