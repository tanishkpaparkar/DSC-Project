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
/* ══════════════════════════════════════════════════════════════
 *  4. SEARCH ACCOUNT
 * ══════════════════════════════════════════════════════════════ */
void searchAccount() {
    printf("\n--- SEARCH ACCOUNT ---\n");

    int accNo;
    printf("Account Number : ");
    scanf("%d", &accNo);

    int idx = findAccount(accNo);
    if (idx == -1) { printf("Account not found!\n"); return; }

    if (!checkPassword(idx)) return;

    printf("\nAccount Number : %d\n",   accounts[idx].accNo);
    printf("Name           : %s\n",     accounts[idx].name);
    printf("Type           : %s\n",     accounts[idx].type);
    printf("Balance        : %.2f\n",   accounts[idx].balance);
}

/* ══════════════════════════════════════════════════════════════
 *  5. DISPLAY ALL ACCOUNTS
 * ══════════════════════════════════════════════════════════════ */
void displayAll() {
    printf("\n--- ALL ACCOUNTS ---\n");

    if (total == 0) { printf("No accounts found!\n"); return; }

    printf("%-10s %-20s %-10s %-12s\n", "Acc No", "Name", "Type", "Balance");
    printf("%-10s %-20s %-10s %-12s\n", "------", "----", "----", "-------");

    int i;
    for (i = 0; i < total; i++) {
        printf("%-10d %-20s %-10s %-12.2f\n",
               accounts[i].accNo, accounts[i].name,
               accounts[i].type,  accounts[i].balance);
    }
    printf("\nTotal Accounts: %d\n", total);
}

/* ══════════════════════════════════════════════════════════════
 *  6. DELETE ACCOUNT
 * ══════════════════════════════════════════════════════════════ */
void deleteAccount() {
    printf("\n--- DELETE ACCOUNT ---\n");

    int accNo;
    printf("Account Number : ");
    scanf("%d", &accNo);

    int idx = findAccount(accNo);
    if (idx == -1) { printf("Account not found!\n"); return; }

    if (!checkPassword(idx)) return;

    printf("Name    : %s\n", accounts[idx].name);
    printf("Balance : %.2f\n", accounts[idx].balance);
    printf("Confirm delete? (y/n): ");
    char ch;
    scanf(" %c", &ch);

    if (tolower(ch) != 'y') { printf("Deletion cancelled.\n"); return; }

    /* Shift records left to fill the gap */
    int i;
    for (i = idx; i < total - 1; i++)
        accounts[i] = accounts[i + 1];

    total--;
    saveFile();
    printf("Account deleted successfully!\n");
}
/* ══════════════════════════════════════════════════════════════
 *  7. CHANGE PASSWORD
 * ══════════════════════════════════════════════════════════════ */
void changePassword() {
    printf("\n--- CHANGE PASSWORD ---\n");

    int accNo;
    printf("Account Number   : ");
    scanf("%d", &accNo);

    int idx = findAccount(accNo);
    if (idx == -1) { printf("Account not found!\n"); return; }

    if (!checkPassword(idx)) return;

    char pwd1[20], pwd2[20];
    do {
        printf("New Password     : ");
        scanf("%s", pwd1);
        printf("Confirm Password : ");
        scanf("%s", pwd2);
        if (strcmp(pwd1, pwd2) != 0)
            printf("Passwords do not match! Try again.\n");
    } while (strcmp(pwd1, pwd2) != 0);

    strcpy(accounts[idx].password, pwd1);
    saveFile();
    printf("Password changed successfully!\n");
}

/* ══════════════════════════════════════════════════════════════
 *  MAIN MENU
 * ══════════════════════════════════════════════════════════════ */
void showMenu() {
    printf("\n=============================\n");
    printf("   BANK MANAGEMENT SYSTEM\n");
    printf("=============================\n");
    printf("1. Create Account\n");
    printf("2. Deposit Money\n");
    printf("3. Withdraw Money\n");
    printf("4. Search Account\n");
    printf("5. Display All Accounts\n");
    printf("6. Delete Account\n");
    printf("7. Change Password\n");
    printf("0. Exit\n");
    printf("=============================\n");
    printf("Enter choice: ");
}

int main() {
    loadFile();   /* Load saved accounts from file */

    int choice;
    do {
        showMenu();
        scanf("%d", &choice);

        switch (choice) {
        case 1: createAccount();  break;
        case 2: deposit();        break;
        case 3: withdraw();       break;
        case 4: searchAccount();  break;
        case 5: displayAll();     break;
        case 6: deleteAccount();  break;
        case 7: changePassword(); break;
        case 0: printf("Goodbye!\n"); break;
        default: printf("Invalid choice! Try again.\n");
        }

        if (choice != 0) {
            printf("\nPress ENTER to continue...");
            clearInput();
            getchar();
        }

    } while (choice != 0);

    return 0;
}
