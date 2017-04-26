/*-
 ***********************************************************************
 *
 * $Id: hint.c,v 1.47 2017/04/21 19:04:34 klm Exp $
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
#include "all-includes.h"

/*-
 ***********************************************************************
 *
 * Global variables
 *
 ***********************************************************************
 */
extern char const *gpcContextAllocationError;
extern char const *gpcRealErrorLostNullFormat;
extern char const *gpcRealErrorLostNullMalloc;

/*-
 **********************************************************************
 *
 * PwHContextIsValid
 *
 **********************************************************************
 */
int
PwHContextIsValid(PW_H_CONTEXT *psPwHContext)
{
  if (
         psPwHContext == NULL
      || psPwHContext->iHintInfoLevel < PATHWELL_HINT_INFO_LEVEL_MIN
      || psPwHContext->iHintInfoLevel > PATHWELL_HINT_INFO_LEVEL_MAX
      || psPwHContext->iHintPresLevel < PATHWELL_HINT_PRES_LEVEL_MIN
      || psPwHContext->iHintPresLevel > PATHWELL_HINT_PRES_LEVEL_MAX
     )
  {
    return PATHWELL_FALSE;
  }
  return PATHWELL_TRUE;
}


/*-
 **********************************************************************
 *
 * PwHNewContextFromParameters
 *
 **********************************************************************
 */
PW_H_CONTEXT *
PwHNewContextFromParameters(int iDeleteAllowed, int iHintInfoLevel, int iHintPresLevel, int iEnvironment, unsigned int uiSeed)
{
  PW_H_CONTEXT *psPwHContext = NULL;

  /*-
   *********************************************************************
   *
   * Allocate and initialize memory for a new context structure.
   *
   *********************************************************************
   */
  psPwHContext = (PW_H_CONTEXT *)malloc(sizeof(PW_H_CONTEXT));
  if (psPwHContext == NULL)
  {
    return NULL;
  }

  psPwHContext->iDeleteAllowed = iDeleteAllowed;
  /*-
   *********************************************************************
   *
   * Make sure the hint information, presentation, and environmant
   * levels are allowed.
   *
   *********************************************************************
   */
  if (iHintInfoLevel < PATHWELL_HINT_INFO_LEVEL_MIN || iHintInfoLevel > PATHWELL_HINT_INFO_LEVEL_MAX)
  {
    PwHSetError(psPwHContext, "Called PwHNewContextFromParameters() with an invalid HintInfo level.", NULL);
    return psPwHContext;
  }
  psPwHContext->iHintInfoLevel = iHintInfoLevel;

  if (iHintPresLevel < PATHWELL_HINT_PRES_LEVEL_MIN || iHintPresLevel > PATHWELL_HINT_PRES_LEVEL_MAX)
  {
    PwHSetError(psPwHContext, "Called PwHNewContextFromParameters() with an invalid HintPres level.", NULL);
    return psPwHContext;
  }
  psPwHContext->iHintPresLevel = iHintPresLevel;
  if (iEnvironment < PATHWELL_ENV_MIN || iEnvironment > PATHWELL_ENV_MAX)
  {
    PwHSetError(psPwHContext, "Called PwHNewContextFromParameters() with an invalid Environment level.", NULL);
    return psPwHContext;
  }
  psPwHContext->iEnvironment = iEnvironment;

  /*-
   *********************************************************************
   *
   * Set the seed to the PRNG. If the seed is 0, the system time will be
   * used to derive a seed.
   *
   *********************************************************************
   */
  psPwHContext->uiSeed = uiSeed;

  /*-
   *********************************************************************
   *
   * Set all non-initialized pointers to NULL
   *
   *********************************************************************
   */
  psPwHContext->pcError = NULL;
  psPwHContext->pcHint = NULL;
  psPwHContext->pcNearbyTopology = NULL;
  psPwHContext->pcNearbyPassword = NULL;

  return psPwHContext;
}


/*-
 **********************************************************************
 *
 * PwHFreeContext
 *
 **********************************************************************
 */
void
PwHFreeContext(PW_H_CONTEXT **ppsPwHContext)
{
  PW_H_CONTEXT *psPwHContext = *ppsPwHContext;

  if(psPwHContext != NULL)
  {
    if
    (
         psPwHContext->pcError != NULL
      && psPwHContext->pcError != gpcContextAllocationError
      && psPwHContext->pcError != gpcRealErrorLostNullFormat
      && psPwHContext->pcError != gpcRealErrorLostNullMalloc
    )
    {
      free(psPwHContext->pcError);
    }
    if (psPwHContext->pcHint != NULL)
    {
      free(psPwHContext->pcHint);
    }
    if (psPwHContext->pcNearbyPassword != NULL)
    {
      free(psPwHContext->pcNearbyPassword);
    }
    if (psPwHContext->pcNearbyTopology != NULL)
    {
      free(psPwHContext->pcNearbyTopology);
    }
    free(psPwHContext);
    *ppsPwHContext = NULL;
  }
  return;
}


/*-
 **********************************************************************
 *
 * PwHGenHint
 *
 **********************************************************************
 */
int
PwHGenHint(PW_H_CONTEXT *psPwHContext, PW_C_CONTEXT *psPwCContext)
{
  char *pcTemp = NULL;

  if (PwHContextIsValid(psPwHContext) == PATHWELL_FALSE)
  {
    PwHSetError(psPwHContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwHContextIsValid(psPwHContext) == PATHWELL_TRUE);
  if (PwCContextIsValid((PW_C_CONTEXT *) psPwCContext) == PATHWELL_FALSE)
  {
    PwHSetError(psPwHContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwCContextIsValid(psPwCContext) == PATHWELL_TRUE);

  /*-
   *********************************************************************
   *
   * Call the appropriate GenHintInfo function.
   *
   *********************************************************************
   */
  switch (PwHGetEnvironment(psPwHContext))
  {
  case PATHWELL_ENV_CLI:
    switch (psPwHContext->iHintInfoLevel)
    {
    case PATHWELL_HINT_INFO_LEVEL_1:
      return PwHGenHintInfoLevel1(psPwHContext, psPwCContext);
      break;
    case PATHWELL_HINT_INFO_LEVEL_2:
      return PwHGenHintInfoLevel2(psPwHContext, psPwCContext);
      break;
    case PATHWELL_HINT_INFO_LEVEL_3:
      return PwHGenHintInfoLevel3(psPwHContext, psPwCContext);
      break;
    case PATHWELL_HINT_INFO_LEVEL_4:
      return PwHGenHintInfoLevel4(psPwHContext, psPwCContext);
      break;
    default:
      PwHSetError(psPwHContext, "Invalid hint information level. That should not happen.", NULL);
      return ER;
      break;
    }
    break;

#ifdef USE_JSON
  case PATHWELL_ENV_JAVASCRIPT:
    switch (psPwHContext->iHintInfoLevel)
    {
    case PATHWELL_HINT_INFO_LEVEL_1:
      return PwHGenHintInfoLevel1(psPwHContext, psPwCContext);
      break;
    case PATHWELL_HINT_INFO_LEVEL_2:
      return PwHGenHintInfoLevel2Json(psPwHContext, psPwCContext);
      break;
    case PATHWELL_HINT_INFO_LEVEL_3:
      return PwHGenHintInfoLevel3Json(psPwHContext, psPwCContext);
      break;
    case PATHWELL_HINT_INFO_LEVEL_4:
      return PwHGenHintInfoLevel4Json(psPwHContext, psPwCContext);
      break;
    default:
      PwHSetError(psPwHContext, "Invalid hint information level. That should not happen.", NULL);
      return ER;
      break;
    }
    break;
#endif

  default:
    PwHSetError(psPwHContext, "Invalid environment level. That should not happen.", NULL);
    return ER;
    break;
  }
}


/*-
 **********************************************************************
 *
 * PwHGenHintDiagram
 *
 **********************************************************************
 */
char *
PwHGenHintDiagram(PW_H_CONTEXT *psPwHContext, const char *pcSrcBuf)
{
  char *pcHint = NULL;
  int   aiFinalModMapping[PATHWELL_HINT_MAX_DIST] = {};
  int   aiFinalModPos[PATHWELL_HINT_MAX_DIST] = {};
  int   aiFinalModTaken[PATHWELL_HINT_MAX_DIST] = {};
  int   i = 0;
  int   j = 0;
  int   k = 0;
  int   iHintLevel = psPwHContext->iHintInfoLevel;
  int   iMappedPos = 0;
  int   iMappedIndex = 0;
  int   iMinVal = 0;
  int   iMinPos = 0;
  int   iNumModifications = 0;
  int   iPrintLength = 0;
  int   iSettled = PATHWELL_FALSE;
  int   iSpaces = 0;

  if (PwHContextIsValid(psPwHContext) != PATHWELL_TRUE)
  {
    PwHSetError(psPwHContext, "Invalid context. That should not happen.", NULL);
    return NULL;
  }
  PATHWELL_ASSERT(PwHContextIsValid(psPwHContext) == PATHWELL_TRUE);

  pcHint = PwSAppendToDynamicString(pcHint, "\n");
  if (pcHint == NULL)
  {
    PwHSetError(psPwHContext, "Could not generate a hint.", NULL);
    return NULL;
  }

  iPrintLength = strlen(pcSrcBuf);
  iNumModifications = psPwHContext->iNumModifications;
  for (i = 0; i < iNumModifications; i++)
  {
    aiFinalModPos[i] = psPwHContext->piModifications[i][1];
  }

  /*-
   *********************************************************************
   *
   * Make primary changes for printing to the modification positions.
   *
   *********************************************************************
   */
  for (i = 0; i < iNumModifications; i++)
  {
    /*-
     *******************************************************************
     *
     * In the case of a deletion, later modification indices need to be
     * incremented.
     *
     *******************************************************************
     */
    if (psPwHContext->piModifications[i][0] == PATHWELL_MOD_DELETION)
    {
      for (j = i+1; j < iNumModifications; j++)
      {
        if (psPwHContext->piModifications[i][1] <= psPwHContext->piModifications[j][1])
        {
          aiFinalModPos[j]++;
        }
      }
    }

    /*-
     *******************************************************************
     *
     * In the case of an insertion, previous modification indices that
     * occur after this one need to be incremented.
     *
     *******************************************************************
     */
    else if (psPwHContext->piModifications[i][0] == PATHWELL_MOD_INSERTION)
    {
      for (j = 0; j < i; j++)
      {
        if (psPwHContext->piModifications[i][1] <= psPwHContext->piModifications[j][1])
        {
          aiFinalModPos[j]++;
        }
      }
      iPrintLength++;
    }
  }

  /*-
   *********************************************************************
   *
   * Iterate on corner cases until the printing positions settle.
   *
   *********************************************************************
   */
  while (iSettled != PATHWELL_TRUE)
  {
    iSettled = PATHWELL_TRUE;
    for (i = 0; i < iNumModifications; i++)
    {
      for (j = i + 1; j < iNumModifications; j++)
      {
        if (aiFinalModPos[i] == aiFinalModPos[j])
        {
          if (psPwHContext->piModifications[i][0] != PATHWELL_MOD_DELETION && psPwHContext->piModifications[j][0] == PATHWELL_MOD_INSERTION)
          {
            aiFinalModPos[i]++;
          }
          else
          {
            aiFinalModPos[j]++;
          }
          iSettled = PATHWELL_FALSE;
        }
      }
    }
  }

  /*-
   *********************************************************************
   *
   * Develop a mapping of the order in which to print the modifications.
   *
   *********************************************************************
   */
  for (i = 0; i < iNumModifications; i++)
  {
    iMinVal = iPrintLength;
    for (j = 0; j < iNumModifications; j++)
    {
      if (aiFinalModTaken[j] == PATHWELL_FALSE && aiFinalModPos[j] <= iMinVal)
      {
        iMinVal = aiFinalModPos[j];
        iMinPos = j;
      }
    }
    aiFinalModMapping[i] = iMinPos;
    aiFinalModTaken[iMinPos] = PATHWELL_TRUE;
  }

  /*-
   *********************************************************************
   *
   * Print the original topology/password with spaces inserted to
   * accommodate insertions.
   *
   *********************************************************************
   */
  if (iHintLevel < 4)
  {
    pcHint = PwSAppendToDynamicString(pcHint, "                                        ");
  }
  else
  {
    pcHint = PwSAppendToDynamicString(pcHint, "                    ");
  }
  for (i = 0; i < iNumModifications; i++)
  {
    pcHint = PwSAppendToDynamicString(pcHint, "  ");
  }
  for (i = 0; i < iPrintLength; i++)
  {
    for (j = 0; j < iNumModifications; j++)
    {
      if (psPwHContext->piModifications[j][0] == PATHWELL_MOD_INSERTION && i == aiFinalModPos[j])
      {
        pcHint = PwSAppendToDynamicString(pcHint, " ");
        iSpaces++;
        break;
      }
    }
    if (j == iNumModifications)
    {
      pcHint = PwSFormatDynamicString(pcHint, "%c", pcSrcBuf[i - iSpaces]);
    }
  }
  pcHint = PwSAppendToDynamicString(pcHint, "\n");

  /*-
   *********************************************************************
   *
   * Print the suggested modifications left to right, one per line, with
   * one line of spacing in between.
   *
   *********************************************************************
   */
  for (i = 0; i < iNumModifications; i++)
  {
    iMappedIndex = aiFinalModMapping[i];
    iMappedPos = aiFinalModPos[iMappedIndex];

    if (iHintLevel < 4)
    {
      pcHint = PwSAppendToDynamicString(pcHint, "                                        ");
    }
    else
    {
      pcHint = PwSAppendToDynamicString(pcHint, "                    ");
    }
    for (j = 0; j < iNumModifications; j++)
    {
      pcHint = PwSAppendToDynamicString(pcHint, "  ");
    }
    for (j = 0; j < iPrintLength; j++)
    {
      for (k = 0; k < iNumModifications; k++)
      {
        if (j == aiFinalModPos[k])
        {
          break;
        }
      }
      pcHint = PwSAppendToDynamicString(pcHint, (k == iNumModifications || j  < iMappedPos) ? " " : "|");
    }
    pcHint = PwSAppendToDynamicString(pcHint, "\n");

    for (j = 0; j < i+1; j++)
    {
      pcHint = PwSAppendToDynamicString(pcHint, "  ");
    }
    switch(psPwHContext->piModifications[aiFinalModMapping[i]][0])
    {
    case PATHWELL_MOD_DELETION:
      if (iHintLevel < 4)
      {
        pcHint = PwSAppendToDynamicString(pcHint, "delete -------------------------------");
      }
      else
      {
        pcHint = PwSAppendToDynamicString(pcHint, "delete -----------");
      }
      break;
    case PATHWELL_MOD_INSERTION:
      pcHint = PwSFormatDynamicString(pcHint, "insert ");
      if (iHintLevel < 4)
      {
        pcHint = PwSFormatDynamicString(pcHint, "%s %s -", (psPwHContext->piModifications[aiFinalModMapping[i]][2] == 'u') ? "an" : "a", PwSClassToString(psPwHContext->piModifications[aiFinalModMapping[i]][2]));
        switch (psPwHContext->piModifications[aiFinalModMapping[i]][2])
        {
        case 'd':
          pcHint = PwSAppendToDynamicString(pcHint, "----------------------");
          break;
        case 'l':
          pcHint = PwSAppendToDynamicString(pcHint, "-------");
          break;
        case 's':
          pcHint = PwSAppendToDynamicString(pcHint, "----------");
          break;
        case 'u':
          pcHint = PwSAppendToDynamicString(pcHint, "------");
        default:
          break;
        }
      }
      else
      {
        pcHint = PwSFormatDynamicString(pcHint, "a '%c' -----", psPwHContext->piModifications[aiFinalModMapping[i]][2]);
      }
      break;
    case PATHWELL_MOD_SUBSTITUTION:
      pcHint = PwSFormatDynamicString(pcHint, "replace with ");
      if (iHintLevel < 4)
      {
        pcHint = PwSFormatDynamicString(pcHint, "%s %s -", (psPwHContext->piModifications[aiFinalModMapping[i]][2] == 'u') ? "an" : "a", PwSClassToString(psPwHContext->piModifications[aiFinalModMapping[i]][2]));
        switch (psPwHContext->piModifications[aiFinalModMapping[i]][2])
        {
        case 'd':
          pcHint = PwSAppendToDynamicString(pcHint, "----------------");
          break;
        case 'l':
          pcHint = PwSAppendToDynamicString(pcHint, "-");
          break;
        case 's':
          pcHint = PwSAppendToDynamicString(pcHint, "----");
          break;
        default:
          break;
        }
      }
      else
      {
        pcHint = PwSFormatDynamicString(pcHint, "'%c' -", psPwHContext->piModifications[aiFinalModMapping[i]][2]);
      }
      break;
    default:
      break;
    }
    for (j = i; j < iNumModifications; j++)
    {
      pcHint = PwSAppendToDynamicString(pcHint, "--");
    }
    for (j = 0; j < iPrintLength; j++)
    {
      for (k = 0; k < iNumModifications; k++)
      {
        if (j == aiFinalModPos[k])
        {
          break;
        }
      }

      pcHint = PwSAppendToDynamicString(pcHint, (
                                                  (j < iMappedPos) ?         "-" :
                                                  (j == iMappedPos) ?        "+" :
                                                  (k != iNumModifications) ? "|" :
                                                                                  " "
                                                ));
    }
    pcHint = PwSAppendToDynamicString(pcHint, "\n");
  }

  return pcHint;
}


/*-
 **********************************************************************
 *
 * PwHGenHintInfoLevel1
 *
 * Note: In this HintInfo level, only the pathwell check that failed
 *       is printed to the user. The HintPres level has no effect on this
 *       function.
 *
 **********************************************************************
 */
int
PwHGenHintInfoLevel1(PW_H_CONTEXT *psPwHContext, PW_C_CONTEXT *psPwCContext)
{
  char *pcTemp = NULL;

  /*-
   *********************************************************************
   *
   * Check the database rejection code and print the appropriate error
   * message.
   *
   *********************************************************************
   */

  switch(PwCGetCheckCode((PW_C_CONTEXT *) psPwCContext))
  {
  case PATHWELL_FAILED_MINLEN:
    pcTemp = PwSAppendToDynamicString(psPwHContext->pcHint, "The new password did not meet the minimum length requirement.");
    break;
  case PATHWELL_FAILED_MINLEV:
    pcTemp = PwSAppendToDynamicString(psPwHContext->pcHint, "The new password failed the minlev check.");
    break;
  case PATHWELL_FAILED_MAXUSE:
    pcTemp = PwSAppendToDynamicString(psPwHContext->pcHint, "The new password failed the maxuse check.");
    break;
  case PATHWELL_FAILED_BLACKLIST:
    pcTemp = PwSAppendToDynamicString(psPwHContext->pcHint, "The new password failed the blacklist check.");
    break;
  default:
    PwHSetError(psPwHContext, "Invalid rejection code. That should not happen", NULL);
    return PATHWELL_FALSE;
    break;
  }

  if (pcTemp == NULL)
  {
    PwHSetError(psPwHContext, "Unable to allocate memory for hint string.", NULL);
    return ER;
  }

  psPwHContext->pcHint = pcTemp;
  return ER_OK;
}


/*-
 **********************************************************************
 *
 * PwHGenHintInfoLevel2
 *
 * Note: In this HintInfo level, only information pertaining to the
 *       rejected and suggested topologies is revealed on-screen
 *       to the user. The presentation of that information is
 *       governed by the HintPres level.
 *
 **********************************************************************
 */
int
PwHGenHintInfoLevel2(PW_H_CONTEXT *psPwHContext, PW_C_CONTEXT *psPwCContext)
{
  char         *pcTemp = NULL;
  char         *pcSanitizedTopology = NULL;
  const char   *pcTopology = NULL;
  int           i = 0;
  int           iTopologyLength = 0;

  switch(PwCGetCheckCode((PW_C_CONTEXT *) psPwCContext))
  {
  case PATHWELL_FAILED_MINLEN:
    pcTemp = PwSAppendToDynamicString(psPwHContext->pcHint, "The new password did not meet the minimum length requirement.");
    break;
  case PATHWELL_FAILED_MINLEV:
    pcTemp = PwSAppendToDynamicString(psPwHContext->pcHint, "The new password failed the minlev check.");
    break;
  case PATHWELL_FAILED_MAXUSE:
    pcTemp = PwSAppendToDynamicString(psPwHContext->pcHint, "The new password failed the maxuse check.");
    break;
  case PATHWELL_FAILED_BLACKLIST:
    if (PwHGenNearbyTopology(psPwHContext, psPwCContext) != ER_OK)
    {
      return ER;
    }
    pcTopology = PwTGetTopology(psPwCContext->psNewPwTContext);
    iTopologyLength = strlen(pcTopology) / 2;

    pcSanitizedTopology = (char *)malloc(iTopologyLength + 1);
    if (pcSanitizedTopology == NULL)
    {
      PwHSetError(psPwHContext, "Could not allocate space for a sanitized topology string.", NULL);
      return ER;
    }
    for (i = 0; i < iTopologyLength; i++)
    {
      pcSanitizedTopology[i] = pcTopology[i*2 + 1];
    }
    pcSanitizedTopology[iTopologyLength] = '\0';

    /*-
     *********************************************************************
     *
     * Generate the appropriate message for the hint level.
     *
     *********************************************************************
     */
    switch (psPwHContext->iHintPresLevel)
    {
    case PATHWELL_HINT_PRES_LEVEL_1:
      pcTemp = PwSFormatDynamicString(psPwHContext->pcHint,
                 "The new password failed the blacklist check. Try making a password with this topology: %s.",
                 psPwHContext->pcNearbyTopology
                 );
      break;
    case PATHWELL_HINT_PRES_LEVEL_2:
      pcTemp = PwHGenHintDiagram(psPwHContext, pcSanitizedTopology);
      break;
    case PATHWELL_HINT_PRES_LEVEL_3:
      pcTemp = PwHGenHintDiagram(psPwHContext, pcSanitizedTopology);
      pcTemp = PwSFormatDynamicString(pcTemp, "\n This should produce the following topology: %s\n", psPwHContext->pcNearbyTopology);
      break;
    default:
      PwHSetError(psPwHContext, "Unsupported hint presentation value. This is a bug.", NULL);
      return ER;
      break;
    }
    break;
  default:
    PwHSetError(psPwHContext, "Invalid rejection code. That should not happen.", NULL);
    return PATHWELL_FALSE;
    break;
  }

  if (pcTemp == NULL)
  {
    PwHSetError(psPwHContext, "Unable to allocate memory for hint string.", NULL);
    return ER;
  }

  psPwHContext->pcHint = pcTemp;
  free(pcSanitizedTopology);
  return ER_OK;
}


#ifdef USE_JSON
/*-
 **********************************************************************
 *
 * PwHGenHintInfoLevel2Json
 *
 * Note: In this HintInfo level, only information pertaining to the
 *       rejected and suggested topologies is revealed on-screen
 *       to the user. The output is in JSON format, intended to be
 *       given to a front-end GUI.
 *
 **********************************************************************
 */
int
PwHGenHintInfoLevel2Json(PW_H_CONTEXT *psPwHContext, PW_C_CONTEXT *psPwCContext)
{
  char         *pcTemp = NULL;
  char         *pcSanitizedTopology = NULL;
  const char   *pcTopology = NULL;
  int           i = 0;
  int           iTopologyLength = 0;

  if (PwHGenNearbyTopology(psPwHContext, psPwCContext) != ER_OK)
  {
    return ER;
  }

  pcTopology = PwTGetTopology(psPwCContext->psNewPwTContext);
  iTopologyLength = strlen(pcTopology) / 2;
  pcSanitizedTopology = (char *)malloc(iTopologyLength + 1);
  if (pcSanitizedTopology == NULL)
  {
    PwHSetError(psPwHContext, "Could not allocate space for a sanitized topology string.", NULL);
    return ER;
  }
  for (i = 0; i < iTopologyLength; i++)
  {
    pcSanitizedTopology[i] = pcTopology[i*2 + 1];
  }
  pcSanitizedTopology[iTopologyLength] = '\0';

  pcTemp = PwHGenHintJson(psPwHContext, pcSanitizedTopology);
  if (pcTemp == NULL)
  {
    /* PwHContext already has an error string */
    return ER;
  }

  psPwHContext->pcHint = pcTemp;
  return ER_OK;
}
#endif


/*-
 **********************************************************************
 *
 * PwHGenHintInfoLevel3
 *
 * Note: In this HintInfo level, only information pertaining to the
 *       rejected plaintext and suggested topologies is revealed
 *       on-screen to the user. The presentation is governed by the
 *       HintPres level.
 *
 **********************************************************************
 */
int
PwHGenHintInfoLevel3(PW_H_CONTEXT *psPwHContext, PW_C_CONTEXT *psPwCContext)
{
  char *pcTemp = NULL;

  switch(PwCGetCheckCode((PW_C_CONTEXT *) psPwCContext))
  {
  case PATHWELL_FAILED_MINLEN:
    pcTemp = PwSAppendToDynamicString(psPwHContext->pcHint, "The new password did not meet the minimum length requirement.");
    break;
  case PATHWELL_FAILED_MINLEV:
    pcTemp = PwSAppendToDynamicString(psPwHContext->pcHint, "The new password failed the minlev check.");
    break;
  case PATHWELL_FAILED_MAXUSE:
    pcTemp = PwSAppendToDynamicString(psPwHContext->pcHint, "The new password failed the maxuse check.");
    break;
  case PATHWELL_FAILED_BLACKLIST:
    if (PwHGenNearbyTopology(psPwHContext, psPwCContext) != ER_OK)
    {
      return ER;
    }

    /*-
     *********************************************************************
     *
     * Generate the appropriate message for the hint level.
     *
     *********************************************************************
     */
    switch (psPwHContext->iHintPresLevel)
    {
    case PATHWELL_HINT_PRES_LEVEL_1:
      pcTemp = PwSFormatDynamicString(psPwHContext->pcHint,
                 "The new password failed the blacklist check. Try making a password with this topology: %s.",
                 psPwHContext->pcNearbyTopology
                 );
      break;
    case PATHWELL_HINT_PRES_LEVEL_2:
      pcTemp = PwHGenHintDiagram(psPwHContext, PwTGetPassword(psPwCContext->psNewPwTContext));
      break;
    case PATHWELL_HINT_PRES_LEVEL_3:
      pcTemp = PwHGenHintDiagram(psPwHContext, PwTGetPassword(psPwCContext->psNewPwTContext));
      pcTemp = PwSFormatDynamicString(pcTemp, "\n This should produce the following topology: %s\n", psPwHContext->pcNearbyTopology);
      break;
    default:
      PwHSetError(psPwHContext, "Unsupported hint presentation level. That should not happen.", NULL);
      return ER;
      break;
    }
    break;
  default:
    PwHSetError(psPwHContext, "Invalid rejection code. That should not happen.", NULL);
    return PATHWELL_FALSE;
    break;
  }

  if (pcTemp == NULL)
  {
    PwHSetError(psPwHContext, "Unable to allocate memory for hint string.", NULL);
    return ER;
  }

  psPwHContext->pcHint = pcTemp;
  return ER_OK;
}


#ifdef USE_JSON
/*-
 **********************************************************************
 *
 * PwHGenHintInfoLevel3Json
 *
 * Note: In this HintInfo level, only information pertaining to the
 *       rejected password and suggested topology is revealed on-screen
 *       to the user. The output is in JSON format, intended to be
 *       given to a front-end GUI.
 *
 **********************************************************************
 */
int
PwHGenHintInfoLevel3Json(PW_H_CONTEXT *psPwHContext, PW_C_CONTEXT *psPwCContext)
{
  char         *pcTemp = NULL;

  if (PwHGenNearbyTopology(psPwHContext, psPwCContext) != ER_OK)
  {
    return ER;
  }

  pcTemp = PwHGenHintJson(psPwHContext, PwTGetPassword(psPwCContext->psNewPwTContext));
  if (pcTemp == NULL)
  {
    PwHSetError(psPwHContext, "Unable to allocate memory for hint string.", NULL);
    return ER;
  }

  psPwHContext->pcHint = pcTemp;
  return ER_OK;
}
#endif


/*-
 **********************************************************************
 *
 * PwHGenHintInfoLevel4
 *
 * Note: In this HintInfo level, there is full disclosure of information
 *       pertaining to the rejected plaintext and suggested plaintexts
 *       on-screen to the user. The presentation is governed by the
 *       HintPres level.
 *
 **********************************************************************
 */
int
PwHGenHintInfoLevel4(PW_H_CONTEXT *psPwHContext, PW_C_CONTEXT *psPwCContext)
{
  char *pcTemp = NULL;

  switch(PwCGetCheckCode((PW_C_CONTEXT *) psPwCContext))
  {
  case PATHWELL_FAILED_MINLEN:
    pcTemp = PwSAppendToDynamicString(psPwHContext->pcHint, "The new password did not meet the minimum length requirement.");
    break;
  case PATHWELL_FAILED_MINLEV:
    pcTemp = PwSAppendToDynamicString(psPwHContext->pcHint, "The new password failed the minlev check.");
    break;
  case PATHWELL_FAILED_MAXUSE:
    pcTemp = PwSAppendToDynamicString(psPwHContext->pcHint, "The new password failed the maxuse check.");
    break;
  case PATHWELL_FAILED_BLACKLIST:
    if (PwHGenNearbyTopology(psPwHContext, psPwCContext) != ER_OK)
    {
      return ER;
    }

    if (PwHGenNearbyPassword(psPwHContext, psPwCContext) != ER_OK)
    {
      return ER;
    }

    /*-
     *********************************************************************
     *
     * Generate the appropriate message for the hint level.
     *
     *********************************************************************
     */
    switch (psPwHContext->iHintPresLevel)
    {
    case PATHWELL_HINT_PRES_LEVEL_1:
      pcTemp = PwSFormatDynamicString(psPwHContext->pcHint,
                 "The new password failed the blacklist check. Try making your new password something like this: %s.",
                 psPwHContext->pcNearbyPassword
                 );
      break;
    case PATHWELL_HINT_PRES_LEVEL_2:
      pcTemp = PwHGenHintDiagram(psPwHContext, PwTGetPassword(psPwCContext->psNewPwTContext));
      break;
    case PATHWELL_HINT_PRES_LEVEL_3:
      pcTemp = PwHGenHintDiagram(psPwHContext, PwTGetPassword(psPwCContext->psNewPwTContext));
      pcTemp = PwSFormatDynamicString(pcTemp, "\n This should produce the following password: %s\n", psPwHContext->pcNearbyPassword);
      break;
    default:
      PwHSetError(psPwHContext, "Unsupported hint presentation level. That should not happen.", NULL);
      return ER;
      break;
    }

    break;
  default:
    PwHSetError(psPwHContext, "Invalid rejection code. That should not happen.", NULL);
    return PATHWELL_FALSE;
    break;
  }

  if (pcTemp == NULL)
  {
    PwHSetError(psPwHContext, "Unable to allocate memory for hint string.", NULL);
    return ER;
  }

  psPwHContext->pcHint = pcTemp;
  return ER_OK;
}


#ifdef USE_JSON
/*-
 **********************************************************************
 *
 * PwHGenHintInfoLevel4JSON
 *
 * Note: In this HintInfo level, only information pertaining to the
 *       rejected and suggested plaintexts is revealed on-screen
 *       to the user. The output is in JSON format, intended to be
 *       given to a front-end GUI.
 *
 **********************************************************************
 */
int
PwHGenHintInfoLevel4Json(PW_H_CONTEXT *psPwHContext, PW_C_CONTEXT *psPwCContext)
{
  char         *pcTemp = NULL;

  if (PwHGenNearbyTopology(psPwHContext, psPwCContext) != ER_OK)
  {
    return ER;
  }

  if (PwHGenNearbyPassword(psPwHContext, psPwCContext) != ER_OK)
  {
    return ER;
  }

  pcTemp = PwHGenHintJson(psPwHContext, PwTGetPassword(psPwCContext->psNewPwTContext));
  if (pcTemp == NULL)
  {
    PwHSetError(psPwHContext, "Unable to allocate memory for hint string.", NULL);
    return ER;
  }

  psPwHContext->pcHint = pcTemp;
  return ER_OK;
}
#endif

#ifdef USE_JSON
/*-
 **********************************************************************
 *
 * PwHGenHintJson
 *
 **********************************************************************
 */
char *
PwHGenHintJson(PW_H_CONTEXT *psPwHContext, const char *pcSrcBuf)
{
  char          acNew[2] = {};
  char          acOld[2] = {};
  char         *pcRejected = NULL;
  char         *pcTemp = NULL;
  int           i = 0, j = 0;
  int           iIndex = 0;
  int           aiFinalModMapping[PATHWELL_HINT_MAX_DIST] = {};
  int           aiFinalModPos[PATHWELL_HINT_MAX_DIST] = {};
  int           aiFinalModTaken[PATHWELL_HINT_MAX_DIST] = {};
  int           iHintLevel = psPwHContext->iHintInfoLevel;
  int           iMinVal = 0;
  int           iMinPos = 0;
  int           iNumModifications = 0;
  int           iPrintLength = 0;
  int           iSettled = PATHWELL_FALSE;
  int           iSpaces = 0;
  json_t       *psJsonRoot = NULL;
  json_t       *psJsonTmpArray = NULL;
  json_t       *psJsonModArray = NULL;

  iPrintLength = strlen(pcSrcBuf);
  iNumModifications = psPwHContext->iNumModifications;
  for (i = 0; i < iNumModifications; i++)
  {
    aiFinalModPos[i] = psPwHContext->piModifications[i][1];
  }

  /*-
   *********************************************************************
   *
   * Make primary changes for printing to the modification positions.
   *
   *********************************************************************
   */
  for (i = 0; i < iNumModifications; i++)
  {
    /*-
     *******************************************************************
     *
     * In the case of a deletion, later modification indices need to be
     * incremented.
     *
     *******************************************************************
     */
    if (psPwHContext->piModifications[i][0] == PATHWELL_MOD_DELETION)
    {
      for (j = i+1; j < iNumModifications; j++)
      {
        if (psPwHContext->piModifications[i][1] <= psPwHContext->piModifications[j][1])
        {
          aiFinalModPos[j]++;
        }
      }
    }

    /*-
     *******************************************************************
     *
     * In the case of an insertion, previous modification indices that
     * occur after this one need to be incremented.
     *
     *******************************************************************
     */
    else if (psPwHContext->piModifications[i][0] == PATHWELL_MOD_INSERTION)
    {
      for (j = 0; j < i; j++)
      {
        if (psPwHContext->piModifications[i][1] <= psPwHContext->piModifications[j][1])
        {
          aiFinalModPos[j]++;
        }
      }
      iPrintLength++;
    }
  }

  /*-
   *********************************************************************
   *
   * Iterate on corner cases until the printing positions settle.
   *
   *********************************************************************
   */
  while (iSettled != PATHWELL_TRUE)
  {
    iSettled = PATHWELL_TRUE;
    for (i = 0; i < iNumModifications; i++)
    {
      for (j = i + 1; j < iNumModifications; j++)
      {
        if (aiFinalModPos[i] == aiFinalModPos[j])
        {
          if (psPwHContext->piModifications[i][0] != PATHWELL_MOD_DELETION && psPwHContext->piModifications[j][0] == PATHWELL_MOD_INSERTION)
          {
            aiFinalModPos[i]++;
          }
          else
          {
            aiFinalModPos[j]++;
          }
          iSettled = PATHWELL_FALSE;
        }
      }
    }
  }

  /*-
   *********************************************************************
   *
   * Develop a mapping of the order in which to print the modifications.
   *
   *********************************************************************
   */
  for (i = 0; i < iNumModifications; i++)
  {
    iMinVal = iPrintLength;
    for (j = 0; j < iNumModifications; j++)
    {
      if (aiFinalModTaken[j] == PATHWELL_FALSE && aiFinalModPos[j] <= iMinVal)
      {
        iMinVal = aiFinalModPos[j];
        iMinPos = j;
      }
    }
    aiFinalModMapping[i] = iMinPos;
    aiFinalModTaken[iMinPos] = PATHWELL_TRUE;
  }

  /*-
   *********************************************************************
   *
   * Build the JSON object. Insert the topologies.
   *
   *********************************************************************
   */
  psJsonRoot = json_object();
  if (psJsonRoot == NULL)
  {
    PwHSetError(psPwHContext, "Failed to create JSON object.", NULL);
    return NULL;
  }

  pcRejected = (char *) malloc(iPrintLength + 1);
  for (i = 0; i < iPrintLength; i++)
  {
    for (j = 0; j < iNumModifications; j++)
    {
      if (psPwHContext->piModifications[j][0] == PATHWELL_MOD_INSERTION && i == aiFinalModPos[j])
      {
        pcRejected[i] = ' ';
        iSpaces++;
        break;
      }
    }
    if (j == iNumModifications)
    {
      pcRejected[i] = pcSrcBuf[i-iSpaces];
    }
  }
  pcRejected[i] = '\0';

  if (iHintLevel < 3)
  {
    json_object_set_new(psJsonRoot, "rejected topology", json_string(pcSrcBuf));
  }
  else
  {
    json_object_set_new(psJsonRoot, "rejected password", json_string(pcSrcBuf));
  }

  if (iHintLevel < 4)
  {
    json_object_set_new(psJsonRoot, "suggested topology", json_string(PwHGetNearbyTopology(psPwHContext)));
  }
  else
  {
    json_object_set_new(psJsonRoot, "suggested password", json_string(PwHGetNearbyPassword(psPwHContext)));
  }

  /*-
   *********************************************************************
   *
   * Build the modification array. Insert it into the JSON object.
   *
   *********************************************************************
   */
  psJsonTmpArray = json_array();
  if (psJsonTmpArray == NULL)
  {
    PwHSetError(psPwHContext, "Failed to create JSON temporary array object.", NULL);
    return NULL;
  }

  psJsonModArray = json_array();
  if (psJsonModArray == NULL)
  {
    PwHSetError(psPwHContext, "Failed to create JSON modification array object.", NULL);
    return NULL;
  }

  for (i = 0; i < psPwHContext->iNumModifications; i++)
  {
    iIndex = aiFinalModPos[aiFinalModMapping[i]];
    switch (psPwHContext->piModifications[aiFinalModMapping[i]][0])
    {
    case PATHWELL_MOD_DELETION:
      if (iHintLevel < 3)
      {
        acOld[0] = psPwHContext->piModifications[aiFinalModMapping[i]][2];
      }
      else
      {
        acOld[0] = pcRejected[aiFinalModPos[aiFinalModMapping[i]]];
      }
      psJsonTmpArray = json_pack("{sissss}", "index", iIndex, "type", "deletion", "old value", acOld);
      break;
    case PATHWELL_MOD_INSERTION:
      acNew[0] = psPwHContext->piModifications[aiFinalModMapping[i]][2];
      psJsonTmpArray = json_pack("{sissss}", "index", iIndex, "type", "insertion", "new value", acNew);
      break;
    case PATHWELL_MOD_SUBSTITUTION:
      acOld[0] = pcRejected[aiFinalModPos[aiFinalModMapping[i]]];
      acNew[0] = psPwHContext->piModifications[aiFinalModMapping[i]][2];
      psJsonTmpArray = json_pack("{sissssss}", "index", iIndex, "type", "substitution", "old value", acOld, "new value", acNew);
      break;
    default:
      printf("in default\n");
      break;
    }
    json_array_append(psJsonModArray, psJsonTmpArray);
  }
  json_object_set_new(psJsonRoot, "modifications", psJsonModArray);

  /*-
   *********************************************************************
   *
   * Dump the JSON object as a string.
   *
   *********************************************************************
   */
  pcTemp = json_dumps(psJsonRoot, JSON_COMPACT | JSON_PRESERVE_ORDER);

  /*-
   *********************************************************************
   *
   * Check errors and cleanup.
   *
   *********************************************************************
   */
  if (pcTemp == NULL)
  {
    PwHSetError(psPwHContext, "Failed to create JSON output.", NULL);
  }

  json_decref(psJsonTmpArray);
  json_decref(psJsonModArray);
  json_decref(psJsonRoot);

  return pcTemp;
}
#endif


/*-
 **********************************************************************
 *
 * PwHGenNearbyPassword
 *
 **********************************************************************
 */
int
PwHGenNearbyPassword(PW_H_CONTEXT *psPwHContext, PW_C_CONTEXT *psPwCContext)
{
  const char *pcRejectedPassword = NULL;
  char       *pcNewPassword = NULL;
  int         i, j;
  int         iModificationIndex = 0;
  int         iNewPasswordLength = 0;
  int         iNumModifications = 0;
  int         iRejectedPasswordLength = 0;

  if (PwHContextIsValid(psPwHContext) == PATHWELL_FALSE)
  {
    PwHSetError(psPwHContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwHContextIsValid(psPwHContext) == PATHWELL_TRUE);

  if (PwCContextIsValid(psPwCContext) == PATHWELL_FALSE)
  {
    PwCSetError(psPwCContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwCContextIsValid(psPwCContext) == PATHWELL_TRUE);

  if (psPwHContext->pcNearbyTopology == NULL)
  {
    PwHSetError(psPwHContext, "pcNearbyTopology has not been set.", NULL);
    return ER;
  }
  iNumModifications = psPwHContext->iNumModifications;

  /*-
   *********************************************************************
   *
   * Allocate a string for the suggested password, leaving room
   * for possible insertions, and copy the rejected password/topology
   * into it.
   *
   *********************************************************************
   */
  pcRejectedPassword = PwTGetPassword(psPwCContext->psNewPwTContext);
  iNewPasswordLength = strlen(pcRejectedPassword);
  pcNewPassword = (char *)malloc(iNewPasswordLength + iNumModifications + 1);
  if (pcNewPassword == NULL)
  {
    PwHSetError(psPwHContext, "Could not allocate string for new password.", NULL);
    return ER;
  }
  strncpy(pcNewPassword, pcRejectedPassword, iNewPasswordLength);
  for (i = 0; i < PATHWELL_HINT_MAX_DIST + 1; i++)
  {
    pcNewPassword[iNewPasswordLength + i] = '\0';
  }

  /*-
   *********************************************************************
   *
   * Perform the modifications used to generate the nearby topology,
   * choosing a random character from the class where necessary.
   *
   *********************************************************************
   */
  for (i = 0; i < iNumModifications; i++)
  {
    iModificationIndex = psPwHContext->piModifications[i][1];
    switch (psPwHContext->piModifications[i][0])
    {
    case PATHWELL_MOD_INSERTION:
      for (j = iNewPasswordLength; j > iModificationIndex; j--)
      {
        pcNewPassword[j] = pcNewPassword[j-1];
      }
      psPwHContext->piModifications[i][2] = PwSGetRandomCharInClass(psPwHContext->piModifications[i][2]);
      pcNewPassword[iModificationIndex] = psPwHContext->piModifications[i][2];
      iNewPasswordLength++;
      break;
    case PATHWELL_MOD_DELETION:
      psPwHContext->piModifications[i][3] = pcNewPassword[iModificationIndex];
      for (j = iModificationIndex; j < iNewPasswordLength; j++)
      {
        pcNewPassword[j] = pcNewPassword[j+1];
      }
      iNewPasswordLength--;
      break;
    case PATHWELL_MOD_SUBSTITUTION:
      psPwHContext->piModifications[i][2] = PwSGetRandomCharInClass(psPwHContext->piModifications[i][2]);
      psPwHContext->piModifications[i][3] = pcNewPassword[iModificationIndex];
      pcNewPassword[iModificationIndex] = psPwHContext->piModifications[i][2];
      break;
    default:
      break;
    }
  }

  psPwHContext->pcNearbyPassword = pcNewPassword;
  return ER_OK;
}

/*-
 **********************************************************************
 *
 * PwHGenNearbyTopology
 *
 **********************************************************************
 */
int
PwHGenNearbyTopology(PW_H_CONTEXT *psPwHContext, PW_C_CONTEXT *psPwCContext)
{
  char            acClasses[4] = {'d', 'l', 's', 'u'};
  char           *pcDirtyTopology = NULL;
  char           *pcNewTopology = NULL;
  char           *pcRejectedTopology = NULL;
  char           *pcStartTopology = NULL;
  int             i = 0;
  int             j = 0;
  int             iGenAttempts = 0;
  int             iModificationClass = 0;
  int             iModificationIndex = 0;
  int             iModificationType = 0;
  int             iNewTopologyLength = 0;
  int             iNumModifications = 0;
  int             iTypeMax = 0;
  int             iRejectedTopologyLength = 0;
  int             iSuccessFlag = PATHWELL_FALSE;
  PW_C_CONTEXT   *psLocalPwCContext = NULL;
  PW_T_CONTEXT   *psPwTContext = NULL;

  pcRejectedTopology = (char *) PwTGetTopology(psPwCContext->psNewPwTContext);
  iRejectedTopologyLength = strlen(pcRejectedTopology) / 2;
  pcDirtyTopology = (char *)malloc((iRejectedTopologyLength + PATHWELL_HINT_MAX_DIST) * 2 + 1);
  if (pcDirtyTopology == NULL)
  {
    PwHSetError(psPwHContext, "Could not allocate string for new topology.", NULL);
    goto CLEANUP;
  }
  pcNewTopology = (char *)malloc(iRejectedTopologyLength + PATHWELL_HINT_MAX_DIST + 1);
  if (pcNewTopology == NULL)
  {
    PwHSetError(psPwHContext, "Could not allocate string for new topology.", NULL);
    goto CLEANUP;
  }
  pcStartTopology = (char *)malloc(iRejectedTopologyLength + 1);
  if (pcStartTopology == NULL)
  {
    PwHSetError(psPwHContext, "Could not allocate string for new topology.", NULL);
    goto CLEANUP;
  }
  for (i = 0; i < iRejectedTopologyLength; i++)
  {
    pcStartTopology[i] = pcRejectedTopology[i*2 + 1];
  }
  pcStartTopology[iRejectedTopologyLength] = '\0';


  /*-
   *********************************************************************
   *
   * Clone the PwCContext and create a new PwTContext.
   *
   *********************************************************************
   */
  psLocalPwCContext = PwCNewContextFromParameters(psPwCContext->psPwDContext,
                        psPwCContext->psOldPwTContext,
                        psPwCContext->psNewPwTContext,
                        PwCGetMaxUse(psPwCContext),
                        PwCGetMinLen(psPwCContext),
                        PwCGetMinLev(psPwCContext),
                        PwCIsBlacklistEnabled(psPwCContext)
                        );
  if (PwCContextIsValid(psLocalPwCContext) == PATHWELL_FALSE)
  {
    PwHSetError(psPwHContext, "Could not clone context. That should not happen", NULL);
    goto CLEANUP;
  }
  PATHWELL_ASSERT(PwCContextIsValid(psLocalPwCContext) == PATHWELL_TRUE);

  psPwTContext = PwTNewContextFromParameters(PwTGetEncoding(psLocalPwCContext->psNewPwTContext), PwTGetTokenSet(psLocalPwCContext->psNewPwTContext), NULL, pcRejectedTopology, NULL);
  if (PwTContextIsValid(psPwTContext) == PATHWELL_FALSE)
  {
    PwHSetError(psPwHContext, "Could not allocate PwTContext. That should not happen", NULL);
    goto CLEANUP;
  }
  PATHWELL_ASSERT(PwTContextIsValid(psPwTContext) == PATHWELL_TRUE);
  psLocalPwCContext->psNewPwTContext = psPwTContext;

  /*-
   *********************************************************************
   *
   * Generate a number of random modifications to make to the rejected
   * topology.
   *
   *********************************************************************
   */
  PwSSeedRNG(psPwHContext->uiSeed);
  iTypeMax = psPwHContext->iDeleteAllowed ? 3 : 2;
  while (iSuccessFlag != PATHWELL_TRUE && iGenAttempts < PATHWELL_HINT_MAX_TRIES)
  {
    iNumModifications = (random() % PATHWELL_HINT_MAX_DIST) + 1;
    psPwHContext->iNumModifications = iNumModifications;
    iNewTopologyLength = iRejectedTopologyLength;
    strncpy(pcNewTopology, pcStartTopology, iRejectedTopologyLength);
    for (i = 0; i < PATHWELL_HINT_MAX_DIST + 1; i++)
    {
      pcNewTopology[iRejectedTopologyLength + i] = '\0';
    }
    for (i = 0; i < iNumModifications; i++)
    {
      psPwHContext->piModifications[i][0] = iModificationType = random() % iTypeMax;
      psPwHContext->piModifications[i][1] = iModificationIndex = random() % (strlen(pcNewTopology) + ((iModificationType == PATHWELL_MOD_INSERTION) ? 1 : 0));
      iModificationClass = random() % 4;
      switch (iModificationType)
      {
      case PATHWELL_MOD_INSERTION:
        for (j = iNewTopologyLength; j > iModificationIndex; j--)
        {
          pcNewTopology[j] = pcNewTopology[j-1];
        }
        pcNewTopology[iModificationIndex] = acClasses[iModificationClass];
        psPwHContext->piModifications[i][2] = acClasses[iModificationClass];
        iNewTopologyLength++;
        break;
      case PATHWELL_MOD_DELETION:
        psPwHContext->piModifications[i][2] = pcNewTopology[iModificationIndex];
        for (j = iModificationIndex; j < iNewTopologyLength; j++)
        {
          pcNewTopology[j] = pcNewTopology[j+1];
        }
        iNewTopologyLength--;
        break;
      case PATHWELL_MOD_SUBSTITUTION:
        psPwHContext->piModifications[i][3] = pcNewTopology[iModificationIndex];
        pcNewTopology[iModificationIndex] = acClasses[iModificationClass];
        psPwHContext->piModifications[i][2] = acClasses[iModificationClass];
        break;
      default:
        PwHSetError(psPwHContext, "Invalid modification type. This should not happen.", NULL);
        goto CLEANUP;
        break;
      }
    }
    /*-
     *******************************************************************
     *
     * If the Lev distance between the modified and rejected topologies
     * is not equal to the number of modifications made, one of the
     * modifications has 'undone' another, so we throw out the modified
     * topology and start over.
     *
     *******************************************************************
     */
    if (PwSLevenshteinDistance(pcStartTopology, pcNewTopology) < iNumModifications)
    {
      continue;
    }

    /*-
     *******************************************************************
     *
     * If the modified topology is also rejected by the database,
     * increment the number of attempts and start over.
     *
     *******************************************************************
     */
    for (i = 0; i < iNewTopologyLength; i++)
    {
      pcDirtyTopology[i*2] = '?';
      pcDirtyTopology[i*2 + 1] = pcNewTopology[i];
    }
    pcDirtyTopology[i] = '\0';
    PwTSetTopology(psPwTContext, pcDirtyTopology);
    if (PwCAllCheck(psLocalPwCContext) == ER)
    {
      PwHSetError(psPwHContext, "Pathwell AllCheck failed.", NULL);
      goto CLEANUP;
    }

    if (PwCGetCheckCode(psLocalPwCContext) == PATHWELL_ALLCHECK_PASS)
    {
      iSuccessFlag = PATHWELL_TRUE;
    }
    else
    {
      iGenAttempts++;
    }
  }

  if (psPwHContext->pcNearbyTopology != NULL)
  {
    free(psPwHContext->pcNearbyTopology);
  }
  psPwHContext->pcNearbyTopology = pcNewTopology;

CLEANUP:
  PwCFreeContext(&psLocalPwCContext);
  PwTFreeContext(&psPwTContext);
  if (pcDirtyTopology != NULL)
  {
    free(pcDirtyTopology);
  }
  if (pcStartTopology != NULL)
  {
    free(pcStartTopology);
  }

  /*-
   ********************************************************************
   *
   * No acceptable nearby topology could be found in a reasonable
   * number of attempts.
   *
   ********************************************************************
   */
  if (iSuccessFlag != PATHWELL_TRUE)
  {
    PwHSetError(psPwHContext, "Failed to find a valid nearby topology.", NULL);
    return ER;
  }

  return ER_OK;
}


/*-
 **********************************************************************
 *
 * PwHGetEnvironment
 *
 **********************************************************************
 */
int
PwHGetEnvironment(PW_H_CONTEXT *psPwHContext)
{
  if (PwHContextIsValid(psPwHContext) == PATHWELL_FALSE)
  {
    PwHSetError(psPwHContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwHContextIsValid(psPwHContext) == PATHWELL_TRUE);

  return psPwHContext->iEnvironment;
}


/*-
 **********************************************************************
 *
 * PwHGetError
 *
 **********************************************************************
 */
const char *
PwHGetError(PW_H_CONTEXT *psPwHContext)
{
  if (psPwHContext == NULL)
  {
    return gpcContextAllocationError;
  }
  return psPwHContext->pcError;
}


/*-
 **********************************************************************
 *
 * PwHGetHint
 *
 **********************************************************************
 */
char *
PwHGetHint(PW_H_CONTEXT *psPwHContext)
{
  if (PwHContextIsValid(psPwHContext) == PATHWELL_FALSE)
  {
    PwHSetError(psPwHContext, "Invalid context. That should not happen.", NULL);
    return NULL;
  }
  PATHWELL_ASSERT(PwHContextIsValid(psPwHContext) == PATHWELL_TRUE);

  return psPwHContext->pcHint;
}


/*-
 **********************************************************************
 *
 * PwHGetNearbyPassword
 *
 **********************************************************************
 */
char *
PwHGetNearbyPassword(PW_H_CONTEXT *psPwHContext)
{
 if (PwHContextIsValid(psPwHContext) == PATHWELL_FALSE)
  {
    PwHSetError(psPwHContext, "Invalid context. That should not happen.", NULL);
    return NULL;
  }
  PATHWELL_ASSERT(PwHContextIsValid(psPwHContext) == PATHWELL_TRUE);

  return psPwHContext->pcNearbyPassword;
}


/*-
 **********************************************************************
 *
 * PwHGetNearbyTopology
 *
 **********************************************************************
 */
char *
PwHGetNearbyTopology(PW_H_CONTEXT *psPwHContext)
{
  if (PwHContextIsValid(psPwHContext) == PATHWELL_FALSE)
  {
    PwHSetError(psPwHContext, "Invalid context. That should not happen.", NULL);
    return NULL;
  }
  PATHWELL_ASSERT(PwHContextIsValid(psPwHContext) == PATHWELL_TRUE);

  return psPwHContext->pcNearbyTopology;
}
