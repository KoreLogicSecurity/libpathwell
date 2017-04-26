/*-
 ***********************************************************************
 *
 * $Id: chkpw.h,v 1.10 2017/04/18 22:12:50 klm Exp $
 *
 ***********************************************************************
 *
 * Copyright 2014-2017 The PathWell Project, All Rights Reserved.
 *
 * This software, having been partly or wholly developed and/or
 * sponsored by KoreLogic, Inc., is hereby released under the terms
 * and conditions set forth in the project's "README.LICENSE" file.
 * For a list of all contributors and sponsors, please refer to the
 * project's "README.CREDITS" file.
 *
 ***********************************************************************
 */
#ifndef _CHKPW_H_INCLUDED
#define _CHKPW_H_INCLUDED

/*-
 ***********************************************************************
 *
 * Macros
 *
 ***********************************************************************
 */
#define PROGRAM_NAME "pathwell-chkpw"

#define XER_OK 0
#define XER_Usage 1
#define XER_Bootstrap 2
#define XER_Setup 3
#define XER_Check 4

#define APP_AUTHTOK_ERR 1
#define APP_SERVICE_ERR 2

#define CHKPW_DEFAULT_SEED 0
#define CHKPW_SEED_MIN 1
#define CHKPW_SEED_MAX UINT_MAX

/*-
 ***********************************************************************
 *
 * Typedefs
 *
 ***********************************************************************
 */
typedef struct _PROPERTIES
{
  char               *pcDbFile;
  char               *pcEncoding;
  char               *pcNewPassword;
  char               *pcOldPassword;
  char               *pcTokenSet;
  int                 iBlacklist;
  int                 iBlacklisted;
  int                 iDeleteAllowed;
  int                 iEncoding;
  int                 iEnvironment;
  int                 iHintInfoLevel;
  int                 iHintPresLevel;
  int                 iIncrementUseCount;
  int                 iLeveled;
  int                 iMaxUse;
  int                 iMinLen;
  int                 iMinLev;
  int                 iTokenSet;
  PW_C_CONTEXT       *psPwCContext;
  PW_D_CONTEXT       *psPwDContext;
  PW_H_CONTEXT       *psPwHContext;
  PW_L_CONTEXT       *psPwLContext;
  PW_T_CONTEXT       *psNewPwTContext;
  PW_T_CONTEXT       *psOldPwTContext;
  unsigned int       *puiUseCount;
  unsigned int        uiSeed;
} PROPERTIES;

/*-
 ***********************************************************************
 *
 * Function Prototypes
 *
 ***********************************************************************
 */
void                AppCleanProperties(PROPERTIES *psProperties);
PROPERTIES         *AppNewProperties(void);
void                AppUsage(void);

#endif /* !_CHKPW_H_INCLUDED */
