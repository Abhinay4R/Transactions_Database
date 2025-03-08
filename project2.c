#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef enum{FAILURE,SUCCESS}                                               status_code;
typedef enum{NULL_KEY,MONTHS,SELLER_ID,BUYER_ID,UNIQUE_PAIR_ID,ENERGY,TIME} key        ;  
 

#define HASH_TABLE_SIZE           100003 // A large prime number for better distribution
#define MAX_LINE_LENGTH           100
#define MIN_INPUT_NO              100000
#define MAX_INPUT_NO              999999
#define FREQUENT_BUYING_THRESHOLD 5 
#define MIN_YEAR                  1950
#define MAX_YEAR                  2050

typedef struct time_and_date_of_the_transaction
{
    int date       ;
    int month      ;
    int year       ;
    int hours      ;
    int minutes    ;
}time_stamp;
//structure declaration for time of the transaction

typedef struct Energy_record_node_tag
{
    int            transaction_id              ;
    int            seller_id                   ;
    int            buyer_id                    ;
    float          energy_amount               ;
    float          price_per_unit              ;
    float          transaction_money           ;
    unsigned long  unique_pair_id              ; 
    unsigned long  transaction_hash            ;
    time_stamp     time                        ;
    struct Energy_record_node_tag *next_record ;
}Record;
//structure declaration for the node of each transaction Record

typedef struct Buyer_tag
{
    int    BID                   ;//Buyer ID
    float  Energy                ;
    struct Buyer_tag *next_buyer ;
}Buyer;
//structure declaration for the node of Each Buyer

typedef struct Seller_tag
{
    int    SID                        ;//Seller Id
    float  price_below_300_units      ;
    float  price_above_300_units      ;
    float  Revenue                    ;
    Buyer* regular_buyer_list         ;//list to maintain regular buyers
    struct Seller_tag *next_seller    ;//next node address
    
}Seller;
//structures to store the revenues and Energy of Sellers and Buyers

typedef struct Unique_pair_tag
{
    unsigned long upid       ;//uniquepairID
    int           SID        ;//Seller ID
    int           BID        ;//Buyer ID
    int           NOT        ;//Number of transactions
    float         revenue    ;
    struct Unique_pair_tag *next_unique_pair;
}Pair;
//structues to store attributes of uniquepairs

typedef struct transaction_hash_node {
    int           id       ; // 5 digits transaction ID is enough
    unsigned long hashValue; // Store the hash value
    struct transaction_hash_node* next;
} transaction_hash_node;
//structure that will be chained as the linked list in the hash function in the Hashtable array

typedef struct Database_tag
{
    Record* record_list             ;
    Seller* seller_list             ;
    Buyer*  buyer_list              ;
    Pair*   pairs_list              ;
    key     sorted_based_on         ;
    transaction_hash_node* hashTable[HASH_TABLE_SIZE]; // Hash table with chaining
}Database;
//structure declaration that will hold the Record List, Seller list, Buyer list, pairs_list, Hashtable array.

//functions that implement transactions
////////////////////////////////////
void        initialise_database(Database*)                                                                ;
void        time_fill(time_stamp*, int, int, int, int, int)                                               ;
void        populate_seller_list(Database*)                                                               ;
void        populate_buyer_list(Database*)                                                                ;
void        filling_pairs(Database*)                                                                      ;
void        populate_pairs_list(Database* db)                                                             ;
void        fill_the_regular_buyers_list_in_seller_nodes(Database *db)                                    ;
void        fill_the_price_per_unit_fields(Database *db)                                                  ;
float       revenue_of_seller(Database*, int)                                                             ;
Pair*       find_pair_that_has_maximum_transactions(Database*)                                            ;
status_code update_existing_transaction(Database* db, int TID, float EA, float PPU, time_stamp samay)     ;
status_code insert_record_into_record_list(Database*, int, int ,int , float, float, time_stamp*, Record**);

/////////////////////////
//functions that I used in the sorting implementation 

Record* merge_sorted_lists(Record*, Record*, key)                 ;
Record* division(Record*)                                         ;
Record* sort_records(Record*, key)                                ;
Record* sort_records_wrapper(Database*, key)                      ;
Buyer*  divide_buyer_list(Buyer*)                                 ;
Buyer*  merge_buyer_lists(Buyer*, Buyer*)                         ;
Buyer*  merge_sort_buyer_list(Buyer*)                             ;
Seller* merge_sort_seller_list(Seller*)                           ;
Seller* divide_seller_list(Seller*)                               ;
Seller* merge_seller_lists(Seller*, Seller*)                      ;
Pair*   divide_pairs_list(Pair*)                                  ;
Pair*   merge_pairs_lists(Pair*, Pair*)                           ;
Pair*   merge_sort_pairs_list(Pair*)                              ;
void    sort_sellers_by_revenue(Database*)                        ;
void    sort_buyers_by_energy(Database*)                          ;
void    sort_pairs_list_based_on_number_of_transactions(Database*);



///////////////////////////
//functions in to implement hash_table 

void          initialise_the_hashtable(Database *db)                         ;
status_code   storing_the_transactionID_along_with_hash(Database *db, int id);
unsigned long hashing_function(int id)                                       ;

//////////////////////////
//functions in to validate inputs 
// Calculate check digit for a 5-digit number
static int f(int,int);
int  calculate_check_digit(int number)                                   ;
int  compare_time_stamps(time_stamp, time_stamp)                         ;
int  compare_records(Record*, Record*, key)                              ;
bool validate_complete_number(int number)                                ;
bool is_time_input_valid(time_stamp)                                     ;
bool is_transaction_id_input_unique(Database*,int)                       ;
bool valid_seller_id_and_buyer_id(int, int)                              ;
bool is_transaction_input_valid(int)                                     ;
bool is_energy_input_valid(float)                                        ;
bool is_priceperunit_input_valid(float)                                  ;
bool validity_checker(Database*,int, int, int, float, float, time_stamp*);

////////////////////////
//functions in display.c
void display_records(Database*)                                     ;
void display_records_between_time(Database*, time_stamp, time_stamp);
void display_transaction_with_maximum_energy_amount(Database*)      ;
void display_sellers_list(Database*)                                ;
void display_buyers_list(Database*)                                 ;
void display_sorted_pairs(Database*)                                ;
void clear(Database*)                                               ;
//////////////////////////////////////////////////////////////////

// Dihedral permutation table
static const int permuatation_table[8][10] = {
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
    {1, 5, 7, 6, 2, 8, 3, 0, 9, 4},
    {5, 8, 0, 3, 7, 9, 6, 1, 4, 2},
    {8, 9, 1, 6, 0, 4, 3, 5, 2, 7},
    {9, 4, 5, 3, 1, 2, 6, 8, 7, 0},
    {4, 2, 8, 6, 5, 7, 3, 9, 0, 1},
    {2, 7, 9, 3, 8, 0, 6, 4, 1, 5},
    {7, 0, 4, 6, 9, 1, 3, 2, 5, 8}
};

// Non-commutative multiplication table (D5 group)
static const int d5_mult_table[10][10] = {
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
    {1, 2, 3, 4, 0, 6, 7, 8, 9, 5},
    {2, 3, 4, 0, 1, 7, 8, 9, 5, 6},
    {3, 4, 0, 1, 2, 8, 9, 5, 6, 7},
    {4, 0, 1, 2, 3, 9, 5, 6, 7, 8},
    {5, 9, 8, 7, 6, 0, 4, 3, 2, 1},
    {6, 5, 9, 8, 7, 1, 0, 4, 3, 2},
    {7, 6, 5, 9, 8, 2, 1, 0, 4, 3},
    {8, 7, 6, 5, 9, 3, 2, 1, 0, 4},
    {9, 8, 7, 6, 5, 4, 3, 2, 1, 0}
};

// Inverse lookup table
static const int inv_table[10] = {0, 4, 3, 2, 1, 5, 9, 8, 7, 6};

// Implementation of f(a,b) as described
static int f(int a, int b) {
    return d5_mult_table[a][permuatation_table[0][b]];
}

int calculate_check_digit(int number) {
    if (number < 10000 || number > 99999) return -1;
    
    // individual digits
    int a1 = (number / 10000) % 10;
    int a2 = (number / 1000) % 10 ;
    int a3 = (number / 100) % 10  ;
    int a4 = (number / 10) % 10   ;
    int a5 = number % 10          ;
    
    // Calculate c = f(a1,f(a2,f(a3,f(a4,a5))))
    int c = f(a1, f(a2, f(a3, f(a4, a5))));
    
    // Return inverse of c in the group
    return inv_table[c];
}

bool validate_complete_number(int number) {
    if (number < MIN_INPUT_NO || number > MAX_INPUT_NO) return false;
    
    int first_five       = number / 10                      ;
    int check_digit      = number % 10                      ;
    int calculated_check = calculate_check_digit(first_five);
    
    return (calculated_check == check_digit);
}

//filling the timestamp structure 
bool is_time_input_valid(time_stamp time)
{
    bool valid = false ;
    if (time.month >= 1 && time.month <= 12)
    {
        int max_days;
        if (time.month == 2)
        {
            if ((time.year % 4 == 0 && time.year % 100 != 0) || (time.year % 400 == 0))
            //leap year logic
            {
                max_days = 29;
                //leap years feb have 29 days
            }
            else
            {
                max_days = 28;
                //normal feb
            }
        }

        else if (time.month == 4 || time.month == 6 || time.month == 9 || time.month == 11)
        {
            max_days = 30;
        }

        else
        {
            max_days = 31;
        }

        if (time.date >= 1 && time.date <= max_days)
        {
            if (time.hours >= 0 && time.hours < 24 && time.minutes >= 0 && time.minutes < 60)
            //valid in 24 hour clock format
            {
                if (time.year >= MIN_YEAR && time.year <= MAX_YEAR)
                {
                    valid = true;
                }
            }
        }
    }

    return valid;
}
//checking the validity of the time input

// Unique transaction ID function implementation using a hashmap approach
// Maintaining a hashmap so that the lookup is only constant time
bool is_transaction_id_input_unique(Database *db, int id) {
    unsigned long index = hashing_function(id); // Get the hash of the ID
    transaction_hash_node* curr = db->hashTable[index]; // Access hash table through database
    bool valid = true; // Initialize flag to SUCCESS
    
    while (curr) { // Loop through the linked list
        if (curr->id == id) { // Compare integer ID
            valid = false; // Set flag to FAILURE if found
            break; // Exit the loop since we found the ID
        }
        curr = curr->next; // Move to the next node
    }
    
    return valid; // Return the flag (either valid or not)
}

// for each of these validity checkers this program implements a 
//validity checking by using a non-commutative function and use the final output with a 
// verifying digit is determind by the VERHOEFF-GUMM ALGORITHM 
bool valid_seller_id_and_buyer_id(int SID,int BID)
{
    bool valid = false ;
    if(MIN_INPUT_NO<=SID && SID <= MAX_INPUT_NO && MIN_INPUT_NO<= BID && MAX_INPUT_NO>=BID )
    {
        int check_digit_of_SID = calculate_check_digit(SID/10);
        int check_digit_of_BID = calculate_check_digit(BID/10);
        if((check_digit_of_SID == SID %10)&&(check_digit_of_BID== BID %10))
        {
            valid = true ;
        }
    }
 return valid ;
}

bool is_transaction_input_valid(int TID)
{
    bool valid = false ;
    if(MIN_INPUT_NO<=TID && TID<=MAX_INPUT_NO ){
        int check_digit = calculate_check_digit(TID/10);
        if(check_digit == TID %10)
        {
            valid = true ;
        }
    }
    return valid; 
}

bool is_energy_input_valid(float number)
{
    return (number > 0);

}

bool is_priceperunit_input_valid(float number)
{
    return (number > 0);

} 

// Validity checker function
bool validity_checker(Database *db, int TID, int SID, int BID, float EA, float PPU, time_stamp *Time) {
    time_stamp samay = *Time;
    bool valid = true; // Assume input is valid initially

    // Check time validity
    if (!is_time_input_valid(samay)) {
        printf("ERROR: Invalid time input. Please check date, month, year, hours, and minutes.\n");
        valid = false;
    }

    // Check transaction ID uniqueness
    if (!is_transaction_id_input_unique(db, TID)) { // Pass Database pointer
        printf("ERROR: Transaction ID %d is not unique. It already exists in the system.\n", TID);
        valid = false;
    }

    // Check transaction ID validity 
    if (!is_transaction_input_valid(TID)) {
        printf("ERROR: Transaction ID %d is invalid.\n", TID);
        valid = false;
    }

    // Check seller and buyer ID validity
    if (!valid_seller_id_and_buyer_id(SID, BID)) {
        printf("ERROR: Seller ID %d or Buyer ID %d is invalid.\n", SID, BID);
        valid = false;
    }

    // Check energy amount validity
    if (!is_energy_input_valid(EA)) {
        printf("ERROR: Energy amount %.2f is invalid. It must be a positive number.\n", EA);
        valid = false;
    }

    // Check price per unit validity
    if (!is_priceperunit_input_valid(PPU)) {
        printf("ERROR: Price per unit %.2f is invalid. It must be a positive number.\n", PPU);
        valid = false;
    }

    // If any input is invalid, print a general error message
    if (!valid) {
        printf("ERROR: Invalid input detected. The transaction %d was not added to the system.\n",TID);
    }

    return valid;
}

int compare_time_stamps(time_stamp t1, time_stamp t2)
{
    int result;

    if (t1.year < t2.year)
    {
        result = -1;
    }
    else if (t1.year > t2.year)
    {
        result = 1;
    }
    else if (t1.month < t2.month)
    {
        result = -1;
    }
    else if (t1.month > t2.month)
    {
        result = 1;
    }
    else if (t1.date < t2.date)
    {
        result = -1;
    }
    else if (t1.date > t2.date)
    {
        result = 1;
    }
    else if (t1.hours < t2.hours)
    {
        result = -1;
    }
    else if (t1.hours > t2.hours)
    {
        result = 1;
    }
    else if (t1.minutes < t2.minutes)
    {
        result = -1;
    }
    else if (t1.minutes > t2.minutes)
    {
        result = 1;
    }
    else
    {
        result = 0;
    }

    return result;
}

int compare_records(Record *a, Record *b, key sorting_key) {
    switch (sorting_key) {
        case MONTHS:
            return a->time.month - b->time.month;
        case SELLER_ID:
            return a->seller_id - b->seller_id;
        case BUYER_ID:
            return a->buyer_id - b->buyer_id;
        case UNIQUE_PAIR_ID:
            return (a->seller_id * 10000 + a->buyer_id) - (b->seller_id * 10000 + b->buyer_id);
        case ENERGY:
            return (a->energy_amount > b->energy_amount) - (a->energy_amount < b->energy_amount);
        case TIME:
            return compare_time_stamps(a->time,b->time) ;
        default:
            return 0;
    }
}
/////////////////////////////////


void display_records(Database *db) {
    if (!db || !db->record_list) {
        printf("No records to display.\n");
        return;
    }

    Record *current = db->record_list;
    printf("\nDisplaying all Transactions:\n");
    while (current) {
        printf("TRANSACTION DETAILS\n TID: %d \n SID: %d \n BID: %d \n Energy Amount: %.2f \n Price: %.2f \n Date: %02d-%02d-%04d\n Time: %02d:%02d\n\n", 
               current->transaction_id , 
               current->seller_id      , 
               current->buyer_id       , 
               current->energy_amount  , 
               current->price_per_unit ,
               current->time.date      ,
               current->time.month     ,
               current->time.year      ,
               current->time.hours     ,
               current->time.minutes)  ;
        current = current->next_record ;
    }
}

void display_records_between_time(Database *db, time_stamp t1, time_stamp t2) {
    if (!db || !db->record_list) {
        printf("No records to display.\n");
        return;
    }
    bool valid_inputs = is_time_input_valid(t1)&& is_time_input_valid(t2);
    int  smaller      = compare_time_stamps(t1, t2)                      ;
    if (smaller == 1) {
        time_stamp temp = t1;
        t1 = t2;
        t2 = temp;
    }
    if(!valid_inputs)
    {
        printf("The input timestamps are invalid.Returning without any changes!...\n\n");
        return;
    }
    printf("All transactions between %02d/%02d/%04d and %02d/%02d/%04d are:\n", 
           t1.date, t1.month, t1.year, t2.date, t2.month, t2.year);
    
    Record *current = db->record_list;
    while (current) {
        int a = compare_time_stamps(t1, current->time);
        int b = compare_time_stamps(t2, current->time);
        if ((a != 1) && (b != -1)) {
            printf("TRANSACTION DETAILS\n TID: %d \n SID: %d \n BID: %d \n Energy Amount: %.2f \n Price: %.2f \n Date: %02d-%02d-%04d\n Time: %02d:%02d\n\n", 
                   current->transaction_id, 
                   current->seller_id     , 
                   current->buyer_id      , 
                   current->energy_amount , 
                   current->price_per_unit,
                   current->time.date     ,
                   current->time.month    ,
                   current->time.year     ,
                   current->time.hours    ,
                   current->time.minutes);
        }
        current = current->next_record;
    }
}

void display_transaction_with_maximum_energy_amount(Database *db)
{
    if (db->record_list == NULL)
    {
        printf("No transactions available.\n");
        return;
    }

    Record *max_energy_record = db->record_list;
    Record *current_record = db->record_list   ;

    while (current_record != NULL)
    {
        if (current_record->energy_amount > max_energy_record->energy_amount)
        {
            max_energy_record = current_record;
        }
        current_record = current_record->next_record;
    }

    printf("\n\n The transaction with maximum energy amount is : \n");
    printf(" TRANSACTION DETAILS \n TID: %d \n SID: %d \n BID: %d \n Energy Amount: %.2f \n Price: %.2f \n Date: %02d-%02d-%04d\n Time: %02d:%02d\n\n",
           max_energy_record->transaction_id ,
           max_energy_record->seller_id      ,
           max_energy_record->buyer_id       ,
           max_energy_record->energy_amount  ,
           max_energy_record->price_per_unit ,
           max_energy_record->time.date      ,
           max_energy_record->time.month     ,
           max_energy_record->time.year      ,
           max_energy_record->time.hours     ,
           max_energy_record->time.minutes);
}


// Function to display all sellers along with their regular buyers
void display_sellers_list(Database* db) {
    if (!db || !db->seller_list) {
        printf("No sellers to display.\n");
        return;
    }

    printf("Displaying all sellers along with their attributes and regular buyers:\n");
    printf("------------------------------------------------\n");

    Seller* current = db->seller_list;
    while (current) {
        printf("Seller ID: %d\n", current->SID);
        printf("Revenue: %.2f\n", current->Revenue);
        printf("Price below 300 units: %.2f\n", current->price_below_300_units);
        printf("Price above 300 units: %.2f\n", current->price_above_300_units);

        // Display Regular Buyers
        printf("Regular Buyers: ");
        Buyer* buyer = current->regular_buyer_list;  // Assuming a linked list of buyers
        if (!buyer) {
            printf("None");
        } else {
            while (buyer) {
                printf("%d ", buyer->BID);  // Assuming Buyer has an ID field `BID`
                buyer = buyer->next_buyer;
            }
        }
        printf("\n------------------------------------------------\n");

        current = current->next_seller;
    }
}


// Function to display all buyers
void display_buyers_list(Database* db) {
    if (!db || !db->buyer_list) {
        printf("No buyers to display.\n");
        return;
    }

    printf("Displaying all buyers along with their attributes:\n");
    printf("------------------------------------------------\n");
    
    Buyer* current = db->buyer_list;
    while (current) {
        printf("Buyer ID: %d\n", current->BID);
        printf("Total Energy: %.2f\n", current->Energy);
        printf("------------------------------------------------\n");
        current = current->next_buyer;
    }
}

// Helper function to display sorted pairs (for verification)
void display_sorted_pairs(Database* db) {
    if (!db || !db->pairs_list) {
        printf("No pairs to display.\n");
        return;
    }
    
    printf("\nPairs sorted by number of transactions:\n");
    printf("----------------------------------------\n");
    
    Pair* current = db->pairs_list;
    while (current) {
        printf("Seller ID: %d, Buyer ID: %d\n", current->SID, current->BID);
        printf("Number of Transactions: %d\n", current->NOT);
        printf("Total Revenue: %.2f\n", current->revenue);
        printf("----------------------------------------\n");
        current = current->next_unique_pair;
    }
}


// Initialize the hash table within the Database structure
void initialise_the_hashtable(Database *db) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        db->hashTable[i] = NULL; // Set all entries in the table to NULL initially
    }
}

// Hash function to compute an index based on transaction ID
unsigned long hashing_function(int id) {
    unsigned long hash = 0;
    for (int i = 0; i < 5; i++) {
        hash = (hash * 31 + (id % 10)) % HASH_TABLE_SIZE; // Extract each digit and compute hash
        id /= 10; // Move to the next digit
    }
    return hash;
}

// Store the transaction ID along with its hash value in the hash table
status_code storing_the_transactionID_along_with_hash(Database *db, int id) {
    status_code SC = SUCCESS;
    unsigned long index = hashing_function(id); // Get the hash of the ID
    unsigned long hashValue = hashing_function(id); // Store the hash value

    transaction_hash_node* newTransaction = (transaction_hash_node*)malloc(sizeof(transaction_hash_node));
    if (!newTransaction) {
        printf("Memory allocation failed!\n");
        SC = FAILURE; // Allocation failed
    } else {
        newTransaction->id = id;                     // Store the integer ID
        newTransaction->hashValue = hashValue;       // Store the hash value
        newTransaction->next = db->hashTable[index]; // Link to the existing list
        db->hashTable[index] = newTransaction;       // Update hash table with new transaction
    }
    return SC; // Success
}

// I have stored these as a part of meta data that will 
// ensure the checking of the transaction ID , Seller ID, Buyer ID 

///////////////////////////////
Record *merge_sorted_lists(Record *left, Record *right, key sorting_key) {
    if (!left) return right;
    if (!right) return left;

    Record *head = NULL, *tail = NULL;
    if (compare_records(left, right, sorting_key) <= 0) {
        head = left;
        left = left->next_record;
    } else {
        head = right;
        right = right->next_record;
    }
    tail = head;

    while (left && right) {
        if (compare_records(left, right, sorting_key) <= 0) {
            tail->next_record = left;
            left = left->next_record;
        } else {
            tail->next_record = right;
            right = right->next_record;
        }
        tail = tail->next_record;
    }
    tail->next_record = (left) ? left : right;
    return head;
}

Record *division(Record *head)
{
    Record *slow=head,*fast=head->next_record;
    while(fast!=NULL && fast->next_record!=NULL)
    {
        slow=slow->next_record;
        fast=fast->next_record->next_record;
    }
    return slow;
}

Record *sort_records(Record *head, key sorting_key) {
    if (!head || !head->next_record) return head;

    Record *mid = division(head);
    Record *second_half = mid->next_record;
    mid->next_record = NULL;

    Record *left_sorted = sort_records(head, sorting_key);
    Record *right_sorted = sort_records(second_half, sorting_key);

    return merge_sorted_lists(left_sorted, right_sorted, sorting_key);
}

Record* sort_records_wrapper(Database* Database_pointer, key sorting_key)
{
    if(Database_pointer->sorted_based_on == sorting_key)
    {
        Record* head = Database_pointer->record_list ;
        return  head                                 ;
    }
    Record* head = Database_pointer->record_list;
    head = sort_records(head, sorting_key);
    Database_pointer->sorted_based_on = sorting_key;
    return head; 
}

// Division function for seller list
Seller* divide_seller_list(Seller* head) {
    if (!head || !head->next_seller) return NULL;
    
    Seller *slow = head, *fast = head->next_seller;
    while (fast && fast->next_seller) {
        slow = slow->next_seller;
        fast = fast->next_seller->next_seller;
    }
    return slow;
}

// Division function for buyer list
Buyer* divide_buyer_list(Buyer* head) {
    if (!head || !head->next_buyer) return NULL;
    
    Buyer *slow = head, *fast = head->next_buyer;
    while (fast && fast->next_buyer) {
        slow = slow->next_buyer;
        fast = fast->next_buyer->next_buyer;
    }
    return slow;
}

// Merge function for seller list (sorted by Revenue)
Seller* merge_seller_lists(Seller* first, Seller* second) {
    if (!first) return second;
    if (!second) return first;
    
    Seller* merged_head = NULL;
    
    // Compare revenues and merge
    if (first->Revenue <= second->Revenue) {
        merged_head = first;
        merged_head->next_seller = merge_seller_lists(first->next_seller, second);
    } else {
        merged_head = second;
        merged_head->next_seller = merge_seller_lists(first, second->next_seller);
    }
    
    return merged_head;
}

// Merge function for buyer list (sorted by Energy)
Buyer* merge_buyer_lists(Buyer* first, Buyer* second) {
    if (!first) return second;
    if (!second) return first;
    
    Buyer* merged_head = NULL;
    
    // Compare energy amounts and merge
    if (first->Energy <= second->Energy) {
        merged_head = first;
        merged_head->next_buyer = merge_buyer_lists(first->next_buyer, second);
    } else {
        merged_head = second;
        merged_head->next_buyer = merge_buyer_lists(first, second->next_buyer);
    }
    
    return merged_head;
}

// Merge sort for seller list
Seller* merge_sort_seller_list(Seller* head) {
    if (!head || !head->next_seller) return head;
    
    // Find middle point and split list
    Seller* middle = divide_seller_list(head);
    Seller* second_half = middle->next_seller;
    middle->next_seller = NULL;
    
    // Recursively sort both halves
    Seller* first_sorted = merge_sort_seller_list(head);
    Seller* second_sorted = merge_sort_seller_list(second_half);
    
    // Merge the sorted halves
    return merge_seller_lists(first_sorted, second_sorted);
}

// Merge sort for buyer list
Buyer* merge_sort_buyer_list(Buyer* head) {
    if (!head || !head->next_buyer) return head;
    
    // Find middle point and split list
    Buyer* middle = divide_buyer_list(head);
    Buyer* second_half = middle->next_buyer;
    middle->next_buyer = NULL;
    
    // Recursively sort both halves
    Buyer* first_sorted = merge_sort_buyer_list(head);
    Buyer* second_sorted = merge_sort_buyer_list(second_half);
    
    // Merge the sorted halves
    return merge_buyer_lists(first_sorted, second_sorted);
}

// Wrapper functions for use with Database structure
void sort_sellers_by_revenue(Database* db) {
    if (!db || !db->seller_list) return;
    db->seller_list = merge_sort_seller_list(db->seller_list);
}

void sort_buyers_by_energy(Database* db) {
    if (!db || !db->buyer_list) return;
    db->buyer_list = merge_sort_buyer_list(db->buyer_list);
}



// Function to split the pairs list into two halves
Pair* divide_pairs_list(Pair* head) {
    if (!head || !head->next_unique_pair) return NULL;
    
    Pair *slow = head;
    Pair *fast = head->next_unique_pair;
    
    while (fast && fast->next_unique_pair) {
        slow = slow->next_unique_pair;
        fast = fast->next_unique_pair->next_unique_pair;
    }
    
    return slow;
}

// Function to merge two sorted lists of pairs
Pair* merge_pairs_lists(Pair* first, Pair* second) {
    // Handle base cases
    if (!first) return second;
    if (!second) return first;
    
    Pair* merged_head = NULL;
    
    // Compare number of transactions (NOT) and merge accordingly
    if (first->NOT <= second->NOT) {
        merged_head = first;
        merged_head->next_unique_pair = merge_pairs_lists(first->next_unique_pair, second);
    } else {
        merged_head = second;
        merged_head->next_unique_pair = merge_pairs_lists(first, second->next_unique_pair);
    }
    
    return merged_head;
}

// Main merge sort function for pairs list
Pair* merge_sort_pairs_list(Pair* head) {
    // Base case: if list is empty or has only one element
    if (!head || !head->next_unique_pair) return head;
    
    // Find middle point and split list
    Pair* middle = divide_pairs_list(head);
    Pair* second_half = middle->next_unique_pair;
    middle->next_unique_pair = NULL;
    
    // Recursively sort both halves
    Pair* first_sorted = merge_sort_pairs_list(head);
    Pair* second_sorted = merge_sort_pairs_list(second_half);
    
    // Merge the sorted halves
    return merge_pairs_lists(first_sorted, second_sorted);
}

// Wrapper function to use with Database structure
void sort_pairs_list_based_on_number_of_transactions(Database* db) {
    if (!db || !db->pairs_list) {
        printf("No pairs to sort.\n");
        return;
    }
    
    db->pairs_list = merge_sort_pairs_list(db->pairs_list);
}
///////////////////////////////////////////

void initialise_database(Database* database_of_structures)
{
    database_of_structures= (Database*)malloc(sizeof(Database));
    if(database_of_structures)
    {
    // INITIALLY WHEN THE DATABASE STARTED
    database_of_structures->record_list             = NULL     ;
    database_of_structures->seller_list             = NULL     ;
    database_of_structures->buyer_list              = NULL     ;
    database_of_structures->pairs_list              = NULL     ;
    database_of_structures->sorted_based_on         = NULL_KEY ;
    initialise_the_hashtable(database_of_structures)          ;
    }
    else
    {
        printf("DATABASE INITIALISATION FAILED DUE TO NO FREE MEMORY :(");
    }
    //IF WE ALREADY HAVE THE FILE WHICH CONTAINS THE DATABASE
    //FROM THE PREVIOUS RUN PROGRAM INPUTS
    // THEN WE MUST INITIALISE THOSE INTO OUR CURRENT DATABASE? i think
    //!!!!! need to clarify with Sidhu.
    //!!!!! code that abhinay will write afterwards
}


void time_fill(time_stamp *time, int Date, int Month, int Year, int Hours, int Minutes)
{
    time->date    = Date    ;
    time->month   = Month   ;
    time->year    = Year    ;
    time->hours   = Hours   ;
    time->minutes = Minutes ;
}
//filling the timestamp structure 



status_code insert_record_into_record_list(Database *db,int TID,int SID,int BID,float EA,float PPU,time_stamp  *samay,Record** lpptr)
{
    status_code SC  = FAILURE ;
    bool valid_input= validity_checker(db,TID,SID,BID,EA,PPU,samay);
    if(valid_input)
    {
        Record* nptr                        ;
        Record* list_pointer                ;
        SC                   =  SUCCESS     ;
        list_pointer         =  *lpptr      ;
        nptr=(Record*)malloc(sizeof(Record));
        
    
        if(nptr==NULL)
        {
          SC=FAILURE;
        }
        else
        {
         nptr->transaction_id   = TID                  ;
         nptr->buyer_id         = BID                  ;
         nptr->seller_id        = SID                  ;
         nptr->energy_amount    = EA                   ;
         nptr->price_per_unit   = PPU                  ;
         nptr->time.date        = samay->date          ;
         nptr->time.month       = samay->month         ;
         nptr->time.year        = samay->year          ;
         nptr->time.hours       = samay->hours         ;        
         nptr->time.minutes     = samay->minutes       ;
         nptr->transaction_money= PPU*EA               ;
         nptr->transaction_hash = hashing_function(TID);
         nptr->unique_pair_id   = SID * MIN_INPUT_NO + BID   ;



         nptr->next_record      = list_pointer  ;
         list_pointer           = nptr          ;
         *lpptr                 = list_pointer  ;

         
         storing_the_transactionID_along_with_hash(db,TID);
         db->sorted_based_on = NULL_KEY                   ;
        }
    }
    return SC;
}


void populate_buyer_list(Database *db)
{
    if (!db->record_list) return;
    
    Record *current = db->record_list;
    while (current)
    {
        Buyer *bptr = db->buyer_list;
        Buyer *prev = NULL;
        while (bptr && bptr->BID != current->buyer_id)
        {
            prev = bptr;
            bptr = bptr->next_buyer;
        }
        if (!bptr)
        {
            Buyer *new_buyer = (Buyer *)malloc(sizeof(Buyer));
            new_buyer->BID = current->buyer_id;
            new_buyer->Energy = current->energy_amount;
            new_buyer->next_buyer = NULL;
            
            if (prev)
                prev->next_buyer = new_buyer;
            else
                db->buyer_list = new_buyer;
        }
        else
        {
            bptr->Energy += current->energy_amount;
        }
        current = current->next_record;
    }
}
 

void populate_seller_list(Database *db)
{
    if (!db->record_list) return;
    
    Record *current = db->record_list;
    while (current)
    {
        Seller *sptr = db->seller_list;
        Seller *prev = NULL;
        while (sptr && sptr->SID != current->seller_id)
        {
            prev = sptr;
            sptr = sptr->next_seller;
        }
        if (!sptr)
        {
            Seller *new_seller = (Seller *)malloc(sizeof(Seller));
            new_seller->SID = current->seller_id;
            new_seller->Revenue = current->transaction_money;
            new_seller->price_below_300_units = 0.0; // Initialized to some default value(say zero)
            new_seller->price_above_300_units = 0.0; // Initialized to some default value(say zero)
            new_seller->next_seller = NULL;
            new_seller->regular_buyer_list = NULL;
            
            if (prev)
                prev->next_seller = new_seller;
            else
                db->seller_list = new_seller;
        }
        else
        {
            sptr->Revenue += current->transaction_money;
        }
        current = current->next_record;
    }
}


void populate_pairs_list(Database* db) {
    if (!db) return;

    Record* current = db->record_list;
    while (current) {
        unsigned long upid = current->unique_pair_id;
        Pair* pair = db->pairs_list;
        Pair* prev = NULL;

        // Search for existing pair
        while (pair && pair->upid != upid) {
            prev = pair;
            pair = pair->next_unique_pair;
        }

        if (!pair) {
            // Create a new pair if not found
            Pair* new_pair = (Pair*)malloc(sizeof(Pair));
            if (!new_pair) return;

            new_pair->upid = upid;
            new_pair->SID = current->seller_id;
            new_pair->BID = current->buyer_id;
            new_pair->NOT = 1;  // Number of transactions
            new_pair->revenue = current->transaction_money;
            new_pair->next_unique_pair = NULL;

            // Insert into the pairs list
            if (prev) {
                prev->next_unique_pair = new_pair;
            } else {
                db->pairs_list = new_pair;
            }
        } else {
            // Update existing pair
            pair->NOT++;
            pair->revenue += current->transaction_money;
        }

        current = current->next_record;
    }
}


// Function to find the pair with maximum transactions
Pair* find_pair_that_has_maximum_transactions(Database* db) {
    if (!db || !db->record_list) {
        return NULL;
    }
    populate_pairs_list(db);
    // Find pair with maximum transactions
    Pair* current_pair = db->pairs_list;
    Pair* max_pair = NULL;
    int max_transactions = 0;
    
    while (current_pair) {
        if (current_pair->NOT > max_transactions) {
            max_transactions = current_pair->NOT;
            max_pair = current_pair;
        }
        current_pair = current_pair->next_unique_pair;
    }
    
    return max_pair;
}


// Function to calculate revenue of a seller
float revenue_of_seller(Database* db, int sid) {
    if (!db || !db->record_list) {
        printf("No transaction records available.\n");
        return 0.0f;
    }
    
    float revenue = 0.0f;
    bool seller_found = false;
    Record* current = db->record_list;
    
    while (current) {
        if (current->seller_id == sid) {
            revenue += current->transaction_money;
            seller_found = true;
        }
        current = current->next_record;
    }
    
    if (!seller_found) {
        printf("The seller (ID: %d) does not exist in the transaction records.\n", sid);
    }
    
    return revenue;
}


// Function to fill pairs list based on transactions
void filling_pairs(Database* db) {
    if (!db || !db->record_list) {
        printf("No records available to create pairs.\n");
        return;
    }
    // First, sort records based on unique pair ID
    db->record_list = sort_records_wrapper(db, UNIQUE_PAIR_ID);
    // Clear existing pairs list
    while (db->pairs_list) {
        Pair* temp = db->pairs_list;
        db->pairs_list = db->pairs_list->next_unique_pair;
        free(temp);
    }
    db->pairs_list = NULL;

    Record* current = db->record_list;
    Pair* current_pair = NULL;
    unsigned long current_unique_pair_id = -1;

    while (current) {
        if (current->unique_pair_id != current_unique_pair_id) {
            // Create new pair
            Pair* new_pair = (Pair*)malloc(sizeof(Pair));
            if (!new_pair) {
                printf("Memory allocation failed for new pair!\n");
                return;
            }

            new_pair->BID = current->buyer_id;
            new_pair->SID = current->seller_id;
            new_pair->upid = current->unique_pair_id;
            new_pair->revenue = current->transaction_money;
            new_pair->NOT = 1;
            new_pair->next_unique_pair = NULL;

            // Add to pairs list
            if (!db->pairs_list) {
                db->pairs_list = new_pair;
            } else {
                current_pair->next_unique_pair = new_pair;
            }
            current_pair = new_pair;
            current_unique_pair_id = current->unique_pair_id;
        } else {
            // Update existing pair
            current_pair->NOT++;
            current_pair->revenue += current->transaction_money;
        }
        
        current = current->next_record;
    }
}


status_code update_existing_transaction(Database* db, int TID, float EA, float PPU, time_stamp samay)
{
    if (!db) {
        printf("Error: Database pointer is NULL.\n");
        return FAILURE;
    }

    status_code done = FAILURE;
    Record* head = db->record_list;
    
    bool valid = is_transaction_input_valid(TID) && is_priceperunit_input_valid(PPU) && is_energy_input_valid(EA);
    bool already_present = !(is_transaction_id_input_unique(db, TID)) && is_time_input_valid(samay);

    if (!head) {
        printf("The database is Empty. Cannot update transaction.\n");
    }
    else if (!valid) {
        printf("The input details are not valid! No changes were made.\n");
    }
    else if (valid && !already_present) {
        printf("Any transaction with the TransactionID: %d does not exist in our Database.\n", TID);
    }
    else if (valid && already_present) {
        Record* node_pointer = head;
        while (node_pointer && node_pointer->transaction_id != TID) {
            node_pointer = node_pointer->next_record;
        }
        
        if (!node_pointer) {
            printf("Transaction ID: %d not found.\n", TID);
            return FAILURE;
        }

        node_pointer->energy_amount = EA;
        node_pointer->price_per_unit = PPU;
        node_pointer->time.date = samay.date;
        node_pointer->time.month = samay.month;
        node_pointer->time.year = samay.year;
        node_pointer->time.hours = samay.hours;
        node_pointer->time.minutes = samay.minutes;
        node_pointer->transaction_money = PPU * EA ;
        db->sorted_based_on = NULL_KEY ;
        done = SUCCESS;
    }

    return done;
}

void fill_the_regular_buyers_list_in_seller_nodes(Database *db) {
    if (!db || !db->pairs_list) return;

    Pair *current_pair = db->pairs_list;
    
    while (current_pair) {
        if (current_pair->NOT >= FREQUENT_BUYING_THRESHOLD) {
            // Find the seller in the seller list
            Seller *seller = db->seller_list;
            while (seller && seller->SID != current_pair->SID) {
                seller = seller->next_seller;
            }

            if (seller) {
                // Check if the buyer is already in the regular buyer list
                Buyer *buyer = seller->regular_buyer_list;
                Buyer *prev = NULL;
                while (buyer && buyer->BID != current_pair->BID) {
                    prev = buyer;
                    buyer = buyer->next_buyer;
                }

                // If not found, add new buyer to the regular buyer list
                if (!buyer) {
                    Buyer *new_buyer = (Buyer *)malloc(sizeof(Buyer));
                    if (!new_buyer) return;

                    new_buyer->BID = current_pair->BID;
                    new_buyer->Energy = 0;  // We can update this later if needed
                    new_buyer->next_buyer = NULL;

                    if (prev) {
                        prev->next_buyer = new_buyer;
                    } else {
                        seller->regular_buyer_list = new_buyer;
                    }
                }
            }
        }
        current_pair = current_pair->next_unique_pair;
    }
}


void fill_the_price_per_unit_fields(Database *db) {
    if (!db || !db->record_list) return;

    Record *current_record = db->record_list;
    
    while (current_record) {
        // Find the seller in the seller list
        Seller *seller = db->seller_list;
        while (seller && seller->SID != current_record->seller_id) {
            seller = seller->next_seller;
        }

        if (seller) {
            // Store any recorded price in the seller's structure
            if (current_record->energy_amount <= 300) {
                seller->price_below_300_units = current_record->price_per_unit;
            } else {
                seller->price_above_300_units = current_record->price_per_unit;
            }
        }
        current_record = current_record->next_record;
    }
}


///////////////////////////

void clear(Database *db) {
    if (!db) return;

    // Free the Record list
    Record *current_record = db->record_list;
    while (current_record) {
        Record *temp = current_record;
        current_record = current_record->next_record;
        free(temp);
    }
    db->record_list = NULL;

    // Free the Seller list
    Seller *current_seller = db->seller_list;
    while (current_seller) {
        // Free the regular_buyer_list for each seller
        Buyer *current_buyer = current_seller->regular_buyer_list;
        while (current_buyer) {
            Buyer *temp = current_buyer;
            current_buyer = current_buyer->next_buyer;
            free(temp);
        }
        Seller *temp = current_seller;
        current_seller = current_seller->next_seller;
        free(temp);
    }
    db->seller_list = NULL;

    // Free the Buyer list
    Buyer *current_buyer = db->buyer_list;
    while (current_buyer) {
        Buyer *temp = current_buyer;
        current_buyer = current_buyer->next_buyer;
        free(temp);
    }
    db->buyer_list = NULL;

    // Free the Pair list
    Pair *current_pair = db->pairs_list;
    while (current_pair) {
        Pair *temp = current_pair;
        current_pair = current_pair->next_unique_pair;
        free(temp);
    }
    db->pairs_list = NULL;

    // Free the hash table
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        transaction_hash_node *current_node = db->hashTable[i];
        while (current_node) {
            transaction_hash_node *temp = current_node;
            current_node = current_node->next;
            free(temp);
        }
        db->hashTable[i] = NULL;
    }
    Database* temp = db ;
    free(db);
    temp = NULL;
}

///////////////////////////
///////////////////////////

int main() {
    // Initialize the database
    Database db;
    initialise_database(&db);

    // Initialize some transactions
    time_stamp ts1, ts2, ts3, ts4, ts5, ts6, ts7, ts8, ts9, ts10, ts11, ts12;

    time_fill(&ts1, 21, 12, 2023, 2, 23);
    time_fill(&ts2, 12, 12, 2023, 6, 45);
    time_fill(&ts3, 05, 1, 2024, 14, 30);
    time_fill(&ts4, 18, 2, 2023, 9, 15);
    time_fill(&ts5, 7, 4, 2022, 11, 50);
    time_fill(&ts6, 30, 06, 2023, 22, 10);
    time_fill(&ts7, 9, 7, 2024, 5, 5);
    time_fill(&ts8, 15, 8, 2023, 17, 40);  
    time_fill(&ts9, 1, 9, 2022, 8, 55);
    time_fill(&ts10, 27, 10, 2023, 12, 20);
    time_fill(&ts11, 3, 11, 2024, 16, 35);
    time_fill(&ts12, 22, 5, 2023, 20, 0);


    // Insert some initial records
    insert_record_into_record_list(&db, 123455, 123455, 123400, 18.5, 20.0, &ts1 , &db.record_list);
	insert_record_into_record_list(&db, 164832, 356872, 176581, 57.9, 15.0, &ts2 , &db.record_list);
	insert_record_into_record_list(&db, 234551, 536806, 304937, 57.9, 15.0, &ts3 , &db.record_list);
	insert_record_into_record_list(&db, 300002, 200030, 100004, 18.5, 20.0, &ts4 , &db.record_list);
	insert_record_into_record_list(&db, 456783, 654311, 789125, 45.2, 30.0, &ts5 , &db.record_list);
	insert_record_into_record_list(&db, 567898, 987644, 123780, 78.1, 12.5, &ts6 , &db.record_list);
	insert_record_into_record_list(&db, 678902, 345686, 567127, 33.3, 25.4, &ts7 , &db.record_list);
	insert_record_into_record_list(&db, 789018, 123469, 678911, 91.2, 10.1, &ts8 , &db.record_list);
	insert_record_into_record_list(&db, 890123, 234573, 789349, 66.7, 44.8, &ts9 , &db.record_list);
	insert_record_into_record_list(&db, 901237, 345686, 890562, 54.5, 28.9, &ts10, &db.record_list);
	insert_record_into_record_list(&db, 345677, 456794, 901786, 99.9, 05.0, &ts11, &db.record_list);
	insert_record_into_record_list(&db, 456783, 567802, 123890, 12.3, 32.1, &ts12, &db.record_list);


    int choice;
    time_stamp start_time, end_time;

    printf("\n=== Energy Trading Record Management System ===\n");

    do {
        printf("\nMain Menu:\n");
        printf("1. Add New Transaction\n");
        printf("2. Display All Transactions\n");
        printf("3. View Transactions in Time Period\n");
        printf("4. View Transaction with Highest Energy Amount\n");
        printf("5. View Seller/Buyer Statistics\n");
        printf("6. View Pair Analysis\n");
        printf("7. Update existing Transaction.\n");
        printf("0. Exit\n");
        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                int tid, sid, bid;
                float energy_amount, price_per_unit;
                time_stamp transaction_time;

                printf("\nEnter Transaction Details:\n");
                printf("Transaction ID: ");
                scanf("%d", &tid);
                printf("Seller ID: ");
                scanf("%d", &sid);
                printf("Buyer ID: ");
                scanf("%d", &bid);
                printf("Energy Amount (kWh): ");
                scanf("%f", &energy_amount);
                printf("Price per kWh: ");
                scanf("%f", &price_per_unit);
                printf("Enter Date (DD MM YYYY HH MM): ");
                scanf("%d %d %d %d %d",
                      &transaction_time.date,
                      &transaction_time.month,
                      &transaction_time.year,
                      &transaction_time.hours,
                      &transaction_time.minutes);

                if (insert_record_into_record_list(&db, tid, sid, bid, energy_amount, price_per_unit, &transaction_time, &db.record_list) == SUCCESS) {
                    printf("Transaction added successfully!\n");
                } else {
                    printf("Failed to add transaction. Please check the input.\n");
                }
                break;
            }

            case 2:
                display_records(&db);
                break;

            case 3: {
                printf("\nEnter one Input Date (DD MM YYYY HH MM): ");
                scanf("%d %d %d %d %d",
                      &start_time.date,
                      &start_time.month,
                      &start_time.year,
                      &start_time.hours,
                      &start_time.minutes);

                printf("Enter Another Input Date (DD MM YYYY HH MM): ");
                scanf("%d %d %d %d %d",
                      &end_time.date,
                      &end_time.month,
                      &end_time.year,
                      &end_time.hours,
                      &end_time.minutes);

                display_records_between_time(&db, start_time, end_time);
                break;
            }

            case 4:
                display_transaction_with_maximum_energy_amount(&db);
                break;

            case 5: {
                
                int sub_choice;
                printf("\nSeller/Buyer Statistics:\n");
                printf("1. View Sorted Seller List (by Revenue)\n");
                printf("2. View Sorted Buyer List (by Energy)\n");
                printf("3. Query Revenue of a Seller\n");
                printf("Enter choice: ");
                scanf("%d", &sub_choice);

                if (sub_choice == 1) {
                    populate_seller_list(&db)                        ;
                    populate_pairs_list(&db)                         ;
                    fill_the_regular_buyers_list_in_seller_nodes(&db);
                    fill_the_price_per_unit_fields(&db)              ;
                    sort_sellers_by_revenue(&db)                     ;
                    display_sellers_list(&db)                        ;
                } else if (sub_choice == 2) {
                    printf("Buyers list sorted and diplayed as per Energy Amount is:\n\n");
                    populate_buyer_list(&db)  ;
                    sort_buyers_by_energy(&db);
                    display_buyers_list(&db)  ;
                } else if (sub_choice == 3) {
                    populate_seller_list(&db)                        ;
                    populate_pairs_list(&db)                         ;
                    fill_the_regular_buyers_list_in_seller_nodes(&db);
                    fill_the_price_per_unit_fields(&db)              ;
                    int sid;
                    printf("Enter Seller ID: ");
                    scanf("%d", &sid);
                    printf("The revenue of this seller is: %.2f\n", revenue_of_seller(&db, sid));
                } else {
                    printf("Invalid choice. Please try again.\n");
                }
                break;
            }

            case 6: {
                int sub_choice;
                printf("\nPair Analysis:\n");
                printf("1. Find Pair with Maximum Transactions\n");
                printf("2. View Pairs Sorted by Transaction Count\n");
                printf("Enter choice: ");
                scanf("%d", &sub_choice);

                filling_pairs(&db);

                if (sub_choice == 1) {
                    Pair* max_pair = find_pair_that_has_maximum_transactions(&db);
                    if (max_pair) {
                        printf("Pair with maximum transactions: SID-%d, BID-%d\n",
                               max_pair->SID, max_pair->BID);
                    } else {
                        printf("No transactions available.\n");
                    }
                } else if (sub_choice == 2) {
                    sort_pairs_list_based_on_number_of_transactions(&db);
                    display_sorted_pairs(&db);
                } else {
                    printf("Invalid choice. Please try again.\n");
                }
                break;
            }
            case 7: {
                int TID          ;
                float PPU,EA     ;
                time_stamp samay ;
                printf("Please input The Transaction ID for which you wish to update the information:");
                scanf("%d",&TID) ;
                printf("Please input new parameters ENERGY_AMOUNT: ");
                scanf("%f",&PPU) ;
                printf("Please input new parameters PRICE_PER_UNIT:");
                scanf("%f",&EA ) ;
                printf("Enter Date (DD MM YYYY HH MM): ");
                scanf("%d %d %d %d %d",
                      &samay.date  ,
                      &samay.month ,
                      &samay.year  ,
                      &samay.hours ,
                      &samay.minutes);
                status_code sc = update_existing_transaction(&db,TID,EA,PPU,samay);
                if(sc == SUCCESS)
                {
                    printf("Transaction %d has successfully been updated.\n",TID);
                }
                else//sc= FAILURE
                {
                    printf("ERROR: Transaction updation was failed!");
                }
                break;
            }
            case 0:
                // Free allocated memory before exiting
                printf("\nExiting program in 1..2..3...EXITED:)\n");
                clear(&db);
                break;

            default:
                printf("\nInvalid choice. Please try again :(\n");
        }

    } while (choice != 0);
    return 0;
}