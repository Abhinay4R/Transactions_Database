# Console-Based Transaction Management System

This repository contains a C implementation of a console-based transaction management system developed as a coursework project. The system is designed to process and manage a large number of transactions, with a strong focus on data integrity, input validation, and optimized data retrieval.

## Project Backstory

This project was a college coursework assignment that quickly evolved into a deeper learning experience. It provided a hands-on opportunity to apply concepts from discrete mathematics, particularly in the implementation of the Verhoeff-Gumm algorithm for robust data validation. The process of building this system from the ground up was a journey into practical debugging, custom benchmarking, and various testing techniques. The project sparked a significant interest in me regarding efficient data structures and algorithms, which I continued to explore even after its submission.

## Key Features

  * **Verhoeff-Gumm Algorithm:** Implemented a full validation algorithm based on the Verhoeff-Gumm checksum, a discrete mathematics concept, to ensure the integrity of transaction, seller, and buyer IDs. [cite\_start]This algorithm provides an early check for data entry errors before querying the main database, significantly improving performance[cite: 1, 2, 3].
  * [cite\_start]**Custom Hashing:** A custom hashing function is used to store transaction IDs in a hash table with chaining, enabling constant-time `O(1)` lookups to check for duplicate IDs[cite: 1].
  * [cite\_start]**Optimized Data Management:** The system uses linked lists for managing transaction records, sellers, and buyers, along with a `Database` struct to hold all the data structures[cite: 1].
  * [cite\_start]**Extensive Benchmarking:** A separate C++ file is included to benchmark the Verhoeff-Gumm algorithm against other simpler checksum methods, demonstrating its superior error detection capabilities[cite: 3].
  * **Readability and Understandability:** The code is well-structured and commented, making it easy to read and understand the underlying logic and algorithms.

## Technical Details & Implementation Nitpicks

### Verhoeff-Gumm Implementation

[cite\_start]The validation logic is based on the Verhoeff-Gumm algorithm, which uses a non-commutative multiplication table (D5 group) and a dihedral permutation table[cite: 1, 2].

  * [cite\_start]**Tables:** The core of the algorithm relies on two static tables, `permuatation_table` and `d5_mult_table`, which define the operations for calculating the check digit[cite: 2].
  * [cite\_start]**Check Digit Calculation:** A `calculate_check_digit` function is used to generate the checksum digit for a 5-digit number[cite: 2].
  * [cite\_start]**Input Validation:** The `validity_checker` function combines the Verhoeff-Gumm check with other validations like time range, uniqueness (using the hash table), and positive number checks for energy and price[cite: 1].

### Data Structures & Algorithms

The system is built on a foundation of efficient data structures to manage transactions effectively.

  * [cite\_start]**Linked Lists:** All main data entities—`Record`, `Seller`, `Buyer`, and `Unique_pair`—are implemented as linked lists, allowing for dynamic allocation and management of data[cite: 1].
  * [cite\_start]**Hash Table:** A hash table with a size of `100003` (a large prime number) is used to store `transaction_id`s, ensuring that the `is_transaction_id_input_unique` check is highly performant[cite: 1].
  * [cite\_start]**Sorting Algorithms:** The program includes a full implementation of `merge_sort` to sort various lists (records, sellers, buyers, and pairs) based on different criteria like `MONTHS`, `SELLER_ID`, `BUYER_ID`, `ENERGY`, and `TIME`[cite: 1].
  * [cite\_start]**Input/Output:** The `project2.c` file processes a hardcoded input from `input1.txt` and generates the corresponding output in `output1.txt`, demonstrating the system's functionality[cite: 1, 4, 5].

## Technologies Used

  * [cite\_start]**C:** The core programming language for the transaction management system[cite: 1].
  * [cite\_start]**C++:** Used for the dedicated benchmarking code to evaluate algorithm performance[cite: 3].
  * **Discrete Mathematics:** The theoretical foundation for the Verhoeff-Gumm algorithm used for validation.
  * **Data Structures:** Implementation of linked lists, hash tables, and arrays.

## Getting Started

### Prerequisites

You need a C compiler (e.g., `gcc` or `clang`) to compile and run the source code.

### Installation and Compilation

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/your-username/your-repo-name.git
    cd your-repo-name
    ```
2.  **Compile the source code:**
    ```bash
    gcc -o project2 project2.c utility_verhoeff_gumm.c -std=c11
    ```
3.  **Compile the benchmarking code (optional):**
    ```bash
    g++ -o benchmarks verhoeffmann-benchmarks.cpp -std=c++17
    ```

### Usage

1.  **Run the compiled executable:**
    ```bash
    ./project2
    ```
2.  The program will process the data from `input1.txt`, manage the transactions, and provide various display options.

## License

This project is licensed under the MIT License - see the `LICENSE.md` file for details.

## Contact

If you have any questions or feedback, feel free to reach out:

  * [cite\_start]**Name:** Abhinay Ragam [cite: 6]
  * **Email:** [abhinayragam@gmail.com](mailto:abhinayragam@gmail.com)
  * **LinkedIn:** [https://www.linkedin.com/in/abhinay-ragam-1a7a99295/]

-----
