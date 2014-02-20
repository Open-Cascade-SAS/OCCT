// Created on: 1997-04-21
// Created by: JLF, CAL
// Copyright (c) 1997-1999 Matra Datavision
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

//-Version	

//-Design	Declaration des variables specifiques aux plotters

//-Warning	

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Graphic3d_Plotter.ixx>

//-Aliases

//-Global data definitions

//-Constructors

Graphic3d_Plotter::Graphic3d_Plotter ():
MyPlottingState (Standard_False) {

}

//-Destructors

void Graphic3d_Plotter::Destroy () {

}

//-Methods, in order

Standard_Boolean Graphic3d_Plotter::BeginPlot (const Handle(Graphic3d_DataStructureManager)& /*aProjector*/) {

	if (MyPlottingState)
		Graphic3d_PlotterDefinitionError::Raise
			("Graphic3d_Plotter::BeginPlot\n");

	MyPlottingState	= Standard_True;

	Graphic3d_PlotterDefinitionError::Raise
		("Graphic3d_Plotter::BeginPlot\n");

	return MyPlottingState;

}

void Graphic3d_Plotter::EndPlot () {

	if (! MyPlottingState)
		Graphic3d_PlotterDefinitionError::Raise
			("Graphic3d_Plotter::EndPlot\n");

	MyPlottingState	= Standard_False;

	Graphic3d_PlotterDefinitionError::Raise
		("Graphic3d_Plotter::EndPlot\n");

}

Standard_Boolean Graphic3d_Plotter::PlottingState () const {

	return (MyPlottingState);

}
