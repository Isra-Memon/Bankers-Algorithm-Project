#!/bin/sh
set -eu
gcc -std=c11 -Wall -Wextra -pedantic bankers.c -o bankers
printf '6\n4\n7\n' | ./bankers > /tmp/bankers-safe.txt
grep -q 'SAFE STATE' /tmp/bankers-safe.txt
grep -q 'P1 -> P3 -> P4 -> P0 -> P2' /tmp/bankers-safe.txt
printf '6\n5\n1\n1\n0\n2\n7\n' | ./bankers > /tmp/bankers-grant.txt
grep -q 'GRANTED' /tmp/bankers-grant.txt
printf '6\n5\n1\n2\n0\n0\n7\n' | ./bankers > /tmp/bankers-deny.txt
grep -q 'DENIED: Request exceeds remaining need' /tmp/bankers-deny.txt
printf '6\n5\n0\n0\n0\n3\n7\n' | ./bankers > /tmp/bankers-wait.txt
grep -q 'WAIT: Insufficient available resources' /tmp/bankers-wait.txt
printf '0\n6\n4\n7\n' | ./bankers > /tmp/bankers-invalid.txt
grep -q 'Enter an integer from 1 to 7' /tmp/bankers-invalid.txt
echo 'All smoke tests passed.'
