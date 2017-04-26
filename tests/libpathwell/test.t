#!/bin/sh
#!/usr/bin/perl -w
eval 'exec perl -x ${0} ${1+"${@}"} ;'
  if (0);
######################################################################
#
# $Id: test.t,v 1.35 2017/04/21 19:39:39 ssegreti Exp $
#
######################################################################
#
# Copyright 2013-2017 The PathWell Project, All Rights Reserved.
#
# This software, having been partly or wholly developed and/or
# sponsored by KoreLogic, Inc., is hereby released under the terms
# and conditions set forth in the project's "README.LICENSE" file.
# For a list of all contributors and sponsors, please refer to the
# project's "README.CREDITS" file.
#
######################################################################
#
# Purpose: Tests for libpathwell.
#
######################################################################

use 5.008;
use strict;
use Test::More 0.045;

BEGIN
{
  ####################################################################
  #
  # The Properties hash is essentially private. Those parts of the
  # program that wish to access or modify the data in this hash need
  # to call GetProperties() to obtain a reference.
  #
  ####################################################################

  my (%hProperties);

  ####################################################################
  #
  # Initialize platform-specific variables.
  #
  ####################################################################

  if ($^O =~ /MSWin(32|64)/i)
  {
    $hProperties{'OsClass'} = "WINX";
    $hProperties{'Extension'} = ".exe";
  }
  else
  {
    $hProperties{'OsClass'} = "UNIX";
    $hProperties{'Extension'} = "";
  }

  ####################################################################
  #
  # Define helper routines.
  #
  ####################################################################

  sub GetProperties
  {
    return \%hProperties;
  }

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

  ####################################################################
  #
  # Create a test plan.
  #
  ####################################################################

  plan('tests' => GetTestCount());
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

  my @aOutput;
  my %hKvps;
  my $sCommand;
  my $sCommandLine;
  my $sActualStatus;
  my $sSrcDir = $ENV{'srcdir'} || ".";
  my $phProperties = GetProperties();

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=1

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test release string.

=cut

  open(FH, "< $sSrcDir/../../VERSION") || die ("Failed to open \"VERSION\" ($!).\n");
  %hKvps = ();
  while (my $sLine = <FH>)
  {
    $sLine =~ s/[\r\n]+$//;
    if ($sLine =~ /^(0x[0-9A-Fa-f]{8})$/)
    {
      $hKvps{'VERSION'} = $1;
    }
  }
  close(FH);
  $sCommandLine = qq($^X $sSrcDir/../../utils/version2string -t tar -v $hKvps{'VERSION'} | tr -d '\n');
  @aOutput = qx($sCommandLine);

  $sCommand = "api_PwVGetReleaseString" . $$phProperties{'Extension'};
  $sCommandLine = qq(./$sCommand $aOutput[0]);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test release string.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=2

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test library version.

=cut

  open(FH, "< $sSrcDir/../../VERSION.LIBRARY") || die ("Failed to open \"VERSION.LIBRARY\" ($!).\n");
  %hKvps = ();
  while (my $sLine = <FH>)
  {
    $sLine =~ s/[\r\n]+$//;
    if ($sLine =~ /^(CURRENT|REVISION|AGE)=(\d+)$/)
    {
      $hKvps{$1} = $2;
    }
  }
  close(FH);

  $sCommand = "api_PwVGetLibraryVersion" . $$phProperties{'Extension'};
  $sCommandLine = qq(./$sCommand $hKvps{'CURRENT'}:$hKvps{'REVISION'}:$hKvps{'AGE'});
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test library version.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=3

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test module version.

=cut

  open(FH, "< $sSrcDir/../../VERSION.MODULE") || die ("Failed to open \"VERSION.MODULE\" ($!).\n");
  %hKvps = ();
  while (my $sLine = <FH>)
  {
    $sLine =~ s/[\r\n]+$//;
    if ($sLine =~ /^(CURRENT|REVISION|AGE)=(\d+)$/)
    {
      $hKvps{$1} = $2;
    }
  }
  close(FH);

  $sCommand = "api_PwVGetModuleVersion" . $$phProperties{'Extension'};
  $sCommandLine = qq(./$sCommand $hKvps{'CURRENT'}:$hKvps{'REVISION'}:$hKvps{'AGE'});
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test module version.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=4

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test topology to ID conversions.

=cut

  $sCommand = "api_PwTTopologyToId" . $$phProperties{'Extension'};
  $sCommandLine = qq(./$sCommand);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test topology to ID conversions.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=5

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test ID to topology conversions.

=cut

  $sCommand = "api_PwTIdToTopology" . $$phProperties{'Extension'};
  $sCommandLine = qq(./$sCommand);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test ID to topology conversions.");


#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=6

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test Lev Distance calculations.

=cut

  $sCommand = "api_PwLCalcLevDistance" . $$phProperties{'Extension'};
  $sCommandLine = qq(./$sCommand);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test Lev Distance calculations.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=7

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test Lev Distance checks.

=cut

  $sCommand = "api_PwLCheckLevDistance" . $$phProperties{'Extension'};
  $sCommandLine = qq(./$sCommand);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test Lev Distance checks.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=8

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test chomp operations.

=cut

  $sCommand = "api_PwSAppendToDynamicString" . $$phProperties{'Extension'};
  $sCommandLine = qq(./$sCommand);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test PwSAppendToDynamicString.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=9

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test chomp operations.

=cut

  $sCommand = "api_PwSChomp" . $$phProperties{'Extension'};
  $sCommandLine = qq(./$sCommand);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test PwSChomp.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=10

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test database schema creation.

=cut

  $sCommand = "api_PwDCreateSchema" . $$phProperties{'Extension'};
  $sCommandLine = qq(./$sCommand);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test database schema creation.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=11

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test database schema deletion.

=cut

  $sCommand = "api_PwDDeleteSchema" . $$phProperties{'Extension'};
  $sCommandLine = qq(./$sCommand);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test database schema deletion.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=12

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test database schema verification.

=cut

  $sCommand = "api_PwDVerifySchema" . $$phProperties{'Extension'};
  $sCommandLine = qq(./$sCommand);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test database schema verification.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=13

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test loading blacklisted topologies.

=cut

  $sCommand = "api_PwDLoadBlacklistedTopologies" . $$phProperties{'Extension'};
  $sCommandLine = qq(./$sCommand $sSrcDir/../../data/topology_blacklist.default);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test loading blacklisted topologies.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=14

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test use count increments.

=cut

  $sCommand = "api_PwDIncrementUseCount" . $$phProperties{'Extension'};
  $sCommandLine = qq(./$sCommand);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test use count increments.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=15

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test use count decrements.

=cut

  $sCommand = "api_PwDDecrementUseCount" . $$phProperties{'Extension'};
  $sCommandLine = qq(./$sCommand);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test use count decrements.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=16

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test clearing use count.

=cut

  $sCommand = "api_PwDClearUseCount" . $$phProperties{'Extension'};
  $sCommandLine = qq(./$sCommand);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test clearing use counts.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=17

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test passwords in various token sets.

=cut

  $sCommand = "api_PwTIsPasswordInTokenSet" . $$phProperties{'Extension'};
  $sCommandLine = qq(./$sCommand);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test passwords in various token sets.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=18

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test password to topology conversions.

=cut

  $sCommand = "api_PwTPasswordToTopology" . $$phProperties{'Extension'};
  $sCommandLine = qq(./$sCommand);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test password to topology conversions.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=19

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test base N conversions.

=cut

  $sCommand = "api_PwSBaseNTo63Bit" . $$phProperties{'Extension'};
  $sCommandLine = qq(./$sCommand);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test base N conversions.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=20

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test dirname operations.

=cut

  $sCommand = "api_PwSDirname" . $$phProperties{'Extension'};
  $sCommandLine = qq(./$sCommand);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test dirname operations.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=21

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test Levenshtein distance calculations.

=cut

  $sCommand = "api_PwSLevenshteinDistance" . $$phProperties{'Extension'};
  $sCommandLine = qq(./$sCommand);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test Levenshtein distance calculations.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=22

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test blacklisted topologies.

=cut

  $sCommand = "api_PwDTopologyIsBlacklisted" . $$phProperties{'Extension'};
  $sCommandLine = qq(./$sCommand $sSrcDir/../../data/topology_blacklist.default);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test blacklisted topologies.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=23

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test PwCAllCheck.

=cut

  $sCommand = "api_PwCAllCheck" . $$phProperties{'Extension'};
  $sCommandLine = qq(./$sCommand $sSrcDir/../../data/topology_blacklist.default);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test AllCheck().");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  
=pod

=head1 TEST_GROUP=24

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test generating nearby topology.

=cut

  $sCommand = "api_PwHGenNearbyTopology" . $$phProperties{'Extension'};
  $sCommandLine = qq(./$sCommand $sSrcDir/../../data/topology_blacklist.default);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test generating nearby topology.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=25

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test generating nearby password.

=cut

  $sCommand = "api_PwHGenNearbyPassword" . $$phProperties{'Extension'};
  $sCommandLine = qq(./$sCommand $sSrcDir/../../data/topology_blacklist.default);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test generating nearby password.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=26

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test generating hints.

=cut

  $sCommand = "api_PwHGenHint" . $$phProperties{'Extension'};
  $sCommandLine = qq(./$sCommand $sSrcDir/../../data/topology_blacklist.default);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test generating hints.");

