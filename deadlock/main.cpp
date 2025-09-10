#include <iostream>
#include <vector>
#include <mutex>
#include <string>
#include <chrono>
#include <thread>
using namespace std::literals;



/*
 * Deadlock
 * - A thread is deadlocked when it cannot run
 * - Often used to refer to "mutual deadlock"
 *      - Two or more threads are waiting for each other
 *      - Thread A waits for thread B to do something
 *      - Thread B is waiting for A to do something
 *      - Threads A nad B are waiting for an event that can never happen
 *
 * - The classic example involves waiting for mutexes
 * */

/*
 * Mutual Deadlock
 * - Can also occur when waiting for
 *      - The result of a computation performed by another thread
 *      - A message sent by another
 *      - A GUI event produced by another thread
 * - The second most common problem in multi-threading code
 * - Often caused by threads trying to lock mutexes in different orders
 * */

/*
 * Deadlock Avoidance
 * - A simple way to avoid deadlock
 * - Both threads try to acquire the locks in the same order
 *      - Thread A and thread B both try to lock mutex1 first
 *      - The successful thread then tries to lock mutex2
 *      */

/*
 * Deadlock Avoidance
 * - This is not ideal
 *      - Relies on the programmer
 *      - May not be feasible in large programs
 * - We will look at some better approaches in the next lecture
 * */

/*
 * Dining Philosophers Rules
 * - A philosopher has two states: thinking and eating
 * - Each fork can only be held by one philosopher at a time
 *      - A philosopher can only pick up one fork at a time
 *      - A philosopher must pick up both forks before they can eat
 *      - When a philosopher finishes eating, they put down both forks immediately
 *      - A philosopher may pick up a fork as soon as it is put down by another
 * - A philosopher has no awareness of other philosophers actions
 * - If a philosopher does not eat at all, they will die of starvation*/

/*
 * - Intended scenario
 *      - Philosopher thinks
 *      - Philosopher picks up left fork
 *      - Philosopher thinks
 *      - Philosopher picks up right fork
 *      - Philosopher eats
 *      - Philosopher puts down both forks
 *      - Philosopher thinks
 *      */

/*
 * Implementation
 * - A separate thread for each philosopher
 * - Each fork has an associated mutex
 *      // A mutex prevents more than one philosopher picking up the same fork
 *      std::mutex fork_mutex{nforks};
 *
 *      // A philosopher thread can only pick up a fork if it can lock the corresponding mutex
 *      // Try to pick up the left fork
 *      fork_mutex[rfork].lock();
 *
 *      // Succeeded- this philosopher can now eat
 *      // .....
 *      */

/*
 * Deadlock
 * - All the philosophers pick up their left fork
 * - None of the right forks are available
 *      - B picks up fork 2
 *      - Fork 2 is A's right fork
 *      - A cannot eat without picking up fork 2
 *      - Fork 2 will not become available until B has finished eting
 *      - B cannot start eating because C has taken fork 3
 * - The philosopher threads are deadlocked
 *      - The philosophers cannot enter the "eating" state
 *      */

/*
 * Deadlock Avoidance
 * - Make the threads acquire locks in the same order
 *      - Relies on the programmer
 * - Lock multiple mutexes in a single operation
 *      - Thread A locks mut1 and mut2
 *      - Thread B cannot lock mut2 or mut1 during this operation
 *      - A much better solution
 * - C++ provides library features for this
 * */

/*
 * - C++17 has the std::scoped_lock
 * - very similar to the std::lock_guard
 *      - Except it can lock more than one mutex at the same time
 *          std::scoped_lock scope_lck(mut1, mut2,...);
 * - The mutexes are locked in the order given in the constructor call
 *      - In the destructor, the mutexes are unlocked in the reverse order
 * - This avoids the possibility of deadlock with multiple mutexes
 * */

/*
 * std::scoped_lock Caveat
 * - scoped_lock can be used with a single mutex
 *      std::scoped_lock scoped_lck(mut);
 *
 * - It is easy to accidentally omit the argument
 *      std::scoped_lock scoped_lck;
 * - This will compile and run, but not actually perform any locking
 *      - May cause an unexpected data race
 *      */

/*
 * Deadlock Avoidance Before C++17
 *      - it can lock multiple mutexes in a single operation
 *          // Lock two mutexes
 *          std::lock(mut1, mut2)
 *          */

/*
 * Adopting Locks
 * - std::unique_lock can "adopt" the locks
 *      - Pass the std::adopt_lock option to its constructor
 *      - The std::unique_lock object now owns the lock
 *              // Lock two mutexes
 *              std::lock(mut1, mut2);
 *
 *              //Each lock is adopted by a unique_lock object
 *              std::unique_lock<std::mutex> uniq_lk1(mut1, std::adopt_lock);
 *              std::unique_lock<std::mutex> uniq_lk2(mut1, std::adopt_lock);
 *              */

/*
 * Deferring Locks
 * - Alternatively we can "defer" the locking
 *      - Pass the std::defer_lock option to the constructor
 *      - Then lock the mutexes later
 *          // Each unique_lock object is associated with a mutex
 *          std::unique_lock<std::mutex> uniq_lk1(mut1, std::defer_lock);
 *          std::unique_lock<std::mutex> uniq_lk2(mut2, std::defer_lock);
 *
 *          // Lock the mutexes
 *          std::lock(lk1, lk2);
 *          */

/*
 * std::try_lock()
 * - Also locks multiple mutexes in a single operation
 *          // Lock the mutexes
 *          std::try_lock(uniq_lk1, unik_lk2);
 * - Returns immediately if it cannot obtain all the locks
 *      - On failure, it returns the index of the object that failed to lock(0 for the first argument)
 *      - On success, it returns -1*/

/*
 * - Sometimes this approach is not suitable
 *      - it is not feasible to acquire multiple lock simultaneously
 * - A common technique is to impose an ordering
 * - A thread cannot lock a mutex unless it has already locked a mutex with a lower status
 *      - ID number
 *      - Alphabetical name
 * - The Williams book has a hierarchical_mutex that implements this
 * */


/* Deadlock Avoidance Guidelines
 * - Avoid waiting for a thread while holding a lock
 *      - The other thread may need the lock to proceed
 * - Try to avoid waiting for other threads
 *      - The other thread may be waiting for your thread
 * - Try to avoid nested locks
 *      - if your thread already holds a lock, do not acquire another one
 *      - if you need multiple locks, acquire them in a single operation
 * - Avoid calling functions within a critical section
 *      - Unless you are certain the function does not try to lock
 *      */


/*
 * Deadlock Avoidance
 * - Use try_lock() with a timeout
 *      - Instead of blocking lock()
 *      - May result in livelock
 *      */

/*
 * Deadlock Avoidance
 * - use std::lock()
 *      - Lock both mutexes in a single operation*/

/*
 * Livelock
 * - A program cannot make progress
 *      - In the deadlock, the threads are inactive
 *      - In livelock, the threads are still active
 *
 * - Livelock can result from badly done deadlock avoidance
 *      - A thread cannot get a lock
 *      - Instead of blocking indefinitely, it backs off and tries again
 *      */

/*
 * Livelock Example
 *
 *
 *         void funcA()
 *         {
 *              bool locked = false;
 *
 *              while(!locked) {
 *                  std::lock_guard(mut1); // lock mut1
 *                  std::this_thread::sleep_for(1s);
 *                  locked = mut2.try_lock(); // try to lock mut2
 *              }
 *          }
 *          void funcB()
 *          {
 *              // Same as FuncA, but with mut1 and mut2 interchanged
 *          }
 *
 *
 *          */

/*
 * Livelock Analogy
 * - Imagine two very polite people
 * - They walk down a corridor together
 * - They reach a narrow door
 *      - They each try to go through the door at the same time
 *      - Each one stops and waits for the other to go through the door
 *      - Then they both try to go through the door at the same time
 *      - Then each one stops and waits for the other to go through the door, etc
 *      */

/*
 * Livelock Avoidance
 * - Use std::scoped_lock or std::lock()
 *      - The thread can acquire multiple locks in a single operation
 *      - Built in deadlock avoidance
 *
 *
 *          void funcA()
 *          {
 *              std::scoped_lock scoped_lck(mut1, mut2); // lock both mutexes
 *              // .....
 *          }
 *
 *          void funcB()
 *          {
 *              std::scoped_lock scoped_lck (mut2, mut1); // lock both mutexes
 *              // .....
 *          }
 *          */


/*
 * Thread Priority
 * - We could assign different priorities to threads
 * - Not directly supported by C++
 * - Most thread implementations allow it
 *      - Accessible via std::threads's native_handle()
 *      - A high priority thread will run more often
 *      - A low priority thread will be suspended or be interrupted more often
 *
 * - The high priority thread will lock the mutex first
 * - the low priority thread will lock the mutex after-wards
 * */

/*
 * Resource Starvation
 * - A thread cannot get the resources it needs to run
 *      - In deadlock and livelock, the thread cannot acquire a lock it
 *
 * - Lack of system resources can prevent a thread starting
 *      - System memory exhausted
 *      - Maximum supported number of threads is already running
 *
 * - Low priority threads may get starved of processor time
 *      - Higher priority threads are given preference by the scheduler
 *      - Good schedulers try to avid this
 *      */

/*
 * Livelock Practical
 *
 *
 * Deadlock
 * - The previous attempt resulted in deadlock
 *      - All the philosphers picked up their left fork
 *      - None of the right forks were available
 *      - The philosophers were stuck in the "thinking" state
 *      */

/*
 * - We try to avoid the deadlock
 * - Add a time-out and retry:
 *      - Philosopher picks up left fork
 *      - Philosopher tries to pick up right fork
 *      - philosopher cannot pick up right fork
 *      - philosopher puts down the left fork
 *      - philosopher waits
 *      - philosopher picks up left fork again
 *      */

/*
 * - This creates a situation of livelock:
 *      - All the philosopher pick up their left forks at the same time
 *      - All the philosopher try to pick up their right fork
 *      - All the philosophers put down their left forks at the same time
 *      - All the philosophers pick up their left forks at the same time
 *
 * - The philosopher threads are livelocked
 *      - The philosophers are active, but cannot enter the "eating" state
 *      */

/*
 * Solutions
 * - Add randomness
 *      - The philosophers pick up and put down their forks at different times
 *      - Reduces the probability of starvation
 *      - Does not completely eliminate it
 *
 * - Provide a central arbitrator to coordinate the philosophers
 *      - Only allow one philosopher to pick up a fork at a time
 *      - Only one philosopher can eat at a time
 *      - reduces parallelism
 *
 * - Use a shared lock
 *      - In effect, a philosopher picks up both forks at the same time
 *      */

/*
 * - Introduce a fork hierarchy
 *      - The philosopher must pick up the lower-numbered fork first
 *      - A picks up fork 0
 *      - B picks up fork 1
 *      - C picks up fork 2
 *      - D picks up fork 3
 *      - E tries to pick up fork 0 (instead of 4)
 *
 * - This leaves fork 4 available
 *      - D picks up fork 3
 *      - D starts eating
 *      */

/*
 * Mutex Conclusion
 * - We have covered
 *          - Basic mutex class (std::mutex)
 *          - Mutex wrappers (std::lock_guard and std::unique_lock)
 *          - Mutexes and time-outs (std::timed_mutex, std::unique_lock)
 *          - Shared mutexes (std::shared_mutex and std::shared_lock)
 *          - Static and thread-local data
 *          - Deadlock
 *          - Deadlock avoidance (std::scoped_lock, std::lock())
 *          - Livelock and livelock avoidance
 *          */

/*
 * Locking Guidelines
 * - Locking impacts on other threads
 *      - They will have to wait longer for a resource they need
 *      - This affects performance
 *
 * - Always hold a lock for the shortest possible time
 * - Avoid locking lengthy operations if possible]
 *      - e.g. input/output
 *      */

/*
 * Recommendations for Reading Shared Data
 * - Reading
 *      - Lock
 *      - Make a copy of the shared data
 *      - Unlock and process the copy
 * */

/*
 * Recommendations for Writing Shared Data
 * - Writing
 *      - Lock
 *      - Make a copy of the shared data
 *      - Unlock and process the copy
 *      - Lock again
 *      - Update the shared data from the copy
 *      - Unlock
 *      */

/*
 * Locking Guidelines for Data Structures
 * - Do not lock any more elements than necessary
 *      - e.g. Accessing a single element in a linked list
 *      - Do not lock the entire list
 *      - This will block other threads from accessing unrelated elements
 *
 * - Do not make locking too fine-grained
 *      - Do not lock individual elements when inserting and deleting
 *      - Another thread may need to access a neighbouring element
 *      - Data race
 *      - Need to lock neighbouring elements in a single operation
 *      */

/*
 * Pros and Cons of mutexes
 * - Fairly straight forward way to protect shared data
 *      - Prevent data races and race conditions
 * - Locking and unlocking are slow operations
 * - Low level
 *      - the programmer must remember to use a mutex
 *      - The programmer must use the right mutex
 *      - The programmer must understand how different threads can modify the data
 * - Most real world programs use higher level structures
 *      - Mutex wrapper classes
 *      - Classes from the next section
 *      */

std::mutex mut1, mut2;
void funcA() {
    std::cout << "Thread A trying to lock mutexes 1 and 2....." << std::endl;
    std::scoped_lock scoped_lck(mut1, mut2);
    std::cout << "Thread A has locked mutexes 1 and 2" << std::endl;
    std::this_thread::sleep_for(50ms);
    std::cout << "Thread A releasing mutexes 1 and 2...." << std::endl;
}
void funcB() {
    std::cout << "Thread B trying to lock mutexes 2 and 1....." << std::endl;
    std::scoped_lock scoped_lck(mut2, mut1);
    std::cout << "Thread A has locked mutexes 1 and 2" << std::endl;
    std::this_thread::sleep_for(50ms);
    std::cout << "Thread A releasing mutexes 2 and 1...." << std::endl;
}
void funcA1() {
    std::cout << "Thread A trying to lock mutexes 1 and 2......" << std::endl;
    std::lock(mut1, mut2); // lock both mutexes
    std::cout << "Thread A has locked mutexes 1 and 2" << std::endl;

    // Adopt the locks
    std::unique_lock<std::mutex> uniq_lk1(mut1, std::adopt_lock); // Associate each mutex
    std::unique_lock<std::mutex> uniq_lk2(mut2, std::adopt_lock); // with a unique lock
    std::cout << "Thread A has adopted the locks" << std::endl;

    std::this_thread::sleep_for(50ms);
    std::cout << "Thread A releasing mutexes 1 and 2" << std::endl;
}
void funcB1() {
    std::cout << "Thread B trying to lock mutexes 2 and 1......" << std::endl;
    std::lock(mut2, mut1); // lock both mutexes
    std::cout << "Thread B has locked mutexes 1 and 2" << std::endl;

    // Adopt the locks
    std::unique_lock<std::mutex> uniq_lk1(mut2, std::adopt_lock); // Associate each mutex
    std::unique_lock<std::mutex> uniq_lk2(mut1, std::adopt_lock); // with a unique lock
    std::cout << "Thread B has adopted the locks" << std::endl;

    std::this_thread::sleep_for(50ms);
    std::cout << "Thread B releasing mutexes 2 and 1" << std::endl;
}
void funcA2() {
    std::unique_lock<std::mutex> uniq_lck1(mut1, std::defer_lock); // Associate mutex with lock...
    std::unique_lock<std::mutex> uniq_lck2(mut2, std::defer_lock); // ...but don't lock it yet

    std::cout << "Thread A trying to lock mutexes 1 and 2....." << std::endl;
    std::lock(uniq_lck1, uniq_lck2);
    std::cout << "Thread A has locked mutexes 1 and 2" << std::endl;
    std::this_thread::sleep_for(50ms);
    std::cout << "Thread A releasing mutexes 1 nad 2" << std::endl;
}
void funcB2() {
    std::unique_lock<std::mutex> uniq_lck2(mut2, std::defer_lock); // Associate mutex with lock...
    std::unique_lock<std::mutex> uniq_lck1(mut1, std::defer_lock); // ...but don't lock it yet

    std::cout << "Thread B trying to lock mutexes 2 and 1....." << std::endl;
    std::lock(uniq_lck2, uniq_lck1);
    std::cout << "Thread B has locked mutexes 2 and 1" << std::endl;
    std::this_thread::sleep_for(50ms);
    std::cout << "Thread B releasing mutexes 2 nad 1" << std::endl;
}
void funcA3() {
    std::unique_lock<std::mutex> uniq_lk1(mut1, std::defer_lock);
    std::unique_lock<std::mutex> uniq_lk2(mut2, std::defer_lock);

    std::cout << "Thread A trying to lock mutexes 1 and 2......" << std::endl;

    // Now lock both of them
    auto idx = std::try_lock(uniq_lk1, uniq_lk2);
    if (idx != -1) {
        std::cout << "try_lock failed on mutex with index " << idx << std::endl;

    }
    else {
        std::cout << "Thread A has locked mutexes 1 and 2" << std::endl;
        std::this_thread::sleep_for(50ms);
    }
}
void funcB3() {
    std::unique_lock<std::mutex> uniq_lk2(mut2, std::defer_lock);
    std::unique_lock<std::mutex> uniq_lk1(mut1, std::defer_lock);

    std::cout << "Thread B trying to lock mutexes 2 and 1......" << std::endl;

    // Now lock both of them
    auto idx = std::try_lock(uniq_lk1, uniq_lk2);
    if (idx != -1) {
        std::cout << "try_lock failed on mutex with index " << idx << std::endl;

    }
    else {
        std::cout << "Thread B has locked mutexes 2 and 1" << std::endl;
        std::this_thread::sleep_for(50ms);
    }
}
// Livelock example
std::mutex lmut1, lmut2;
void funcZ() {
    std::this_thread::sleep_for(10ms);
    bool locked {false};
    while (!locked) {
        std::lock_guard<std::mutex> lck_guard(lmut1); // lock lmut 1
        std::cout << "After you, Claude!" << std::endl;
        std::this_thread::sleep_for(2s);
        locked = lmut2.try_lock(); // try to lock lmut2
    }
    if (locked) {
        std::cout << "ThreadZ has locked both mutexes" << std::endl;
    }
}

void funcY() {
    bool locked {false};
    while (!locked) {
        std::lock_guard<std::mutex> lck_guard(lmut2); // lock lmut2
        std::cout << "After you Cecil" << std::endl;
        std::this_thread::sleep_for(2s);
        locked = lmut1.try_lock(); // try to lock lmut 1
    }
    if (locked) {
        std::cout << "ThreadY has locked both mutexes" << std::endl;
    }
}
constexpr int nforks {5};
constexpr int nphilosophers {nforks};
std::vector<std::string> names {"A", "B", "C", "D", "E"};

// Keep track of how many times a philosopher is able to eat
std::vector<int> mouthfuls(nphilosophers,0);

// A philosopher who has not picked up both forks is thinking
constexpr auto think_time = 2s;

// A philosopher has picked up both forks is eating
constexpr auto eat_time = 1s;

// A mutex prevents more than one philosopher from picking up the same fork
// A philosopher thread can only pick up a fork if it can lock the corresponding mutex

std::mutex fork_mutex[nforks];

// mutex to protect output
std::mutex print_mutex;

// Functions to display information about the 'nth' philosopher

// Interactions with a fork
void print(int n, const std::string &str, int lforkno, int rforkno) {
    std::lock_guard<std::mutex> print_lock(print_mutex);
    std::cout << "Philosopher " << names[n] << str << lforkno << " and " << rforkno << std::endl;
}
// The philosophers state
void print(int n, const std::string &str) {
    std::lock_guard<std::mutex> print_lock(print_mutex);
    std::cout << "Philosopher " << names[n] << str << std::endl;
}
std::mutex increment_mut;
void increment_value(int phil) {
    std::lock_guard<std::mutex> increment_lock(increment_mut);
    mouthfuls[phil]++;
}


// Thread which represents a dining philosopher
void dine(int nphilo) {
    // Philosopher A has fork 0 on their left
    // and fork 1 on their right
    // Philosopher B has fork 1 on their left
    // and fork 2 on their right
    // ...
    // Philosopher E has fork 4 on their left
    // and fork 0 on their right
    //
    // Each philosopher must pick up their left fork first
    int lfork = nphilo;
    int rfork = (nphilo+1) % nforks;

    print(nphilo, "\'s forks are ", lfork, rfork);
    print(nphilo, " is thinking....");
    std::this_thread::sleep_for(think_time);
    // Make an attempt to eat
    print(nphilo, " reaches for forks ", lfork, rfork);
    // Try to pick up both forks
    std::lock(fork_mutex[lfork], fork_mutex[rfork]);
    std::unique_lock<std::mutex> left_fork_lock(fork_mutex[lfork], std::adopt_lock);
    std::unique_lock<std::mutex> right_fork_lock(fork_mutex[rfork], std::adopt_lock);
    print(nphilo, " picks up fork ", lfork, rfork);
    // succeeded - this philosopher can now eat
    print(nphilo, " is eating...");
    increment_value(nphilo);


    std::this_thread::sleep_for(eat_time);

    print(nphilo, " puts down forks ", lfork, rfork);
    print(nphilo, " is thinking....");

    std::this_thread::sleep_for(think_time);
}


int main() {


//    std::vector<std::thread> philos;
//    for (int i = 0; i < nphilosophers; ++i) {
//        philos.push_back(std::move(std::thread{dine, i}));
//    }
//
//    for (auto &philo : philos) {
//        philo.join();
//    }
//
//    // how many times were the philosophers able to eat?
//    for (int i = 0; i < nphilosophers; ++i) {
//        std::cout << "Philosopher " << names[i];
//        std::cout << " had " << mouthfuls[i] << " mouthfuls" << std::endl;
//    }

    std::thread thrZ (funcZ);
    std::this_thread::sleep_for(10ms);
    std::thread thrY(funcY);

    thrZ.join(), thrY.join();
    return 0;
}
