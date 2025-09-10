#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <mutex>
int global_int {0};
void increment_int(int &global) {
    for (int i{0}; i < 100000; ++i) {
        global++;
    }
    std::cout << global << std::endl;
}
void hello(int a) {
    std::cout << "hello from thread " << a << std::endl;
}
void hola(int b) {
    std::cout << "hola from thread " << b << std::endl;
}

void bonjour(int c) {
    std::cout << "bonjour from thread " <<  c << std::endl;
}

void hello1(int num) {
    // Add a delay
    std::this_thread::sleep_for(std::chrono::seconds(num));
    std::cout << "Hello from thread " << num << std::endl;
}

void print(std::string str) {
    // a very artificial way to display a string!
    for (int i = 0; i < 5; ++i) {
        std::cout << str[0] << str[1] << str[2] << std::endl;
    }
}

std::mutex protect;
void enter(std::vector<int> &vec1) {
    for (int i{0}; i < vec1.size(); ++i) {
        protect.lock();
        std::cout << vec1[i];
    }
    std::cout << std::endl;
    protect.unlock();
}

/*
 * Starting multiple threads
 * - We can start multiple threads
 *          std::thread thr1(hello)
 *          std::thread thr2(hello)
 *          std::thread thr3(hello)
 *
 *          // Wait for them to finish
 *
 *          thr1.join();
 *          thr2.join();
 *          thr3.join();
 *          */

/*
 * Data Sharing Between Threads
 * - The threads in a program share the same memory space
 *      - it is very easy to share data between the threads
 * - The only requirement is that the data is visible to the thread functions
 *      - Global or static variable, for global thread functions
 *      - Static class member, for class member thread functions
 *      - Local variable captured by lambda expressions (by reference) to ensure each thread is working with the same object, if captured by value then all threads would have a different clone and won't see changes.
 *      */

/*
 * - Threads interleave their execution
 * - Threads can interfere with each other's actions
 * - Modifying shared data can cause data corruption
 *      - This is the main source of bug in concurrent programs
 *      */

/*
 * Data Race
 * - A "data race" occurs when:
 *      - Two or more threads access the same memory location
 *      - And at least one of the threads modifies it
 *      - Potentially conflicting accesses to the same memory location
 *
 * - Only safe if the threads are synchronized
 *      - One thread accesses the memory location at a time
 *      - The other threads have to wait until it is safe for them to access it
 *      - In effect, the threads execute sequentially while they access it
 *
 * - A data race causes undefined behaviour
 *      - The program is not guaranteed to behave consistently.
 *      */

/*
 * Race Condition
 * - The outcome is affected by timing changes
 *      - e.g. One client clears a database table
 *      - Another client inserts an entry into the same table
 *
 * - A data race is a special case of a race condition
 *      - The outcome depends on when the threads are scheduled to run
 *      */

/*
 * Memory Location
 * - In C++, a "memory location" is a scalar object
 *          - A built-in variable
 *          - A pointer
 *          - An element in a container
 *
 * - Or a scalar sub-object
 *      - A struct or class member which is a scalar object
 *
 * - Also an obscure case:
 *      - A series of contiguous bitfields within the same word
 *      - Unless they contain a zero-length bitfield(!)
 *      */

/*
 * Compound Objects
 * - C++ STL containers are memory locations
 *      - Multiple threads modifying the same object may conflict
 *      - Should be synchronized
 *
 * - For our own types, we can choose the behaviour
 *      - Classes can provide their own synchronization
 *      - Easier to work with
 *      - Calling a sequence of member functions may be problematic
 *      - Usually better to implement them as memory locations
 *      */

/*
 * Data Race Consequences
 * - In this program, the data race caused interleaved output
 *      - std::cout is a special case
 *      - Nothing worse than output interleaving can happen
 *
 * - The consequence of a data race can be much more serious
 *      - Incorrect results
 *      - Incorrect program flow
 *      - "Torn" writes and reads
 *      - Objects which are improperly constructed or destroyed
 *      */

/*
 * Incorrect Results
 *
 *          int x = 5; // Shared variable x
 *          int y = 2*x+3; // Calculate the value of y(1)
 *          int z = 3*x=2; // calculate the value of z(2)
 *
 * - Thread A evaluates y
 *      - Thread B interleaves and changes the value of x to 6
 *      - Thread A uses the new value of x to evaluate z
 *      - the calculated values of y and z are inconsistent
 *
 * - This can even occur inside a statement
 *      y = x*x+2*x+1; // Another thread could change the value of x
 *      */

/*
 * Torn Writes and Reads
 * - A "torn" write
 *      - Can occur when writing data requires operations
 *      - Another writing thread interleaves between the write operations
 *
 *              Threads A nad B write to a memory location
 *              Thread A starts writing 0x1234
 *              Thread B interleaves and writes 0x4567
 *              Thread A continues writing 0x1234
 *              The memory location contains part of Thread A's number and part of Thread B's number
 *              */

/*
 * Torn Writes and Reads
 * - A "torn" read
 *      - Can occur when reading and writing requires multiple operations
 *      - A writing thread interleaves between the read operations
 *
 *              Threads A reads from a memory location
 *              Thread A starts reading 0x4321
 *              Thread B interleaves and writes 0x9876
 *              Thread A continues reading
 *              Thread A has read 0x4376
 *              Thread A has read part of the original value and part of the new value
 *              */

/*
 * Improper Construction
 * - Thread A creates a shared object
 *      - it calls the class's constructor
 * - Thread B interleaves
 *      - Thread B also calls the class's constructor, at the same memory location
 * - We have torn write
 *      - The object will be initialized with a mixture of values from each thread*/

/*
 * Improper Construction
 * - Partially constructed object
 *      // Vector of polymorphic objects
 *      std::vector<Base *> vec;
 *
 *      // The Base constructor adds the object to the vector
 *      Base::Base() {
 *          vec.push_back(this);
 *      }
 *
 *      // The Derived constructor calls the Base constructor first
 *      // then it initializes the Derived members
 *      Derived::Derived() : Base(),....
 *      */

/*
 * Improper Construction
 * - Thread A calls Derived's constructor
 *      - Derived's constructor calls Base's constructor
 *      - Base's constructor pushes the Base part of the object onto the vector
 *
 * - Thread B interleaves
 *      - Thread B accesses the element in vec
 *      - The Derived constructor has not completed
 *      - Thread B will see a partially constructed object
 * - Thread A initializes the Derived part of the object
 * */

/*
 * Improper Destruction
 *
 * - Destructor of reference-counted object
 *          *ref_count--;
 *          if (*ref_count == 0) {
 *              delete p;
 *              delete ref_count;
 *           }
 *
 *
 *
 *  - Thread A decrements the counter
 *         - Thread B interleaves and decrements the counter
 *         - Thread A checks the counter and may release the members
 *         - Thread B checks the counter and may release the members
 *         */

/*
 * Improper Destruction
 * - if ref_count was initially 2
 *      - p and ref_count could be deleted twice
 *
 * - if ref_count was initially 1
 *      - p and ref_count may not be deleted at all
 *      */

/*
 * Managing Data Races
 * - There are no "benign" data races
 *      - All data races in C++ are potentially dangerous
 * - Can be very difficult to detect and replicate
 *      - Intermittent errors
 *      - Sensitive to environments
 *      - Often dependent on timing coincidences or system load
 *
 * - The only good solution is to prevent data from occurring
 * */

/*
 * Shared Data
 * - Avoid sharing data between different threads
 * - If unavoidable, synchronize the threads
 *      - Impose an ordering on how the threads access the shared data
 * - This has substantial costs
 *      - Increased execution time
 *      - Increased program complexity
 *      */



int main() {
//    std::cout << "Hello, World!" << std::endl;
//
//
//    // Start 3 threads
//    std::cout << "starting 3 threads:" << std::endl;
//    std::thread thread1(hello1, 1);
//    std::thread thread2(hello1, 2);
//    std::thread thread3(hello1, 3);
//
//    // wait for the threads to finish
//    thread1.join();
//    thread2.join();
//    thread3.join();

//    std::thread thr1(print, "abc");
//    std::thread thr2(print, "def");
//    std::thread thr3(print, "xyz");
//
//    // wait for tasks to complete
//    thr1.join();
//    thr2.join();
//    thr3.join();


//    int num1 {0};
//    int num2{0};
//    int num3{0};
//
//    std::thread thr4(increment_int, std::ref(num1));
//    std::thread thr5(increment_int, std::ref(num2));
//    std::thread thr6(increment_int, std::ref(num3));
//
//
//    thr4.join();
//    thr5.join();
//    thr6.join();
//
//
//    std::cout << num1 << std::endl;

    std::vector<int> nums{1,2,3,4,5,6};
    std::thread practice1(enter, std::ref(nums));
    std::thread practice2(enter, std::ref(nums));
    std::thread practice3(enter, std::ref(nums));

    practice1.join(); practice2.join(); practice3.join();


    return 0;
}
