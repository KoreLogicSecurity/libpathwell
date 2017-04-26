########################################################################
#
# $Id: configure.ac,v 1.63 2015/12/09 14:51:00 klm Exp $
#
########################################################################

dnl ####################################################################
dnl #
dnl # Set project/package and library version information.
dnl #
dnl ####################################################################

m4_define(m4_libpathwell_current, m4_esyscmd([egrep '^CURRENT=[0-9]+$' VERSION.LIBRARY | awk -F= '{print $2}' | tr -d '\n']))
m4_define(m4_libpathwell_revision, m4_esyscmd([egrep '^REVISION=[0-9]+$' VERSION.LIBRARY | awk -F= '{print $2}' | tr -d '\n']))
m4_define(m4_libpathwell_age, m4_esyscmd([egrep '^AGE=[0-9]+$' VERSION.LIBRARY | awk -F= '{print $2}' | tr -d '\n']))
m4_define(m4_libpathwell_version, m4_esyscmd([echo -n "m4_libpathwell_current:m4_libpathwell_revision:m4_libpathwell_age"]))

m4_define(m4_pam_pathwell_current, m4_esyscmd([egrep '^CURRENT=[0-9]+$' VERSION.MODULE | awk -F= '{print $2}' | tr -d '\n']))
m4_define(m4_pam_pathwell_revision, m4_esyscmd([egrep '^REVISION=[0-9]+$' VERSION.MODULE | awk -F= '{print $2}' | tr -d '\n']))
m4_define(m4_pam_pathwell_age, m4_esyscmd([egrep '^AGE=[0-9]+$' VERSION.MODULE | awk -F= '{print $2}' | tr -d '\n']))
m4_define(m4_pam_pathwell_version, m4_esyscmd([echo -n "m4_pam_pathwell_current:m4_pam_pathwell_revision:m4_pam_pathwell_age"]))

m4_define(m4_libpathwell_db_schema_version, m4_esyscmd([cat VERSION.SCHEMA | tr -d '\n']))

m4_define(m4_libpathwell_release_number, m4_esyscmd([cat VERSION | tr -d '\n']))
m4_define(m4_libpathwell_release_string, m4_esyscmd([utils/version2string -t tar -v `cat VERSION` | tr -d '\n']))

m4_define(m4_libpathwell_major, m4_esyscmd([utils/version2string -t tar -v `cat VERSION` | awk -F. '{print $1}' | tr -d '\n']))
m4_define(m4_libpathwell_minor, m4_esyscmd([utils/version2string -t tar -v `cat VERSION` | awk -F. '{print $2}' | tr -d '\n']))
m4_define(m4_libpathwell_patch, m4_esyscmd([utils/version2string -t tar -v `cat VERSION` | awk -F. '{print $3}' | tr -d '\n']))

dnl ####################################################################
dnl #
dnl # Initialize.
dnl #
dnl ####################################################################

AC_PREREQ(2.63)
AC_INIT([libpathwell], [m4_libpathwell_release_string], [libpathwell-project@korelogic.com], [libpathwell])
AC_CONFIG_AUX_DIR(utils)
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/libpathwell/api.c])
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE([foreign])

AC_DEFINE(PW_V_LIBRARY_CURRENT, [m4_libpathwell_current], [Define to the current version number of this library.])
AC_DEFINE(PW_V_LIBRARY_REVISION, [m4_libpathwell_revision], [Define to the revision version number of this library.])
AC_DEFINE(PW_V_LIBRARY_AGE, [m4_libpathwell_age], [Define to the age version number of this library.])
AC_DEFINE(PW_V_LIBRARY_VERSION, ["m4_libpathwell_version"], [Define to the version number of this library.])
libpathwell_version="m4_libpathwell_version"
AC_SUBST(libpathwell_version)

AC_DEFINE(PW_V_MODULE_CURRENT, [m4_pam_pathwell_current], [Define to the current version number of this module.])
AC_DEFINE(PW_V_MODULE_REVISION, [m4_pam_pathwell_revision], [Define to the revision version number of this module.])
AC_DEFINE(PW_V_MODULE_AGE, [m4_pam_pathwell_age], [Define to the age version number of this module.])
AC_DEFINE(PW_V_MODULE_VERSION, ["m4_pam_pathwell_version"], [Define to the version number of this module.])
pam_pathwell_version="m4_pam_pathwell_version"
AC_SUBST(pam_pathwell_version)

AC_DEFINE(PW_V_DB_SCHEMA_VERSION, [m4_libpathwell_db_schema_version], [Define to the version number of the DB schema.])

AC_DEFINE(PW_V_RELEASE_NUMBER, [m4_libpathwell_release_number], [Define to the release number of this package.])

AC_DEFINE(PW_V_RELEASE_MAJOR, [m4_libpathwell_major], [Define to the major release number of this package.])
AC_DEFINE(PW_V_RELEASE_MINOR, [m4_libpathwell_minor], [Define to the minor release number of this package.])
AC_DEFINE(PW_V_RELEASE_PATCH, [m4_libpathwell_patch], [Define to the patch release number of this package.])

: ${CFLAGS="-O3"}

LIB_LIST="/usr /usr/local /usr/pkg /opt /opt/local" 

dnl ####################################################################
dnl #
dnl # Check for programs.
dnl #
dnl ####################################################################

AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

dnl ####################################################################
dnl #
dnl # Check for certain operating systems that need special treatment.
dnl #
dnl ####################################################################

AC_EXEEXT

dnl ####################################################################
dnl #
dnl # Check for Libtool.
dnl #
dnl ####################################################################

AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

case "${target_os}" in
*mingw*)
  libtool_flags="-no-undefined"
  AC_SUBST(libtool_flags)
  ;;
esac

dnl ####################################################################
dnl #
dnl # Check for libraries.
dnl #
dnl ####################################################################

dnl ####################################################################
dnl #
dnl # Check for header files.
dnl #
dnl ####################################################################

dnl ####################################################################
dnl #
dnl # Check for typedefs, structures, and compiler characteristics.
dnl #
dnl ####################################################################

dnl ####################################################################
dnl #
dnl # Check for library functions.
dnl #
dnl ####################################################################

dnl ####################################################################
dnl #
dnl # Check for GMP.
dnl #
dnl ####################################################################

my_gmpdir_specified="0"

CheckGmpComponents()
{
  GMPDIR=${1}
  INCLUDE_LIST="gmp.h"
  for INCLUDE_FILE in ${INCLUDE_LIST} ; do
    if test ! -f "${GMPDIR}/include/${INCLUDE_FILE}" ; then
      my_target="${target_cpu}-${target_os}"
      if test ! -f "${GMPDIR}/include/${my_target}/${INCLUDE_FILE}" ; then
        return 1
      fi
    fi
  done
  my_gmpdir=${GMPDIR}
  return 0
}

AC_MSG_CHECKING([for GMP support])
AC_ARG_WITH(gmp,
  [  --with-gmp=DIR          Use GMP with includes and libs from [DIR]/include and [DIR]/lib],
  [
    my_gmpdir_specified="1"
    CheckGmpComponents "${withval}"
  ],
  [
    for my_basedir in ${LIB_LIST} ; do
      for my_dir in "${my_basedir}" "${my_basedir}/gmp" ; do
        CheckGmpComponents "${my_dir}" && break 2
      done
    done
  ]
)
if test -z "${my_gmpdir}" ; then
  AC_MSG_RESULT([missing or incomplete])
  echo
  echo "Unable to locate required GMP components. Use --with-gmp=DIR to"
  echo "specify a known or different location. The files that must exist for"
  echo "this check to pass are:"
  echo
  echo "  DIR/include/gmp.h"
  echo
  exit 1
else
  AC_MSG_RESULT([${my_gmpdir}])
  gmp_cflags="-I${my_gmpdir}/include"
  gmp_ldadd="-lgmp"
  gmp_ldflags="-L${my_gmpdir}/lib"
  case "${target_os}" in
  *mingw*)
    if test ${my_gmpdir_specified} -eq 1 ; then
      export PATH="${PATH}:${my_gmpdir}/bin"
    fi
    ;;
  esac
  AC_SUBST(gmp_cflags)
  AC_SUBST(gmp_ldadd)
  AC_SUBST(gmp_ldflags)
  OLD_CFLAGS=${CFLAGS}
  OLD_LIBS=${LIBS}
  OLD_LD_LIBRARY_PATH=${LD_LIBRARY_PATH}
  CFLAGS="${CFLAGS} ${gmp_cflags}"
  LIBS="${LIBS} ${gmp_ldflags} ${gmp_ldadd}"
  export LD_LIBRARY_PATH="${my_gmpdir}/lib:${LD_LIBRARY_PATH}"
  AC_MSG_CHECKING([whether or not GMP release number is ok])
  AC_RUN_IFELSE(
    [
      AC_LANG_SOURCE(
      [[
#include <gmp.h>
int main() { unsigned int uiVersion = (__GNU_MP_VERSION << 24) | (__GNU_MP_VERSION_MINOR << 16) | __GNU_MP_VERSION_PATCHLEVEL;  return (uiVersion >= 0x05000002) ? 0 : 1; }
      ]]
      )
    ],
    [gmp_version_ok="1"],
    [gmp_version_ok="0"],
    [gmp_version_ok="2"]
  )
  if test ${gmp_version_ok} -eq 1 ; then
    AC_MSG_RESULT([yes])
  elif test ${gmp_version_ok} -eq 2 ; then
    AC_MSG_RESULT([skipped (cross-compiling)])
  else
    AC_MSG_RESULT([no (5.0.2 or higher required)])
    exit 1
  fi
  CFLAGS=${OLD_CFLAGS}
  LIBS=${OLD_LIBS}
  export LD_LIBRARY_PATH=${OLD_LD_LIBRARY_PATH}
fi

dnl ####################################################################
dnl #
dnl # Check for JSON.
dnl #
dnl ####################################################################

CheckJsonComponents()
{
  JSONDIR=${1}
  INCLUDE_LIST="jansson.h"
  for INCLUDE_FILE in ${INCLUDE_LIST} ; do
    if test ! -f "${JSONDIR}/include/${INCLUDE_FILE}" ; then
      return 1
    fi
  done
  my_jsondir=${JSONDIR}
  return 0
}

AC_MSG_CHECKING([for JSON support])
AC_ARG_WITH(json,
  [  --with-json=DIR         Use JSON with includes and libs from [DIR]/include and [DIR]/lib
  --without-json          Disable JSON support],
  [
    case `echo ${withval} | tr "A-Z" "a-z"` in
    no)
      use_json="0"
      ;;
    *)
      use_json="1"
      CheckJsonComponents "${withval}"
      ;;
    esac
  ],
  [
    use_json="1"
    for my_basedir in ${LIB_LIST} ; do
      for my_dir in "${my_basedir}" "${my_basedir}/jansson" ; do
        CheckJsonComponents "${my_dir}" && break 2
      done
    done
  ]
)
if test ${use_json} -ne 0 ; then
  if test -z "${my_jsondir}" ; then
    AC_MSG_RESULT([missing or incomplete])
    echo
    echo "Unable to locate required JSON components. Use --with-json=DIR to"
    echo "specify a known or different location. The files that must exist for"
    echo "this check to pass are:"
    echo
    echo "  DIR/include/jansson.h"
    echo
    exit 1
  else
    AC_MSG_RESULT([${my_jsondir}])
    json_cflags="-I${my_jsondir}/include"
    json_ldadd="-ljansson"
    json_ldflags="-L${my_jsondir}/lib"
    AC_SUBST(json_cflags)
    AC_SUBST(json_ldadd)
    AC_SUBST(json_ldflags)
    OLD_CFLAGS=${CFLAGS}
    OLD_LIBS=${LIBS}
    OLD_LD_LIBRARY_PATH=${LD_LIBRARY_PATH}
    CFLAGS="${CFLAGS} ${json_cflags}"
    LIBS="${LIBS} ${json_ldflags} ${json_ldadd}"
    export LD_LIBRARY_PATH="${my_jsondir}/lib:${LD_LIBRARY_PATH}"
    AC_MSG_CHECKING([whether or not JSON release number is ok])
    AC_RUN_IFELSE(
      [
        AC_LANG_SOURCE(
        [[
#include <jansson.h>
int main() { return (JANSSON_VERSION_HEX >= 0x00020301) ? 0 : 1; }
        ]]
        )
      ],
      [json_version_ok="1"],
      [json_version_ok="0"],
      [json_version_ok="2"]
    )
    if test ${json_version_ok} -eq 1 ; then
      AC_DEFINE(USE_JSON, 1, [Define to 1 if JSON support is enabled.])
      AC_MSG_RESULT([yes])
    elif test ${json_version_ok} -eq 2 ; then
      AC_MSG_RESULT([skipped (cross-compiling)])
    else
      AC_MSG_RESULT([no (2.3.1 or higher required)])
      exit 1
    fi
    CFLAGS=${OLD_CFLAGS}
    LIBS=${OLD_LIBS}
    export LD_LIBRARY_PATH=${OLD_LD_LIBRARY_PATH}
  fi
else
  AC_MSG_RESULT(check explicitly disabled)
fi

dnl ####################################################################
dnl #
dnl # Check for PAM.
dnl #
dnl ####################################################################

my_pamdir_specified="0"

CheckPAMComponents()
{
  PAMDIR=${1}
  INCLUDE_LIST="security/pam_appl.h security/pam_modules.h"
  for INCLUDE_FILE in ${INCLUDE_LIST} ; do
    if test ! -f "${PAMDIR}/include/${INCLUDE_FILE}" ; then
      return 1
    fi
  done
  my_pamdir=${PAMDIR}
  return 0
}

AC_MSG_CHECKING([for PAM support])
AC_ARG_WITH(pam,
  [  --with-pam=DIR          Use PAM with includes and libs from [DIR]/include and [DIR]/lib
  --without-pam           Disable PAM support],
  [
    case `echo ${withval} | tr "A-Z" "a-z"` in
    no)
      use_pam="0"
      ;;
    *)
      use_pam="1"
      my_pamdir_specified="1"
      CheckPAMComponents "${withval}"
      ;;
    esac
  ],
  [
    use_pam="1"
    for my_basedir in ${LIB_LIST} ; do
      for my_dir in "${my_basedir}" "${my_basedir}/pam" ; do
        CheckPAMComponents "${my_dir}" && break 2
      done
    done
  ]
)
if test ${use_pam} -ne 0 ; then
  AC_DEFINE(USE_PAM, 1, [Define to 1 if PAM support is enabled.])
  if test -z "${my_pamdir}" ; then
    AC_MSG_RESULT([missing or incomplete])
    echo
    echo "Unable to locate required PAM components. Use --with-pam=DIR to"
    echo "specify a known or different location. The files that must exist for"
    echo "this check to pass are:"
    echo
    echo "  DIR/include/pam_appl.h"
    echo "  DIR/include/pam_modules.h"
    echo
    exit 1
  else
    AC_MSG_RESULT([${my_pamdir}])
    pam_cflags="-I${my_pamdir}/include"
    pam_ldadd="-lpam"
    pam_ldflags="-L${my_pamdir}/lib"
    pam_doc_subdirs="pam_pathwell"
    pam_src_subdirs="pam_pathwell"
    AC_SUBST(pam_cflags)
    AC_SUBST(pam_ldadd)
    AC_SUBST(pam_ldflags)
    AC_SUBST(pam_doc_subdirs)
    AC_SUBST(pam_src_subdirs)
    OLD_CFLAGS=${CFLAGS}
    OLD_LIBS=${LIBS}
    OLD_LD_LIBRARY_PATH=${LD_LIBRARY_PATH}
    CFLAGS="${CFLAGS} ${pam_cflags}"
    LIBS="${LIBS} ${pam_ldflags} ${pam_ldadd}"
    export LD_LIBRARY_PATH="${my_pamdir}/lib:${LD_LIBRARY_PATH}"
    AC_MSG_CHECKING([whether or not PAM release number is ok])
    AC_RUN_IFELSE(
      [
        AC_LANG_SOURCE(
        [[
#include <security/_pam_types.h>
int main() { unsigned int uiVersion = (__LINUX_PAM__ << 16) | (__LINUX_PAM_MINOR__); return (uiVersion >= 0x00010000) ? 0 : 1; }
        ]]
        )
      ],
      [pam_version_ok="1"],
      [pam_version_ok="0"],
      [pam_version_ok="2"]
    )
    if test ${pam_version_ok} -eq 1 ; then
      AC_MSG_RESULT([yes])
    elif test ${pam_version_ok} -eq 2 ; then
      AC_MSG_RESULT([skipped (cross-compiling)])
    else
      AC_MSG_RESULT([no (1.0 or higher required)])
      exit 1
    fi
    CFLAGS=${OLD_CFLAGS}
    LIBS=${OLD_LIBS}
    export LD_LIBRARY_PATH=${OLD_LD_LIBRARY_PATH}
  fi
else
  AC_MSG_RESULT(check explicitly disabled)
fi

dnl ####################################################################
dnl #
dnl # Check for PCRE.
dnl #
dnl ####################################################################

my_pcredir_specified="0"

CheckPcreComponents()
{
  PCREDIR=${1}
  INCLUDE_LIST="pcre.h"
  for INCLUDE_FILE in ${INCLUDE_LIST} ; do
    if test ! -f "${PCREDIR}/include/${INCLUDE_FILE}" ; then
      return 1
    fi
  done
  my_pcredir=${PCREDIR}
  return 0
}

AC_MSG_CHECKING([for PCRE support])
AC_ARG_WITH(pcre,
  [  --with-pcre=DIR         Use PCRE with includes and libs from [DIR]/include and [DIR]/lib],
  [
    my_pcredir_specified="1"
    CheckPcreComponents "${withval}"
  ],
  [
    for my_basedir in ${LIB_LIST} ; do
      for my_dir in "${my_basedir}" "${my_basedir}/pcre" ; do
        CheckPcreComponents "${my_dir}" && break 2
      done
    done
  ]
)
if test -z "${my_pcredir}" ; then
  AC_MSG_RESULT([missing or incomplete])
  echo
  echo "Unable to locate required PCRE components. Use --with-pcre=DIR to"
  echo "specify a known or different location. The files that must exist for"
  echo "this check to pass are:"
  echo
  echo "  DIR/include/pcre.h"
  echo
  exit 1
else
  AC_MSG_RESULT([${my_pcredir}])
  pcre_cflags="-I${my_pcredir}/include -DPCRE_STATIC"
  pcre_ldadd="-lpcre"
  pcre_ldflags="-L${my_pcredir}/lib"
  case "${target_os}" in
  *mingw*)
    if test ${my_pcredir_specified} -eq 1 ; then
      export PATH="${PATH}:${my_pcredir}/bin"
    fi
    ;;
  esac
  AC_SUBST(pcre_cflags)
  AC_SUBST(pcre_ldadd)
  AC_SUBST(pcre_ldflags)
  OLD_CFLAGS=${CFLAGS}
  OLD_LIBS=${LIBS}
  OLD_LD_LIBRARY_PATH=${LD_LIBRARY_PATH}
  CFLAGS="${CFLAGS} ${pcre_cflags}"
  LIBS="${LIBS} ${pcre_ldflags} ${pcre_ldadd}"
  export LD_LIBRARY_PATH="${my_pcredir}/lib:${LD_LIBRARY_PATH}"
  AC_MSG_CHECKING([whether or not PCRE release number is ok])
  AC_RUN_IFELSE(
    [
      AC_LANG_SOURCE(
      [[
#include <pcre.h>
int main() { unsigned int uiVersion = (PCRE_MAJOR << 16) | (PCRE_MINOR); return (uiVersion >= 0x0008000c) ? 0 : 1; }
      ]]
      )
    ],
    [pcre_version_ok="1"],
    [pcre_version_ok="0"],
    [pcre_version_ok="2"]
  )
  if test ${pcre_version_ok} -eq 1 ; then
    AC_MSG_RESULT([yes])
  elif test ${pcre_version_ok} -eq 2 ; then
    AC_MSG_RESULT([skipped (cross-compiling)])
  else
    AC_MSG_RESULT([no (8.12 or higher required)])
    exit 1
  fi
  CFLAGS=${OLD_CFLAGS}
  LIBS=${OLD_LIBS}
  export LD_LIBRARY_PATH=${OLD_LD_LIBRARY_PATH}
fi

dnl ####################################################################
dnl #
dnl # Check for SQLite.
dnl #
dnl ####################################################################

my_sqlitedir_specified="0"

CheckSqliteComponents()
{
  SQLITEDIR=${1}
  INCLUDE_LIST="sqlite3.h"
  for INCLUDE_FILE in ${INCLUDE_LIST} ; do
    if test ! -f "${SQLITEDIR}/include/${INCLUDE_FILE}" ; then
      return 1
    fi
  done
  my_sqlitedir=${SQLITEDIR}
  return 0
}

AC_MSG_CHECKING([for SQLite support])
AC_ARG_WITH(sqlite,
  [  --with-sqlite=DIR       Use SQLite with includes and libs from [DIR]/include and [DIR]/lib],
  [
    my_sqlitedir_specified="1"
    CheckSqliteComponents "${withval}"
  ],
  [
    for my_basedir in ${LIB_LIST} ; do
      for my_dir in "${my_basedir}" "${my_basedir}/sqlite" ; do
        CheckSqliteComponents "${my_dir}" && break 2
      done
    done
  ]
)
if test -z "${my_sqlitedir}" ; then
  AC_MSG_RESULT([missing or incomplete])
  echo
  echo "Unable to locate required SQLite components. Use --with-sqlite=DIR to"
  echo "specify a known or different location. The files that must exist for"
  echo "this check to pass are:"
  echo
  echo "  DIR/include/sqlite3.h"
  echo
  exit 1
else
  AC_MSG_RESULT([${my_sqlitedir}])
  sqlite_cflags="-I${my_sqlitedir}/include"
  sqlite_ldadd="-lsqlite3"
  sqlite_ldflags="-L${my_sqlitedir}/lib"
  case "${target_os}" in
  *mingw*)
    if test ${my_sqlitedir_specified} -eq 1 ; then
      export PATH="${PATH}:${my_sqlitedir}/bin"
    fi
    ;;
  esac
  AC_SUBST(sqlite_cflags)
  AC_SUBST(sqlite_ldadd)
  AC_SUBST(sqlite_ldflags)
  OLD_CFLAGS=${CFLAGS}
  OLD_LIBS=${LIBS}
  OLD_LD_LIBRARY_PATH=${LD_LIBRARY_PATH}
  CFLAGS="${CFLAGS} ${sqlite_cflags}"
  LIBS="${LIBS} ${sqlite_ldflags} ${sqlite_ldadd}"
  export LD_LIBRARY_PATH="${my_sqlitedir}/lib:${LD_LIBRARY_PATH}"
  AC_MSG_CHECKING([whether or not SQLite release number is ok])
  AC_RUN_IFELSE(
    [
      AC_LANG_SOURCE(
      [[
#include <sqlite3.h>
int main() { unsigned int uiVersion = sqlite3_libversion_number(); return (uiVersion >= 3007009) ? 0 : 1; }
      ]]
      )
    ],
    [sqlite_version_ok="1"],
    [sqlite_version_ok="0"],
    [sqlite_version_ok="2"]
  )
  if test ${sqlite_version_ok} -eq 1 ; then
    AC_MSG_RESULT([yes])
  elif test ${sqlite_version_ok} -eq 2 ; then
    AC_MSG_RESULT([skipped (cross-compiling)])
  else
    AC_MSG_RESULT([no (3.7.9 or higher required)])
    exit 1
  fi
  CFLAGS=${OLD_CFLAGS}
  LIBS=${OLD_LIBS}
  export LD_LIBRARY_PATH=${OLD_LD_LIBRARY_PATH}
fi

dnl ####################################################################
dnl #
dnl # Check for TAP. Do not replicate the code in this section without
dnl # analyzing the differences relative to other peer library checks.
dnl #
dnl ####################################################################

my_tapdir=""
my_tapdir_found="0"
my_tapdir_specified="0"

CheckTAPComponents()
{
  TAPDIR=${1}
  if test -d "${TAPDIR}" ; then
    INCLUDE_LIST="tap.h"
    for INCLUDE_FILE in ${INCLUDE_LIST} ; do
      if test ! -f "${TAPDIR}/include/${INCLUDE_FILE}" ; then
        return 1
      fi
    done
    my_tapdir=${TAPDIR}
    my_tapdir_found="1"
  else
    return 1
  fi
  return 0
}

AC_MSG_CHECKING([for TAP support])
AC_ARG_WITH(tap,
  [  --with-tap=DIR          Use TAP with includes and libs from [DIR]/include and [DIR]/lib],
  [
    my_tapdir_specified="1"
    CheckTAPComponents "${withval}"
  ],
  [
    for my_basedir in ${LIB_LIST} ; do
      for my_dir in "${my_basedir}" "${my_basedir}/tap" ; do
        CheckTAPComponents "${my_dir}" && break 2
      done
    done
  ]
)
if test ${my_tapdir_found} -eq 1 ; then
  AC_MSG_RESULT([${my_tapdir}])
  tap_cflags="-I${my_tapdir}/include"
  tap_ldadd="-ltap"
  tap_ldflags="-L${my_tapdir}/lib"
  AC_SUBST(tap_cflags)
  AC_SUBST(tap_ldadd)
  AC_SUBST(tap_ldflags)
  OLD_CFLAGS=${CFLAGS}
  OLD_LIBS=${LIBS}
  OLD_LD_LIBRARY_PATH=${LD_LIBRARY_PATH}
  CFLAGS="${CFLAGS} ${tap_cflags}"
  LIBS="${LIBS} ${tap_ldflags} ${tap_ldadd}"
  export LD_LIBRARY_PATH="${my_tapdir}/lib:${LD_LIBRARY_PATH}"
  AC_MSG_CHECKING([whether or not TAP release is ok])
  AC_RUN_IFELSE(
    [
      AC_LANG_SOURCE(
      [[
#include <tap.h>
int main() { close(1); plan_tests(1); diag("test"); ok(1 == 1, "1 == 1"); return exit_status(); }
      ]]
      )
    ],
    [tap_version_ok="1"],
    [tap_version_ok="0"],
    [tap_version_ok="2"]
  )
  if test ${tap_version_ok} -eq 1 ; then
    AC_MSG_RESULT([yes])
  elif test ${tap_version_ok} -eq 2 ; then
    AC_MSG_RESULT([skipped (cross-compiling)])
  else
    AC_MSG_RESULT([no (1.03 or higher required)])
    exit 1
  fi
  CFLAGS=${OLD_CFLAGS}
  LIBS=${OLD_LIBS}
  export LD_LIBRARY_PATH=${OLD_LD_LIBRARY_PATH}
else
  if test ${my_tapdir_specified} -eq 1 ; then
    AC_MSG_RESULT([missing, incomplete, or incorrect])
    echo
    echo "Unable to locate required TAP components. Use --with-tap=DIR to"
    echo "specify a known or different location. The files that must exist for"
    echo "this check to pass are:"
    echo
    echo "  DIR/include/tap.h"
    echo
    exit 1
  else
    AC_MSG_RESULT([skipped])
  fi
fi

dnl ####################################################################
dnl #
dnl # Check whether or not to enable debugging support.
dnl #
dnl ####################################################################

AC_MSG_CHECKING([whether to enable debugging support])
AC_ARG_ENABLE(debugging,
  [  --enable-debugging      Enable debugging support (disabled by default)],
  [
    case `echo ${enableval} | tr "A-Z" "a-z"` in
    yes)
      enable_debugging="1"
      ;;
    *)
      enable_debugging="0"
      ;;
    esac
  ],
  [ enable_debugging="0" ]
)
if test ${enable_debugging} -eq 1 ; then
  AC_MSG_RESULT([yes])
  CFLAGS="-g -DPATHWELL_DEBUG ${CFLAGS}"
else
  AC_MSG_RESULT([no])
fi

dnl ####################################################################
dnl #
dnl # Set security directory.
dnl #
dnl ####################################################################

AC_MSG_CHECKING([for a custom security dir])
AC_ARG_WITH(securedir,
  [  --with-securedir=DIR    Where to install PAMs @<:@default=$libdir/security@:>@],
  [
    case `echo ${withval} | tr "A-Z" "a-z"` in
    ""|n|no|y|ye|yes)
      SECUREDIR=""
      ;;
    *)
      SECUREDIR=${withval}
      ;;
    esac
  ],
  [
    SECUREDIR="${libdir}/security"
  ]
)
AC_MSG_RESULT([${SECUREDIR}])
if test -z "${SECUREDIR}" ; then
  AC_MSG_ERROR([*** security dir is invalid or missing ... try using --with-securedir=DIR])
fi
AC_SUBST(SECUREDIR)

dnl ####################################################################
dnl #
dnl # Create files.
dnl #
dnl ####################################################################

AC_CONFIG_FILES(
  [
    Makefile
    data/Makefile
    doc/Makefile
    doc/chkpw/Makefile
    doc/create-db/Makefile
    doc/export-db/Makefile
    doc/id2topology/Makefile
    doc/pam_pathwell/Makefile
    doc/pathwell_api/Makefile
    doc/pathwell_charsets/Makefile
    doc/pw2id/Makefile
    doc/pw2topology/Makefile
    doc/setuc/Makefile
    doc/topology2id/Makefile
    include/Makefile
    src/Makefile
    src/chkpw/Makefile
    src/create-db/Makefile
    src/export-db/Makefile
    src/id2topology/Makefile
    src/libpathwell/Makefile
    src/pam_pathwell/Makefile
    src/pw2id/Makefile
    src/pw2topology/Makefile
    src/setuc/Makefile
    src/topology2id/Makefile
    tests/Makefile
    tests/chkpw/Makefile
    tests/id2topology/Makefile
    tests/libpathwell/Makefile
    tests/pw2id/Makefile
    tests/pw2topology/Makefile
    tests/topology2id/Makefile
  ]
)

AC_CONFIG_HEADERS([include/config.h])

AC_CONFIG_LINKS(
  [
    include/pathwell.h:include/pathwell.h
    include/pathwell-pstdint.h:include/pstdint.h
    src/chkpw/pathwell-chkpw.c:src/chkpw/chkpw.c
    src/chkpw/pathwell-chkpw.h:src/chkpw/chkpw.h
    src/create-db/pathwell-create-db.c:src/create-db/create-db.c
    src/create-db/pathwell-create-db.h:src/create-db/create-db.h
    src/export-db/pathwell-export-db:src/export-db/export-db
    src/id2topology/pathwell-id2topology.c:src/id2topology/id2topology.c
    src/id2topology/pathwell-id2topology.h:src/id2topology/id2topology.h
    src/pw2id/pathwell-pw2id.c:src/pw2id/pw2id.c
    src/pw2id/pathwell-pw2id.h:src/pw2id/pw2id.h
    src/pw2topology/pathwell-pw2topology.c:src/pw2topology/pw2topology.c
    src/pw2topology/pathwell-pw2topology.h:src/pw2topology/pw2topology.h
    src/setuc/pathwell-setuc.c:src/setuc/setuc.c
    src/setuc/pathwell-setuc.h:src/setuc/setuc.h
    src/topology2id/pathwell-topology2id.c:src/topology2id/topology2id.c
    src/topology2id/pathwell-topology2id.h:src/topology2id/topology2id.h
  ]
)

AC_OUTPUT

