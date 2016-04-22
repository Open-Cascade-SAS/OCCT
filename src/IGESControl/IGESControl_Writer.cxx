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

//cky 16.01.99 Remove couts.
//rln 28.12.98 CCI60005

#include <Bnd_Box.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <BndLib_AddSurface.hxx>
#include <BRepBndLib.hxx>
#include <BRepToIGES_BREntity.hxx>
#include <BRepToIGESBRep_Entity.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomToIGES_GeomCurve.hxx>
#include <GeomToIGES_GeomSurface.hxx>
#include <gp_XYZ.hxx>
#include <IGESControl_Controller.hxx>
#include <IGESControl_Writer.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_Protocol.hxx>
#include <IGESSelect_WorkLibrary.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Static.hxx>
#include <Message_ProgressIndicator.hxx>
#include <OSD_OpenFile.hxx>
#include <ShapeAnalysis_ShapeTolerance.hxx>
#include <Standard_Stream.hxx>
#include <Standard_Transient.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_FinderProcess.hxx>
#include <XSAlgo.hxx>
#include <XSAlgo_AlgoContainer.hxx>

#include <errno.h>
IGESControl_Writer::IGESControl_Writer ()
    :  myTP (new Transfer_FinderProcess(10000)) ,
       myIsComputed (Standard_False)
{
//  faudrait aussi (?) prendre les parametres par defaut ... ?
  IGESControl_Controller::Init();
  myEditor.Init(IGESSelect_WorkLibrary::DefineProtocol());
  myEditor.SetUnitName(Interface_Static::CVal ("write.iges.unit"));
  myEditor.ApplyUnit(); 
  myWriteMode = Interface_Static::IVal ("write.iges.brep.mode");
  myModel = myEditor.Model();
}

IGESControl_Writer::IGESControl_Writer
  (const Standard_CString unit, const Standard_Integer modecr)
    :  myTP (new Transfer_FinderProcess(10000)) ,
       myWriteMode (modecr) , myIsComputed (Standard_False)
{
//  faudrait aussi (?) prendre les parametres par defaut ... ?
  IGESControl_Controller::Init();
  myEditor.Init(IGESSelect_WorkLibrary::DefineProtocol());
  myEditor.SetUnitName(unit);
  myEditor.ApplyUnit();
  myModel = myEditor.Model();
}

IGESControl_Writer::IGESControl_Writer
  (const Handle(IGESData_IGESModel)& model, const Standard_Integer modecr)
    :  myTP (new Transfer_FinderProcess(10000)) ,
       myModel (model) , 
       myEditor (model,IGESSelect_WorkLibrary::DefineProtocol()) ,
       myWriteMode (modecr) , myIsComputed (Standard_False)     {  }

Standard_Boolean IGESControl_Writer::AddShape (const TopoDS_Shape& theShape)
{
  if (theShape.IsNull()) return Standard_False;
  
  // for progress indication
  Handle(Message_ProgressIndicator) progress = myTP->GetProgress();
  if ( ! progress.IsNull() ) { 
    Standard_Integer nbfaces=0;
    for( TopExp_Explorer exp(theShape,TopAbs_FACE); exp.More(); exp.Next() )
      nbfaces++;
    progress->SetScale ( "Faces", 0, nbfaces, 1 );
  }
  
  XSAlgo::AlgoContainer()->PrepareForTransfer();
  
  //  modified by NIZHNY-EAP Tue Aug 29 11:16:54 2000 ___BEGIN___
  Handle(Standard_Transient) info;
  Standard_Real Tol = Interface_Static::RVal("write.precision.val");
  Standard_Real maxTol = Interface_Static::RVal("read.maxprecision.val");
  TopoDS_Shape Shape = XSAlgo::AlgoContainer()->ProcessShape( theShape, Tol, maxTol, 
                                                              "write.iges.resource.name", 
                                                              "write.iges.sequence", info,
                                                              progress );
  //  modified by NIZHNY-EAP Tue Aug 29 11:17:01 2000 ___END___
  BRepToIGES_BREntity   B0;  B0.SetTransferProcess(myTP); B0.SetModel(myModel);
  BRepToIGESBRep_Entity B1;  B1.SetTransferProcess(myTP); B1.SetModel(myModel);
  Handle(IGESData_IGESEntity) ent = myWriteMode? B1.TransferShape(Shape) : B0.TransferShape(Shape);

  if(ent.IsNull())
    return Standard_False;
//  modified by NIZHNY-EAP Tue Aug 29 11:37:18 2000 ___BEGIN___
  XSAlgo::AlgoContainer()->MergeTransferInfo(myTP, info);
//  modified by NIZHNY-EAP Tue Aug 29 11:37:25 2000 ___END___
  
  //22.10.98 gka BUC60080

  Standard_Integer oldnb = myModel->NbEntities();
  Standard_Boolean aent = AddEntity (ent);
  Standard_Integer newnb = myModel->NbEntities();

  Standard_Real oldtol = myModel->GlobalSection().Resolution(), newtol;
  
  Standard_Integer tolmod = Interface_Static::IVal("write.precision.mode");
  if (tolmod == 2)
    newtol = Interface_Static::RVal("write.precision.val");
  else {
    ShapeAnalysis_ShapeTolerance stu; 
    Standard_Real Tolv = stu.Tolerance (Shape, tolmod, TopAbs_VERTEX);
    Standard_Real Tole = stu.Tolerance (Shape, tolmod, TopAbs_EDGE); 

    if (tolmod == 0 ) {   //Average
      Standard_Real Tol1 = (Tolv + Tole) / 2;
      newtol = (oldtol * oldnb + Tol1 * (newnb - oldnb)) / newnb;
    }
    else if (tolmod < 0) {//Least
      newtol = Min (Tolv, Tole);
      if (oldnb > 0) newtol = Min (oldtol, newtol);
    }
    else {                //Greatest
      newtol = Max (Tolv, Tole);
      if (oldnb > 0) newtol = Max (oldtol, newtol);
    }
  }
  
  IGESData_GlobalSection gs = myModel->GlobalSection();
  gs.SetResolution (newtol / gs.UnitValue());//rln 28.12.98 CCI60005

  //#34 22.10.98 rln BUC60081
  Bnd_Box box;
  BRepBndLib::Add (Shape, box);
  if (!(box.IsVoid() || box.IsOpenXmax() || box.IsOpenYmax() || box.IsOpenZmax() || box.IsOpenXmin() || box.IsOpenYmin() || box.IsOpenZmin())){
    Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
    box.Get (aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
    gs.MaxMaxCoords (gp_XYZ (aXmax / gs.UnitValue(),
	     		     aYmax / gs.UnitValue(),
	  		     aZmax / gs.UnitValue()));
    gs.MaxMaxCoords (gp_XYZ (aXmin / gs.UnitValue(),
 			     aYmin / gs.UnitValue(),
			     aZmin / gs.UnitValue()));
  }

  myModel->SetGlobalSection(gs);

  return aent;
}

Standard_Boolean IGESControl_Writer::AddGeom (const Handle(Standard_Transient)& geom)
{
  if (geom.IsNull() || !geom->IsKind (STANDARD_TYPE (Geom_Geometry)))
    return Standard_False;
  DeclareAndCast(Geom_Curve,Curve,geom);
  DeclareAndCast(Geom_Surface,Surf,geom);
  Handle(IGESData_IGESEntity) ent; 

//  On reconnait : Curve et Surface de Geom
//   quid de Point; Geom2d ?

//  GeomToIGES_GeomPoint GP;
  GeomToIGES_GeomCurve GC;    GC.SetModel(myModel);
  GeomToIGES_GeomSurface GS;  GS.SetModel(myModel);

  //#34 22.10.98 rln BUC60081
  IGESData_GlobalSection gs = myModel->GlobalSection();
  Bnd_Box box;

  if (!Curve.IsNull()) {
    ent = GC.TransferCurve(Curve,Curve->FirstParameter(),Curve->LastParameter());
    BndLib_Add3dCurve::Add (GeomAdaptor_Curve (Curve), 0, box); }
  else if (!Surf.IsNull()) {
    Standard_Real U1,U2,V1,V2;
    Surf->Bounds(U1,U2,V1,V2);
    ent = GS.TransferSurface(Surf,U1,U2,V1,V2);
    BndLib_AddSurface::Add (GeomAdaptor_Surface (Surf), 0, box);
  }

  Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  box.Get (aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
  gs.MaxMaxCoords (gp_XYZ (aXmax / gs.UnitValue(),
			   aYmax / gs.UnitValue(),
			   aZmax / gs.UnitValue()));
  gs.MaxMaxCoords (gp_XYZ (aXmin / gs.UnitValue(),
			   aYmin / gs.UnitValue(),
			   aZmin / gs.UnitValue()));
  myModel->SetGlobalSection(gs);
  return AddEntity (ent);
}

Standard_Boolean IGESControl_Writer::AddEntity (const Handle(IGESData_IGESEntity)& ent)
{
  if (ent.IsNull()) return Standard_False;
  myModel->AddWithRefs(ent,IGESSelect_WorkLibrary::DefineProtocol());
  myIsComputed = Standard_False;
  return Standard_True;
}

void IGESControl_Writer::ComputeModel ()
{
  if (!myIsComputed) {
    myEditor.ComputeStatus();
    myEditor.AutoCorrectModel();
    myIsComputed = Standard_True;
  }
}

Standard_Boolean IGESControl_Writer::Write
  (Standard_OStream& S, const Standard_Boolean fnes)
{
  if (!S) return Standard_False;
  ComputeModel();
  Standard_Integer nbEnt = myModel->NbEntities();
#ifdef OCCT_DEBUG
  cout<<" IGES Write : "<<nbEnt<<" ent.s"<< flush;
#endif
  if(!nbEnt)
    return Standard_False;
  IGESData_IGESWriter IW (myModel);
//  ne pas oublier le mode fnes ... a transmettre a IW
  IW.SendModel (IGESSelect_WorkLibrary::DefineProtocol());
#ifdef OCCT_DEBUG
  cout<<" ...  ecriture  ..."<<flush;
#endif
  if (fnes) IW.WriteMode() = 10;
  Standard_Boolean status = IW.Print(S);
#ifdef OCCT_DEBUG
  cout<<" ...  fichier ecrit  ..."<<endl;
#endif
  return status;
}

Standard_Boolean IGESControl_Writer::Write
  (const Standard_CString file, const Standard_Boolean fnes)
{
  ofstream fout;
  OSD_OpenStream(fout,file,ios::out);
  if (!fout) return Standard_False;
#ifdef OCCT_DEBUG
  cout<<" Ecriture fichier ("<< (fnes ? "fnes" : "IGES") <<"): "<<file<<endl;
#endif
  Standard_Boolean res = Write (fout,fnes);

  errno = 0;
  fout.close();
  res = fout.good() && res && !errno;

  return res;
}
