#include <iostream>
#include <string>
#include <vector>
using namespace std;

// Forward declarations
class BankAccount;
class Person;
class Transaction;
class BankDatabase;

// Transaction class
class Transaction {
private:
    int transactionId;
    static int nextId;
    string description;
    double amount;
    string date;
    
public:
    // Constructor
    Transaction(const string& desc, double amt, const string& d)
        : description(desc), amount(amt), date(d) {
        transactionId = nextId++;
        cout << "Transaction created: " << description << ", Amount: " << amount << endl;
    }
    
    // Display transaction details
    void display() const {
        cout << "Transaction #" << transactionId << ": " << description
             << ", Amount: $" << amount << ", Date: " << date << endl;
    }
    
    // Getter for amount
    double getAmount() const {
        return amount;
    }
    
    // Getter for description
    string getDescription() const {
        return description;
    }
    
    // Getter for date
    string getDate() const {
        return date;
    }
};

// Initialize static member
int Transaction::nextId = 1000;

// BankAccount class
class BankAccount {
private:
    int accountNumber;
    static int nextAccountNumber;
    double balance;
    vector<Transaction> transactions;
    
    // Private constructor - accounts must be created through BankDatabase
    BankAccount(double initialBalance = 0.0)
        : balance(initialBalance) {
        accountNumber = nextAccountNumber++;
        cout << "Account #" << accountNumber << " created with initial balance: $" << balance << endl;
    }
    
public:
    // Friend declarations
    friend class Person;             // Person class can access private members
    friend class BankDatabase;       // BankDatabase can create accounts and access private members
    friend void auditAccount(const BankAccount& account); // Audit function can access private members
    friend void printAccountStatement(const BankAccount& account); // Allow printAccountStatement access
    
    // Non-friend public methods
    
    // Get account number
    int getAccountNumber() const {
        return accountNumber;
    }
    
    // Get current balance
    double getBalance() const {
        return balance;
    }
    
    // Display account information
    void displayInfo() const {
        cout << "Account #" << accountNumber << ", Balance: $" << balance << endl;
    }
    
    // Display transaction history
    void displayTransactions() const {
        cout << "Transaction history for Account #" << accountNumber << ":" << endl;
        if (transactions.empty()) {
            cout << "  No transactions found." << endl;
            return;
        }
        
        for (const auto& transaction : transactions) {
            cout << "  ";
            transaction.display();
        }
    }
};

// Initialize static member
int BankAccount::nextAccountNumber = 10000;

// Person class (friend of BankAccount)
class Person {
private:
    string name;
    string address;
    vector<BankAccount*> accounts;
    
public:
    // Constructor
    Person(const string& n, const string& addr) : name(n), address(addr) {
        cout << "Person created: " << name << endl;
    }
    
    // Destructor
    ~Person() {
        cout << "Person destroyed: " << name << endl;
    }
    
    // Display person information
    void displayInfo() const {
        cout << "Name: " << name << ", Address: " << address << endl;
        cout << "Accounts:" << endl;
        
        if (accounts.empty()) {
            cout << "  No accounts found." << endl;
            return;
        }
        
        for (const auto& account : accounts) {
            cout << "  ";
            account->displayInfo();
        }
    }
    
    // Add account to person
    void addAccount(BankAccount* account) {
        accounts.push_back(account);
        cout << "Account #" << account->accountNumber << " added to " << name << endl;
    }
    
    // Deposit money to account (can access account's private members)
    bool deposit(BankAccount* account, double amount, const string& date) {
        if (amount <= 0) {
            cout << "Invalid deposit amount." << endl;
            return false;
        }
        
        // Find the account
        bool found = false;
        for (const auto& acc : accounts) {
            if (acc == account) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            cout << "Account not owned by this person." << endl;
            return false;
        }
        
        // Direct access to private member balance
        account->balance += amount;
        
        // Create a transaction and add it to the account's history
        Transaction transaction("Deposit", amount, date);
        account->transactions.push_back(transaction);
        
        cout << "Deposited $" << amount << " to Account #" << account->accountNumber 
             << ". New balance: $" << account->balance << endl;
        
        return true;
    }
    
    // Withdraw money from account (can access account's private members)
    bool withdraw(BankAccount* account, double amount, const string& date) {
        if (amount <= 0) {
            cout << "Invalid withdrawal amount." << endl;
            return false;
        }
        
        // Find the account
        bool found = false;
        for (const auto& acc : accounts) {
            if (acc == account) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            cout << "Account not owned by this person." << endl;
            return false;
        }
        
        // Check if there's enough balance
        if (account->balance < amount) {
            cout << "Insufficient funds." << endl;
            return false;
        }
        
        // Direct access to private member balance
        account->balance -= amount;
        
        // Create a transaction and add it to the account's history
        Transaction transaction("Withdrawal", -amount, date);
        account->transactions.push_back(transaction);
        
        cout << "Withdrew $" << amount << " from Account #" << account->accountNumber 
             << ". New balance: $" << account->balance << endl;
        
        return true;
    }
    
    // Getters
    string getName() const { return name; }
    string getAddress() const { return address; }
};

// BankDatabase class (friend of BankAccount)
class BankDatabase {
private:
    vector<BankAccount*> accounts;
    
public:
    // Destructor to clean up accounts
    ~BankDatabase() {
        cout << "BankDatabase destroying accounts..." << endl;
        for (auto& account : accounts) {
            delete account;
        }
    }
    
    // Create a new account (can access BankAccount's private constructor)
    BankAccount* createAccount(double initialBalance = 0.0) {
        BankAccount* newAccount = new BankAccount(initialBalance);
        accounts.push_back(newAccount);
        return newAccount;
    }
    
    // Find account by number
    BankAccount* findAccount(int accountNumber) const {
        for (auto& account : accounts) {
            if (account->accountNumber == accountNumber) {
                return account;
            }
        }
        return nullptr;
    }
    
    // Display all accounts
    void displayAllAccounts() const {
        cout << "All accounts in the database:" << endl;
        if (accounts.empty()) {
            cout << "  No accounts found." << endl;
            return;
        }
        
        for (const auto& account : accounts) {
            cout << "  ";
            account->displayInfo();
        }
    }
    
    // Calculate total bank assets
    double calculateTotalAssets() const {
        double total = 0.0;
        for (const auto& account : accounts) {
            total += account->balance;  // Direct access to private member
        }
        return total;
    }
    
    // Transfer money between accounts (direct access to private members)
    bool transferFunds(BankAccount* fromAccount, BankAccount* toAccount, 
                       double amount, const string& date) {
        if (amount <= 0) {
            cout << "Invalid transfer amount." << endl;
            return false;
        }
        
        if (fromAccount->balance < amount) {
            cout << "Insufficient funds for transfer." << endl;
            return false;
        }
        
        // Direct access to private member balance
        fromAccount->balance -= amount;
        toAccount->balance += amount;
        
        // Create transactions for both accounts
        Transaction withdrawalTx("Transfer to Account #" + to_string(toAccount->accountNumber), 
                               -amount, date);
        Transaction depositTx("Transfer from Account #" + to_string(fromAccount->accountNumber), 
                            amount, date);
        
        fromAccount->transactions.push_back(withdrawalTx);
        toAccount->transactions.push_back(depositTx);
        
        cout << "Transferred $" << amount << " from Account #" << fromAccount->accountNumber 
             << " to Account #" << toAccount->accountNumber << endl;
        
        return true;
    }
};

// Friend function for BankAccount (not a member of any class)
void auditAccount(const BankAccount& account) {
    cout << "Auditing Account #" << account.accountNumber << endl;
    
    double calculatedBalance = 0.0;
    
    cout << "Transaction audit:" << endl;
    for (const auto& transaction : account.transactions) {
        transaction.display();
        calculatedBalance += transaction.getAmount();
    }
    
    cout << "Initial balance: $" << (account.balance - calculatedBalance) << endl;
    cout << "Transactions total: $" << calculatedBalance << endl;
    cout << "Current balance: $" << account.balance << endl;
    
    if (calculatedBalance == account.balance) {
        cout << "Audit result: PASSED - Balance matches transactions" << endl;
    }
    else {
        cout << "Audit result: FAILED - Balance discrepancy found!" << endl;
    }
}

// Friend function declaration for printing account statement
void printAccountStatement(const BankAccount& account);

// Implementation of printAccountStatement
void printAccountStatement(const BankAccount& account) {
    cout << "\n========== ACCOUNT STATEMENT ==========" << endl;
    cout << "Account Number: " << account.accountNumber << endl;
    cout << "Current Balance: $" << account.balance << endl;
    cout << "Transaction History:" << endl;
    
    if (account.transactions.empty()) {
        cout << "  No transactions found." << endl;
    }
    else {
        for (const auto& transaction : account.transactions) {
            cout << "  " << transaction.getDate() << " - " 
                 << transaction.getDescription() << ": $"
                 << transaction.getAmount() << endl;
        }
    }
    cout << "=======================================" << endl;
}

int main() {
    cout << "===== Friend Classes and Friend Functions Demo =====" << endl;
    
    // Create a bank database
    BankDatabase bank;
    
    // Create some people
    Person alice("Alice Smith", "123 Main St");
    Person bob("Bob Johnson", "456 Oak Ave");
    
    // Part 1: Create accounts and associate with people
    cout << "\n1. Account Creation:" << endl;
    BankAccount* aliceChecking = bank.createAccount(1000.0);
    alice.addAccount(aliceChecking);
    
    BankAccount* aliceSavings = bank.createAccount(5000.0);
    alice.addAccount(aliceSavings);
    
    BankAccount* bobChecking = bank.createAccount(2000.0);
    bob.addAccount(bobChecking);
    
    // Display all accounts in the bank
    bank.displayAllAccounts();
    
    // Display people information
    cout << "\nPeople information:" << endl;
    alice.displayInfo();
    bob.displayInfo();
    
    // Part 2: Perform transactions
    cout << "\n2. Performing Transactions:" << endl;
    
    // Alice deposits money
    alice.deposit(aliceChecking, 500.0, "2023-05-15");
    
    // Bob withdraws money
    bob.withdraw(bobChecking, 200.0, "2023-05-16");
    
    // Bank transfers money between accounts
    bank.transferFunds(aliceSavings, aliceChecking, 1000.0, "2023-05-17");
    
    // Alice withdraws again
    alice.withdraw(aliceChecking, 300.0, "2023-05-18");
    
    // Part 3: Display account information and transactions
    cout << "\n3. Account Information After Transactions:" << endl;
    
    // Display updated account information
    cout << "\nUpdated account information:" << endl;
    aliceChecking->displayInfo();
    aliceSavings->displayInfo();
    bobChecking->displayInfo();
    
    // Display transaction history
    cout << "\nTransaction history:" << endl;
    cout << "Alice's Checking Account:" << endl;
    aliceChecking->displayTransactions();
    
    cout << "\nAlice's Savings Account:" << endl;
    aliceSavings->displayTransactions();
    
    cout << "\nBob's Checking Account:" << endl;
    bobChecking->displayTransactions();
    
    // Part 4: Use friend functions
    cout << "\n4. Using Friend Functions:" << endl;
    
    // Audit Alice's checking account
    auditAccount(*aliceChecking);
    
    // Print account statement
    printAccountStatement(*aliceChecking);
    
    // Calculate total bank assets
    double totalAssets = bank.calculateTotalAssets();
    cout << "\nTotal bank assets: $" << totalAssets << endl;
    
    cout << "\nEnd of program" << endl;
    return 0;
}