#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "backend.h"

#define max_user 1000
#define max_accounts 1000

// ------------------------------------------- STRUCTURES -------------------------------------------------------
// Note: 'account' struct is now defined in the header file
//       so we don't need to define it here.

account *accounts[max_accounts];
int accountcount = 0;

typedef struct
{
    int id;
    char username[50];
    char password[50];
} user;

user users[max_user];
int usercount = 0;

// ------------------------------------------- BLOCKCHAIN STRUCTURES --------------------------------------------

// --- FIX APPLIED HERE: Changed from 5 to 1 for instant mining ---
#define BLOCK_CAP 1  
#define MAX_PENDING 1000
#define HASH_STR_LEN 65

typedef struct Transaction {
    int txID;
    int fromAcc;
    int toAcc;
    float amount;
    char remark[100];
    char timestamp[30];
} Transaction;

typedef struct Block {
    int index;
    char timestamp[30];
    int transactionCount;
    Transaction transactions[BLOCK_CAP];
    char previousHash[HASH_STR_LEN];
    char currHash[HASH_STR_LEN];
    struct Block* next;
} Block;

Block* blockchainHead = NULL;
Block* blockchainTail = NULL;
int blockCount = 0;

Transaction pendingPool[MAX_PENDING];
int pendingCount = 0;
int nextTxID = 1;

// ------------------------------------------- (INTERNAL) HELPER FUNCTIONS ----------------------------------------
// These functions are "static" meaning they are private to this file
// and not exposed in the header.

static void saveuserstofile()
{
    FILE *fp = fopen("users.dat", "wb");
    if (fp == NULL)
    {
        // In a real GUI app, you'd log this error.
        return;
    }
    fwrite(&usercount, sizeof(int), 1, fp);
    fwrite(users, sizeof(user), usercount, fp);
    fclose(fp);
}

static void loadusersfromfile()
{
    FILE *fp = fopen("users.dat", "rb");
    if (fp == NULL)
    {
        return;
    }
    fread(&usercount, sizeof(int), 1, fp);
    fread(users, sizeof(user), usercount, fp);
    fclose(fp);
}

static void saveaccountstofile()
{
    FILE *fp = fopen("accounts.dat", "wb");
    if (fp == NULL)
    {
        return;
    }
    fwrite(&accountcount, sizeof(int), 1, fp);
    for (int i = 0; i < accountcount; i++)
    {
        fwrite(accounts[i], sizeof(account), 1, fp);
    }
    fclose(fp);
}

static void loadaccountsfromfile()
{
    FILE *fp = fopen("accounts.dat", "rb");
    if (fp == NULL)
    {
        return;
    }
    fread(&accountcount, sizeof(int), 1, fp);
    for (int i = 0; i < accountcount; i++)
    {
        account *acc = (account *)malloc(sizeof(account));
        if (acc) {
            fread(acc, sizeof(account), 1, fp);
            accounts[i] = acc;
        }
    }
    fclose(fp);
}

static account *findaccount(int id)
{
    for (int i = 0; i < accountcount; i++)
    {
        if (accounts[i]->accID == id)
        {
            return accounts[i];
        }
    }
    return NULL;
}

static unsigned long djb2_hash(const char* str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + (unsigned char)c;
    return hash;
}

static void compute_hash_for_block(Block* blk, char out[HASH_STR_LEN]) {
    char buf[4096];
    buf[0] = 0;
    sprintf(buf + strlen(buf), "%d|%s|%s|", blk->index, blk->timestamp, blk->previousHash);
    for (int i = 0; i < blk->transactionCount; i++) {
        Transaction *t = &blk->transactions[i];
        sprintf(buf + strlen(buf), "%d:%d->%d:%.2f:%s|", t->txID, t->fromAcc, t->toAcc, t->amount, t->timestamp);
    }
    unsigned long h = djb2_hash(buf);
    sprintf(out, "%lx", h);
}

static void createGenesisBlock() {
    // Only create if one doesn't exist (e.g., on first-ever run)
    if (blockchainHead != NULL) return;

    Block* genesis = (Block*)malloc(sizeof(Block));
    if (!genesis) return;
    genesis->index = 0;
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(genesis->timestamp, sizeof(genesis->timestamp), "%Y-%m-%d %H:%M:%S", tm_info);
    genesis->transactionCount = 0;
    strcpy(genesis->previousHash, "0");
    compute_hash_for_block(genesis, genesis->currHash);
    genesis->next = NULL;
    blockchainHead = blockchainTail = genesis;
    blockCount = 1;
}

static void addBlockFromPending() {
    if (pendingCount == 0) return;

    Block* blk = (Block*)malloc(sizeof(Block));
    if (!blk) return;

    blk->index = blockCount;
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(blk->timestamp, sizeof(blk->timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

    int take = (pendingCount < BLOCK_CAP) ? pendingCount : BLOCK_CAP;
    blk->transactionCount = take;
    for (int i = 0; i < take; i++) {
        blk->transactions[i] = pendingPool[i];
    }
    for (int i = take; i < pendingCount; i++) {
        pendingPool[i - take] = pendingPool[i];
    }
    pendingCount -= take;

    if (blockchainTail != NULL) {
        strcpy(blk->previousHash, blockchainTail->currHash);
    } else {
        strcpy(blk->previousHash, "0");
    }

    compute_hash_for_block(blk, blk->currHash);

    blk->next = NULL;
    if (blockchainTail) {
        blockchainTail->next = blk;
        blockchainTail = blk;
    } else {
        blockchainHead = blockchainTail = blk;
    }
    blockCount++;
}

static void recordTransaction(int fromAcc, int toAcc, float amount, const char* remark) {
    if (pendingCount >= MAX_PENDING) {
        return;
    }
    Transaction t;
    t.txID = nextTxID++;
    t.fromAcc = fromAcc;
    t.toAcc = toAcc;
    t.amount = amount;
    strncpy(t.remark, remark, sizeof(t.remark)-1);
    t.remark[sizeof(t.remark)-1] = 0;

    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(t.timestamp, sizeof(t.timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

    pendingPool[pendingCount++] = t;

    if (pendingCount >= BLOCK_CAP) {
        addBlockFromPending();
    }
}

// ------------------------------------------- PUBLIC API FUNCTIONS -----------------------------------------------
// These functions implement the prototypes from backend.h

void initialize_system()
{
    loadaccountsfromfile();
    loadusersfromfile();
    // We create genesis block only if no chain is loaded (which we assume if head is NULL)
    // A more robust system would load/save the chain from a file.
    createGenesisBlock();
}

void shutdown_system()
{
    saveaccountstofile();
    saveuserstofile();

    // free account memory
    for (int i = 0; i < accountcount; i++) {
        free(accounts[i]);
        accounts[i] = NULL;
    }

    // free blockchain memory
    Block* cur = blockchainHead;
    while (cur != NULL) {
        Block* next = cur->next;
        free(cur);
        cur = next;
    }
    blockchainHead = blockchainTail = NULL;
}

int perform_login(int accid, const char* username, const char* password)
{
    if (!username || !password) return 0; // Safety check

    for (int i = 0; i < usercount; i++)
    {
        if (users[i].id == accid &&
            strcmp(users[i].username, username) == 0 &&
            strcmp(users[i].password, password) == 0)
        {
            return 1; // 1 = Success
        }
    }
    return 0; // 0 = Failure
}

int perform_register(int id, const char* username, const char* password)
{
    if (usercount >= max_user)
    {
        return 1; // 1 = User limit reached
    }
    if (!username || !password) return 2; // Invalid input

    user newUser;
    newUser.id = id;
    strncpy(newUser.username, username, sizeof(newUser.username) - 1);
    newUser.username[sizeof(newUser.username) - 1] = 0;

    strncpy(newUser.password, password, sizeof(newUser.password) - 1);
    newUser.password[sizeof(newUser.password) - 1] = 0;

    users[usercount++] = newUser;
    return 0; // 0 = Success
}

int perform_create_account(int id, const char* name, const char* phno, float balance)
{
    if (accountcount >= max_accounts)
    {
        return 1; // 1 = Account limit reached
    }
    if (findaccount(id) != NULL) {
        return 3; // 3 = Account ID already exists
    }

    account *newacc = (account *)malloc(sizeof(account));
    if (newacc == NULL)
    {
        return 2; // 2 = Memory allocation failed
    }

    newacc->accID = id;
    strncpy(newacc->name, name, sizeof(newacc->name) - 1);
    newacc->name[sizeof(newacc->name) - 1] = 0;

    strncpy(newacc->phno, phno, sizeof(newacc->phno) - 1);
    newacc->phno[sizeof(newacc->phno) - 1] = 0;

    newacc->balance = balance;
    accounts[accountcount++] = newacc;

    return 0; // 0 = Success
}

int perform_update_account_name(int id, const char* newName)
{
    account *acc = findaccount(id);
    if (acc == NULL)
    {
        return 1; // 1 = Not found
    }
    strncpy(acc->name, newName, sizeof(acc->name) - 1);
    acc->name[sizeof(acc->name) - 1] = 0;
    return 0; // 0 = Success
}

int perform_update_account_phone(int id, const char* newPhone)
{
    account *acc = findaccount(id);
    if (acc == NULL)
    {
        return 1; // 1 = Not found
    }
    strncpy(acc->phno, newPhone, sizeof(acc->phno) - 1);
    acc->phno[sizeof(acc->phno) - 1] = 0;
    return 0; // 0 = Success
}

int perform_update_account_balance(int id, float newBalance)
{
    account *acc = findaccount(id);
    if (acc == NULL)
    {
        return 1; // 1 = Not found
    }
    acc->balance = newBalance;
    return 0; // 0 = Success
}

int perform_delete_account(int id)
{
    int found = 0;
    for (int i = 0; i < accountcount; i++)
    {
        if (accounts[i]->accID == id)
        {
            found = 1;
            free(accounts[i]);
            // Shift remaining elements down
            for (int j = i; j < accountcount - 1; j++)
                accounts[j] = accounts[j + 1];
            accountcount--;
            accounts[accountcount] = NULL; // Clear last (now duplicate) pointer
            break;
        }
    }
    if (!found)
        return 1; // 1 = Not found

    return 0; // 0 = Success
}

int get_account_details(int id, account* acc_out)
{
    if (!acc_out) return 1; // Bad output pointer

    account *acc = findaccount(id);
    if (acc == NULL)
    {
        return 1; // 1 = Not found
    }

    // Copy data to the output struct
    memcpy(acc_out, acc, sizeof(account));
    return 0; // 0 = Success
}

// For string-building, we use a large static buffer.
// This is simple, but not thread-safe. Good enough for this project.
#define MAX_BUFFER_SIZE 16384
static char g_display_buffer[MAX_BUFFER_SIZE];

const char* get_all_accounts_summary()
{
    if (accountcount <= 0)
    {
        return "No accounts found!\n";
    }

    // Clear the buffer
    g_display_buffer[0] = 0;

    char line[256];
    snprintf(line, sizeof(line), "\n--- Accounts (%d) ---\n", accountcount);
    strncat(g_display_buffer, line, MAX_BUFFER_SIZE - strlen(g_display_buffer) - 1);

    for (int i = 0; i < accountcount; i++)
    {
        snprintf(line, sizeof(line), "ID: %d, Name: %s, Phone: %s, Balance: $%.2f\n",
               accounts[i]->accID, accounts[i]->name, accounts[i]->phno, accounts[i]->balance);

        if (strlen(g_display_buffer) + strlen(line) + 1 >= MAX_BUFFER_SIZE) {
            // Stop if buffer is full
            strncat(g_display_buffer, "... (buffer full) ...\n", MAX_BUFFER_SIZE - strlen(g_display_buffer) - 1);
            break;
        }
        strncat(g_display_buffer, line, MAX_BUFFER_SIZE - strlen(g_display_buffer) - 1);
    }
    return g_display_buffer;
}

int perform_deposit(int id, float amount)
{
    account *acc = findaccount(id);
    if (acc == NULL)
    {
        return 1; // 1 = Account not found
    }
    if (amount <= 0)
    {
        return 2; // 2 = Invalid amount
    }
    acc->balance += amount;

    char remark[100];
    snprintf(remark, sizeof(remark), "Deposit by %s", acc->name);
    recordTransaction(0, acc->accID, amount, remark);

    return 0; // 0 = Success
}

int perform_withdraw(int id, float amount)
{
    account *acc = findaccount(id);
    if (acc == NULL)
    {
        return 1; // 1 = Account not found
    }
    if (amount <= 0)
    {
        return 2; // 2 = Invalid amount
    }
    if (amount > acc->balance)
    {
        return 3; // 3 = Insufficient funds
    }
    acc->balance -= amount;

    char remark[100];
    snprintf(remark, sizeof(remark), "Withdrawal by %s", acc->name);
    recordTransaction(acc->accID, 0, amount, remark);

    return 0; // 0 = Success
}

int perform_transfer(int fromID, int toID, float amount)
{
    account *from = findaccount(fromID);
    if (from == NULL)
    {
        return 1; // 1 = Sender not found
    }
    account *to = findaccount(toID);
    if (to == NULL)
    {
        return 2; // 2 = Receiver not found
    }
    if (amount <= 0 || amount > from->balance)
    {
        return 3; // 3 = Invalid amount or insufficient funds
    }
    from->balance -= amount;
    to->balance += amount;

    char remark[100];
    snprintf(remark, sizeof(remark), "Transfer %d->%d", fromID, toID);
    recordTransaction(fromID, toID, amount, remark);

    return 0; // 0 = Success
}

const char* get_blockchain_string()
{
    if (blockchainHead == NULL) {
        return "Blockchain is empty.\n";
    }

    g_display_buffer[0] = 0; // Clear buffer
    char line[512];

    Block* cur = blockchainHead;
    while (cur != NULL) {
        snprintf(line, sizeof(line), "\n--- Block %d ---\n", cur->index);
        strncat(g_display_buffer, line, MAX_BUFFER_SIZE - strlen(g_display_buffer) - 1);

        snprintf(line, sizeof(line), "Timestamp     : %s\n", cur->timestamp);
        strncat(g_display_buffer, line, MAX_BUFFER_SIZE - strlen(g_display_buffer) - 1);

        snprintf(line, sizeof(line), "Previous Hash : %s\n", cur->previousHash);
        strncat(g_display_buffer, line, MAX_BUFFER_SIZE - strlen(g_display_buffer) - 1);

        snprintf(line, sizeof(line), "Current Hash  : %s\n", cur->currHash);
        strncat(g_display_buffer, line, MAX_BUFFER_SIZE - strlen(g_display_buffer) - 1);

        snprintf(line, sizeof(line), "Transactions (%d):\n", cur->transactionCount);
        strncat(g_display_buffer, line, MAX_BUFFER_SIZE - strlen(g_display_buffer) - 1);

        for (int i = 0; i < cur->transactionCount; i++) {
            Transaction *t = &cur->transactions[i];
            snprintf(line, sizeof(line), "  TX %d | %d -> %d | %.2f | %s | %s\n",
                   t->txID, t->fromAcc, t->toAcc, t->amount, t->remark, t->timestamp);
            strncat(g_display_buffer, line, MAX_BUFFER_SIZE - strlen(g_display_buffer) - 1);
        }

        if (strlen(g_display_buffer) > MAX_BUFFER_SIZE - 1024) {
             strncat(g_display_buffer, "... (buffer full) ...\n", MAX_BUFFER_SIZE - strlen(g_display_buffer) - 1);
             break; // Stop if buffer is getting full
        }
        cur = cur->next;
    }

    if (pendingCount > 0) {
        snprintf(line, sizeof(line), "\n--- Pending Transactions (%d) ---\n", pendingCount);
        strncat(g_display_buffer, line, MAX_BUFFER_SIZE - strlen(g_display_buffer) - 1);

        for (int i = 0; i < pendingCount; i++) {
            Transaction *t = &pendingPool[i];
            snprintf(line, sizeof(line), "  (P) TX %d | %d -> %d | %.2f | %s | %s\n",
                   t->txID, t->fromAcc, t->toAcc, t->amount, t->remark, t->timestamp);
            strncat(g_display_buffer, line, MAX_BUFFER_SIZE - strlen(g_display_buffer) - 1);
        }
    }
    return g_display_buffer;
}

int perform_validate_chain() {
    if (blockchainHead == NULL) return 1; // Empty chain is valid

    Block* cur = blockchainHead;
    while (cur->next != NULL) {
        Block* nxt = cur->next;
        // Check hash linkage
        if (strcmp(nxt->previousHash, cur->currHash) != 0) {
            return 0; // Chain broken
        }

        // Recompute and check current block's hash
        char recomputed[HASH_STR_LEN];
        compute_hash_for_block(cur, recomputed);
        if (strcmp(recomputed, cur->currHash) != 0) {
            return 0; // Data tampered
        }
        cur = cur->next;
    }

    // Check the last block's hash
    char recomputedLast[HASH_STR_LEN];
    compute_hash_for_block(cur, recomputedLast);
    if (strcmp(recomputedLast, cur->currHash) != 0) {
        return 0; // Last block tampered
    }

    return 1; // 1 = Valid
}