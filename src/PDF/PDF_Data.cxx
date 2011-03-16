// File:	PDF_Data.cxx
//      	------------
// Author:	DAUTRY Philippe
//		<fid@fox.paris1.matra-dtv.fr>
// Copyright:	Matra Datavision 1997

// Version:	0.0
// History:	Version	Date		Purpose
//		0.0	Mar 13 1997	Creation



#include <PDF_Data.ixx>


//=======================================================================
//function : PDF_Data
//purpose  : 
//=======================================================================

PDF_Data::PDF_Data() :
myVersion(0)
{}

//=======================================================================
//function : PDF_Data
//purpose  : 
//=======================================================================

PDF_Data::PDF_Data(const Standard_Integer aVersionNumber) :
myVersion(aVersionNumber)
{}

