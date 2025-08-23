#include <chrono>
#include <iostream>
#include <random>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <numeric>

#include "../enchantum/include/enchantum/enchantum.hpp"

// Test with different policies
#undef ENCHANTUM_ENUM_DISPATCH_POLICY

// Define synthetic enums with various sizes and patterns
enum class Tiny3 { A, B, C };
enum class Small8 { Apple, Banana, Cherry, Date, Elderberry, Fig, Grape, Honeydew };
enum class Medium12 { Alpha, Beta, Gamma, Delta, Epsilon, Zeta, Eta, Theta, Iota, Kappa, Lambda, Mu };
enum class Large16 { 
    ActionScript, Bash, CPlusPlus, Dart, Erlang, Fortran, Go, Haskell,
    Java, Kotlin, Lisp, MATLAB, Nim, Objective_C, Python, R
};
enum class Large32 {
    Abacus, Babylon, Calculator, Database, Enterprise, Framework, Gateway, Hardware,
    Interface, JavaScript, Kubernetes, Library, Middleware, Network, Operating_System, Protocol,
    Query, Runtime, Server, Technology, Unix, Virtual_Machine, Wireless, eXtensible,
    Yield, Zone, Alpha_A, Beta_B, Gamma_C, Delta_D, Epsilon_E, Zeta_F
};
enum class Large64 {
    Application01, Browser02, Console03, Document04, Environment05, Framework06, 
    Gateway07, Hardware08, Interface09, JavaScript10, Kubernetes11, Library12,
    Middleware13, Network14, Operating15, Protocol16, Query17, Runtime18,
    Server19, Technology20, Unix21, Virtual22, Wireless23, eXtensible24,
    Yield25, Zone26, Alpha27, Beta28, Gamma29, Delta30, Epsilon31, Zeta32,
    Application33, Browser34, Console35, Document36, Environment37, Framework38,
    Gateway39, Hardware40, Interface41, JavaScript42, Kubernetes43, Library44,
    Middleware45, Network46, Operating47, Protocol48, Query49, Runtime50,
    Server51, Technology52, Unix53, Virtual54, Wireless55, eXtensible56,
    Yield57, Zone58, Alpha59, Beta60, Gamma61, Delta62, Epsilon63, Zeta64
};

// Bucket-heavy enum (many names start with same char and have same length)
enum class BucketHeavy16 {
    Apple01, Apple02, Apple03, Apple04, Apple05, Apple06, Apple07, Apple08,
    Bread01, Bread02, Bread03, Bread04, Bread05, Bread06, Bread07, Bread08
};

template<typename E>
std::vector<std::string> get_valid_names();

template<>
std::vector<std::string> get_valid_names<Tiny3>() {
    return {"A", "B", "C"};
}

template<>
std::vector<std::string> get_valid_names<Small8>() {
    return {"Apple", "Banana", "Cherry", "Date", "Elderberry", "Fig", "Grape", "Honeydew"};
}

template<>
std::vector<std::string> get_valid_names<Medium12>() {
    return {"Alpha", "Beta", "Gamma", "Delta", "Epsilon", "Zeta", "Eta", "Theta", "Iota", "Kappa", "Lambda", "Mu"};
}

template<>
std::vector<std::string> get_valid_names<Large16>() {
    return {"ActionScript", "Bash", "CPlusPlus", "Dart", "Erlang", "Fortran", "Go", "Haskell",
            "Java", "Kotlin", "Lisp", "MATLAB", "Nim", "Objective_C", "Python", "R"};
}

template<>
std::vector<std::string> get_valid_names<Large32>() {
    return {"Abacus", "Babylon", "Calculator", "Database", "Enterprise", "Framework", "Gateway", "Hardware",
            "Interface", "JavaScript", "Kubernetes", "Library", "Middleware", "Network", "Operating_System", "Protocol",
            "Query", "Runtime", "Server", "Technology", "Unix", "Virtual_Machine", "Wireless", "eXtensible",
            "Yield", "Zone", "Alpha_A", "Beta_B", "Gamma_C", "Delta_D", "Epsilon_E", "Zeta_F"};
}

template<>
std::vector<std::string> get_valid_names<Large64>() {
    return {"Application01", "Browser02", "Console03", "Document04", "Environment05", "Framework06",
            "Gateway07", "Hardware08", "Interface09", "JavaScript10", "Kubernetes11", "Library12",
            "Middleware13", "Network14", "Operating15", "Protocol16", "Query17", "Runtime18",
            "Server19", "Technology20", "Unix21", "Virtual22", "Wireless23", "eXtensible24",
            "Yield25", "Zone26", "Alpha27", "Beta28", "Gamma29", "Delta30", "Epsilon31", "Zeta32",
            "Application33", "Browser34", "Console35", "Document36", "Environment37", "Framework38",
            "Gateway39", "Hardware40", "Interface41", "JavaScript42", "Kubernetes43", "Library44",
            "Middleware45", "Network46", "Operating47", "Protocol48", "Query49", "Runtime50",
            "Server51", "Technology52", "Unix53", "Virtual54", "Wireless55", "eXtensible56",
            "Yield57", "Zone58", "Alpha59", "Beta60", "Gamma61", "Delta62", "Epsilon63", "Zeta64"};
}

template<>
std::vector<std::string> get_valid_names<BucketHeavy16>() {
    return {"Apple01", "Apple02", "Apple03", "Apple04", "Apple05", "Apple06", "Apple07", "Apple08",
            "Bread01", "Bread02", "Bread03", "Bread04", "Bread05", "Bread06", "Bread07", "Bread08"};
}

// Benchmark helper to prevent DCE
template<typename T>
void DoNotOptimize(T&& value) {
    asm volatile("" : : "r,m"(value) : "memory");
}

struct BenchResult {
    std::string enum_name;
    std::string policy;
    std::string workload;
    double mean_ns;
    double p90_ns;
    size_t iterations;
};

template<typename E>
std::vector<std::string> generate_workload(const std::string& type, size_t count) {
    std::vector<std::string> workload;
    std::vector<std::string> valid_names = get_valid_names<E>();
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, valid_names.size() - 1);
    
    if (type == "100%_positive") {
        for (size_t i = 0; i < count; ++i) {
            workload.push_back(valid_names[dis(gen)]);
        }
    } else if (type == "80%_positive") {
        for (size_t i = 0; i < count; ++i) {
            if (i % 5 == 0) {
                workload.push_back("NonExistent" + std::to_string(i));
            } else {
                workload.push_back(valid_names[dis(gen)]);
            }
        }
    } else if (type == "20%_positive") {
        for (size_t i = 0; i < count; ++i) {
            if (i % 5 == 0) {
                workload.push_back(valid_names[dis(gen)]);
            } else {
                workload.push_back("NonExistent" + std::to_string(i));
            }
        }
    } else if (type == "100%_negative_diff_len") {
        for (size_t i = 0; i < count; ++i) {
            workload.push_back("VeryLongNameThatDoesNotExist" + std::to_string(i));
        }
    } else if (type == "100%_negative_same_len") {
        for (size_t i = 0; i < count; ++i) {
            if (!valid_names.empty()) {
                std::string invalid = valid_names[0];
                invalid[0] = invalid[0] == 'Z' ? 'A' : invalid[0] + 1;
                workload.push_back(invalid + std::to_string(i % 10));
            }
        }
    }
    
    return workload;
}

template<typename E>
BenchResult benchmark_enum(const std::string& policy_name, const std::string& workload_type) {
    const size_t total_iterations = 100000;
    std::vector<std::string> workload = generate_workload<E>(workload_type, total_iterations);
    
    std::vector<double> times;
    times.reserve(100);
    
    // Warm up
    for (int i = 0; i < 10; ++i) {
        for (const auto& name : workload) {
            auto result = enchantum::cast<E>(name);
            DoNotOptimize(result);
        }
    }
    
    // Actual benchmark
    for (int run = 0; run < 100; ++run) {
        auto start = std::chrono::steady_clock::now();
        
        for (const auto& name : workload) {
            auto result = enchantum::cast<E>(name);
            DoNotOptimize(result);
        }
        
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        times.push_back(static_cast<double>(duration.count()) / total_iterations);
    }
    
    // Calculate statistics
    std::sort(times.begin(), times.end());
    double mean = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
    double p90 = times[static_cast<size_t>(times.size() * 0.9)];
    
    return {typeid(E).name(), policy_name, workload_type, mean, p90, total_iterations};
}

void run_benchmarks() {
    std::vector<BenchResult> results;
    
    std::vector<std::string> workloads = {
        "100%_positive", "80%_positive", "20%_positive", 
        "100%_negative_diff_len", "100%_negative_same_len"
    };
    
    std::cout << "Running benchmarks..." << std::endl;
    
    for (const auto& workload : workloads) {
        std::cout << "Workload: " << workload << std::endl;
        
        results.push_back(benchmark_enum<Tiny3>("AUTO", workload));
        results.push_back(benchmark_enum<Small8>("AUTO", workload));
        results.push_back(benchmark_enum<Medium12>("AUTO", workload));
        results.push_back(benchmark_enum<Large16>("AUTO", workload));
        results.push_back(benchmark_enum<Large32>("AUTO", workload));
        results.push_back(benchmark_enum<Large64>("AUTO", workload));
        results.push_back(benchmark_enum<BucketHeavy16>("AUTO", workload));
    }
    
    // Output results
    std::cout << "\n=== BENCHMARK RESULTS ===\n";
    std::cout << std::left << std::setw(20) << "Enum" 
              << std::setw(12) << "Policy" 
              << std::setw(25) << "Workload"
              << std::setw(12) << "Mean(ns)"
              << std::setw(12) << "P90(ns)" << "\n";
    std::cout << std::string(80, '-') << "\n";
    
    for (const auto& result : results) {
        std::cout << std::left << std::setw(20) << result.enum_name
                  << std::setw(12) << result.policy
                  << std::setw(25) << result.workload
                  << std::setw(12) << std::fixed << std::setprecision(2) << result.mean_ns
                  << std::setw(12) << std::fixed << std::setprecision(2) << result.p90_ns << "\n";
    }
}

int main() {
    std::cout << "Enhanced Enchantum Cast Benchmark\n";
    std::cout << "Policy: AUTO (default)\n";
    std::cout << "================================\n\n";
    
    run_benchmarks();
    
    return 0;
}