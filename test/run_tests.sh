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


DEMO="./demo"
RESULTS="results"
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



# === TESTER === #

function runTests() {

  rm -rf "./$RESULTS"

  if ! [ -r "./$DEMO" ]; then
    log "${WARN} Demo program not found, comparison will be skipped"
    return
  fi

  local userProgram=$1

  if ! [ -r "$userProgram" ]; then
    log "${ERROR} Missing the user program."
    log "${INFO} Usage: script.sh path/to/program"
    return
  fi

  chmod +x "$userProgram"
  chmod +x "$DEMO"

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


    # Workaround: piping hides segfault message from terminal
    # Run demo program
    $DEMO console $test > $demoResult 2> /dev/null | cat

    # Run user program
    $userProgram $test > $userResult 2> /dev/null | cat

  done

  log "${OK} $nTests tests completed"

}



# === RESULTS === #

# Analyse the user program's performance
function anaylsePerformance() {

  # Locate all results
  local results=()
  for result in ./${RESULTS}/* ; do
    if [ -d "$result" ]; then
      local results+=("$result")
    fi
  done

  if [ ${#results[@]} -eq 0 ] || [ ${#results[@]} -eq 1 ] && ! [ -d ${#results[0]} ]; then
    return
  fi

  # Collect point scoring
  local points=0
  local total=0

  # Print helpful information about problems
  local printIncorrect=0

  # Table header
  echo
  log "   ${BOLD}-- Performance Analysis --${RESET}\n"
  printf "  ${BOLD}%12s  %12s${RESET}\n" "Test" "Output";

  for i in ${!results[@]}; do
    local result=${results[$i]}

    # Compare for exact results
    cmp --silent "$result/demo_output" "$result/user_output"
    if [ $? -eq 0 ]; then
      local comparison="Correct"
      local comparisonColour="${GREEN}"
      ((points+=3))
      ((total+=3))
    else
      local comparison="Incorrect"
      local comparisonColour="$RED$BOLD"
      local printIncorrect=1
      ((total+=3))
    fi

    local testName=$(basename "$result")
    printf "  %12s %b %12s %b\n" ${testName:0:12} "$comparisonColour" $comparison "$RESET"

  done

  # Print total score
  local percentage=$(echo "$points / $total * 100" | bc -l | awk '{print int($1)}')
  echo
  log "Program score is ${BOLD}${points}/${total}${RESET} (${percentage}%) points\n"


  # Show that tests failed
  if [ "$points" -ne "$total" ]; then
    exit 1
  fi

}


runTests "$programPath"
anaylsePerformance
