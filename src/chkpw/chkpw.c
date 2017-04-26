/*-
 ***********************************************************************
 *
 * $Id: chkpw.c,v 1.42 2017/04/21 19:04:34 klm Exp $
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
 * Main
 *
 ***********************************************************************
 */
int
main(int iArgumentCount, char *ppcArgumentVector[])
{
  char acLine[PATHWELL_MAX_LINE_SIZE] = "";
  char c = 0;
  char *pcValue = NULL;
  const char *pcError = NULL;
  const char *pcHint = NULL;
  int iCheckValue = 0;
  int iError = XER_OK;
  int iPasswordOk = PATHWELL_FALSE;
  int iValueIndex = 0;
  int iValueLength = 0;
  PROPERTIES *psProperties = NULL;
  unsigned int uiCheckCode = 0;

  /*-
   *********************************************************************
   *
   * Punch in and go to work.
   *
   *********************************************************************
   */
  opterr = 0;

  psProperties = AppNewProperties();
  if (psProperties == NULL)
  {
    fprintf(stderr, "%s: Error='Program failed to initialize.'\n", PROGRAM_NAME);
    return XER_Bootstrap;
  }

  /*-
   *********************************************************************
   *
   * Process command line arguments.
   *
   *********************************************************************
   */
  if (iArgumentCount < 2)
  {
    AppUsage();
  }

  while ((c = getopt(iArgumentCount, ppcArgumentVector, "bd:e:h:iL:l:mp:S:s:t:u:")) != -1)
  {
    switch (c)
    {
    case 'b':
      psProperties->iBlacklist = PATHWELL_TRUE;
      break;
    case 'd':
      psProperties->pcDbFile = optarg;
      if (psProperties->pcDbFile == NULL)
      {
        AppUsage();
      }
      break;
    case 'e':
      pcValue = optarg;
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
        fprintf(stderr, "%s: Error='The \"environment\" option must be an integer in the range [%d-%d].'\n", PROGRAM_NAME, PATHWELL_ENV_MIN, PATHWELL_ENV_MAX);
        exit(XER_Setup);
      }
      psProperties->iEnvironment = atoi(pcValue);
      if
      (
            psProperties->iEnvironment < PATHWELL_ENV_MIN
         || psProperties->iEnvironment > PATHWELL_ENV_MAX
      )
      {
        fprintf(stderr, "%s: Error='The \"environment\" option must be an integer in the range [%d-%d].'\n", PROGRAM_NAME, PATHWELL_ENV_MIN, PATHWELL_ENV_MAX);
        exit(XER_Setup);
      }
      break;
    case 'h':
      pcValue = optarg;
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
        fprintf(stderr, "%s: Error='The \"hint-info-level\" option must be an integer in the range [%d-%d].'\n", PROGRAM_NAME, PATHWELL_HINT_INFO_LEVEL_MIN, PATHWELL_HINT_INFO_LEVEL_MAX);
        exit(XER_Setup);
      }
      psProperties->iHintInfoLevel = atoi(pcValue);
      if
      (
           psProperties->iHintInfoLevel < PATHWELL_HINT_INFO_LEVEL_MIN
        || psProperties->iHintInfoLevel > PATHWELL_HINT_INFO_LEVEL_MAX
      )
      {
        fprintf(stderr, "%s: Error='The \"hint-info-level\" option must be an integer in the range [%d-%d].'\n", PROGRAM_NAME, PATHWELL_HINT_INFO_LEVEL_MIN, PATHWELL_HINT_INFO_LEVEL_MAX);
        exit(XER_Setup);
      }
      break;
    case 'i':
      psProperties->iIncrementUseCount = PATHWELL_TRUE;
      break;
    case 'L':
      pcValue = optarg;
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
        fprintf(stderr, "%s: Error='The \"minlev\" option must be an integer greater than or equal to zero.'\n", PROGRAM_NAME);
        exit(XER_Setup);
      }
      psProperties->iMinLev = atoi(pcValue);
      break;
    case 'l':
      pcValue = optarg;
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
        fprintf(stderr, "%s: Error='The \"minlen\" option must be an integer greater than or equal to zero.'\n", PROGRAM_NAME);
        exit(XER_Setup);
      }
      psProperties->iMinLen = atoi(pcValue);
      break;
    case 'm':
      psProperties->iDeleteAllowed = 1;
      break;
    case 'p':
      pcValue = optarg;
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
        fprintf(stderr, "%s: Error='The \"hint-pres-level\" option must be an integer in the range [%d-%d].'\n", PROGRAM_NAME, PATHWELL_HINT_PRES_LEVEL_MIN, PATHWELL_HINT_PRES_LEVEL_MAX);
        exit(XER_Setup);
      }
      psProperties->iHintPresLevel = atoi(pcValue);
      if
      (
           psProperties->iHintPresLevel < PATHWELL_HINT_PRES_LEVEL_MIN
        || psProperties->iHintPresLevel > PATHWELL_HINT_PRES_LEVEL_MAX
      )
      {
        fprintf(stderr, "%s: Error='The \"hint-pres-level\" option must be an integer in the range [%d-%d].'\n", PROGRAM_NAME, PATHWELL_HINT_PRES_LEVEL_MIN, PATHWELL_HINT_PRES_LEVEL_MAX);
        exit(XER_Setup);
      }
      break;
    case 'S':
      pcValue = optarg;
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
        fprintf (stderr, "%s: Error='The \"seed\" option must be an integer in the range [%d-%d].'\n", PROGRAM_NAME, CHKPW_SEED_MIN, CHKPW_SEED_MAX);
        exit(XER_Setup);
      }
      psProperties->uiSeed = atoi(pcValue);
      if
      (
           psProperties->uiSeed < CHKPW_SEED_MIN
        || psProperties->uiSeed > CHKPW_SEED_MAX
      )
      {
        fprintf (stderr, "%s: Error='The \"seed\" option must be an integer in the range [%d-%d].'\n", PROGRAM_NAME, CHKPW_SEED_MIN, CHKPW_SEED_MAX);
        exit(XER_Setup);
      }
      break;
    case 's':
      pcValue = optarg;
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
        fprintf (stderr, "%s: Error='The \"token-set\" option must be an integer in the range [%d-%d].'\n", PROGRAM_NAME, PATHWELL_MIN_TOKEN_SET_ID, PATHWELL_MAX_TOKEN_SET_ID);
        exit(XER_Setup);
      }
      psProperties->iTokenSet = atoi(pcValue);
      if
      (
            psProperties->iTokenSet < PATHWELL_MIN_TOKEN_SET_ID
         || psProperties->iTokenSet > PATHWELL_MAX_TOKEN_SET_ID
      )
      {
        fprintf (stderr, "%s: Error='The \"token-set\" option must be an integer in the range [%d-%d].'\n", PROGRAM_NAME, PATHWELL_MIN_TOKEN_SET_ID, PATHWELL_MAX_TOKEN_SET_ID);
        exit(XER_Setup);
      }
      if (psProperties->iTokenSet != PATHWELL_TOKEN_SET_ID_1)
      {
        fprintf (stderr, "%s: Error='Support for token set %d is currently under development. Only token set %d is supported at this time.'\n", PROGRAM_NAME, psProperties->iTokenSet, PATHWELL_TOKEN_SET_ID_1);
        exit(XER_Setup);
      }
      break;
    case 't':
      psProperties->pcEncoding = optarg;
      if (strcasecmp(psProperties->pcEncoding, "baseN+1") == 0)
      {
        psProperties->iEncoding = PATHWELL_ENCODING_BASENP1;
      }
      else if (strcasecmp(psProperties->pcEncoding, "bitmask") == 0)
      {
        psProperties->iEncoding = PATHWELL_ENCODING_BITMASK;
      }
      else
      {
        fprintf(stderr, "%s: Error='The \"encoding-type\" option must be one of \"baseN+1\" or \"bitmask\".'\n", PROGRAM_NAME);
        exit(XER_Setup);
      }
      break;
    case 'u':
      pcValue = optarg;
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
        fprintf(stderr, "%s: Error='The \"maxuse\" option must be an integer greater than or equal to zero.'\n", PROGRAM_NAME);
        exit(XER_Setup);
      }
      psProperties->iMaxUse = atoi(pcValue);
      break;
    default:
      AppUsage();
      break;
    }
  }

  /*
   *********************************************************************
   *
   * It's an error unless two arguments remain.
   *
   *********************************************************************
   */
  if (iArgumentCount - optind != 2)
  {
    AppUsage();
  }

  /*
   *********************************************************************
   *
   * Establish a database context, and make sure the database exists.
   *
   *********************************************************************
   */
  if (!PwDDbFileExists(psProperties->pcDbFile))
  {
    fprintf(stderr, "%s: Error='The specified database does not exist.'\n", PROGRAM_NAME);
    exit(XER_Setup);
  }

  psProperties->psPwDContext = PwDNewContextFromParameters(psProperties->pcDbFile, SQLITE_OPEN_READWRITE, NULL, 0);

  if (!PwDContextIsValid(psProperties->psPwDContext))
  {
    fprintf(stderr, "%s: Error='%s'\n", PROGRAM_NAME, PwDGetError(psProperties->psPwDContext));
    exit(XER_Setup);
  }

  /*
   *********************************************************************
   *
   * Convert the old password to a topology and an ID.
   *
   *********************************************************************
   */
  psProperties->pcOldPassword = ppcArgumentVector[optind];
  psProperties->psOldPwTContext = PwTNewContextFromParameters(psProperties->iEncoding,
                                    psProperties->iTokenSet,
                                    psProperties->pcOldPassword,
                                    NULL,
                                    NULL
                                    );
  if (!PwTContextIsValid(psProperties->psOldPwTContext))
  {
    fprintf(stderr, "%s: Error='%s'\n", PROGRAM_NAME, PwTGetError(psProperties->psOldPwTContext));
    exit(XER_Setup);
  }
  if (PwTPasswordToTopology(psProperties->psOldPwTContext) != ER_OK)
  {
    fprintf(stderr, "%s: Error='%s'\n", PROGRAM_NAME, PwTGetError(psProperties->psOldPwTContext));
    exit(XER_Setup);
  }
  if (PwTTopologyToId(psProperties->psOldPwTContext) != ER_OK)
  {
    fprintf(stderr, "%s: Error='%s'\n", PROGRAM_NAME, PwTGetError(psProperties->psOldPwTContext));
    exit(XER_Setup);
  }
  optind++;

  /*
   *********************************************************************
   *
   * Convert the new password to a topology and an ID.
   *
   *********************************************************************
   */
  psProperties->pcNewPassword = ppcArgumentVector[optind];
  psProperties->psNewPwTContext = PwTNewContextFromParameters(
                                    psProperties->iEncoding,
                                    psProperties->iTokenSet,
                                    psProperties->pcNewPassword,
                                    NULL,
                                    NULL
                                    );
  if (psProperties->psNewPwTContext == NULL)
  {
    fprintf(stderr, "%s: Error='%s'\n", PROGRAM_NAME, PwTGetError(psProperties->psNewPwTContext));
    exit(XER_Setup);
  }
  if (PwTPasswordToTopology(psProperties->psNewPwTContext) != ER_OK)
  {
    fprintf(stderr, "%s: Error='%s'\n", PROGRAM_NAME, PwTGetError(psProperties->psNewPwTContext));
    exit(XER_Setup);
  }
  if (PwTTopologyToId(psProperties->psNewPwTContext) != ER_OK)
  {
    fprintf(stderr, "%s: Error='%s'\n", PROGRAM_NAME, PwTGetError(psProperties->psNewPwTContext));
    exit(XER_Setup);
  }
  optind++;


  /*-
   *********************************************************************
   *
   * Set up the PathWell checking context.
   *
   *********************************************************************
   */
  psProperties->psPwCContext = PwCNewContextFromParameters(psProperties->psPwDContext,
                                 psProperties->psOldPwTContext,
                                 psProperties->psNewPwTContext,
                                 psProperties->iMaxUse,
                                 psProperties->iMinLen,
                                 psProperties->iMinLev,
                                 psProperties->iBlacklist
                                 );
  if (PwCContextIsValid(psProperties->psPwCContext) != PATHWELL_TRUE)
  {
    pcError = PwCGetError(psProperties->psPwCContext);
    fprintf(stderr, "%s: Couldn't create PwC context. Error='%s'\n", PROGRAM_NAME, pcError);
    iCheckValue = APP_SERVICE_ERR;
    goto GAME_OVER;
  }

  /*-
   *********************************************************************
   *
   * Perform the actual PathWell checks.
   *
   *********************************************************************
   */
  if (PwCAllCheck(psProperties->psPwCContext) != ER_OK)
  {
    pcError = PwCGetError(psProperties->psPwCContext);
    fprintf(stderr, "%s: Couldn't perform the PathWell checks. Error='%s'\n", PROGRAM_NAME, pcError);
    iCheckValue = APP_SERVICE_ERR;
    goto GAME_OVER;
  }

  uiCheckCode = PwCGetCheckCode(psProperties->psPwCContext);

  /*-
   *********************************************************************
   *
   * Conditionally generate a hint if one or more of the PathWell checks
   * failed.
   *
   *********************************************************************
   */
  if (uiCheckCode != PATHWELL_ALLCHECK_PASS)
  {
    psProperties->psPwHContext = PwHNewContextFromParameters(psProperties->iDeleteAllowed,
                                   psProperties->iHintInfoLevel,
                                   psProperties->iHintPresLevel,
                                   psProperties->iEnvironment,
                                   psProperties->uiSeed
                                   );
    if (PwHContextIsValid(psProperties->psPwHContext) != PATHWELL_TRUE)
    {
      pcError = PwHGetError(psProperties->psPwHContext);
      fprintf(stderr, "%s: Invalid PwHContext. Error='%s'\n", PROGRAM_NAME, pcError);
      iCheckValue = APP_SERVICE_ERR;
      goto GAME_OVER;
    }

    if (PwHGenHint(psProperties->psPwHContext, psProperties->psPwCContext) != ER_OK)
    {
      pcError = PwHGetError(psProperties->psPwHContext);
      fprintf(stderr, "%s: Couldn't generate a hint. Error='%s'\n", PROGRAM_NAME, pcError);
      iCheckValue = APP_SERVICE_ERR;
      goto GAME_OVER;
    }
    else
    {
      pcHint = PwHGetHint(psProperties->psPwHContext);
      goto GAME_OVER;
    }
    PwHFreeContext(&psProperties->psPwHContext); // should'nt ever reach here
  }


  /*
   *********************************************************************
   *
   * If we made it this far, the new password is OK.
   *
   *********************************************************************
   */
  iPasswordOk = PATHWELL_TRUE;

  /*-
   *********************************************************************
   *
   * Conditionally increment the use count.
   *
   *********************************************************************
   */
  if (psProperties->iIncrementUseCount)
  {
    iError = PwDIncrementUseCount(psProperties->psPwDContext, psProperties->psNewPwTContext);
    if (iError != ER_OK)
    {
      pcError = PwDGetError(psProperties->psPwDContext);
      iCheckValue = APP_SERVICE_ERR;
      goto GAME_OVER;
    }
  }

  /*
   *********************************************************************
   *
   * If we landed here, it's either because there was an error or the
   * new password is not OK.
   *
   *********************************************************************
   */
GAME_OVER:
  if (iCheckValue == APP_SERVICE_ERR)
  {
    fprintf(stderr, "%s: Error='%s'\n", PROGRAM_NAME, pcError);
    iError = XER_Check;
  }
  else
  {
    fprintf(stdout, "OldPassword='%s';\n", psProperties->pcOldPassword);
    fprintf(stdout, "OldTopology='%s';\n", PwTGetTopology(psProperties->psOldPwTContext));
    fprintf(stdout, "OldId='%" PRId64 "';\n", PwTGetId(psProperties->psOldPwTContext));
    fprintf(stdout, "NewPassword='%s';\n", psProperties->pcNewPassword);
    fprintf(stdout, "NewTopology='%s';\n", PwTGetTopology(psProperties->psNewPwTContext));
    fprintf(stdout, "NewId='%" PRId64 "';\n", PwTGetId(psProperties->psNewPwTContext));
    fprintf(stdout, "PasswordOk='%s';\n", (iPasswordOk == PATHWELL_TRUE) ? "true" : "false");
    fprintf(stdout, "PasswordHint='%s';\n", (pcHint == NULL) ? "" : pcHint);
  }

  /*
   *********************************************************************
   *
   * Shut down and go home.
   *
   *********************************************************************
   */
  AppCleanProperties(psProperties);

  return iError;
}


/*-
 ***********************************************************************
 *
 * AppCleanProperties
 *
 ***********************************************************************
 */
void
AppCleanProperties(PROPERTIES *psProperties)
{
  if (psProperties != NULL)
  {
    PwCFreeContext(&psProperties->psPwCContext);
    PwHFreeContext(&psProperties->psPwHContext);
    PwDFreeContext(&psProperties->psPwDContext);
    PwLFreeContext(&psProperties->psPwLContext);
    PwTFreeContext(&psProperties->psOldPwTContext);
    PwTFreeContext(&psProperties->psNewPwTContext);
    free(psProperties);
  }
  return;
}


/*-
 ***********************************************************************
 *
 * AppNewProperties
 *
 ***********************************************************************
 */
PROPERTIES *
AppNewProperties(void)
{
  PROPERTIES *psProperties = (PROPERTIES *)calloc(1, sizeof(PROPERTIES));

  if (psProperties == NULL)
  {
    return NULL;
  }
  psProperties->iBlacklist = PATHWELL_FALSE;
  psProperties->iDeleteAllowed = PATHWELL_HINT_MOD_DELETE_ALLOWED_DEFAULT;
  psProperties->iEncoding = PATHWELL_DEFAULT_ENCODING;
  psProperties->iEnvironment = PATHWELL_ENV_DEFAULT;
  psProperties->iHintInfoLevel = PATHWELL_HINT_INFO_LEVEL_MIN;
  psProperties->iHintPresLevel = PATHWELL_HINT_PRES_LEVEL_MIN;
  psProperties->iTokenSet = PATHWELL_DEFAULT_TOKEN_SET_ID;
  psProperties->uiSeed = CHKPW_DEFAULT_SEED;

  return psProperties;
}


/*-
 ***********************************************************************
 *
 * AppUsage
 *
 ***********************************************************************
 */
void
AppUsage(void)
{
  fprintf(stderr, "\n");
  fprintf(stderr, "Usage: %s [-bim] [-e environment] [-h hint-info-level] [-L minlev] [-l minlen] [-p hint-pres-level] [-s token-set] [-t encoding-type] [-u maxuse] -d db old-password new-password\n", PROGRAM_NAME);
  fprintf(stderr, "\n");
  fprintf(stderr, "Where options can be any of the following:\n");
  fprintf
  (
    stderr,
    "\n\
  -b\n\
                        Enable blacklist checking.\n\
  -d <db>\n\
                        Path and name of the database that is to be consulted\n\
                        when performing PathWell checks.\n\
  -e <environment>\n\
                        Environment where application is meant to be used.\n\
                        The following environments are supported: 0) CLI and \n\
                        1) JavaScript. The default environment is CLI (i.e.,\n\
                        the default value is '0'). Setting the environment to\n\
                        JavaScript will produce JSON-formatted output.\n\
  -h <hint-info-level>\n\
                        Desired hinting information level. The supported levels\n\
                        (listed below) provide information indicating:\n\
                          1) only that a check failed (default)\n\
                          2) rejected topology / suggested topology\n\
                          3) rejected password / suggested topology\n\
                          4) rejected password / suggested password\n\
  -i\n\
                        Increment the use count if the new password satisfies\n\
                        all enabled checks. If no checks are enabled, the use\n\
                        count is updated as a matter of course.\n\
  -L <minlev>\n\
                        Minimun Levenshtein distance required for topologies\n\
                        derived from candidate passwords. This option is\n\
                        disabled by default (i.e., the default value is '0').\n\
  -l <minlen>\n\
                        Minimun length required for candidate passwords.\n\
                        This option is disabled by default (i.e., the default\n\
                        value is '0').\n\
  -m\n\
                        Enable deletions as a suggested password modification.\n\
                        This option is disabled by default.\n\
  -p <hint-pres-level>\n\
                        Desired hinting presentation level. The supported\n\
                        levels (listed below) render the hint suggestion as:\n\
                          1) direct printing of topology/password (default)\n\
                          2) pictorial representation of change(s)\n\
                          3) both 1 and 2\n\
  -S <seed>\n\
                        Seed for the PRNG, which, is used, in turn, to influence\n\
                        the output produced by hinting information levels 2-4.\n\
                        Any positive 32-bit integer value is permitted. If not\n\
                        specified, the seed will be selected automatically.\n\
                        This option is useful for testing where deterministic\n\
                        results are desired.\n\
  -s <token-set>\n\
                        ID of the token set to use for performing conversions.\n\
                        The following token sets are supported:\n\
                          1) [dlus] (default)\n\
                          2) [dlusf] (under development)\n\
                          3) [dlusfx] (under development)\n\
                          4) [dlusfxH] (under development)\n\
  -t <encoding-type>\n\
                        The type of encoding to use for conversions. Must be\n\
                        one of 'baseN+1' or 'bitmask'. The default value is\n\
                        'baseN+1'.\n\
  -u <maxuse>\n\
                        Maximum use count allowed for topologies derived from\n\
                        candidate passwords. This option is disabled by default\n\
                        (i.e., the default value is '0').\n\
\n"
  );
  fprintf(stderr, "\n");
  exit(XER_Usage);
}
