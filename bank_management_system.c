/*
 * ============================================================
 *   BANK MANAGEMENT SYSTEM
 *   Language  : C
 *   Concepts  : Structures, File Handling, Functions
 * ============================================================
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX 100
#define FILE_NAME "accounts.dat"

/* ── Structure ─────────────────────────────────────────────── */
struct Account {
    int   accNo;
    char  name[50];
    char  type[20];     /* Savings / Current */
    float balance;
    char  password[20];
};

/* ── Global array and count ────────────────────────────────── */
struct Account accounts[MAX];
int total = 0;

/* ── Clear leftover characters from input buffer ───────────── */
void clearInput() {
    while (getchar() != '\n');
}

/* ── Load all accounts from file into array ────────────────── */
void loadFile() {
    FILE *fp = fopen(FILE_NAME, "rb");
    if (fp == NULL) return;           /* No file yet – first run */

    total = 0;
    while (fread(&accounts[total], sizeof(struct Account), 1, fp) == 1) {
        total++;
        if (total >= MAX) break;
    }
    fclose(fp);
}

/* ── Save entire array back to file ────────────────────────── */
void saveFile() {
    FILE *fp = fopen(FILE_NAME, "wb");
    if (fp == NULL) {
        printf("ERROR: Could not save data!\n");
        return;
    }
    fwrite(accounts, sizeof(struct Account), total, fp);
    fclose(fp);
}

/* ── Return index of account, or -1 if not found ───────────── */
int findAccount(int accNo) {
    int i;
    for (i = 0; i < total; i++) {
        if (accounts[i].accNo == accNo)
            return i;
    }
    return -1;
}

/* ── Ask for password and verify ───────────────────────────── */
int checkPassword(int idx) {
    char pwd[20];
    printf("Enter Password : ");
    scanf("%s", pwd);
    if (strcmp(pwd, accounts[idx].password) == 0)
        return 1;
    printf("Wrong password! Access denied.\n");
    return 0;
}

/* ══════════════════════════════════════════════════════════════
 *  1. CREATE ACCOUNT
 * ══════════════════════════════════════════════════════════════ */
void createAccount() {
    printf("\n--- CREATE ACCOUNT ---\n");

    if (total >= MAX) {
        printf("Account limit reached!\n");
        return;
    }

    struct Account a;
    int ch;
    char pwd1[20], pwd2[20];

    printf("Account Number  : ");
    scanf("%d", &a.accNo);
    clearInput();

    if (findAccount(a.accNo) != -1) {
        printf("Account number already exists!\n");
        return;
    }

    printf("Account Holder  : ");
    fgets(a.name, sizeof(a.name), stdin);
    a.name[strcspn(a.name, "\n")] = '\0';   /* Remove newline */

    printf("Account Type\n");
    printf("  1. Savings\n");
    printf("  2. Current\n");
    printf("Your Choice     : ");
    scanf("%d", &ch);
    if (ch == 2) strcpy(a.type, "Current");
    else         strcpy(a.type, "Savings");

    printf("Initial Deposit : ");
    scanf("%f", &a.balance);

    if (a.balance < 0) {
        printf("Balance cannot be negative!\n");
        return;
    }

    /* Set password with confirmation */
    do {
        printf("Set Password    : ");
        scanf("%s", pwd1);
        printf("Confirm Password: ");
        scanf("%s", pwd2);
        if (strcmp(pwd1, pwd2) != 0)
            printf("Passwords do not match! Try again.\n");
    } while (strcmp(pwd1, pwd2) != 0);

    strcpy(a.password, pwd1);
    accounts[total++] = a;
    saveFile();

    printf("\nAccount created successfully!\n");
    printf("Account No : %d\n", a.accNo);
    printf("Name       : %s\n", a.name);
    printf("Type       : %s\n", a.type);
    printf("Balance    : %.2f\n", a.balance);
}

/* ══════════════════════════════════════════════════════════════
 *  2. DEPOSIT
 * ══════════════════════════════════════════════════════════════ */
void deposit() {
    printf("\n--- DEPOSIT MONEY ---\n");

    int accNo;
    printf("Account Number : ");
    scanf("%d", &accNo);

    int idx = findAccount(accNo);
    if (idx == -1) { printf("Account not found!\n"); return; }

    if (!checkPassword(idx)) return;

    float amount;
    printf("Amount         : ");
    scanf("%f", &amount);

    if (amount <= 0) { printf("Amount must be greater than zero!\n"); return; }

    accounts[idx].balance += amount;
    saveFile();

    printf("Deposit successful!\n");
    printf("New Balance : %.2f\n", accounts[idx].balance);
}

/* ══════════════════════════════════════════════════════════════
 *  3. WITHDRAW
 * ══════════════════════════════════════════════════════════════ */
void withdraw() {
    printf("\n--- WITHDRAW MONEY ---\n");

    int accNo;
    printf("Account Number : ");
    scanf("%d", &accNo);

    int idx = findAccount(accNo);
    if (idx == -1) { printf("Account not found!\n"); return; }

    if (!checkPassword(idx)) return;

    float amount;
    printf("Amount         : ");
    scanf("%f", &amount);

    if (amount <= 0) { printf("Amount must be greater than zero!\n"); return; }
    if (amount > accounts[idx].balance) {
        printf("Insufficient balance! Available: %.2f\n", accounts[idx].balance);
        return;
    }

    accounts[idx].balance -= amount;
    saveFile();

    printf("Withdrawal successful!\n");
    printf("New Balance : %.2f\n", accounts[idx].balance);
}
