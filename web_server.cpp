#include "httplib.h"
#include <iostream>
#include <signal.h> // For handling shutdown signals

// Include your C backend API
extern "C" {
    #include "c_backend/backend.h"
}

// --- Graceful Shutdown ---
httplib::Server svr;

void handle_shutdown(int signal) {
    std::cout << "\nCaught signal " << signal << ". Shutting down..." << std::endl;
    shutdown_system();
    svr.stop();
}

int main(void) {
    // 1. Initialize your C backend
    initialize_system();

    // 2. Define API Endpoints

    // --- Auth Endpoints ---
    svr.Post("/api/register", [](const httplib::Request &req, httplib::Response &res) {
        if (req.has_param("id") && req.has_param("username") && req.has_param("password")) {
            int id = std::stoi(req.get_param_value("id"));
            std::string username = req.get_param_value("username");
            std::string password = req.get_param_value("password");
            int result = perform_register(id, username.c_str(), password.c_str());
            res.set_header("Content-Type", "application/json");
            if (result == 0) {
                res.set_content("{\"success\": true, \"message\": \"Registration successful!\"}", "application/json");
            } else {
                res.status = 400;
                res.set_content("{\"success\": false, \"message\": \"Registration failed. User limit reached?\"}", "application/json");
            }
        } else {
            res.status = 400;
            res.set_content("{\"success\": false, \"message\": \"Missing registration data.\"}", "application/json");
        }
    });

    svr.Post("/api/login", [](const httplib::Request &req, httplib::Response &res) {
        if (req.has_param("id") && req.has_param("username") && req.has_param("password")) {
            int id = std::stoi(req.get_param_value("id"));
            std::string username = req.get_param_value("username");
            std::string password = req.get_param_value("password");
            int result = perform_login(id, username.c_str(), password.c_str());
            res.set_header("Content-Type", "application/json");
            if (result == 1) {
                res.set_content("{\"success\": true, \"message\": \"Login successful!\"}", "application/json");
            } else {
                res.status = 401;
                res.set_content("{\"success\": false, \"message\": \"Login failed. Check credentials.\"}", "application/json");
            }
        } else {
            res.status = 400;
            res.set_content("{\"success\": false, \"message\": \"Missing login data.\"}", "application/json");
        }
    });

    // --- NEW: Create Account (Module 1) ---
    svr.Post("/api/create_account", [](const httplib::Request &req, httplib::Response &res) {
        res.set_header("Content-Type", "application/json");
        if (req.has_param("id") && req.has_param("name") && req.has_param("phno") && req.has_param("balance")) {
            int id = std::stoi(req.get_param_value("id"));
            std::string name = req.get_param_value("name");
            std::string phno = req.get_param_value("phno");
            float balance = std::stof(req.get_param_value("balance"));

            int result = perform_create_account(id, name.c_str(), phno.c_str(), balance);

            if (result == 0) {
                res.set_content("{\"success\": true, \"message\": \"Account created successfully!\"}", "application/json");
            } else if (result == 3) {
                 res.status = 400;
                 res.set_content("{\"success\": false, \"message\": \"Account ID already exists.\"}", "application/json");
            } else {
                res.status = 400;
                res.set_content("{\"success\": false, \"message\": \"Failed to create account.\"}", "application/json");
            }
        } else {
            res.status = 400;
            res.set_content("{\"success\": false, \"message\": \"Missing form data.\"}", "application/json");
        }
    });

    // --- Deposit (Module 2) ---
    svr.Post("/api/deposit", [](const httplib::Request &req, httplib::Response &res) {
        res.set_header("Content-Type", "application/json");
        if (req.has_param("id") && req.has_param("amount")) {
            int id = std::stoi(req.get_param_value("id"));
            float amount = std::stof(req.get_param_value("amount"));
            int result = perform_deposit(id, amount);
            if (result == 0) {
                res.set_content("{\"success\": true, \"message\": \"Deposit successful!\"}", "application/json");
            } else if (result == 1) {
                res.status = 404;
                res.set_content("{\"success\": false, \"message\": \"Account not found.\"}", "application/json");
            } else {
                res.status = 400;
                res.set_content("{\"success\": false, \"message\": \"Invalid deposit amount.\"}", "application/json");
            }
        } else {
            res.status = 400;
            res.set_content("{\"success\": false, \"message\": \"Missing deposit data.\"}", "application/json");
        }
    });

    // --- NEW: Withdraw (Module 3) ---
    svr.Post("/api/withdraw", [](const httplib::Request &req, httplib::Response &res) {
        res.set_header("Content-Type", "application/json");
        if (req.has_param("id") && req.has_param("amount")) {
            int id = std::stoi(req.get_param_value("id"));
            float amount = std::stof(req.get_param_value("amount"));
            int result = perform_withdraw(id, amount);
            if (result == 0) {
                res.set_content("{\"success\": true, \"message\": \"Withdrawal successful!\"}", "application/json");
            } else if (result == 1) {
                res.status = 404;
                res.set_content("{\"success\": false, \"message\": \"Account not found.\"}", "application/json");
            } else if (result == 3) {
                res.status = 400;
                res.set_content("{\"success\": false, \"message\": \"Insufficient funds.\"}", "application/json");
            } else {
                res.status = 400;
                res.set_content("{\"success\": false, \"message\": \"Invalid withdrawal amount.\"}", "application/json");
            }
        } else {
            res.status = 400;
            res.set_content("{\"success\": false, \"message\": \"Missing withdrawal data.\"}", "application/json");
        }
    });

    // --- NEW: Transfer (Module 4) ---
    svr.Post("/api/transfer", [](const httplib::Request &req, httplib::Response &res) {
        res.set_header("Content-Type", "application/json");
        if (req.has_param("fromID") && req.has_param("toID") && req.has_param("amount")) {
            int fromID = std::stoi(req.get_param_value("fromID"));
            int toID = std::stoi(req.get_param_value("toID"));
            float amount = std::stof(req.get_param_value("amount"));
            int result = perform_transfer(fromID, toID, amount);
            if (result == 0) {
                res.set_content("{\"success\": true, \"message\": \"Transfer successful!\"}", "application/json");
            } else if (result == 1) {
                res.status = 404;
                res.set_content("{\"success\": false, \"message\": \"Sender account not found.\"}", "application/json");
            } else if (result == 2) {
                res.status = 404;
                res.set_content("{\"success\": false, \"message\": \"Receiver account not found.\"}", "application/json");
            } else {
                res.status = 400;
                res.set_content("{\"success\": false, \"message\": \"Invalid amount or insufficient funds.\"}", "application/json");
            }
        } else {
            res.status = 400;
            res.set_content("{\"success\": false, \"message\": \"Missing transfer data.\"}", "application/json");
        }
    });

    // --- Display Accounts (Module 5) ---
    svr.Get("/api/accounts", [](const httplib::Request &req, httplib::Response &res) {
        const char* accounts_summary = get_all_accounts_summary();
        res.set_content(accounts_summary, "text/plain; charset=utf-8");
    });

    // --- NEW: Update Account (Module 6) ---
    svr.Post("/api/update_account", [](const httplib::Request &req, httplib::Response &res) {
        res.set_header("Content-Type", "application/json");
        if (req.has_param("id")) {
            int id = std::stoi(req.get_param_value("id"));
            bool updated = false;

            if (req.has_param("name") && !req.get_param_value("name").empty()) {
                if (perform_update_account_name(id, req.get_param_value("name").c_str()) == 0) {
                    updated = true;
                }
            }
            if (req.has_param("phno") && !req.get_param_value("phno").empty()) {
                 if (perform_update_account_phone(id, req.get_param_value("phno").c_str()) == 0) {
                    updated = true;
                 }
            }

            if (updated) {
                res.set_content("{\"success\": true, \"message\": \"Account updated successfully!\"}", "application/json");
            } else {
                res.status = 404;
                 res.set_content("{\"success\": false, \"message\": \"Account not found or no new data provided.\"}", "application/json");
            }
        } else {
            res.status = 400;
            res.set_content("{\"success\": false, \"message\": \"Account ID is required.\"}", "application/json");
        }
    });

    // --- NEW: Delete Account (Module 7) ---
    svr.Post("/api/delete_account", [](const httplib::Request &req, httplib::Response &res) {
        res.set_header("Content-Type", "application/json");
        if (req.has_param("id")) {
            int id = std::stoi(req.get_param_value("id"));
            int result = perform_delete_account(id);
            if (result == 0) {
                res.set_content("{\"success\": true, \"message\": \"Account deleted successfully!\"}", "application/json");
            } else {
                res.status = 404;
                res.set_content("{\"success\": false, \"message\": \"Account not found.\"}", "application/json");
            }
        } else {
            res.status = 400;
            res.set_content("{\"success\": false, \"message\": \"Account ID is required.\"}", "application/json");
        }
    });

    // --- NEW: View Account (Module 8) ---
    svr.Post("/api/view_account", [](const httplib::Request &req, httplib::Response &res) {
        res.set_header("Content-Type", "application/json");
        if (req.has_param("id")) {
            int id = std::stoi(req.get_param_value("id"));

            account acc_buffer; // Create a buffer struct
            int result = get_account_details(id, &acc_buffer); // Pass its address

            if (result == 0) { // Success
                // Manually build a JSON string from the struct
                std::string json_response = "{ \"success\": true, \"account\": { ";
                json_response += "\"id\": " + std::to_string(acc_buffer.accID) + ", ";
                json_response += "\"name\": \"" + std::string(acc_buffer.name) + "\", ";
                json_response += "\"phone\": \"" + std::string(acc_buffer.phno) + "\", ";
                json_response += "\"balance\": " + std::to_string(acc_buffer.balance);
                json_response += "} }";
                res.set_content(json_response, "application/json");
            } else {
                res.status = 404; // Not Found
                res.set_content("{\"success\": false, \"message\": \"Account not found.\"}", "application/json");
            }
        } else {
             res.status = 400;
             res.set_content("{\"success\": false, \"message\": \"Account ID is required.\"}", "application/json");
        }
    });

    // --- Display Blockchain (Module 9) ---
    svr.Get("/api/blockchain", [](const httplib::Request &req, httplib::Response &res) {
        const char* blockchain_data = get_blockchain_string();
        res.set_content(blockchain_data, "text/plain; charset=utf-8");
    });

    // --- NEW: Validate Blockchain (Module 10) ---
    svr.Get("/api/validate_chain", [](const httplib::Request &req, httplib::Response &res) {
        res.set_header("Content-Type", "application/json");
        int result = perform_validate_chain();
        if (result == 1) {
            res.set_content("{\"success\": true, \"message\": \"Blockchain is valid and secure!\"}", "application/json");
        } else {
            res.status = 500;
            res.set_content("{\"success\": false, \"message\": \"DANGER: Blockchain validation FAILED. Chain is broken or has been tampered with.\"}", "application/json");
        }
    });

    // 3. Serve Static Frontend Files
    const char* web_root = "./www";
    if (!svr.set_mount_point("/", web_root)) {
        std::cerr << "Error: The frontend directory '" << web_root << "' does not exist." << std::endl;
        shutdown_system();
        return 1;
    }

    // 4. Setup Signal Handler for Graceful Shutdown (Module 11)
    signal(SIGINT, handle_shutdown);
    signal(SIGTERM, handle_shutdown);

    // 5. Start the Server
    std::cout << "Server starting on http://localhost:8080" << std::endl;
    std::cout << "Access the web UI at: http://localhost:8080" << std::endl;
    std::cout << "Press Ctrl+C to stop the server." << std::endl;

    svr.listen("localhost", 8080);

    std::cout << "Server stopped." << std::endl;
    return 0;
}