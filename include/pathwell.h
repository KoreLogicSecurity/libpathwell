/*-
 ***********************************************************************
 *
 * $Id: pathwell.h,v 1.107.2.5 2015/09/30 16:05:53 klm Exp $
 *
 ***********************************************************************
 *
 * Copyright 2013-2015 The PathWell Project, All Rights Reserved.
 *
 * This software, having been partly or wholly developed and/or
 * sponsored by KoreLogic, Inc., is hereby released under the terms
 * and conditions set forth in the project's "README.LICENSE" file.
 * For a list of all contributors and sponsors, please refer to the
 * project's "README.CREDITS" file.
 *
 ***********************************************************************
 */
#ifndef _PATHWELL_H_INCLUDED
#define _PATHWELL_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PATHWELL_DEBUG
  #include <assert.h>
  #define PATHWELL_ASSERT(condition) assert(condition)
#else
  #define PATHWELL_ASSERT(condition)
#endif

#ifdef HAVE_INTTYPES_H
  #include <inttypes.h>
#else
  #ifdef HAVE_STDINT_H
    #include <stdint.h>
  #else
    #include <pathwell-pstdint.h>
  #endif
#endif

#include <sqlite3.h>
#include <stddef.h>
#include <sys/types.h> // Note: This was added for mode_t.

#if defined(WIN32) && !defined(_MODE_T_) // Note: This is here for Visual Studio. MinGW defines _MODE_T_.
  typedef unsigned short mode_t;
#endif

/*-
 ***********************************************************************
 *
 * Macros
 *
 ***********************************************************************
 */
#define ER_OK 0
#define ER -1

#define PATHWELL_COMMON_BLACKLIST_FILE "/usr/share/PathWell/topology_blacklist.default"
#define PATHWELL_CUSTOM_BLACKLIST_FILE "/etc/PathWell/topology_blacklist.local"
#define PATHWELL_DB_BUNDLE_SIZE 10000
#define PATHWELL_DB_BUSY_TIMEOUT 2000
#define PATHWELL_DB_TABLE_NAME_MAX_SIZE 64
#define PATHWELL_ENCODING_BASENP1 0x00000001
#define PATHWELL_ENCODING_BITMASK 0x00000002
#define PATHWELL_MIN_TOKEN_SET_ID 1
#define PATHWELL_TOKEN_SET_ID_1 1
#define PATHWELL_TOKEN_SET_ID_2 2
#define PATHWELL_TOKEN_SET_ID_3 3
#define PATHWELL_TOKEN_SET_ID_4 4
#define PATHWELL_MAX_TOKEN_SET_ID 4
#define PATHWELL_TOKEN_SET_ID_1_TOKENS "dlus"
#define PATHWELL_TOKEN_SET_ID_2_TOKENS "dlusf"
#define PATHWELL_TOKEN_SET_ID_3_TOKENS "dlusfx"
#define PATHWELL_TOKEN_SET_ID_4_TOKENS "dlusfxH"
#define PATHWELL_TOKENS_REGEX "^(\\?[%s])(?!.*,\\1)(?:,(\\?[%s])(?!.*,\\2))*$"
#define PATHWELL_TOKENS_REGEX_SIZE 57 /* 57 = strlen("^(\\?[dlusfxH])(?!.*,\\1)(?:,(\\?[dlusfxH])(?!.*,\\2))*$") + 1 */

#define PATHWELL_MAX_LINE_SIZE 1024
#define PATHWELL_MAX_PASSWORD_SIZE 512
#define PATHWELL_MAX_TOPOLOGY_SIZE ((PATHWELL_MAX_PASSWORD_SIZE) * 2)

#define PATHWELL_DEFAULT_DB_FILE "/var/lib/PathWell/PathWell.db"
#define PATHWELL_DEFAULT_ENCODING PATHWELL_ENCODING_BASENP1
#define PATHWELL_DEFAULT_TOKEN_SET_ID 1

#define PATHWELL_MAX_32BIT_NUMBER_SIZE 11 /* 4294967295 */

#define PATHWELL_FALSE 0
#define PATHWELL_INDETERMINATE -1
#define PATHWELL_TRUE 1

#define PATHWELL_MAX_ID 0x7FFFFFFFFFFFFFFF
#define PATHWELL_MIN_BASENP1_ID 1
#define PATHWELL_MAX_BASENP1_ID PATHWELL_MAX_ID
#define PATHWELL_MIN_BITMASK_ID 0b10000100 // This is represented as base2 to expose the encoding scheme.
#define PATHWELL_MAX_BITMASK_ID PATHWELL_MAX_ID

#define PW_S_CHOMP_LEFT 1
#define PW_S_CHOMP_RIGHT 2
#define PW_S_CHOMP_BOTH 3

#define PATHWELL_COLON_C ':'
#define PATHWELL_COLON_S ":"
#define PATHWELL_DOT_C '.'
#define PATHWELL_DOT_S "."
#define PATHWELL_SLASH_S "/"
#define PATHWELL_SLASH_C '/'

/*-
 ***********************************************************************
 *
 * PathWell Structure Declarations
 *
 ***********************************************************************
 */
typedef struct _PW_D_CANNED_SQL
{
  const char         *pcName;
  const char         *pcCreateSql;
  const char         *pcDeleteSql;
} PW_D_CANNED_SQL;

typedef struct _PW_D_CONTEXT
{
  char               *pcError; // This member must be aligned with the same in PW_E_CONTEXT.
  char               *pcDbFile;
  char               *pcDbLock;
  int                 iDbFileFlags;
  int                 iDbLockFlags;
  int                 iValid;
  sqlite3            *psDbHandle;
} PW_D_CONTEXT;

typedef struct _PW_E_CONTEXT
{
  char               *pcError;
} PW_E_CONTEXT;

typedef struct _PW_T_CONTEXT
{
  char               *pcError; // This member must be aligned with the same in PW_E_CONTEXT.
  char               *pcPassword;
  char               *pcTopology;
  int                 iEncoding;
  int                 iTokenSet;
  int                 iValid;
  int64_t            *pi64Id;
} PW_T_CONTEXT;

typedef struct _PW_L_CONTEXT // This typedef must follow the one for PW_T_CONTEXT.
{
  char               *pcError; // This member must be aligned with the same in PW_E_CONTEXT.
  int                 iValid;
  PW_T_CONTEXT       *psPwTContextA;
  PW_T_CONTEXT       *psPwTContextB;
} PW_L_CONTEXT;

/*-
 ***********************************************************************
 *
 * PathWell Database Macros (PwD)
 *
 ***********************************************************************
 */
#define PwDSetError(psPwDContext, pcFormat, ...) PwESetError((PW_E_CONTEXT *)psPwDContext, pcFormat, __VA_ARGS__)

/*-
 ***********************************************************************
 *
 * PathWell Database Routines (PwD)
 *
 ***********************************************************************
 */
int                 PwDAcquireConnection(PW_D_CONTEXT *psPwDContext);
int                 PwDBindLibraryResources(PW_D_CONTEXT *psPwDContext, int iForce);
void                PwDClearError(PW_D_CONTEXT *psPwDContext);
int                 PwDClearUseCount(PW_D_CONTEXT *psPwDContext, PW_T_CONTEXT *psPwTContext);
int                 PwDContextIsValid(PW_D_CONTEXT *psPwDContext);
int                 PwDCreateSchema(PW_D_CONTEXT *psPwDContext);
int                 PwDCreateTable(PW_D_CONTEXT *psPwDContext, const char *pcName);
int                 PwDCreateTables(PW_D_CONTEXT *psPwDContext);
int                 PwDCreateTrigger(PW_D_CONTEXT *psPwDContext, const char *pcName);
int                 PwDCreateTriggers(PW_D_CONTEXT *psPwDContext);
int                 PwDDbDirectoryExists(char *pcDbDirectory);
int                 PwDDbFileExists(char *pcDbFile);
int                 PwDDecrementUseCount(PW_D_CONTEXT *psPwDContext, PW_T_CONTEXT *psPwTContext);
int                 PwDDeleteSchema(PW_D_CONTEXT *psPwDContext);
int                 PwDDeleteTable(PW_D_CONTEXT *psPwDContext, const char *pcName);
int                 PwDDeleteTables(PW_D_CONTEXT *psPwDContext);
int                 PwDDeleteTrigger(PW_D_CONTEXT *psPwDContext, const char *pcName);
int                 PwDDeleteTriggers(PW_D_CONTEXT *psPwDContext);
void                PwDFreeContext(PW_D_CONTEXT *psPwDContext);
int                 PwDFreeLibraryResources(PW_D_CONTEXT *psPwDContext, int iForce);
const char         *PwDGetError(PW_D_CONTEXT *psPwDContext);
int                *PwDGetSchemaVersion(PW_D_CONTEXT *psPwDContext);
unsigned int       *PwDGetUseCount(PW_D_CONTEXT *psPwDContext, PW_T_CONTEXT *psPwTContext);
int                 PwDIncrementUseCount(PW_D_CONTEXT *psPwDContext, PW_T_CONTEXT *psPwTContext);
int                 PwDInitializeDatabase(PW_D_CONTEXT *psPwDContext);
int                 PwDLoadBlacklistedTopologies(PW_D_CONTEXT *psPwDContext, char *pcFile);
PW_D_CONTEXT       *PwDNewContextFromParameters(char *pcDbFile, int iDbFileFlags, char *pcDbLock, int iDbLockFlags);
int                 PwDReleaseConnection(PW_D_CONTEXT *psPwDContext);
int                 PwDSetSchemaVersion(PW_D_CONTEXT *psPwDContext, int iVersion, int iForce);
int                 PwDSetUseCount(PW_D_CONTEXT *psPwDContext, PW_T_CONTEXT *psPwTContext, unsigned int uiUseCount);
int                 PwDTopologyIsBlacklisted(PW_D_CONTEXT *psPwDContext, char *pcTopology);
int                 PwDTopologyIsInUse(PW_D_CONTEXT *psPwDContext, PW_T_CONTEXT *psPwTContext);
int                 PwDVerifySchema(PW_D_CONTEXT *psPwDContext);

/*-
 ***********************************************************************
 *
 * PathWell Error Routines (PwE)
 *
 ***********************************************************************
 */
int                 PwESetError(PW_E_CONTEXT *psPwEContext, const char *pcFormat, ...);

/*-
 ***********************************************************************
 *
 * PathWell Lev Distance Macros (PwL)
 *
 ***********************************************************************
 */
#define PwLSetError(psPwLContext, pcFormat, ...) PwESetError((PW_E_CONTEXT *)psPwLContext, pcFormat, __VA_ARGS__)

/*-
 ***********************************************************************
 *
 * PathWell Lev Distance Routines (PwL)
 *
 ***********************************************************************
 */
int                 PwLCalcLevDistance(PW_L_CONTEXT *psPwLContext);
int                 PwLCheckLevDistance(PW_L_CONTEXT *psPwLContext, int iMinDist);
void                PwLClearError(PW_L_CONTEXT *psPwLContext);
int                 PwLContextIsValid(PW_L_CONTEXT *psPwLContext);
void                PwLFreeContext(PW_L_CONTEXT *psPwLContext);
const char         *PwLGetError(PW_L_CONTEXT *psPwLContext);
PW_T_CONTEXT       *PwLGetPwTContext(PW_L_CONTEXT *psPwLContext, char cMember);
PW_T_CONTEXT       *PwLGetPwTContextA(PW_L_CONTEXT *psPwLContext);
PW_T_CONTEXT       *PwLGetPwTContextB(PW_L_CONTEXT *psPwLContext);
PW_L_CONTEXT       *PwLNewContextFromParameters(PW_T_CONTEXT *psPwTContextA, PW_T_CONTEXT *psPwTContextB);
int                 PwLSetPwTContext(PW_L_CONTEXT *psPwLContext, PW_T_CONTEXT *psPwTContext, char cMember);
int                 PwLSetPwTContextA(PW_L_CONTEXT *psPwLContext, PW_T_CONTEXT *psPwTContext);
int                 PwLSetPwTContextB(PW_L_CONTEXT *psPwLContext, PW_T_CONTEXT *psPwTContext);

/*-
 ***********************************************************************
 *
 * PathWell Support Macros (PwS)
 *
 ***********************************************************************
 */
#define PwSMin2(iValueA, iValueB) ((iValueA) < (iValueB) ? (iValueA) : (iValueB))
#define PwSMin3(iValueA, iValueB, iValueC) PwSMin2(PwSMin2((iValueA), (iValueB)), (iValueC))

/*-
 ***********************************************************************
 *
 * PathWell Support Routines (PwS)
 *
 ***********************************************************************
 */
int                 PwSBaseNTo63Bit(char *pcValue, int64_t *pi64Value, int iBase);
char               *PwSChomp(char *pcString, char *pcChompSet, int iMode);
int                 PwSCreateDirectory(char *pcPath, mode_t tMode);
char               *PwSDirname(char *pcPath);
int                 PwSLevenshteinDistance(char *pcStringA, char *pcStringB);

/*-
 ***********************************************************************
 *
 * PathWell Topology Macros (PwT)
 *
 ***********************************************************************
 */
#define PwTSetError(psPwTContext, pcFormat, ...) PwESetError((PW_E_CONTEXT *)psPwTContext, pcFormat, __VA_ARGS__)

/*-
 ***********************************************************************
 *
 * PathWell Topology Routines (PwT)
 *
 ***********************************************************************
 */
int                 PwTBaseNp1IdToTopology(PW_T_CONTEXT *psPwTContext);
int                 PwTBitmaskIdToTopology(PW_T_CONTEXT *psPwTContext);
void                PwTClearError(PW_T_CONTEXT *psPwTContext);
int                 PwTContextIsValid(PW_T_CONTEXT *psPwTContext);
void                PwTFreeContext(PW_T_CONTEXT *psPwTContext);
int                 PwTGetEncoding(PW_T_CONTEXT *psPwTContext);
const char         *PwTGetError(PW_T_CONTEXT *psPwTContext);
int64_t             PwTGetId(PW_T_CONTEXT *psPwTContext);
const char         *PwTGetPassword(PW_T_CONTEXT *psPwTContext);
int                 PwTGetTokenSet(PW_T_CONTEXT *psPwTContext);
char               *PwTGetTokenSetTokens(PW_T_CONTEXT *psPwTContext);
const char         *PwTGetTopology(PW_T_CONTEXT *psPwTContext);
int                 PwTIdToTopology(PW_T_CONTEXT *psPwTContext);
int                 PwTIsPasswordInTokenSet(PW_T_CONTEXT *psPwTContext);
PW_T_CONTEXT       *PwTNewContext(void);
PW_T_CONTEXT       *PwTNewContextFromParameters(int iEncoding, int iTokenSet, char *pcPassword, char *pcTopology, int64_t *pi64Id);
int                 PwTPasswordToTopology(PW_T_CONTEXT *psPwTContext);
int                 PwTSetEncoding(PW_T_CONTEXT *psPwTContext, int iEncoding);
int                 PwTSetId(PW_T_CONTEXT *psPwTContext, int64_t i64Id);
int                 PwTSetIdFromString(PW_T_CONTEXT *psPwTContext, char *pcId);
int                 PwTSetPassword(PW_T_CONTEXT *psPwTContext, char *pcPassword);
int                 PwTSetTokenSet(PW_T_CONTEXT *psPwTContext, int iTokenSet);
int                 PwTSetTopology(PW_T_CONTEXT *psPwTContext, char *pcTopology);
int                 PwTTopologyToBaseNp1Id(PW_T_CONTEXT *psPwTContext);
int                 PwTTopologyToBitmaskId(PW_T_CONTEXT *psPwTContext);
int                 PwTTopologyToId(PW_T_CONTEXT *psPwTContext);
int                 PwTVerifyTopologyString(PW_T_CONTEXT *psPwTContext);

/*-
 ***********************************************************************
 *
 * PathWell Version Routines (PwV)
 *
 ***********************************************************************
 */
int                 PwVGetLibraryAge(void);
int                 PwVGetLibraryCurrent(void);
int                 PwVGetLibraryRevision(void);
const char         *PwVGetLibraryVersion(void);
int                 PwVGetModuleAge(void);
int                 PwVGetModuleCurrent(void);
int                 PwVGetModuleRevision(void);
const char         *PwVGetModuleVersion(void);
int                 PwVGetReleaseMajor(void);
int                 PwVGetReleaseMinor(void);
uint32_t            PwVGetReleaseNumber(void);
int                 PwVGetReleasePatch(void);
const char         *PwVGetReleaseString(void);

#ifdef __cplusplus
}
#endif

#endif /* !_PATHWELL_H_INCLUDED */
