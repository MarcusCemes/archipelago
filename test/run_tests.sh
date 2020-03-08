#!/bin/bash
# Archipelago Test Suite v1.0.0
# Author: Marcus Cemes <marcus.cemes@epfl.ch>
# Heavily inspired by the Speleo Test Suite
# https://github.com/MarcusCemes/speleo-test-suite

# Strict execution
set -u


# Parse args
if [ -n "${1:-}" ]; then
  programPath=$(realpath "$1")
else
  programPath=""
fi

# Set the working directory to the script location
if [ -z "${BASH_SOURCE%/*}" ] ; then
  echo "BASH_SOURCE variable is not set!"
  echo "Will try to continue, but errors may occur"
fi
cd "$(realpath "${BASH_SOURCE%/*}")"


DEMO="demo"
RESULT="results"
TESTS="tests"


# === LOGGER === #

# Terminal constants
RESET="\033[0m"
BOLD="\033[1m"
RED="\033[31m"
GREEN="\033[32m"
YELLOW="\033[33m"
BLUE="\033[34m"
CYAN="\033[36m"
GREY="\033[37m"
CLEAR_LINE="\033[K"

# Pretty Symbols
OK="${GREEN}✓${RESET} "
ERROR="${RED}✗${RESET} "
PENDING="${CYAN}→${RESET} "
WARN="${YELLOW}⚠${RESET} "
PROMPT="${BLUE}?${RESET} "
INFO="${BLUE}i${RESET} "

clearBeforeWrite=0

function log_temp() {
  log_raw "$1"
  clearBeforeWrite=1
}

function log() {
  log_raw "$1"
  echo
}

function log_raw() {
  if [ "$clearBeforeWrite" = "1" ]; then
    echo -en "\r${CLEAR_LINE}"
    clearBeforeWrite=0
  fi

  echo -en "  $1"
}



# === TEMP FILE === #

TEMP_FILES=()

# Remove temp files on quit
function _cleanTempFiles {
  if [ ${#TEMP_FILES[@]} -ne 0 ]; then
    for file in ${TEMP_FILES[@]}; do
      rm -f $file
    done
  fi
}

trap _cleanTempFiles EXIT

# Create a temporary file, return path as global $RETURN
function tempFile() {
  RETURN=$(mktemp)
  TEMP_FILES+=("$RETURN")
}




# === TESTER === #

function runTests() {

  rm -rf "./$RESULTS"

  if ! [ -r "./$DEMO" ]; then
    log "${WARN} Demo program not found, comparison will be skipped"
    return
  fi

  local userProgram=$1

  if ! [ -r "$userProgram" ]; then
    log "${ERROR} Compiled user program missing. This is likely a bug."
    return
  fi

  # Locate all tests
  local tests=()
  for test in ./$TESTS/*; do
    if [ -r "$test" ]; then
      local tests+=("$test")
    fi
  done

  local nTests=${#tests[@]} # number of tests

  if [ $nTests -eq 0 ]; then
    log "${WARN} There are no tests to run"
    return
  fi

  if ! [ -d "./$RESULTS" ]; then
    mkdir "./$RESULTS"
  else
    rm -rf ./$RESULTS/*
  fi

  # Set time format for time command
  TIMEFORMAT="%3U"

  # Run the tests
  for i in ${!tests[@]}; do
    log_temp "${PENDING} Runnning test $i/$nTests";

    local test=${tests[$i]}
    local testName=$(basename "$test")
    local testDir="./$RESULTS/$testName"

    if ! [ -d "$testDir" ]; then
      mkdir "$testDir"
    else
      rm -rf $testDir/*
    fi

    local demoResult="$testDir/demo_output"
    local userResult="$testDir/user_output"

    # Run demo program
    if [ "$runDemo" = "1" ]; then
      ./$DEMO console $test > $demoResult
      sleep 0.5 # Allow CPU to "cool off"
    fi

    # Run user program
    $userProgram $test > $userResult

    sleep 0.5 # Allow CPU to "cool off"
  done

  log "${OK} $nTests tests completed"

}

runTests "$programPath"