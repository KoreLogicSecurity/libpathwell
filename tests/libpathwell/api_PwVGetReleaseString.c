/*-
 ***********************************************************************
 *
 * $Id: api_PwVGetReleaseString.c,v 1.2 2013/08/16 01:10:04 klm Exp $
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
#include <tap.h>

int
main(int iArgumentCount, char *ppcArgumentVector[])
{
  const char *pcRelease = NULL;

  plan_tests(1);
  pcRelease = PwVGetReleaseString();
  ok(iArgumentCount == 2 && strcmp(pcRelease, ppcArgumentVector[1]) == 0, "release string");

  return exit_status();
}
