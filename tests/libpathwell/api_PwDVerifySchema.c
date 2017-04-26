/*-
 ***********************************************************************
 *
 * $Id: api_PwDVerifySchema.c,v 1.7 2017/04/20 13:23:39 klm Exp $
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
#include <string.h>
#include <tap.h>

int
main(int iArgumentCount, char *ppcArgumentVector[])
{
  int iError = 0;
  int iIndex = 0;
  PW_D_CONTEXT *psPwDContext = NULL;

  plan_tests(10);

  psPwDContext = PwDNewContextFromParameters(":memory:", SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL, 0);
  ok(PwDContextIsValid(psPwDContext), "%s", "new context");
  if (!PwDContextIsValid(psPwDContext))
  {
    diag("%s", PwDGetError(psPwDContext));
  }

  iError = PwDAcquireConnection(psPwDContext);
  ok(iError == 0, "%s", "acquire connection");
  if (iError != ER_OK)
  {
    diag("%s", PwDGetError(psPwDContext));
  }

  iError = PwDVerifySchema(psPwDContext);
  ok(iError != 0 && strcmp(PwDGetError(psPwDContext), "Failed to prepare SQL statement (no such table: version).") == 0, "%s", "verify schema that does not exist");
//if (iError != ER_OK)
//{
//  diag("%s", PwDGetError(psPwDContext));
//}

  iError = PwDCreateSchema(psPwDContext);
  ok(iError == 0, "%s", "create schema");
  if (iError != ER_OK)
  {
    diag("%s", PwDGetError(psPwDContext));
  }

  iError = PwDVerifySchema(psPwDContext);
  ok(iError == 0, "%s", "verify schema");
  if (iError != ER_OK)
  {
    diag("%s", PwDGetError(psPwDContext));
  }

  iError = PwDSetSchemaVersion(psPwDContext, PW_V_DB_SCHEMA_VERSION + 1, PATHWELL_FALSE);
  ok(iError != 0 && strncmp(PwDGetError(psPwDContext), "DB version mismatch", strlen("DB version mismatch")) == 0, "%s", "set schema version to an improper value");
//if (iError != ER_OK)
//{
//  diag("%s", PwDGetError(psPwDContext));
//}

  iError = PwDSetSchemaVersion(psPwDContext, PW_V_DB_SCHEMA_VERSION, PATHWELL_TRUE);
  ok(iError == 0, "%s", "set schema version to an improper value (forcibly)");
  if (iError != ER_OK)
  {
    diag("%s", PwDGetError(psPwDContext));
  }

  iError = PwDVerifySchema(psPwDContext);
  ok(iError == 0, "%s", "verify schema version set to an improper value");
  if (iError != ER_OK)
  {
    diag("%s", PwDGetError(psPwDContext));
  }

  iError = PwDDeleteSchema(psPwDContext);
  ok(iError == 0, "%s", "delete schema");
  if (iError != ER_OK)
  {
    diag("%s", PwDGetError(psPwDContext));
  }

  iError = PwDReleaseConnection(psPwDContext);
  ok(iError == 0, "%s", "release connection");
  if (iError != ER_OK)
  {
    diag("%s", PwDGetError(psPwDContext));
  }

  PwDFreeContext(&psPwDContext);

  return exit_status();
}
