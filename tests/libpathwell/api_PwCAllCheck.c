/*-
 ***********************************************************************
 *
 * $Id: api_PwCAllCheck.c,v 1.2 2017/04/24 14:18:25 ssegreti Exp $
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
#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif
#include <pathwell.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tap.h>

typedef struct _TEST_TUPLES
{
  char acOldPassword[PATHWELL_MAX_PASSWORD_SIZE];
  char acNewPassword[PATHWELL_MAX_PASSWORD_SIZE];
  int iCheckCode;
  int iBlacklistEnabled;
  int iMaxUse;
  int iMinLen;
  int iMinLev;
} TEST_TUPLES;

int
main(int iArgumentCount, char *ppcArgumentVector[])
{
  int iBlacklistEnabled = PATHWELL_FALSE;
  int iError = 0;
  int iIndex = 0;
  int iMaxUse = 0;
  int iMinLen = 0;
  int iMinLev = 0;
  PW_D_CONTEXT *psPwDContext = NULL;
  PW_C_CONTEXT *psPwCContext = NULL;
  PW_T_CONTEXT *psNewPwTContext = NULL;
  PW_T_CONTEXT *psOldPwTContext = NULL;
  TEST_TUPLES asTuples[] =
  {
    { "", "mypassword1", PATHWELL_ALLCHECK_FAIL_BLACKLIST, PATHWELL_TRUE, 0, 0, 0},
    { "", "mypassword1", PATHWELL_ALLCHECK_FAIL_MINLEN, PATHWELL_FALSE, 0, 99, 0},
    { "mypassword", "mypassword1", PATHWELL_ALLCHECK_FAIL_MINLEV, PATHWELL_FALSE, 0, 0, 5},
    { "", "mypassword1", PATHWELL_ALLCHECK_FAIL_BLACKLIST, PATHWELL_TRUE, 0, 0, 0}
  };

  plan_tests(4); //adjust this later

  // establish a database context, and fill it with topologies from the file
  psPwDContext = PwDNewContextFromParameters(":memory:", SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL, 0);
  //ok(PwDContextIsValid(psPwDContext), "%s", "new context");
  if (!PwDContextIsValid(psPwDContext))
  {
    diag("%s", PwDGetError(psPwDContext));
  }

  iError = PwDAcquireConnection(psPwDContext);
  //ok(iError == 0, "%s", "acquire connection");
  if (iError != ER_OK)
  {
    diag("%s", PwDGetError(psPwDContext));
  }

  iError = PwDCreateSchema(psPwDContext);
  //ok(iError == 0, "%s", "create schema");
  if (iError != ER_OK)
  {
    diag("%s", PwDGetError(psPwDContext));
  }

  iError = PwDLoadBlacklistedTopologies(psPwDContext, ppcArgumentVector[1]);
  //ok(iError == 0, "%s", "load blacklisted topologies");
  if (iError != ER_OK)
  {
    diag("%s", PwDGetError(psPwDContext));
  }

  for (iIndex = 0; iIndex < (sizeof(asTuples) / sizeof(asTuples[0])); iIndex++)
  {
    iBlacklistEnabled = asTuples[iIndex].iBlacklistEnabled;
    iMaxUse = asTuples[iIndex].iMaxUse;
    iMinLen = asTuples[iIndex].iMinLen;
    iMinLev = asTuples[iIndex].iMinLev;

    //convert the new password to a topology and an ID
    psNewPwTContext = PwTNewContextFromParameters(PATHWELL_DEFAULT_ENCODING, PATHWELL_DEFAULT_TOKEN_SET_ID, asTuples[iIndex].acNewPassword, NULL, NULL);
    if (PwTContextIsValid(psNewPwTContext) != PATHWELL_TRUE)
    {
      diag("%s", PwTGetError(psNewPwTContext));
    }

    if (PwTPasswordToTopology(psNewPwTContext) != ER_OK)
    {
      diag("%s", PwTGetError(psNewPwTContext));
    }
    if (PwTTopologyToId(psNewPwTContext) != ER_OK)
    {
      diag("%s", PwTGetError(psNewPwTContext));
    }

    psOldPwTContext = PwTNewContextFromParameters(PATHWELL_DEFAULT_ENCODING, PATHWELL_DEFAULT_TOKEN_SET_ID, asTuples[iIndex].acOldPassword, NULL, NULL);
    if (PwTContextIsValid(psOldPwTContext) != PATHWELL_TRUE)
    {
      diag("%s", PwTGetError(psOldPwTContext));
    }

    if (PwTPasswordToTopology(psOldPwTContext) != ER_OK)
    {
      diag("%s", PwTGetError(psOldPwTContext));
    }
    if (PwTTopologyToId(psOldPwTContext) != ER_OK)
    {
      diag("%s", PwTGetError(psOldPwTContext));
    }

    //set up the pathwell checking context
    psPwCContext = PwCNewContextFromParameters(psPwDContext, psOldPwTContext, psNewPwTContext, iMaxUse, iMinLen, iMinLev, iBlacklistEnabled);
    //ok(PwCContextIsValid(psPwCContext) == PATHWELL_TRUE, "%s %s.", "creating PwC context for", asTuples[iIndex].acRejectedPassword);
    if (PwCContextIsValid(psPwCContext) != PATHWELL_TRUE)
    {
      diag("%s", PwCGetError(psPwCContext));
    }

    if (PwCAllCheck(psPwCContext) != ER_OK)
    {
      diag("PwCAllCheck failed. Error: %s", PwCGetError(psPwCContext));
    }

    ok(PwCGetCheckCode(psPwCContext) == asTuples[iIndex].iCheckCode, "Checking password pair \"%s\", \"%s\". Check code was %d", asTuples[iIndex].acOldPassword, asTuples[iIndex].acNewPassword, PwCGetCheckCode(psPwCContext));

    //clean up
    PwCFreeContext(&psPwCContext);
    PwTFreeContext(&psOldPwTContext);
    PwTFreeContext(&psNewPwTContext);
  }

  iError = PwDReleaseConnection(psPwDContext);
  //ok(iError == 0, "%s", "release connection");
  if (iError != ER_OK)
  {
    diag("%s", PwDGetError(psPwDContext));
  }

  PwDFreeContext(&psPwDContext);

  return exit_status();
}
