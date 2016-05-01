
#pragma once

#include <string>
#include <array>
#include <vector>
#include <thread>
#include <functional>
#include <unordered_map>
#include <chrono>
#include <mutex>

#include "util/murmur3.hpp"
#include "thread/Semaphore.hpp"
#include "util/logging.hpp"

namespace thread {

	class TaskMaster {

		static const int numThreads = 4;

	public:
		class Task {
		private:
			std::function<void()> m_func;
			std::vector<std::string> m_writeAccess;
			std::vector<std::string> m_readAccess;

			friend class TaskMaster;
			Task(); // disabled

		public:
			std::string m_name;

			operator bool() const {
				return m_func != nullptr;
			}

			Task(std::string taskName, std::function<void()> func) {
				m_func = func;
				m_name = taskName;
			}

			void clearFunction() {
				m_func = nullptr;
			}

			Task& requireWrite(const std::string& str) {
				m_writeAccess.push_back(str);
				return *this;
			}

			Task& requireRead(const std::string& str) {
				m_readAccess.push_back(str);
				return *this;
			}

			void run(TaskMaster* taskMaster) {
				LOG("running task: %s", m_name.c_str());
				if (m_func) {
					m_func();
				}
				taskMaster->onTaskFinished(*this);
				LOG("finished task: %s", m_name.c_str());
			}
		};

	private:
		class TaskThread {
			std::thread m_thread;
			Semaphore m_semaphore;
			TaskMaster* m_pTaskMaster;
			Task m_task;
			bool m_done;
			bool m_running;

			void runTask() {
				m_task.run(m_pTaskMaster);
				m_done = true;
			}

			void threadEntry() {
				while (m_running) {
					if (!m_task)
						wait();
					runTask();
					if (m_task) {
						m_task.clearFunction();
						m_pTaskMaster->tick(); // can start next task now.
					}
				}
			}

		public:

			TaskThread(TaskMaster* pTaskMaster) : m_task("NoTask", nullptr) {
				m_thread = std::thread(std::bind(&TaskThread::threadEntry, this));
				m_pTaskMaster = pTaskMaster;
				m_done = true;
				m_running = true;
			}

			void setTask(const Task& task) {
				m_task = task;
			}

			const Task& getTask() {
				return m_task;
			}

			void shutdown() {
				m_running = false;
				while (!isDone()) {
					std::this_thread::sleep_for(std::chrono::milliseconds(2000));
				}
				m_semaphore.signal();
				m_thread.join();
			}

			void start() {
				m_done = false;
				m_semaphore.signal();
			}

			void wait() {
				m_semaphore.wait();
			}

			bool isDone() const {
				return m_done;
			}
		};

		struct ResourceState {
			ResourceState() : readers(0), writers(0) {}
			int readers;
			int writers;
		};

		struct Murmur3
		{
		public:
			std::size_t operator() (const std::string& s) const
			{
				int32_t hashValue;
				MurmurHash3_x86_32(s.c_str(), s.length(), 3, &hashValue);
				return hashValue;
			}
		};

		std::mutex m_taskMutex;
		std::vector<Task> m_tasks;
		std::unordered_map<std::string, ResourceState, Murmur3> m_resources;
		std::array<TaskThread*, numThreads> m_threads;

		void startTask(int threadIndex, Task& task) {
			for (const auto& readResource : task.m_readAccess) {
				++m_resources[readResource].readers;
			}
			for (const auto& writeResource : task.m_writeAccess) {
				++m_resources[writeResource].writers;
			}

			m_threads[threadIndex]->setTask(task);
			m_threads[threadIndex]->start();
		}

		void onTaskFinished(const Task& task) {
			std::lock_guard<std::mutex> lock(m_taskMutex);
			for (const auto& readResource : task.m_readAccess) {
				--m_resources[readResource].readers;
			}
			for (const auto& writeResource : task.m_writeAccess) {
				--m_resources[writeResource].writers;
			}
		}

		int getFreeThreadIndex() {
			for (int i = 0; i < numThreads; ++i) {
				if (m_threads[i]->isDone()) {
					return i;
				}
			}
			return -1;
		}

	public:
		TaskMaster() {
			for (int i = 0; i < numThreads; ++i) {
				m_threads[i] = new TaskThread(this);
			}
		}

		~TaskMaster() {
			for (int i = 0; i < numThreads; ++i) {
				m_threads[i]->shutdown();
				delete m_threads[i];
			}
		}

		void addTask(const Task& task) {
			std::lock_guard<std::mutex> lock(m_taskMutex);
			m_tasks.push_back(task);
		}

		void tick() {
			std::lock_guard<std::mutex> lock(m_taskMutex);
			for (int i = 0; i<numThreads; ++i) {
				int freeThreadIndex = getFreeThreadIndex();
				if (freeThreadIndex >= 0) {
					for (auto it = m_tasks.begin(); it != m_tasks.end(); ++it) {
						auto& task = *it;
						bool okToStart = true;
						for (auto& readResource : task.m_readAccess) {
							if (m_resources[readResource].writers > 0) {
								okToStart = false;
							}
						}
						for (auto& writeResource : task.m_writeAccess) {
							auto& resourceState = m_resources[writeResource];
							if (resourceState.readers > 0 || resourceState.writers > 0) {
								okToStart = false;
							}
						}

						if (okToStart) {
							startTask(freeThreadIndex, task);
							m_tasks.erase(it);
							return;
						}
					}
				}
				else {
					LOG("No TaskThreads are free :(");
					return;
				}
			}
		}
	};

}