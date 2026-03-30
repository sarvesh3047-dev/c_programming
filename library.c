#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#define MAX_USERS 100
#define MAX_BOOKS 100
#define MAX_CART 50
#define MAX_TRANSACTIONS 500

/* ================= COLORS ================= */
#define RESET   "\x1b[0m"
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"

/* ================= STRUCTURES ================= */
typedef struct {
    char username[50];
    char password[50];
} User;

typedef struct {
    int id;
    char name[100];
    float buyPrice;
    float rentPrice;
    int quantity;
} Book;

typedef struct {
    char username[50];
    char bookName[100];
    char type[20]; // BUY/RENT + Payment
    float amount;
    char date[50];
} Transaction;

typedef struct {
    char name[100];
    float price;
    char type[10];
} CartItem;

/* ================= GLOBAL VARIABLES ================= */
User users[MAX_USERS];
Book books[MAX_BOOKS];
Transaction transactions[MAX_TRANSACTIONS];
CartItem cart[MAX_CART];

int userCount = 0;
int bookCount = 0;
int transactionCount = 0;
int cartCount = 0;

/* forward declarations for functions used before their definitions */
void saveBooks();

/* ================= HELPER FUNCTIONS ================= */
void clearInputBuffer() { while(getchar() != '\n'); }

void printLine(char c,int len){ for(int i=0;i<len;i++) printf("%c",c); printf("\n"); }

char* getCurrentTime(){
    time_t t = time(NULL);
    static char buffer[50];
    strftime(buffer,sizeof(buffer),"%Y-%m-%d %H:%M:%S",localtime(&t));
    return buffer;
}

int readInt(){
    int val;
    while(scanf("%d",&val)!=1){ 
        printf(RED "Invalid input! Enter a number: " RESET); 
        clearInputBuffer(); 
    }
    clearInputBuffer();
    return val;
}

float readFloat(){
    float val;
    while(scanf("%f",&val)!=1){ 
        printf(RED "Invalid input! Enter a number: " RESET); 
        clearInputBuffer(); 
    }
    clearInputBuffer();
    return val;
}

/* ================= ADMIN FUNCTIONS ================= */
void addBook(){
    if(bookCount>=MAX_BOOKS){ printf(RED "Book limit reached!\n" RESET); return; }
    books[bookCount].id = bookCount+1;
    printf(CYAN "Enter Book Name: " RESET); scanf(" %[^\n]", books[bookCount].name);
    printf(CYAN "Enter Buy Price: " RESET); books[bookCount].buyPrice = readFloat();
    printf(CYAN "Enter Rent Price: " RESET); books[bookCount].rentPrice = readFloat();
        printf(CYAN "Enter Quantity: " RESET); books[bookCount].quantity = readInt();
        bookCount++;
    printf(GREEN "Book Added Successfully!\n" RESET);
}

void viewBooks(){
    if(bookCount==0){ printf(RED "No books available.\n" RESET); return; }
    printLine('-',100);
    printf(YELLOW "| %-4s | %-35s | %-10s | %-10s | %-8s |\n" RESET,"ID","Name","Buy Price","Rent Price","Qty");
    printLine('-',100);
    for(int i=0;i<bookCount;i++)
        printf("| %-4d | %-35s | " MAGENTA "%-10.2f" RESET " | " MAGENTA "%-10.2f" RESET " | " MAGENTA "%-8d" RESET " |\n",
               books[i].id, books[i].name, books[i].buyPrice, books[i].rentPrice, books[i].quantity);
    printLine('-',100);
}

void searchBook(){
    char query[100]; int found=0;
    printf(CYAN "Enter book name to search: " RESET); scanf(" %[^\n]", query);
    printLine('-',70);
    for(int i=0;i<bookCount;i++){
        if(strstr(books[i].name,query)!=NULL){
            printf(GREEN "%d. %s | Buy: %.2f | Rent: %.2f\n" RESET,
                   books[i].id, books[i].name, books[i].buyPrice, books[i].rentPrice);
            found=1;
        }
    }
    if(!found) printf(RED "No books found matching '%s'\n" RESET, query);
}

void updateBook(){
    int id;
    printf(CYAN "Enter Book ID to update quantity: " RESET); id = readInt();
    if(id<1 || id>bookCount){ printf(RED "Invalid Book ID\n" RESET); return; }

    printf("Book: %s\nCurrent Quantity: %d\n", books[id-1].name, books[id-1].quantity);
    printf(CYAN "Enter new Quantity (>=0): " RESET);
    int newQty = readInt();
    if(newQty < 0){ printf(RED "Invalid quantity. Update cancelled.\n" RESET); return; }
    books[id-1].quantity = newQty;
    saveBooks();
    printf(GREEN "Book quantity updated successfully!\n" RESET);
}

void viewUsers(){
    if(userCount==0){ printf(RED "No registered users.\n" RESET); return; }
    printf(CYAN "\n--- REGISTERED USERS ---\n" RESET);
    for(int i=0;i<userCount;i++) printf(YELLOW "%d. %s\n" RESET,i+1,users[i].username);
}

void viewTransactions(){
    if(transactionCount==0){ printf(RED "No transactions found.\n" RESET); return; }
    printLine('-',90);
    printf(YELLOW "| %-4s | %-15s | %-30s | %-10s | %-10s | %-20s |\n" RESET,
           "No","User","Book Name","Type","Amount","Date");
    printLine('-',90);
    for(int i=0;i<transactionCount;i++)
        printf("| %-4d | %-15s | %-30s | %-10s | " MAGENTA "%-10.2f" RESET " | %-20s |\n",
               i+1, transactions[i].username, transactions[i].bookName,
               transactions[i].type, transactions[i].amount, transactions[i].date);
    printLine('-',90);
}

void viewStats(){
    float revenue=0;
    for(int i=0;i<transactionCount;i++) revenue+=transactions[i].amount;

    printf(CYAN "\n--- LIBRARY STATISTICS ---\n" RESET);
    printf(YELLOW "Total Users       : " RESET "%d\n", userCount);
    printf(YELLOW "Total Books       : " RESET "%d\n", bookCount);
    printf(YELLOW "Total Transactions: " RESET "%d\n", transactionCount);
    printf(YELLOW "Total Revenue     : " RESET MAGENTA "%.2f\n" RESET, revenue);
}

void adminMenu(){
    int choice;
    while(1){
        printf(CYAN "\n===== ADMIN MENU =====\n" RESET);
        printf("1. Add Book\n2. View Books\n3. Search Book\n4. Update Book\n5. View Users\n6. View Transactions\n7. View Stats\n8. Logout\n");
        printf(CYAN "Choice: " RESET); choice=readInt();
        switch(choice){
            case 1:addBook();break;
            case 2:viewBooks();break;
            case 3:searchBook();break;
            case 4:updateBook();break;
            case 5:viewUsers();break;
            case 6:viewTransactions();break;
            case 7:viewStats();break;
            case 8:return;
            default: printf(RED "Invalid Choice\n" RESET);
        }
    }
}

void adminLogin(){
    char user[50], pass[50];
    printf(CYAN "Admin Username: " RESET); scanf("%s", user);
    printf(CYAN "Admin Password: " RESET); scanf("%s", pass);
    if(strcmp(user,"admin")==0 && strcmp(pass,"admin123")==0){
        printf(GREEN "Admin Login Successful!\n" RESET);
        adminMenu();
    } else printf(RED "Invalid Admin Credentials!\n" RESET);
}

/* ================= USER FUNCTIONS ================= */
void registerUser(){
    if(userCount>=MAX_USERS){ printf(RED "User limit reached!\n" RESET); return; }
    printf(CYAN "Enter Username: " RESET); scanf("%s", users[userCount].username);
    printf(CYAN "Enter Password: " RESET); scanf("%s", users[userCount].password);
    userCount++;
    printf(GREEN "Registration Successful!\n" RESET);
}

int loginUser(char loggedUser[]){
    char user[50], pass[50];
    printf(CYAN "Username: " RESET); scanf("%s", user);
    printf(CYAN "Password: " RESET); scanf("%s", pass);
    for(int i=0;i<userCount;i++){
        if(strcmp(user,users[i].username)==0 && strcmp(pass,users[i].password)==0){
            strcpy(loggedUser,user); return 1;
        }
    }
    return 0;
}

/* ================= CART & PAYMENT ================= */
void addToCart(int id,char type[],char username[]){
    if(id<1||id>bookCount){ printf(RED "Invalid Book ID\n" RESET); return; }
    if(books[id-1].quantity <= 0){ printf(RED "book out of stock\n" RESET); return; }
    if(cartCount>=MAX_CART){ printf(RED "Cart is full!\n" RESET); return; }

    strcpy(cart[cartCount].name, books[id-1].name);
    strcpy(cart[cartCount].type, type);
    cart[cartCount].price = (strcmp(type,"BUY")==0)? books[id-1].buyPrice : books[id-1].rentPrice;
    cartCount++;
    printf(GREEN "Book '%s' added to cart successfully!\n" RESET, books[id-1].name);
}

void viewCart(){
    if(cartCount==0){ printf(RED "Cart is empty!\n" RESET); return; }
    printLine('-',50);
    printf(YELLOW "| %-3s | %-25s | %-5s | %-8s |\n" RESET,"No","Book Name","Type","Price");
    printLine('-',50);
    for(int i=0;i<cartCount;i++)
        printf("| %-3d | %-25s | %-5s | " MAGENTA "%-8.2f" RESET " |\n", i+1, cart[i].name, cart[i].type, cart[i].price);
    printLine('-',50);
}

void removeFromCart(){
    if(cartCount==0){ printf(RED "Cart is empty!\n" RESET); return; }
    int id; viewCart(); printf(CYAN "Enter item number to remove: " RESET); id=readInt();
    if(id<1||id>cartCount){ printf(RED "Invalid item\n" RESET); return; }
    for(int i=id-1;i<cartCount-1;i++) cart[i]=cart[i+1];
    cartCount--;
    printf(GREEN "Item removed from cart\n" RESET);
}

void checkout(char username[]){
    if(cartCount==0){ printf(RED "Cart is empty!\n" RESET); return; }
    float total=0; for(int i=0;i<cartCount;i++) total+=cart[i].price;

    printf(CYAN "\nTotal Amount: " MAGENTA "%.2f\n" RESET, total);
    printf(CYAN "Select Payment Method:\n1. Cash\n2. Card\nChoice: " RESET);

    int payChoice; char paymentMethod[20]; payChoice=readInt();
    switch(payChoice){
        case 1: strcpy(paymentMethod,"Cash"); printf(GREEN "Payment Successful via Cash!\n" RESET); break;
        case 2:{
            strcpy(paymentMethod,"Card"); char card[20], cvv[5];
            printf(CYAN "Enter Card Number: " RESET); scanf("%s", card);
            printf(CYAN "Enter CVV: " RESET); scanf("%s", cvv);
            printf(GREEN "Payment Successful via Card!\n" RESET);
            break;
        }
        default: printf(RED "Invalid Payment Option! Payment cancelled.\n" RESET); return;
    }

    for(int i=0;i<cartCount;i++){
        strcpy(transactions[transactionCount].username, username);
        sprintf(transactions[transactionCount].type,"%s (%s)", cart[i].type, paymentMethod);
        strcpy(transactions[transactionCount].bookName, cart[i].name);
        transactions[transactionCount].amount=cart[i].price;
        strcpy(transactions[transactionCount].date,getCurrentTime());
        transactionCount++;
    }

    /* Decrement book quantities for purchased/rented items */
    for(int i=0;i<cartCount;i++){
        for(int j=0;j<bookCount;j++){
            if(strcmp(books[j].name, cart[i].name)==0){
                books[j].quantity = (books[j].quantity>0)? books[j].quantity-1 : 0;
                break;
            }
        }
    }
    /* persist updated quantities */
    saveBooks();

    printf(GREEN "Bill Generated Successfully!\n" RESET);
    cartCount=0;
}

void generateBill(char username[]){
    int found=0; float total=0;
    printLine('-',60);
    printf(YELLOW "| %-25s | %-15s | %-10s |\n" RESET,"Book Name","Type","Price");
    printLine('-',60);
    for(int i=transactionCount-1;i>=0;i--){
        if(strcmp(transactions[i].username, username)==0){
            printf("| %-25s | %-15s | " MAGENTA "%-10.2f" RESET " |\n",
                   transactions[i].bookName, transactions[i].type, transactions[i].amount);
            total+=transactions[i].amount; found=1;
        }
    }
    printLine('-',60);
    if(found) printf(CYAN "TOTAL AMOUNT: " MAGENTA "%.2f\n" RESET, total);
    else printf(RED "No transactions to generate bill.\n" RESET);
}

void viewUserTransactions(char username[]){
    int found=0;
    printLine('-',70);
    for(int i=0;i<transactionCount;i++){
        if(strcmp(transactions[i].username,username)==0){
            printf(GREEN "%s | %s | %s | %.2f | %s\n" RESET,
                   transactions[i].username,transactions[i].bookName,
                   transactions[i].type,transactions[i].amount,transactions[i].date);
            found=1;
        }
    }
    if(!found) printf(RED "No transactions found\n" RESET);
    printLine('-',70);
}

void userMenu(char username[]){
    int choice,id;
    while(1){
        printf(CYAN "\n===== USER MENU =====\n" RESET);
        printf("1. View Books\n2. Search Book\n3. Buy Book\n4. Rent Book\n5. View Cart\n6. Remove from Cart\n7. Checkout\n8. View My Transactions\n9. Generate Bill\n10. Logout\n");
        printf(CYAN "Choice: " RESET); choice=readInt();
        switch(choice){
            case 1: viewBooks(); break;
            case 2: searchBook(); break;
            case 3: viewBooks(); printf(CYAN "Enter Book ID: " RESET); id=readInt(); addToCart(id,"BUY",username); break;
            case 4: viewBooks(); printf(CYAN "Enter Book ID: " RESET); id=readInt(); addToCart(id,"RENT",username); break;
            case 5: viewCart(); break;
            case 6: removeFromCart(); break;
            case 7: checkout(username); break;
            case 8: viewUserTransactions(username); break;
            case 9: generateBill(username); break;
            case 10: return;
            default: printf(RED "Invalid Choice\n" RESET);
        }
    }
}

/* ================= FILE I/O ================= */
// Simple text-based persistence. Files used:
// users.txt         -> username|password
// books.txt         -> id|name|buyPrice|rentPrice
// transactions.txt  -> username|bookName|type|amount|date

void loadUsers(){
    FILE *f = fopen("users.txt","r");
    if(!f) return; // no file yet
    char line[256];
    while(fgets(line,sizeof(line),f)){
        if(userCount>=MAX_USERS) break;
        line[strcspn(line,"\n")] = 0;
        char *u = strtok(line,"|");
        char *p = strtok(NULL,"|");
        if(u && p){ strncpy(users[userCount].username,u,sizeof(users[userCount].username)-1); users[userCount].username[sizeof(users[userCount].username)-1]=0;
                      strncpy(users[userCount].password,p,sizeof(users[userCount].password)-1); users[userCount].password[sizeof(users[userCount].password)-1]=0;
                      userCount++; }
    }
    fclose(f);
}

void saveUsers(){
    FILE *f = fopen("users.txt","w");
    if(!f) { printf(RED "Failed to save users: %s\n" RESET, strerror(errno)); return; }
    for(int i=0;i<userCount;i++) fprintf(f,"%s|%s\n",users[i].username,users[i].password);
    fclose(f);
}

void loadBooks(){
    FILE *f = fopen("books.txt","r");
    if(!f) return;
    char line[512];
    while(fgets(line,sizeof(line),f)){
        if(bookCount>=MAX_BOOKS) break;
        line[strcspn(line,"\n")] = 0;
        char *id = strtok(line,"|");
        char *name = strtok(NULL,"|");
        char *buy = strtok(NULL,"|");
        char *rent = strtok(NULL,"|");
        char *qty = strtok(NULL,"|");
        if(name && buy && rent){
            books[bookCount].id = bookCount+1;
            strncpy(books[bookCount].name,name,sizeof(books[bookCount].name)-1); books[bookCount].name[sizeof(books[bookCount].name)-1]=0;
            books[bookCount].buyPrice = atof(buy);
            books[bookCount].rentPrice = atof(rent);
            if(qty) books[bookCount].quantity = atoi(qty);
            else books[bookCount].quantity = 1;
            bookCount++;
        }
    }
    fclose(f);
}

void saveBooks(){
    FILE *f = fopen("books.txt","w");
    if(!f) { printf(RED "Failed to save books: %s\n" RESET, strerror(errno)); return; }
    for(int i=0;i<bookCount;i++) fprintf(f,"%d|%s|%.2f|%.2f|%d\n",books[i].id,books[i].name,books[i].buyPrice,books[i].rentPrice, books[i].quantity);
    fclose(f);
}

void loadTransactions(){
    FILE *f = fopen("transactions.txt","r");
    if(!f) return;
    char line[512];
    while(fgets(line,sizeof(line),f)){
        if(transactionCount>=MAX_TRANSACTIONS) break;
        line[strcspn(line,"\n")] = 0;
        char *u = strtok(line,"|");
        char *b = strtok(NULL,"|");
        char *t = strtok(NULL,"|");
        char *amt = strtok(NULL,"|");
        char *d = strtok(NULL,"|");
        if(u && b && t && amt && d){
            strncpy(transactions[transactionCount].username,u,sizeof(transactions[transactionCount].username)-1); transactions[transactionCount].username[sizeof(transactions[transactionCount].username)-1]=0;
            strncpy(transactions[transactionCount].bookName,b,sizeof(transactions[transactionCount].bookName)-1); transactions[transactionCount].bookName[sizeof(transactions[transactionCount].bookName)-1]=0;
            strncpy(transactions[transactionCount].type,t,sizeof(transactions[transactionCount].type)-1); transactions[transactionCount].type[sizeof(transactions[transactionCount].type)-1]=0;
            transactions[transactionCount].amount = atof(amt);
            strncpy(transactions[transactionCount].date,d,sizeof(transactions[transactionCount].date)-1); transactions[transactionCount].date[sizeof(transactions[transactionCount].date)-1]=0;
            transactionCount++; }
    }
    fclose(f);
}

void saveTransactions(){
    FILE *f = fopen("transactions.txt","w");
    if(!f) { printf(RED "Failed to save transactions: %s\n" RESET, strerror(errno)); return; }
    for(int i=0;i<transactionCount;i++) fprintf(f,"%s|%s|%s|%.2f|%s\n",transactions[i].username,transactions[i].bookName,transactions[i].type,transactions[i].amount,transactions[i].date);
    fclose(f);
}

/* ================= MAIN ================= */
int main(){
    int choice; char loggedUser[50];
    // Load persisted data (if any)
    loadUsers();
    loadBooks();
    loadTransactions();
    while(1){
        printf(CYAN "\n===== LIBRARY SYSTEM =====\n" RESET);
        printf("1. Admin Login\n2. User Register\n3. User Login\n4. Exit\n");
        printf(CYAN "Choice: " RESET); choice=readInt();
        switch(choice){
            case 1: adminLogin(); break;
            case 2: registerUser(); break;
            case 3: if(loginUser(loggedUser)){ printf(GREEN "Login Successful!\n" RESET); userMenu(loggedUser); } else printf(RED "Invalid Credentials!\n" RESET); break;
            case 4:
                // Save data before exiting
                saveUsers();
                saveBooks();
                saveTransactions();
                printf(GREEN "Data saved. Exiting...\n" RESET);
                exit(0);
            default: printf(RED "Invalid Choice\n" RESET);
        }
    }
}
