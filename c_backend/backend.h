

#ifndef PBL_BACKEND_H
#define PBL_BACKEND_H

// ------------------------------------------- STRUCTURES -------------------------------------------------------
// We expose the 'account' struct so the GUI can request details.
typedef struct account
{
    int accID;
    char name[50];
    char phno[11]; // fixed: 10 digits + null terminator
    float balance;
} account;


// This 'extern "C"' block is ESSENTIAL.
// It tells the C++ compiler to treat these as C functions,
// which prevents C++ "name mangling" and allows linking.
#ifdef __cplusplus
extern "C" {
#endif

// --- System Functions ---

/**
 * @brief Initializes the backend system.
 * Loads users, loads accounts, and creates the genesis block.
 * Must be called once when the GUI application starts.
 */
void initialize_system();

/**
 * @brief Shuts down the backend system.
 * Saves all data to files and frees memory.
 * Must be called once when the GUI application exits.
 */
void shutdown_system();


// --- Auth Functions ---

/**
 * @brief Attempts to log in a user.
 * @param accid The user's account ID.
 * @param username The user's username.
 * @param password The user's password.
 * @return 1 on success (credentials match), 0 on failure.
 */
int perform_login(int accid, const char* username, const char* password);

/**
 * @brief Registers a new user.
 * @param id The new user's ID.
 * @param username The new user's username.
 * @param password The new user's password.
 * @return 0 on success.
 * @return 1 if user limit is reached.
 */
int perform_register(int id, const char* username, const char* password);


// --- Account Management Functions ---

/**
 * @brief Creates a new bank account.
 * @param id The new account ID.
 * @param name The account holder's name.
 * @param phno The account holder's phone number.
 * @param balance The initial balance.
 * @return 0 on success.
 * @return 1 if account limit is reached.
 * @return 2 if memory allocation fails.
 * @return 3 if an account with this ID already exists.
 */
int perform_create_account(int id, const char* name, const char* phno, float balance);

/**
 * @brief Updates the name for a given account.
 * @param id The account ID to update.
 * @param newName The new name.
 * @return 0 on success, 1 if account not found.
 */
int perform_update_account_name(int id, const char* newName);

/**
 * @brief Updates the phone number for a given account.
 * @param id The account ID to update.
 * @param newPhone The new phone number.
 * @return 0 on success, 1 if account not found.
 */
int perform_update_account_phone(int id, const char* newPhone);

/**
 * @brief Updates the balance for a given account.
 * (Note: This is a direct edit. Use deposit/withdraw for transactions).
 * @param id The account ID to update.
 * @param newBalance The new balance.
 * @return 0 on success, 1 if account not found.
 */
int perform_update_account_balance(int id, float newBalance);

/**
 * @brief Deletes an account.
 * @param id The ID of the account to delete.
 * @return 0 on success, 1 if account not found.
 */
int perform_delete_account(int id);

/**
 * @brief Gets the details for a single account.
 * @param id The ID of the account to find.
 * @param[out] acc_out A pointer to an account struct to be filled with data.
 * @return 0 on success, 1 if account not found.
 */
int get_account_details(int id, account* acc_out);

/**
 * @brief Returns a formatted string containing all account details.
 * The GUI can display this in a text area.
 * @return A pointer to a static string. Do not free this pointer.
 */
const char* get_all_accounts_summary();


// --- Banking (Transaction) Functions ---

/**
 * @brief Deposits money into an account.
 * This creates a blockchain transaction.
 * @param id The account ID.
 * @param amount The amount to deposit.
 * @return 0 on success.
 * @return 1 if account not found.
 * @return 2 if amount is invalid (<= 0).
 */
int perform_deposit(int id, float amount);

/**
 * @brief Withdraws money from an account.
 * This creates a blockchain transaction.
 * @param id The account ID.
 * @param amount The amount to withdraw.
 * @return 0 on success.
 * @return 1 if account not found.
 * @return 2 if amount is invalid (<= 0).
 * @return 3 if funds are insufficient.
 */
int perform_withdraw(int id, float amount);

/**
 * @brief Transfers money between two accounts.
 * This creates a blockchain transaction.
 * @param fromID The sender's account ID.
 * @param toID The receiver's account ID.
 * @param amount The amount to transfer.
 * @return 0 on success.
 * @return 1 if sender not found.
 * @return 2 if receiver not found.
 * @return 3 if amount is invalid or funds are insufficient.
 */
int perform_transfer(int fromID, int toID, float amount);


// --- Blockchain Functions ---

/**
 * @brief Returns a formatted string of the entire blockchain.
 * The GUI can display this in a text area.
 * @return A pointer to a static string. Do not free this pointer.
 */
const char* get_blockchain_string();

/**
 * @brief Validates the integrity of the blockchain.
 * @return 1 if the chain is valid, 0 if it is broken.
 */
int perform_validate_chain();


#ifdef __cplusplus
} // extern "C"
#endif

#endif // PBL_BACKEND_H