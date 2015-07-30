#!/usr/bin/perl -w
######################################################################
#
# $Id: test.t,v 1.10 2013/09/09 16:00:52 klm Exp $
#
######################################################################
#
# Copyright 2013-2013 KoreLogic Inc., All Rights Reserved.
#
# This software, having been partly or wholly developed and/or
# sponsored by KoreLogic, Inc., is hereby released under the terms
# and conditions set forth in the project's "README.LICENSE" file.
# For a list of all contributors and sponsors, please refer to the
# project's "README.CREDITS" file.
#
######################################################################
#
# Purpose: Tests for KoreLogic::PathWell::PwTRoutines.
#
######################################################################

use 5.008;
use strict;
use Test::More 0.045;

BEGIN
{
  plan('tests' => GetTestCount());

  sub GetTestCount
  {
    my $sCount = 0;
    open(FH, __FILE__) or die("Error='Failed to open test file ($!).'\n");
    while (my $sLine = <FH>)
    {
      $sCount += $1 if ($sLine =~ /^=head1 TEST_COUNT=(\d+)/)
    }
    close(FH);
    return $sCount;
  }
}

######################################################################
#
# VerifyExitStatus
#
######################################################################

sub VerifyExitStatus
{
  my ($sActualStatus, $sTargetExitCode, $sTargetSignal) = @_;

  my $sActualSignal = $sActualStatus & 127;
# my $sActualCoreDump = $sActualStatus & 128; # Reserved for future use.
  my $sActualExitCode = $sActualStatus >> 8;

  return ($sActualExitCode == $sTargetExitCode && $sActualSignal == $sTargetSignal) ? 1 : 0;
}


######################################################################
#
# Tests
#
######################################################################

  ####################################################################
  #
  # Declare common variables and data structures.
  #
  ####################################################################

  my @aOutput = ();
  my $sActualStatus = 0;
  my $sCommand = "../../src/topology2id/pathwell-topology2id";
  my $sCommandLine = "";
  my $sErrFile = "test.t.stderr";
  my $sMatch = 0;
  my $sOutFile = "test.t.stdout";
  my $sSrcDir = $ENV{'srcdir'} || ".";

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=1

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test usage when no arguments are given.

=cut

  $sCommandLine = qq($sCommand > $sOutFile 2> $sErrFile);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  $sMatch = 0;
  if (open(FH, "< $sErrFile"))
  {
    while (my $sLine = <FH>)
    {
      if ($sLine =~ /^Usage:/)
      {
        $sMatch = 1;
        last;
      }
    }
    close(FH);
  }
  ok(VerifyExitStatus($sActualStatus, 1, 0) && $sMatch, "Test usage when no arguments are given.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=3

=head1 TEST_COUNT=14

=head1 TEST_SUMMARY

Test other conversions.

=cut

  my @aTuples =
  (
    ["echo ''", "-f -", "Topology=''; ID='undef'; Error='Topology contains one or more tokens not from the specified token set.';"],
    ["echo '?z'", "-f -", "Topology='?z'; ID='0';"],
    ["echo '?u?l?l?l?l?d'", "-f - -t bitmask", "Topology='?u?l?l?l?l?d'; ID='158036';"],
    ["echo '?s?s?s'", "-f -", "Topology='?s?s?s'; ID='124';"],
    ["echo '?H?x'", "-f - -s 4", "Topology='?H?x'; ID='62';"],
    ["echo '?H?H?H?H'", "-f - -t bitmask -s 4", "Topology='?H?H?H?H'; ID='150966';"],
    ["echo '?s?s?s'", "-f - -s 3 -t baseN+1", "Topology='?s?s?s'; ID='228';"],

    ["", "''", "Topology=''; ID='undef'; Error='Topology contains one or more tokens not from the specified token set.';"],
    ["", "'?z'", "Topology='?z'; ID='0';"],
    ["", "-t bitmask '?u?l?l?l?l?d'", "Topology='?u?l?l?l?l?d'; ID='158036';"],
    ["", "'?s?s?s'", "Topology='?s?s?s'; ID='124';"],
    ["", "-s 4 '?H?x'", "Topology='?H?x'; ID='62';"],
    ["", "-t bitmask -s 4 '?H?H?H?H'", "Topology='?H?H?H?H'; ID='150966';"],
    ["", "-s 3 -t baseN+1 '?s?s?s'", "Topology='?s?s?s'; ID='228';"],
  );

  my $sArgs1 = "";
  my $sArgs2 = "";

  foreach my $paArguments (@aTuples)
  {
    $sArgs1 = @{$paArguments}[0];
    if ($sArgs1 ne "")
    {
      $sArgs1 =~ s/ *$/ | /g;
    }

    $sArgs2 = @{$paArguments}[1];
    $sArgs2 =~ s/^ */ /g;
    $sArgs2 =~ s/ +$//g;

    $sCommandLine = $sArgs1 . $sCommand . $sArgs2;

    chomp(@aOutput = qx($sCommandLine));

    ok($aOutput[0] eq @{$paArguments}[2] , "Test command: \`$sCommandLine\`.");
  }

