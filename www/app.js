document.addEventListener('DOMContentLoaded', () => {

    // --- Get All Views and Elements ---
    const authView = document.getElementById('auth-view');
    const dashboardView = document.getElementById('dashboard-view');
    const displayArea = document.getElementById('display-area');

    // --- Auth View Elements ---
    const loginFormContainer = document.getElementById('login-form-container');
    const registerFormContainer = document.getElementById('register-form-container');
    const loginForm = document.getElementById('login-form');
    const registerForm = document.getElementById('register-form');
    const authMessage = document.getElementById('auth-message');
    const showRegisterBtn = document.getElementById('show-register');
    const showLoginBtn = document.getElementById('show-login');

    // Get error message divs
    const idError = document.getElementById('id-error');
    const passwordError = document.getElementById('password-error');

    // --- Dashboard Elements ---
    const btnLogout = document.getElementById('btn-logout');
    const sidebarButtons = document.querySelectorAll('.nav-button');
    const allWidgets = document.querySelectorAll('.widget');

    // --- Helper function to show one widget at a time ---
    function showWidget(widgetId) {
        allWidgets.forEach(widget => {
            widget.classList.add('hidden');
        });
        const widgetToShow = document.getElementById(widgetId);
        if (widgetToShow) {
            widgetToShow.classList.remove('hidden');
        }
    }

    // --- Helper function to set active sidebar button ---
    function setActiveButton(button) {
        sidebarButtons.forEach(btn => {
            btn.classList.remove('active');
        });
        if (button) { // Add check in case button is null
            button.classList.add('active');
        }
    }

    // --- View Switching Function ---
    function showView(viewId) {
        if (viewId === 'dashboard') {
            dashboardView.classList.remove('hidden');
            authView.classList.add('hidden');
            showWidget('widget-system-log');
            const defaultActiveButton = document.getElementById('btn-display-accounts');
            if (defaultActiveButton) {
                setActiveButton(defaultActiveButton);
            }
        } else {
            authView.classList.remove('hidden');
            dashboardView.classList.add('hidden');
        }
    }

    // --- Message Function ---
    function showAuthMessage(message, isError = false) {
        authMessage.textContent = message;
        authMessage.className = isError ? "message error" : "message";
    }

    // --- Helper function for handling form submissions ---
    async function handleFormSubmit(form, endpoint, requiresFullDetail = false) {
        form.addEventListener('submit', async (event) => {
            event.preventDefault();
            const formData = new FormData(form);
            const body = new URLSearchParams(formData);

            try {
                const response = await fetch(endpoint, {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                    body: body
                });

                const result = await response.json();

                showWidget('widget-system-log');

                if (response.ok) {
                    if (requiresFullDetail && result.account) {
                        const acc = result.account;
                        displayArea.textContent = `--- Account Details ---\n` +
                            `ID:      ${acc.id}\n` +
                            `Name:    ${acc.name}\n` +
                            `Phone:   ${acc.phone}\n` +
                            `Balance: $${acc.balance.toFixed(2)}`;
                    } else {
                        displayArea.textContent = result.message;
                    }
                    form.reset();
                } else {
                    displayArea.textContent = `Error: ${result.message}`;
                }
            } catch (err) {
                displayArea.textContent = `Request failed: ${err}`;
            }
        });
    }

    // --- Auth View Listeners ---

    // ** THIS IS THE MISSING CODE **
    showRegisterBtn.addEventListener('click', (e) => {
        e.preventDefault();
        loginFormContainer.classList.add('hidden');
        registerFormContainer.classList.remove('hidden');
        authMessage.textContent = ''; // Clear messages
    });

    // ** THIS IS THE MISSING CODE **
    showLoginBtn.addEventListener('click', (e) => {
        e.preventDefault();
        registerFormContainer.classList.add('hidden');
        loginFormContainer.classList.remove('hidden');
        authMessage.textContent = ''; // Clear messages
    });

    // --- Validation Functions ---
    function validateId(id) {
        if (!/^\d{6,9}$/.test(id)) {
            return 'User ID must be between 6 and 9 digits.';
        }
        return '';
    }

    function validatePassword(password) {
        if (password.length <= 8) {
            return 'Password must be more than 8 characters.';
        }
        if (!/[A-Z]/.test(password)) {
            return 'Password must contain an uppercase letter.';
        }
        if (!/[a-z]/.test(password)) {
            return 'Password must contain a lowercase letter.';
        }
        if (!/[0-9]/.test(password)) {
            return 'Password must contain a digit.';
        }
        if (!/[!@#$%^&*]/.test(password)) {
            return 'Password must contain a special symbol (!@#$%^&*).';
        }
        return '';
    }

    // --- Register Form Submit with Validation ---
    registerForm.addEventListener('submit', async (event) => {
        event.preventDefault();

        idError.textContent = '';
        passwordError.textContent = '';
        authMessage.textContent = '';

        const formData = new FormData(registerForm);
        const body = new URLSearchParams(formData);
        const id = formData.get('id');
        const password = formData.get('password');

        const idValidationError = validateId(id);
        const passwordValidationError = validatePassword(password);

        let isValid = true;

        if (idValidationError) {
            idError.textContent = idValidationError;
            isValid = false;
        }
        if (passwordValidationError) {
            passwordError.textContent = passwordValidationError;
            isValid = false;
        }

        if (!isValid) {
            return;
        }

        try {
            const response = await fetch('/api/register', { method: 'POST', body: body });
            const result = await response.json();
            if (response.ok) {
                showAuthMessage(result.message, false);
                registerForm.reset();
                showLoginBtn.click();
            } else {
                showAuthMessage(result.message, true);
            }
        } catch (error) {
            showAuthMessage('Registration request failed.', true);
        }
    });

    // Login Form Submit (no validation, just submit)
    loginForm.addEventListener('submit', async (event) => {
        event.preventDefault();
        authMessage.textContent = '';
        const formData = new FormData(loginForm);
        const body = new URLSearchParams(formData);
        try {
            const response = await fetch('/api/login', { method: 'POST', body: body });
            const result = await response.json();
            if (response.ok) {
                showView('dashboard');
                displayArea.textContent = `Login successful! Welcome. Select an action from the sidebar.`;
            } else {
                showAuthMessage(result.message, true);
            }
        } catch (error) {
            showAuthMessage('Login request failed.', true);
        }
    });

    // Logout Button
    btnLogout.addEventListener('click', () => {
        showView('auth');
        showAuthMessage('You have been logged out.', false);
    });

    // --- Dashboard Sidebar Navigation ---

    document.getElementById('btn-create-account').addEventListener('click', (e) => {
        showWidget('widget-create-account');
        setActiveButton(e.currentTarget);
    });
    document.getElementById('btn-deposit').addEventListener('click', (e) => {
        showWidget('widget-deposit');
        setActiveButton(e.currentTarget);
    });
    document.getElementById('btn-withdraw').addEventListener('click', (e) => {
        showWidget('widget-withdraw');
        setActiveButton(e.currentTarget);
    });
    document.getElementById('btn-transfer').addEventListener('click', (e) => {
        showWidget('widget-transfer');
        setActiveButton(e.currentTarget);
    });
    document.getElementById('btn-update-account').addEventListener('click', (e) => {
        showWidget('widget-update-account');
        setActiveButton(e.currentTarget);
    });
    document.getElementById('btn-delete-account').addEventListener('click', (e) => {
        showWidget('widget-delete-account');
        setActiveButton(e.currentTarget);
    });
    document.getElementById('btn-view-account').addEventListener('click', (e) => {
        showWidget('widget-view-account');
        setActiveButton(e.currentTarget);
    });

    document.getElementById('btn-display-accounts').addEventListener('click', async (e) => {
        showWidget('widget-system-log');
        setActiveButton(e.currentTarget);
        displayArea.textContent = 'Loading accounts...';
        try {
            const response = await fetch('/api/accounts');
            displayArea.textContent = await response.text();
        } catch(err) { displayArea.textContent = 'Error fetching accounts.'; }
    });

    document.getElementById('btn-display-blockchain').addEventListener('click', async (e) => {
        showWidget('widget-system-log');
        setActiveButton(e.currentTarget);
        displayArea.textContent = 'Loading blockchain...';
        try {
            const response = await fetch('/api/blockchain');
            displayArea.textContent = await response.text();
        } catch(err) { displayArea.textContent = 'Error fetching blockchain.'; }
    });

    document.getElementById('btn-validate-chain').addEventListener('click', async (e) => {
        showWidget('widget-system-log');
        setActiveButton(e.currentTarget);
        displayArea.textContent = 'Validating chain...';
        try {
            const response = await fetch('/api/validate_chain');
            const result = await response.json();
            displayArea.textContent = result.message;
        } catch(err) { displayArea.textContent = 'Error validating chain.'; }
    });


    // --- Connect all forms to their API endpoints ---
    handleFormSubmit(document.getElementById('create-account-form'), '/api/create_account');
    handleFormSubmit(document.getElementById('deposit-form'), '/api/deposit');
    handleFormSubmit(document.getElementById('withdraw-form'), '/api/withdraw');
    handleFormSubmit(document.getElementById('transfer-form'), '/api/transfer');
    handleFormSubmit(document.getElementById('update-account-form'), '/api/update_account');
    handleFormSubmit(document.getElementById('delete-account-form'), '/api/delete_account');
    handleFormSubmit(document.getElementById('view-account-form'), '/api/view_account', true);

    // --- Initial State ---
    showView('auth'); // Show login/register page by default
});