/*-
 ***********************************************************************
 *
 * $Id: api.c,v 1.243.2.6 2015/09/30 19:11:21 klm Exp $
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
 * Global variables
 *
 ***********************************************************************
 */
static char const *gpcContextAllocationError = "context allocation error";
static char const *gpcRealErrorLostNullFormat = "real error lost due to a format error";
static char const *gpcRealErrorLostNullMalloc = "real error lost due to a memory error";
static int giPwDLibraryReferenceCount = 0;
static PW_D_CANNED_SQL gasCannedSql[] =
{
  {
    "base4p1",
    "CREATE TABLE IF NOT EXISTS base4p1 (id INTEGER PRIMARY KEY NOT NULL, use_count INTEGER DEFAULT 0 NOT NULL)",
    "DROP TABLE IF EXISTS base4p1"
  },
  {
    "base5p1",
    "CREATE TABLE IF NOT EXISTS base5p1 (id INTEGER PRIMARY KEY NOT NULL, use_count INTEGER DEFAULT 0 NOT NULL)",
    "DROP TABLE IF EXISTS base5p1"
  },
  {
    "base6p1",
    "CREATE TABLE IF NOT EXISTS base6p1 (id INTEGER PRIMARY KEY NOT NULL, use_count INTEGER DEFAULT 0 NOT NULL)",
    "DROP TABLE IF EXISTS base6p1"
  },
  {
    "base7p1",
    "CREATE TABLE IF NOT EXISTS base7p1 (id INTEGER PRIMARY KEY NOT NULL, use_count INTEGER DEFAULT 0 NOT NULL)",
    "DROP TABLE IF EXISTS base7p1"
  },
  {
    "bitmap2",
    "CREATE TABLE IF NOT EXISTS bitmap2 (id INTEGER PRIMARY KEY NOT NULL, use_count INTEGER DEFAULT 0 NOT NULL)",
    "DROP TABLE IF EXISTS bitmap2"
  },
  {
    "bitmap3",
    "CREATE TABLE IF NOT EXISTS bitmap3 (id INTEGER PRIMARY KEY NOT NULL, use_count INTEGER DEFAULT 0 NOT NULL)",
    "DROP TABLE IF EXISTS bitmap3"
  },
  {
    "topology_blacklist",
    "CREATE TABLE IF NOT EXISTS topology_blacklist (topology TEXT PRIMARY KEY NOT NULL)",
    "DROP TABLE IF EXISTS topology_blacklist"
  },
  {
    "base4p1_zero_use_count",
    "CREATE TRIGGER IF NOT EXISTS base4p1_zero_use_count AFTER UPDATE OF use_count ON base4p1 BEGIN DELETE FROM base4p1 WHERE id = old.id AND new.use_count <= 0 ; END",
    "DROP TRIGGER IF EXISTS base4p1_zero_use_count"
  },
  {
    "base5p1_zero_use_count",
    "CREATE TRIGGER IF NOT EXISTS base5p1_zero_use_count AFTER UPDATE OF use_count ON base5p1 BEGIN DELETE FROM base5p1 WHERE id = old.id AND new.use_count <= 0 ; END",
    "DROP TRIGGER IF EXISTS base5p1_zero_use_count"
  },
  {
    "base6p1_zero_use_count",
    "CREATE TRIGGER IF NOT EXISTS base6p1_zero_use_count AFTER UPDATE OF use_count ON base6p1 BEGIN DELETE FROM base6p1 WHERE id = old.id AND new.use_count <= 0 ; END",
    "DROP TRIGGER IF EXISTS base6p1_zero_use_count"
  },
  {
    "base7p1_zero_use_count",
    "CREATE TRIGGER IF NOT EXISTS base7p1_zero_use_count AFTER UPDATE OF use_count ON base7p1 BEGIN DELETE FROM base7p1 WHERE id = old.id AND new.use_count <= 0 ; END",
    "DROP TRIGGER IF EXISTS base7p1_zero_use_count"
  },
  {
    "bitmap2_zero_use_count",
    "CREATE TRIGGER IF NOT EXISTS bitmap2_zero_use_count AFTER UPDATE OF use_count ON bitmap2 BEGIN DELETE FROM bitmap2 WHERE id = old.id AND new.use_count <= 0 ; END",
    "DROP TRIGGER IF EXISTS bitmap2_zero_use_count"
  },
  {
    "bitmap3_zero_use_count",
    "CREATE TRIGGER IF NOT EXISTS bitmap3_zero_use_count AFTER UPDATE OF use_count ON bitmap3 BEGIN DELETE FROM bitmap3 WHERE id = old.id AND new.use_count <= 0 ; END",
    "DROP TRIGGER IF EXISTS bitmap3_zero_use_count"
  },
  {
    "version",
    "CREATE TABLE IF NOT EXISTS version (id INTEGER PRIMARY KEY NOT NULL, schema INTEGER DEFAULT 0 NOT NULL)",
    "DROP TABLE IF EXISTS version"
  },
};
static unsigned char gaucCharacterSet1[256] = // Do not edit this array manually as it was auto-generated.
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
static unsigned char gaucCharacterSet2[256] = // Do not edit this array manually as it was auto-generated.
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
static unsigned char gaucCharacterSet3[256] = // Do not edit this array manually as it was auto-generated.
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
static unsigned char gaucCharacterSet4[256] = // Do not edit this array manually as it was auto-generated.
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
static unsigned char gaucCharacterSet5[256] = // Do not edit this array manually as it was auto-generated.
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1,
  0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0,
  1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1,
  0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0,
};
static unsigned char gaucCharacterSet6[256] = // Do not edit this array manually as it was auto-generated.
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1,
  0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0,
  0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
static unsigned char gaucCharacterSet7[256] = // Do not edit this array manually as it was auto-generated.
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1,
};
static unsigned char gaucTokenSet1[256] = // Do not edit this array manually as it was auto-generated.
{
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
static unsigned char gaucTokenSet2[256] = // Do not edit this array manually as it was auto-generated.
{
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1,
  0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0,
  1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1,
  0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0,
};
static unsigned char gaucTokenSet3[256] = // Do not edit this array manually as it was auto-generated.
{
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0,
};
static unsigned char gaucTokenSet4[256] = // Do not edit this array manually as it was auto-generated.
{
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1,
};

/*-
 ***********************************************************************
 *
 * PwDAcquireConnection
 *
 ***********************************************************************
 */
int
PwDAcquireConnection(PW_D_CONTEXT *psPwDContext)
{
  char *pcError = NULL;
  int iError = ER_OK;
  int iSetOwnerships = PATHWELL_FALSE;
  int *piVersion = NULL;
  sqlite3 *psDbHandle = NULL;

  /*-
   *********************************************************************
   *
   * A valid context is required.
   *
   *********************************************************************
   */
  if (PwDContextIsValid(psPwDContext) == PATHWELL_FALSE)
  {
    PwDSetError(psPwDContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwDContextIsValid(psPwDContext) == PATHWELL_TRUE);

  /*-
   *********************************************************************
   *
   * Conditionally bind library resources (based on reference count).
   *
   *********************************************************************
   */
  iError = PwDBindLibraryResources(psPwDContext, 0);
  if (iError != ER_OK)
  {
    return ER; // Note: Error message already set.
  }

  /*-
   *********************************************************************
   *
   * Abort if the database doesn't exist and the SQLITE_OPEN_CREATE flag
   * is not set since the caller is attempting to perform an action that
   * won't work. If the database doesn't exist and the flag is set, then
   * make provisions for setting its file ownerships once created.
   *
   *********************************************************************
   */
  if (!PwDDbFileExists(psPwDContext->pcDbFile))
  {
    if ((psPwDContext->iDbFileFlags & SQLITE_OPEN_CREATE) != SQLITE_OPEN_CREATE)
    {
      PwDSetError(psPwDContext, "The SQLITE_OPEN_CREATE flag must be set when acquiring a DB connection for a DB that does not yet exist.", NULL);
      return ER;
    }
    iSetOwnerships = PATHWELL_TRUE;
  }

  /*-
   *********************************************************************
   *
   * Attempt to open the database.
   *
   *********************************************************************
   */
  iError = sqlite3_open_v2(psPwDContext->pcDbFile, &psDbHandle, psPwDContext->iDbFileFlags, NULL);
  if (iError != SQLITE_OK)
  {
    PwDSetError(psPwDContext, "Failed to acquire DB connection (%s).", psPwDContext->pcDbFile, NULL);
    return ER;
  }
  psPwDContext->psDbHandle = psDbHandle;

#ifdef WIN32
  /* Empty */
#else
  /*-
   *********************************************************************
   *
   * Conditionally set file ownerships. Note that a failure here is not
   * considered fatal since the only ownership that is likely to change
   * is the group owner.
   *
   *********************************************************************
   */
  if (iSetOwnerships == PATHWELL_TRUE && (getuid() == 0 || geteuid() == 0))
  {
//FIXME Investigate using fchown() to avoid race conditions. Doing so, however, requires obtaining the file descriptor from SQLite's API, which may or may not be possible in a stock release. There's at least one patch in the mailing lists that uses sqlite3_file_control() and SQLITE_FCNTL_FD.
    iError = chown(psPwDContext->pcDbFile, 0, 0);
    if (iError == -1)
    {
      /* Empty */
    }
  }
#endif

  /*-
   *********************************************************************
   *
   * Enable an internal timer-based busy handler.
   *
   *********************************************************************
   */
  iError = sqlite3_busy_timeout(psDbHandle, PATHWELL_DB_BUSY_TIMEOUT);
  if (iError != SQLITE_OK)
  {
    PwDSetError(psPwDContext, "Failed to enable busy handler.", NULL);
    return ER;
  }

  return ER_OK;
}


/*-
 ***********************************************************************
 *
 * PwDBindLibraryResources
 *
 ***********************************************************************
 */
int
PwDBindLibraryResources(PW_D_CONTEXT *psPwDContext, int iForce)
{
  int iError = ER_OK;

  /*-
   *********************************************************************
   *
   * A valid context is required.
   *
   *********************************************************************
   */
  if (PwDContextIsValid(psPwDContext) == PATHWELL_FALSE)
  {
    PwDSetError(psPwDContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwDContextIsValid(psPwDContext) == PATHWELL_TRUE);

  /*-
   *********************************************************************
   *
   * Normally, library resources are bound when the reference count is
   * zero, but if the caller forces the issue, we'll honor the request.
   *
   *********************************************************************
   */
  if (iForce)
  {
    iError = sqlite3_initialize();
    if (iError != SQLITE_OK)
    {
      PwDSetError(psPwDContext, "Failed to bind SQLite library resources.", NULL);
      return ER;
    }
    giPwDLibraryReferenceCount++;
    return ER_OK;
  }

  /*-
   *********************************************************************
   *
   * If the reference count is greater than zero, do nothing.
   *
   *********************************************************************
   */
  if (giPwDLibraryReferenceCount > 0)
  {
    return ER_OK;
  }

  /*-
   *********************************************************************
   *
   * It's an error and possibly a bug if reference count is negative.
   *
   *********************************************************************
   */
  if (giPwDLibraryReferenceCount < 0)
  {
    PwDSetError(psPwDContext, "Reference count is negative. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(giPwDLibraryReferenceCount == 0);

  /*-
   *********************************************************************
   *
   * Bind library resources and conditionally increment the reference count.
   *
   *********************************************************************
   */
  iError = sqlite3_initialize();
  if (iError != SQLITE_OK)
  {
    PwDSetError(psPwDContext, "Failed to bind SQLite library resources.", NULL);
    return ER;
  }
  giPwDLibraryReferenceCount++;

  return ER_OK;
}


/*-
 ***********************************************************************
 *
 * PwDClearError
 *
 ***********************************************************************
 */
void
PwDClearError(PW_D_CONTEXT *psPwDContext)
{
  if (psPwDContext != NULL)
  {
    if
    (
         psPwDContext->pcError != NULL
      && psPwDContext->pcError != gpcContextAllocationError
      && psPwDContext->pcError != gpcRealErrorLostNullFormat
      && psPwDContext->pcError != gpcRealErrorLostNullMalloc
    )
    {
      free(psPwDContext->pcError);
      psPwDContext->pcError = NULL;
    }
  }
  return;
}


/*-
 ***********************************************************************
 *
 * PwDClearUseCount
 *
 ***********************************************************************
 */
int
PwDClearUseCount(PW_D_CONTEXT *psPwDContext, PW_T_CONTEXT *psPwTContext)
{
  char aacBaseNp1Tables[][PATHWELL_DB_TABLE_NAME_MAX_SIZE] =
  {
    "base4p1",
    "base5p1",
    "base6p1",
    "base7p1",
    "base8p1",
  };
  char aacBitmaskTables[][PATHWELL_DB_TABLE_NAME_MAX_SIZE] =
  {
    "bitmap2",
    "bitmap3",
    "bitmap3",
    "bitmap3",
    "bitmap3"
  };
  char *pcSql = NULL;
  char *pcTable = NULL;
  const char *pcError = NULL;
  const char *pcSqlFormat = "UPDATE %s SET use_count = 0 WHERE id = ?";
  int iDbOpened = 0;
  int iError = ER_OK;
  int iSize = 0;
  int iSuccess = 0;
  int iTableIndex = 0;
  sqlite3_stmt *psStatement = NULL;

  /*-
   *********************************************************************
   *
   * Valid contexts are required.
   *
   *********************************************************************
   */
  if (PwDContextIsValid(psPwDContext) == PATHWELL_FALSE)
  {
    PwDSetError(psPwDContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwDContextIsValid(psPwDContext) == PATHWELL_TRUE);

  if (PwTContextIsValid(psPwTContext) == PATHWELL_FALSE)
  {
    PwDSetError(psPwDContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwTContextIsValid(psPwTContext) == PATHWELL_TRUE);

  /*-
   *********************************************************************
   *
   * Determine the token set and encoding in use.
   *
   *********************************************************************
   */
  switch (PwTGetTokenSet(psPwTContext))
  {
  case PATHWELL_TOKEN_SET_ID_1:
    iTableIndex = 0;
    break;
  case PATHWELL_TOKEN_SET_ID_2:
    iTableIndex = 1;
    break;
  case PATHWELL_TOKEN_SET_ID_3:
    iTableIndex = 2;
    break;
  case PATHWELL_TOKEN_SET_ID_4:
    iTableIndex = 3;
    break;
  default:
    PwDSetError(psPwDContext, "Invalid or unsupported token set.", NULL);
    return ER;
    break;
  }

  if (PwTGetEncoding(psPwTContext) == PATHWELL_ENCODING_BITMASK)
  {
    pcTable = aacBitmaskTables[iTableIndex];
  }
  else
  {
    pcTable = aacBaseNp1Tables[iTableIndex];
  }

  /*-
   *********************************************************************
   *
   * Allocate memory for and initialize the SQL statement.
   *
   *********************************************************************
   */
  iSize = strlen(pcSqlFormat) + strlen(pcTable) - strlen("%s") + 1;
  pcSql = malloc(sizeof(char) * iSize);
  if (pcSql == NULL)
  {
    PwDSetError(psPwDContext, "Failed to allocate memory (%s).", strerror(errno), NULL);
    return ER;
  }
  snprintf(pcSql, iSize, pcSqlFormat, pcTable);

  /*-
   *********************************************************************
   *
   * Conditionally open the database.
   *
   *********************************************************************
   */
  if (psPwDContext->psDbHandle == NULL)
  {
    iError = PwDAcquireConnection(psPwDContext);
    if (iError != ER_OK)
    {
      return ER; // Note: Error message already set.
    }
    iDbOpened = 1;
  }

  /*-
   *********************************************************************
   *
   * Prepare the SQL statement and bind its parameters (if any).
   *
   *********************************************************************
   */
  iError = sqlite3_prepare_v2(psPwDContext->psDbHandle, pcSql, strlen(pcSql), &psStatement, NULL);
  if (iError != SQLITE_OK)
  {
    PwDSetError(psPwDContext, "Failed to prepare SQL statement (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }
  iError = sqlite3_bind_int64(psStatement, 1, PwTGetId(psPwTContext));
  if (iError != SQLITE_OK)
  {
    PwDSetError(psPwDContext, "Failed to bind SQL parameter (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }

  /*-
   *********************************************************************
   *
   * Execute the SQL statement.
   *
   *********************************************************************
   */
  iError = sqlite3_step(psStatement);
  if (iError != SQLITE_DONE)
  {
    PwDSetError(psPwDContext, "Failed to execute SQL statement (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }
  iSuccess = 1;

PW_D_CLEANUP:
  /*-
   *********************************************************************
   *
   * Release any locks/resources associated with the SQL statement.
   *
   *********************************************************************
   */
  if (psStatement != NULL)
  {
    sqlite3_finalize(psStatement);
  }

  if (pcSql != NULL)
  {
    free(pcSql);
  }

  /*-
   *********************************************************************
   *
   * Conditionally close the database.
   *
   *********************************************************************
   */
  if (psPwDContext->psDbHandle != NULL && iDbOpened)
  {
    iError = PwDReleaseConnection(psPwDContext);
    if (iError != ER_OK)
    {
      return ER; // Note: Error message already set.
    }
  }

  return (iSuccess) ? ER_OK : ER;
}


/*-
 ***********************************************************************
 *
 * PwDContextIsValid
 *
 ***********************************************************************
 */
int
PwDContextIsValid(PW_D_CONTEXT *psPwDContext)
{
  return (psPwDContext != NULL && psPwDContext->iValid == PATHWELL_TRUE) ? PATHWELL_TRUE : PATHWELL_FALSE;
}


/*-
 ***********************************************************************
 *
 * PwDCreateSchema
 *
 ***********************************************************************
 */
int
PwDCreateSchema(PW_D_CONTEXT *psPwDContext)
{
  int iError = 0;

  /*-
   *********************************************************************
   *
   * Create tables.
   *
   *********************************************************************
   */
  iError = PwDCreateTables(psPwDContext);
  if (iError != ER_OK)
  {
    return ER; // Note: Error message already set.
  }

  /*-
   *********************************************************************
   *
   * Create triggers.
   *
   *********************************************************************
   */
  iError = PwDCreateTriggers(psPwDContext);
  if (iError != ER_OK)
  {
    return ER; // Note: Error message already set.
  }

  /*-
   *********************************************************************
   *
   * Forceably set the schema version. This should be the last step in
   * creating the schema since it is used by PwDVerifySchema().
   *
   *********************************************************************
   */
  iError = PwDSetSchemaVersion(psPwDContext, PW_V_DB_SCHEMA_VERSION, PATHWELL_TRUE);
  if (iError != ER_OK)
  {
    return ER; // Note: Error message already set.
  }

  return ER_OK;
}


/*-
 ***********************************************************************
 *
 * PwDCreateTable
 *
 ***********************************************************************
 */
int
PwDCreateTable(PW_D_CONTEXT *psPwDContext, const char *pcName)
{
  char *pcSql = NULL;
  char *pcTable = NULL;
  const char *pcError = NULL;
  const char *pcSqlFormat = NULL;
  int iDbOpened = 0;
  int iError = ER_OK;
  int iIndex = 0;
  int iSize = 0;
  int iSuccess = 0;
  sqlite3_stmt *psStatement = NULL;

  /*-
   *********************************************************************
   *
   * A valid context and name argument are required.
   *
   *********************************************************************
   */
  if (PwDContextIsValid(psPwDContext) == PATHWELL_FALSE)
  {
    PwDSetError(psPwDContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwDContextIsValid(psPwDContext) == PATHWELL_TRUE);

  if (pcName == NULL)
  {
    PwDSetError(psPwDContext, "Name argument is not defined. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(pcName != NULL);

  /*-
   *********************************************************************
   *
   * Locate the SQL statement template.
   *
   *********************************************************************
   */
  for (iIndex = 0; iIndex < sizeof(gasCannedSql) / sizeof(gasCannedSql[0]); iIndex++)
  {
    if (strcmp(gasCannedSql[iIndex].pcName, pcName) == 0)
    {
      pcSqlFormat = gasCannedSql[iIndex].pcCreateSql;
      break;
    }
  }

  /*-
   *********************************************************************
   *
   * Allocate memory for and initialize the SQL statement.
   *
   *********************************************************************
   */
  iSize = strlen(pcSqlFormat) + 1;
  pcSql = malloc(sizeof(char) * iSize);
  if (pcSql == NULL)
  {
    PwDSetError(psPwDContext, "Failed to allocate memory (%s).", strerror(errno), NULL);
    return ER;
  }
  snprintf(pcSql, iSize, "%s", pcSqlFormat);

  /*-
   *********************************************************************
   *
   * Conditionally open the database.
   *
   *********************************************************************
   */
  if (psPwDContext->psDbHandle == NULL)
  {
    iError = PwDAcquireConnection(psPwDContext);
    if (iError != ER_OK)
    {
      return ER; // Note: Error message already set.
    }
    iDbOpened = 1;
  }

  /*-
   *********************************************************************
   *
   * Prepare the SQL statement and bind its parameters (if any).
   *
   *********************************************************************
   */
  iError = sqlite3_prepare_v2(psPwDContext->psDbHandle, pcSql, strlen(pcSql), &psStatement, NULL);
  if (iError != SQLITE_OK)
  {
    PwDSetError(psPwDContext, "Failed to prepare SQL statement (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }

  /*-
   *********************************************************************
   *
   * Execute the SQL statement.
   *
   *********************************************************************
   */
  iError = sqlite3_step(psStatement);
  if (iError != SQLITE_DONE)
  {
    PwDSetError(psPwDContext, "Failed to execute SQL statement (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }
  iSuccess = 1;

PW_D_CLEANUP:
  /*-
   *********************************************************************
   *
   * Release any locks/resources associated with the SQL statement.
   *
   *********************************************************************
   */
  if (psStatement != NULL)
  {
    sqlite3_finalize(psStatement);
  }

  if (pcSql != NULL)
  {
    free(pcSql);
  }

  /*-
   *********************************************************************
   *
   * Conditionally close the database.
   *
   *********************************************************************
   */
  if (psPwDContext->psDbHandle != NULL && iDbOpened)
  {
    iError = PwDReleaseConnection(psPwDContext);
    if (iError != ER_OK)
    {
      return ER; // Note: Error message already set.
    }
  }

  if (!iSuccess)
  {
    return ER;
  }

  return (iSuccess) ? ER_OK : ER;
}


/*-
 ***********************************************************************
 *
 * PwDCreateTables
 *
 ***********************************************************************
 */
int
PwDCreateTables(PW_D_CONTEXT *psPwDContext)
{
  int iError = 0;
  int iIndex = 0;

  for (iIndex = 0; iIndex < sizeof(gasCannedSql) / sizeof(gasCannedSql[0]); iIndex++)
  {
    iError = PwDCreateTable(psPwDContext, gasCannedSql[iIndex].pcName);
    if (iError != ER_OK)
    {
      return ER; // Note: Error message already set.
    }
  }

  return ER_OK;
}


/*-
 ***********************************************************************
 *
 * PwDCreateTrigger
 *
 ***********************************************************************
 */
int
PwDCreateTrigger(PW_D_CONTEXT *psPwDContext, const char *pcName)
{
  char *pcSql = NULL;
  char *pcTable = NULL;
  const char *pcError = NULL;
  const char *pcSqlFormat = NULL;
  int iDbOpened = 0;
  int iError = ER_OK;
  int iIndex = 0;
  int iSize = 0;
  int iSuccess = 0;
  sqlite3_stmt *psStatement = NULL;

  /*-
   *********************************************************************
   *
   * A valid context and name argument are required.
   *
   *********************************************************************
   */
  if (PwDContextIsValid(psPwDContext) == PATHWELL_FALSE)
  {
    PwDSetError(psPwDContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwDContextIsValid(psPwDContext) == PATHWELL_TRUE);

  if (pcName == NULL)
  {
    PwDSetError(psPwDContext, "Name argument is not defined. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(pcName != NULL);

  /*-
   *********************************************************************
   *
   * Locate the SQL statement template.
   *
   *********************************************************************
   */
  for (iIndex = 0; iIndex < sizeof(gasCannedSql) / sizeof(gasCannedSql[0]); iIndex++)
  {
    if (strcmp(gasCannedSql[iIndex].pcName, pcName) == 0)
    {
      pcSqlFormat = gasCannedSql[iIndex].pcCreateSql;
      break;
    }
  }

  /*-
   *********************************************************************
   *
   * Allocate memory for and initialize the SQL statement.
   *
   *********************************************************************
   */
  iSize = strlen(pcSqlFormat) + 1;
  pcSql = malloc(sizeof(char) * iSize);
  if (pcSql == NULL)
  {
    PwDSetError(psPwDContext, "Failed to allocate memory (%s).", strerror(errno), NULL);
    return ER;
  }
  snprintf(pcSql, iSize, "%s", pcSqlFormat);

  /*-
   *********************************************************************
   *
   * Conditionally open the database.
   *
   *********************************************************************
   */
  if (psPwDContext->psDbHandle == NULL)
  {
    iError = PwDAcquireConnection(psPwDContext);
    if (iError != ER_OK)
    {
      return ER; // Note: Error message already set.
    }
    iDbOpened = 1;
  }

  /*-
   *********************************************************************
   *
   * Prepare the SQL statement and bind its parameters (if any).
   *
   *********************************************************************
   */
  iError = sqlite3_prepare_v2(psPwDContext->psDbHandle, pcSql, strlen(pcSql), &psStatement, NULL);
  if (iError != SQLITE_OK)
  {
    PwDSetError(psPwDContext, "Failed to prepare SQL statement (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }

  /*-
   *********************************************************************
   *
   * Execute the SQL statement.
   *
   *********************************************************************
   */
  iError = sqlite3_step(psStatement);
  if (iError != SQLITE_DONE)
  {
    PwDSetError(psPwDContext, "Failed to execute SQL statement (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }
  iSuccess = 1;

PW_D_CLEANUP:
  /*-
   *********************************************************************
   *
   * Release any locks/resources associated with the SQL statement.
   *
   *********************************************************************
   */
  if (psStatement != NULL)
  {
    sqlite3_finalize(psStatement);
  }

  if (pcSql != NULL)
  {
    free(pcSql);
  }

  /*-
   *********************************************************************
   *
   * Conditionally close the database.
   *
   *********************************************************************
   */
  if (psPwDContext->psDbHandle != NULL && iDbOpened)
  {
    iError = PwDReleaseConnection(psPwDContext);
    if (iError != ER_OK)
    {
      return ER; // Note: Error message already set.
    }
  }

  return (iSuccess) ? ER_OK : ER;
}


/*-
 ***********************************************************************
 *
 * PwDCreateTriggers
 *
 ***********************************************************************
 */
int
PwDCreateTriggers(PW_D_CONTEXT *psPwDContext)
{
  int iError = 0;
  int iIndex = 0;

  for (iIndex = 0; iIndex < sizeof(gasCannedSql) / sizeof(gasCannedSql[0]); iIndex++)
  {
    iError = PwDCreateTrigger(psPwDContext, gasCannedSql[iIndex].pcName);
    if (iError != ER_OK)
    {
      return ER; // Note: Error message already set.
    }
  }

  return ER_OK;
}


/*-
 ***********************************************************************
 *
 * PwDDbDirectoryExists
 *
 ***********************************************************************
 */
int
PwDDbDirectoryExists(char *pcDbDirectory)
{
  struct stat sStatEntry = { 0 };

  if (pcDbDirectory == NULL)
  {
    return PATHWELL_FALSE;
  }
  PATHWELL_ASSERT(pcDbDirectory != NULL);

  /*-
   *********************************************************************
   *
   * For our purposes, DB directory existence is asserted if stat()
   * succeeds and the resolved (think symbolic links) path actually
   * represents a directory. Note however, that there are cases where
   * stat() will fail even though a directory may actually exist (e.g,
   * EACCES), but we don't care becuase if we can't get to the file
   * for whatever reason, then it does not exist as far as we're
   * concerned.
   *
   *********************************************************************
   */
  if (stat(pcDbDirectory, &sStatEntry) == 0 && S_ISDIR(sStatEntry.st_mode))
  {
    return PATHWELL_TRUE;
  }

  return PATHWELL_FALSE;
}


/*-
 ***********************************************************************
 *
 * PwDDbFileExists
 *
 ***********************************************************************
 */
int
PwDDbFileExists(char *pcDbFile)
{
  struct stat sStatEntry = { 0 };

  if (pcDbFile == NULL)
  {
    return PATHWELL_FALSE;
  }
  PATHWELL_ASSERT(pcDbFile != NULL);

  /*-
   *********************************************************************
   *
   * For our purposes, DB file existence is asserted if stat() succeeds
   * and the resolved (think symbolic links) path actually represents a
   * regular file. Note however, that there are cases where stat() will
   * fail even though a file may actually exist (e.g, EACCES), but we
   * don't care becuase if we can't get to the file for whatever reason,
   * then it does not exist as far as we're concerned.
   *
   *********************************************************************
   */
  if (stat(pcDbFile, &sStatEntry) == 0 && S_ISREG(sStatEntry.st_mode))
  {
    return PATHWELL_TRUE;
  }

  return PATHWELL_FALSE;
}


/*-
 ***********************************************************************
 *
 * PwDDecrementUseCount
 *
 ***********************************************************************
 */
int
PwDDecrementUseCount(PW_D_CONTEXT *psPwDContext, PW_T_CONTEXT *psPwTContext)
{
  char aacBaseNp1Tables[][PATHWELL_DB_TABLE_NAME_MAX_SIZE] =
  {
    "base4p1",
    "base5p1",
    "base6p1",
    "base7p1",
    "base8p1",
  };
  char aacBitmaskTables[][PATHWELL_DB_TABLE_NAME_MAX_SIZE] =
  {
    "bitmap2",
    "bitmap3",
    "bitmap3",
    "bitmap3",
    "bitmap3"
  };
  char *pcSql = NULL;
  char *pcTable = NULL;
  const char *pcError = NULL;
  const char *pcSqlFormat = "UPDATE %s SET use_count = (CASE WHEN use_count <= 0 THEN 0 ELSE use_count - 1 END) WHERE id = ?";
  int iDbOpened = 0;
  int iError = ER_OK;
  int iSize = 0;
  int iSuccess = 0;
  int iTableIndex = 0;
  sqlite3_stmt *psStatement = NULL;

  /*-
   *********************************************************************
   *
   * Valid contexts are required.
   *
   *********************************************************************
   */
  if (PwDContextIsValid(psPwDContext) == PATHWELL_FALSE)
  {
    PwDSetError(psPwDContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwDContextIsValid(psPwDContext) == PATHWELL_TRUE);

  if (PwTContextIsValid(psPwTContext) == PATHWELL_FALSE)
  {
    PwDSetError(psPwDContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwTContextIsValid(psPwTContext) == PATHWELL_TRUE);

  /*-
   *********************************************************************
   *
   * Determine the token set and encoding in use.
   *
   *********************************************************************
   */
  switch (PwTGetTokenSet(psPwTContext))
  {
  case PATHWELL_TOKEN_SET_ID_1:
    iTableIndex = 0;
    break;
  case PATHWELL_TOKEN_SET_ID_2:
    iTableIndex = 1;
    break;
  case PATHWELL_TOKEN_SET_ID_3:
    iTableIndex = 2;
    break;
  case PATHWELL_TOKEN_SET_ID_4:
    iTableIndex = 3;
    break;
  default:
    PwDSetError(psPwDContext, "Invalid or unsupported token set.", NULL);
    return ER;
    break;
  }

  if (PwTGetEncoding(psPwTContext) == PATHWELL_ENCODING_BITMASK)
  {
    pcTable = aacBitmaskTables[iTableIndex];
  }
  else
  {
    pcTable = aacBaseNp1Tables[iTableIndex];
  }

  /*-
   *********************************************************************
   *
   * Allocate memory for and initialize the SQL statement.
   *
   *********************************************************************
   */
  iSize = strlen(pcSqlFormat) + (strlen(pcTable) * 1) - (strlen("%s") * 1) + 1;
  pcSql = malloc(sizeof(char) * iSize);
  if (pcSql == NULL)
  {
    PwDSetError(psPwDContext, "Failed to allocate memory (%s).", strerror(errno), NULL);
    return ER;
  }
  snprintf(pcSql, iSize, pcSqlFormat, pcTable);

  /*-
   *********************************************************************
   *
   * Conditionally open the database.
   *
   *********************************************************************
   */
  if (psPwDContext->psDbHandle == NULL)
  {
    iError = PwDAcquireConnection(psPwDContext);
    if (iError != ER_OK)
    {
      return ER; // Note: Error message already set.
    }
    iDbOpened = 1;
  }

  /*-
   *********************************************************************
   *
   * Prepare the SQL statement and bind its parameters (if any).
   *
   *********************************************************************
   */
  iError = sqlite3_prepare_v2(psPwDContext->psDbHandle, pcSql, strlen(pcSql), &psStatement, NULL);
  if (iError != SQLITE_OK)
  {
    PwDSetError(psPwDContext, "Failed to prepare SQL statement (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }
  iError = sqlite3_bind_int64(psStatement, 1, PwTGetId(psPwTContext));
  if (iError != SQLITE_OK)
  {
    PwDSetError(psPwDContext, "Failed to bind SQL parameter (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }

  /*-
   *********************************************************************
   *
   * Execute the SQL statement.
   *
   *********************************************************************
   */
  iError = sqlite3_step(psStatement);
  if (iError != SQLITE_DONE)
  {
    PwDSetError(psPwDContext, "Failed to execute SQL statement (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }
  iSuccess = 1;

PW_D_CLEANUP:
  /*-
   *********************************************************************
   *
   * Release any locks/resources associated with the SQL statement.
   *
   *********************************************************************
   */
  if (psStatement != NULL)
  {
    sqlite3_finalize(psStatement);
  }

  if (pcSql != NULL)
  {
    free(pcSql);
  }

  /*-
   *********************************************************************
   *
   * Conditionally close the database.
   *
   *********************************************************************
   */
  if (psPwDContext->psDbHandle != NULL && iDbOpened)
  {
    iError = PwDReleaseConnection(psPwDContext);
    if (iError != ER_OK)
    {
      return ER; // Note: Error message already set.
    }
  }

  return (iSuccess) ? ER_OK : ER;
}


/*-
 ***********************************************************************
 *
 * PwDDeleteSchema
 *
 ***********************************************************************
 */
int
PwDDeleteSchema(PW_D_CONTEXT *psPwDContext)
{
  int iError = 0;

  /*-
   *********************************************************************
   *
   * Delete tables.
   *
   *********************************************************************
   */
  iError = PwDDeleteTables(psPwDContext);
  if (iError != ER_OK)
  {
    return ER; // Note: Error message already set.
  }

  /*-
   *********************************************************************
   *
   * Delete triggers.
   *
   *********************************************************************
   */
  iError = PwDDeleteTriggers(psPwDContext);
  if (iError != ER_OK)
  {
    return ER; // Note: Error message already set.
  }

  return ER_OK;
}


/*-
 ***********************************************************************
 *
 * PwDDeleteTable
 *
 ***********************************************************************
 */
int
PwDDeleteTable(PW_D_CONTEXT *psPwDContext, const char *pcName)
{
  char *pcSql = NULL;
  char *pcTable = NULL;
  const char *pcError = NULL;
  const char *pcSqlFormat = NULL;
  int iDbOpened = 0;
  int iError = ER_OK;
  int iIndex = 0;
  int iSize = 0;
  int iSuccess = 0;
  sqlite3_stmt *psStatement = NULL;

  /*-
   *********************************************************************
   *
   * A valid context and name argument are required.
   *
   *********************************************************************
   */
  if (PwDContextIsValid(psPwDContext) == PATHWELL_FALSE)
  {
    PwDSetError(psPwDContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwDContextIsValid(psPwDContext) == PATHWELL_TRUE);

  if (pcName == NULL)
  {
    PwDSetError(psPwDContext, "Name argument is not defined. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(pcName != NULL);

  /*-
   *********************************************************************
   *
   * Locate the SQL statement template.
   *
   *********************************************************************
   */
  for (iIndex = 0; iIndex < sizeof(gasCannedSql) / sizeof(gasCannedSql[0]); iIndex++)
  {
    if (strcmp(gasCannedSql[iIndex].pcName, pcName) == 0)
    {
      pcSqlFormat = gasCannedSql[iIndex].pcDeleteSql;
      break;
    }
  }

  /*-
   *********************************************************************
   *
   * Allocate memory for and initialize the SQL statement.
   *
   *********************************************************************
   */
  iSize = strlen(pcSqlFormat) + 1;
  pcSql = malloc(sizeof(char) * iSize);
  if (pcSql == NULL)
  {
    PwDSetError(psPwDContext, "Failed to allocate memory (%s).", strerror(errno), NULL);
    return ER;
  }
  snprintf(pcSql, iSize, "%s", pcSqlFormat);

  /*-
   *********************************************************************
   *
   * Conditionally open the database.
   *
   *********************************************************************
   */
  if (psPwDContext->psDbHandle == NULL)
  {
    iError = PwDAcquireConnection(psPwDContext);
    if (iError != ER_OK)
    {
      return ER; // Note: Error message already set.
    }
    iDbOpened = 1;
  }

  /*-
   *********************************************************************
   *
   * Prepare the SQL statement and bind its parameters (if any).
   *
   *********************************************************************
   */
  iError = sqlite3_prepare_v2(psPwDContext->psDbHandle, pcSql, strlen(pcSql), &psStatement, NULL);
  if (iError != SQLITE_OK)
  {
    PwDSetError(psPwDContext, "Failed to prepare SQL statement (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }

  /*-
   *********************************************************************
   *
   * Execute the SQL statement.
   *
   *********************************************************************
   */
  iError = sqlite3_step(psStatement);
  if (iError != SQLITE_DONE)
  {
    PwDSetError(psPwDContext, "Failed to execute SQL statement (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }
  iSuccess = 1;

PW_D_CLEANUP:
  /*-
   *********************************************************************
   *
   * Release any locks/resources associated with the SQL statement.
   *
   *********************************************************************
   */
  if (psStatement != NULL)
  {
    sqlite3_finalize(psStatement);
  }

  if (pcSql != NULL)
  {
    free(pcSql);
  }

  /*-
   *********************************************************************
   *
   * Conditionally close the database.
   *
   *********************************************************************
   */
  if (psPwDContext->psDbHandle != NULL && iDbOpened)
  {
    iError = PwDReleaseConnection(psPwDContext);
    if (iError != ER_OK)
    {
      return ER; // Note: Error message already set.
    }
  }

  return (iSuccess) ? ER_OK : ER;
}


/*-
 ***********************************************************************
 *
 * PwDDeleteTables
 *
 ***********************************************************************
 */
int
PwDDeleteTables(PW_D_CONTEXT *psPwDContext)
{
  int iError = 0;
  int iIndex = 0;

  for (iIndex = 0; iIndex < sizeof(gasCannedSql) / sizeof(gasCannedSql[0]); iIndex++)
  {
    iError = PwDDeleteTable(psPwDContext, gasCannedSql[iIndex].pcName);
    if (iError != ER_OK)
    {
      return ER; // Note: Error message already set.
    }
  }

  return ER_OK;
}


/*-
 ***********************************************************************
 *
 * PwDDeleteTrigger
 *
 ***********************************************************************
 */
int
PwDDeleteTrigger(PW_D_CONTEXT *psPwDContext, const char *pcName)
{
  char *pcSql = NULL;
  char *pcTable = NULL;
  const char *pcError = NULL;
  const char *pcSqlFormat = NULL;
  int iDbOpened = 0;
  int iError = ER_OK;
  int iIndex = 0;
  int iSize = 0;
  int iSuccess = 0;
  sqlite3_stmt *psStatement = NULL;

  /*-
   *********************************************************************
   *
   * A valid context and name argument are required.
   *
   *********************************************************************
   */
  if (PwDContextIsValid(psPwDContext) == PATHWELL_FALSE)
  {
    PwDSetError(psPwDContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwDContextIsValid(psPwDContext) == PATHWELL_TRUE);

  if (pcName == NULL)
  {
    PwDSetError(psPwDContext, "Name argument is not defined. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(pcName != NULL);

  /*-
   *********************************************************************
   *
   * Locate the SQL statement template.
   *
   *********************************************************************
   */
  for (iIndex = 0; iIndex < sizeof(gasCannedSql) / sizeof(gasCannedSql[0]); iIndex++)
  {
    if (strcmp(gasCannedSql[iIndex].pcName, pcName) == 0)
    {
      pcSqlFormat = gasCannedSql[iIndex].pcDeleteSql;
      break;
    }
  }

  /*-
   *********************************************************************
   *
   * Allocate memory for and initialize the SQL statement.
   *
   *********************************************************************
   */
  iSize = strlen(pcSqlFormat) + 1;
  pcSql = malloc(sizeof(char) * iSize);
  if (pcSql == NULL)
  {
    PwDSetError(psPwDContext, "Failed to allocate memory (%s).", strerror(errno), NULL);
    return ER;
  }
  snprintf(pcSql, iSize, "%s", pcSqlFormat);

  /*-
   *********************************************************************
   *
   * Conditionally open the database.
   *
   *********************************************************************
   */
  if (psPwDContext->psDbHandle == NULL)
  {
    iError = PwDAcquireConnection(psPwDContext);
    if (iError != ER_OK)
    {
      return ER; // Note: Error message already set.
    }
    iDbOpened = 1;
  }

  /*-
   *********************************************************************
   *
   * Prepare the SQL statement and bind its parameters (if any).
   *
   *********************************************************************
   */
  iError = sqlite3_prepare_v2(psPwDContext->psDbHandle, pcSql, strlen(pcSql), &psStatement, NULL);
  if (iError != SQLITE_OK)
  {
    PwDSetError(psPwDContext, "Failed to prepare SQL statement (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }

  /*-
   *********************************************************************
   *
   * Execute the SQL statement.
   *
   *********************************************************************
   */
  iError = sqlite3_step(psStatement);
  if (iError != SQLITE_DONE)
  {
    PwDSetError(psPwDContext, "Failed to execute SQL statement (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }
  iSuccess = 1;

PW_D_CLEANUP:
  /*-
   *********************************************************************
   *
   * Release any locks/resources associated with the SQL statement.
   *
   *********************************************************************
   */
  if (psStatement != NULL)
  {
    sqlite3_finalize(psStatement);
  }

  if (pcSql != NULL)
  {
    free(pcSql);
  }

  /*-
   *********************************************************************
   *
   * Conditionally close the database.
   *
   *********************************************************************
   */
  if (psPwDContext->psDbHandle != NULL && iDbOpened)
  {
    iError = PwDReleaseConnection(psPwDContext);
    if (iError != ER_OK)
    {
      return ER; // Note: Error message already set.
    }
  }

  return (iSuccess) ? ER_OK : ER;
}


/*-
 ***********************************************************************
 *
 * PwDDeleteTriggers
 *
 ***********************************************************************
 */
int
PwDDeleteTriggers(PW_D_CONTEXT *psPwDContext)
{
  int iError = 0;
  int iIndex = 0;

  for (iIndex = 0; iIndex < sizeof(gasCannedSql) / sizeof(gasCannedSql[0]); iIndex++)
  {
    iError = PwDDeleteTrigger(psPwDContext, gasCannedSql[iIndex].pcName);
    if (iError != ER_OK)
    {
      return ER; // Note: Error message already set.
    }
  }

  return ER_OK;
}


/*-
 ***********************************************************************
 *
 * PwDFreeContext
 *
 ***********************************************************************
 */
void
PwDFreeContext(PW_D_CONTEXT *psPwDContext)
{
  if (psPwDContext != NULL)
  {
    if
    (
         psPwDContext->pcError != NULL
      && psPwDContext->pcError != gpcContextAllocationError
      && psPwDContext->pcError != gpcRealErrorLostNullFormat
      && psPwDContext->pcError != gpcRealErrorLostNullMalloc
    )
    {
      free(psPwDContext->pcError);
    }
    if (psPwDContext->pcDbFile != NULL)
    {
      free(psPwDContext->pcDbFile);
    }
    if (psPwDContext->pcDbLock != NULL)
    {
      free(psPwDContext->pcDbLock);
    }
    free(psPwDContext);
    psPwDContext = NULL;
  }

  return;
}


/*-
 ***********************************************************************
 *
 * PwDFreeLibraryResources
 *
 ***********************************************************************
 */
int
PwDFreeLibraryResources(PW_D_CONTEXT *psPwDContext, int iForce)
{
  int iError = ER_OK;

  /*-
   *********************************************************************
   *
   * A valid context is required.
   *
   *********************************************************************
   */
  if (PwDContextIsValid(psPwDContext) == PATHWELL_FALSE)
  {
    PwDSetError(psPwDContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwDContextIsValid(psPwDContext) == PATHWELL_TRUE);

  /*-
   *********************************************************************
   *
   * Normally, library resources are freed when the reference count is
   * zero, but if the caller forces the issue, we'll honor the request
   * and reset the reference count in the process.
   *
   *********************************************************************
   */
  if (iForce)
  {
    iError = sqlite3_shutdown();
    if (iError != SQLITE_OK)
    {
      PwDSetError(psPwDContext, "Failed to free SQLite library resources.", NULL);
      return ER;
    }
    giPwDLibraryReferenceCount = 0;
    return ER_OK;
  }

  /*-
   *********************************************************************
   *
   * If the reference count is already zero, do nothing.
   *
   *********************************************************************
   */
  if (giPwDLibraryReferenceCount == 0)
  {
    return ER_OK;
  }

  /*-
   *********************************************************************
   *
   * It's an error and possibly a bug if reference count is negative.
   *
   *********************************************************************
   */
  if (giPwDLibraryReferenceCount < 0)
  {
    PwDSetError(psPwDContext, "Reference count is negative. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(giPwDLibraryReferenceCount > 0);

  /*-
   *********************************************************************
   *
   * Decrement the reference count and conditionally free library resources.
   *
   *********************************************************************
   */
  if (--giPwDLibraryReferenceCount == 0)
  {
    iError = sqlite3_shutdown();
    if (iError != SQLITE_OK)
    {
      PwDSetError(psPwDContext, "Failed to free SQLite library resources.", NULL);
      return ER;
    }
  }

  return ER_OK;
}


/*-
 ***********************************************************************
 *
 * PwDGetError
 *
 ***********************************************************************
 */
const char *
PwDGetError(PW_D_CONTEXT *psPwDContext)
{
  if (psPwDContext == NULL)
  {
    return gpcContextAllocationError;
  }

  return psPwDContext->pcError;
}


/*-
 ***********************************************************************
 *
 * PwDGetSchemaVersion
 *
 ***********************************************************************
 */
int *
PwDGetSchemaVersion(PW_D_CONTEXT *psPwDContext)
{
  char *pcSql = NULL;
  const char *pcError = NULL;
  const char *pcSqlStatement = "SELECT schema FROM version";
  int *piValue = NULL;
  int iDbOpened = 0;
  int iError = ER_OK;
  int iSize = 0;
  int iSuccess = 0;
  int iRows = 0;
  sqlite3_stmt *psStatement = NULL;

  /*-
   *********************************************************************
   *
   * A valid context is required.
   *
   *********************************************************************
   */
  if (PwDContextIsValid(psPwDContext) == PATHWELL_FALSE)
  {
    PwDSetError(psPwDContext, "Invalid context. That should not happen.", NULL);
    return NULL;
  }
  PATHWELL_ASSERT(PwDContextIsValid(psPwDContext) == PATHWELL_TRUE);

  /*-
   *********************************************************************
   *
   * Allocate memory for and initialize the SQL statement.
   *
   *********************************************************************
   */
  iSize = strlen(pcSqlStatement) + 1;
  pcSql = malloc(sizeof(char) * iSize);
  if (pcSql == NULL)
  {
    PwDSetError(psPwDContext, "Failed to allocate memory (%s).", strerror(errno), NULL);
    return NULL;
  }
  snprintf(pcSql, iSize, "%s", pcSqlStatement);

  /*-
   *********************************************************************
   *
   * Conditionally open the database.
   *
   *********************************************************************
   */
  if (psPwDContext->psDbHandle == NULL)
  {
    iError = PwDAcquireConnection(psPwDContext);
    if (iError != ER_OK)
    {
      return NULL; // Note: Error message already set.
    }
    iDbOpened = 1;
  }

  /*-
   *********************************************************************
   *
   * Prepare the SQL statement and bind its parameters (if any).
   *
   *********************************************************************
   */
  iError = sqlite3_prepare_v2(psPwDContext->psDbHandle, pcSql, strlen(pcSql), &psStatement, NULL);
  if (iError != SQLITE_OK)
  {
    PwDSetError(psPwDContext, "Failed to prepare SQL statement (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }

  /*-
   *********************************************************************
   *
   * Execute the SQL statement, and step through the rows. Note that no
   * more than one row is expected in return. If the statement succeeds
   * but no rows are returned, then treat the requested value as though
   * it was undefined, clear out any residual context errors and return
   * a NULL pointer.
   *
   *********************************************************************
   */
  for (iRows = 0; (iError = sqlite3_step(psStatement)) == SQLITE_ROW; iRows++)
  {
    if (piValue == NULL) // Memory to hold the value should be allocated once rather than once per row.
    {
      piValue = (int *)malloc(sizeof(int));
      if (piValue == NULL)
      {
        PwDSetError(psPwDContext, "Failed to allocate memory (%s).", strerror(errno), NULL);
        goto PW_D_CLEANUP;
      }
    }
    *piValue = sqlite3_column_int(psStatement, 0);
  }
  if (iError != SQLITE_DONE)
  {
    PwDSetError(psPwDContext, "Failed to execute SQL statement (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }
  if (iRows < 0 || iRows > 1)
  {
    char acRows[PATHWELL_MAX_32BIT_NUMBER_SIZE] = "";
    snprintf(acRows, PATHWELL_MAX_32BIT_NUMBER_SIZE, "%d", iRows);
    PwDSetError(psPwDContext, "Expected SQL statement to return no more than one row, but got %s instead.", acRows, NULL);
    goto PW_D_CLEANUP;
  }
  if (iRows == 0)
  {
    PwDClearError(psPwDContext);
    if (piValue != NULL)
    {
      free(piValue);
    }
    piValue = NULL;
  }
  iSuccess = 1;

PW_D_CLEANUP:
  /*-
   *********************************************************************
   *
   * If the operation failed, free the value to prevent a memory leak.
   *
   *********************************************************************
   */
  if (!iSuccess)
  {
    if (piValue != NULL)
    {
      free(piValue);
    }
    piValue = NULL;
  }

  /*-
   *********************************************************************
   *
   * Release any locks/resources associated with the SQL statement.
   *
   *********************************************************************
   */
  if (psStatement != NULL)
  {
    sqlite3_finalize(psStatement);
  }

  if (pcSql != NULL)
  {
    free(pcSql);
  }

  /*-
   *********************************************************************
   *
   * Conditionally close the database.
   *
   *********************************************************************
   */
  if (psPwDContext->psDbHandle != NULL && iDbOpened)
  {
    iError = PwDReleaseConnection(psPwDContext);
    if (iError != ER_OK)
    {
      return NULL; // Note: Error message already set.
    }
  }

  return piValue;
}


/*-
 ***********************************************************************
 *
 * PwDGetUseCount
 *
 ***********************************************************************
 */
unsigned int *
PwDGetUseCount(PW_D_CONTEXT *psPwDContext, PW_T_CONTEXT *psPwTContext)
{
  char aacBaseNp1Tables[][PATHWELL_DB_TABLE_NAME_MAX_SIZE] =
  {
    "base4p1",
    "base5p1",
    "base6p1",
    "base7p1",
    "base8p1",
  };
  char aacBitmaskTables[][PATHWELL_DB_TABLE_NAME_MAX_SIZE] =
  {
    "bitmap2",
    "bitmap3",
    "bitmap3",
    "bitmap3",
    "bitmap3"
  };
  char *pcSql = NULL;
  char *pcTable = NULL;
  const char *pcError = NULL;
  const char *pcSqlFormat = "SELECT use_count FROM %s WHERE id = ?";
  int iDbOpened = 0;
  int iError = ER_OK;
  int iRows = 0;
  int iSize = 0;
  int iSuccess = 0;
  int iTableIndex = 0;
  sqlite3_stmt *psStatement = NULL;
  unsigned int *puiValue = NULL;

  /*-
   *********************************************************************
   *
   * Valid contexts are required.
   *
   *********************************************************************
   */
  if (PwDContextIsValid(psPwDContext) == PATHWELL_FALSE)
  {
    PwDSetError(psPwDContext, "Invalid context. That should not happen.", NULL);
    return NULL;
  }
  PATHWELL_ASSERT(PwDContextIsValid(psPwDContext) == PATHWELL_TRUE);

  if (PwTContextIsValid(psPwTContext) == PATHWELL_FALSE)
  {
    PwDSetError(psPwDContext, "Invalid context. That should not happen.", NULL);
    return NULL;
  }
  PATHWELL_ASSERT(PwTContextIsValid(psPwTContext) == PATHWELL_TRUE);

  /*-
   *********************************************************************
   *
   * Determine the token set and encoding in use.
   *
   *********************************************************************
   */
  switch (PwTGetTokenSet(psPwTContext))
  {
  case PATHWELL_TOKEN_SET_ID_1:
    iTableIndex = 0;
    break;
  case PATHWELL_TOKEN_SET_ID_2:
    iTableIndex = 1;
    break;
  case PATHWELL_TOKEN_SET_ID_3:
    iTableIndex = 2;
    break;
  case PATHWELL_TOKEN_SET_ID_4:
    iTableIndex = 3;
    break;
  default:
    PwDSetError(psPwDContext, "Invalid or unsupported token set.", NULL);
    return NULL;
    break;
  }

  if (PwTGetEncoding(psPwTContext) == PATHWELL_ENCODING_BITMASK)
  {
    pcTable = aacBitmaskTables[iTableIndex];
  }
  else
  {
    pcTable = aacBaseNp1Tables[iTableIndex];
  }

  /*-
   *********************************************************************
   *
   * Allocate memory for and initialize the SQL statement.
   *
   *********************************************************************
   */
  iSize = strlen(pcSqlFormat) + (strlen(pcTable) * 1) - (strlen("%s") * 1) + 1;
  pcSql = malloc(sizeof(char) * iSize);
  if (pcSql == NULL)
  {
    PwDSetError(psPwDContext, "Failed to allocate memory (%s).", strerror(errno), NULL);
    return NULL;
  }
  snprintf(pcSql, iSize, pcSqlFormat, pcTable);

  /*-
   *********************************************************************
   *
   * Conditionally open the database.
   *
   *********************************************************************
   */
  if (psPwDContext->psDbHandle == NULL)
  {
    iError = PwDAcquireConnection(psPwDContext);
    if (iError != ER_OK)
    {
      return NULL; // Note: Error message already set.
    }
    iDbOpened = 1;
  }

  /*-
   *********************************************************************
   *
   * Prepare the SQL statement and bind its parameters (if any).
   *
   *********************************************************************
   */
  iError = sqlite3_prepare_v2(psPwDContext->psDbHandle, pcSql, strlen(pcSql), &psStatement, NULL);
  if (iError != SQLITE_OK)
  {
    PwDSetError(psPwDContext, "Failed to prepare SQL statement (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }
  iError = sqlite3_bind_int64(psStatement, 1, PwTGetId(psPwTContext));
  if (iError != SQLITE_OK)
  {
    PwDSetError(psPwDContext, "Failed to bind SQL parameter (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }

  /*-
   *********************************************************************
   *
   * Execute the SQL statement, and step through the rows. Note that no
   * more than one row is expected in return. If the statement succeeds
   * but no rows are returned, then treat the requested value as though
   * it was undefined, clear out any residual context errors and return
   * a NULL pointer.
   *
   *********************************************************************
   */
  for (iRows = 0; (iError = sqlite3_step(psStatement)) == SQLITE_ROW; iRows++)
  {
    if (puiValue == NULL) // Memory to hold the value should be allocated once rather than once per row.
    {
      puiValue = (unsigned int *)malloc(sizeof(int));
      if (puiValue == NULL)
      {
        PwDSetError(psPwDContext, "Failed to allocate memory (%s).", strerror(errno), NULL);
        goto PW_D_CLEANUP;
      }
    }
    *puiValue = sqlite3_column_int(psStatement, 0);
  }
  if (iError != SQLITE_DONE)
  {
    PwDSetError(psPwDContext, "Failed to execute SQL statement (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }
  if (iRows < 0 || iRows > 1)
  {
    char acRows[PATHWELL_MAX_32BIT_NUMBER_SIZE] = "";
    snprintf(acRows, PATHWELL_MAX_32BIT_NUMBER_SIZE, "%d", iRows);
    PwDSetError(psPwDContext, "Expected SQL statement to return no more than one row, but got %s instead.", acRows, NULL);
    goto PW_D_CLEANUP;
  }
  if (iRows == 0)
  {
    PwDClearError(psPwDContext);
    if (puiValue != NULL)
    {
      free(puiValue);
    }
    puiValue = NULL;
  }
  iSuccess = 1;

PW_D_CLEANUP:
  /*-
   *********************************************************************
   *
   * If the operation failed, free the value to prevent a memory leak.
   *
   *********************************************************************
   */
  if (!iSuccess)
  {
    if (puiValue != NULL)
    {
      free(puiValue);
    }
    puiValue = NULL;
  }

  /*-
   *********************************************************************
   *
   * Release any locks/resources associated with the SQL statement.
   *
   *********************************************************************
   */
  if (psStatement != NULL)
  {
    sqlite3_finalize(psStatement);
  }

  if (pcSql != NULL)
  {
    free(pcSql);
  }

  /*-
   *********************************************************************
   *
   * Conditionally close the database.
   *
   *********************************************************************
   */
  if (psPwDContext->psDbHandle != NULL && iDbOpened)
  {
    iError = PwDReleaseConnection(psPwDContext);
    if (iError != ER_OK)
    {
      return NULL; // Note: Error message already set.
    }
  }

  return puiValue;
}


/*-
 ***********************************************************************
 *
 * PwDIncrementUseCount
 *
 ***********************************************************************
 */
int
PwDIncrementUseCount(PW_D_CONTEXT *psPwDContext, PW_T_CONTEXT *psPwTContext)
{
  char aacBaseNp1Tables[][PATHWELL_DB_TABLE_NAME_MAX_SIZE] =
  {
    "base4p1",
    "base5p1",
    "base6p1",
    "base7p1",
    "base8p1",
  };
  char aacBitmaskTables[][PATHWELL_DB_TABLE_NAME_MAX_SIZE] =
  {
    "bitmap2",
    "bitmap3",
    "bitmap3",
    "bitmap3",
    "bitmap3"
  };
  char *pcSql = NULL;
  char *pcTable = NULL;
  const char *pcError = NULL;
  const char *pcSqlFormat = "INSERT OR REPLACE INTO %s (id, use_count) VALUES (?, (COALESCE((SELECT use_count FROM %s WHERE id = ?), 0) + 1))";
  int iDbOpened = 0;
  int iError = ER_OK;
  int iSize = 0;
  int iSuccess = 0;
  int iTableIndex = 0;
  sqlite3_stmt *psStatement = NULL;

  /*-
   *********************************************************************
   *
   * Valid contexts are required.
   *
   *********************************************************************
   */
  if (PwDContextIsValid(psPwDContext) == PATHWELL_FALSE)
  {
    PwDSetError(psPwDContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwDContextIsValid(psPwDContext) == PATHWELL_TRUE);

  if (PwTContextIsValid(psPwTContext) == PATHWELL_FALSE)
  {
    PwDSetError(psPwDContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwTContextIsValid(psPwTContext) == PATHWELL_TRUE);

  /*-
   *********************************************************************
   *
   * Determine the token set and encoding in use.
   *
   *********************************************************************
   */
  switch (PwTGetTokenSet(psPwTContext))
  {
  case PATHWELL_TOKEN_SET_ID_1:
    iTableIndex = 0;
    break;
  case PATHWELL_TOKEN_SET_ID_2:
    iTableIndex = 1;
    break;
  case PATHWELL_TOKEN_SET_ID_3:
    iTableIndex = 2;
    break;
  case PATHWELL_TOKEN_SET_ID_4:
    iTableIndex = 3;
    break;
  default:
    PwDSetError(psPwDContext, "Invalid or unsupported token set.", NULL);
    return ER;
    break;
  }

  if (PwTGetEncoding(psPwTContext) == PATHWELL_ENCODING_BITMASK)
  {
    pcTable = aacBitmaskTables[iTableIndex];
  }
  else
  {
    pcTable = aacBaseNp1Tables[iTableIndex];
  }

  /*-
   *********************************************************************
   *
   * Allocate memory for and initialize the SQL statement.
   *
   *********************************************************************
   */
  iSize = strlen(pcSqlFormat) + (strlen(pcTable) * 2) - (strlen("%s") * 2) + 1;
  pcSql = malloc(sizeof(char) * iSize);
  if (pcSql == NULL)
  {
    PwDSetError(psPwDContext, "Failed to allocate memory (%s).", strerror(errno), NULL);
    return ER;
  }
  snprintf(pcSql, iSize, pcSqlFormat, pcTable, pcTable);

  /*-
   *********************************************************************
   *
   * Conditionally open the database.
   *
   *********************************************************************
   */
  if (psPwDContext->psDbHandle == NULL)
  {
    iError = PwDAcquireConnection(psPwDContext);
    if (iError != ER_OK)
    {
      return ER; // Note: Error message already set.
    }
    iDbOpened = 1;
  }

  /*-
   *********************************************************************
   *
   * Prepare the SQL statement and bind its parameters (if any).
   *
   *********************************************************************
   */
  iError = sqlite3_prepare_v2(psPwDContext->psDbHandle, pcSql, strlen(pcSql), &psStatement, NULL);
  if (iError != SQLITE_OK)
  {
    PwDSetError(psPwDContext, "Failed to prepare SQL statement (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }
  iError = sqlite3_bind_int64(psStatement, 1, PwTGetId(psPwTContext));
  if (iError != SQLITE_OK)
  {
    PwDSetError(psPwDContext, "Failed to bind SQL parameter (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }
  iError = sqlite3_bind_int64(psStatement, 2, PwTGetId(psPwTContext));
  if (iError != SQLITE_OK)
  {
    PwDSetError(psPwDContext, "Failed to bind SQL parameter (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }

  /*-
   *********************************************************************
   *
   * Execute the SQL statement.
   *
   *********************************************************************
   */
  iError = sqlite3_step(psStatement);
  if (iError != SQLITE_DONE)
  {
    PwDSetError(psPwDContext, "Failed to execute SQL statement (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }
  iSuccess = 1;

PW_D_CLEANUP:
  /*-
   *********************************************************************
   *
   * Release any locks/resources associated with the SQL statement.
   *
   *********************************************************************
   */
  if (psStatement != NULL)
  {
    sqlite3_finalize(psStatement);
  }

  if (pcSql != NULL)
  {
    free(pcSql);
  }

  /*-
   *********************************************************************
   *
   * Conditionally close the database.
   *
   *********************************************************************
   */
  if (psPwDContext->psDbHandle != NULL && iDbOpened)
  {
    iError = PwDReleaseConnection(psPwDContext);
    if (iError != ER_OK)
    {
      return ER; // Note: Error message already set.
    }
  }

  return (iSuccess) ? ER_OK : ER;
}


/*-
 ***********************************************************************
 *
 * PwDInitializeDatabase
 *
 ***********************************************************************
 */
int
PwDInitializeDatabase(PW_D_CONTEXT *psPwDContext)
{
  char *pcBlacklistedTopologies = NULL;
  int iError = 0;

  /*-
   *********************************************************************
   *
   * Create the database schema.
   *
   *********************************************************************
   */
  iError = PwDCreateSchema(psPwDContext);
  if (iError != ER_OK)
  {
    return ER; // Note: Error message already set.
  }

  /*-
   *********************************************************************
   *
   * Conditionally load the system-wide set of blacklisted topologies.
   *
   *********************************************************************
   */
  if (PwDDbFileExists(PATHWELL_CUSTOM_BLACKLIST_FILE))
  {
    pcBlacklistedTopologies = PATHWELL_CUSTOM_BLACKLIST_FILE;
  }
  else
  {
    if (PwDDbFileExists(PATHWELL_COMMON_BLACKLIST_FILE))
    {
      pcBlacklistedTopologies = PATHWELL_COMMON_BLACKLIST_FILE;
    }
    else
    {
      pcBlacklistedTopologies = NULL; // No custom/common file exists, but that's OK.
    }
  }
  if (pcBlacklistedTopologies != NULL)
  {
    iError = PwDLoadBlacklistedTopologies(psPwDContext, pcBlacklistedTopologies);
    if (iError != ER_OK)
    {
      return ER; // Note: Error message already set.
    }
  }

  return ER_OK;
}


/*-
 ***********************************************************************
 *
 * PwDLoadBlacklistedTopologies
 *
 ***********************************************************************
 */
int
PwDLoadBlacklistedTopologies(PW_D_CONTEXT *psPwDContext, char *pcFile)
{
  char *pcSql = NULL;
  char *pcSqlError = NULL;
  char *pcTopology = NULL;
  char **ppcTopologies = NULL;
  const char *pcError = NULL;
  const char *pcSqlFormat = "INSERT OR REPLACE INTO topology_blacklist (topology) VALUES (?)";
  FILE *pFile = NULL;
  int iCount = 0;
  int iDbOpened = 0;
  int iError = ER_OK;
  int iIndex = 0;
  int iSize = 0;
  int iSuccess = 0;
  sqlite3_stmt *psStatement = NULL;

  /*-
   *********************************************************************
   *
   * A valid context and file argument are required.
   *
   *********************************************************************
   */
  if (PwDContextIsValid(psPwDContext) == PATHWELL_FALSE)
  {
    PwDSetError(psPwDContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwDContextIsValid(psPwDContext) == PATHWELL_TRUE);

  if (pcFile == NULL)
  {
    PwDSetError(psPwDContext, "File argument is not defined. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(pcFile != NULL);

  /*-
   *********************************************************************
   *
   * Open the input file.
   *
   *********************************************************************
   */
  pFile = fopen(pcFile, "r");
  if (pFile == NULL)
  {
    PwDSetError(psPwDContext, "Failed to open \"%s\" (%s).", pcFile, strerror(errno), NULL);
    return ER;
  }

  /*-
   *********************************************************************
   *
   * Allocate memory for and initialize the SQL statement.
   *
   *********************************************************************
   */
  iSize = strlen(pcSqlFormat) + 1;
  pcSql = malloc(sizeof(char) * iSize);
  if (pcSql == NULL)
  {
    PwDSetError(psPwDContext, "Failed to allocate memory (%s).", strerror(errno), NULL);
    return ER;
  }
  snprintf(pcSql, iSize, "%s", pcSqlFormat);

  /*-
   *********************************************************************
   *
   * Conditionally open the database.
   *
   *********************************************************************
   */
  if (psPwDContext->psDbHandle == NULL)
  {
    iError = PwDAcquireConnection(psPwDContext);
    if (iError != ER_OK)
    {
      return ER; // Note: Error message already set.
    }
    iDbOpened = 1;
  }

  /*-
   *********************************************************************
   *
   * Prepare the SQL statement.
   *
   *********************************************************************
   */
  iError = sqlite3_prepare_v2(psPwDContext->psDbHandle, pcSql, strlen(pcSql), &psStatement, NULL);
  if (iError != SQLITE_OK)
  {
    PwDSetError(psPwDContext, "Failed to prepare SQL statement (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }

  /*-
   *********************************************************************
   *
   * Allocate memory to hold bundles of topologies.
   *
   *********************************************************************
   */
  ppcTopologies = (char **)malloc(sizeof(char*) * PATHWELL_DB_BUNDLE_SIZE);
  if (ppcTopologies == NULL)
  {
    PwDSetError(psPwDContext, "Failed to allocate memory (%s).", strerror(errno), NULL);
    goto PW_D_CLEANUP;
  }
  for (iIndex = 0; iIndex < PATHWELL_DB_BUNDLE_SIZE; iIndex++)
  {
    ppcTopologies[iIndex] = (char *)malloc(sizeof(char) * PATHWELL_MAX_TOPOLOGY_SIZE);
    if (ppcTopologies[iIndex] == NULL)
    {
      PwDSetError(psPwDContext, "Failed to allocate memory (%s).", strerror(errno), NULL);
      goto PW_D_CLEANUP;
    }
  }

  /*-
   *********************************************************************
   *
   * Process the input file one line at a time, but load topologies in
   * bundles since that's more efficient.
   *
   *********************************************************************
   */
  iCount = 0;
  while (fgets(ppcTopologies[iCount], PATHWELL_MAX_TOPOLOGY_SIZE, pFile) != NULL)
  {
    if (++iCount % PATHWELL_DB_BUNDLE_SIZE == 0)
    {
      iError = sqlite3_exec(psPwDContext->psDbHandle, "BEGIN TRANSACTION", NULL, NULL, &pcSqlError);
      if (iError != SQLITE_OK)
      {
        PwDSetError(psPwDContext, "%s", pcSqlError, NULL);
        goto PW_D_CLEANUP;
      }
      for (iIndex = 0; iIndex < iCount; iIndex++)
      {
        pcTopology = PwSChomp(ppcTopologies[iIndex], " \n\r\t", PW_S_CHOMP_BOTH);
        if (pcTopology == NULL)
        {
          PwDSetError(psPwDContext, "Failed to chomp blacklisted topology. That should not happen.", NULL);
          goto PW_D_CLEANUP;
        }
        if (pcTopology[0] == 0)
        {
          free(pcTopology);
          continue; // Ignore blank lines.
        }
//FIXME Insert a regular expression test here to ensure that the string represents a valid topology.
        iError = sqlite3_bind_text(psStatement, 1, pcTopology, -1, SQLITE_TRANSIENT);
        if (iError != SQLITE_OK)
        {
          PwDSetError(psPwDContext, "Failed to bind SQL parameter (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
          goto PW_D_CLEANUP;
        }
        iError = sqlite3_step(psStatement);
        if (iError != SQLITE_DONE)
        {
          PwDSetError(psPwDContext, "Failed to execute SQL statement (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
          goto PW_D_CLEANUP;
        }
        iError = sqlite3_reset(psStatement);
        if (iError != SQLITE_OK)
        {
          PwDSetError(psPwDContext, "Failed to execute SQL statement (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
          goto PW_D_CLEANUP;
        }
        free(pcTopology);
      }
      sqlite3_exec(psPwDContext->psDbHandle, "END TRANSACTION", NULL, NULL, &pcSqlError);
      if (iError != SQLITE_OK)
      {
        PwDSetError(psPwDContext, "%s", pcSqlError, NULL);
        goto PW_D_CLEANUP;
      }
      iCount = 0;
    }
  }
  if (ferror(pFile))
  {
    PwDSetError(psPwDContext, "Read error (%s).", strerror(errno), NULL);
    goto PW_D_CLEANUP;
  }
  if (iCount > 0)
  {
    iError = sqlite3_exec(psPwDContext->psDbHandle, "BEGIN TRANSACTION", NULL, NULL, &pcSqlError);
    if (iError != SQLITE_OK)
    {
      PwDSetError(psPwDContext, "%s", pcSqlError, NULL);
      goto PW_D_CLEANUP;
    }
    for (iIndex = 0; iIndex < iCount; iIndex++)
    {
      pcTopology = PwSChomp(ppcTopologies[iIndex], " \n\r\t", PW_S_CHOMP_BOTH);
      if (pcTopology == NULL)
      {
        PwDSetError(psPwDContext, "Failed to chomp blacklisted topology. That should not happen.", NULL);
        goto PW_D_CLEANUP;
      }
      if (pcTopology[0] == 0)
      {
        free(pcTopology);
        continue; // Ignore blank lines.
      }
//FIXME Insert a regular expression test here to ensure that the string represents a valid topology.
      iError = sqlite3_bind_text(psStatement, 1, pcTopology, -1, SQLITE_TRANSIENT);
      if (iError != SQLITE_OK)
      {
        PwDSetError(psPwDContext, "Failed to bind SQL parameter (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
        goto PW_D_CLEANUP;
      }
      iError = sqlite3_step(psStatement);
      if (iError != SQLITE_DONE)
      {
        PwDSetError(psPwDContext, "Failed to execute SQL statement (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
        goto PW_D_CLEANUP;
      }
      iError = sqlite3_reset(psStatement);
      if (iError != SQLITE_OK)
      {
        PwDSetError(psPwDContext, "Failed to execute SQL statement (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
        goto PW_D_CLEANUP;
      }
      free(pcTopology);
    }
    sqlite3_exec(psPwDContext->psDbHandle, "END TRANSACTION", NULL, NULL, &pcSqlError);
    if (iError != SQLITE_OK)
    {
      PwDSetError(psPwDContext, "%s", pcSqlError, NULL);
      goto PW_D_CLEANUP;
    }
  }
  iSuccess = 1;

PW_D_CLEANUP:
  /*-
   *********************************************************************
   *
   * Release any locks/resources associated with the SQL statement.
   *
   *********************************************************************
   */
  if (psStatement != NULL)
  {
    sqlite3_finalize(psStatement);
  }

  if (pcSql != NULL)
  {
    free(pcSql);
  }

  if (pcSqlError != NULL)
  {
    sqlite3_free(pcSqlError);
  }

  if (pFile != NULL)
  {
    fclose(pFile);
  }

  if (ppcTopologies != NULL)
  {
    for (iIndex = 0; iIndex < PATHWELL_DB_BUNDLE_SIZE; iIndex++)
    {
      if (ppcTopologies[iIndex] != NULL)
      {
        free(ppcTopologies[iIndex]);
      }
    }
    free(ppcTopologies);
  }

  /*-
   *********************************************************************
   *
   * Conditionally close the database.
   *
   *********************************************************************
   */
  if (psPwDContext->psDbHandle != NULL && iDbOpened)
  {
    iError = PwDReleaseConnection(psPwDContext);
    if (iError != ER_OK)
    {
      return ER; // Note: Error message already set.
    }
  }

  return (iSuccess) ? ER_OK : ER;
}


/*-
 ***********************************************************************
 *
 * PwDNewContextFromParameters
 *
 ***********************************************************************
 */
PW_D_CONTEXT *
PwDNewContextFromParameters(char *pcDbFile, int iDbFileFlags, char *pcDbLock, int iDbLockFlags)
{
  int iSize = 0;
  PW_D_CONTEXT *psPwDContext = NULL;

  /*-
   *********************************************************************
   *
   * Allocate and initialize memory for a new context structure.
   *
   *********************************************************************
   */
  psPwDContext = calloc(1, sizeof(PW_D_CONTEXT));
  if (psPwDContext == NULL)
  {
    return NULL;
  }

  /*-
   *********************************************************************
   *
   * Initialize members related to the DB file. If no DB file was given,
   * assume the caller wants to use the default value.
   *
   *********************************************************************
   */
  if (pcDbFile == NULL)
  {
    pcDbFile = PATHWELL_DEFAULT_DB_FILE;
  }
  iSize = strlen(pcDbFile) + 1;
  psPwDContext->pcDbFile = (char *)malloc(sizeof(char) * iSize);
  if (psPwDContext->pcDbFile == NULL)
  {
    PwDSetError(psPwDContext, "Failed to allocate memory (%s).", strerror(errno), NULL);
    return psPwDContext;
  }
  snprintf(psPwDContext->pcDbFile, iSize, "%s", pcDbFile);
  psPwDContext->iDbFileFlags = iDbFileFlags;

  /*-
   *********************************************************************
   *
   * Initialize members related to the DB file. If no DB lock was given,
   * assume the caller wants to use the default value, which is derived.
   *
   *********************************************************************
   */
  if (pcDbLock == NULL)
  {
    iSize = strlen(pcDbFile) + strlen(".lock") + 1;
  }
  else
  {
    iSize = strlen(pcDbLock) + 1;
  }
  psPwDContext->pcDbLock = (char *)malloc(sizeof(char) * iSize);
  if (psPwDContext->pcDbLock == NULL)
  {
    PwDSetError(psPwDContext, "Failed to allocate memory (%s).", strerror(errno), NULL);
    return psPwDContext;
  }
  if (pcDbLock == NULL)
  {
    snprintf(psPwDContext->pcDbLock, iSize, "%s%s", pcDbFile, ".lock");
  }
  else
  {
    snprintf(psPwDContext->pcDbLock, iSize, "%s", pcDbLock);
  }
  psPwDContext->iDbLockFlags = iDbLockFlags;

  /*-
   *********************************************************************
   *
   * If we made it this far, the context is considered valid.
   *
   *********************************************************************
   */
  psPwDContext->iValid = PATHWELL_TRUE;

  return psPwDContext;
}


/*-
 ***********************************************************************
 *
 * PwDReleaseConnection
 *
 ***********************************************************************
 */
int
PwDReleaseConnection(PW_D_CONTEXT *psPwDContext)
{
  int iError = ER_OK;

  /*-
   *********************************************************************
   *
   * A valid context is required.
   *
   *********************************************************************
   */
  if (PwDContextIsValid(psPwDContext) == PATHWELL_FALSE)
  {
    PwDSetError(psPwDContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwDContextIsValid(psPwDContext) == PATHWELL_TRUE);

  /*-
   *********************************************************************
   *
   * Conditionally close the database.
   *
   *********************************************************************
   */
  if (psPwDContext->psDbHandle != NULL)
  {
    iError = sqlite3_close(psPwDContext->psDbHandle);
    if (iError != SQLITE_OK)
    {
      PwDSetError(psPwDContext, "Failed to release DB connection.", NULL);
      return ER;
    }
    psPwDContext->psDbHandle = NULL;
  }

  /*-
   *********************************************************************
   *
   * Conditionally free library resources (based on reference count).
   *
   *********************************************************************
   */
  iError = PwDFreeLibraryResources(psPwDContext, 0);
  if (iError != SQLITE_OK)
  {
    return ER; // Note: Error message already set.
  }

  return ER_OK;
}


/*-
 ***********************************************************************
 *
 * PwDSetSchemaVersion
 *
 ***********************************************************************
 */
int
PwDSetSchemaVersion(PW_D_CONTEXT *psPwDContext, int iVersion, int iForce)
{
  char acError[(10 * 2) + 4 + 1] = ""; // (strlen("4294967295") * 2) + strlen(" != ") + 1
  char *pcSql = NULL;
  const char *pcError = NULL;
  const char *pcSqlStatement = "INSERT OR REPLACE INTO version (id, schema) VALUES (1, ?)";
  int iDbOpened = 0;
  int iError = ER_OK;
  int iSize = 0;
  int iSuccess = 0;
  sqlite3_stmt *psStatement = NULL;

  /*-
   *********************************************************************
   *
   * A valid context is required.
   *
   *********************************************************************
   */
  if (PwDContextIsValid(psPwDContext) == PATHWELL_FALSE)
  {
    PwDSetError(psPwDContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwDContextIsValid(psPwDContext) == PATHWELL_TRUE);

  /*-
   *********************************************************************
   *
   * Normally, the specified schema version must match the one that was
   * compiled into the library, but if the caller forces the issue, the
   * request will be honored.
   *
   *********************************************************************
   */
  if (iVersion != PW_V_DB_SCHEMA_VERSION && iForce != PATHWELL_TRUE)
  {
    snprintf(acError, 25, "%d != %d", iVersion, PW_V_DB_SCHEMA_VERSION);
    PwDSetError(psPwDContext, "DB version mismatch (%s).", acError, NULL);
    return ER;
  }

  /*-
   *********************************************************************
   *
   * Allocate memory for and initialize the SQL statement.
   *
   *********************************************************************
   */
  iSize = strlen(pcSqlStatement) + 1;
  pcSql = malloc(sizeof(char) * iSize);
  if (pcSql == NULL)
  {
    PwDSetError(psPwDContext, "Failed to allocate memory (%s).", strerror(errno), NULL);
    return ER;
  }
  snprintf(pcSql, iSize, "%s", pcSqlStatement);

  /*-
   *********************************************************************
   *
   * Conditionally open the database.
   *
   *********************************************************************
   */
  if (psPwDContext->psDbHandle == NULL)
  {
    iError = PwDAcquireConnection(psPwDContext);
    if (iError != ER_OK)
    {
      return ER; // Note: Error message already set.
    }
    iDbOpened = 1;
  }

  /*-
   *********************************************************************
   *
   * Prepare the SQL statement and bind its parameters (if any).
   *
   *********************************************************************
   */
  iError = sqlite3_prepare_v2(psPwDContext->psDbHandle, pcSql, strlen(pcSql), &psStatement, NULL);
  if (iError != SQLITE_OK)
  {
    PwDSetError(psPwDContext, "Failed to prepare SQL statement (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }
  iError = sqlite3_bind_int(psStatement, 1, iVersion);
  if (iError != SQLITE_OK)
  {
    PwDSetError(psPwDContext, "Failed to bind SQL parameter (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }

  /*-
   *********************************************************************
   *
   * Execute the SQL statement.
   *
   *********************************************************************
   */
  iError = sqlite3_step(psStatement);
  if (iError != SQLITE_DONE)
  {
    PwDSetError(psPwDContext, "Failed to execute SQL statement (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }
  iSuccess = 1;

PW_D_CLEANUP:
  /*-
   *********************************************************************
   *
   * Release any locks/resources associated with the SQL statement.
   *
   *********************************************************************
   */
  if (psStatement != NULL)
  {
    sqlite3_finalize(psStatement);
  }

  if (pcSql != NULL)
  {
    free(pcSql);
  }

  /*-
   *********************************************************************
   *
   * Conditionally close the database.
   *
   *********************************************************************
   */
  if (psPwDContext->psDbHandle != NULL && iDbOpened)
  {
    iError = PwDReleaseConnection(psPwDContext);
    if (iError != ER_OK)
    {
      return ER; // Note: Error message already set.
    }
  }

  return (iSuccess) ? ER_OK : ER;
}


/*-
 ***********************************************************************
 *
 * PwDSetUseCount
 *
 ***********************************************************************
 */
int
PwDSetUseCount(PW_D_CONTEXT *psPwDContext, PW_T_CONTEXT *psPwTContext, unsigned int uiUseCount)
{
  char aacBaseNp1Tables[][PATHWELL_DB_TABLE_NAME_MAX_SIZE] =
  {
    "base4p1",
    "base5p1",
    "base6p1",
    "base7p1",
    "base8p1",
  };
  char aacBitmaskTables[][PATHWELL_DB_TABLE_NAME_MAX_SIZE] =
  {
    "bitmap2",
    "bitmap3",
    "bitmap3",
    "bitmap3",
    "bitmap3"
  };
  char *pcSql = NULL;
  char *pcTable = NULL;
  const char *pcError = NULL;
  const char *pcSqlFormat = "INSERT OR REPLACE INTO %s (id, use_count) VALUES (?, ?)";
  int iDbOpened = 0;
  int iError = ER_OK;
  int iSize = 0;
  int iSuccess = 0;
  int iTableIndex = 0;
  sqlite3_stmt *psStatement = NULL;

  /*-
   *********************************************************************
   *
   * Valid contexts are required.
   *
   *********************************************************************
   */
  if (PwDContextIsValid(psPwDContext) == PATHWELL_FALSE)
  {
    PwDSetError(psPwDContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwDContextIsValid(psPwDContext) == PATHWELL_TRUE);

  if (PwTContextIsValid(psPwTContext) == PATHWELL_FALSE)
  {
    PwDSetError(psPwDContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwTContextIsValid(psPwTContext) == PATHWELL_TRUE);

  /*-
   *********************************************************************
   *
   * The new use count must be >= 0.
   *
   *********************************************************************
   */
//FIXME Since uiUseCount is unsigned, it will never be less than zero.
//      Some compilers will complain about this being an autological
//      comparison, so the code has been commented out, but retained
//      as a reminder in case use counts ever become signed integers.
//if (uiUseCount < 0)
//{
//  PwDSetError(psPwDContext, "New use count: (%" PRId64 ") must be greater than or equal to 0.", uiUseCount, NULL);
//  return ER;
//}
  PATHWELL_ASSERT(uiUseCount >= 0);

  /*-
   *********************************************************************
   *
   * Determine the token set and encoding in use.
   *
   *********************************************************************
   */
  switch (PwTGetTokenSet(psPwTContext))
  {
  case PATHWELL_TOKEN_SET_ID_1:
    iTableIndex = 0;
    break;
  case PATHWELL_TOKEN_SET_ID_2:
    iTableIndex = 1;
    break;
  case PATHWELL_TOKEN_SET_ID_3:
    iTableIndex = 2;
    break;
  case PATHWELL_TOKEN_SET_ID_4:
    iTableIndex = 3;
    break;
  default:
    PwDSetError(psPwDContext, "Invalid or unsupported token set.", NULL);
    return ER;
    break;
  }

  if (PwTGetEncoding(psPwTContext) == PATHWELL_ENCODING_BITMASK)
  {
    pcTable = aacBitmaskTables[iTableIndex];
  }
  else
  {
    pcTable = aacBaseNp1Tables[iTableIndex];
  }

  /*-
   *********************************************************************
   *
   * Allocate memory for and initialize the SQL statement.
   *
   *********************************************************************
   */
  iSize = strlen(pcSqlFormat) + (strlen(pcTable) * 1) - (strlen("%s") * 1) + 1;
  pcSql = malloc(sizeof(char) * iSize);
  if (pcSql == NULL)
  {
    PwDSetError(psPwDContext, "Failed to allocate memory (%s).", strerror(errno), NULL);
    return ER;
  }
  snprintf(pcSql, iSize, pcSqlFormat, pcTable);

  /*-
   *********************************************************************
   *
   * Conditionally open the database.
   *
   *********************************************************************
   */
  if (psPwDContext->psDbHandle == NULL)
  {
    iError = PwDAcquireConnection(psPwDContext);
    if (iError != ER_OK)
    {
      return ER; // Note: Error message already set.
    }
    iDbOpened = 1;
  }

  /*-
   *********************************************************************
   *
   * Prepare the SQL statement and bind its parameters (if any).
   *
   *********************************************************************
   */
  iError = sqlite3_prepare_v2(psPwDContext->psDbHandle, pcSql, strlen(pcSql), &psStatement, NULL);
  if (iError != SQLITE_OK)
  {
    PwDSetError(psPwDContext, "Failed to prepare SQL statement (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }
  iError = sqlite3_bind_int64(psStatement, 1, PwTGetId(psPwTContext));
  if (iError != SQLITE_OK)
  {
    PwDSetError(psPwDContext, "Failed to bind SQL parameter (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }
  iError = sqlite3_bind_int64(psStatement, 2, uiUseCount);
  if (iError != SQLITE_OK)
  {
    PwDSetError(psPwDContext, "Failed to bind SQL parameter (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }

  /*-
   *********************************************************************
   *
   * Execute the SQL statement.
   *
   *********************************************************************
   */
  iError = sqlite3_step(psStatement);
  if (iError != SQLITE_DONE)
  {
    PwDSetError(psPwDContext, "Failed to execute SQL statement (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }
  iSuccess = 1;

PW_D_CLEANUP:
  /*-
   *********************************************************************
   *
   * Release any locks/resources associated with the SQL statement.
   *
   *********************************************************************
   */
  if (psStatement != NULL)
  {
    sqlite3_finalize(psStatement);
  }

  if (pcSql != NULL)
  {
    free(pcSql);
  }

  /*-
   *********************************************************************
   *
   * Conditionally close the database.
   *
   *********************************************************************
   */
  if (psPwDContext->psDbHandle != NULL && iDbOpened)
  {
    iError = PwDReleaseConnection(psPwDContext);
    if (iError != ER_OK)
    {
      return ER; // Note: Error message already set.
    }
  }

  return (iSuccess) ? ER_OK : ER;
}


/*-
 ***********************************************************************
 *
 * PwDTopologyIsBlacklisted
 *
 ***********************************************************************
 */
int
PwDTopologyIsBlacklisted(PW_D_CONTEXT *psPwDContext, char *pcTopology)
{
  char *pcSql = NULL;
  const char *pcError = NULL;
  const char *pcSqlFormat = "SELECT COUNT(topology) FROM topology_blacklist WHERE topology = ?";
  int iDbOpened = 0;
  int iError = ER_OK;
  int iResult = PATHWELL_FALSE;
  int iRows = 0;
  int iSize = 0;
  int iSuccess = 0;
  int iValue = 0;
  sqlite3_stmt *psStatement = NULL;

  /*-
   *********************************************************************
   *
   * A valid context and topology argument is required.
   *
   *********************************************************************
   */
  if (PwDContextIsValid(psPwDContext) == PATHWELL_FALSE)
  {
    PwDSetError(psPwDContext, "Invalid context. That should not happen.", NULL);
    return PATHWELL_INDETERMINATE;
  }
  PATHWELL_ASSERT(PwDContextIsValid(psPwDContext) == PATHWELL_TRUE);

  if (pcTopology == NULL)
  {
    PwDSetError(psPwDContext, "Topology argument is not defined. That should not happen.", NULL);
    return PATHWELL_INDETERMINATE;
  }
  PATHWELL_ASSERT(pcTopology != NULL);

  /*-
   *********************************************************************
   *
   * Allocate memory for and initialize the SQL statement.
   *
   *********************************************************************
   */
  iSize = strlen(pcSqlFormat) + 1;
  pcSql = malloc(sizeof(char) * iSize);
  if (pcSql == NULL)
  {
    PwDSetError(psPwDContext, "Failed to allocate memory (%s).", strerror(errno), NULL);
    return PATHWELL_INDETERMINATE;
  }
  snprintf(pcSql, iSize, "%s", pcSqlFormat);

  /*-
   *********************************************************************
   *
   * Conditionally open the database.
   *
   *********************************************************************
   */
  if (psPwDContext->psDbHandle == NULL)
  {
    iError = PwDAcquireConnection(psPwDContext);
    if (iError != ER_OK)
    {
      return PATHWELL_INDETERMINATE; // Note: Error message already set.
    }
    iDbOpened = 1;
  }

  /*-
   *********************************************************************
   *
   * Prepare the SQL statement and bind its parameters (if any).
   *
   *********************************************************************
   */
  iError = sqlite3_prepare_v2(psPwDContext->psDbHandle, pcSql, strlen(pcSql), &psStatement, NULL);
  if (iError != SQLITE_OK)
  {
    PwDSetError(psPwDContext, "Failed to prepare SQL statement (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }
  iError = sqlite3_bind_text(psStatement, 1, pcTopology, strlen(pcTopology), SQLITE_TRANSIENT);
  if (iError != SQLITE_OK)
  {
    PwDSetError(psPwDContext, "Failed to bind SQL parameter (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }

  /*-
   *********************************************************************
   *
   * Execute the SQL statement.
   *
   *********************************************************************
   */
  for (iRows = 0; (iError = sqlite3_step(psStatement)) == SQLITE_ROW; iRows++)
  {
    iValue = sqlite3_column_int(psStatement, 0);
  }
  if (iError != SQLITE_DONE)
  {
    PwDSetError(psPwDContext, "Failed to execute SQL statement (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }
  if (iRows != 1)
  {
    char acRows[PATHWELL_MAX_32BIT_NUMBER_SIZE] = "";
    snprintf(acRows, PATHWELL_MAX_32BIT_NUMBER_SIZE, "%d", iRows);
    PwDSetError(psPwDContext, "Expected SQL statement to return no more than one row, but got %s instead.", acRows, NULL);
    goto PW_D_CLEANUP;
  }
  iResult = (iValue) ? PATHWELL_TRUE : PATHWELL_FALSE;
  iSuccess = 1;

PW_D_CLEANUP:
  /*-
   *********************************************************************
   *
   * Release any locks/resources associated with the SQL statement.
   *
   *********************************************************************
   */
  if (psStatement != NULL)
  {
    sqlite3_finalize(psStatement);
  }

  if (pcSql != NULL)
  {
    free(pcSql);
  }

  /*-
   *********************************************************************
   *
   * Conditionally close the database.
   *
   *********************************************************************
   */
  if (psPwDContext->psDbHandle != NULL && iDbOpened)
  {
    iError = PwDReleaseConnection(psPwDContext);
    if (iError != ER_OK)
    {
      return PATHWELL_INDETERMINATE; // Note: Error message already set.
    }
  }

  return (iSuccess) ? iResult : PATHWELL_INDETERMINATE;
}


/*-
 ***********************************************************************
 *
 * PwDTopologyIsInUse
 *
 ***********************************************************************
 */
int
PwDTopologyIsInUse(PW_D_CONTEXT *psPwDContext, PW_T_CONTEXT *psPwTContext)
{
  char aacBaseNp1Tables[][PATHWELL_DB_TABLE_NAME_MAX_SIZE] =
  {
    "base4p1",
    "base5p1",
    "base6p1",
    "base7p1",
    "base8p1",
  };
  char aacBitmaskTables[][PATHWELL_DB_TABLE_NAME_MAX_SIZE] =
  {
    "bitmap2",
    "bitmap3",
    "bitmap3",
    "bitmap3",
    "bitmap3"
  };
  char *pcSql = NULL;
  char *pcTable = NULL;
  const char *pcError = NULL;
  const char *pcSqlFormat = "SELECT COALESCE(use_count, 0) FROM %s WHERE id = ?";
  int iDbOpened = 0;
  int iError = ER_OK;
  int iResult = PATHWELL_FALSE;
  int iRows = 0;
  int iSize = 0;
  int iSuccess = 0;
  int iTableIndex = 0;
  int iValue = 0;
  sqlite3_stmt *psStatement = NULL;

  /*-
   *********************************************************************
   *
   * Valid contexts are required.
   *
   *********************************************************************
   */
  if (PwDContextIsValid(psPwDContext) == PATHWELL_FALSE)
  {
    PwDSetError(psPwDContext, "Invalid context. That should not happen.", NULL);
    return PATHWELL_INDETERMINATE;
  }
  PATHWELL_ASSERT(PwDContextIsValid(psPwDContext) == PATHWELL_TRUE);

  if (PwTContextIsValid(psPwTContext) == PATHWELL_FALSE)
  {
    PwDSetError(psPwDContext, "Invalid context. That should not happen.", NULL);
    return PATHWELL_INDETERMINATE;
  }
  PATHWELL_ASSERT(PwTContextIsValid(psPwTContext) == PATHWELL_TRUE);

  /*-
   *********************************************************************
   *
   * Determine the token set and encoding in use.
   *
   *********************************************************************
   */
  switch (PwTGetTokenSet(psPwTContext))
  {
  case PATHWELL_TOKEN_SET_ID_1:
    iTableIndex = 0;
    break;
  case PATHWELL_TOKEN_SET_ID_2:
    iTableIndex = 1;
    break;
  case PATHWELL_TOKEN_SET_ID_3:
    iTableIndex = 2;
    break;
  case PATHWELL_TOKEN_SET_ID_4:
    iTableIndex = 3;
    break;
  default:
    PwDSetError(psPwDContext, "Invalid or unsupported token set.", NULL);
    return PATHWELL_INDETERMINATE;
    break;
  }

  if (PwTGetEncoding(psPwTContext) == PATHWELL_ENCODING_BITMASK)
  {
    pcTable = aacBitmaskTables[iTableIndex];
  }
  else
  {
    pcTable = aacBaseNp1Tables[iTableIndex];
  }

  /*-
   *********************************************************************
   *
   * Allocate memory for and initialize the SQL statement.
   *
   *********************************************************************
   */
  iSize = strlen(pcSqlFormat) + strlen(pcTable) - strlen("%s") + 1;
  pcSql = malloc(sizeof(char) * iSize);
  if (pcSql == NULL)
  {
    PwDSetError(psPwDContext, "Failed to allocate memory (%s).", strerror(errno), NULL);
    return PATHWELL_INDETERMINATE;
  }
  snprintf(pcSql, iSize, pcSqlFormat, pcTable);

  /*-
   *********************************************************************
   *
   * Conditionally open the database.
   *
   *********************************************************************
   */
  if (psPwDContext->psDbHandle == NULL)
  {
    iError = PwDAcquireConnection(psPwDContext);
    if (iError != ER_OK)
    {
      return PATHWELL_INDETERMINATE; // Note: Error message already set.
    }
    iDbOpened = 1;
  }

  /*-
   *********************************************************************
   *
   * Prepare the SQL statement and bind its parameters (if any).
   *
   *********************************************************************
   */
  iError = sqlite3_prepare_v2(psPwDContext->psDbHandle, pcSql, strlen(pcSql), &psStatement, NULL);
  if (iError != SQLITE_OK)
  {
    PwDSetError(psPwDContext, "Failed to prepare SQL statement (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }
  iError = sqlite3_bind_int64(psStatement, 1, PwTGetId(psPwTContext));
  if (iError != SQLITE_OK)
  {
    PwDSetError(psPwDContext, "Failed to bind SQL parameter (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }

  /*-
   *********************************************************************
   *
   * Execute the SQL statement.
   *
   *********************************************************************
   */
  for (iRows = 0; (iError = sqlite3_step(psStatement)) == SQLITE_ROW; iRows++)
  {
    iValue = sqlite3_column_int(psStatement, 0);
  }
  if (iError != SQLITE_DONE)
  {
    PwDSetError(psPwDContext, "Failed to execute SQL statement (%s).", ((pcError = sqlite3_errmsg(psPwDContext->psDbHandle)) == NULL) ? "null" : pcError, NULL);
    goto PW_D_CLEANUP;
  }
  if (iRows != 1)
  {
    char acRows[PATHWELL_MAX_32BIT_NUMBER_SIZE] = "";
    snprintf(acRows, PATHWELL_MAX_32BIT_NUMBER_SIZE, "%d", iRows);
    PwDSetError(psPwDContext, "Expected SQL statement to return no more than one row, but got %s instead.", acRows, NULL);
    goto PW_D_CLEANUP;
  }
  iResult = (iValue) ? PATHWELL_TRUE : PATHWELL_FALSE;
  iSuccess = 1;

PW_D_CLEANUP:
  /*-
   *********************************************************************
   *
   * Release any locks/resources associated with the SQL statement.
   *
   *********************************************************************
   */
  if (psStatement != NULL)
  {
    sqlite3_finalize(psStatement);
  }

  if (pcSql != NULL)
  {
    free(pcSql);
  }

  /*-
   *********************************************************************
   *
   * Conditionally close the database.
   *
   *********************************************************************
   */
  if (psPwDContext->psDbHandle != NULL && iDbOpened)
  {
    iError = PwDReleaseConnection(psPwDContext);
    if (iError != ER_OK)
    {
      return PATHWELL_INDETERMINATE; // Note: Error message already set.
    }
  }

  return (iSuccess) ? iResult : PATHWELL_INDETERMINATE;
}


/*-
 ***********************************************************************
 *
 * PwDVerifySchema
 *
 ***********************************************************************
 */
int
PwDVerifySchema(PW_D_CONTEXT *psPwDContext)
{
  char acError[(10 * 2) + 4 + 1] = ""; // (strlen("4294967295") * 2) + strlen(" != ") + 1
  int *piVersion = NULL;

  /*-
   *********************************************************************
   *
   * Make sure that the schema version is correct.
   *
   *********************************************************************
   */
  piVersion = PwDGetSchemaVersion(psPwDContext);
  if (piVersion == NULL && PwDGetError(psPwDContext) != NULL)
  {
    return ER; // Note: Error message already set.
  }
  if (piVersion == NULL)
  {
    PwDSetError(psPwDContext, "DB version undefined. Make sure the DB has been initialized properly.", acError, NULL);
    return ER;
  }
  if (*piVersion != PW_V_DB_SCHEMA_VERSION)
  {
    snprintf(acError, 25, "%d != %d", *piVersion, PW_V_DB_SCHEMA_VERSION);
    PwDSetError(psPwDContext, "DB version mismatch (%s).", acError, NULL);
    free(piVersion);
    return ER;
  }
  free(piVersion);

  return ER_OK;
}


/*-
 ***********************************************************************
 *
 * PwESetError
 *
 ***********************************************************************
 */
int
PwESetError(PW_E_CONTEXT *psPwEContext, const char *pcFormat, ...)
{
  char *pcArgument = NULL;
  char *pcError = NULL;
  int iSize = 0;
  va_list pVaList;

  if (psPwEContext == NULL)
  {
    return ER; // There's nothing we can do about this one.
  }
  PATHWELL_ASSERT(psPwEContext != NULL);
  if (pcFormat == NULL)
  {
    psPwEContext->pcError = (char *)gpcRealErrorLostNullFormat;
    return ER;
  }
  PATHWELL_ASSERT(pcFormat != NULL);
  va_start(pVaList, pcFormat);
  for (pcArgument = (char *)pcFormat, iSize = 0; pcArgument != NULL; pcArgument = va_arg(pVaList, char *))
  {
    iSize += strlen(pcArgument);
  }
  iSize++;
  va_end(pVaList);
  pcError = (char *)realloc(psPwEContext->pcError, (sizeof(char) * iSize));
  if (pcError == NULL)
  {
    psPwEContext->pcError = (char *)gpcRealErrorLostNullMalloc;
    return ER;
  }
  va_start(pVaList, pcFormat);
  vsnprintf(pcError, iSize, pcFormat, pVaList);
  va_end(pVaList);
  psPwEContext->pcError = pcError;

  return ER_OK;
}


/*-
 ***********************************************************************
 *
 * PwLCalcLevDistance
 *
 ***********************************************************************
 */
int
PwLCalcLevDistance(PW_L_CONTEXT *psPwLContext)
{
  char acStringA[PATHWELL_MAX_TOPOLOGY_SIZE];
  char acStringB[PATHWELL_MAX_TOPOLOGY_SIZE];
  const char *pcTopologyA = NULL;
  const char *pcTopologyB = NULL;
  int iDistance = -1;
  int iError = 0;
  int iIndex = 0;
  int iLengthA = 0;
  int iLengthB = 0;
  PW_T_CONTEXT *psPwTContextA = NULL;
  PW_T_CONTEXT *psPwTContextB = NULL;

  /*-
   *********************************************************************
   *
   * A valid context is required.
   *
   *********************************************************************
   */
  if (PwLContextIsValid(psPwLContext) == PATHWELL_FALSE)
  {
    PwLSetError(psPwLContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwLContextIsValid(psPwLContext) == PATHWELL_TRUE);

  /*-
   *********************************************************************
   *
   * Confirm that both member contexts are valid.
   *
   *********************************************************************
   */
  psPwTContextA = PwLGetPwTContextA(psPwLContext);
  psPwTContextB = PwLGetPwTContextB(psPwLContext);
  if ((PwTContextIsValid(psPwTContextA) == PATHWELL_FALSE) || (PwTContextIsValid(psPwTContextB) == PATHWELL_FALSE))
  {
    PwLSetError(psPwLContext, "Invalid member context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwTContextIsValid(psPwTContextA) == PATHWELL_TRUE && PwTContextIsValid(psPwTContextB) == PATHWELL_TRUE);

  /*-
   *********************************************************************
   *
   * Confirm that both member toplogies are defined.
   *
   *********************************************************************
   */
  pcTopologyA = PwTGetTopology(psPwLContext->psPwTContextA);
  if (pcTopologyA == NULL)
  {
    PwLSetError(psPwLContext, "Topology A is not defined. That should not happen.", NULL);
    return ER;
  }
  pcTopologyB = PwTGetTopology(psPwLContext->psPwTContextB);
  if (pcTopologyB == NULL)
  {
    PwLSetError(psPwLContext, "Topology B is not defined. That should not happen.", NULL);
    return ER;
  }

  /*-
   *********************************************************************
   *
   * Convert both topologies into a compact topology strings (no '?'s).
   *
   *********************************************************************
   */
  iLengthA = strlen(pcTopologyA);
  iLengthB = strlen(pcTopologyB);
  if (iLengthA >= PATHWELL_MAX_TOPOLOGY_SIZE)
  {
    PwLSetError(psPwLContext, "Topology A exceeds maximum allowed length. That should not happen.", NULL);
    return ER;
  }
  if (iLengthB >= PATHWELL_MAX_TOPOLOGY_SIZE)
  {
    PwLSetError(psPwLContext, "Topology B exceeds maximum allowed length. That should not happen.", NULL);
    return ER;
  }
  if (strcmp(pcTopologyA, "?z") == 0)
  {
    acStringA[0] = 0;
  }
  else
  {
    for (iIndex = 0; iIndex < strlen(pcTopologyA) / 2; iIndex++)
    {
      acStringA[iIndex] = pcTopologyA[(iIndex * 2) + 1];
    }
    acStringA[iIndex] = 0;
  }
  if (strcmp(pcTopologyB, "?z") == 0)
  {
    acStringB[0] = 0;
  }
  else
  {
    for (iIndex = 0; iIndex < strlen(pcTopologyB) / 2; iIndex++)
    {
      acStringB[iIndex] = pcTopologyB[(iIndex * 2) + 1];
    }
    acStringB[iIndex] = 0;
  }

  /*-
   *********************************************************************
   *
   * Compute and return the Levenshtein distance.
   *
   *********************************************************************
   */
  iDistance = PwSLevenshteinDistance(acStringA, acStringB);

  return iDistance;
}


/*-
 ***********************************************************************
 *
 * PwLCheckLevDistance
 *
 ***********************************************************************
 */
int
PwLCheckLevDistance(PW_L_CONTEXT *psPwLContext, int iMinDistance)
{
  int iActualDistance = PwLCalcLevDistance(psPwLContext);

  if (iActualDistance == ER)
  {
    return PATHWELL_INDETERMINATE;
  }
  else
  {
    return (iActualDistance >= iMinDistance) ? PATHWELL_TRUE : PATHWELL_FALSE;
  }
}

/*-
 ***********************************************************************
 *
 * PwLClearError
 *
 ***********************************************************************
 */
void
PwLClearError(PW_L_CONTEXT *psPwLContext)
{
  if (psPwLContext != NULL)
  {
    if
    (
         psPwLContext->pcError != NULL
      && psPwLContext->pcError != gpcContextAllocationError
      && psPwLContext->pcError != gpcRealErrorLostNullFormat
      && psPwLContext->pcError != gpcRealErrorLostNullMalloc
    )
    {
      free(psPwLContext->pcError);
      psPwLContext->pcError = NULL;
    }
  }
  return;
}


/*-
 ***********************************************************************
 *
 * PwLContextIsValid
 *
 ***********************************************************************
 */
int
PwLContextIsValid(PW_L_CONTEXT *psPwLContext)
{
  return (psPwLContext != NULL && psPwLContext->iValid == PATHWELL_TRUE) ? PATHWELL_TRUE : PATHWELL_FALSE;
}


/*-
 ***********************************************************************
 *
 * PwLFreeContext
 *
 ***********************************************************************
 */
void
PwLFreeContext(PW_L_CONTEXT *psPwLContext)
{
  if (psPwLContext != NULL)
  {
    if
    (
         psPwLContext->pcError != NULL
      && psPwLContext->pcError != gpcContextAllocationError
      && psPwLContext->pcError != gpcRealErrorLostNullFormat
      && psPwLContext->pcError != gpcRealErrorLostNullMalloc
    )
    {
      free(psPwLContext->pcError);
    }
    free(psPwLContext);
    psPwLContext = NULL;
  }
  return;
}


/*-
 ***********************************************************************
 *
 * PwLGetError
 *
 ***********************************************************************
 */
const char *
PwLGetError(PW_L_CONTEXT *psPwLContext)
{
  if (psPwLContext == NULL)
  {
    return gpcContextAllocationError;
  }

  return psPwLContext->pcError;
}


/*-
 ***********************************************************************
 *
 * PwLGetPwTContext
 *
 ***********************************************************************
 */
PW_T_CONTEXT *
PwLGetPwTContext(PW_L_CONTEXT *psPwLContext, char cMember)
{
  /*-
   *********************************************************************
   *
   * A valid context is required.
   *
   *********************************************************************
   */
  if (PwLContextIsValid(psPwLContext) == PATHWELL_FALSE)
  {
    PwLSetError(psPwLContext, "Invalid context. That should not happen.", NULL);
    return NULL;
  }
  PATHWELL_ASSERT(PwLContextIsValid(psPwLContext) == PATHWELL_TRUE);

  /*-
   *********************************************************************
   *
   * Get the appropriate member, and return it to the caller.
   *
   *********************************************************************
   */
  switch (cMember)
  {
  case 'A':
  case 'a':
    return (PW_T_CONTEXT *)psPwLContext->psPwTContextA;
    break;
  case 'B':
  case 'b':
    return (PW_T_CONTEXT *)psPwLContext->psPwTContextB;
    break;
  default:
    /* Empty */
    break;
  }

  return NULL;
}


/*-
 ***********************************************************************
 *
 * PwLGetPwTContextA
 *
 ***********************************************************************
 */
PW_T_CONTEXT *
PwLGetPwTContextA(PW_L_CONTEXT *psPwLContext)
{
  return PwLGetPwTContext(psPwLContext, 'A');
}


/*-
 ***********************************************************************
 *
 * PwLGetPwTContextB
 *
 ***********************************************************************
 */
PW_T_CONTEXT *
PwLGetPwTContextB(PW_L_CONTEXT *psPwLContext)
{
  return PwLGetPwTContext(psPwLContext, 'B');
}


/*-
 ***********************************************************************
 *
 * PwLNewContextFromParameters
 *
 ***********************************************************************
 */
PW_L_CONTEXT *
PwLNewContextFromParameters(PW_T_CONTEXT *psPwTContextA, PW_T_CONTEXT *psPwTContextB)
{
  PW_L_CONTEXT *psPwLContext = NULL;

  /*-
   *********************************************************************
   *
   * Allocate and initialize memory for a new context structure.
   *
   *********************************************************************
   */
  psPwLContext = calloc(1, sizeof(PW_L_CONTEXT));
  if (psPwLContext == NULL)
  {
    return NULL;
  }

  /*-
   *********************************************************************
   *
   * Make sure the caller has provided valid member contexts.
   *
   *********************************************************************
   */
  if (!PwTContextIsValid(psPwTContextA))
  {
    PwLSetError(psPwLContext, "Invalid member context (A).", NULL);
    return psPwLContext;
  }
  if (!PwTContextIsValid(psPwTContextB))
  {
    PwLSetError(psPwLContext, "Invalid member context (B).", NULL);
    return psPwLContext;
  }
  psPwLContext->psPwTContextA = psPwTContextA;
  psPwLContext->psPwTContextB = psPwTContextB;

  /*-
   *********************************************************************
   *
   * If we made it this far, the context is considered valid.
   *
   *********************************************************************
   */
  psPwLContext->iValid = PATHWELL_TRUE;

  return psPwLContext;
}


/*-
 ***********************************************************************
 *
 * PwLSetPwTContext
 *
 ***********************************************************************
 */
int
PwLSetPwTContext(PW_L_CONTEXT *psPwLContext, PW_T_CONTEXT *psPwTContext, char cMember)
{
  /*-
   *********************************************************************
   *
   * A valid context is required.
   *
   *********************************************************************
   */
  if (PwLContextIsValid(psPwLContext) == PATHWELL_FALSE)
  {
    PwLSetError(psPwLContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwLContextIsValid(psPwLContext) == PATHWELL_TRUE);

  /*-
   *********************************************************************
   *
   * Set the appropriate member, and update the context's validity.
   *
   *********************************************************************
   */
  switch (cMember)
  {
  case 'A':
  case 'a':
    psPwLContext->psPwTContextA = psPwTContext;
    break;
  case 'B':
  case 'b':
    psPwLContext->psPwTContextB = psPwTContext;
    break;
  default:
    /* Empty */
    break;
  }
  psPwLContext->iValid = (psPwLContext->psPwTContextA != NULL && psPwLContext->psPwTContextB != NULL) ? PATHWELL_TRUE : PATHWELL_FALSE;

  return ER_OK;
}


/*-
 ***********************************************************************
 *
 * PwLSetPwTContextA
 *
 ***********************************************************************
 */
int
PwLSetPwTContextA(PW_L_CONTEXT *psPwLContext, PW_T_CONTEXT *psPwTContext)
{
  return PwLSetPwTContext(psPwLContext, psPwTContext, 'A');
}


/*-
 ***********************************************************************
 *
 * PwLSetPwTContextB
 *
 ***********************************************************************
 */
int
PwLSetPwTContextB(PW_L_CONTEXT *psPwLContext, PW_T_CONTEXT *psPwTContext)
{
  return PwLSetPwTContext(psPwLContext, psPwTContext, 'B');
}


/*-
 **********************************************************************
 *
 * PwTBaseNp1IdToTopology
 *
 **********************************************************************
 */
int
PwTBaseNp1IdToTopology(PW_T_CONTEXT *psPwTContext)
{
  char cTemp = 0;
  char *pcTokens = NULL;
  char *pcTopology = NULL;
  int i = 0;
  int iBaseNp1 = 0;
  int iTopologySize = 0;
  int64_t i64Q = 0;
  int64_t i64R = 0;
  int64_t i64Value = 0;

  /*-
   *********************************************************************
   *
   * A valid context is required.
   *
   *********************************************************************
   */
  if (PwTContextIsValid(psPwTContext) == PATHWELL_FALSE)
  {
    PwTSetError(psPwTContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwTContextIsValid(psPwTContext) == PATHWELL_TRUE);

  /*-
   *********************************************************************
   *
   * A valid token set is required.
   *
   *********************************************************************
   */
  switch (PwTGetTokenSet(psPwTContext))
  {
  case PATHWELL_TOKEN_SET_ID_1:
  case PATHWELL_TOKEN_SET_ID_2:
  case PATHWELL_TOKEN_SET_ID_3:
  case PATHWELL_TOKEN_SET_ID_4:
    break;
  default:
    PwTSetError(psPwTContext, "Invalid or unsupported token set.", NULL);
    return ER;
    break;
  }

  /*-
   ********************************************************************
   *
   * Handle the only special case (i.e., ?z).
   *
   ********************************************************************
   */
  if (PwTGetId(psPwTContext) == 0)
  {
    PwTSetTopology(psPwTContext, "?z");
    return ER_OK;
  }

  /*-
   ********************************************************************
   *
   * Reject IDs that fall outside the allowed range.
   *
   ********************************************************************
   */
  if (PwTGetId(psPwTContext) < PATHWELL_MIN_BASENP1_ID || PwTGetId(psPwTContext) > PATHWELL_MAX_BASENP1_ID)
  {
    PwTSetError(psPwTContext, "Topology ID falls outside the allowed range.", NULL);
    return ER;
  }

  /*
   ********************************************************************
   *
   * Attempt to perform the conversion.
   *
   ********************************************************************
   */
  pcTokens = PwTGetTokenSetTokens(psPwTContext);
  if (pcTokens == NULL)
  {
    return ER; // Note: Error message already set.
  }
  iBaseNp1 = strlen(pcTokens) + 1;
  i64Value = PwTGetId(psPwTContext);
  do
  {
    i64Q = i64Value / iBaseNp1;
    i64R = i64Value % iBaseNp1;
    if (i64Q > 0 && i64R == 0)
    {
      PwTSetError(psPwTContext, "Invalid baseN+1 topology ID.", NULL);
      return ER;
    }
    else
    {
      if (pcTopology == NULL)
      {
        iTopologySize = 2;
      }
      else
      {
        iTopologySize += 2;
      }
      pcTopology = (char *)realloc(pcTopology, iTopologySize + 1);
      if (pcTopology == NULL)
      {
        PwTSetError(psPwTContext, "Failed to allocate memory (%s).", strerror(errno), NULL);
        return ER;
      }
      PATHWELL_ASSERT(pcTopology != NULL);
      pcTopology[iTopologySize-2] = '?';
      switch (i64R)
      {
      case 1:
        pcTopology[iTopologySize-1] = 'd';
        break;
      case 2:
        pcTopology[iTopologySize-1] = 'l';
        break;
      case 3:
        pcTopology[iTopologySize-1] = 'u';
        break;
      case 4:
        pcTopology[iTopologySize-1] = 's';
        break;
      case 5:
        pcTopology[iTopologySize-1] = 'f';
        break;
      case 6:
        pcTopology[iTopologySize-1] = 'x';
        break;
      case 7:
        pcTopology[iTopologySize-1] = 'H';
        break;
      default:
        PwTSetError(psPwTContext, "Invalid token. That should not happen.", NULL);
        return ER;
        break;
      }
      pcTopology[iTopologySize] = '\0';
    }
    i64Value = i64Q;
  } while (i64Q > 0);

  for (i = 0; i < iTopologySize/4; i++)
  {
    cTemp = pcTopology[(2*i) + 1];
    pcTopology[(2*i) + 1] = pcTopology[iTopologySize-(2*i)-1];
    pcTopology[iTopologySize-(2*i)-1] = cTemp;
  }

  PwTSetTopology(psPwTContext, pcTopology);

  return ER_OK;
}


/*-
 **********************************************************************
 *
 * PwTBitmaskIdToTopology
 *
 **********************************************************************
 */
int
PwTBitmaskIdToTopology(PW_T_CONTEXT *psPwTContext)
{
  char *pcTopology = NULL;
  int i = 0;
  int iBitsPerToken = 0;
  int iNumShifts = 0;
  int iTemp = 0;
  int iTopologySize = 0;
  int64_t i64Id = 0;
  uint64_t ui64BinaryTokens = 0;
  uint64_t ui64Temp = 0;
  unsigned int uiTopologyLength = 0;

  /*-
   *********************************************************************
   *
   * A valid context is required.
   *
   *********************************************************************
   */
  if (PwTContextIsValid(psPwTContext) == PATHWELL_FALSE)
  {
    PwTSetError(psPwTContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwTContextIsValid(psPwTContext) == PATHWELL_TRUE);

  /*-
   *********************************************************************
   *
   * A valid token set is required.
   *
   *********************************************************************
   */
  switch (PwTGetTokenSet(psPwTContext))
  {
  case PATHWELL_TOKEN_SET_ID_1:
    iBitsPerToken = 2;
    break;
  case PATHWELL_TOKEN_SET_ID_2:
  case PATHWELL_TOKEN_SET_ID_3:
  case PATHWELL_TOKEN_SET_ID_4:
    iBitsPerToken = 3;
    break;
  default:
    PwTSetError(psPwTContext, "Invalid or unsupported token set.", NULL);
    return ER;
    break;
  }

  /*-
   ********************************************************************
   *
   * Handle the only special case (i.e., ?z).
   *
   ********************************************************************
   */
  if (PwTGetId(psPwTContext) == 0)
  {
    PwTSetTopology(psPwTContext, "?z");
    return ER_OK;
  }

  /*-
   ********************************************************************
   *
   * Reject IDs that fall outside the allowed range.
   *
   ********************************************************************
   */
  if (PwTGetId(psPwTContext) < PATHWELL_MIN_BITMASK_ID || PwTGetId(psPwTContext) > PATHWELL_MAX_BITMASK_ID)
  {
    PwTSetError(psPwTContext, "Topology ID falls outside the allowed range.", NULL);
    return ER;
  }

  /*
   ********************************************************************
   *
   * Read the declared length of the topology.
   *
   ********************************************************************
   */
  i64Id = PwTGetId(psPwTContext);
  for (iNumShifts = 0; iNumShifts < 63-6; iNumShifts++)
  {
    if ((((uint64_t)(i64Id << iNumShifts) >> 63) & 0x1) == 1) // Found the high bit.
    {
      uiTopologyLength = ((uint64_t)i64Id >> (63 - 6 - iNumShifts + 1)) & 0b11111; // Read the length of the topology from the fields index [1-5].
      break;
    }
  }

  /*-
   ********************************************************************
   *
   * Check to make sure the declared topology length matches the
   * actual topology length.
   *
   ********************************************************************
   */
  if (63-5-iNumShifts != uiTopologyLength*iBitsPerToken)
  {
    PwTSetError(psPwTContext, "Invalid topology ID.", NULL);
    return ER;
  }

  /*-
   ********************************************************************
   *
   * Perform the bulk of the conversion computation.
   *
   ********************************************************************
   */
  ui64BinaryTokens = ((uint64_t)i64Id << (iNumShifts + 1 + 5)) >> (iNumShifts + 1 + 5);
  pcTopology = (char *)realloc(pcTopology, sizeof(char)*(2*uiTopologyLength)+1);
  for (i = 0; i < uiTopologyLength; i++)
  {
    ui64Temp = ui64BinaryTokens >> (uiTopologyLength - i - 1) * iBitsPerToken;
    pcTopology[2*i] = '?';
    if (iBitsPerToken == 2)
    {
      ui64Temp = ui64Temp & 3;
      switch (ui64Temp)
      {
      case 0:
        pcTopology[2*i+1] = 'd';
        break;
      case 1:
        pcTopology[2*i+1] = 'l';
        break;
      case 2:
        pcTopology[2*i+1] = 'u';
        break;
      case 3:
        pcTopology[2*i+1] = 's';
        break;
      default:
        PwTSetError(psPwTContext, "Invalid token. That should not happen.", NULL);
        return ER;
        break;
      }
    }
    else
    {
      ui64Temp = ui64Temp & 7;
      switch (ui64Temp)
      {
      case 0:
        pcTopology[2*i+1] = 'd';
        break;
      case 1:
        pcTopology[2*i+1] = 'l';
        break;
      case 2:
        pcTopology[2*i+1] = 'u';
        break;
      case 3:
        pcTopology[2*i+1] = 's';
        break;
      case 4:
        pcTopology[2*i+1] = 'f';
        break;
      case 5:
        pcTopology[2*i+1] = 'x';
        break;
      case 6:
        pcTopology[2*i+1] = 'H';
        break;
      default:
        PwTSetError(psPwTContext, "Invalid token. That should not happen.", NULL);
        return ER;
        break;
      }
    }
  }
  pcTopology[2*uiTopologyLength] = '\0';
  PwTSetTopology(psPwTContext, pcTopology);

  return ER_OK;
}


/*-
 ***********************************************************************
 *
 * PwTClearError
 *
 ***********************************************************************
 */
void
PwTClearError(PW_T_CONTEXT *psPwTContext)
{
  if (psPwTContext != NULL)
  {
    if
    (
         psPwTContext->pcError != NULL
      && psPwTContext->pcError != gpcContextAllocationError
      && psPwTContext->pcError != gpcRealErrorLostNullFormat
      && psPwTContext->pcError != gpcRealErrorLostNullMalloc
    )
    {
      free(psPwTContext->pcError);
      psPwTContext->pcError = NULL;
    }
  }
  return;
}


/*-
 ***********************************************************************
 *
 * PwTContextIsValid
 *
 ***********************************************************************
 */
int
PwTContextIsValid(PW_T_CONTEXT *psPwTContext)
{
  return (psPwTContext != NULL && psPwTContext->iValid == PATHWELL_TRUE) ? PATHWELL_TRUE : PATHWELL_FALSE;
}


/*-
 ***********************************************************************
 *
 * PwTFreeContext
 *
 ***********************************************************************
 */
void
PwTFreeContext(PW_T_CONTEXT *psPwTContext)
{
  if (psPwTContext != NULL)
  {
    if
    (
         psPwTContext->pcError != NULL
      && psPwTContext->pcError != gpcContextAllocationError
      && psPwTContext->pcError != gpcRealErrorLostNullFormat
      && psPwTContext->pcError != gpcRealErrorLostNullMalloc
    )
    {
      free(psPwTContext->pcError);
    }
    if (psPwTContext->pcPassword != NULL)
    {
      free(psPwTContext->pcPassword);
    }
    if (psPwTContext->pcTopology != NULL)
    {
      free(psPwTContext->pcTopology);
    }
    if (psPwTContext->pi64Id != NULL)
    {
      free(psPwTContext->pi64Id);
    }
    free(psPwTContext);
    psPwTContext = NULL;
  }

  return;
}


/*-
 ***********************************************************************
 *
 * PwTGetEncoding
 *
 ***********************************************************************
 */
int
PwTGetEncoding(PW_T_CONTEXT *psPwTContext)
{
  if (PwTContextIsValid(psPwTContext) == PATHWELL_FALSE)
  {
    PwTSetError(psPwTContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwTContextIsValid(psPwTContext) == PATHWELL_TRUE);

  return psPwTContext->iEncoding;
}


/*-
 ***********************************************************************
 *
 * PwTGetError
 *
 ***********************************************************************
 */
const char *
PwTGetError(PW_T_CONTEXT *psPwTContext)
{
  if (psPwTContext == NULL)
  {
    return gpcContextAllocationError;
  }

  return psPwTContext->pcError;
}


/*-
 ***********************************************************************
 *
 * PwTGetId
 *
 ***********************************************************************
 */
int64_t
PwTGetId(PW_T_CONTEXT *psPwTContext)
{
  if (PwTContextIsValid(psPwTContext) == PATHWELL_FALSE)
  {
    PwTSetError(psPwTContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwTContextIsValid(psPwTContext) == PATHWELL_TRUE);

  return (psPwTContext->pi64Id != NULL) ? *psPwTContext->pi64Id : -1;
}


/*-
 ***********************************************************************
 *
 * PwTGetPassword
 *
 ***********************************************************************
 */
const char *
PwTGetPassword(PW_T_CONTEXT *psPwTContext)
{
  if (PwTContextIsValid(psPwTContext) == PATHWELL_FALSE)
  {
    PwTSetError(psPwTContext, "Invalid context. That should not happen.", NULL);
    return NULL;
  }
  PATHWELL_ASSERT(PwTContextIsValid(psPwTContext) == PATHWELL_TRUE);

  return psPwTContext->pcPassword;
}


/*-
 ***********************************************************************
 *
 * PwTGetTokenSet
 *
 ***********************************************************************
 */
int
PwTGetTokenSet(PW_T_CONTEXT *psPwTContext)
{
  if (PwTContextIsValid(psPwTContext) == PATHWELL_FALSE)
  {
    PwTSetError(psPwTContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwTContextIsValid(psPwTContext) == PATHWELL_TRUE);

  return psPwTContext->iTokenSet;
}


/*-
 ***********************************************************************
 *
 * PwTGetTokenSetTokens
 *
 ***********************************************************************
 */
char *
PwTGetTokenSetTokens(PW_T_CONTEXT *psPwTContext)
{
  char *pcTokens = NULL;

  if (PwTContextIsValid(psPwTContext) == PATHWELL_FALSE)
  {
    PwTSetError(psPwTContext, "Invalid context. That should not happen.", NULL);
    return NULL;
  }
  PATHWELL_ASSERT(PwTContextIsValid(psPwTContext) == PATHWELL_TRUE);

  switch (psPwTContext->iTokenSet)
  {
  case PATHWELL_TOKEN_SET_ID_1:
    pcTokens = PATHWELL_TOKEN_SET_ID_1_TOKENS;
    break;
  case PATHWELL_TOKEN_SET_ID_2:
    pcTokens = PATHWELL_TOKEN_SET_ID_2_TOKENS;
    break;
  case PATHWELL_TOKEN_SET_ID_3:
    pcTokens = PATHWELL_TOKEN_SET_ID_3_TOKENS;
    break;
  case PATHWELL_TOKEN_SET_ID_4:
    pcTokens = PATHWELL_TOKEN_SET_ID_4_TOKENS;
    break;
  default:
    PwTSetError(psPwTContext, "Invalid or unsupported token set.", NULL);
    return NULL;
    break;
  }

  return pcTokens;
}


/*-
 ***********************************************************************
 *
 * PwTGetTopology
 *
 ***********************************************************************
 */
const char *
PwTGetTopology(PW_T_CONTEXT *psPwTContext)
{
  if (PwTContextIsValid(psPwTContext) == PATHWELL_FALSE)
  {
    PwTSetError(psPwTContext, "Invalid context. That should not happen.", NULL);
    return NULL;
  }
  PATHWELL_ASSERT(PwTContextIsValid(psPwTContext) == PATHWELL_TRUE);

  return psPwTContext->pcTopology;
}


/*-
 ***********************************************************************
 *
 * PwTIdToTopology
 *
 ***********************************************************************
 */
int
PwTIdToTopology(PW_T_CONTEXT *psPwTContext)
{
  /*-
   *********************************************************************
   *
   * A valid context is required.
   *
   *********************************************************************
   */
  if (PwTContextIsValid(psPwTContext) == PATHWELL_FALSE)
  {
    PwTSetError(psPwTContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwTContextIsValid(psPwTContext) == PATHWELL_TRUE);

  return (PwTGetEncoding(psPwTContext) == PATHWELL_ENCODING_BITMASK) ? PwTBitmaskIdToTopology(psPwTContext) : PwTBaseNp1IdToTopology(psPwTContext);
}


/*-
 ***********************************************************************
 *
 * PwTIsPasswordInTokenSet
 *
 ***********************************************************************
 */
int
PwTIsPasswordInTokenSet(PW_T_CONTEXT *psPwTContext)
{
  const char *pcPassword = NULL;
  int iIndex = 0;
  unsigned char *pucTokenSet = NULL;

  /*-
   *********************************************************************
   *
   * A valid context is required.
   *
   *********************************************************************
   */
  if (PwTContextIsValid(psPwTContext) == PATHWELL_FALSE)
  {
    PwTSetError(psPwTContext, "Invalid context. That should not happen.", NULL);
    return PATHWELL_INDETERMINATE;
  }
  PATHWELL_ASSERT(PwTContextIsValid(psPwTContext) == PATHWELL_TRUE);

  /*-
   *********************************************************************
   *
   * A valid token set is required.
   *
   *********************************************************************
   */
  switch (PwTGetTokenSet(psPwTContext))
  {
  case PATHWELL_TOKEN_SET_ID_1:
    pucTokenSet = gaucTokenSet1;
    break;
  case PATHWELL_TOKEN_SET_ID_2:
    pucTokenSet = gaucTokenSet2;
    break;
  case PATHWELL_TOKEN_SET_ID_3:
    pucTokenSet = gaucTokenSet3;
    break;
  case PATHWELL_TOKEN_SET_ID_4:
    pucTokenSet = gaucTokenSet4;
    break;
  default:
    PwTSetError(psPwTContext, "Invalid or unsupported token set.", NULL);
    return PATHWELL_INDETERMINATE;
    break;
  }

  /*-
   *********************************************************************
   *
   * A valid password is required.
   *
   *********************************************************************
   */
  pcPassword = PwTGetPassword(psPwTContext);
  if (pcPassword == NULL)
  {
    PwTSetError(psPwTContext, "Password undefined.", NULL);
    return PATHWELL_INDETERMINATE;
  }

  /*-
   *********************************************************************
   *
   * Verify that each character is covered by the specified token set.
   *
   *********************************************************************
   */
  for (iIndex = 0; iIndex < strlen(pcPassword); iIndex++)
  {
    if ((unsigned char)pcPassword[iIndex] > 255 || !pucTokenSet[(unsigned char)pcPassword[iIndex]])
    {
      return PATHWELL_FALSE;
    }
  }

  return PATHWELL_TRUE;
}


/*-
 ***********************************************************************
 *
 * PwTNewContext
 *
 ***********************************************************************
 */
PW_T_CONTEXT *
PwTNewContext(void)
{
  PW_T_CONTEXT *psPwTContext = NULL;

  psPwTContext = calloc(1, sizeof(PW_T_CONTEXT));
  if (psPwTContext == NULL)
  {
    return NULL;
  }
  psPwTContext->pcError = NULL;
  psPwTContext->pcPassword = NULL;
  psPwTContext->pcTopology = NULL;
  psPwTContext->iEncoding = PATHWELL_DEFAULT_ENCODING;
  psPwTContext->iTokenSet = PATHWELL_DEFAULT_TOKEN_SET_ID;
  psPwTContext->pi64Id = NULL;
  psPwTContext->iValid = PATHWELL_TRUE;

  return psPwTContext;
}


/*-
 ***********************************************************************
 *
 * PwTNewContextFromParameters
 *
 ***********************************************************************
 */
PW_T_CONTEXT *
PwTNewContextFromParameters(int iEncoding, int iTokenSet, char *pcPassword, char *pcTopology, int64_t *pi64Id)
{
  int iSize = 0;
  PW_T_CONTEXT *psPwTContext = NULL;

  /*-
   *********************************************************************
   *
   * Allocate and initialize memory for a new context structure.
   *
   *********************************************************************
   */
  psPwTContext = calloc(1, sizeof(PW_T_CONTEXT));
  if (psPwTContext == NULL)
  {
    return NULL;
  }

  /*-
   *********************************************************************
   *
   * Make sure the specified encoding scheme is valid/supported.
   *
   *********************************************************************
   */
  switch (iEncoding)
  {
  case PATHWELL_ENCODING_BASENP1:
  case PATHWELL_ENCODING_BITMASK:
    psPwTContext->iEncoding = iEncoding;
    break;
  default:
    PwTSetError(psPwTContext, "Invalid or unsupported encoding scheme.", NULL);
    return psPwTContext;
    break;
  }

  /*-
   *********************************************************************
   *
   * Make sure the specified token set is valid/supported.
   *
   *********************************************************************
   */
  switch (iTokenSet)
  {
  case PATHWELL_TOKEN_SET_ID_1:
  case PATHWELL_TOKEN_SET_ID_2:
  case PATHWELL_TOKEN_SET_ID_3:
  case PATHWELL_TOKEN_SET_ID_4:
    psPwTContext->iTokenSet = iTokenSet;
    break;
  default:
    PwTSetError(psPwTContext, "Invalid or unsupported token set.", NULL);
    return psPwTContext;
    break;
  }

  /*-
   *********************************************************************
   *
   * Make sure the caller has provided a password, toplogy, or ID.
   *
   *********************************************************************
   */
  if (pcPassword == NULL && pcTopology == NULL && pi64Id == NULL)
  {
    PwTSetError(psPwTContext, "At least one of the following parameters must be specified: password, topology, or ID.", NULL);
    return psPwTContext;
  }

  /*-
   *********************************************************************
   *
   * Conditionally set the password.
   *
   *********************************************************************
   */
  if (pcPassword != NULL)
  {
    iSize = strlen(pcPassword) + 1;
    psPwTContext->pcPassword = (char *)malloc(sizeof(char) * iSize);
    if (psPwTContext->pcPassword == NULL)
    {
      PwTSetError(psPwTContext, "Failed to allocate memory (%s).", strerror(errno), NULL);
      return psPwTContext;
    }
    strncpy(psPwTContext->pcPassword, pcPassword, iSize);
  }

  /*-
   *********************************************************************
   *
   * Conditionally set the topology.
   *
   *********************************************************************
   */
  if (pcTopology != NULL)
  {
    iSize = strlen(pcTopology) + 1;
    psPwTContext->pcTopology = (char *)malloc(sizeof(char) * iSize);
    if (psPwTContext->pcTopology == NULL)
    {
      PwTSetError(psPwTContext, "Failed to allocate memory (%s).", strerror(errno), NULL);
      return psPwTContext;
    }
    strncpy(psPwTContext->pcTopology, pcTopology, iSize);
  }

  /*-
   *********************************************************************
   *
   * Conditionally set the ID.
   *
   *********************************************************************
   */
  if (pi64Id != NULL)
  {
    if (*pi64Id < 0)
    {
      PwTSetError(psPwTContext, "ID values must be greater than or equal to zero.", NULL);
      return psPwTContext;
    }
    iSize = 1;
    psPwTContext->pi64Id = (int64_t *)malloc(sizeof(int64_t) * iSize);
    if (psPwTContext->pi64Id == NULL)
    {
      PwTSetError(psPwTContext, "Failed to allocate memory (%s).", strerror(errno), NULL);
      return psPwTContext;
    }
    *psPwTContext->pi64Id = *pi64Id;
  }

  /*-
   *********************************************************************
   *
   * If we made it this far, the context is considered valid.
   *
   *********************************************************************
   */
  psPwTContext->iValid = PATHWELL_TRUE;

  return psPwTContext;
}


/*-
 ***********************************************************************
 *
 * PwTPasswordToTopology
 *
 ***********************************************************************
 */
int
PwTPasswordToTopology(PW_T_CONTEXT *psPwTContext)
{
  char *pc = NULL;
  char *pcTopology = NULL;
  int iIndex = 0;
  int iLength = 0;
  int iSize = 0;
  int iStatus = 0;

  /*-
   *********************************************************************
   *
   * A valid context is required.
   *
   *********************************************************************
   */
  if (PwTContextIsValid(psPwTContext) == PATHWELL_FALSE)
  {
    PwTSetError(psPwTContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwTContextIsValid(psPwTContext) == PATHWELL_TRUE);

  /*-
   *********************************************************************
   *
   * A valid password is required.
   *
   *********************************************************************
   */
  iStatus = PwTIsPasswordInTokenSet(psPwTContext);
  if (iStatus != PATHWELL_TRUE)
  {
    if (iStatus == PATHWELL_INDETERMINATE)
    {
      return ER; // Note: Error message already set.
    }
    PwTSetError(psPwTContext, "One or more password characters are not in the specified token set.", NULL);
    return ER;
  }

  /*-
   *********************************************************************
   *
   * Attempt to peform the conversion, but only for passwords that are
   * comprised entirely of characters from the specified token set. If
   * the password is an empty string, the corresponding toplogy should
   * be "?z" as this is a special case.
   *
   *********************************************************************
   */
  iLength = strlen(psPwTContext->pcPassword);
  iSize = ((iLength == 0) ? 2 : iLength * 2) + 1;
  pcTopology = pc = (char *)realloc(psPwTContext->pcTopology, (sizeof(char) * iSize));
  if (pcTopology == NULL)
  {
    PwTSetError(psPwTContext, "Failed to allocate memory (%s).", strerror(errno), NULL);
    return ER;
  }
  for (iIndex = 0, pc = pcTopology; iIndex < iLength; iIndex++)
  {
    *pc++ = '?';
    if (gaucCharacterSet1[(unsigned char)psPwTContext->pcPassword[iIndex]])
    {
      *pc++ = 'd';
    }
    else if (gaucCharacterSet2[(unsigned char)psPwTContext->pcPassword[iIndex]])
    {
      *pc++ = 'l';
    }
    else if (gaucCharacterSet3[(unsigned char)psPwTContext->pcPassword[iIndex]])
    {
      *pc++ = 'u';
    }
    else if (gaucCharacterSet4[(unsigned char)psPwTContext->pcPassword[iIndex]])
    {
      *pc++ = 's';
    }
    else if (gaucCharacterSet5[(unsigned char)psPwTContext->pcPassword[iIndex]])
    {
      *pc++ = 'f';
    }
    else if (gaucCharacterSet6[(unsigned char)psPwTContext->pcPassword[iIndex]])
    {
      *pc++ = 'x';
    }
    else if (gaucCharacterSet7[(unsigned char)psPwTContext->pcPassword[iIndex]])
    {
      *pc++ = 'H';
    }
    else
    {
      PwTSetError(psPwTContext, "Invalid or unsupported password character. That should not happen.", NULL);
      return ER;
    }
  }
  if (iLength == 0) // Handle the special case where the password was an empty string.
  {
    *pc++ = '?';
    *pc++ = 'z';
  }
  *pc = '\0';
  psPwTContext->pcTopology = pcTopology;

  return ER_OK;
}


/*-
 ***********************************************************************
 *
 * PwTSetEncoding
 *
 ***********************************************************************
 */
int
PwTSetEncoding(PW_T_CONTEXT *psPwTContext, int iEncoding)
{
  if (PwTContextIsValid(psPwTContext) == PATHWELL_FALSE)
  {
    PwTSetError(psPwTContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwTContextIsValid(psPwTContext) == PATHWELL_TRUE);

  psPwTContext->iEncoding = iEncoding;
  return ER_OK;
}


/*-
 ***********************************************************************
 *
 * PwTSetId
 *
 ***********************************************************************
 */
int
PwTSetId(PW_T_CONTEXT *psPwTContext, int64_t i64Id)
{
  int iSize = 0;

  if (PwTContextIsValid(psPwTContext) == PATHWELL_FALSE)
  {
    PwTSetError(psPwTContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwTContextIsValid(psPwTContext) == PATHWELL_TRUE);

  if (psPwTContext->pi64Id == NULL)
  {
    iSize = 1;
    psPwTContext->pi64Id = (int64_t *)malloc(sizeof(int64_t) * iSize);
    if (psPwTContext->pi64Id == NULL)
    {
      PwTSetError(psPwTContext, "Failed to allocate memory (%s).", strerror(errno), NULL);
      return ER;
    }
  }
  *psPwTContext->pi64Id = i64Id;

  return ER_OK;
}


/*-
 ***********************************************************************
 *
 * PwTSetIdFromString
 *
 ***********************************************************************
 */
int
PwTSetIdFromString(PW_T_CONTEXT *psPwTContext, char *pcId)
{
  int iError = 0;
  int64_t i64Id = 0;

  if (PwTContextIsValid(psPwTContext) == PATHWELL_FALSE)
  {
    PwTSetError(psPwTContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwTContextIsValid(psPwTContext) == PATHWELL_TRUE);

  iError = PwSBaseNTo63Bit(pcId, &i64Id, 10);
  if (iError != 0)
  {
    PwTSetError(psPwTContext, "ID %s a 63-bit integer.", (iError < 0) ? "could not be converted to" : "is too large to be represented as", NULL);
    return ER;
  }

  return PwTSetId(psPwTContext, i64Id);
}


/*-
 ***********************************************************************
 *
 * PwTSetPassword
 *
 ***********************************************************************
 */
int
PwTSetPassword(PW_T_CONTEXT *psPwTContext, char *pcPassword)
{
  char *pcTempPassword = NULL;
  int iSize = 0;

  if (PwTContextIsValid(psPwTContext) == PATHWELL_FALSE)
  {
    PwTSetError(psPwTContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwTContextIsValid(psPwTContext) == PATHWELL_TRUE);

  if (pcPassword == NULL)
  {
    if (psPwTContext->pcPassword != NULL)
    {
      free(psPwTContext->pcPassword);
    }
  }
  else
  {
    iSize = strlen(pcPassword) + 1;
    pcTempPassword = (char *)realloc(psPwTContext->pcPassword, sizeof(char) * iSize);
    if (pcTempPassword == NULL)
    {
      PwTSetError(psPwTContext, "Failed to allocate memory (%s).", strerror(errno), NULL);
      return ER;
    }
    PATHWELL_ASSERT(pcTempPassword != NULL);
    strncpy(pcTempPassword, pcPassword, iSize);
  }
  psPwTContext->pcPassword = pcTempPassword;
  return ER_OK;
}


/*-
 ***********************************************************************
 *
 * PwTSetTokenSet
 *
 ***********************************************************************
 */
int
PwTSetTokenSet(PW_T_CONTEXT *psPwTContext, int iTokenSet)
{
  if (PwTContextIsValid(psPwTContext) == PATHWELL_FALSE)
  {
    PwTSetError(psPwTContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwTContextIsValid(psPwTContext) == PATHWELL_TRUE);

  psPwTContext->iTokenSet = iTokenSet;
  return ER_OK;
}


/*-
 ***********************************************************************
 *
 * PwTSetTopology
 *
 ***********************************************************************
 */
int
PwTSetTopology(PW_T_CONTEXT *psPwTContext, char *pcTopology)
{
  char *pcTempTopology = NULL;
  int iSize = 0;

  if (PwTContextIsValid(psPwTContext) == PATHWELL_FALSE)
  {
    PwTSetError(psPwTContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwTContextIsValid(psPwTContext) == PATHWELL_TRUE);
  if (pcTopology == NULL)
  {
    if (psPwTContext->pcTopology != NULL)
    {
      free(psPwTContext->pcTopology);
    }
  }
  else
  {
    iSize = strlen(pcTopology) + 1;
    pcTempTopology = (char *)realloc(psPwTContext->pcTopology, sizeof(char) * iSize);
    if (pcTempTopology == NULL)
    {
      PwTSetError(psPwTContext, "Failed to allocate memory (%s).", strerror(errno), NULL);
      return ER;
    }
    PATHWELL_ASSERT(pcTempTopology != NULL);
    strncpy(pcTempTopology, pcTopology, iSize);
  }
  psPwTContext->pcTopology = pcTempTopology;
  return ER_OK;
}


/*-
 ***********************************************************************
 *
 * PwTTopologyToBaseNp1Id
 *
 ***********************************************************************
 */
int
PwTTopologyToBaseNp1Id(PW_T_CONTEXT *psPwTContext)
{
  char *pc = NULL;
  char *pcBaseNp1Topology = NULL;
  char *pcTokens = NULL;
  int iBaseNp1 = 0;
  int iError = 0;
  int iErrorFlag = 0;
  int iIndex = 0;
  int iTokenCount = 0;
  int64_t i64Id = 0;

  /*-
   *********************************************************************
   *
   * A valid context is required.
   *
   *********************************************************************
   */
  if (PwTContextIsValid(psPwTContext) == PATHWELL_FALSE)
  {
    PwTSetError(psPwTContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwTContextIsValid(psPwTContext) == PATHWELL_TRUE);

  /*-
   *********************************************************************
   *
   * Initialize various structure members.
   *
   *********************************************************************
   */
  PwTClearError(psPwTContext);
  PwTSetId(psPwTContext, 0);

  /*-
   *********************************************************************
   *
   * Reject token sets that aren't supported and topologies that don't
   * contain the right tokens.
   *
   *********************************************************************
   */
  iError = PwTVerifyTopologyString(psPwTContext);
  if (iError != ER_OK)
  {
    return ER; // Note: Error message already set.
  }

  /*-
   *********************************************************************
   *
   * Attempt to peform the conversion, but only for those topologies
   * that fall within specified limits and yield valid IDs. The IDs
   * produced employ a baseN plus 1 encoding scheme where N represents
   * the token set size and 1 represents a shift, which is required
   * to prevent ID collisions. Rather than assigning tokens a value
   * in the range [0-N], they are assigned a value in the range
   * [1-(N+1)]. This ensures that no token or topology string maps
   * to a value of zero. The table below provides a mapping of tokens
   * to values for each of the currently supported token sets.
   *
   *   +------------------------------------------+
   *   |            BaseN+1 Token Map             |
   *   +-------+---------------+-------+----------+
   *   | Token |       N       | Value |  Meaning |
   *   +-------+---------------+-------+----------+
   *   |    ?d | 4 | 5 | 6 | 7 |     1 |   Digits |
   *   |    ?l | 4 | 5 | 6 | 7 |     2 |   Lowers |
   *   |    ?u | 4 | 5 | 6 | 7 |     3 |   Uppers |
   *   |    ?s | 4 | 5 | 6 | 7 |     4 | Specials |
   *   |    ?f | - | 5 | 6 | 7 |     5 | Reserved |
   *   |    ?x | - | - | 6 | 7 |     6 | Reserved |
   *   |    ?H | - | - | - | 7 |     7 | Reserved |
   *   +-------+---------------+-------+----------+
   *
   *  Note that the topology string "?z" represents a special case, and
   *  its corresponding ID is always zero.
   *
   *********************************************************************
   */
  if (strcmp(psPwTContext->pcTopology, "?z") == 0)
  {
    return ER_OK;
  }
  iTokenCount = strlen(psPwTContext->pcTopology) / (2 * sizeof(char));
  pcBaseNp1Topology = malloc(sizeof(char) * (iTokenCount + 1));
  if (pcBaseNp1Topology == NULL)
  {
    iErrorFlag = 1;
    PwTSetError(psPwTContext, "Failed to allocate memory (%s).", strerror(errno), NULL);
    goto PW_T_CLEANUP;
  }
  for (iIndex = 0, pc = pcBaseNp1Topology; iIndex < iTokenCount; iIndex++)
  {
    switch (psPwTContext->pcTopology[(iIndex * 2) + 1])
    {
    case 'd':
      *pc++ = '1';
      break;
    case 'l':
      *pc++ = '2';
      break;
    case 'u':
      *pc++ = '3';
      break;
    case 's':
      *pc++ = '4';
      break;
    case 'f':
      *pc++ = '5';
      break;
    case 'x':
      *pc++ = '6';
      break;
    case 'H':
      *pc++ = '7';
      break;
    default:
      iErrorFlag = 1;
      PwTSetError(psPwTContext, "Invalid token. That should not happen.", NULL);
      goto PW_T_CLEANUP;
      break;
    }
  }
  *pc = 0;
  pcTokens = PwTGetTokenSetTokens(psPwTContext);
  if (pcTokens == NULL)
  {
    iErrorFlag = 1;
    goto PW_T_CLEANUP; // Note: Error message already set.
  }
  iBaseNp1 = strlen(pcTokens) + 1;
  iError = PwSBaseNTo63Bit(pcBaseNp1Topology, &i64Id, iBaseNp1);
  if (iError != 0)
  {
    iErrorFlag = 1;
    switch (iError)
    {
    case -1:
      PwTSetError(psPwTContext, "Failed to convert topology.", NULL);
      break;
    default:
      PwTSetError(psPwTContext, "Topology can not be represented as a 63-bit integer.", NULL);
      break;
    }
    goto PW_T_CLEANUP;
  }
  PwTSetId(psPwTContext, i64Id);

PW_T_CLEANUP:
  if (pcBaseNp1Topology != NULL)
  {
    free(pcBaseNp1Topology);
  }

  return (iErrorFlag) ? ER : ER_OK;
}


/*-
 ***********************************************************************
 *
 * PwTTopologyToBitmaskId
 *
 ***********************************************************************
 */
int
PwTTopologyToBitmaskId(PW_T_CONTEXT *psPwTContext)
{
  int iBitsPerToken = 0;
  int iDigit = 0;
  int iError = 0;
  int iIndex = 0;
  int iMaxTokens = 0;
  int iShiftWidth = 0;
  int iTokenCount = 0;
  int64_t i64Id = 0;
  int64_t i64IdHeader = 0;

  /*-
   *********************************************************************
   *
   * A valid context is required.
   *
   *********************************************************************
   */
  if (PwTContextIsValid(psPwTContext) == PATHWELL_FALSE)
  {
    PwTSetError(psPwTContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwTContextIsValid(psPwTContext) == PATHWELL_TRUE);

  /*-
   *********************************************************************
   *
   * Initialize various structure members.
   *
   *********************************************************************
   */
  PwTClearError(psPwTContext);
  PwTSetId(psPwTContext, 0);

  /*-
   *********************************************************************
   *
   * Reject token sets that aren't supported and topologies that don't
   * contain the right tokens.
   *
   *********************************************************************
   */
  iError = PwTVerifyTopologyString(psPwTContext);
  if (iError != ER_OK)
  {
    return ER; // Note: Error message already set.
  }

  /*-
   *********************************************************************
   *
   * Attempt to peform the conversion, but only for those topologies
   * that fall within specified limits and yield valid IDs. The IDs
   * produced employ the following variable length encoding scheme:
   *
   *   <marker> <length> <token> [<token> ...]
   *
   * where the highest bit (a.k.a. the marker bit) is always set, the
   * next 5 bits represent the topology's length (in tokens), and the
   * remaining bits (taken in groups of 2 or 3 depending on the token
   * set size) represent the individual tokens. If the token set size
   * equals 4, then each token is represented by a 2-bit value; if it
   * is 5-7, then each token is represented by a 3-bit value. Currently,
   * there are no plans to support token set sizes > 7. The maximum ID
   * currently supported is 2^63 - 1. Consequently, topology lengths
   * are limited to 28 tokens for 2-bit mappings (i.e., 0 < token set
   * size <= 4) and 19 tokens for 3-bit mappings (i.e., 4 < token set
   * size <= 8). Below are the currently supported 2- and 3-bit token
   * maps:
   *
   *   +--------------------------+
   *   |      2-bit Token Map     |
   *   +-------+-------+----------+
   *   | Token | Value |  Meaning |
   *   +-------+-------+----------+
   *   |    ?d |    00 |   Digits |
   *   |    ?l |    01 |   Lowers |
   *   |    ?u |    10 |   Uppers |
   *   |    ?s |    11 | Specials |
   *   +-------+-------+----------+
   *
   *   +--------------------------+
   *   |      3-bit Token Map     |
   *   +-------+-------+----------+
   *   | Token | Value |  Meaning |
   *   +-------+-------+----------+
   *   |    ?d |   000 |   Digits |
   *   |    ?l |   001 |   Lowers |
   *   |    ?u |   010 |   Uppers |
   *   |    ?s |   011 | Specials |
   *   |    ?f |   100 | Reserved |
   *   |    ?x |   101 | Reserved |
   *   |    ?H |   110 | Reserved |
   *   +-------+-------+----------+
   *
   *  Note that the topology string "?z" represents a special case, and
   *  its corresponding ID is always zero.
   *
   *********************************************************************
   */
  if (strcmp(psPwTContext->pcTopology, "?z") == 0)
  {
    return ER_OK;
  }
  if (psPwTContext->iTokenSet <= PATHWELL_DEFAULT_TOKEN_SET_ID)
  {
    iBitsPerToken = 2;
    iMaxTokens = 28; // Reason: 1 bit marker + 5 bit length + (2 bits per token * 28 tokens) = 62 bits (result must not exceed 63 bits)
  }
  else
  {
    iBitsPerToken = 3;
    iMaxTokens = 19; // Reason: 1 bit marker + 5 bit length + (3 bits per token * 19 tokens) = 63 bits (result must not exceed 63 bits)
  }
  iTokenCount = strlen(psPwTContext->pcTopology) / (2 * sizeof(char));
  if (iTokenCount > iMaxTokens)
  {
    PwTSetError(psPwTContext, "Topology can not be represented as a 63-bit integer.", NULL);
    return ER;
  }
  for (iIndex = 0; iIndex < iTokenCount; iIndex++)
  {
    switch (psPwTContext->pcTopology[(iIndex * 2) + 1])
    {
    case 'd':
      iDigit = 0;
      break;
    case 'l':
      iDigit = 1;
      break;
    case 'u':
      iDigit = 2;
      break;
    case 's':
      iDigit = 3;
      break;
    case 'f':
      iDigit = 4;
      break;
    case 'x':
      iDigit = 5;
      break;
    case 'H':
      iDigit = 6;
      break;
    default:
      PwTSetError(psPwTContext, "Invalid token. That should not happen.", NULL);
      return ER;
      break;
    }
    iShiftWidth = iTokenCount - iIndex - 1;
    i64Id |= ((int64_t)iDigit << (iBitsPerToken * iShiftWidth));
  }
  iShiftWidth = iIndex;
  i64IdHeader = 0x20 | iTokenCount;
  i64Id |= (i64IdHeader << (iBitsPerToken * iShiftWidth));
  PwTSetId(psPwTContext, i64Id);

  return ER_OK;
}


/*-
 ***********************************************************************
 *
 * PwTTopologyToId
 *
 ***********************************************************************
 */
int
PwTTopologyToId(PW_T_CONTEXT *psPwTContext)
{
  /*-
   *********************************************************************
   *
   * A valid context is required.
   *
   *********************************************************************
   */
  if (PwTContextIsValid(psPwTContext) == PATHWELL_FALSE)
  {
    PwTSetError(psPwTContext, "Invalid context. That should not happen.", NULL);
    return ER;
  }
  PATHWELL_ASSERT(PwTContextIsValid(psPwTContext) == PATHWELL_TRUE);

  return (PwTGetEncoding(psPwTContext) == PATHWELL_ENCODING_BITMASK) ? PwTTopologyToBitmaskId(psPwTContext) : PwTTopologyToBaseNp1Id(psPwTContext);
}


/*-
 ***********************************************************************
 *
 * PwTVerifyTopologyString
 *
 ***********************************************************************
 */
int
PwTVerifyTopologyString(PW_T_CONTEXT *psPwTContext)
{
  char acRegex[PATHWELL_TOKENS_REGEX_SIZE] = { 0 };
  char *pcTokens = NULL;
  const char *pcPcreError = NULL;
  int aiPcreOVector[PCRE_OVECTOR_ARRAY_SIZE] = { 0 };
  int iErrorFlag = 0;
  int iPcreErrorOffset = 0;
  int iPcreResult = 0;
  pcre *psPcre = NULL;

  /*-
   *********************************************************************
   *
   * Reject token sets that aren't supported and topology strings that
   * don't contain the right tokens.
   *
   *********************************************************************
   */
  pcTokens = PwTGetTokenSetTokens(psPwTContext);
  if (pcTokens == NULL)
  {
    iErrorFlag = 1;
    goto PW_T_CLEANUP; // Note: Error message already set.
  }
  snprintf(acRegex, PATHWELL_TOKENS_REGEX_SIZE, "^(?:[?]z|(?:[?][%s])+)$", pcTokens);
  psPcre = pcre_compile(acRegex, 0, &pcPcreError, &iPcreErrorOffset, NULL);
  if (psPcre == NULL)
  {
    iErrorFlag = 1;
    PwTSetError(psPwTContext, "Failed to compile topology regular expression. That should not happen.", NULL);
    goto PW_T_CLEANUP;
  }
  PATHWELL_ASSERT(psPcre != NULL);
  iPcreResult = pcre_exec(psPcre, 0, psPwTContext->pcTopology, strlen(psPwTContext->pcTopology), 0, 0, aiPcreOVector, PCRE_OVECTOR_ARRAY_SIZE);
  if (iPcreResult < 0)
  {
    switch (iPcreResult)
    {
    case PCRE_ERROR_NOMATCH:
      iErrorFlag = 1;
      PwTSetError(psPwTContext, "Topology contains one or more tokens not from the specified token set.", NULL);
      goto PW_T_CLEANUP;
      break;
    default:
      iErrorFlag = 1;
      PwTSetError(psPwTContext, "Failed to execute topology regular expression. That should not happen.", NULL);
      goto PW_T_CLEANUP;
      break;
    }
  }

PW_T_CLEANUP:
  if (psPcre != NULL)
  {
    pcre_free(psPcre);
  }

  return (iErrorFlag) ? ER : ER_OK;
}


/*-
 ***********************************************************************
 *
 * PwVGetLibraryAge
 *
 ***********************************************************************
 */
int
PwVGetLibraryAge(void)
{
  return PW_V_LIBRARY_AGE;
}


/*-
 ***********************************************************************
 *
 * PwVGetLibraryCurrent
 *
 ***********************************************************************
 */
int
PwVGetLibraryCurrent(void)
{
  return PW_V_LIBRARY_CURRENT;
}


/*-
 ***********************************************************************
 *
 * PwVGetLibraryRevision
 *
 ***********************************************************************
 */
int
PwVGetLibraryRevision(void)
{
  return PW_V_LIBRARY_REVISION;
}


/*-
 ***********************************************************************
 *
 * PwVGetLibraryVersion
 *
 ***********************************************************************
 */
const char *
PwVGetLibraryVersion(void)
{
  return PW_V_LIBRARY_VERSION;
}


/*-
 ***********************************************************************
 *
 * PwVGetModuleAge
 *
 ***********************************************************************
 */
int
PwVGetModuleAge(void)
{
  return PW_V_MODULE_AGE;
}


/*-
 ***********************************************************************
 *
 * PwVGetModuleCurrent
 *
 ***********************************************************************
 */
int
PwVGetModuleCurrent(void)
{
  return PW_V_MODULE_CURRENT;
}


/*-
 ***********************************************************************
 *
 * PwVGetModuleRevision
 *
 ***********************************************************************
 */
int
PwVGetModuleRevision(void)
{
  return PW_V_MODULE_REVISION;
}


/*-
 ***********************************************************************
 *
 * PwVGetModuleVersion
 *
 ***********************************************************************
 */
const char *
PwVGetModuleVersion(void)
{
  return PW_V_MODULE_VERSION;
}


/*-
 ***********************************************************************
 *
 * PwVGetReleaseMajor
 *
 ***********************************************************************
 */
int
PwVGetReleaseMajor(void)
{
  return PW_V_RELEASE_MAJOR;
}


/*-
 ***********************************************************************
 *
 * PwVGetReleaseMinor
 *
 ***********************************************************************
 */
int
PwVGetReleaseMinor(void)
{
  return PW_V_RELEASE_MINOR;
}


/*-
 ***********************************************************************
 *
 * PwVGetReleaseNumber
 *
 ***********************************************************************
 */
uint32_t
PwVGetReleaseNumber(void)
{
  return PW_V_RELEASE_NUMBER;
}


/*-
 ***********************************************************************
 *
 * PwVGetReleasePatch
 *
 ***********************************************************************
 */
int
PwVGetReleasePatch(void)
{
  return PW_V_RELEASE_PATCH;
}


/*-
 ***********************************************************************
 *
 * PwVGetReleaseString
 *
 ***********************************************************************
 */
const char *
PwVGetReleaseString(void)
{
  return PACKAGE_VERSION;
}
