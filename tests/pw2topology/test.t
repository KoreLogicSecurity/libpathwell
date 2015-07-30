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
  my $sCommand = "../../src/pw2topology/pathwell-pw2topology";
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

=head1 TEST_COUNT=12

=head1 TEST_SUMMARY

Test other conversions.

=cut

  my @aTuples =
  (
    ["echo ''", "-f -", "Password=''; Topology='?z';"],
    ["echo 'Summer2013!'", "-f -", "Password='Summer2013!'; Topology='?u?l?l?l?l?l?d?d?d?d?s';"],
    ["echo 'Change123.'", "-f -", "Password='Change123.'; Topology='?u?l?l?l?l?l?d?d?d?s';"],
    ["echo '4ppl3'", "-f - -s 3", "Password='4ppl3'; Topology='?d?l?l?l?d';"],
    ["echo 'abc123***'", "-f - -s 1", "Password='abc123***'; Topology='?l?l?l?d?d?d?s?s?s';"],
    ["echo 'abc123***'", "-f - -s 4", "Password='abc123***'; Topology='?l?l?l?d?d?d?s?s?s';"],

    ["", "''", "Password=''; Topology='?z';"],
    ["", "'Summer2013!'", "Password='Summer2013!'; Topology='?u?l?l?l?l?l?d?d?d?d?s';"],
    ["", "'Change123.'", "Password='Change123.'; Topology='?u?l?l?l?l?l?d?d?d?s';"],
    ["", "-s 3 '4ppl3'", "Password='4ppl3'; Topology='?d?l?l?l?d';"],
    ["", "-s 1 'abc123***'", "Password='abc123***'; Topology='?l?l?l?d?d?d?s?s?s';"],
    ["", "-s 4 'abc123***'", "Password='abc123***'; Topology='?l?l?l?d?d?d?s?s?s';"],
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


