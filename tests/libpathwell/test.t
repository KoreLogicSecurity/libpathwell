#!/usr/bin/perl -w
######################################################################
#
# $Id: test.t,v 1.26 2013/11/05 02:14:34 klm Exp $
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

  my @aOutput;
  my %hKvps;
  my $sCommandLine;
  my $sActualStatus;
  my $sSrcDir = $ENV{'srcdir'} || ".";

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
  $sCommandLine = qq($sSrcDir/../../utils/version2string -t tar -v $hKvps{'VERSION'} | tr -d '\n');
  @aOutput = qx($sCommandLine);

  $sCommandLine = qq(./api_PwVGetReleaseString $aOutput[0]);
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

  $sCommandLine = qq(./api_PwVGetLibraryVersion $hKvps{'CURRENT'}:$hKvps{'REVISION'}:$hKvps{'AGE'});
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

  $sCommandLine = qq(./api_PwVGetModuleVersion $hKvps{'CURRENT'}:$hKvps{'REVISION'}:$hKvps{'AGE'});
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

  $sCommandLine = qq(./api_PwTTopologyToId);
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

  $sCommandLine = qq(./api_PwTIdToTopology);
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

  $sCommandLine = qq(./api_PwLCalcLevDistance);
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

  $sCommandLine = qq(./api_PwLCheckLevDistance);
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

  $sCommandLine = qq(./api_PwSChomp);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test PwSChomp.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=9

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test database schema creation.

=cut

  $sCommandLine = qq(./api_PwDCreateSchema);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test database schema creation.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=10

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test database schema deletion.

=cut

  $sCommandLine = qq(./api_PwDDeleteSchema);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test database schema deletion.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=11

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test database schema verification.

=cut

  $sCommandLine = qq(./api_PwDVerifySchema);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test database schema verification.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=12

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test loading blacklisted topologies.

=cut

  $sCommandLine = qq(./api_PwDLoadBlacklistedTopologies $sSrcDir/../../data/topology_blacklist.default);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test loading blacklisted topologies.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=13

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test use count increments.

=cut

  $sCommandLine = qq(./api_PwDIncrementUseCount);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test use count increments.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=14

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test use count decrements.

=cut

  $sCommandLine = qq(./api_PwDDecrementUseCount);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test use count decrements.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=15

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test clearing use count.

=cut

  $sCommandLine = qq(./api_PwDClearUseCount);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test clearing use counts.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=16

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test passwords in various token sets.

=cut

  $sCommandLine = qq(./api_PwTIsPasswordInTokenSet);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test passwords in various token sets.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=17

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test password to topology conversions.

=cut

  $sCommandLine = qq(./api_PwTPasswordToTopology);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test password to topology conversions.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=18

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test base N conversions.

=cut

  $sCommandLine = qq(./api_PwSBaseNTo63Bit);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test base N conversions.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=19

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test dirname operations.

=cut

  $sCommandLine = qq(./api_PwSDirname);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test dirname operations.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=20

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test Levenshtein distance calculations.

=cut

  $sCommandLine = qq(./api_PwSLevenshteinDistance);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test Levenshtein distance calculations.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=21

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test blacklisted topologies.

=cut

  $sCommandLine = qq(./api_PwDTopologyIsBlacklisted $sSrcDir/../../data/topology_blacklist.default);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  ok(VerifyExitStatus($sActualStatus, 0, 0), "Test blacklisted topologies.");

