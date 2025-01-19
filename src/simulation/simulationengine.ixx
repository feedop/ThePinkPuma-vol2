export module simulation:engine;

import std;
import glm;

import math;
import inversekinematics;
import :config;
import :data;

export class SimulationEngine
{
public:
	SimulationEngine() = default;

	~SimulationEngine()
	{
		if (running || simulationThread.joinable())
			stop();
	}

	void start()
	{
		if (simulationThread.joinable())
			simulationThread.join();
		if (simulationThread.joinable())
			simulationThread.join();

		running = true;
		simulationThread = std::thread(&SimulationEngine::threadFunction, this);
	}

	void stop()
	{
		running = false;
		simulationThread.join();
	}

	void pause()
	{
		std::lock_guard<decltype(cvMutex)> lk(cvMutex);
		paused = true;
	}

	void resume()
	{
		std::lock_guard<decltype(cvMutex)> lk(cvMutex);
		paused = false;
		cv.notify_one();
	}

	void setConfig(const SimulationConfig& config)
	{
		const std::lock_guard<std::mutex> lock(configMutex);
		this->config = config;
	}

	void setQ(const std::array<float, 6>& newQ)
	{
		// Mutex unnecessary, this shouldn't be called if simulation is in progress
		data.q1 = data.q2 = newQ;
	}

	void adjustQ(int index, bool forward)
	{
		// Mutex unnecessary, this shouldn't be called if simulation is in progress
		static constexpr float step = 0.01f;
		static constexpr float minQ2 = 0.2f;
		data.q1[index] += forward ? step : -step;

		if (index == 2) [[unlikely]]
		{
			data.q1[index] = std::max(minQ2, data.q1[index]);
		}
		else
		{
			math::normalizeAngle(data.q1[index]);
		}

		data.q2[index] = data.q1[index];
	}

	SimulationData getData()
	{
		const std::lock_guard<std::mutex> lock(dataMutex);
		return data;
	}

	inline bool isPaused()
	{
		return paused;
	}

	inline bool isRunning()
	{
		return running;
	}

	void setL(float newL1, float newL3, float newL4)
	{
		l1 = newL1;
		l3 = newL3;
		l4 = newL4;
	}

public: // Hack, friend not working
	SimulationConfig config;
	SimulationData data = { 0, 0, 0, 1.0f, 0, 0, 0, 0, 0, 1.0f, 0, 0, 0 };
	float l1, l3, l4;

	std::mutex configMutex, dataMutex, cvMutex;
	std::condition_variable cv;
	std::atomic_bool running = false;
	bool paused = false;
	std::thread simulationThread;

	void threadFunction()
	{
		data.t = 0;
		SimulationData localData, localDataCopy;
		SimulationConfig localConfig;
		{
			const std::lock_guard<std::mutex> lock(configMutex);
			localConfig = config;
		}
		{
			const std::lock_guard<std::mutex> lock(dataMutex);
			localData = data;
		}

		// Calculate for start and end (left scene)
		localDataCopy = localData;
		glm::mat4 startF5 = math::translate(localConfig.startPosition) * glm::toMat4(localConfig.startQuat);
		std::array<float, 6> startQ = solveInverseKinematics(localDataCopy.q1, startF5, l1, l3, l4);
		glm::mat4 endF5 = math::translate(localConfig.endPosition) * glm::toMat4(localConfig.endQuat);
		std::array<float, 6> endQ = solveInverseKinematics(localDataCopy.q1, endF5, l1, l3, l4);

		while (running)
		{
			nap();

			auto clockBegin = std::chrono::steady_clock::now();
			float fraction = localData.t / localConfig.totalTime;

			// 1. Interpolate q
			for (int i = 0; i < 6; i++)
			{
				if (i == 2) [[unlikely]]
				{
					localData.q1[i] = math::lerp(startQ[i], endQ[i], fraction);
				}
				else
				{
					localData.q1[i] = math::lerpAngle(startQ[i], endQ[i], fraction);
				}
			}

			// 2. Interpolate position and rotation
			auto position = math::lerp(localConfig.startPosition, localConfig.endPosition, fraction);
			auto rotation = math::slerp(localConfig.startQuat, localConfig.endQuat, fraction);
			glm::mat4 F5 = math::translate(position) * glm::toMat4(rotation);
			localData.q2 = solveInverseKinematics(localData.q2, F5, l1, l3, l4);

			if (localData.t >= localConfig.totalTime)
			{
				running = false;
				return;
			}
			localData.t += localConfig.dt;

			// Copy data
			{
				const std::lock_guard<std::mutex> lock(dataMutex);
				data = localData;
			}
			
			// Sleep
			int ns = localConfig.dt * 1e9;
			auto clockEnd = std::chrono::steady_clock::now();
			int elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(clockEnd - clockBegin).count();

			if (elapsed < ns)
				std::this_thread::sleep_for(std::chrono::nanoseconds(ns - elapsed));
			else
				std::cerr << "Can't sleep - the simulation step took too long\n";
		}
	}

	void nap()
	{
		std::unique_lock<decltype(cvMutex)> lk(cvMutex);
		cv.wait(lk, [this] { return !paused; });
	}
};