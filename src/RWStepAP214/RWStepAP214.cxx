// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <RWStepAP214.ixx>
#include <RWHeaderSection.hxx>

#include <StepAP214_Protocol.hxx>

#include <StepAP214.hxx>
#include <RWStepAP214_ReadWriteModule.hxx>
#include <RWStepAP214_GeneralModule.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_ReaderLib.hxx>
#include <StepData_WriterLib.hxx>


static int init = 0;

void RWStepAP214::Init()
	{
	  if (init) return;    init = 1;
	  RWHeaderSection::Init();
	  Handle(StepAP214_Protocol) proto = StepAP214::Protocol();
	  Interface_GeneralLib::SetGlobal (new RWStepAP214_GeneralModule,proto);
	  Interface_ReaderLib::SetGlobal  (new RWStepAP214_ReadWriteModule,proto);
	  StepData_WriterLib::SetGlobal   (new RWStepAP214_ReadWriteModule,proto);
	}
