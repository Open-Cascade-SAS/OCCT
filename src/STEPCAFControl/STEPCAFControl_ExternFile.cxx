// File:	STEPCAFControl_ExternFile.cxx
// Created:	Thu Sep 28 16:35:48 2000
// Author:	Andrey BETENEV
//		<abv@doomox.nnov.matra-dtv.fr>

#include <STEPCAFControl_ExternFile.ixx>

//=======================================================================
//function : STEPCAFControl_ExternFile
//purpose  : 
//=======================================================================

STEPCAFControl_ExternFile::STEPCAFControl_ExternFile ()
     : myLoadStatus(IFSelect_RetVoid), myTransferStatus(Standard_False),
       myWriteStatus(IFSelect_RetVoid)
{
}
