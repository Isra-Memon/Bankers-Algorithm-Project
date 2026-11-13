# Banker's Algorithm — Deadlock Avoidance Simulator

A terminal-based C implementation of the Banker's safety and resource-request algorithms for an Operating Systems course project.

## Requirements
- Linux/Ubuntu, macOS or Windows with GCC
- GCC with C11 support

## Build and run
```bash
gcc -std=c11 -Wall -Wextra -pedantic bankers.c -o bankers
./bankers
```

## Menu
1. Enter process/resource data
2. Display Allocation, Maximum, Need and Available
3. Recalculate and display Need
4. Check safe state with step-by-step Work changes and safe sequence
5. Request additional resources with safety check and rollback
6. Load textbook sample
7. Exit

## Quick demonstration
Choose **6**, then **4**. Expected safe sequence: `P1 -> P3 -> P4 -> P0 -> P2`.

Choose **5**, process `1`, request `1 0 2`. The request should be granted.

See [TEST_CASES.md](TEST_CASES.md) for more cases.

## Algorithm
`Need = Maximum - Allocation`. The safety algorithm starts with `Work = Available` and repeatedly finds an unfinished process whose Need does not exceed Work. After it completes, its allocated resources are returned to Work. A safe state exists if every process can complete.

For a resource request, verify it does not exceed Need or Available, tentatively allocate, run the safety algorithm, and roll back if unsafe.

An **unsafe state is not necessarily a deadlock**.

## Team division
- Member 1: matrices, input, Need and validation
- Member 2: safety check, safe sequence and step-by-step output
- Member 3: resource requests, integration, testing and report

All members should understand the complete algorithm.

## Constraints
At most 10 processes and 10 resource types; resource quantities must be non-negative integers up to 1,000,000. The simulator does not allocate actual operating-system resources.

## Report outline
Title Page, Abstract, Table of Contents, Introduction, System Requirements, Implementation, Results and Discussion, References. Replace team names, CMS IDs and submission date before handing in.
