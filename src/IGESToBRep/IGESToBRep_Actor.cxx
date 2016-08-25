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


#include <BRepLib.hxx>
#include <IGESData_GlobalSection.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESToBRep.hxx>
#include <IGESToBRep_Actor.hxx>
#include <IGESToBRep_CurveAndSurface.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Static.hxx>
#include <Message_ProgressSentry.hxx>
#include <ShapeExtend_Explorer.hxx>
#include <ShapeFix_ShapeTolerance.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TransferBRep.hxx>
#include <TransferBRep_ShapeBinder.hxx>
#include <XSAlgo.hxx>
#include <XSAlgo_AlgoContainer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESToBRep_Actor,Transfer_ActorOfTransientProcess)

//=======================================================================
//function : IGESToBRep_Actor
//purpose  : 
//=======================================================================
IGESToBRep_Actor::IGESToBRep_Actor ()
{  thecontinuity = 0;  theeps = 0.0001;  }


//=======================================================================
//function : SetModel
//purpose  : 
//=======================================================================
void IGESToBRep_Actor::SetModel (const Handle(Interface_InterfaceModel)& model)
{
  themodel = model;
  theeps = Handle(IGESData_IGESModel)::DownCast(themodel)->GlobalSection().Resolution();
}


//=======================================================================
//function : SetContinuity
//purpose  : 
//=======================================================================
void  IGESToBRep_Actor::SetContinuity (const Standard_Integer continuity)
{
  thecontinuity = continuity;
}


//=======================================================================
//function : GetContinuity
//purpose  : 
//=======================================================================
Standard_Integer  IGESToBRep_Actor::GetContinuity () const
{
  return thecontinuity;
}

//=======================================================================
//function : Recognize
//purpose  : 
//=======================================================================
Standard_Boolean  IGESToBRep_Actor::Recognize
(const Handle(Standard_Transient)& start)
{
  DeclareAndCast(IGESData_IGESModel,mymodel,themodel);
  DeclareAndCast(IGESData_IGESEntity,ent,start);
  if (ent.IsNull()) return Standard_False;

//   Cas reconnus
  Standard_Integer typnum = ent->TypeNumber();
  Standard_Integer fornum = ent->FormNumber();
  if (IGESToBRep::IsCurveAndSurface(ent) ||
      ((typnum == 402 && (fornum == 1 || fornum == 7 || 
                         fornum == 14 || fornum == 15))  ||
       (typnum == 408 ) || (typnum == 308)))
    return Standard_True;

//  Cas restants : non reconnus
  return Standard_False;
}
//=======================================================================
//function : EncodeRegul
//purpose  : INTERNAL to encode regularity on edges
//=======================================================================

static Standard_Boolean  EncodeRegul (const TopoDS_Shape& sh)
{
  Standard_Real tolang = Interface_Static::RVal("read.encoderegularity.angle");
  if (sh.IsNull()) return Standard_True;
  if (tolang <= 0) return Standard_True;
  try {
    OCC_CATCH_SIGNALS
    BRepLib::EncodeRegularity (sh,tolang);
  }
  catch(Standard_Failure) {
    return Standard_False;
  }
  return Standard_True;
}
//=======================================================================
//function : TrimTolerances
//purpose  : Trims tolerances of the shape according to static parameters
//          
//=======================================================================

static void TrimTolerances (const TopoDS_Shape& shape,
			    const Standard_Real tol)
{
  if( Interface_Static::IVal("read.maxprecision.mode")==1) {
    ShapeFix_ShapeTolerance SFST;
    SFST.LimitTolerance (shape, 0, Max(tol,Interface_Static::RVal ("read.maxprecision.val")));
  }
}



//=======================================================================
//function : Transfer
//purpose  : 
//=======================================================================
Handle(Transfer_Binder) IGESToBRep_Actor::Transfer
(const Handle(Standard_Transient)& start, const Handle(Transfer_TransientProcess)& TP)
{
  DeclareAndCast(IGESData_IGESModel,mymodel,themodel);
  DeclareAndCast(IGESData_IGESEntity,ent,start);
  if (mymodel.IsNull() || ent.IsNull()) return NullResult();
  Standard_Integer anum = mymodel->Number(start);
  
  if (Interface_Static::IVal("read.iges.faulty.entities") == 0 && mymodel->IsErrorEntity(anum)) 
    return NullResult();
  TopoDS_Shape shape;

//   appeler le transfert seulement si type OK
  Standard_Integer typnum = ent->TypeNumber();
  Standard_Integer fornum = ent->FormNumber();
  Standard_Real eps;
  if (IGESToBRep::IsCurveAndSurface(ent) ||
      (typnum == 402 && (fornum == 1 || fornum == 7|| 
                         fornum == 14 || fornum == 15)) || 
      (typnum == 408) || (typnum == 308)) {

    // Start progress scope (no need to check if progress exists -- it is safe)
    Message_ProgressSentry aPSentry(TP->GetProgress(), "Transfer stage", 0, 2, 1);

    XSAlgo::AlgoContainer()->PrepareForTransfer();
    IGESToBRep_CurveAndSurface CAS;
    CAS.SetModel(mymodel);
    CAS.SetContinuity(thecontinuity);
    CAS.SetTransferProcess(TP);
    Standard_Integer Ival = Interface_Static::IVal("read.precision.mode");
    if ( Ival == 0)
      eps = mymodel->GlobalSection().Resolution();
    else
      eps = Interface_Static::RVal("read.precision.val"); //:10 ABV 11 Nov 97
//:10      eps = BRepAPI::Precision(); 
    Ival = Interface_Static::IVal("read.iges.bspline.approxd1.mode");
    CAS.SetModeApprox ( (Ival > 0) );
    Ival = Interface_Static::IVal("read.surfacecurve.mode");
    CAS.SetSurfaceCurve (Ival);

    if (eps > 1.E-08) {
      CAS.SetEpsGeom(eps);
      theeps = eps*CAS.GetUnitFactor();
//      Interface_Static::SetRVal("lastpreci",theeps);
    }
    Standard_Integer nbTPitems = TP->NbMapped();
    {
      try {
        OCC_CATCH_SIGNALS
	shape = CAS.TransferGeometry(ent);
      }
      catch(Standard_Failure) {
	shape.Nullify();
      }
    }

    // Switch to fix stage.
    aPSentry.Next();
    
    // fixing shape
    Handle(Standard_Transient) info;
    shape = XSAlgo::AlgoContainer()->ProcessShape( shape, theeps, CAS.GetMaxTol(), 
                                                   "read.iges.resource.name", 
                                                   "read.iges.sequence", info,
                                                   TP->GetProgress() );
    XSAlgo::AlgoContainer()->MergeTransferInfo(TP, info, nbTPitems);
  }

  ShapeExtend_Explorer SBE;
  if (SBE.ShapeType(shape,Standard_True) != TopAbs_SHAPE) {
    if (!shape.IsNull()) {
      EncodeRegul (shape);
      //#74 rln 03.03.99 S4135
      TrimTolerances (shape, UsedTolerance());
   //   Shapes().Append(shape);
    }
  }
  Handle(TransferBRep_ShapeBinder) binder;
  if (!shape.IsNull()) binder = new TransferBRep_ShapeBinder(shape);
  return binder;
}


    Standard_Real  IGESToBRep_Actor::UsedTolerance () const
      {  return theeps;  }
