# Banker's Algorithm — Test Cases

Compile with `gcc -std=c11 -Wall -Wextra bankers.c -o bankers` and run `./bankers`.

## Safe state example
Processes: 5; resource types: 3.
Available: `3 3 2`

Allocation (P0–P4):
```
0 1 0
2 0 0
3 0 2
2 1 1
0 0 2
```
Maximum:
```
7 5 3
3 2 2
9 0 2
2 2 2
4 3 3
```
Expected Need:
```
7 4 3
1 2 2
6 0 0
0 1 1
4 3 1
```
Expected: SAFE; one valid sequence is P1 P3 P4 P0 P2.

## Resource requests
Using the safe state above:
- P1 requests `1 0 2`: should be GRANTED and leave a safe state.
- P0 requests `0 0 3`: should be rejected because it exceeds available C resources.
- P1 requests `2 0 0`: should be rejected because it exceeds P1's remaining A need.

## Validation cases to implement
- Zero or >10 process/resource counts.
- Negative Available, Allocation, Maximum or Request entries.
- Allocation greater than Maximum.
- Non-numeric input and missing values.
- Invalid process ID.

**Known issue:** The current program uses unchecked `scanf` and fixed-size arrays. Invalid input can lead to incorrect calculations or out-of-bounds access. Do not use untrusted input until validation is added.
