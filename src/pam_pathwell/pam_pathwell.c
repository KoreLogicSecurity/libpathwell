/*-
 ***********************************************************************
 *
 * $Id: pam_pathwell.c,v 1.103 2017/04/25 16:21:19 klm Exp $
 *
 ***********************************************************************
 *
 * Copyright 2013-2017 The PathWell Project, All Rights Reserved.
 *
 * This software, having been partly or wholly developed and/or
 * sponsored by KoreLogic, Inc., is hereby released under the terms
 * and conditions set forth in the project's "README.LICENSE" file.
 * For a list of all contributors and sponsors, please refer to the
 * project's "README.CREDITS" file.
 *
 ***********************************************************************
 */
#include "all-includes.h"

/*-
 ***********************************************************************
 *
 * pam_sm_chauthtok
 *
 ***********************************************************************
 */
int
pam_sm_chauthtok(pam_handle_t *psPamHandle, int iPamFlags, int iArgumentCount, const char **ppcArgumentVector)
{
  char               *pcDbFile = PATHWELL_DEFAULT_DB_FILE;
  const char         *pcError = NULL;
  const char         *pcHint = NULL;
  const char         *pcNewAuthToken = NULL;
  const char         *pcOldAuthToken = NULL;
  const char         *pcUser = NULL;
  const char         *pcValue = NULL;
  int                 iBlacklist = PATHWELL_FALSE;
  int                 iBlacklisted = PATHWELL_FALSE;
  int                 iDebug = PATHWELL_FALSE;
  int                 iDelayed = PATHWELL_FALSE;
  int                 iEncoding = PATHWELL_DEFAULT_ENCODING;
  int                 iError = 0;
  int                 iHintInfoLevel = PATHWELL_HINT_INFO_LEVEL_1;
  int                 iHintPresLevel = PATHWELL_HINT_PRES_LEVEL_3;
  int                 iIndex = 0;
  int                 iInUse = PATHWELL_FALSE;
  int                 iLeveled = PATHWELL_FALSE;
  int                 iMaxUse = 1; // Note: This implies that leveling is enabled by default.
  int                 iMinLen = 0; // Note: This implies that length checks are disabled by default.
  int                 iMinLev = 0; // Note: This implies that Levenshtein checks are disabled by default.
  int                 iMode = PW_PAM_MODE_NOT_SET;
  int                 iOldPwTContextFabricated = PATHWELL_FALSE;
  int                 iReturnValue = PAM_SUCCESS;
  int                 iTokenSet = PATHWELL_DEFAULT_TOKEN_SET_ID;
  int                 iUseAuthToken = PATHWELL_FALSE;
  int                 iValueIndex = 0;
  int                 iValueLength = 0;
  PW_C_CONTEXT       *psPwCContext = NULL;
  PW_D_CONTEXT       *psPwDContext = NULL;
  PW_H_CONTEXT       *psPwHContext = NULL;
  PW_L_CONTEXT       *psPwLContext = NULL;
  PW_T_CONTEXT       *psNewPwTContext = NULL;
  PW_T_CONTEXT       *psOldPwTContext = NULL;
  unsigned int       *puiUseCount = NULL;

  /*-
   ***********************************************************************
   *
   * Conditionally log a checkpoint.
   *
   ***********************************************************************
   */
  if (iDebug)
  {
    pam_syslog
    (
      psPamHandle,
      LOG_DEBUG,
      "Release='%s'; Library='%s'; Module='%s'; PamFlags='0x%08x'; Mode='%s'; User='%s'; Status='done'; Reason='Checkpoint reached (module started).';",
      PwVGetReleaseString(),
      PwVGetLibraryVersion(),
      PwVGetModuleVersion(),
      iPamFlags,
      (iMode == PW_PAM_MODE_ENFORCE) ? "enforce" : "monitor",
      (pcUser == NULL) ? "(null)" : pcUser
    );
  }

  /*-
   ***********************************************************************
   *
   * Process module arguments.
   *
   ***********************************************************************
   */
  for (iIndex = 0; iIndex < iArgumentCount; iIndex++)
  {
    if (strcasecmp(ppcArgumentVector[iIndex], PW_PAM_OPT_BLACKLIST) == 0)
    {
      iBlacklist = PATHWELL_TRUE;
    }
    else if (strncasecmp(ppcArgumentVector[iIndex], PW_PAM_OPT_DB, strlen(PW_PAM_OPT_DB)) == 0)
    {
      pcDbFile = (char *)&ppcArgumentVector[iIndex][strlen(PW_PAM_OPT_DB)];
    }
    else if (strcasecmp(ppcArgumentVector[iIndex], PW_PAM_OPT_DEBUG) == 0)
    {
      iDebug = PATHWELL_TRUE;
    }
    else if (strcasecmp(ppcArgumentVector[iIndex], PW_PAM_OPT_DELAYED) == 0)
    {
      iDelayed = PATHWELL_TRUE;
    }
    else if (strncasecmp(ppcArgumentVector[iIndex], PW_PAM_OPT_ENCODING, strlen(PW_PAM_OPT_ENCODING)) == 0)
    {
      pcValue = &ppcArgumentVector[iIndex][strlen(PW_PAM_OPT_ENCODING)];
      if (strcasecmp(pcValue, "baseN+1") == 0 || strcasecmp(pcValue, "baseNp1") == 0)
      {
        iEncoding = PATHWELL_ENCODING_BASENP1;
      }
      else if (strcasecmp(pcValue, "bitmask") == 0)
      {
        iEncoding = PATHWELL_ENCODING_BITMASK;
      }
      else
      {
        pcError = "The \"encoding\" option must be one of \"baseN+1\" or \"bitmask\".";
        iReturnValue = PAM_SERVICE_ERR;
        goto USAGE_CLEANUP;
      }
    }
    else if (strncasecmp(ppcArgumentVector[iIndex], PW_PAM_OPT_HINTINFOLEVEL, strlen(PW_PAM_OPT_HINTINFOLEVEL)) == 0)
    {
      pcValue = &ppcArgumentVector[iIndex][strlen(PW_PAM_OPT_HINTINFOLEVEL)];
      if (strcasecmp(pcValue, "1") == 0)
      {
        iHintInfoLevel = PATHWELL_HINT_INFO_LEVEL_1;
      }
      else if (strcasecmp(pcValue, "2") == 0)
      {
        iHintInfoLevel = PATHWELL_HINT_INFO_LEVEL_2;
      }
      else if (strcasecmp(pcValue, "3") == 0)
      {
        iHintInfoLevel = PATHWELL_HINT_INFO_LEVEL_3;
      }
      else if (strcasecmp(pcValue, "4") == 0)
      {
        iHintInfoLevel = PATHWELL_HINT_INFO_LEVEL_4;
      }
      else
      {
        pcError = "The \"hintinfolevel\" option must be an integer in the range [1-4].";
        iReturnValue = PAM_SERVICE_ERR;
        goto USAGE_CLEANUP;
      }
    }
    else if (strncasecmp(ppcArgumentVector[iIndex], PW_PAM_OPT_HINTPRESLEVEL, strlen(PW_PAM_OPT_HINTPRESLEVEL)) == 0)
    {
      pcValue = &ppcArgumentVector[iIndex][strlen(PW_PAM_OPT_HINTPRESLEVEL)];
      if (strcasecmp(pcValue, "1") == 0)
      {
        iHintPresLevel = PATHWELL_HINT_PRES_LEVEL_1;
      }
      else if (strcasecmp(pcValue, "2") == 0)
      {
        iHintPresLevel = PATHWELL_HINT_PRES_LEVEL_2;
      }
      else if (strcasecmp(pcValue, "3") == 0)
      {
        iHintPresLevel = PATHWELL_HINT_PRES_LEVEL_3;
      }
      else
      {
        pcError = "The \"hintpreslevel\" option must be an integer in the range [1-3].";
        iReturnValue = PAM_SERVICE_ERR;
        goto USAGE_CLEANUP;
      }
    }
    else if (strncasecmp(ppcArgumentVector[iIndex], PW_PAM_OPT_MAXUSE, strlen(PW_PAM_OPT_MAXUSE)) == 0)
    {
      pcValue = &ppcArgumentVector[iIndex][strlen(PW_PAM_OPT_MAXUSE)];
      iValueLength = strlen(pcValue);
      for (iValueIndex = 0; iValueIndex < iValueLength; iValueIndex++)
      {
        if (!isdigit((int)pcValue[iValueIndex]))
        {
          break;
        }
      }
      if (iValueLength < 1 || iValueIndex != iValueLength)
      {
        pcError = "The \"maxuse\" option must be an integer greater than or equal to zero.";
        iReturnValue = PAM_SERVICE_ERR;
        goto USAGE_CLEANUP;
      }
      iMaxUse = atoi(pcValue);
    }
    else if (strncasecmp(ppcArgumentVector[iIndex], PW_PAM_OPT_MINLEN, strlen(PW_PAM_OPT_MINLEN)) == 0)
    {
      pcValue = &ppcArgumentVector[iIndex][strlen(PW_PAM_OPT_MINLEN)];
      iValueLength = strlen(pcValue);
      for (iValueIndex = 0; iValueIndex < iValueLength; iValueIndex++)
      {
        if (!isdigit((int)pcValue[iValueIndex]))
        {
          break;
        }
      }
      if (iValueLength < 1 || iValueIndex != iValueLength)
      {
        pcError = "The \"minlen\" option must be an integer greater than or equal to zero.";
        iReturnValue = PAM_SERVICE_ERR;
        goto USAGE_CLEANUP;
      }
      iMinLen = atoi(pcValue);
    }
    else if (strncasecmp(ppcArgumentVector[iIndex], PW_PAM_OPT_MINLEV, strlen(PW_PAM_OPT_MINLEV)) == 0)
    {
      pcValue = &ppcArgumentVector[iIndex][strlen(PW_PAM_OPT_MINLEV)];
      iValueLength = strlen(pcValue);
      for (iValueIndex = 0; iValueIndex < iValueLength; iValueIndex++)
      {
        if (!isdigit((int)pcValue[iValueIndex]))
        {
          break;
        }
      }
      if (iValueLength < 1 || iValueIndex != iValueLength)
      {
        pcError = "The \"minlev\" option must be an integer greater than or equal to zero.";
        iReturnValue = PAM_SERVICE_ERR;
        goto USAGE_CLEANUP;
      }
      iMinLev = atoi(pcValue);
    }
    else if (strncasecmp(ppcArgumentVector[iIndex], PW_PAM_OPT_MODE, strlen(PW_PAM_OPT_MODE)) == 0)
    {
      pcValue = &ppcArgumentVector[iIndex][strlen(PW_PAM_OPT_MODE)];
      if (strcasecmp(pcValue, "enforce") == 0)
      {
        iMode = PW_PAM_MODE_ENFORCE;
      }
      else if (strcasecmp(pcValue, "monitor") == 0)
      {
        iMode = PW_PAM_MODE_MONITOR;
      }
      else
      {
        pcError = "The \"mode\" argument must be one of \"monitor\" or \"enforce\".";
        iReturnValue = PAM_SERVICE_ERR;
        goto USAGE_CLEANUP;
      }
    }
    else if (strncasecmp(ppcArgumentVector[iIndex], PW_PAM_OPT_TOKENSET, strlen(PW_PAM_OPT_TOKENSET)) == 0)
    {
      pcValue = &ppcArgumentVector[iIndex][strlen(PW_PAM_OPT_TOKENSET)];
      if (strcasecmp(pcValue, "1") == 0)
      {
        iTokenSet = PATHWELL_TOKEN_SET_ID_1;
      }
      else if (strcasecmp(pcValue, "2") == 0)
      {
        iTokenSet = PATHWELL_TOKEN_SET_ID_2;
      }
      else if (strcasecmp(pcValue, "3") == 0)
      {
        iTokenSet = PATHWELL_TOKEN_SET_ID_3;
      }
      else if (strcasecmp(pcValue, "4") == 0)
      {
        iTokenSet = PATHWELL_TOKEN_SET_ID_4;
      }
      else
      {
        pcError = "The \"tokenset\" option must be an integer in the range [1-4].";
        iReturnValue = PAM_SERVICE_ERR;
        goto USAGE_CLEANUP;
      }
    }
    else if (strcasecmp(ppcArgumentVector[iIndex], PW_PAM_OPT_USE_AUTHTOK) == 0)
    {
      iUseAuthToken = PATHWELL_TRUE;
    }
    else
    {
      pcError = "One or more invalid or unsupported options were specified. Refer to the documentation for a list of supported options and the correct usage syntax.";
      iReturnValue = PAM_SERVICE_ERR;
      goto USAGE_CLEANUP;
    }
  }

  /*-
   ***********************************************************************
   *
   * If the mode argument is not set or valid, throw an error.
   *
   ***********************************************************************
   */
  switch (iMode)
  {
  case PW_PAM_MODE_ENFORCE:
  case PW_PAM_MODE_MONITOR:
    break;
  case PW_PAM_MODE_NOT_SET:
    pcError = "The \"mode\" argument must be one of \"monitor\" or \"enforce\".";
    iReturnValue = PAM_SERVICE_ERR;
    goto USAGE_CLEANUP;
    break;
  default:
    pcError = "Invalid mode. That should not happen.";
    iReturnValue = PAM_SERVICE_ERR;
    goto USAGE_CLEANUP;
    break;
  }

USAGE_CLEANUP:
  /*-
   *******************************************************************
   *
   * Clean up.
   *
   *******************************************************************
   */
  if (pcError != NULL)
  {
    pam_syslog
    (
      psPamHandle,
      LOG_DEBUG,
      "Release='%s'; Library='%s'; Module='%s'; PamFlags='0x%08x'; Mode='%s'; User='%s'; Status='fail'; Reason='%s';",
      PwVGetReleaseString(),
      PwVGetLibraryVersion(),
      PwVGetModuleVersion(),
      iPamFlags,
      (iMode == PW_PAM_MODE_ENFORCE) ? "enforce" : (iMode == PW_PAM_MODE_MONITOR) ? "monitor" : "",
      (pcUser == NULL) ? "(null)" : pcUser,
      pcError
    );
    return iReturnValue;
  }

  /*-
   ***********************************************************************
   *
   * Conditionally log a checkpoint.
   *
   ***********************************************************************
   */
  if (iDebug)
  {
    pam_syslog
    (
      psPamHandle,
      LOG_DEBUG,
      "Release='%s'; Library='%s'; Module='%s'; PamFlags='0x%08x'; Mode='%s'; User='%s'; Status='done'; Reason='Checkpoint reached (arguments processed).';",
      PwVGetReleaseString(),
      PwVGetLibraryVersion(),
      PwVGetModuleVersion(),
      iPamFlags,
      (iMode == PW_PAM_MODE_ENFORCE) ? "enforce" : "monitor",
      (pcUser == NULL) ? "(null)" : pcUser
    );
  }

  /*-
   ***********************************************************************
   *
   * Conditionally perform any preliminary module checks.
   *
   ***********************************************************************
   */
  if (iPamFlags & PAM_PRELIM_CHECK)
  {
    /*-
     *******************************************************************
     *
     * Conditionally make sure the database exists and that its schema
     * checks out. If it does not exist, create and initialize it and
     * log a warning message; if that should fail, simply bail out.
     *
     *******************************************************************
     */
    if (PwDDbFileExists(pcDbFile))
    {
      psPwDContext = PwDNewContextFromParameters(pcDbFile, SQLITE_OPEN_READWRITE, NULL, 0);
      if (!PwDContextIsValid(psPwDContext))
      {
        pcError = PwDGetError(psPwDContext);
        iReturnValue = PAM_SERVICE_ERR;
        goto PRELIM_CLEANUP;
      }
      iError = PwDVerifySchema(psPwDContext);
      if (iError != ER_OK)
      {
        pcError = PwDGetError(psPwDContext);
        iReturnValue = PAM_SERVICE_ERR;
        goto PRELIM_CLEANUP;
      }
    }
    else
    {
      psPwDContext = PwDNewContextFromParameters(pcDbFile, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL, 0);
      if (!PwDContextIsValid(psPwDContext))
      {
        pcError = PwDGetError(psPwDContext);
        iReturnValue = PAM_SERVICE_ERR;
        goto PRELIM_CLEANUP;
      }
      iError = PwDInitializeDatabase(psPwDContext);
      if (iError != ER_OK)
      {
        pcError = PwDGetError(psPwDContext);
        iReturnValue = PAM_SERVICE_ERR;
        goto PRELIM_CLEANUP;
      }
      pam_syslog
      (
        psPamHandle,
        LOG_DEBUG,
        "Release='%s'; Library='%s'; Module='%s'; PamFlags='0x%08x'; Mode='%s'; User='%s'; Status='pass'; Reason='PathWell database created.';",
        PwVGetReleaseString(),
        PwVGetLibraryVersion(),
        PwVGetModuleVersion(),
        iPamFlags,
        (iMode == PW_PAM_MODE_ENFORCE) ? "enforce" : "monitor",
        (pcUser == NULL) ? "(null)" : pcUser
      );
    }

PRELIM_CLEANUP:
    /*-
     *******************************************************************
     *
     * Clean up.
     *
     *******************************************************************
     */
    if (pcError != NULL)
    {
      pam_syslog
      (
        psPamHandle,
        LOG_DEBUG,
        "Release='%s'; Library='%s'; Module='%s'; PamFlags='0x%08x'; Mode='%s'; User='%s'; Status='fail'; Reason='%s';",
        PwVGetReleaseString(),
        PwVGetLibraryVersion(),
        PwVGetModuleVersion(),
        iPamFlags,
        (iMode == PW_PAM_MODE_ENFORCE) ? "enforce" : "monitor",
        (pcUser == NULL) ? "(null)" : pcUser,
        pcError
      );
    }
    else
    {
      if (iDebug)
      {
        pam_syslog
        (
          psPamHandle,
          LOG_DEBUG,
          "Release='%s'; Library='%s'; Module='%s'; PamFlags='0x%08x'; Mode='%s'; User='%s'; Status='done'; Reason='Checkpoint reached (preliminary checks completed).';",
          PwVGetReleaseString(),
          PwVGetLibraryVersion(),
          PwVGetModuleVersion(),
          iPamFlags,
          (iMode == PW_PAM_MODE_ENFORCE) ? "enforce" : "monitor",
          (pcUser == NULL) ? "(null)" : pcUser
        );
      }
    }
    PwDFreeContext(&psPwDContext);

    return iReturnValue;
  }

  /*-
   ***********************************************************************
   *
   * Conditionally acquire, verify, and update the user's password.
   *
   ***********************************************************************
   */
  if (iPamFlags & PAM_UPDATE_AUTHTOK)
  {
    /*-
     *********************************************************************
     *
     * Conditionally acquire a database context/connection.
     *
     *********************************************************************
     */
    psPwDContext = PwDNewContextFromParameters(pcDbFile, SQLITE_OPEN_READWRITE, NULL, 0);
    if (PwDContextIsValid(psPwDContext) != PATHWELL_TRUE)
    {
      pcError = PwDGetError(psPwDContext);
      iReturnValue = PAM_SERVICE_ERR;
      goto UPDATE_CLEANUP;
    }

    /*-
     *******************************************************************
     *
     * Acquire the user's name and old password. Note that while the
     * user's name should always be defined, the password may or may
     * not be defined. One case where this is true is when the user's
     * password is being set by the superuser. Another case is when
     * user has no password.
     *
     *******************************************************************
     */
    iError = pam_get_item(psPamHandle, PAM_USER, (const void **)&pcUser);
    if (iError != PAM_SUCCESS || pcUser == NULL)
    {
      pcError = "Failed to obtain username via pam_get_item().";
      iReturnValue = (pcUser == NULL) ? PAM_USER_UNKNOWN : iError;
      goto UPDATE_CLEANUP;
    }
    iError = pam_get_item(psPamHandle, PAM_OLDAUTHTOK, (const void **)&pcOldAuthToken);
    if (iError != PAM_SUCCESS)
    {
      pcError = "Failed to obtain old password via pam_get_item().";
      iReturnValue = iError;
      goto UPDATE_CLEANUP;
    }
    if (pcOldAuthToken == NULL)
    {
      /* Empty. */
    }

    /*-
     *******************************************************************
     *
     * Conditionally acquire/verify the user's new password. If it's
     * already defined, assume that it's also been verified. Note that
     * the new password must be defined if use_authtok option is set.
     *
     *******************************************************************
     */
    iError = pam_get_item(psPamHandle, PAM_AUTHTOK, (const void **)&pcNewAuthToken);
    if (iError != PAM_SUCCESS)
    {
      pcError = "Failed to obtain new password via pam_get_item().";
      iReturnValue = iError;
      goto UPDATE_CLEANUP;
    }
    if (pcNewAuthToken == NULL)
    {
      if (iUseAuthToken == PATHWELL_TRUE)
      {
        pcError = "The new password must be defined by a previously stacked module when the \"use_authtok\" option is enabled.";
        iReturnValue = PAM_AUTHTOK_RECOVERY_ERR;
        goto UPDATE_CLEANUP;
      }
      iError = pam_get_authtok_noverify(psPamHandle, &pcNewAuthToken, NULL);
      if (iError != PAM_SUCCESS)
      {
        pcError = "Failed to obtain new password via pam_get_authtok_noverify().";
        iReturnValue = iError;
        goto UPDATE_CLEANUP;
      }
      if (pcNewAuthToken == NULL)
      {
        pcError = "The new password (1st entry) is not defined, so there is nothing that can be done.";
        iReturnValue = PAM_AUTHTOK_ERR;
        goto UPDATE_CLEANUP;
      }
      iError = pam_get_authtok_verify(psPamHandle, &pcNewAuthToken, NULL);
      if (iError != PAM_SUCCESS)
      {
        pcError = "Failed to verify new password via pam_get_authtok_verify().";
        iReturnValue = iError;
        goto UPDATE_CLEANUP;
      }
      if (pcNewAuthToken == NULL)
      {
        pcError = "The new password (2nd entry) is not defined, so there is nothing that can be done.";
        iReturnValue = PAM_AUTHTOK_ERR;
        goto UPDATE_CLEANUP;
      }
    }

    /*-
     *******************************************************************
     *
     * Convert the old password to a topology and ID. In the case where
     * the old password is not defined, a context representing an empty
     * string is created. Note that a valid context is required for the
     * subsequent password checks that are to be performed.
     *
     *******************************************************************
     */
    if (pcOldAuthToken != NULL)
    {
      psOldPwTContext = PwTNewContextFromParameters(iEncoding, iTokenSet, (char *)pcOldAuthToken, NULL, NULL);
      if (!PwTContextIsValid(psOldPwTContext))
      {
        pcError = PwTGetError(psOldPwTContext);
        iReturnValue = PAM_SERVICE_ERR;
        goto UPDATE_CLEANUP;
      }
      iError = PwTPasswordToTopology(psOldPwTContext); // Note: This is considered an authentication token manipulation.
      if (iError != ER_OK)
      {
        pcError = PwTGetError(psOldPwTContext);
        iReturnValue = PAM_AUTHTOK_ERR;
        goto UPDATE_CLEANUP;
      }
      iError = PwTTopologyToId(psOldPwTContext); // Note: This is considered an authentication token manipulation.
      if (iError != ER_OK)
      {
        pcError = PwTGetError(psOldPwTContext);
        iReturnValue = PAM_AUTHTOK_ERR;
        goto UPDATE_CLEANUP;
      }
    }
    else
    {
      psOldPwTContext = PwTNewContextFromParameters(iEncoding, iTokenSet, (char *)"", "?z", 0);
      if (!PwTContextIsValid(psOldPwTContext))
      {
        pcError = PwTGetError(psOldPwTContext);
        iReturnValue = PAM_SERVICE_ERR;
        goto UPDATE_CLEANUP;
      }
      iOldPwTContextFabricated = PATHWELL_TRUE;
    }

    /*-
     *******************************************************************
     *
     * Convert the new password to a topology and ID. In the case where
     * the new password is not defined and code execution arrives here,
     * abort.
     *
     *******************************************************************
     */
    if (pcNewAuthToken != NULL)
    {
      psNewPwTContext = PwTNewContextFromParameters(iEncoding, iTokenSet, (char *)pcNewAuthToken, NULL, NULL);
      if (!PwTContextIsValid(psNewPwTContext))
      {
        pcError = PwTGetError(psNewPwTContext);
        iReturnValue = PAM_SERVICE_ERR;
        goto UPDATE_CLEANUP;
      }
      iError = PwTPasswordToTopology(psNewPwTContext); // Note: This is considered an authentication token manipulation.
      if (iError != ER_OK)
      {
        pcError = PwTGetError(psNewPwTContext);
        iReturnValue = PAM_AUTHTOK_ERR;
        goto UPDATE_CLEANUP;
      }
      iError = PwTTopologyToId(psNewPwTContext); // Note: This is considered an authentication token manipulation.
      if (iError != ER_OK)
      {
        pcError = PwTGetError(psNewPwTContext);
        iReturnValue = PAM_AUTHTOK_ERR;
        goto UPDATE_CLEANUP;
      }
    }
    else
    {
      pcError = "The new password is not defined. That should not happen.";
      iReturnValue = PAM_SERVICE_ERR;
      goto UPDATE_CLEANUP;
    }

    /*-
     *******************************************************************
     *
     * Conditionally perform PathWell checks (superuser excluded). 
     *
     *******************************************************************
     */
    if (iMode == PW_PAM_MODE_ENFORCE && getuid() != 0)
    {
      psPwCContext = PwCNewContextFromParameters(psPwDContext, psOldPwTContext, psNewPwTContext, iMaxUse, iMinLen, iMinLev, iBlacklist);
      if (PwCContextIsValid(psPwCContext) != PATHWELL_TRUE)
      {
        pcError = PwCGetError(psPwCContext);
        iReturnValue = PAM_SERVICE_ERR;
        goto UPDATE_CLEANUP;
      }
      if (PwCAllCheck(psPwCContext) != ER_OK)
      {
        pcError = PwCGetError(psPwCContext);
        iReturnValue = PAM_SERVICE_ERR;
        goto UPDATE_CLEANUP;
      }
      if (PwCGetCheckCode(psPwCContext) != PATHWELL_ALLCHECK_PASS)
      {
        psPwHContext = PwHNewContextFromParameters
        (
          PATHWELL_HINT_MOD_DELETE_ALLOWED_DEFAULT,
          iHintInfoLevel,
          iHintPresLevel,
          PATHWELL_ENV_DEFAULT,
          0
        );
        if (PwHContextIsValid(psPwHContext) != PATHWELL_TRUE)
        {
          pcError = PwHGetError(psPwHContext);
          iReturnValue = PAM_SERVICE_ERR;
          goto UPDATE_CLEANUP;
        }
        if (PwHGenHint(psPwHContext, psPwCContext) != ER_OK)
        {
          pcError = PwHGetError(psPwHContext);
          iReturnValue = PAM_SERVICE_ERR;
          goto UPDATE_CLEANUP;
        }
        pcHint = PwHGetHint(psPwHContext);
        iReturnValue = PAM_AUTHTOK_ERR;
        goto UPDATE_CLEANUP;
      }
    }

    /*-
     *******************************************************************
     *
     * Conditionally increment the use count for the topology being
     * tracked. Normally, the topology of the new password is used.
     * If the 'delayed' option has been enabled, however, the topology
     * of the old password (if provided by the caller) is incremented
     * instead. Note that nothing is incremented if the requisite
     * context is undefined or fabricated.
     *
     *******************************************************************
     */
    if (iMode == PW_PAM_MODE_MONITOR)
    {
      PW_T_CONTEXT *psPwTContext = NULL;
      if (iDelayed)
      {
        if (psOldPwTContext != NULL && iOldPwTContextFabricated == PATHWELL_FALSE)
	{
          psPwTContext = psOldPwTContext;
	}
      }
      else
      {
        if (psNewPwTContext != NULL)
        {
          psPwTContext = psNewPwTContext;
        }
      }
      if (psPwTContext != NULL)
      {
        iError = PwDIncrementUseCount(psPwDContext, psPwTContext);
        if (iError != ER_OK)
        {
          pcError = PwDGetError(psPwDContext);
          iReturnValue = PAM_SERVICE_ERR;
          goto UPDATE_CLEANUP;
        }
      }
    }

UPDATE_CLEANUP:
    /*-
     *******************************************************************
     *
     * Clean up.
     *
     *******************************************************************
     */
    if (pcError != NULL || pcHint != NULL)
    {
      pam_syslog
      (
        psPamHandle,
        LOG_DEBUG,
        "Release='%s'; Library='%s'; Module='%s'; PamFlags='0x%08x'; Mode='%s'; User='%s'; Status='fail'; Reason='%s';",
        PwVGetReleaseString(),
        PwVGetLibraryVersion(),
        PwVGetModuleVersion(),
        iPamFlags,
        (iMode == PW_PAM_MODE_ENFORCE) ? "enforce" : "monitor",
        (pcUser == NULL) ? "(null)" : pcUser,
        (pcHint != NULL) ? "Password rejected." : pcError
      );
      if (pcHint != NULL)
      {
        pam_error(psPamHandle, "%s: %s", MODULE_NAME, pcHint);
      }
      if (pcNewAuthToken != NULL)
      {
        pam_set_item(psPamHandle, PAM_AUTHTOK, NULL);
      }
    }
    else
    {
      pam_syslog
      (
        psPamHandle,
        LOG_DEBUG,
        "Release='%s'; Library='%s'; Module='%s'; PamFlags='0x%08x'; Mode='%s'; User='%s'; Status='pass'; Reason='%s';",
        PwVGetReleaseString(),
        PwVGetLibraryVersion(),
        PwVGetModuleVersion(),
        iPamFlags,
        (iMode == PW_PAM_MODE_ENFORCE) ? "enforce" : "monitor",
        (pcUser == NULL) ? "(null)" : pcUser,
        "Password accepted."
      );
      if (iDebug)
      {
        pam_syslog
        (
          psPamHandle,
          LOG_DEBUG,
          "Release='%s'; Library='%s'; Module='%s'; PamFlags='0x%08x'; Mode='%s'; User='%s'; Status='done'; Reason='Checkpoint reached (authentication token update completed).';",
          PwVGetReleaseString(),
          PwVGetLibraryVersion(),
          PwVGetModuleVersion(),
          iPamFlags,
          (iMode == PW_PAM_MODE_ENFORCE) ? "enforce" : "monitor",
          (pcUser == NULL) ? "(null)" : pcUser
        );
      }
    }
    PwCFreeContext(&psPwCContext);
    PwDFreeContext(&psPwDContext);
    PwHFreeContext(&psPwHContext);
    PwLFreeContext(&psPwLContext);
    PwTFreeContext(&psNewPwTContext);
    PwTFreeContext(&psOldPwTContext);
    if (puiUseCount != NULL)
    {
      free(puiUseCount);
    }

    return iReturnValue;
  }

  return PAM_IGNORE;
}
