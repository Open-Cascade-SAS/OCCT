// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//#54 rln 24.12.98 CCI60005

#include <gp_XYZ.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESGeom_BSplineCurve.hxx>
#include <IGESGeom_ToolBSplineCurve.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <Standard_DomainError.hxx>
#include <TColgp_HArray1OfXYZ.hxx>
#include <TColStd_HArray1OfReal.hxx>

// MGE 29/07/98
//=======================================================================
//function : IGESGeom_ToolBSplineCurve
//purpose  : 
//=======================================================================
IGESGeom_ToolBSplineCurve::IGESGeom_ToolBSplineCurve ()
{
}


//=======================================================================
//function : ReadOwnParams
//purpose  : 
//=======================================================================

void IGESGeom_ToolBSplineCurve::ReadOwnParams(const Handle(IGESGeom_BSplineCurve)& ent,
                                              const Handle(IGESData_IGESReaderData)& /* IR */,
                                              IGESData_ParamReader& PR) const
{
  // MGE 29/07/98
  // Building of messages
  //========================================
  Message_Msg Msg99("XSTEP_99");
  Message_Msg Msg100("XSTEP_100");
  Message_Msg Msg101("XSTEP_101");
  Message_Msg Msg102("XSTEP_102");
  Message_Msg Msg103("XSTEP_103");
  //========================================

  Standard_Integer anIndex, aDegree;
  Standard_Boolean aPlanar, aClosed, aPolynomial, aPeriodic;
  Standard_Real aUmin, aUmax, normX,normY,normZ;
  gp_XYZ aNorm (0.,0.,0.);
  Handle(TColStd_HArray1OfReal) allKnots;
  Handle(TColStd_HArray1OfReal) allWeights;
  Handle(TColgp_HArray1OfXYZ)   allPoles;

  //Standard_Boolean st; //szv#4:S4163:12Mar99 moved down

  //st = PR.ReadInteger(PR.Current(), Msg97, anIndex); //szv#4:S4163:12Mar99 moved in if
  //st = PR.ReadInteger(PR.Current(), "Upper Index Of Sum", anIndex);

  //szv#4:S4163:12Mar99 optimized
  /*if (st && anIndex >= 0) {
    allPoles   = new TColgp_HArray1OfXYZ(0, anIndex);
    // allWeights = new TColStd_HArray1OfReal(1, anIndex+1);  done by ReadReals
  }
  
  if (st && anIndex < 0)
  { 
    PR.SendFail(Msg97);
    anIndex = 0;
  }*/
  if (PR.ReadInteger(PR.Current(), anIndex)) {
    if (anIndex < 0) {
      Message_Msg Msg97("XSTEP_97");
      PR.SendFail(Msg97);
      anIndex = 0;
    }
    else {
      allPoles   = new TColgp_HArray1OfXYZ(0, anIndex);
      // allWeights = new TColStd_HArray1OfReal(1, anIndex+1);  done by ReadReals
    }
  }
  else{
    Message_Msg Msg97("XSTEP_97");
    PR.SendFail(Msg97);
  }

  //st = PR.ReadInteger(PR.Current(), Msg98, aDegree); //szv#4:S4163:12Mar99 moved in if
//  if (st && ! allWeights.IsNull() )   done by ReadReals
//    allKnots = new TColStd_HArray1OfReal(-aDegree, anIndex+1);
  if (!PR.ReadInteger(PR.Current(), aDegree)){
    aDegree = 0; //szv#4:S4163:12Mar99 `st=` not needed
    Message_Msg Msg98("XSTEP_98");
    PR.SendFail(Msg98);
  }
  //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadBoolean(PR.Current(), Msg99, aPlanar);
  PR.ReadBoolean(PR.Current(), Msg100, aClosed);
  PR.ReadBoolean(PR.Current(), Msg101, aPolynomial);
  PR.ReadBoolean(PR.Current(), Msg102, aPeriodic);

//st = PR.ReadBoolean(PR.Current(), "Planar/Non Planar Flag", aPlanar);
//st = PR.ReadBoolean(PR.Current(), "Open/Closed Flag", aClosed);
//st = PR.ReadBoolean(PR.Current(), "Rational/Polynomial Flag", aPolynomial);
//st = PR.ReadBoolean(PR.Current(), "NonPeriodic/Periodic Flag", aPeriodic);

  Standard_Integer nbKnots = anIndex + aDegree + 2;
  // Reading all the knot sequences 

  PR.ReadReals(PR.CurrentList(nbKnots), Msg103 , allKnots, -aDegree); //szv#4:S4163:12Mar99 `st=` not needed

//st = PR.ReadReals
//  (PR.CurrentList(nbKnots), "Knot sequence values", allKnots, -aDegree);

  if (! allPoles.IsNull() )
    {
      Message_Msg Msg104("XSTEP_104");
      Message_Msg Msg105("XSTEP_105");
      PR.ReadReals(PR.CurrentList(anIndex+1), Msg104, allWeights,0); //szv#4:S4163:12Mar99 `st=` not needed
      //st = PR.ReadReals(PR.CurrentList(anIndex+1), "Weights", allWeights,0);

      for (Standard_Integer I = 0; I <= anIndex; I ++) 
	{
          gp_XYZ tempPole;
          //st = PR.ReadXYZ(PR.CurrentList(1, 3), Msg105, tempPole); //szv#4:S4163:12Mar99 moved down
          //st = PR.ReadXYZ(PR.CurrentList(1, 3), "Control Points", tempPole);
          if (PR.ReadXYZ(PR.CurrentList(1, 3), Msg105, tempPole)) allPoles->SetValue(I, tempPole);
	}
    }

  if (!PR.ReadReal(PR.Current(), aUmin)){
    Message_Msg Msg106("XSTEP_106");
    PR.SendFail(Msg106);
  } //szv#4:S4163:12Mar99 `st=` not needed
  if (!PR.ReadReal(PR.Current(), aUmax)){
    Message_Msg Msg107("XSTEP_107");
    PR.SendFail(Msg107);
  } //szv#4:S4163:12Mar99 `st=` not needed
/*
  st = PR.ReadReal(PR.Current(), "Starting Parameter Value", aUmin);
  st = PR.ReadReal(PR.Current(), "Ending Parameter Value", aUmax);
*/
  Standard_Boolean st = Standard_False;
  if (PR.DefinedElseSkip()){
    st = PR.ReadReal(PR.Current(),  normX);
    if(!st){
      Message_Msg Msg108("XSTEP_108");
      PR.SendFail(Msg108);
    }
  }
    //st = PR.ReadReal(PR.Current(), "Unit Normal X", normX);
  else normX = 0.;
  if (PR.DefinedElseSkip()){
    st = PR.ReadReal(PR.Current(),  normY);
    if(!st){
      Message_Msg Msg108("XSTEP_108");
      PR.SendFail(Msg108);
    }
  }
    //st = PR.ReadReal(PR.Current(), "Unit Normal Y", normY);
  else normY = 0.;
  if (PR.DefinedElseSkip()){
    st = PR.ReadReal(PR.Current(), normZ);
    if(!st){
      Message_Msg Msg108("XSTEP_108");
      PR.SendFail(Msg108);
    }
  }
    //st = PR.ReadReal(PR.Current(), "Unit Normal Z", normZ);
  else normZ = 0.;
  if (st) aNorm.SetCoord(normX,normY,normZ);

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init
    (anIndex,aDegree, aPlanar, aClosed, aPolynomial, aPeriodic,
     allKnots, allWeights, allPoles, aUmin, aUmax, aNorm);
}


//=======================================================================
//function : WriteOwnParams
//purpose  : 
//=======================================================================

void IGESGeom_ToolBSplineCurve::WriteOwnParams
  (const Handle(IGESGeom_BSplineCurve)& ent, IGESData_IGESWriter& IW)  const
{
  Standard_Integer low, up ;
  Standard_Integer I ;
  Standard_Integer index  = ent->UpperIndex();
  Standard_Integer degree = ent->Degree();
  IW.Send(index);
  IW.Send(degree);
  IW.SendBoolean(ent->IsPlanar());
  IW.SendBoolean(ent->IsClosed());
  IW.SendBoolean(ent->IsPolynomial());
  IW.SendBoolean(ent->IsPeriodic());

  low = -degree;
  up  = index + 1;
  for (I = low; I <= up; I ++)
    IW.Send(ent->Knot(I));

  low = 0;
  up  = index;
  for (I = low; I <= up; I ++)
    IW.Send(ent->Weight(I));

  for (I = low; I <= up; I ++) {
    IW.Send((ent->Pole(I)).X());
    IW.Send((ent->Pole(I)).Y());
    IW.Send((ent->Pole(I)).Z());
  }
  IW.Send(ent->UMin());
  IW.Send(ent->UMax());
  IW.Send(ent->Normal().X());
  IW.Send(ent->Normal().Y());
  IW.Send(ent->Normal().Z());
}


//=======================================================================
//function : OwnShared
//purpose  : 
//=======================================================================

void IGESGeom_ToolBSplineCurve::OwnShared(const Handle(IGESGeom_BSplineCurve)& /* ent */,
                                          Interface_EntityIterator& /* iter */) const
{
}


//=======================================================================
//function : OwnCopy
//purpose  : 
//=======================================================================

void IGESGeom_ToolBSplineCurve::OwnCopy 
  (const Handle(IGESGeom_BSplineCurve)& another,
   const Handle(IGESGeom_BSplineCurve)& ent, Interface_CopyTool& /* TC */) const
{
  Standard_Integer I;
  Standard_Integer low, up;
  Standard_Integer anIndex, aDegree;
  Standard_Boolean aPlanar, aClosed, aPolynomial, aPeriodic;
  Handle(TColStd_HArray1OfReal) allKnots, allWeights;
  Handle(TColgp_HArray1OfXYZ) allPoles;
  Standard_Real aUmin, aUmax;
  gp_XYZ aNorm;

  anIndex = another->UpperIndex();
  aDegree = another->Degree();
  aPlanar = another->IsPlanar();
  aClosed = another->IsClosed();
  aPolynomial = another->IsPolynomial();
  aPeriodic = another->IsPeriodic();

  allKnots = new TColStd_HArray1OfReal(-aDegree, anIndex+1);

  low = -aDegree;
  up  = anIndex + 1;
  for (I = low; I <= up; I++)
    allKnots->SetValue(I, another->Knot(I));

  allWeights = new TColStd_HArray1OfReal(0, anIndex);

  low = 0;
  up  = anIndex;
  for (I = low; I <= up; I++)
    allWeights->SetValue(I, another->Weight(I));

  allPoles = new TColgp_HArray1OfXYZ(0, anIndex);

  for (I = low; I <= up; I++) 
    allPoles->SetValue(I, (another->Pole(I)).XYZ());

  aUmin = another->UMin();
  aUmax = another->UMax();
  aNorm = another->Normal();

  ent->Init (anIndex,aDegree, aPlanar, aClosed, aPolynomial, aPeriodic,
	     allKnots, allWeights, allPoles, aUmin, aUmax, aNorm);
}


//=======================================================================
//function : DirChecker
//purpose  : 
//=======================================================================

IGESData_DirChecker IGESGeom_ToolBSplineCurve::DirChecker
  (const Handle(IGESGeom_BSplineCurve)& /* ent */ )   const
{
  IGESData_DirChecker DC(126, 0, 5);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
//  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  DC.HierarchyStatusIgnored();
  return DC;
}


//=======================================================================
//function : OwnCheck
//purpose  : 
//=======================================================================

void IGESGeom_ToolBSplineCurve::OwnCheck(const Handle(IGESGeom_BSplineCurve)& ent,
                                         const Interface_ShareTool&,
                                         Handle(Interface_Check)& ach)  const
{
  // MGE 29/07/98
  // Building of messages
  //========================================
  //Message_Msg Msg104("XSTEP_104");
  //Message_Msg Msg109("XSTEP_109");
  //========================================

  Standard_Real eps = 1.E-04;    // Tolerance des tests ??
//  Standard_Real norm = ent->Normal().SquareModulus();

  //modified by rln 17/12/97 check of flag PROP2 according to IGES Standard
  //It is possible to compare V(0) and V(1) only if StartingParameter = FirstKnot
  //and EndingParameter = LastKnot (else we must build real geometrical curve)
  //The fail is replaced with warning because it is not a serious problem
  //if (ent->UMin() == ent->Knot(-ent->Degree()        ) &&
  //  ent->UMax() == ent->Knot( ent->UpperIndex() + 1)   ) {
  //  Standard_Real udif = ent->UMax() - ent->UMin();
  //  if (udif < 0) udif = -udif;
  //    Standard_Real udif = ent->Pole(0).SquareDistance (ent->Pole(ent->UpperIndex()));
  //   if (udif <  eps * eps && !ent->IsClosed())
  //     ach.AddWarning("V(0) == V(1) for an Open Curve (PROP2 = 0)");
  //   if (udif >= eps * eps &&  ent->IsClosed())
  //     ach.AddWarning("V(0) != V(1) for a Closed Curve (PROP2 = 1)");
  //}

  Standard_Integer lower = 0;
  Standard_Integer upper = ent->UpperIndex();
  Standard_Boolean Flag  = Standard_True;
  
  Standard_Integer I; // svv Jan 11 2000 : porting on DEC
  for (I = 0; ((I < upper) && (Flag)); I++)
    Flag &= (ent->Weight(I) > 0);
  
  if (!Flag) {
    Message_Msg Msg104("XSTEP_104");
    ach->SendFail(Msg104);
  }
  
  Flag = Standard_True;
  Standard_Real tempVal = ent->Weight(lower);
  
  for (I = lower; ((I < upper) && (Flag)); I++)
    Flag &= (ent->Weight(I) == tempVal);
/*
  if (Flag && !ent->IsPolynomial(Standard_True))
    ach.AddWarning("All weights equal & PROP3 != 1 (Curve Not Polynomial)");
  if (!Flag && ent->IsPolynomial(Standard_True))
    ach.AddWarning("All weights not equal & PROP3 != 0 (Curve Not Rational)");
*/

  if (ent->IsPlanar()) {
    gp_XYZ aNorm = ent->Normal();
    Standard_Real epsn = eps * 10.;    // Tolerance ?? ici large
    Standard_Real normod = aNorm.SquareModulus();
    if (normod < epsn) {
      Message_Msg Msg109("XSTEP_109");
      ach->AddWarning(Msg109);
    }
  }
}


//=======================================================================
//function : OwnDump
//purpose  : 
//=======================================================================

void IGESGeom_ToolBSplineCurve::OwnDump(const Handle(IGESGeom_BSplineCurve)& ent,
                                        const IGESData_IGESDumper& /* dumper */,
                                        const Handle(Message_Messenger)& S,
                                        const Standard_Integer level)  const
{
  Standard_Integer upind = ent->UpperIndex();
  S << "BSplineCurve from IGESGeom" << endl;
  S << "Sum UpperIndex : " << upind;
  S << "   Degree : " << ent->Degree() << "  ";
  S << (ent->IsPlanar() ? "Planar" : "NonPlanar") << endl;
  S << (ent->IsClosed() ? "Closed" : "Open") << "  ";
  S << (ent->IsPeriodic() ? "Periodic" : "NonPeriodic") << "  ";//#54 rln 24.12.98 CCI60005
  S << (ent->IsPolynomial(Standard_True) ? "Polynomial" : "Rational");

  S << endl << "Knots : ";
  IGESData_DumpVals(S ,level,-ent->Degree(), upind+1,ent->Knot);
  S << endl << "Weights : ";
  IGESData_DumpVals(S ,level,0, upind,ent->Weight);
  S << endl << "Control Points (Poles) : ";
  IGESData_DumpListXYZL(S ,level,0, upind, ent->Pole, ent->Location());
  S << endl << "Starting Parameter Value : " << ent->UMin();
  S << "  Ending Parameter Value : " << ent->UMax() << endl;
  S << "Unit Normal : ";
  IGESData_DumpXYZL(S,level, ent->Normal(), ent->Location());
  S << endl;
}
