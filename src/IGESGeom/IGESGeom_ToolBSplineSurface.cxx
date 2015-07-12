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

#include <gp_XYZ.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESGeom_BSplineSurface.hxx>
#include <IGESGeom_ToolBSplineSurface.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <Standard_DomainError.hxx>
#include <TColgp_HArray2OfXYZ.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>

// MGE 31/07/98
//=======================================================================
//function : IGESGeom_ToolBSplineSurface
//purpose  : 
//=======================================================================
IGESGeom_ToolBSplineSurface::IGESGeom_ToolBSplineSurface ()
{
}


//=======================================================================
//function : ReadOwnParams
//purpose  : 
//=======================================================================

void IGESGeom_ToolBSplineSurface::ReadOwnParams
  (const Handle(IGESGeom_BSplineSurface)& ent,
   const Handle(IGESData_IGESReaderData)& /* IR */, IGESData_ParamReader& PR) const
{
  // MGE 31/07/98
  // Building of messages
  //========================================
  Message_Msg Msg100("XSTEP_100");
  Message_Msg Msg101("XSTEP_101");
  Message_Msg Msg102("XSTEP_102");
  Message_Msg Msg103("XSTEP_103");
  Message_Msg Msg159("XSTEP_159");
  //========================================

  Standard_Integer I, J;
  Standard_Integer anIndexU, anIndexV, aDegU, aDegV;
  Standard_Boolean aCloseU, aCloseV, aPolynom, aPeriodU, aPeriodV;
  Standard_Real aUmin, aUmax, aVmin = 0., aVmax = 0.;
  Standard_Real tempVal;
  gp_XYZ tempXYZ;
  Handle(TColStd_HArray1OfReal) allKnotsU;
  Handle(TColStd_HArray1OfReal) allKnotsV;
  Handle(TColStd_HArray2OfReal) allWeights;
  Handle(TColgp_HArray2OfXYZ)   allPoles;

  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed
  Standard_Boolean FlagindexU = PR.ReadInteger(PR.Current(), anIndexU);

  Standard_Boolean FlagindexV = PR.ReadInteger(PR.Current(), anIndexV);

  if (!FlagindexU || !FlagindexV){ 
    Message_Msg Msg97("XSTEP_97");
    PR.SendFail(Msg97);
  }

  Standard_Boolean FlagdegU  =  PR.ReadInteger(PR.Current(), aDegU);

  Standard_Boolean FlagdegV  =  PR.ReadInteger(PR.Current(), aDegV);

  if (!FlagdegU || !FlagdegV){ 
    Message_Msg Msg98("XSTEP_98");
    PR.SendFail(Msg98);
  }
  //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadBoolean(PR.Current(), Msg100,aCloseU);
  PR.ReadBoolean(PR.Current(), Msg100,aCloseV);
  PR.ReadBoolean(PR.Current(), Msg101, aPolynom);
  PR.ReadBoolean(PR.Current(), Msg102, aPeriodU);
  PR.ReadBoolean(PR.Current(), Msg102, aPeriodV);

/*
  Standard_Boolean FlagindexU =
    PR.ReadInteger(PR.Current(), "Upper Index in U", anIndexU);

  Standard_Boolean FlagindexV =
    PR.ReadInteger(PR.Current(), "Upper Index in V", anIndexV);

  Standard_Boolean FlagdegU  =
    PR.ReadInteger(PR.Current(), "Degree Of First Basis Functions", aDegU);

  Standard_Boolean FlagdegV  =
    PR.ReadInteger(PR.Current(), "Degree Of Second Basis Functions",aDegV);

  st = PR.ReadBoolean(PR.Current(), "Closed/Open flag in U Direction",aCloseU);
  st = PR.ReadBoolean(PR.Current(), "Closed/Open flag in V Direction",aCloseV);
  st = PR.ReadBoolean(PR.Current(), "Polynomial / Rational", aPolynom);
  st = PR.ReadBoolean(PR.Current(), "Periodic flag in U direction", aPeriodU);
  st = PR.ReadBoolean(PR.Current(), "Periodic flag in V direction", aPeriodV);
*/
  if ( FlagdegU && FlagindexU )
    {
//      allKnotsU = new TColStd_HArray1OfReal(-aDegU, anIndexU+1);  done by :
      Standard_Integer tempind = anIndexU+aDegU+2;
      PR.ReadReals(PR.CurrentList(tempind), Msg103, allKnotsU, -aDegU); //szv#4:S4163:12Mar99 `st=` not needed
      //st = PR.ReadReals(PR.CurrentList(tempind), "First knot sequence values", allKnotsU, -aDegU);
    }

  if ( FlagdegV && FlagindexV )
    {
//      allKnotsV = new TColStd_HArray1OfReal(-aDegV, anIndexV+1);  done by :
      Standard_Integer tempind = anIndexV+aDegV+2;
      PR.ReadReals(PR.CurrentList(tempind), Msg103, allKnotsV, -aDegV); //szv#4:S4163:12Mar99 `st=` not needed
      //st = PR.ReadReals(PR.CurrentList(tempind), "Second knot sequence values", allKnotsV, -aDegV);
    }

  if ( FlagindexU && FlagindexV )
    {
      allWeights = new TColStd_HArray2OfReal (0, anIndexU, 0, anIndexV);
      allPoles   = new TColgp_HArray2OfXYZ   (0, anIndexU, 0, anIndexV);

      Standard_Boolean BadWeigth = Standard_False;
      Message_Msg Msg105("XSTEP_105");

      for (J = 0; J <= anIndexV; J ++) {
	for (I = 0; I <= anIndexU; I ++) {
          //st = PR.ReadReal(PR.Current(), Msg104, tempVal); //szv#4:S4163:12Mar99 moved down
          //st = PR.ReadReal(PR.Current(), "Weights", tempVal);
          if (PR.ReadReal(PR.Current(), tempVal)) {
            if(tempVal<Precision::PConfusion()) { // skl for OCC2821 11.06.2003
              BadWeigth = Standard_True;
            }
            allWeights->SetValue(I, J, tempVal);
          }
	  else{
	    Message_Msg Msg104("XSTEP_104");
	    PR.SendFail(Msg104);
	  }
        }
      }
      if(BadWeigth) { // skl for OCC2821 11.06.2003
        for (J = 0; J <= anIndexV; J ++) {
          for (I = 0; I <= anIndexU; I ++) {
            allWeights->SetValue(I, J, 1.);
          }
        }
      }

      for (J = 0; J <= anIndexV; J ++)
	for (I = 0; I <= anIndexU; I ++) {
	  //st = PR.ReadXYZ (PR.CurrentList(1, 3), Msg105, tempXYZ); //szv#4:S4163:12Mar99 moved down
	  //st = PR.ReadXYZ (PR.CurrentList(1, 3), "Control Points", tempXYZ);
	  if (PR.ReadXYZ (PR.CurrentList(1, 3), Msg105, tempXYZ))
	    allPoles->SetValue(I, J, tempXYZ);
	}
    }

  //szv#4:S4163:12Mar99 `st=` not needed
 /* PR.ReadReal(PR.Current(), Msg106, aUmin);
  PR.ReadReal(PR.Current(), Msg107, aUmax);
  PR.ReadReal(PR.Current(), Msg106, aVmin);
  PR.ReadReal(PR.Current(), Msg107, aVmax);
*/
  if (!PR.ReadReal(PR.Current(), aUmin) || !PR.ReadReal(PR.Current(), aVmin)){
    Message_Msg Msg106("XSTEP_106");
    PR.SendFail(Msg106);
  }

  if (!PR.ReadReal(PR.Current(), aUmax) || !PR.ReadReal(PR.Current(), aVmax)){
    Message_Msg Msg107("XSTEP_107");
    PR.SendFail(Msg107);
  }
/*
  st = PR.ReadReal(PR.Current(), "Starting Value For U Direction", aUmin);
  st = PR.ReadReal(PR.Current(), "Ending Value For U Direction", aUmax);
  st = PR.ReadReal(PR.Current(), "Starting Value For V Direction", aVmin);
  st = PR.ReadReal(PR.Current(), "Ending Value For U Direction", aVmax);
*/
//  PROTECTION contre ANSYS 5.3 qui ecrit 3 flottants en plus ...
  Standard_Integer icur = PR.CurrentNumber(), imax = PR.NbParams();
  Standard_Real bid; Standard_Integer pbfin = 0;
  //st = Standard_True; //szv#4:S4163:12Mar99 not needed
  while (imax >= icur) {
    Interface_ParamType pt = PR.ParamType(icur);
    if (pt == Interface_ParamReal) {
      if (!PR.ReadReal (PR.Current(),bid)){
	PR.SendFail(Msg159);
      } //szv#4:S4163:12Mar99 `st=` not needed
      if (pbfin == 0) pbfin = 1;
    }
    else if (pt == Interface_ParamInteger || pt == Interface_ParamVoid) break;
    else pbfin = -1;
    icur ++;
  }
  if (pbfin > 0) PR.SendWarning(Msg159);
  if (pbfin < 0){
    Message_Msg Msg158("XSTEP_158");
    PR.SendFail(Msg158);
  }
  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init
    (anIndexU, anIndexV, aDegU, aDegV, aCloseU, aCloseV,
     aPolynom, aPeriodU, aPeriodV, allKnotsU, allKnotsV,
     allWeights, allPoles, aUmin, aUmax, aVmin, aVmax);
}


//=======================================================================
//function : WriteOwnParams
//purpose  : 
//=======================================================================

void IGESGeom_ToolBSplineSurface::WriteOwnParams
  (const Handle(IGESGeom_BSplineSurface)& ent, IGESData_IGESWriter& IW)  const
{
  Standard_Integer indU = ent->UpperIndexU();
  Standard_Integer indV = ent->UpperIndexV();
  Standard_Integer degU = ent->DegreeU();
  Standard_Integer degV = ent->DegreeV();
  Standard_Integer I, J;
  IW.Send(indU);
  IW.Send(indV);
  IW.Send(degU);
  IW.Send(degV);
  IW.SendBoolean(ent->IsClosedU());
  IW.SendBoolean(ent->IsClosedV());
  IW.SendBoolean(ent->IsPolynomial());
  IW.SendBoolean(ent->IsPeriodicU());
  IW.SendBoolean(ent->IsPeriodicV());

  for (I = -degU; I <= indU + 1; I ++)
    IW.Send(ent->KnotU(I));

  for (I = -degV; I <= indV + 1; I ++)
    IW.Send(ent->KnotV(I));

  for (J = 0; J <= indV; J ++)
    for (I = 0; I <= indU; I ++)
      IW.Send(ent->Weight(I, J));

  for (J = 0; J <= indV; J ++)
    for (I = 0; I <= indU; I ++)  {
      gp_XYZ tempXYZ = ent->Pole(I, J).XYZ();
      IW.Send(tempXYZ.X());
      IW.Send(tempXYZ.Y());
      IW.Send(tempXYZ.Z());
    }
  IW.Send(ent->UMin());
  IW.Send(ent->UMax());
  IW.Send(ent->VMin());
  IW.Send(ent->VMax());
}


//=======================================================================
//function : OwnShared
//purpose  : 
//=======================================================================

void IGESGeom_ToolBSplineSurface::OwnShared(const Handle(IGESGeom_BSplineSurface)& /* ent */,
                                            Interface_EntityIterator& /* iter */) const
{
}


//=======================================================================
//function : OwnCopy
//purpose  : 
//=======================================================================

void IGESGeom_ToolBSplineSurface::OwnCopy(const Handle(IGESGeom_BSplineSurface)& another,
                                          const Handle(IGESGeom_BSplineSurface)& ent,
                                          Interface_CopyTool& /* TC */) const
{
  Standard_Integer I, J;
  Standard_Integer anIndexU, anIndexV, aDegU, aDegV;
  Standard_Boolean aCloseU, aCloseV, aPolynom;
  Standard_Boolean aPeriodU, aPeriodV;
  Standard_Real aUmin, aUmax, aVmin, aVmax;

  anIndexU = another->UpperIndexU();
  anIndexV = another->UpperIndexV();
  aDegU    = another->DegreeU();
  aDegV    = another->DegreeV();
  aCloseU  = another->IsClosedU();
  aCloseV  = another->IsClosedV();
  aPolynom = another->IsPolynomial();
  aPeriodU = another->IsPeriodicU();
  aPeriodV = another->IsPeriodicV();

  Handle(TColStd_HArray1OfReal) allKnotsU  =
    new TColStd_HArray1OfReal(-aDegU, anIndexU+1);
  Handle(TColStd_HArray1OfReal) allKnotsV  =
    new TColStd_HArray1OfReal(-aDegV, anIndexV+1);

  for (I = -aDegU; I <= anIndexU + 1; I ++)
    allKnotsU->SetValue(I, another->KnotU(I));

  for (I = -aDegV; I <= anIndexV + 1; I ++)
    allKnotsV->SetValue(I, another->KnotV(I));

  Handle(TColStd_HArray2OfReal) allWeights  =
    new TColStd_HArray2OfReal (0, anIndexU, 0, anIndexV);
  Handle(TColgp_HArray2OfXYZ)   allPoles    =
    new TColgp_HArray2OfXYZ   (0, anIndexU, 0, anIndexV);

  for (J = 0; J <= anIndexV; J ++)
    for (I = 0; I <= anIndexU; I ++) 
      allWeights->SetValue(I, J, another->Weight(I,J));

  for (J = 0; J <= anIndexV; J ++)
    for (I = 0; I <= anIndexU; I ++) 
      allPoles->SetValue(I, J, another->Pole(I, J).XYZ());

  aUmin = another->UMin();
  aUmax = another->UMax();
  aVmin = another->VMin();
  aVmax = another->VMax();

  ent->Init (anIndexU, anIndexV, aDegU, aDegV, aCloseU, aCloseV,
	     aPolynom, aPeriodU, aPeriodV, allKnotsU, allKnotsV,
	     allWeights, allPoles, aUmin, aUmax, aVmin, aVmax);
  ent->SetFormNumber(another->FormNumber());
}


//=======================================================================
//function : DirChecker
//purpose  : 
//=======================================================================

IGESData_DirChecker IGESGeom_ToolBSplineSurface::DirChecker
  (const Handle(IGESGeom_BSplineSurface)& /* ent */ )   const
{
  IGESData_DirChecker DC(128, 0, 9);
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

void IGESGeom_ToolBSplineSurface::OwnCheck(const Handle(IGESGeom_BSplineSurface)& ent,
                                           const Interface_ShareTool&,
                                           Handle(Interface_Check)& ach)  const
{
  // MGE 31/07/98
  // Building of messages
  //========================================
  //Message_Msg Msg104("XSTEP_104");
  //Message_Msg Msg160("XSTEP_160");
  //Message_Msg Msg161("XSTEP_161");
  //========================================



  //Standard_Real eps = 1.E-04;    // Tolerance des tests ?? //szv#4:S4163:12Mar99 not needed

  //modified by rln 18/12/97 check of flag PROP2 according to IGES Standard
  //The same as in IGESGeom_ToolBSplineCurve::OwnCheck()
  //It is possible to compare U(0) and U(1) only if UStartingParameter = UFirstKnot
  //and UEndingParameter = ULastKnot and the same for V(0),V(1) 
  //(else we must build real geometrical curve)
  //The fail is replaced with warning because it is not a serious problem
/*
  if (ent->UMin() == ent->KnotU(-ent->DegreeU()        ) &&
      ent->UMax() == ent->KnotU( ent->UpperIndexU() + 1)   ) {
    Standard_Real udif = Max (ent->Pole(0, 0).SquareDistance (ent->Pole(ent->UpperIndexU(), 0)),
                              ent->Pole(0, ent->UpperIndexV()).SquareDistance (ent->Pole(ent->UpperIndexU(), ent->UpperIndexV())));
//  if ((ent->UMin() == ent->UMax()) && !ent->IsClosedU())
//    if (udif < eps * eps && !ent->IsClosedU())
//      ach.AddWarning("U(0) == U(1) & PROP1 != 1");
//  if ((ent->UMin() != ent->UMax()) &&  ent->IsClosedU())
//    if (udif >= eps * eps && ent->IsClosedU())
//       ach.AddWarning("U(0) != U(1) & PROP1 != 0");
  }

  if (ent->VMin() == ent->KnotV(-ent->DegreeV()        ) &&
      ent->VMax() == ent->KnotV( ent->UpperIndexV() + 1)   ) {
    Standard_Real vdif = Max (ent->Pole(0, 0).SquareDistance (ent->Pole(0, ent->UpperIndexV())),
                              ent->Pole(ent->UpperIndexU(), 0).SquareDistance (ent->Pole(ent->UpperIndexU(), ent->UpperIndexV())));
//  if ((ent->VMin() == ent->VMax()) && !ent->IsClosedV())
//    if (vdif < eps * eps && !ent->IsClosedV())
//      ach.AddWarning("V(0) == V(1) & PROP2 != 1");
//  if ((ent->VMin() != ent->VMax()) && ent->IsClosedV())
//    if (vdif >= eps * eps && ent->IsClosedV())
//      ach.AddWarning("V(0) != V(1) & PROP2 != 0");
  }
*/

  Standard_Boolean Flag = Standard_True;
  Standard_Integer indU = ent->UpperIndexU();
  Standard_Integer indV = ent->UpperIndexV();
  Standard_Integer I,J;

  if (ent->NbKnotsU() != (indU + ent->DegreeU() +2)) {
    Message_Msg Msg160("XSTEP_160");
    Msg160.Arg(indU + ent->DegreeU() +2);
    ach->SendFail(Msg160);
  }
  if (ent->NbKnotsV() != (indV + ent->DegreeV() +2)) {
    Message_Msg Msg161("XSTEP_161");
    Msg161.Arg(indV + ent->DegreeV() +2);
    ach->SendFail(Msg161);
  }

  for ( J = 0; ((J < indV) && (Flag)); J ++)
    for ( I = 0; ((I < indU) && (Flag)); I ++)
      Flag &= (ent->Weight(I,J) > 0);

  if (!Flag) {
    Message_Msg Msg104("XSTEP_104");
    ach->SendFail(Msg104);
  }

  Flag = Standard_True;
  Standard_Real tempVal = ent->Weight(0,0);

  for ( J = 0; ((J < indV) && (Flag)); J ++)
    for ( I = 0; ((I < indU) && (Flag)); I ++)
      Flag &= (ent->Weight(I,J) == tempVal);

//  Standard_Boolean Flap = ent->IsPolynomial(Standard_True);
//  if (Flag && !Flap)
//    ach.AddWarning("All weights equal & PROP3 != 1. (Surface not Polynomial)");
//  if (!Flag && Flap)
//    ach.AddWarning("All weights not equal & PROP3 != 0 (Surface not Rational)");
}


//=======================================================================
//function : OwnDump
//purpose  : 
//=======================================================================

void IGESGeom_ToolBSplineSurface::OwnDump(const Handle(IGESGeom_BSplineSurface)& ent,
                                          const IGESData_IGESDumper& /* dumper */,
                                          const Handle(Message_Messenger)& S,
                                          const Standard_Integer level)  const
{
  S << "BSplineSurface from IGESGeom" << endl << endl;

  Standard_Integer indU = ent->UpperIndexU();
  Standard_Integer indV = ent->UpperIndexV();
  Standard_Integer degU = ent->DegreeU();
  Standard_Integer degV = ent->DegreeV();

  S << "In U : Upper Knot Index : " << indU;
  S << "   Degree : " << degU << "  ";
  S << (ent->IsClosedU() ? "Closed" : "Open") << "  ";
  S << (ent->IsPeriodicU() ? "Periodic" : "Non Periodic");
  S << endl;
  S << "In V : Upper Knot Index : " << indV;
  S << "   Degree : " << degV << "  ";
  S << (ent->IsClosedV() ? "Closed" : "Open") << "  ";
  S << (ent->IsPeriodicV() ? "Periodic" : "Non Periodic") << "  ";
  S << (ent->IsPolynomial(Standard_True) ? "Polynomial" : "Rational") << endl;

  S << "Starting Parameter for U direction : " << ent->UMin() << endl;
  S << "Ending   Parameter for U direction : " << ent->UMax() << endl;
  S << "Starting Parameter for V direction : " << ent->VMin() << endl;
  S << "Ending   Parameter for V direction : " << ent->VMax() << endl;

  S << " Knots in  U : ";
  IGESData_DumpVals(S ,level,-degU, indU+1,ent->KnotU);
  S << endl;
  S << " Knots in  V : ";
  IGESData_DumpVals(S ,level,-degV, indV+1,ent->KnotV);
  S << endl;
  S << "Weights : (Row -> IndexU, Col -> IndexV)";
  IGESData_DumpRectVals(S ,level,0, indU,0,indV,ent->Weight);
  S << endl;
  S << "Control Points (Poles) : ";
//IGESData_DumpRectXYZL(S,level,1, ent->NbPoles(),ent->Pole, ent->Location());
  if (level < 5) {  S << " [ content : ask level > 4 ]" << endl; return;  }
  gp_GTrsf loca = ent->Location();
  for (Standard_Integer JP = 0; JP <= indV; JP ++)
    for (Standard_Integer IP = 0; IP <= indU; IP ++) {
      if (IP == 0) S << endl;
      S <<"  -  ["<<IP<<","<<JP<<"]: ";
      IGESData_DumpXYZL(S,level,ent->Pole(IP,JP),loca);
    }
  S << endl;
}
