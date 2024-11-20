#!/bin/bash

# Directory containing test files
TEST_DIR="./tests"
# Path to the program executable
PROGRAM="./program"

# Color Codes
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# File to store test results
RESULT_FILE="test_results.log"
> "$RESULT_FILE"

# Summary counters
total=0
passed=0
failed=0

# Arrays to store test details
declare -a test_names
declare -a test_expected
declare -a test_actual
declare -a test_results

echo -e "${YELLOW}Starting Integration Tests...${NC}" | tee "$RESULT_FILE"

# Check if TEST_DIR exists and has .ifj files
if [ ! -d "$TEST_DIR" ]; then
    echo -e "${RED}Error: Test directory '$TEST_DIR' does not exist.${NC}" | tee -a "$RESULT_FILE"
    exit 1
fi

shopt -s nullglob
test_files=("$TEST_DIR"/*.ifj)
if [ ${#test_files[@]} -eq 0 ]; then
    echo -e "${RED}No test files found in '$TEST_DIR'.${NC}" | tee -a "$RESULT_FILE"
    exit 1
fi
shopt -u nullglob

# Iterate over each test case
for test_file in "$TEST_DIR"/*.ifj; do
    ((total++))
    echo -e "${YELLOW}Running Test: $test_file${NC}" | tee -a "$RESULT_FILE"

    # Extract expected error code from filename (e.g., test_parse_error_1.ifj expects exit code 1)
    filename=$(basename "$test_file")
    
    # Use regex to extract the expected error code
    if [[ "$filename" =~ _([0-9]+)\.ifj$ ]]; then
        expected_code="${BASH_REMATCH[1]}"
    else
        expected_code=0
    fi

    # Run the program with the test file
    "$PROGRAM" "$test_file"
    actual_code=$?

    # Determine pass/fail
    if [ "$actual_code" -eq "$expected_code" ]; then
        result="${GREEN}✅ Passed${NC}"
        ((passed++))
        test_results+=("Passed")
    else
        result="${RED}❌ Failed${NC}"
        ((failed++))
        test_results+=("Failed")
    fi

    # Store test details
    test_names+=("$filename")
    test_expected+=("$expected_code")
    test_actual+=("$actual_code")

    # Print test result
    echo -e "${result} (Expected: $expected_code, Got: $actual_code)" | tee -a "$RESULT_FILE"
    echo -e "${YELLOW}----------------------------------------${NC}" | tee -a "$RESULT_FILE"
done

echo -e "\n${YELLOW}Detailed Test Results:${NC}" | tee -a "$RESULT_FILE"

# Table Header with increased Test File column width
printf "${NC}+--------------------------------+----------+----------+---------+${NC}\n" | tee -a "$RESULT_FILE"
printf "${NC}| %-40s | %-8s | %-8s | %-7s |${NC}\n" "Test File" "Expected" "Actual" "Result" | tee -a "$RESULT_FILE"
printf "${NC}+--------------------------------+----------+----------+---------+${NC}\n" | tee -a "$RESULT_FILE"

# Table Rows with updated formatting
for ((i=0; i<total; i++)); do
    # Determine color based on result
    if [ "${test_results[i]}" == "Passed" ]; then
        color="${GREEN}"
    else
        color="${RED}"
    fi

    printf "${NC}| %-40s | %-8s | %-8s | ${color}%-7s${NC} |${NC}\n" \
        "${test_names[i]}" "${test_expected[i]}" "${test_actual[i]}" "${test_results[i]}" | tee -a "$RESULT_FILE"
done

# Table Footer with increased Test File column width
printf "${NC}+--------------------------------+----------+----------+---------+${NC}\n" | tee -a "$RESULT_FILE"

# Final Summary Table
echo -e "\n${YELLOW}Final Test Summary:${NC}" | tee -a "$RESULT_FILE"
printf "${YELLOW}+----------------+--------+${NC}\n" | tee -a "$RESULT_FILE"
printf "${YELLOW}| %-12-s | %-6s |${NC}\n" "Metric" "Count" | tee -a "$RESULT_FILE"
printf "${YELLOW}+----------------+--------+${NC}\n" | tee -a "$RESULT_FILE"
printf "${YELLOW}| %-14s | %-6d |${NC}\n" "Total" "$total" | tee -a "$RESULT_FILE"
printf "${YELLOW}| %-14s | ${GREEN}%-6d${NC} |${NC}\n" "Passed" "$passed" | tee -a "$RESULT_FILE"
printf "${YELLOW}| %-14s | ${RED}%-6d${NC} |${NC}\n" "Failed" "$failed" | tee -a "$RESULT_FILE"
printf "${YELLOW}+----------------+--------+${NC}\n" | tee -a "$RESULT_FILE"

# Exit with appropriate status
if [ "$failed" -ne 0 ]; then
    exit 1
else
    exit 0
fi