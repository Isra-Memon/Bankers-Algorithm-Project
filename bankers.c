/* ============================================================
 * bankers.c — Banker's Algorithm (Deadlock Avoidance)
 * Operating Systems Lab — CSC-251L
 *
 * Compile:  gcc -Wall -Wextra -o bankers bankers.c
 * Run:      ./bankers
 * ============================================================ */

#include <stdio.h>
#include <stdbool.h>

#define MAX_PROC 10
#define MAX_RES  10

int n;                          /* number of processes */
int m;                          /* number of resource types */

int Available[MAX_RES];
int Max[MAX_PROC][MAX_RES];
int Allocation[MAX_PROC][MAX_RES];
int Need[MAX_PROC][MAX_RES];

/* ------------------------------------------------------------
 * Need[i][j] = Max[i][j] - Allocation[i][j]
 * ------------------------------------------------------------ */
void calculateNeed(void) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            Need[i][j] = Max[i][j] - Allocation[i][j];
}

/* ------------------------------------------------------------
 * Print the current state as a table
 * ------------------------------------------------------------ */
void printState(void) {
    printf("\n%-8s %-18s %-18s %-18s\n",
           "Proc", "Allocation", "Max", "Need");
    for (int i = 0; i < n; i++) {
        printf("P%-7d ", i);
        for (int j = 0; j < m; j++) printf("%d ", Allocation[i][j]);
        printf("      ");
        for (int j = 0; j < m; j++) printf("%d ", Max[i][j]);
        printf("      ");
        for (int j = 0; j < m; j++) printf("%d ", Need[i][j]);
        printf("\n");
    }
    printf("\nAvailable: ");
    for (int j = 0; j < m; j++) printf("%d ", Available[j]);
    printf("\n");
}

/* ------------------------------------------------------------
 * Safety Algorithm
 * Returns true if system is in a SAFE state.
 * Fills safeSeq[] with the safe sequence (length n).
 * ------------------------------------------------------------ */
bool isSafe(int safeSeq[]) {
    int Work[MAX_RES];
    bool Finish[MAX_PROC] = {false};
    int count = 0;

    /* Step 1: Work = Available */
    for (int j = 0; j < m; j++)
        Work[j] = Available[j];

    /* Step 2: Find processes that can finish */
    while (count < n) {
        bool found = false;

        for (int i = 0; i < n; i++) {
            if (Finish[i]) continue;

            /* Check Need[i] <= Work for every resource */
            bool canRun = true;
            for (int j = 0; j < m; j++) {
                if (Need[i][j] > Work[j]) { canRun = false; break; }
            }

            if (canRun) {
                /* Process i can finish: release its resources */
                for (int j = 0; j < m; j++)
                    Work[j] += Allocation[i][j];

                safeSeq[count++] = i;
                Finish[i] = true;
                found = true;
            }
        }

        if (!found) break;  /* no process could proceed → unsafe */
    }

    return (count == n);
}

/* ------------------------------------------------------------
 * Resource Request Algorithm
 * pid       — process index (0-based)
 * Request[] — requested resources
 * Returns true if granted, false if denied/wait
 * ------------------------------------------------------------ */
bool requestResources(int pid, int Request[]) {

    /* Step 1: Request must not exceed declared Need */
    for (int j = 0; j < m; j++) {
        if (Request[j] > Need[pid][j]) {
            printf("\nERROR: P%d requested more than its Max claim.\n", pid);
            return false;
        }
    }

    /* Step 2: Request must not exceed Available */
    for (int j = 0; j < m; j++) {
        if (Request[j] > Available[j]) {
            printf("\nP%d must WAIT — resources not currently available.\n", pid);
            return false;
        }
    }

    /* Step 3: Pretend to allocate */
    for (int j = 0; j < m; j++) {
        Available[j]        -= Request[j];
        Allocation[pid][j]  += Request[j];
        Need[pid][j]        -= Request[j];
    }

    /* Step 4: Check safety after pretend allocation */
    int seq[MAX_PROC];
    if (isSafe(seq)) {
        printf("\nRequest GRANTED to P%d.\nSafe sequence: ", pid);
        for (int i = 0; i < n; i++) printf("P%d ", seq[i]);
        printf("\n");
        return true;
    } else {
        /* ROLLBACK */
        for (int j = 0; j < m; j++) {
            Available[j]        += Request[j];
            Allocation[pid][j]  -= Request[j];
            Need[pid][j]        += Request[j];
        }
        printf("\nRequest DENIED — system would become UNSAFE.\n");
        printf("P%d must wait. State has been rolled back.\n", pid);
        return false;
    }
}

/* ------------------------------------------------------------
 * Input helper
 * ------------------------------------------------------------ */
void inputData(void) {
    printf("Enter number of processes (max %d): ", MAX_PROC);
    scanf("%d", &n);
    printf("Enter number of resource types (max %d): ", MAX_RES);
    scanf("%d", &m);

    printf("\nEnter Available resources (%d values):\n", m);
    for (int j = 0; j < m; j++) {
        printf("  Available[%d]: ", j);
        scanf("%d", &Available[j]);
    }

    printf("\nEnter Allocation matrix (%d x %d):\n", n, m);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++) {
            printf("  Allocation[P%d][%d]: ", i, j);
            scanf("%d", &Allocation[i][j]);
        }

    printf("\nEnter Max matrix (%d x %d):\n", n, m);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++) {
            printf("  Max[P%d][%d]: ", i, j);
            scanf("%d", &Max[i][j]);
        }

    calculateNeed();
}

/* ------------------------------------------------------------
 * Main — menu loop
 * ------------------------------------------------------------ */
int main(void) {
    int choice;

    printf("========== BANKER'S ALGORITHM ==========\n");
    inputData();

    while (1) {
        printf("\n========== MENU ==========\n");
        printf("1. Print current state\n");
        printf("2. Check if system is SAFE\n");
        printf("3. Request resources for a process\n");
        printf("4. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {

        case 1:
            printState();
            break;

        case 2: {
            int seq[MAX_PROC];
            if (isSafe(seq)) {
                printf("\nSystem is in a SAFE state.\nSafe sequence: ");
                for (int i = 0; i < n; i++) printf("P%d ", seq[i]);
                printf("\n");
            } else {
                printf("\nSystem is in an UNSAFE state — deadlock possible.\n");
            }
            break;
        }

        case 3: {
            int pid;
            int Request[MAX_RES];

            printf("Enter process number (0 to %d): ", n - 1);
            scanf("%d", &pid);

            if (pid < 0 || pid >= n) {
                printf("Invalid process number.\n");
                break;
            }

            printf("Enter request for P%d (%d values):\n", pid, m);
            for (int j = 0; j < m; j++) {
                printf("  Request[%d]: ", j);
                scanf("%d", &Request[j]);
            }

            requestResources(pid, Request);
            break;
        }

        case 4:
            printf("Exiting.\n");
            return 0;

        default:
            printf("Invalid choice.\n");
        }
    }

    return 0;
}
