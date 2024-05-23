#include <vector>
#include <cstddef>
#include <set>
#include <functional>
#include <random>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <cassert>


uint32_t SolveApproximately(std::vector<uint32_t> tasks, size_t num_machines) {
	std::multiset<uint32_t> machine_workloads;

	for (size_t _ = 0; _ < num_machines; ++_) {
		machine_workloads.insert(0);
	}

	// desc order
	std::sort(tasks.rbegin(), tasks.rend());

	for (uint32_t task : tasks) {
		uint32_t least_busy_workload = machine_workloads.extract(machine_workloads.begin()).value();
		machine_workloads.insert(least_busy_workload + task);
	}

	assert(std::accumulate(tasks.begin(), tasks.end(), 0u)
			== std::accumulate(machine_workloads.begin(), machine_workloads.end(), 0u));

	return *machine_workloads.rbegin();
}


uint32_t SolvePrecisely(std::vector<uint32_t> tasks, size_t num_machines) { 
	std::vector<uint32_t> machine_workloads(num_machines, 0);

	uint32_t result = -1;

	std::function<void(size_t, uint32_t)> brootforce;
	brootforce = [&](size_t task_i, const uint32_t cur_score) {
		if (cur_score >= result) {
			return;
		}
		if (task_i == tasks.size()) {
			result = cur_score;
			return;
		}
		uint32_t target_task = tasks[task_i];

		for (uint32_t& target_machine : machine_workloads) {
			target_machine += target_task;

			brootforce(task_i + 1, std::max(cur_score, target_machine));

			target_machine -= target_task;
		}
	};

	brootforce(0, 0);

	return result;
}


std::mt19937 gen;


template<typename D>
std::vector<uint32_t> GenerateTasks(size_t num_tasks, D distribution) {
	auto random_int = [&] { return std::abs(std::round(distribution(gen))); };

	std::vector<uint32_t> tasks(num_tasks);
	std::generate(tasks.begin(), tasks.end(), random_int);

	return tasks;
}


void PrintMeasures(const std::vector<double>& measures) {
	std::cout << "[";
	bool first = true;
	for (const auto& measure : measures) {
		if (!first) {
			std::cout << ", ";
		} else {
			first = false;
		}
		std::cout << measure;
	}
	std::cout << "]";
}


int main() {
	// consts
	constexpr size_t kNumSamples = 200;
	constexpr size_t kNumMachines = 6;
	constexpr size_t kNumTasks = 16;

	std::normal_distribution normal_d{200.0, 50.0};
	std::uniform_int_distribution uniform_d{50, 150};

	std::vector<double> measures(kNumSamples);
	for (auto& measure : measures) {
		auto tasks = GenerateTasks(kNumTasks, uniform_d);

		uint32_t approximate_result = SolveApproximately(tasks, kNumMachines);
		uint32_t precise_result     = SolvePrecisely    (tasks, kNumMachines);

		assert(approximate_result >= precise_result);

		measure = static_cast<double>(approximate_result) / static_cast<double>(precise_result);
	}

	PrintMeasures(measures);
	return 0;
}
