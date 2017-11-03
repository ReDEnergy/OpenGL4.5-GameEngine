#pragma once
#include <mutex>
#include <condition_variable>

#define MUTEX_RAII_LOCK(M) std::lock_guard<std::mutex> lock(M)
#define MUTEX_UNIQUE_LOCK std::unique_lock<std::mutex>

#define CONDITIONAL_WAIT(cv, _mutex, func) \
	std::unique_lock<std::mutex> lk(_mutex); \
	cv.wait(lk, func);

