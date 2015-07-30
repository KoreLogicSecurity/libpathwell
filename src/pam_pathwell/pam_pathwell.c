/*-
 ***********************************************************************
 *
 * $Id: pam_pathwell.c,v 1.68 2013/11/04 22:37:46 klm Exp $
 *
 ***********************************************************************
 *
 * Copyright 2013-2013 The PathWell Project, All Rights Reserved.
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
  const char         *pcNewAuthToken = NULL;
  const char         *pcOldAuthToken = NULL;
  const char         *pcUser = NULL;
  const char         *pcValue = NULL;
  int                 iBlacklist = PATHWELL_FALSE;
  int                 iBlacklisted = PATHWELL_FALSE;
  int                 iDbRequired = PATHWELL_FALSE;
  int                 iDebug = PATHWELL_FALSE;
  int                 iEncoding = PATHWELL_DEFAULT_ENCODING;
  int                 iError = 0;
  int                 iIndex = 0;
  int                 iInUse = PATHWELL_FALSE;
  int                 iLeveled = PATHWELL_FALSE;
  int                 iMaxUse = 1; // Note: This implies that leveling is enabled by default.
  int                 iMinLen = 0;
  int                 iMinLev = 0;
  int                 iMode = PW_PAM_MODE_NOT_SET;
  int                 iReturnValue = PAM_SUCCESS;
  int                 iTokenSet = PATHWELL_DEFAULT_TOKEN_SET_ID;
  int                 iUseAuthToken = PATHWELL_FALSE;
  int                 iValueIndex = 0;
  int                 iValueLength = 0;
  PW_D_CONTEXT       *psPwDContext = NULL;
  PW_L_CONTEXT       *psPwLContext = NULL;
  PW_T_CONTEXT       *psNewPwTContext = NULL;
  PW_T_CONTEXT       *psOldPwTContext = NULL;
  unsigned int       *puiUseCount = NULL;

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
        pam_syslog
        (
          psPamHandle,
          LOG_ERR,
          "Release='%s'; Library='%s'; Module='%s'; PamFlags='0x%08x'; Mode='%s'; User='%s'; Error='%s';",
          PwVGetReleaseString(),
          PwVGetLibraryVersion(),
          PwVGetModuleVersion(),
          iPamFlags,
          (iMode == PW_PAM_MODE_ENFORCE) ? "enforce" : (iMode == PW_PAM_MODE_MONITOR) ? "monitor" : "",
          (pcUser == NULL) ? "(null)" : pcUser,
          pcError
        );
        return PAM_SERVICE_ERR;
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
        pam_syslog
        (
          psPamHandle,
          LOG_ERR,
          "Release='%s'; Library='%s'; Module='%s'; PamFlags='0x%08x'; Mode='%s'; User='%s'; Error='%s';",
          PwVGetReleaseString(),
          PwVGetLibraryVersion(),
          PwVGetModuleVersion(),
          iPamFlags,
          (iMode == PW_PAM_MODE_ENFORCE) ? "enforce" : (iMode == PW_PAM_MODE_MONITOR) ? "monitor" : "",
          (pcUser == NULL) ? "(null)" : pcUser,
          pcError
        );
        return PAM_SERVICE_ERR;
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
        pam_syslog
        (
          psPamHandle,
          LOG_ERR,
          "Release='%s'; Library='%s'; Module='%s'; PamFlags='0x%08x'; Mode='%s'; User='%s'; Error='%s';",
          PwVGetReleaseString(),
          PwVGetLibraryVersion(),
          PwVGetModuleVersion(),
          iPamFlags,
          (iMode == PW_PAM_MODE_ENFORCE) ? "enforce" : (iMode == PW_PAM_MODE_MONITOR) ? "monitor" : "",
          (pcUser == NULL) ? "(null)" : pcUser,
          pcError
        );
        return PAM_SERVICE_ERR;
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
        pam_syslog
        (
          psPamHandle,
          LOG_DEBUG,
          "Release='%s'; Library='%s'; Module='%s'; PamFlags='0x%08x'; Mode='%s'; User='%s'; Error='%s';",
          PwVGetReleaseString(),
          PwVGetLibraryVersion(),
          PwVGetModuleVersion(),
          iPamFlags,
          (iMode == PW_PAM_MODE_ENFORCE) ? "enforce" : (iMode == PW_PAM_MODE_MONITOR) ? "monitor" : "",
          (pcUser == NULL) ? "(null)" : pcUser,
          pcError
        );
        return PAM_SERVICE_ERR;
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
        pcError = "The \"mode\" option must be one of \"monitor\" or \"enforce\".";
        pam_syslog
        (
          psPamHandle,
          LOG_DEBUG,
          "Release='%s'; Library='%s'; Module='%s'; PamFlags='0x%08x'; Mode='%s'; User='%s'; Error='%s';",
          PwVGetReleaseString(),
          PwVGetLibraryVersion(),
          PwVGetModuleVersion(),
          iPamFlags,
          (iMode == PW_PAM_MODE_ENFORCE) ? "enforce" : (iMode == PW_PAM_MODE_MONITOR) ? "monitor" : "",
          (pcUser == NULL) ? "(null)" : pcUser,
          pcError
        );
        return PAM_SERVICE_ERR;
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
        pam_syslog
        (
          psPamHandle,
          LOG_DEBUG,
          "Release='%s'; Library='%s'; Module='%s'; PamFlags='0x%08x'; Mode='%s'; User='%s'; Error='%s';",
          PwVGetReleaseString(),
          PwVGetLibraryVersion(),
          PwVGetModuleVersion(),
          iPamFlags,
          (iMode == PW_PAM_MODE_ENFORCE) ? "enforce" : (iMode == PW_PAM_MODE_MONITOR) ? "monitor" : "",
          (pcUser == NULL) ? "(null)" : pcUser,
          pcError
        );
        return PAM_SERVICE_ERR;
      }
    }
    else if (strcasecmp(ppcArgumentVector[iIndex], PW_PAM_OPT_USE_AUTHTOK) == 0)
    {
      iUseAuthToken = PATHWELL_TRUE;
    }
    else
    {
      pcError = "One or more invalid or unsupported options were specified. Refer to the documentation for a list of supported options and the correct usage syntax.";
      pam_syslog
      (
        psPamHandle,
        LOG_DEBUG,
        "Release='%s'; Library='%s'; Module='%s'; PamFlags='0x%08x'; Mode='%s'; User='%s'; Error='%s';",
        PwVGetReleaseString(),
        PwVGetLibraryVersion(),
        PwVGetModuleVersion(),
        iPamFlags,
        (iMode == PW_PAM_MODE_ENFORCE) ? "enforce" : (iMode == PW_PAM_MODE_MONITOR) ? "monitor" : "",
        (pcUser == NULL) ? "(null)" : pcUser,
        pcError
      );
      return PAM_SERVICE_ERR;
    }
  }

  /*-
   ***********************************************************************
   *
   * If debug mode is enabled, announce that we are alive.
   *
   ***********************************************************************
   */
  if (iDebug)
  {
    pam_syslog
    (
      psPamHandle,
      LOG_DEBUG,
      "Release='%s'; Library='%s'; Module='%s'; PamFlags='0x%08x'; Mode='%s'; User='%s'; Status='Starting pam_sm_chauthtok';",
      PwVGetReleaseString(),
      PwVGetLibraryVersion(),
      PwVGetModuleVersion(),
      iPamFlags,
      (iMode == PW_PAM_MODE_ENFORCE) ? "enforce" : (iMode == PW_PAM_MODE_MONITOR) ? "monitor" : "",
      (pcUser == NULL) ? "(null)" : pcUser
    );
  }

  /*-
   ***********************************************************************
   *
   * If the mode was not set, there's nothing to do, so bail out early.
   *
   ***********************************************************************
   */
  switch (iMode)
  {
  case PW_PAM_MODE_ENFORCE:
    iDbRequired = (iMaxUse > 0 || iBlacklist == PATHWELL_TRUE) ? PATHWELL_TRUE : PATHWELL_FALSE;
    break;
  case PW_PAM_MODE_MONITOR:
    iDbRequired = PATHWELL_TRUE;
    break;
  case PW_PAM_MODE_NOT_SET:
    return PAM_IGNORE;
    break;
  default:
    pcError = "Invalid mode. That should not happen.";
    pam_syslog
    (
      psPamHandle,
      LOG_DEBUG,
      "Release='%s'; Library='%s'; Module='%s'; PamFlags='0x%08x'; Mode='%s'; User='%s'; Error='%s';",
      PwVGetReleaseString(),
      PwVGetLibraryVersion(),
      PwVGetModuleVersion(),
      iPamFlags,
      (iMode == PW_PAM_MODE_ENFORCE) ? "enforce" : (iMode == PW_PAM_MODE_MONITOR) ? "monitor" : "",
      (pcUser == NULL) ? "(null)" : pcUser,
      pcError
    );
    return PAM_SERVICE_ERR;
    break;
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
     * checks out. If it does not exist, create and initialize it, but
     * if that should fail, simply bail out.
     *
     *******************************************************************
     */
    if (iDbRequired == PATHWELL_TRUE)
    {
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
      }
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
        "Release='%s'; Library='%s'; Module='%s'; PamFlags='0x%08x'; Mode='%s'; User='%s'; Error='%s';",
        PwVGetReleaseString(),
        PwVGetLibraryVersion(),
        PwVGetModuleVersion(),
        iPamFlags,
        (iMode == PW_PAM_MODE_ENFORCE) ? "enforce" : (iMode == PW_PAM_MODE_MONITOR) ? "monitor" : "",
        (pcUser == NULL) ? "(null)" : pcUser,
        pcError
      );
    }
    PwDFreeContext(psPwDContext);
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
     *******************************************************************
     *
     * Acquire the user's name and old password. Note that while the
     * user's name should always be defined, the password may or may
     * not be defined.
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
//FIXME This is OK if the passwd is being set by the superuser (e.g., root runs 'passwd <user>'). Otherwise, it should be treated like an error.
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
        pcError = "The new password is not defined, so there is nothing that can be done.";
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
        pcError = "The new password is not defined, so there is nothing that can be done.";
        iReturnValue = PAM_AUTHTOK_ERR;
        goto UPDATE_CLEANUP;
      }
    }

    /*-
     *******************************************************************
     *
     * Convert the old/new passwords to topologies and then to IDs.
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

    /*-
     *******************************************************************
     *
     * Conditionally reject passwords that are too short.
     *
     *******************************************************************
     */
    if (iMode == PW_PAM_MODE_ENFORCE && iMinLen > 0 && psNewPwTContext != NULL)
    {
      if (strlen(PwTGetPassword(psNewPwTContext)) < iMinLen)
      {
        pcError = "The chosen password does not meet the minimum length requirement.";
        iReturnValue = PAM_AUTHTOK_ERR;
        goto UPDATE_CLEANUP;
      }
    }

    /*-
     *******************************************************************
     *
     * Conditionally reject passwords having a blacklisted topology.
     *
     *******************************************************************
     */
    if (iMode == PW_PAM_MODE_ENFORCE && iBlacklist == PATHWELL_TRUE && psNewPwTContext != NULL)
    {
      psPwDContext = PwDNewContextFromParameters(pcDbFile, SQLITE_OPEN_READWRITE, NULL, 0);
      if (!PwDContextIsValid(psPwDContext))
      {
        pcError = PwDGetError(psPwDContext);
        iReturnValue = PAM_SERVICE_ERR;
        goto UPDATE_CLEANUP;
      }
      iBlacklisted = PwDTopologyIsBlacklisted(psPwDContext, (char *)PwTGetTopology(psNewPwTContext));
      if (iBlacklisted == PATHWELL_INDETERMINATE)
      {
        pcError = PwDGetError(psPwDContext);
        iReturnValue = PAM_SERVICE_ERR;
        goto UPDATE_CLEANUP;
      }
      if (iBlacklisted == PATHWELL_TRUE)
      {
        pcError = "The topology associated with the chosen password has been blacklisted.";
        iReturnValue = PAM_AUTHTOK_ERR;
        goto UPDATE_CLEANUP;
      }
    }

    /*-
     *******************************************************************
     *
     * Conditionally reject passwords that are too similar.
     *
     *******************************************************************
     */
    if (iMode == PW_PAM_MODE_ENFORCE && iMinLev > 0 && psOldPwTContext != NULL && psNewPwTContext != NULL)
    {
      psPwLContext = PwLNewContextFromParameters(psOldPwTContext, psNewPwTContext);
      if (!PwLContextIsValid(psPwLContext))
      {
        pcError = PwLGetError(psPwLContext);
        iReturnValue = PAM_SERVICE_ERR;
        goto UPDATE_CLEANUP;
      }
      iLeveled = PwLCheckLevDistance(psPwLContext, iMinLev);
      if (iLeveled == PATHWELL_INDETERMINATE)
      {
        pcError = PwLGetError(psPwLContext);
        iReturnValue = PAM_SERVICE_ERR;
        goto UPDATE_CLEANUP;
      }
      if (iLeveled != PATHWELL_TRUE)
      {
        pcError = "The topology associated with the chosen password does not meet the minimum required Lev distance.";
        iReturnValue = PAM_AUTHTOK_ERR;
        goto UPDATE_CLEANUP;
      }
    }

    /*-
     *******************************************************************
     *
     * Conditionally reject passwords whose topology use count would
     * exceed the specified limit. Note that PwDGetUseCount() returns
     * NULL if the use count is not yet defined in the database, and
     * under those circumstances, a NULL by itself is not considered
     * an error. Any true error will be accompanied by a corresponding
     * error message, which can be retrieved via PwDGetError().
     *
     *******************************************************************
     */
    if (iMode == PW_PAM_MODE_ENFORCE && iMaxUse > 0 && psNewPwTContext != NULL)
    {
      psPwDContext = PwDNewContextFromParameters(pcDbFile, SQLITE_OPEN_READWRITE, NULL, 0);
      if (!PwDContextIsValid(psPwDContext))
      {
        pcError = PwDGetError(psPwDContext);
        iReturnValue = PAM_SERVICE_ERR;
        goto UPDATE_CLEANUP;
      }
      puiUseCount = PwDGetUseCount(psPwDContext, psNewPwTContext);
      if (puiUseCount == NULL)
      {
        pcError = PwDGetError(psPwDContext);
        if (pcError != NULL)
        {
          iReturnValue = PAM_SERVICE_ERR;
          goto UPDATE_CLEANUP;
        }
        else
        {
          /* The use count has not yet been defined in the database, and that's OK. */
        }
      }
      else
      {
        if (*puiUseCount >= iMaxUse)
        {
          pcError = "The topology associated with the chosen password would exceed the maximum allowed use count.";
          iReturnValue = PAM_AUTHTOK_ERR;
          goto UPDATE_CLEANUP;
        }
      }
    }

    /*-
     *******************************************************************
     *
     * Conditionally increment the topology use count that corresponds
     * to the chosen password.
     *
     *******************************************************************
     */
    if (iMode == PW_PAM_MODE_MONITOR && psNewPwTContext != NULL)
    {
      psPwDContext = PwDNewContextFromParameters(pcDbFile, SQLITE_OPEN_READWRITE, NULL, 0);
      if (!PwDContextIsValid(psPwDContext))
      {
        pcError = PwDGetError(psPwDContext);
        iReturnValue = PAM_SERVICE_ERR;
        goto UPDATE_CLEANUP;
      }
      iError = PwDIncrementUseCount(psPwDContext, psNewPwTContext);
      if (iError != ER_OK)
      {
        pcError = PwDGetError(psPwDContext);
        iReturnValue = PAM_SERVICE_ERR;
        goto UPDATE_CLEANUP;
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
    if (pcError != NULL)
    {
      pam_syslog
      (
        psPamHandle,
        LOG_DEBUG,
        "Release='%s'; Library='%s'; Module='%s'; PamFlags='0x%08x'; Mode='%s'; User='%s'; Error='%s';",
        PwVGetReleaseString(),
        PwVGetLibraryVersion(),
        PwVGetModuleVersion(),
        iPamFlags,
        (iMode == PW_PAM_MODE_ENFORCE) ? "enforce" : (iMode == PW_PAM_MODE_MONITOR) ? "monitor" : "",
        (pcUser == NULL) ? "(null)" : pcUser,
        pcError
      );
      if (pcNewAuthToken != NULL)
      {
        pam_set_item(psPamHandle, PAM_AUTHTOK, NULL);
      }
    }
    else
    {
      if (iDebug)
      {
        pam_syslog
        (
          psPamHandle,
          LOG_DEBUG,
          "Release='%s'; Library='%s'; Module='%s'; PamFlags='0x%08x'; Mode='%s'; User='%s'; Status='Success'; Topology='%s'; Id='%" PRId64 "';",
          PwVGetReleaseString(),
          PwVGetLibraryVersion(),
          PwVGetModuleVersion(),
          iPamFlags,
          (iMode == PW_PAM_MODE_ENFORCE) ? "enforce" : (iMode == PW_PAM_MODE_MONITOR) ? "monitor" : "",
          (pcUser == NULL) ? "(null)" : pcUser,
          PwTGetTopology(psNewPwTContext),
          (unsigned long long) PwTGetId(psNewPwTContext)
        );
      }
      else
      {
        pam_syslog
        (
          psPamHandle,
          LOG_DEBUG,
          "Release='%s'; Library='%s'; Module='%s'; PamFlags='0x%08x'; Mode='%s'; User='%s'; Status='Success';",
          PwVGetReleaseString(),
          PwVGetLibraryVersion(),
          PwVGetModuleVersion(),
          iPamFlags,
          (iMode == PW_PAM_MODE_ENFORCE) ? "enforce" : (iMode == PW_PAM_MODE_MONITOR) ? "monitor" : "",
          (pcUser == NULL) ? "(null)" : pcUser
        );
      }
    }
    PwDFreeContext(psPwDContext);
    PwLFreeContext(psPwLContext);
    PwTFreeContext(psNewPwTContext);
    PwTFreeContext(psOldPwTContext);
    if (puiUseCount != NULL)
    {
      free(puiUseCount);
    }
    return iReturnValue;
  }

  return PAM_IGNORE;
}
