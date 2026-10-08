// Created by Grigore Stefan <g_stefan@yahoo.com>
// Public domain (Unlicense) <http://unlicense.org>
// SPDX-FileCopyrightText: 2016-2026 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: Unlicense

#include <XYO/Platform.hpp>
#include <chrono>
#include <cstdlib>

using namespace XYO::Platform::Multithreading;

// ConditionVariable
//
// - wait() blocks until notifyOne()
// - notifyAll() wakes all waiting threads
// - waitFor() returns false on timeout, true when notified before
// - notify when no thread waits is lost (no state)
// - ping-pong between two threads, wake up time

int errorCount = 0;

void check(bool condition, const char *message) {
	printf("%s %s\r\n", condition ? "[ OK ]" : "[FAIL]", message);
	if (!condition) {
		++errorCount;
	};
};

typedef std::chrono::steady_clock Clock;

int elapsedMilliSeconds(Clock::time_point begin) {
	return (int)std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - begin).count();
};

// A lost wake up makes the test hang,
// the watchdog fails the test instead

struct Watchdog {
		TAtomic<bool> done;
};

void watchdogProcedure(void *this__) {
	const int timeLimit = 60000;
	Watchdog *this_ = reinterpret_cast<Watchdog *>(this__);
	int k;

	for (k = 0; k < timeLimit; k += 10) {
		if (this_->done.get()) {
			return;
		};
		Thread::sleep(10);
	};

	printf("[FAIL] test did not end in %d ms, a wait was not woken up\r\n", timeLimit);
	printf("* Error: test failed\n");
	fflush(stdout);
	std::_Exit(1);
};

// ---

struct Signal {
		CriticalSection criticalSection;
		ConditionVariable conditionVariable;
		bool isReady;
		int waiting;
		TAtomic<int> wokenUp;

		Signal() {
			isReady = false;
			waiting = 0;
			wokenUp.set(0);
		};

		int getWaiting() {
			int retV;
			criticalSection.enter();
			retV = waiting;
			criticalSection.leave();
			return retV;
		};

		void waitAllWaiting(int count) {
			while (getWaiting() < count) {
				Thread::sleep(1);
			};
		};
};

void waitSignalProcedure(void *this__) {
	Signal *this_ = reinterpret_cast<Signal *>(this__);
	this_->criticalSection.enter();
	++this_->waiting;
	while (!this_->isReady) {
		this_->conditionVariable.wait(this_->criticalSection);
	};
	this_->criticalSection.leave();
	this_->wokenUp.fetchAdd(1);
};

void testNotifyOne() {
	Signal signal;
	Thread thread;

	if (!thread.start(waitSignalProcedure, &signal)) {
		check(false, "notifyOne: start thread");
		return;
	};
	signal.waitAllWaiting(1);
	Thread::sleep(100);
	check(signal.wokenUp.get() == 0, "wait() blocks until notified");

	signal.criticalSection.enter();
	signal.isReady = true;
	signal.conditionVariable.notifyOne();
	signal.criticalSection.leave();
	thread.join();
	check(signal.wokenUp.get() == 1, "notifyOne() wakes the waiting thread");
};

void testNotifyAll() {
	const int threadCount = 4;
	Signal signal;
	Thread threads[threadCount];
	int k;

	for (k = 0; k < threadCount; ++k) {
		threads[k].start(waitSignalProcedure, &signal);
	};
	signal.waitAllWaiting(threadCount);
	Thread::sleep(50);
	check(signal.wokenUp.get() == 0, "notifyAll: all threads wait");

	signal.criticalSection.enter();
	signal.isReady = true;
	signal.conditionVariable.notifyAll();
	signal.criticalSection.leave();
	for (k = 0; k < threadCount; ++k) {
		threads[k].join();
	};
	check(signal.wokenUp.get() == threadCount, "notifyAll() wakes all waiting threads");
};

// ---

void testWaitForTimeout() {
	CriticalSection criticalSection;
	ConditionVariable conditionVariable;
	Clock::time_point begin;
	char message[256];
	bool isNotified;
	int time;

	criticalSection.enter();
	begin = Clock::now();
	isNotified = conditionVariable.waitFor(criticalSection, 100);
	time = elapsedMilliSeconds(begin);
	criticalSection.leave();

	sprintf(message, "waitFor(100) timeout: returned %s after %d ms", isNotified ? "true" : "false", time);
	check((!isNotified) && (time >= 80) && (time < 2000), message);
};

struct TimedSignal {
		Signal signal;
		bool isNotified;
		int time;
};

void waitForSignalProcedure(void *this__) {
	TimedSignal *this_ = reinterpret_cast<TimedSignal *>(this__);
	Clock::time_point begin = Clock::now();
	this_->signal.criticalSection.enter();
	++this_->signal.waiting;
	this_->isNotified = true;
	while (!this_->signal.isReady) {
		if (!this_->signal.conditionVariable.waitFor(this_->signal.criticalSection, 10000)) {
			this_->isNotified = false;
			break;
		};
	};
	this_->signal.criticalSection.leave();
	this_->time = elapsedMilliSeconds(begin);
};

void testWaitForNotified() {
	TimedSignal timedSignal;
	Thread thread;
	char message[256];

	timedSignal.isNotified = false;
	timedSignal.time = 0;
	if (!thread.start(waitForSignalProcedure, &timedSignal)) {
		check(false, "waitFor notified: start thread");
		return;
	};
	timedSignal.signal.waitAllWaiting(1);
	Thread::sleep(50);
	timedSignal.signal.criticalSection.enter();
	timedSignal.signal.isReady = true;
	timedSignal.signal.conditionVariable.notifyOne();
	timedSignal.signal.criticalSection.leave();
	thread.join();

	sprintf(message, "waitFor(10000) notified after 50 ms: returned %s after %d ms", timedSignal.isNotified ? "true" : "false", timedSignal.time);
	check(timedSignal.isNotified && (timedSignal.time < 5000), message);
};

void testNotifyWithoutWaiting() {
	CriticalSection criticalSection;
	ConditionVariable conditionVariable;
	bool isNotified;

	conditionVariable.notifyOne();
	conditionVariable.notifyAll();
	criticalSection.enter();
	isNotified = conditionVariable.waitFor(criticalSection, 50);
	criticalSection.leave();
	check(!isNotified, "notify when no thread waits is lost, waitFor(50) times out");
};

// ---

struct PingPong {
		CriticalSection criticalSection;
		ConditionVariable conditionVariable;
		int turn;
		int rounds;
};

void pongProcedure(void *this__) {
	PingPong *this_ = reinterpret_cast<PingPong *>(this__);
	int k;
	this_->criticalSection.enter();
	for (k = 0; k < this_->rounds; ++k) {
		while (this_->turn != 1) {
			this_->conditionVariable.wait(this_->criticalSection);
		};
		this_->turn = 0;
		this_->conditionVariable.notifyAll();
	};
	this_->criticalSection.leave();
};

void testPingPong() {
	PingPong pingPong;
	Thread thread;
	Clock::time_point begin;
	char message[256];
	int time;
	int k;

	pingPong.turn = 0;
	pingPong.rounds = 1000;
	if (!thread.start(pongProcedure, &pingPong)) {
		check(false, "ping-pong: start thread");
		return;
	};

	begin = Clock::now();
	pingPong.criticalSection.enter();
	for (k = 0; k < pingPong.rounds; ++k) {
		pingPong.turn = 1;
		pingPong.conditionVariable.notifyAll();
		while (pingPong.turn != 0) {
			pingPong.conditionVariable.wait(pingPong.criticalSection);
		};
	};
	pingPong.criticalSection.leave();
	time = elapsedMilliSeconds(begin);
	thread.join();

	// polling with sleep(1) needs at least 1 ms per wait, 2 waits per round
	sprintf(message, "ping-pong between two threads, %d rounds in %d ms", pingPong.rounds, time);
	check(time < pingPong.rounds, message);
};

// ---

void test() {
	Watchdog watchdog;
	Thread watchdogThread;

	// keep output if process crashes
	setvbuf(stdout, nullptr, _IONBF, 0);

	watchdog.done.set(false);
	watchdogThread.start(watchdogProcedure, &watchdog);

	printf("- ConditionVariable\r\n");
	testNotifyOne();
	testNotifyAll();
	testWaitForTimeout();
	testWaitForNotified();
	testNotifyWithoutWaiting();
	testPingPong();

	watchdog.done.set(true);
	watchdogThread.join();

	if (errorCount) {
		throw std::runtime_error("test failed");
	};

	printf("Done.\r\n");
};

int main(int, char *[]) {
	try {

		test();

		return 0;

	} catch (const std::exception &e) {
		printf("* Error: %s\n", e.what());
	} catch (...) {
		printf("* Error: Unknown\n");
	};

	return 1;
};
