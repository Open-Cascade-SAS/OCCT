# Copyright (c) 2026 OPEN CASCADE SAS
#
# This file is part of Open CASCADE Technology software library.
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License version 2.1 as published
# by the Free Software Foundation, with special exception defined in the file
# OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
# distribution for complete text of the license and disclaimer of any warranty.
#
# Alternatively, this file may be used under the terms of Open CASCADE
# commercial license or contractual agreement.

# Select the warning from the OCCT build flags before generating package configs.
set (OCCT_LICENSE_WARNING "")
set (OCCT_LICENSE_WARNING_REASON "")
if (NOT BUILD_SHARED_LIBS)
  string (CONCAT OCCT_LICENSE_WARNING_REASON
    "When distributing an application statically linked with OCCT under LGPL 2.1, "
    "provide the materials required by section 6 to relink with a modified OCCT library. ")
elseif (APPLE OR IOS OR ANDROID OR CMAKE_SYSTEM_NAME MATCHES "^(Darwin|iOS|tvOS|watchOS|visionOS|Android)$")
  string (CONCAT OCCT_LICENSE_WARNING_REASON
    "For shared OCCT on Apple platforms or Android, code signing, library validation, "
    "or application store restrictions may prevent use of a modified OCCT library. "
    "Shared linking alone does not establish LGPL 2.1 section 6 compliance. ")
endif()

if (OCCT_LICENSE_WARNING_REASON)
  string (CONCAT OCCT_LICENSE_WARNING_TEXT
    "OCCT license notice: OCCT is available under LGPL 2.1 with the OCCT exception, "
    "or under a separate commercial license or contractual agreement. "
    "${OCCT_LICENSE_WARNING_REASON}"
    "For LGPL distribution, review how users can use a modified OCCT library and the distribution terms. "
    "See LICENSE_LGPL_21.txt and OCCT_LGPL_EXCEPTION.txt. "
    "For commercial licensing, contact https://occt3d.com/")
  message (AUTHOR_WARNING "${OCCT_LICENSE_WARNING_TEXT}")
  set (OCCT_LICENSE_WARNING "message (AUTHOR_WARNING \"${OCCT_LICENSE_WARNING_TEXT}\")")
endif()
