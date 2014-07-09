#------------------------------------------------------------------------
# Check for TCL
# Options: --with-tcl=  :    Specify location of tclConfig.sh
#          --without-tcl=  : Skip check for TCL, assume not installed
# Defines:
#          HAVE_TCL : yes/no
#          Everything from tclConfig.sh
#------------------------------------------------------------------------
AC_DEFUN([SC_TCL_CFG], [
  AC_ARG_WITH(tcl, 
AC_HELP_STRING([--with-tcl=PATH],[Directory containing tclConfig.sh])
AC_HELP_STRING([--without-tcl],  [Assume no tcl libraries available]) , 
              [with_tcl="${withval}";require_tcl=yes],[with_tcl=yes;require_tcl=no])
  
  # If user requested disabling of tcl check
  if test "xno" = "x$with_tcl"; then
    HAVE_TCL=no
    require_tcl=no
  # Otherwise...
  else
    HAVE_TCL=no
    # Search for tclConfig.sh
    if test "xyes" != "x$with_tcl"; then
      # If user specified location
      . "${with_tcl}/tclConfig.sh" || AC_MSG_ERROR("Cannot read file: ${with_tcl}/tclConfig.sh")
      HAVE_TCL=yes
    else
      # Search for tclConfig.sh in usual spots
      tcl_cfg_file=
      AC_MSG_CHECKING([for tclConfig.sh])
      for i in /usr /usr/local ${prefix} /opt/sfw /usr/tcltk; do
        if test -d $i; then
          for j in $i/lib $i/lib/tcl $i/lib/tcl[[8-9]].[[0-9]] $i/lib/itcl; do
            if test -r "$j/tclConfig.sh"; then
              tcl_cfg_file="$j/tclConfig.sh"
              break
            fi
          done
        fi
        if test "x" != "x$tcl_cfg_file"; then
          break
        fi
      done
      AC_MSG_RESULT("$tcl_cfg_file")
      
      if test "x" != "x$tcl_cfg_file"; then
        . "$tcl_cfg_file" || AC_MSG_ERROR("Cannot read file: $tcl_cfg_file") 
        HAVE_TCL=yes

        # if TCL_INCLUDE_SPEC wasn't defined, try to define it using TCL_INC_DIR
        if test "x" = "x$TCL_INCLUDE_SPEC"; then
          inc_path=`expr "x$TCL_INC_DIR" : "x\(.*\)/tcl-private/generic"`
          if test "x" != "$inc_path"; then
            TCL_INCLUDE_SPEC="-I$inc_path"
          elif test "x" != "x$TCL_INC_DIR"; then
            TCL_INCLUDE_SPEC="-I$TCL_INC_DIR"
          fi
        fi
      else
        HAVE_TCL=no
      fi
    fi
    
    # Check for tcl.h
    AC_LANG_C
    save_CPPFLAGS="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS $TCL_INCLUDE_SPEC"
    AC_CHECK_HEADER([tcl.h],[],[HAVE_TCL=no])
    CPPFLAGS="$save_CPPFLAGS"
    
    if test "xyes" = "x$require_tcl"; then
      if test "xno" = "x$HAVE_TCL"; then
        AC_MSG_ERROR([TCL not found.])
      fi
    fi
  fi
])


#------------------------------------------------------------------------
# Check for Tk
# Options: --with-tk=  :    Specify location of Tk
#          --without-tk=  : Skip check for Tk, assume not installed
# Defines:
#          HAVE_TK : yes/no
#          Everything from tkConfig.sh
#------------------------------------------------------------------------
AC_DEFUN([SC_TK_CFG], [
  AC_ARG_WITH(tk, 
AC_HELP_STRING([--with-tk=PATH],[Directory containing tkConfig.sh])
AC_HELP_STRING([--without-tk],  [Assume no Tk libraries available]) , 
              [with_tk="${withval}";require_tk=yes],[with_tk=yes;require_tk=no])
  
  # If user requested disabling of Tk check
  if test "xno" = "x$with_tk"; then
    HAVE_TK=no
    require_tk=no
  # Otherwise...
  else
    HAVE_TK=no
    # Search for tkConfig.sh
    if test "xyes" != "x$with_tk"; then
      # If user specified location
      . "${with_tk}/tkConfig.sh" || AC_MSG_ERROR("Cannot read file: ${with_tk}/tkConfig.sh")
      HAVE_TK=yes
    else
      # Search for tkConfig.sh in usual spots
      tk_cfg_file=
      AC_MSG_CHECKING([for tkConfig.sh])
      for i in /usr /usr/local ${prefix} /opt/sfw /usr/tcltk; do
        if test -d $i; then
          for j in $i/lib $i/lib/tk $i/lib/tk[[8-9]].[[0-9]] $i/lib/itcl; do
            if test -r "$j/tkConfig.sh"; then
              tk_cfg_file="$j/tkConfig.sh"
              break
            fi
          done
        fi
        if test "x" != "x$tk_cfg_file"; then
          break
        fi
      done
      AC_MSG_RESULT("$tk_cfg_file")
      
      if test "x" != "x$tk_cfg_file"; then
        . "$tk_cfg_file" || AC_MSG_ERROR("Cannot read file: $tk_cfg_file") 
        HAVE_TK=yes

        # if TK_INCLUDE_SPEC wasn't defined, try to define it using TK_INC_DIR
        if test "x" = "x$TK_INCLUDE_SPEC"; then
          inc_path=`expr "x$TK_INC_DIR" : "x\(.*\)/tk-private/generic"`
          if test "x" != "$inc_path"; then
            TK_INCLUDE_SPEC="-I$inc_path"
          elif test "x" != "x$TK_INC_DIR"; then
            TK_INCLUDE_SPEC="-I$TK_INC_DIR"
          fi
        fi
      else
        HAVE_TK=no
      fi
    fi
    
    # Check for tk.h
    AC_LANG_C
    HAVE_TK=yes
    save_CPPFLAGS="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS $TCL_INCLUDE_SPEC $TK_INCLUDE_SPEC"
    AC_CHECK_HEADER([tk.h],[],[HAVE_TK=no])
    CPPFLAGS="$save_CPPFLAGS"
    
    if test "xyes" = "x$require_tk"; then
      if test "xno" = "x$HAVE_TK"; then
        AC_MSG_ERROR([Tk not found.])
      fi
    fi
  fi
])
