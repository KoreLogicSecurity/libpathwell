/*-
 ***********************************************************************
 *
 * $Id: api_PwDDeleteSchema.c,v 1.1 2013/08/26 13:36:11 klm Exp $
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

  plan_tests(5);

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

  iError = PwDCreateSchema(psPwDContext);
  ok(iError == 0, "create schema");
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
