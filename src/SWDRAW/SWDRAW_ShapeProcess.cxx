// File:	SWDRAW_ShapeProcess.cxx
// Created:	Sat Jun 19 15:49:27 1999
// Author:	Andrey BETENEV
//		<abv@nnov.matra-dtv.fr>

#include <SWDRAW_ShapeProcess.ixx>
#include <SWDRAW.hxx>

#include <DBRep.hxx>
#include <Draw_Interpretor.hxx>
#include <TopoDS_Shape.hxx>

#include <ShapeProcess.hxx>
#include <ShapeProcess_ShapeContext.hxx>
#include <ShapeProcess_OperLibrary.hxx>

//=======================================================================
//function : ApplySequence
//purpose  : 
//=======================================================================

static Standard_Integer ApplySequence(Draw_Interpretor& di,
				      Standard_Integer argc, const char** argv)
{
  if (argc < 4) { di<<"Specify result, shape, resource name and prefix (optional)"<<"\n"; return 1 /* Error */; }
  Standard_CString arg1 = argv[1];
  Standard_CString arg2 = argv[2];
  Standard_CString arg3 = argv[3];
  Standard_CString arg4 = "";
  if (argc > 4) arg4 = argv[4];
  TopoDS_Shape Shape = DBRep::Get(arg2);
  if (Shape.IsNull()) { di<<"Shape unknown : "<<arg2<<"\n"; return 1 /* Error */; }

  Handle(ShapeProcess_ShapeContext) context = new ShapeProcess_ShapeContext (Shape, arg3);
  ShapeProcess::Perform ( context, arg4 );
  context->PrintStatistics();
  
  TopoDS_Shape result = context->Result();
//  seq.PrintPreparationResult ();
  if ( result.IsNull() ) {
    di << "No result" << "\n";
  }
  else if ( result == Shape ) {
    di << "Shape not modified" << "\n";
  }
  DBRep::Set (arg1,result);
  return 0;
}
  
//=======================================================================
//function : InitCommands
//purpose  : 
//=======================================================================

void SWDRAW_ShapeProcess::InitCommands(Draw_Interpretor& theCommands) 
{
  static Standard_Boolean initactor = 0;
  if (initactor) return;  initactor = 1;
  
  ShapeProcess_OperLibrary::Init();
  
  Standard_CString g = SWDRAW::GroupName(); // "Tests of DivideTool";
 
  theCommands.Add ("SPApply","SPApply result shape rscfilename [sequence]",
		   __FILE__,ApplySequence,g);
}  
