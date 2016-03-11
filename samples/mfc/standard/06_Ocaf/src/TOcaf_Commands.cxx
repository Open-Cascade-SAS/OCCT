// File:	TOcaf_Commands.cxx
// Created:	Tue Feb  8 17:56:02 2000
// Author:	Modelistation
//		<model@brunox.paris1.matra-dtv.fr>


#include <stdafx.h>
#include <TOcaf_Commands.hxx>
#include <TDF_Label.hxx>
#include <TDF_TagSource.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataStd_ChildNodeIterator.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <gp_Trsf.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TNaming_Tool.hxx>
#include <BRep_Tool.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <Geom_SphericalSurface.hxx>
#include <AIS_InteractiveContext.hxx>
#include <TPrsStd_AISViewer.hxx>
#include "TDataStd_Name.hxx"
#include "TFunction_Function.hxx"
#include "TOcafFunction_BoxDriver.hxx"
#include "TFunction_DriverTable.hxx"
#include "TOcafFunction_CylDriver.hxx"
#include "TDF_Reference.hxx"
#include "TOcafFunction_CutDriver.hxx"

//=======================================================================
//function : TOcaf_Commands
//purpose  : 
//=======================================================================

TOcaf_Commands::TOcaf_Commands(const TDF_Label& Main) 
{
  MainLab = Main;
}

//=======================================================================
//function :	CreateBox
//purpose  :	Create a box containing a Named shape, a name, a Function 
//				and a Data structure containing box conctruction parameters
//=======================================================================

TDF_Label TOcaf_Commands::CreateBox(Standard_Real x, Standard_Real y, Standard_Real z, Standard_Real w, Standard_Real l, Standard_Real h, const TCollection_ExtendedString& Name)
{
  // A data structure for our box:
  // the box itself is attached to the BoxLabel label (as his name and his function attribute) 
  // its arguments (dimensions: width, length and height; and position: x, y, z) 
  // are attached to the child labels of the box:
  //
  // 0:1 Box Label ---> Name --->  Named shape ---> Function
  //       |
  //     0:1:1 -- Width Label
  //       |
  //     0:1:2 -- Length Label
  //       |
  //     0:1:3 -- Height Label
  //       |
  //     0:1:4 -- X Label
  //       |
  //     0:1:5 -- Y Label
  //       |
  //     0:1:6 -- Z Label

	// Create a new label in the data structure for the box
    TDF_Label L = TDF_TagSource::NewChild(MainLab);

	// Create the data structure : Set the dimensions, position and name attributes
	TDataStd_Real::Set(L.FindChild(1), w);
	TDataStd_Real::Set(L.FindChild(2), l);
	TDataStd_Real::Set(L.FindChild(3), h);
	TDataStd_Real::Set(L.FindChild(4), x);
	TDataStd_Real::Set(L.FindChild(5), y);
	TDataStd_Real::Set(L.FindChild(6), z);
	TDataStd_Name::Set(L, Name);

	
	// Instanciate a TFunction_Function attribute connected to the current box driver
	// and attach it to the data structure as an attribute of the Box Label
	Handle(TFunction_Function) myFunction = TFunction_Function::Set(L, TOcafFunction_BoxDriver::GetID());

	// Initialize and execute the box driver (look at the "Execute()" code)
    Handle(TFunction_Logbook) log = TFunction_Logbook::Set(L);

	Handle(TFunction_Driver) myBoxDriver;
    // Find the TOcafFunction_BoxDriver in the TFunction_DriverTable using its GUID
	if(!TFunction_DriverTable::Get()->FindDriver(TOcafFunction_BoxDriver::GetID(), myBoxDriver)) return L;
	
	myBoxDriver->Init(L);
    if (myBoxDriver->Execute(log)) MessageBoxW (NULL, L"DFunction_Execute : failed", L"Box", MB_ICONERROR);

	return L;
}


//=======================================================================
//function :	CreateCyl
//purpose  :	Create a cylinder containing a Named shape, a name, a Function 
//				and a Data structure containing cylinder conctruction parameters
//=======================================================================

TDF_Label TOcaf_Commands::CreateCyl(Standard_Real x, Standard_Real y, Standard_Real z, Standard_Real r, Standard_Real h, const TCollection_ExtendedString& Name)
{
  // A data structure for our cylinder:
  // the cylinder itself is attached to the CylinderLabel label (as his name and his function attribute) 
  // its arguments (dimensions: radius and height; and position: x, y, z) 
  // are attached to the child labels of the cylinder:
  //
  // 0:1 Cylinder Label ---> Name --->  Named shape ---> Function
  //       |
  //     0:1:1 -- Radius Label
  //       |
  //     0:1:2 -- Height Label
  //       |
  //     0:1:3 -- X Label
  //       |
  //     0:1:4 -- Y Label
  //       |
  //     0:1:5 -- Z Label

	// Create a new label in the data structure for the cylinder
    TDF_Label L = TDF_TagSource::NewChild(MainLab);

	// Create the data structure : Set the dimensions, position and name attributes
	TDataStd_Real::Set(L.FindChild(1), r);
	TDataStd_Real::Set(L.FindChild(2), h);
	TDataStd_Real::Set(L.FindChild(3), x);
	TDataStd_Real::Set(L.FindChild(4), y);
	TDataStd_Real::Set(L.FindChild(5), z);
	TDataStd_Name::Set(L, Name);


	// Instanciate a TFunction_Function attribute connected to the current cylinder driver
	// and attach it to the data structure as an attribute of the Cylinder Label
	Handle(TFunction_Function) myFunction = TFunction_Function::Set(L, TOcafFunction_CylDriver::GetID());

	// Initialize and execute the cylinder driver (look at the "Execute()" code)
    Handle(TFunction_Logbook) log = TFunction_Logbook::Set(L);

	Handle(TFunction_Driver) myCylDriver;
	// Find the TOcafFunction_CylDriver in the TFunction_DriverTable using its GUID
	if(!TFunction_DriverTable::Get()->FindDriver(TOcafFunction_CylDriver::GetID(), myCylDriver)) return L;

	myCylDriver->Init(L);
    if (myCylDriver->Execute(log)) MessageBoxW (NULL, L"DFunction_Execute : failed", L"Cylinder", MB_ICONERROR);

	return L;
}


TDF_Label TOcaf_Commands::ModifyBox(Standard_Real x, Standard_Real y, Standard_Real z, Standard_Real w, Standard_Real l, Standard_Real h, const TCollection_ExtendedString &Name, Handle(TFunction_Logbook) &log)
{
	// Set the name attribute (if it's not null)
	Handle(TDataStd_Name) myStdName;
	if(Name.Length())
		TDataStd_Name::Set(MainLab, Name);

	// Set the dimensions and position attributes (if changed)
	Handle(TDataStd_Real) curReal;
	MainLab.FindChild(1).FindAttribute(TDataStd_Real::GetID(),curReal);
	if(w!=curReal->Get())
	{
		TDataStd_Real::Set(MainLab.FindChild(1), w);
		// Set the label of the attribute as touched for next recomputation
		log->SetTouched(curReal->Label());
	}

	MainLab.FindChild(2).FindAttribute(TDataStd_Real::GetID(),curReal);
	if(l!=curReal->Get())
	{
		TDataStd_Real::Set(MainLab.FindChild(2), l);
		// Set the label of the attribute as touched for next recomputation
		log->SetTouched(curReal->Label());
	}

	MainLab.FindChild(3).FindAttribute(TDataStd_Real::GetID(),curReal);
	if(h!=curReal->Get())
	{
		TDataStd_Real::Set(MainLab.FindChild(3), h);
		// Set the label of the attribute as touched for next recomputation
		log->SetTouched(curReal->Label());
	}

	MainLab.FindChild(4).FindAttribute(TDataStd_Real::GetID(),curReal);
	if(x!=curReal->Get())
	{
		TDataStd_Real::Set(MainLab.FindChild(4), x);
		// Set the label of the attribute as touched for next recomputation
		log->SetTouched(curReal->Label());
	}

	MainLab.FindChild(5).FindAttribute(TDataStd_Real::GetID(),curReal);
	if(y!=curReal->Get())
	{
		TDataStd_Real::Set(MainLab.FindChild(5), y);
		// Set the label of the attribute as touched for next recomputation
		log->SetTouched(curReal->Label());
	}

	MainLab.FindChild(6).FindAttribute(TDataStd_Real::GetID(),curReal);
	if(z!=curReal->Get())
	{
		TDataStd_Real::Set(MainLab.FindChild(6), z);
		// Set the label of the attribute as touched for next recomputation
		log->SetTouched(curReal->Label());
	}

	// Get the TFunction_Function used to create the box
	Handle(TFunction_Function) TFF; 
	if ( !MainLab.FindAttribute(TFunction_Function::GetID(),TFF) )
	{
		MessageBox (NULL, L"Object cannot be modify.", L"Modification", MB_ICONEXCLAMATION);
		return MainLab;
	}

	// Get the TFunction_FunctionDriver GUID used with the TFunction_Function
	Standard_GUID myDriverID=TFF->GetDriverGUID();
	Handle(TFunction_Driver) myBoxDriver;
	// Find the TOcafFunction_BoxDriver in the TFunction_DriverTable using its GUID 
	TFunction_DriverTable::Get()->FindDriver(myDriverID, myBoxDriver);
	// Execute the cut if it must be (if an attribute changes)
	myBoxDriver->Init(MainLab);
	if (myBoxDriver->MustExecute(log))
	{
		// Set the box touched, it will be usefull to recompute an object which used this box as attribute
		log->SetTouched(MainLab);
		if(myBoxDriver->Execute(log))
			MessageBox (NULL, L"Recompute failed", L"Modify box", MB_ICONEXCLAMATION);
	}

	return MainLab;
}

TDF_Label TOcaf_Commands::ModifyCyl(Standard_Real x, Standard_Real y, Standard_Real z, Standard_Real r, Standard_Real h, const TCollection_ExtendedString &Name, Handle(TFunction_Logbook) &log)
{
	// Set the name attribute (if it's not null)
	Handle(TDataStd_Name) myStdName;
	if(Name.Length())
		TDataStd_Name::Set(MainLab, Name);

	// Set the dimensions and position attributes (if changed)
	Handle(TDataStd_Real) curReal;
	MainLab.FindChild(1).FindAttribute(TDataStd_Real::GetID(),curReal);
	if(r!=curReal->Get())
	{
		TDataStd_Real::Set(MainLab.FindChild(1), r);
		// Set the label of the attribute as touched for next recomputation
		log->SetTouched(curReal->Label());
	}

	MainLab.FindChild(2).FindAttribute(TDataStd_Real::GetID(),curReal);
	if(h!=curReal->Get())
	{
		TDataStd_Real::Set(MainLab.FindChild(2), h);
		// Set the label of the attribute as touched for next recomputation
		log->SetTouched(curReal->Label());
	}

	MainLab.FindChild(3).FindAttribute(TDataStd_Real::GetID(),curReal);
	if(x!=curReal->Get())
	{
		TDataStd_Real::Set(MainLab.FindChild(3), x);
		// Set the label of the attribute as touched for next recomputation
		log->SetTouched(curReal->Label());
	}

	MainLab.FindChild(4).FindAttribute(TDataStd_Real::GetID(),curReal);
	if(y!=curReal->Get())
	{
		TDataStd_Real::Set(MainLab.FindChild(4), y);
		// Set the label of the attribute as touched for next recomputation
		log->SetTouched(curReal->Label());
	}

	MainLab.FindChild(5).FindAttribute(TDataStd_Real::GetID(),curReal);
	if(z!=curReal->Get())
	{
		TDataStd_Real::Set(MainLab.FindChild(5), z);
		// Set the label of the attribute as touched for next recomputation
		log->SetTouched(curReal->Label());
	}

	// Get the TFunction_Function used to create the cylinder
	Handle(TFunction_Function) TFF; 
	if ( !MainLab.FindAttribute(TFunction_Function::GetID(),TFF) )
	{
		MessageBoxW (NULL, L"Object cannot be modify.", L"Modification", MB_ICONEXCLAMATION);
		return MainLab;
	}

	// Get the TFunction_FunctionDriver GUID used with the TFunction_Function
	Standard_GUID myDriverID=TFF->GetDriverGUID();
	Handle(TFunction_Driver) myCylDriver;
	// Find the TOcafFunction_CylDriver in the TFunction_DriverTable using its GUID 
	TFunction_DriverTable::Get()->FindDriver(myDriverID, myCylDriver);
	// Execute the cut if it must be (if an attribute changes)
	myCylDriver->Init(MainLab);
	if (myCylDriver->MustExecute(log))
	{
		// Set the cylinder touched, it will be usefull to recompute an object which used this cylinder as attribute
		log->SetTouched(MainLab);
		if(myCylDriver->Execute(log))
			MessageBoxW (NULL, L"Recompute failed", L"Modify cylinder", MB_ICONEXCLAMATION);
	}

	return MainLab;
}

TDF_Label TOcaf_Commands::Cut(TDF_Label ObjectLab, TDF_Label ToolObjectLab)
{
  // A data structure for our cut operation:
  // the result itself is attached to the Result label (as his name and his function attribute) 
  // its arguments (Original object label; Tool object label) are attached to the child 
  // labels of the Result Label:
  //
  // 0:1 Result Label ---> Name --->  Named shape ---> Function
  //       |
  //     0:1:1 -- Original object label Label
  //       |
  //     0:1:2 --  object label Label
  // 

	// Create a new label in the data structure for the box
    TDF_Label L = TDF_TagSource::NewChild(MainLab);

	// Create the data structure : Set a reference attribute on the Original and the Tool objects, set the name attribute
	TDF_Reference::Set(L.FindChild(1), ObjectLab);
	TDF_Reference::Set(L.FindChild(2), ToolObjectLab);
	Handle(TDataStd_Name) ObjectName;
	ObjectLab.FindAttribute(TDataStd_Name::GetID(),ObjectName);
	TDataStd_Name::Set(L, ObjectName->Get());


	// Instanciate a TFunction_Function attribute connected to the current cut driver
	// and attach it to the data structure as an attribute of the Result Label
	Handle(TFunction_Function) myFunction = TFunction_Function::Set(L, TOcafFunction_CutDriver::GetID());

	// Initialize and execute the cut driver (look at the "Execute()" code)
    Handle(TFunction_Logbook) log = TFunction_Logbook::Set(L);

	Handle(TFunction_Driver) myCutDriver;
    // Find the TOcafFunction_CutDriver in the TFunction_DriverTable using its GUID 
	if(!TFunction_DriverTable::Get()->FindDriver(TOcafFunction_CutDriver::GetID(), myCutDriver)) return L;
		
	myCutDriver->Init(L);
    if (myCutDriver->Execute(log)) MessageBoxW (NULL, L"DFunction_Execute : failed", L"Cut", MB_ICONERROR);

	return L;
}
