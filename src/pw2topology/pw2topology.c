/*-
 ***********************************************************************
 *
 * $Id: pw2topology.c,v 1.36.2.1 2015/09/30 16:05:54 klm Exp $
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
  int iErrors = 0;
  int iFFlag = PATHWELL_FALSE;
  int iIndex = 0;
  int iNumFlags = 0;
  int iSFlag = PATHWELL_FALSE;
  PROPERTIES *psProperties = NULL;

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

  while ((c = getopt(iArgumentCount, ppcArgumentVector, "f:s:")) != -1)
  {
    switch (c)
    {
    case 'f':
      iFFlag = PATHWELL_TRUE;
      psProperties->pcInputFile = optarg;
      if (psProperties->pcInputFile == NULL)
      {
        AppUsage();
      }
      break;
    case 's':
      iSFlag = PATHWELL_TRUE;
      psProperties->pcTokenSet = optarg;
      if (psProperties->pcTokenSet == NULL || psProperties->pcTokenSet[0] == '-')
      {
        AppUsage();
      }
      break;
    default:
      AppUsage();
      break;
    }
    iNumFlags++;
  }

  /*
   *********************************************************************
   *
   * An input file, '-f' is optional. It can be '-' or a regular file.
   *
   *********************************************************************
   */
  if (iFFlag == PATHWELL_TRUE)
  {
    if (strcmp(psProperties->pcInputFile, "-") == 0)
    {
      psProperties->pInputFile = stdin;
    }
    else
    {
      psProperties->pInputFile = fopen(psProperties->pcInputFile, "r");
      if (psProperties->pInputFile == NULL)
      {
        fprintf(stderr,"%s: Error='Unable to open %s (No such file or directory).'\n", PROGRAM_NAME, psProperties->pcInputFile);
        AppCleanProperties(psProperties);
        exit(XER_Setup);
      }
    }
  }

  /*-
   **********************************************************************
   *
   * A token set ID, '-s' is optional.
   *
   *********************************************************************
   */
  if (iSFlag == PATHWELL_TRUE)
  {
    if
    (
         strlen(psProperties->pcTokenSet) <= floor(log10(abs(PATHWELL_MAX_TOKEN_SET_ID))) + 1
      && PATHWELL_MIN_TOKEN_SET_ID <= strtol(psProperties->pcTokenSet, NULL, 10)
      && strtol(psProperties->pcTokenSet, NULL, 10) <= PATHWELL_MAX_TOKEN_SET_ID
    )
    {
      psProperties->iTokenSet = atoi(psProperties->pcTokenSet);
    }
    else
    {
      fprintf(stderr, "Invalid token set ID: %d. Token set IDs must be in range [1-4].\n", atoi(psProperties->pcTokenSet));
      exit(XER_Setup);
    }
  }
  PwTSetTokenSet(psProperties->psPwTContext, psProperties->iTokenSet);

  /*
   *********************************************************************
   *
   * It's an error unless there is at least one argument or we have a
   * valid input file pointer.
   *
   *********************************************************************
   */
  if (iArgumentCount <= (2*iNumFlags)+1 && psProperties->pInputFile == NULL)
  {
    AppUsage();
  }

  /*
   *********************************************************************
   *
   * Convert passwords specified on the command line to topologies.
   *
   *********************************************************************
   */
  if (optind != iArgumentCount)
  {
    for (iIndex = optind; iIndex < iArgumentCount; iIndex++)
    {
      PwTSetPassword(psProperties->psPwTContext, ppcArgumentVector[iIndex]);
      PwTClearError(psProperties->psPwTContext);
      if (PwTPasswordToTopology(psProperties->psPwTContext) != ER_OK)
      {
        printf("Password='%s'; Topology='undef'; Error='%s';\n", PwTGetPassword(psProperties->psPwTContext), PwTGetError(psProperties->psPwTContext));
        iErrors++;
      }
      else
      {
        printf("Password='%s'; Topology='%s';\n", PwTGetPassword(psProperties->psPwTContext), PwTGetTopology(psProperties->psPwTContext));
      }
    }
  }

  /*
   *********************************************************************
   *
   * Conditionally convert passwords provided via file handle.
   *
   *********************************************************************
   */
  if (iFFlag == PATHWELL_TRUE)
  {
    while (fgets(acLine, PATHWELL_MAX_LINE_SIZE, psProperties->pInputFile) != NULL)
    {
      char *pcPassword = PwSChomp(acLine, "\n\r", PW_S_CHOMP_RIGHT);
      PwTSetPassword(psProperties->psPwTContext, pcPassword);
      if (pcPassword != NULL)
      {
        free(pcPassword);
      }
      PwTClearError(psProperties->psPwTContext);
      if (PwTPasswordToTopology(psProperties->psPwTContext) != ER_OK)
      {
        printf("Password='%s'; Topology='undef'; Error='%s';\n", PwTGetPassword(psProperties->psPwTContext), PwTGetError(psProperties->psPwTContext));
        iErrors++;
      }
      else
      {
        printf("Password='%s'; Topology='%s';\n", PwTGetPassword(psProperties->psPwTContext), PwTGetTopology(psProperties->psPwTContext));
      }
    }
  }

  /*
   *********************************************************************
   *
   * Shut down and go home.
   *
   *********************************************************************
   */
  AppCleanProperties(psProperties);

  if (iErrors > 0)
  {
    return XER_Conversion;
  }

  return XER_OK;
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
    if (psProperties->pInputFile != NULL)
    {
      fclose(psProperties->pInputFile);
    }
    PwTFreeContext(psProperties->psPwTContext);
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
  psProperties->iTokenSet = PATHWELL_DEFAULT_TOKEN_SET_ID;
  psProperties->psPwTContext = PwTNewContext();
  if (psProperties->psPwTContext == NULL)
  {
    AppCleanProperties(psProperties);
    return NULL;
  }

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
  fprintf(stderr, "Usage: %s [-f {file|-}] [-s token-set] [password [...]]\n", PROGRAM_NAME);
  fprintf(stderr, "\n");
  fprintf(stderr, "Where options can be any of the following:\n");
  fprintf
  (
    stderr,
    "\n\
  -f <file>\n\
                        File containing items, one per line, to be converted.\n\
                        A value of '-' causes the program to read from stdin.\n\
  -s <token-set>\n\
                        The ID (1-4) of the token set to use for conversions.\n\
                        The following token sets are supported: 1) [dlus], 2)\n\
                        [dlusf], 3) [dlusfx], and 4) [dlusfxH]. The default\n\
                        value is '1'.\n\
\n"
  );
  fprintf(stderr, "\n");
  exit(XER_Usage);
}
