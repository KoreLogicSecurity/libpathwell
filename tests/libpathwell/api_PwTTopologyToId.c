/*-
 ***********************************************************************
 *
 * $Id: api_PwTTopologyToId.c,v 1.16 2013/11/04 15:19:35 klm Exp $
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
#include <inttypes.h>
#include <pathwell.h>
#include <stdio.h>
#include <stdint.h>
#include <tap.h>

typedef struct _TEST_TUPLES
{
  char acType[16];
  uint32_t ui32TokenSet;
  char acTopology[PATHWELL_MAX_TOPOLOGY_SIZE];
  int64_t i64Id;
  char acError[256];
} TEST_TUPLES;

int
main(int iArgumentCount, char *ppcArgumentVector[])
{
  char acDescription[1024] = "";
  int iError = 0;
  int iIndex = 0;
  PW_T_CONTEXT *psPwTContext = NULL;
  TEST_TUPLES asTuples[] =
  {
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "junk", -1, "Topology contains one or more tokens not from the specified token set."},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_2, "junk", -1, "Topology contains one or more tokens not from the specified token set."},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_3, "junk", -1, "Topology contains one or more tokens not from the specified token set."},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_4, "junk", -1, "Topology contains one or more tokens not from the specified token set."},

    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "junk", -1, "Topology contains one or more tokens not from the specified token set."},
    { "bitmask", PATHWELL_TOKEN_SET_ID_2, "junk", -1, "Topology contains one or more tokens not from the specified token set."},
    { "bitmask", PATHWELL_TOKEN_SET_ID_3, "junk", -1, "Topology contains one or more tokens not from the specified token set."},
    { "bitmask", PATHWELL_TOKEN_SET_ID_4, "junk", -1, "Topology contains one or more tokens not from the specified token set."},

    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?z", 0, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?d", 1, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?l", 2, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?u", 3, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?s", 4, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?d?d", 6, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?d?l", 7, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?d?u", 8, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?d?s", 9, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?l?d", 11, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?l?l", 12, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?l?u", 13, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?l?s", 14, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?u?d", 16, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?u?l", 17, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?u?u", 18, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?u?s", 19, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?s?d", 21, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?s?l", 22, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?s?u", 23, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?s?s", 24, ""},

    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?d?d?d", 31, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?d?d?l", 32, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?d?d?u", 33, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?d?d?s", 34, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?d?l?d", 36, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?d?l?l", 37, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?d?l?u", 38, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?d?l?s", 39, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?d?u?d", 41, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?d?u?l", 42, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?d?u?u", 43, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?d?u?s", 44, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?d?s?d", 46, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?d?s?l", 47, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?d?s?u", 48, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?d?s?s", 49, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?l?d?d", 56, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?l?d?l", 57, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?l?d?u", 58, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?l?d?s", 59, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?l?l?d", 61, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?l?l?l", 62, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?l?l?u", 63, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?l?l?s", 64, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?l?u?d", 66, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?l?u?l", 67, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?l?u?u", 68, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?l?u?s", 69, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?l?s?d", 71, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?l?s?l", 72, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?l?s?u", 73, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?l?s?s", 74, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?u?d?d", 81, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?u?d?l", 82, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?u?d?u", 83, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?u?d?s", 84, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?u?l?d", 86, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?u?l?l", 87, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?u?l?u", 88, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?u?l?s", 89, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?u?u?d", 91, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?u?u?l", 92, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?u?u?u", 93, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?u?u?s", 94, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?u?s?d", 96, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?u?s?l", 97, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?u?s?u", 98, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?u?s?s", 99, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?s?d?d", 106, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?s?d?l", 107, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?s?d?u", 108, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?s?d?s", 109, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?s?l?d", 111, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?s?l?l", 112, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?s?l?u", 113, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?s?l?s", 114, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?s?u?d", 116, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?s?u?l", 117, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?s?u?u", 118, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?s?u?s", 119, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?s?s?d", 121, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?s?s?l", 122, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?s?s?u", 123, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?s?s?s", 124, ""},

    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?d?d?l?u?s?l", 4097, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?s?s?s?s?s?s?s", 78124, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?s?s?s?s?s?s?s?s?s?s?s", 48828124, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?s?s?s?d?l?u?s?s?s?s?s?s?s?s?s", 30349609374, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?s?s?s?d?l?u?s?s?s?s?s?s?s?s?s?s?s?s", 3793701171874, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?s?s?s?d?l?u?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s", 1481914520263671874, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?s?s?s?d?l?u?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s", 7409572601318359374, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, "?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s", 7450580596923828124, ""},

    { "baseN+1", PATHWELL_TOKEN_SET_ID_2, "?z", 0, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_2, "?d", 1, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_2, "?l", 2, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_2, "?u", 3, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_2, "?s", 4, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_2, "?f", 5, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_2, "?f?f?f?f?f?f?f?f?f?f?f?f?f?f?f?f?f?f?f?f?f?f?f?f", 4738381338321616895, ""},

    { "baseN+1", PATHWELL_TOKEN_SET_ID_3, "?z", 0, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_3, "?d", 1, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_3, "?l", 2, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_3, "?u", 3, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_3, "?s", 4, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_3, "?f", 5, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_3, "?x", 6, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_3, "?x?x?x?x?x?x?x?x?x?x?x?x?x?x?x?x?x?x?x?x?x?x", 3909821048582988048, ""},

    { "baseN+1", PATHWELL_TOKEN_SET_ID_4, "?z", 0, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_4, "?d", 1, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_4, "?l", 2, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_4, "?u", 3, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_4, "?s", 4, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_4, "?f", 5, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_4, "?x", 6, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_4, "?H", 7, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_4, "?H?H?H?H?H?H?H?H?H?H?H?H?H?H?H?H?H?H?H?H?H", 9223372036854775807, ""},

    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?z", 0, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?d", 132, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?l", 133, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?u", 134, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?s", 135, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?d?d", 544, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?l?d", 548, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?u?d", 552, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?s?d", 556, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?d?l", 545, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?l?l", 549, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?u?l", 553, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?s?l", 557, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?d?u", 546, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?l?u", 550, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?u?u", 554, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?s?u", 558, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?d?s", 547, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?l?s", 551, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?u?s", 555, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?s?s", 559, ""},

    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?d?d?d", 2240, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?d?d?l", 2241, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?d?d?u", 2242, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?d?d?s", 2243, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?d?l?d", 2244, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?d?l?l", 2245, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?d?l?u", 2246, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?d?l?s", 2247, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?d?u?d", 2248, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?d?u?l", 2249, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?d?u?u", 2250, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?d?u?s", 2251, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?d?s?d", 2252, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?d?s?l", 2253, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?d?s?u", 2254, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?d?s?s", 2255, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?l?d?d", 2256, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?l?d?l", 2257, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?l?d?u", 2258, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?l?d?s", 2259, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?l?l?d", 2260, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?l?l?l", 2261, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?l?l?u", 2262, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?l?l?s", 2263, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?l?u?d", 2264, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?l?u?l", 2265, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?l?u?u", 2266, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?l?u?s", 2267, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?l?s?d", 2268, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?l?s?l", 2269, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?l?s?u", 2270, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?l?s?s", 2271, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?u?d?d", 2272, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?u?d?l", 2273, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?u?d?u", 2274, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?u?d?s", 2275, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?u?l?d", 2276, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?u?l?l", 2277, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?u?l?u", 2278, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?u?l?s", 2279, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?u?u?d", 2280, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?u?u?l", 2281, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?u?u?u", 2282, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?u?u?s", 2283, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?u?s?d", 2284, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?u?s?l", 2285, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?u?s?u", 2286, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?u?s?s", 2287, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?s?d?d", 2288, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?s?d?l", 2289, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?s?d?u", 2290, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?s?d?s", 2291, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?s?l?d", 2292, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?s?l?l", 2293, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?s?l?u", 2294, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?s?l?s", 2295, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?s?u?d", 2296, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?s?u?l", 2297, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?s?u?u", 2298, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?s?u?s", 2299, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?s?s?d", 2300, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?s?s?l", 2301, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?s?s?u", 2302, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?s?s?s", 2303, ""},

    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?d?d?l?u?s?l", 155757, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?d?s?s?u?l?s?d", 642972, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?s?s?s?s?s?s?s", 655359, ""},

    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s", 265712378014859263, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s", 1080863910568919039, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, "?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s", 4395513236313604095, ""},

    { "bitmask", PATHWELL_TOKEN_SET_ID_2, "?z", 0, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_2, "?d", 264, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_2, "?l", 265, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_2, "?u", 266, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_2, "?s", 267, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_2, "?f", 268, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_2, "?f?f?f?f?f?f?f?f?f?f?f?f?f?f?f?f?f?f?f", 7432226127911995684, ""},

    { "bitmask", PATHWELL_TOKEN_SET_ID_3, "?z", 0, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_3, "?d", 264, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_3, "?l", 265, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_3, "?u", 266, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_3, "?s", 267, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_3, "?f", 268, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_3, "?x", 269, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_3, "?x?x?x?x?x?x?x?x?x?x?x?x?x?x?x?x?x?x?x", 7452814011922832237, ""},

    { "bitmask", PATHWELL_TOKEN_SET_ID_4, "?z", 0, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_4, "?d", 264, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_4, "?l", 265, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_4, "?u", 266, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_4, "?s", 267, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_4, "?f", 268, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_4, "?x", 269, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_4, "?H", 270, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_4, "?H?H?H?H?H?H?H?H?H?H?H?H?H?H", 206079893663158, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_4, "?H?H?H?H?H?H?H?H?H?H?H?H?H?H?H?H", 13752063147863478, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_4, "?H?H?H?H?H?H?H?H?H?H?H?H?H?H?H?H?H?H?H", 7473401895933668790, ""},
  };

  plan_tests(243);

  psPwTContext = PwTNewContext();
  ok(psPwTContext != NULL, "new context");

  for (iIndex = 0; iIndex < (sizeof(asTuples) / sizeof(asTuples[0])); iIndex++)
  {
    iError = PwTSetTopology(psPwTContext, asTuples[iIndex].acTopology);
//  ok(iError == 0, "set topology");
    iError = PwTSetTokenSet(psPwTContext, asTuples[iIndex].ui32TokenSet);
//  ok(iError == 0, "set token set");
    iError = PwTSetEncoding(psPwTContext, ((strcmp(asTuples[iIndex].acType, "bitmask") == 0) ? PATHWELL_ENCODING_BITMASK : PATHWELL_ENCODING_BASENP1));
//  ok(iError == 0, "set encoding");
    iError = PwTTopologyToId(psPwTContext);
    if (iError == ER_OK)
    {
      snprintf
      (
        acDescription,
        1024,
        "\"%s\", %u, \"%s\", %"PRId64", %"PRId64,
        asTuples[iIndex].acType,
        asTuples[iIndex].ui32TokenSet,
        asTuples[iIndex].acTopology,
        asTuples[iIndex].i64Id,
        *psPwTContext->pi64Id
      );
      ok(*psPwTContext->pi64Id == asTuples[iIndex].i64Id, acDescription);
    }
    else
    {
      snprintf
      (
        acDescription,
        1024,
        "\"%s\", %u, \"%s\", \"%s\", \"%s",
        asTuples[iIndex].acType,
        asTuples[iIndex].ui32TokenSet,
        asTuples[iIndex].acTopology,
        asTuples[iIndex].acError,
        PwTGetError(psPwTContext)
      );
      ok(strcmp(PwTGetError(psPwTContext), asTuples[iIndex].acError) == 0, acDescription);
    }
  }
  PwTFreeContext(psPwTContext);

  return exit_status();
}
