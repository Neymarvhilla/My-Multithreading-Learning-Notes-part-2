#include <iostream>

#include <cstring>
#include <chrono>
#include <vector>
#include <thread>
#include <mutex>
#include <shared_mutex>


/*
 * - Financial data feed for infrequently traded stocks
 *      - Constantly accessed to get the latest price
 *      - The price rarely changes
 *
 * - Audio/video buffers in multimedia players
 *      - Constantly accessed to get the next frame
 *      - Occasionally updated with a block of data
 *
 * - Shared data
 *      - Must protect against a data race
 *      */

/*
 * Data Race
 * - A data race occurs when:
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
 *      - The program is not guaranteed to behave consistently
 *      */

/*
 * - Concurrent Accesses:
 * - High probability of a reader and another reader
 *      - No locking required
 * - Low probability of a writer and reader
 *      - locking required
 * - Low probability of a writer and another writer
 *      - Locking required
 *      */

/*
 * - With std::mutex, all threads are synchronized
 * - They must execute their critical sections sequentially
 *      -Even when it is not necessary
 * - Loss of concurrency reduces performance
 * */

/*
 * Read-write Lock
 * - It would be useful to have "selective" locking
 *      - Lock when there is a thread which is writing
 *      - Do not lock when there are only reading threads
 *      - Often called a "read-write lock"
 *      */


/*
 * std::shared_mutex
 * - std::shared_mutex is defined in <shared_mutex>
 * - It can be locked in two different ways:
 * - Exclusive lock
 *      - No other thread may acquire a lock
 *      - No other thread can enter a critical section
 *
 * - Shared lock
 *      - Other threads may acquire a shared lock
 *      - They can execute critical sections concurrently*/

/*
 * Exclusive lock
 * - std::lock_guard<std::shared_mutex>
 * - std::unique_lock_std::shared_mutex>
 * - Only this thread can execute a critical section
 *      - Other threads must wait until the thread releases its exclusive lock
 * - It can only acquire an exclusive lock when the mutex is unlocked
 *      - If other threads have shared or exclusive locks
 *      - This thread must wait until all the locks are released
 *      */

/*
 * - std::shared_lock<std::shared_mutex>
 * - A thread which has a shared lock can enetr a critical section
 * - It can only acquire a shared lock if there are no exclusive locks
 *      - if another thread has an exclusive lock
 *      - This thread must wait until the exclusive lock is released
 * */


/*
 * shared_mutex usage
 *
 *          std::shared_mutex shmut;
 *
 *          void write()
 *          {
 *              std::lock_guard lck_guard(shmut); // Write thread with exclusive lock
 *              .....
 *           }
 *
 *           void read()
 *           {
 *              std::shared_lock sh_lck(shmut); // Read with shared lock
 *              ....
 *           }*/

/*
 * std::shared_mutex
 * Member Functions
 *
 * - Exclusive locking
 *      - lock()
 *      - try_lock()
 *      - unlock()
 *
 *
 * - Shared locking
 *      - lock_shared()
 *      - try_lock_shared()
 *      - unlock_shared()
 *      */

/*
 * Data Race Avoidance
 * - The writer thread cannot get an exclusive lock
 *      - Until all other threads release their locks
 *      - Those threads have now left their critical sections
 *
 * - The writer thread acquires an exclusive lock
 *      - It enters the critical section
 *      - Reader threads cannot get a shared lock
 *      - Writer threads cannot get an exclusive lock
 *      - Until this thread releases its lock
 *
 * - The writer thread releases its exclusive lock
 *      - it has now left its critical section
 *      */

/*
 * Data Race Avoidance
 * - The reader thread cannot get a shared lock
 *      - Until a writer thread releases its exclusive lock
 *      - The writer thread has now left its critical section
 *
 * - The reader thread acquires a shared lock
 *      - It enters the critical section
 *      - Other reader threads can also get a shared lock
 *
 * - There is no scenario in which there is a data race
 *      - Reader and writer threads cannot interleave in a critical section
 *      */

/*
 * Pros and Cons of std::shared_mutex
 * - Uses more memory than std::mutex
 * - Slower than std::mutex
 * - Best suited to situations where
 *      - Reader threads greatly outnumber writer threads
 *      - Read operations take a long time
 *      */

/*
 * Shared Data
 * - Global Variable
 *      - Accessible to all code in the program
 * - Static Variable at namespace scope
 *      - Accessible to all code in the translation unit
 * - Class member which is declared static
 *      - Potentially accessible to code which calls its member functions
 *      - (if public, accessible to all code)
 * - Local variable which is declared static
 *      - Accessible to all code which calls that function
 *      */

/*
 * Shared Data Initialization
 * - Global variable
 * - Static variable at namespace scope
 * - Static data member of class
 *      - All are initialized when the program starts
 *      - At that point, only one thread is running
 *      - There cannot be a data race
 *      */

/*
 * Static Local Variable
 * - Initialized after the program starts
 * - When the declaration is reached
 *          void func() {
 *              ...
 *              // static local variable
 *              static std::string str("xyz");
 *              ....
 *          }
 *
 * - Two or more threads may call the constructor concurrently
 * - Is there a data race?
 * */

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
 *      - The program is not guaranteed to behave consistently
 *      */

/*
 * Static Local Variable Initialization Before C++11
 * - No language support
 *      - the behaviour was defined
 * - Lock a mutex?
 *      - Required on every pass through the declaration
 *      - Very inefficient
 *      */

/*
 * Static Local Variable Initialization in C++11
 * - The behaviour is now well-defined
 * - Only one thread can initialize the variable
 *      - Any other thread that reaches the declaration is blocked
 *      - Must wait until the first thread has finished initializing the variable
 *      - The threads are synchronized by the implementation
 *      - No data race
 *
 * - Subsequent modifications
 *      - The usual rules for shared data
 *      - There will be a data race, unless we protect against one
 *      */

/*
 * Singleton Class
 * - Used to implement the Singleton design pattern
 * - A singleton class has only a single instance
 *      - e.g. a logger class that maintains an audit trail
 * - Its constructor is private
 * - The copy and move operators are deleted
 *      - The program cannot create more objects
 * - A static member function returns the unique instance
 *      - if the instance does not already exist, it is created and Initialized
 *      - Otherwise, the existing object is returned
 *      */

/*
 *      class Singleton {
 *          // Pointer to the unique instance
 *          static Singleton *single;
 *
 *      public:
 *          // Static member function which returns the unique instance
 *          static Singleton *get_singleton() {
 *              if (single == nullptr)
 *                  single = new Singleton;
 *              return single;
 *          }
 *          .... // Class functionality
 *     }
 *
 *     - This has a data race
 *     */

/*
 * - The first thread to reach the definition creates the unique instance
 *      - subsequent threads use the object created by the first thread
 *      - The object remains in existence until the program terminates
 *      */

/*
 * Thread local variables
 * - C++ supports thread-local variables
 *      - Same as static and global variables
 *      - However, there is a separate object for each thread
 *      - With a static variable, there is a single object which is shared by all threads
 * - We use the thread_local keyword to declare them
 * */

/*
 * Thread-local variables
 * - Global variables or at namespace scope
 * - Data members of a class
 * - Local variables in a function
 * */
/*
 * - Global and namespace scope
 *      - Always constructed at or before the first use in a translation unit
 *      - It is safe to use them in dynamic libraries (DLLs)
 *
 * - Local variables
 *      - Initialized in the same way as static local variables
 * - In all cases
 *      - Destroyed when the thread completes execution
 *      */

/*
 * - We can make a random number engine thread-local
 *      - This gives each thread its own object
 *
 * - This ensures that each thread generates the same sequence
 *      - Useful for testing
 *
 *          // Thread-local random number engine
 *          std::thread_local mt19937 mt;
 *
 *          void func()
 *          {
 *              std::uniform_real_distribution<double> dist(0,1);
 *
 *              for (int i =0; i < 10; ++i)
 *                  std::cout << dist(mt) << ", ";
 *              }*/

/*
 * */
char * reverse_string(char *str) {
    size_t size = std::strlen(str) + 1;
    char * new_string = new char[size];
    int size_for_loop = std::strlen(str) - 1;
    for (int i {size_for_loop}; i >= 0; --i) {
        new_string[size_for_loop - i] = str[i];
    }

    for (size_t i {0}; i < std::strlen(new_string); ++i) {
        str[i] = new_string[i];
    }
    delete [] new_string;
    return str;
}



std::mutex mut;
// shared variable
int x {0};
void write() {
    std::lock_guard<std::mutex> lck_guard(mut);
    // start of the critical section
    ++x;
    // End of the critical section
}

void read() {
    std::lock_guard<std::mutex> lck_guard(mut);
    // start of the critical section
    using namespace std::literals;
    std::cout << x << std::endl;
    // End of the critical section
}

std::shared_mutex shmut;

// shared variable
int y {0};
void write1() {
    std::lock_guard<std::shared_mutex> lck_guard(shmut);
    // start of critical section
    ++y;
    // end of critical section
}

void read2() {
    std::shared_lock<std::shared_mutex> lck_guard(shmut);
    using namespace std::literals;
    std::this_thread::sleep_for(100ms);
    // End of critical section
}

class Singleton{
public:
    // delete copy constructor
    Singleton(const Singleton &source) = delete;
    // delete copy assignment
    Singleton &operator=(const Singleton &source) = delete;
    // delete move constructor
    Singleton(Singleton &&source) = delete;
    // delete move assignment
    Singleton &operator=(Singleton &&source) = delete;
    // constructor
    Singleton() {std::cout << "initializing Singleton" << std::endl;};
};


// this method return the only instance ever of our singleton class
//
Singleton &get_singleton() {
    static Singleton single;
    return single;
}

// in our entry points, we want to create references/aliases to the only instance of our Singleton class
// we deleted the copy/move
// the first thread calls the function that initializes
// subsequent threads don't have to do this anymore

std::mutex single_mut;
void task() {
    Singleton &single = get_singleton();
    std::lock_guard<std::mutex> ls(single_mut);
    std::cout << &single << std::endl;
}

int main() {
//    std::cout << "Hello, World!" << std::endl;
//
//    char  str[] = "messi";
//    reverse_string(str);
//    for (int i{0}; i < std::strlen(str); ++i) {
//        std::cout << str[i];
//    }
//    std::cout << std::endl;

    /*
     * instead of creating lots of named objects, you can just create a vector of threads.
     * And then, when you start each thread, you push that back onto the vector.
     * And this will transfer ownership of the execution thread from the thread object to the vector element,
     * because this uses move semantics*/

//    std::vector<std::thread> threads;
//    for (int i{0}; i < 20; ++i) {
//        threads.push_back(std::thread(read));
//    }
//    threads.push_back(std::thread(write));
//    threads.push_back(std::thread(write));
//    for (int i{0}; i < 20; ++i) {
//        threads.push_back(std::thread(read));
//    }
//
//    for (auto &thread: threads) {
//        thread.join();
//    }
        std::vector<std::thread> threads;
        for (int i{0}; i < 10; ++i) {
            threads.push_back(std::thread(task));
        }
        for (auto &thread : threads) {
            thread.join();
        }


    return 0;
}
