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


#include <Interface_Statics.hxx>
#include <StepAP214.hxx>
#include <StepAP214_Protocol.hxx>

StaticHandle(StepAP214_Protocol, proto);

Handle(StepAP214_Protocol) StepAP214::Protocol()

	{
		InitHandleVoid(StepAP214_Protocol, proto);
		return proto;
	}

