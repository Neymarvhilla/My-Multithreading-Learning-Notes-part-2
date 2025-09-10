#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <string>

using namespace std::literals;


/*
 * std::lock_guard
 *
 * EXCEPTION THROWN IN CRITICAL SECTION
 *
 *            try {
 *              task_mutex.lock(); // lock the mutex before the critical section
 *              // Critical section throws an exception
 *
 *              task_mutex.unlock(); // Never gets called
 *            }
 *
 *            catch (std::exception& e) {
 *              .....
 *            }
 *
 *            - The mutex will be left locked
 *            */


/*
 * - When the exception is thrown:
 *      - The destructors are called for all objects in scope
 *      - The program flow jumps into the catch handler
 *      - The unlock call is never executed
 *      - The mutex remains locked
 *
 * - All other threads which are waiting to lock the mutex are blocked
 * - if main() is joined on these blocked threads
 *      - main() will be blocked as well
 *      - The entire program is blocked
 *      */

/*
 * Drawbacks of std::mutex
 * - Calling lock()* requires a corresponding call to unlock()
 *      - if not, the mutex will remain locked after the thread exits
 *
 * - unlock() must always be called, even if
 *      - There are multiple paths through the critical section
 *      - An exception is thrown
 *
 * - Relies on the programmer to get it right
 * - For these reasons, we do not normally use std::mutex directly
 * */

/*
 * Mutex Wrapper Classes
 * - The C++ library provides mutex wrapper classes
 *      - Classes with a mutex object as a private member
 *      - Defined in <mutex>
 *
 * - These use the RAII idiom for managing resources
 *      - In this case, the resource is a lock on a mutex
 *      - The constructor locks the mutex
 *      - The destructor unlocks the mutex
 *
 * - We create the wrapper class on the stack
 *      - The mutex will always be unlocked when the object goes out of scope
 *      - including when an exception is thrown
 * -*/

/*
 * std::lock_guard
 * - std::lock_guard is a very basic wrapper
 *      - Has a constructor and destructor only
 *
 * - The constructor takes a mutex object as argument
 *      - Initializes its member from the argument
 *      - lock it
 *
 * - The destructor unlocks the mutex
 * */

/*
 * std::lock_guard
 * - std::lock_guard is a template class
 * - The template parameter is the type of the mutex
 *      // Create a wrapper object for task_mutex
 *      // which has type std::mutex
 *      std::lock_guard<std::mutex> lck_guard(task_mutex);*/

/*
 * USING std::lock_guard() Output Example
 * - Do not explicitly lock the mutex
 * - Create an std::lock_guard object
 * - Pass the mutex to its constructor
 * */

/*
 * - lck_guard is created
 *      - its constructor calls lock()
 *
 * - lck_guard goes out of scope
 *      - its destructor calls unlock()
 *
 * - if an exception is thrown
 *      - lck_guard's destructor is called and unlocks the mutex
 *
 * - The mutex is never left unlocked
 *      - However, the mutex is still locked after the end of the critical section
 *      - Other threads cannot lock the mutex until the lck_guard is destroyed
 *      */

/*
 * std::unique_lock
 *
 * - The same basic features as std::lock_guard
 *      - Mutex data member
 *      - Constructor locks the mutex
 *      - Destructor unlocks it
 *
 * - It also has an unlock() member function
 *      - We can call this after the critical section
 *      - Avoids blocking other threads while we execute non-critical code
 *
 * - If we do not call unlock(), the destructor will unlock the mutex
 *      - The lock is always released
 *      */

/*
 * std::unique_lock Constructor Options
 * - The default
 *      - Call the mutex's lock() member function
 *
 * - Try to get the lock
 *      - Do not wait if unsuccessful
 *      - (Timed mutex) wait with a time-out if unsuccessful
 *
 * - Do not lock the mutex
 *      - it will be locked later
 *      - Or the mutex is already locked
 *      */

/*
 * std::try_to_lock
 *      - Calls the mutex's try_lock() member function
 *      - The owns_lock() member function checks if the mutex is locked
 *
 *
 * - std::defer_lock
 *      - Does not lock the mutex
 *      - Can lock it later by calling the lock() member function
 *      - Or by passing the std::unique_lock object to std::lock()
 *
 * - std::adopt_lock
 *      - Takes a mutex that is already locked
 *      - Avoids locking the mutex twice
 *      */

/*
 * - A std::unique_lock object cannot be copied
 * - it can be moved
 *      - The lock is transferred to another std::unique_lock object
 *      - Can only be done within the same thread
 *
 * - We can write a function that creates a lock object and returns it
 *      - The function could lock different types of mutex, depending on its arguments
 *      - Factory design pattern
 *      */

/*
 * std::unique_lock vs std::lock_guard
 * - std::unique_lock is much more flexible, but
 *      - Requires slightly more storage
 *      - is slightly slower
 * - Recommendations:
 *      - Use lock_guard to lock a mutex for an entire scope
 *      - Use unique_lock if you need to unlock within the scope
 *      - Use unique_lock if you need the extra features*/


/*
 * Timeouts and Mutexes
 *
 * Multiple Locking
 * - A thread locks an std::mutex
 * - It must not call lock() again until it has called unlock()
 *      - Undefined behaviour
 *      - Usually the program blocks indefinitely
 *      */

/*
 * std::recursive_mutex
 * - its lock() member function can be called repeatedly
 *      - Without calling unlock() first
 *      - For each lock() call, there must eventually be an unlock() call
 *
 * - Normally a sign of bad design
 * */

/*
 * std::timed_mutex
 *
 * - Similar to std::mutex, but with extra member functions
 * - try_lock_for()
 *      - keep trying to lock the mutex for a specified duration
 * - try_lock_until()
 *      - Keep trying to lock the mutex until a specified time
 * - These return bool
 *      - true if the mutex was locked
 *      - Otherwise false
 *      */

/*
 * std::unique_lock
 * - std::unique_lock has member functions
 *      - try_lock_for()
 *      - try_lock_until()
 * - These are forwarded to the wrapped mutex
 *      - Will only compile if the mutex supports the operations
 *      */

/*
 * std::chrono clocks
 *
 * - chrono::system_clock
 *      - Gets time from operating system
 *      - May change erratically
 *      - Use it for time points
 *
 * - chrono::steady_clock
 *      - Always increases steadily
 *      - Use it for measuring intervals
 *
 * - try_lock_for() and try_lock_until() may return later than requested
 *      - Due to scheduling issues
 *      */

std::mutex print_mutex;
void task(std::string str) {
    for (int i{0}; i < 5; ++i) {
        // Create an std::unique_lock object
        // This calls print_mutex.lock()

        std::unique_lock<std::mutex> uniq_lck(print_mutex);
        // start of critical section
        std::cout << str[0] << str[1] << str[2] << std::endl;
        // End of critical section

        // Unlock the mutex
        uniq_lck.unlock();
        std::this_thread::sleep_for(50ms);
    }
}

std::timed_mutex the_mutex;

void task1() {
    std::cout << "Task1 trying to lock the mutex" << std::endl;
    the_mutex.lock();
    std::cout << "Task1 locks the mutex" << std::endl;
    // we want the current thread to sleep for 5 seconds
    std::this_thread::sleep_for(5s);
    std::cout << "Task1 unlocking the the mutex" << std::endl;
    the_mutex.unlock();
}

void task2() {
    // we want the current thread to sleep for 500ms
    std::this_thread::sleep_for(500ms);
    std::cout << "Task2 trying to lock the mutex" << std::endl;
    // Try for 1 second to lock the mutex
    while(!the_mutex.try_lock_for(1s)) {
        // returned false
        std::cout << "Task2 could not lock the mutex" << std::endl;
        // Try again on the next iteration
    }
    // Returned true - the mutex is now locked

    // Start of critical section
    std::cout << "Task2 has locked the mutex" << std::endl;
    // End of critical section
}

void Task2() {
    std::this_thread::sleep_for(500ms);
    std::cout << "Task2 trying to lock the mutex" << std::endl;
    auto deadline{std::chrono::system_clock::now() + 900ms};
    // Try to lock the mutex until the deadline
    while(!the_mutex.try_lock_until(deadline)) {
        // Returned false
        // Update "deadline" and try again
        deadline = std::chrono::system_clock::now() + 900ms;
        std::cout << "Task2 could not lock the mutex" << std::endl;
    }

    // Returned true - the mutex is now locked

    // start of critical section
    std::cout << "Task2 has locked the mutex" << std::endl;
    // End of critical section
    the_mutex.unlock();
}

void Task1() {
    std::cout << "Task1 trying to lock the mutex" << std::endl;
    std::lock_guard<std::timed_mutex> lck_guard(the_mutex);
    std::cout << "Task1 locks the mutex" << std::endl;
    std::this_thread::sleep_for(5s);
    std::cout << "Task1 unlocking the mutex" << std::endl;
}

void Task3() {
    std::this_thread::sleep_for(500ms);
    std::cout << "Task2 trying to lock the mutex" << std::endl;
    std::unique_lock<std::timed_mutex> uniq_lck(the_mutex, std::defer_lock);

    // Try for 1 second to lock the mutex
    while(!uniq_lck.try_lock_for(1s)) {
        // Failed to lock
        std::cout << "Task2 could not lock the mutex" << std::endl;
        // Try again next iteration
    }
    // Succeeded in locking the mutex
    //start of critical section
    std::cout << "Task2 has locked the mutex" << std::endl;
    // End of critical section
}
int main() {
//    std::cout << "Hello, World!" << std::endl;
//
//    std::thread thr1(task, "abc");
//    std::thread thr2(task, "def");
//    std::thread thr3(task, "xyz");
//
//    thr1.join(); thr2.join(); thr3.join();

    std::thread v1(task1);
    std::thread v2(Task3);
    v1.join(), v2.join();
    return 0;
}
