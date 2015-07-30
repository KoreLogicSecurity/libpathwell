/*-
 ***********************************************************************
 *
 * $Id: api_PwDVerifySchema.c,v 1.3 2013/11/04 15:06:44 klm Exp $
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
#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif
#include <pathwell.h>
#include <stdio.h>
#include <stdint.h>
#include <tap.h>

int
main(int iArgumentCount, char *ppcArgumentVector[])
{
  int iError = 0;
  int iIndex = 0;
  PW_D_CONTEXT *psPwDContext = NULL;

  plan_tests(10);

  psPwDContext = PwDNewContextFromParameters(":memory:", SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL, 0);
  ok(PwDContextIsValid(psPwDContext), "new context");
  if (!PwDContextIsValid(psPwDContext))
  {
    diag(PwDGetError(psPwDContext));
  }

  iError = PwDAcquireConnection(psPwDContext);
  ok(iError == 0, "acquire connection");
  if (iError != ER_OK)
  {
    diag(PwDGetError(psPwDContext));
  }

  iError = PwDVerifySchema(psPwDContext);
  ok(iError != 0 && strcmp(PwDGetError(psPwDContext), "Failed to prepare SQL statement (no such table: version).") == 0, "verify schema that does not exist");
//if (iError != ER_OK)
//{
//  diag(PwDGetError(psPwDContext));
//}

  iError = PwDCreateSchema(psPwDContext);
  ok(iError == 0, "create schema");
  if (iError != ER_OK)
  {
    diag(PwDGetError(psPwDContext));
  }

  iError = PwDVerifySchema(psPwDContext);
  ok(iError == 0, "verify schema");
  if (iError != ER_OK)
  {
    diag(PwDGetError(psPwDContext));
  }

  iError = PwDSetSchemaVersion(psPwDContext, PW_V_DB_SCHEMA_VERSION + 1, PATHWELL_FALSE);
  ok(iError != 0 && strncmp(PwDGetError(psPwDContext), "DB version mismatch", strlen("DB version mismatch")) == 0, "set schema version to an improper value");
//if (iError != ER_OK)
//{
//  diag(PwDGetError(psPwDContext));
//}

  iError = PwDSetSchemaVersion(psPwDContext, PW_V_DB_SCHEMA_VERSION, PATHWELL_TRUE);
  ok(iError == 0, "set schema version to an improper value (forcibly)");
  if (iError != ER_OK)
  {
    diag(PwDGetError(psPwDContext));
  }

  iError = PwDVerifySchema(psPwDContext);
  ok(iError == 0, "verify schema version set to an improper value");
  if (iError != ER_OK)
  {
    diag(PwDGetError(psPwDContext));
  }

  iError = PwDDeleteSchema(psPwDContext);
  ok(iError == 0, "delete schema");
  if (iError != ER_OK)
  {
    diag(PwDGetError(psPwDContext));
  }

  iError = PwDReleaseConnection(psPwDContext);
  ok(iError == 0, "release connection");
  if (iError != ER_OK)
  {
    diag(PwDGetError(psPwDContext));
  }

  PwDFreeContext(psPwDContext);

  return exit_status();
}
