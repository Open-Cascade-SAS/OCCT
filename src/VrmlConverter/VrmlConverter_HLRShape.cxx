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


#include <Bnd_Box.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBndLib.hxx>
#include <Precision.hxx>
#include <StdPrs_HLRToolShape.hxx>
#include <TopoDS_Shape.hxx>
#include <Vrml_Material.hxx>
#include <Vrml_Separator.hxx>
#include <VrmlConverter_DeflectionCurve.hxx>
#include <VrmlConverter_Drawer.hxx>
#include <VrmlConverter_HLRShape.hxx>
#include <VrmlConverter_LineAspect.hxx>
#include <VrmlConverter_Projector.hxx>

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
void VrmlConverter_HLRShape::Add(Standard_OStream&                          anOStream,
				 const TopoDS_Shape&                        aShape,
				 const Handle (VrmlConverter_Drawer)&       aDrawer,
				 const Handle (VrmlConverter_Projector)&    aProjector)
{
  StdPrs_HLRToolShape Tool(aShape,aProjector->Projector());

  Standard_Integer NbEdge = Tool.NbEdges();
  Standard_Integer i;
  Standard_Real U1,U2;
  BRepAdaptor_Curve TheCurve;

  Standard_Real theRequestedDeflection;
  if(aDrawer->TypeOfDeflection() == Aspect_TOD_RELATIVE)   // TOD_RELATIVE, TOD_ABSOLUTE
    {
      Bnd_Box box;
      BRepBndLib::AddClose(aShape, box);
      
      Standard_Real  Xmin, Xmax, Ymin, Ymax, Zmin, Zmax, diagonal;
      box.Get( Xmin, Ymin, Zmin, Xmax, Ymax, Zmax );
      if (!(box.IsOpenXmin() || box.IsOpenXmax() ||
	    box.IsOpenYmin() || box.IsOpenYmax() ||
	    box.IsOpenZmin() || box.IsOpenZmax()))
	{
	  
	  diagonal = Sqrt ((Xmax - Xmin)*( Xmax - Xmin) + ( Ymax - Ymin)*( Ymax - Ymin) + ( Zmax - Zmin)*( Zmax - Zmin));
	  diagonal = Max(diagonal, Precision::Confusion());
	  theRequestedDeflection = aDrawer->DeviationCoefficient() * diagonal;      
	}
      else
	{
	  diagonal =1000000.;
	  theRequestedDeflection = aDrawer->DeviationCoefficient() * diagonal;  
	}
      
    }
  else 
    {
      theRequestedDeflection = aDrawer->MaximalChordialDeviation(); 
    }
  

  Handle(VrmlConverter_LineAspect) latmp = new VrmlConverter_LineAspect; 
  latmp->SetMaterial(aDrawer->LineAspect()->Material());
  latmp->SetHasMaterial(aDrawer->LineAspect()->HasMaterial());

  Handle(VrmlConverter_LineAspect) laHL = new VrmlConverter_LineAspect;
  Handle(VrmlConverter_LineAspect) laSL = new VrmlConverter_LineAspect; 

  laHL = aDrawer->HiddenLineAspect();
  laSL = aDrawer->SeenLineAspect();

  Vrml_Separator SE1;
  Vrml_Separator SE2;
  Vrml_Separator SE3;

  Standard_Boolean flag = Standard_False; // to check a call of Vrml_Separator.Print(anOStream)
  
  SE1.Print(anOStream);

// Adds  (if  they  are  defined) Camera or/and Light and MatrixTransform.
  aProjector->Add(anOStream);

  if (laSL->HasMaterial()){
	
    Handle(Vrml_Material) MSL;
    MSL = laSL->Material();
	
    MSL->Print(anOStream);
    laSL->SetHasMaterial(Standard_False);

    flag = Standard_True;
    // Separator 2 {
    SE2.Print(anOStream);
  }
  aDrawer->SetLineAspect(laSL);

//  aDrawer->SetLineAspect(aDrawer->SeenLineAspect());
  for (i=1;i<=NbEdge;i++){
    for(Tool.InitVisible(i);Tool.MoreVisible();Tool.NextVisible()){
      Tool.Visible(TheCurve,U1,U2);
      VrmlConverter_DeflectionCurve::Add(anOStream, TheCurve, U1, U2, theRequestedDeflection);
//	VrmlConverter_DeflectionCurve::Add(anOStream, TheCurve, U1, U2, aDrawer);
    }
  }

      if (flag){
    // Separator 2 }
	SE2.Print(anOStream);
	flag = Standard_False;
      }

  if(aDrawer->DrawHiddenLine()){

    if (laHL->HasMaterial()){
	
      Handle(Vrml_Material) MHL;
      MHL = laHL->Material();
	
      MHL->Print(anOStream);
      laHL->SetHasMaterial(Standard_False);

      flag = Standard_True;
      // Separator 3 {
      SE3.Print(anOStream);
    }
    aDrawer->SetLineAspect(laHL);
//    aDrawer->SetLineAspect(aDrawer->HiddenLineAspect());
    
    for (i=1;i<=NbEdge;i++){
      for(Tool.InitHidden(i);Tool.MoreHidden();Tool.NextHidden()){
	Tool.Hidden(TheCurve,U1,U2);
	VrmlConverter_DeflectionCurve::Add(anOStream, TheCurve, U1, U2, theRequestedDeflection);
//	VrmlConverter_DeflectionCurve::Add(anOStream, TheCurve, U1, U2, aDrawer);
      }
    }
    if (flag){
      // Separator 3 }
      SE3.Print(anOStream);
      flag = Standard_False;
    }
  }

  // Separator 1 }
  SE1.Print(anOStream);
  
  aDrawer->SetLineAspect(latmp);
}   
