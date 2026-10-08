# Multithreading

Namespace `XYO::Platform::Multithreading`. Five building blocks:

| Name | Header | What it is |
|------|--------|------------|
| `TAtomic<T>` | `Multithreading/TAtomic.hpp` | atomic value, acquire/release by default (header only) |
| `CriticalSection` | `Multithreading/CriticalSection.hpp` | mutex |
| `ConditionVariable` | `Multithreading/ConditionVariable.hpp` | sleep until another thread notifies |
| `Thread` | `Multithreading/Thread.hpp` | OS thread running a `void (*)(void *)` |
| `Processor::getCount()` | `Multithreading/Processor.hpp` | number of logical processors |

They are intentionally small. Higher level tools (thread pools, workers,
queues, RAII lock guards) belong to `xyo-multithreading` and other libraries
built on top of these.

---

## TAtomic&lt;T&gt;

A thin wrapper over `std::atomic<T>` with **safe default memory orders** and
camelCase names. In single thread builds it is a plain `T`.

```cpp
TAtomic<int> counter;              // value-initialized: 0
TAtomic<bool> isReady(false);
TAtomic<Node *> head(nullptr);
```

| Operation | Default order | Returns |
|-----------|---------------|---------|
| `get()` / `operator T()` | `acquire` | current value |
| `set(v)` / `operator=(v)` | `release` | — |
| `exchange(v)` | `acq_rel` | previous value |
| `compareExchange(expected, desired)` | `acq_rel` | `true` if swapped, else loads the current value into `expected` |
| `compareExchangeWeak(expected, desired)` | `acq_rel` | same, may fail spuriously — use in retry loops |
| `fetchAdd(n)` / `fetchSub(n)` | `acq_rel` | previous value (integral and pointer types) |

Every operation takes an optional last argument `std::memory_order`.
Constants: `isAtomic` (false in single thread mode) and `isAlwaysLockFree`.

### Why acquire/release by default

- `set()` **publishes** everything the thread wrote before it;
- `get()` that reads that value **sees** everything written before the `set()`.

This is what "flag + data" hand-off needs, it is free on x86/x64 (plain
loads/stores) and correct on ARM (where relaxed would break it).

```cpp
struct Message {
	int payload;               // plain data
	TAtomic<bool> isReady;     // guard
};

// producer
message.payload = 42;
message.isReady.set(true);     // release: payload is visible before the flag

// consumer
if (message.isReady.get()) {   // acquire
	use(message.payload);      // guaranteed to read 42
};
```

### When to pass an order explicitly

Use `std::memory_order_relaxed` **only** for values that publish nothing,
such as statistics counters:

```cpp
requests.fetchAdd(1, std::memory_order_relaxed);
size_t total = requests.get(std::memory_order_relaxed);
```

Valid orders: `set` — `relaxed`, `release`, `seq_cst`; `get` — `relaxed`,
`acquire`, `seq_cst`. Passing `acquire` to `set` or `release` to `get` is
undefined behaviour (the same rule as `std::atomic`).

### Pitfalls

- `value = value + 1` is **two** atomic operations (a `get` then a `set`), not
  an increment. Use `fetchAdd`.
- A compare-and-swap loop reloads `expected` on failure, do not reload it
  yourself:

  ```cpp
  int current = maximum.get();
  while (current < candidate &&
         !maximum.compareExchangeWeak(current, candidate)) {
  };
  ```

- `T` must be trivially copyable (same requirement as `std::atomic<T>`).
  For larger structs check `isAlwaysLockFree`; if it is false the standard
  library uses an internal lock.
- The constructors are `constexpr`, so a `static TAtomic<...>` is constant
  initialized — safe to use from other static constructors.

---

## CriticalSection

A mutex. Windows: `CRITICAL_SECTION`, Linux: `pthread_mutex_t`.

```cpp
class Registry {
		CriticalSection criticalSection;
		std::map<int, std::string> items;

	public:
		void add(int key, const std::string &value) {
			criticalSection.enter();
			items[key] = value;
			criticalSection.leave();
		};
};
```

Rules:

- **Always pair `enter()` with `leave()` on every path**, including
  exceptions. The library does not provide a guard class; if the protected code
  can throw, use a small local guard:

  ```cpp
  struct Lock {
  	CriticalSection &section;
  	Lock(CriticalSection &section_) : section(section_) { section.enter(); };
  	~Lock() { section.leave(); };
  };
  ```

- **Do not enter it twice from the same thread.** Windows critical sections
  are recursive, Linux default mutexes are not (the second `enter()`
  deadlocks). Code that works on Windows can hang on Linux.
- Not copyable or movable (`XYO_PLATFORM_DISALLOW_COPY_ASSIGN_MOVE`). Keep it
  as a member next to the data it protects.
- On Linux `enter()` / `leave()` / the constructor throw `std::runtime_error`
  if the pthread call fails.
- In single thread builds `enter()` and `leave()` are empty inline functions;
  `CriticalSection::isCriticalSection` tells which one you got.

---

## ConditionVariable

Lets a thread **sleep without using CPU** until another thread changes some
state and notifies it. Always used with a `CriticalSection`. Windows:
`CONDITION_VARIABLE`, Linux: `pthread_cond_t` on `CLOCK_MONOTONIC` (timeouts
are not affected by changes of the system clock). Multi thread builds only.

The condition variable has **no memory**: a notify when no thread waits is
lost. So the *state* lives in ordinary variables protected by the critical
section, and the waiter always checks it in a loop:

```cpp
CriticalSection criticalSection;
ConditionVariable conditionVariable;
bool isReady = false;                       // protected by criticalSection

// waiting thread
criticalSection.enter();
while (!isReady) {                          // loop: wake ups can be spurious
	conditionVariable.wait(criticalSection);
};
// ... isReady is true and we hold the critical section
criticalSection.leave();

// notifying thread
criticalSection.enter();
isReady = true;                             // change state first
conditionVariable.notifyOne();              // then notify, still inside
criticalSection.leave();
```

| Method | Behaviour |
|--------|-----------|
| `wait(cs)` | `cs` must be entered **exactly once** by the caller. Leaves it, sleeps, enters it again before returning. |
| `waitFor(cs, ms)` | Same, with a timeout. Returns `false` on timeout. Negative `ms` is treated as 0. |
| `notifyOne()` | Wakes one waiting thread (if any). |
| `notifyAll()` | Wakes all waiting threads. |

Errors from the OS are reported as `std::runtime_error`.

### Waiting with a deadline

`waitFor` returning `false` means "the time passed", not "the condition is
false" — check the state as well:

```cpp
criticalSection.enter();
bool timedOut = false;
while (!isReady) {
	if (!conditionVariable.waitFor(criticalSection, 1000)) {
		timedOut = !isReady;
		break;
	};
};
criticalSection.leave();
```

Each call waits up to the full `ms` again, so with spurious wake ups the total
time can exceed it. For an exact deadline compute the remaining time with
`std::chrono::steady_clock` before each call.

### Pitfalls

- Waiting without checking state in a loop → lost or spurious wake ups.
- Changing the state **outside** the critical section → the waiter can check
  the state, miss the change and sleep forever.
- Several conditions on the same variable (e.g. "not empty" and "not full") →
  use `notifyAll()`, or one `ConditionVariable` per condition.

---

## Thread

Runs a plain function on a new OS thread. Windows: `CreateThread`, Linux:
`pthread_create`. Multi thread builds only.

```cpp
typedef void (*ThreadProcedure)(void *);
typedef void (*ThreadProcess)(void);

bool start(ThreadProcedure procedure, void *this_,
           ThreadProcess before = nullptr, ThreadProcess after = nullptr);
void join();
bool isRunning();
static void sleep(int milliSeconds);
```

The procedure is a function pointer, not `std::function`: pass state through
`this_`. A lambda **without captures** converts to it:

```cpp
struct Job {
	int input;
	int output;
};

Job job{21, 0};
Thread thread;
if (!thread.start([](void *this__) {
	    Job *this_ = reinterpret_cast<Job *>(this__);
	    this_->output = this_->input * 2;
    }, &job)) {
	// could not create the thread
};
thread.join();
```

Behaviour to know:

- **`start()` joins first.** Calling it on a thread that is still running
  blocks until the previous run ends, then starts the new one. A `Thread`
  object can be reused.
- **The destructor joins.** A `Thread` going out of scope waits for its
  procedure. Make sure the procedure ends (see the stop flag below) or the
  destructor blocks forever.
- **Exceptions are swallowed.** An exception escaping the procedure is caught
  and discarded (so it cannot terminate the process). Catch and report inside
  your procedure if you care.
- **`before` / `after`** run on the new thread, before and after the procedure
  (`after` also runs when the procedure threw). Use them for per-thread setup
  and cleanup, e.g. initializing thread-local allocators.
- **`isRunning()`** is `true` from `start()` until the procedure and `after`
  have finished. It is useful for polling, but use `join()` to wait.
- **`join()`** can be called any number of times; it does nothing if no thread
  is running.
- **No detach, no kill.** Stop threads cooperatively.
- `Thread::sleep(ms)` sleeps the calling thread; negative values return
  immediately. In single thread builds it exists as the free function
  `XYO::Platform::Multithreading::Thread::sleep`.
- `this_` must stay alive until the thread ends.

### Stopping a thread cooperatively

```cpp
struct Worker {
	TAtomic<bool> stop;
	Thread thread;

	static void procedure(void *this__) {
		Worker *this_ = reinterpret_cast<Worker *>(this__);
		while (!this_->stop.get()) {
			// ... do a slice of work ...
			Thread::sleep(10);
		};
	};

	bool start() {
		stop.set(false);
		return thread.start(procedure, this);
	};

	void finish() {
		stop.set(true);
		thread.join();
	};

	~Worker() {
		finish();
	};
};
```

When the thread waits on a `ConditionVariable` instead of sleeping, set the
stop flag **inside** the critical section and `notifyAll()`, like any other
state change.

---

## Processor

```cpp
int32_t n = Processor::getCount();   // >= 1
```

Number of logical processors available to the process (Windows: all processor
groups, Linux: online CPUs). Always at least 1. Typical use: size a thread pool.

```cpp
int32_t count = Processor::getCount();
std::vector<std::unique_ptr<Thread>> threads;
for (int32_t k = 0; k < count; ++k) {
	threads.push_back(std::make_unique<Thread>());
	threads.back()->start(procedure, &sharedState);
};
```

(`Thread` is not movable, hence the `unique_ptr`, or use a fixed array.)

---

## Complete example: producer / consumer queue

```cpp
#include <XYO/Platform.hpp>
#include <deque>

using namespace XYO::Platform::Multithreading;

struct Queue {
	CriticalSection criticalSection;
	ConditionVariable conditionVariable;
	std::deque<int> items;          // protected by criticalSection
	bool isClosed = false;          // protected by criticalSection

	void push(int value) {
		criticalSection.enter();
		items.push_back(value);
		conditionVariable.notifyOne();
		criticalSection.leave();
	};

	void close() {
		criticalSection.enter();
		isClosed = true;
		conditionVariable.notifyAll();
		criticalSection.leave();
	};

	// false when closed and empty
	bool pop(int &value) {
		criticalSection.enter();
		while (items.empty() && !isClosed) {
			conditionVariable.wait(criticalSection);
		};
		bool retV = !items.empty();
		if (retV) {
			value = items.front();
			items.pop_front();
		};
		criticalSection.leave();
		return retV;
	};
};

struct Consumer {
	Queue *queue;
	TAtomic<int> sum;
};

void consume(void *this__) {
	Consumer *this_ = reinterpret_cast<Consumer *>(this__);
	int value;
	while (this_->queue->pop(value)) {
		this_->sum.fetchAdd(value);
	};
};

int main(int, char *[]) {
	Queue queue;
	Consumer consumer;
	consumer.queue = &queue;

	Thread threads[4];
	for (auto &thread : threads) {
		thread.start(consume, &consumer);
	};
	for (int k = 1; k <= 100; ++k) {
		queue.push(k);
	};
	queue.close();
	for (auto &thread : threads) {
		thread.join();
	};

	printf("sum = %d\n", consumer.sum.get()); // 5050
	return 0;
};
```

`std::deque::push_back` can throw `std::bad_alloc` while the critical section
is entered; in production code use a guard as shown in
[CriticalSection](#criticalsection).
