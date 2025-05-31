#include <enchantum/enchantum.hpp> // Should include all necessary headers
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <numeric> // For std::iota if used
#include <random>  // For shuffling or random selection

// --- Enums for Benchmarking ---

enum class SmallContiguousEnum { A, B, C, D, E };

enum class LargeContiguousEnum {
    V0, V1, V2, V3, V4, V5, V6, V7, V8, V9,
    V10, V11, V12, V13, V14, V15, V16, V17, V18, V19,
    V20, V21, V22, V23, V24, V25, V26, V27, V28, V29,
    V30, V31, V32, V33, V34, V35, V36, V37, V38, V39,
    V40, V41, V42, V43, V44, V45, V46, V47, V48, V49,
    // ... up to V99 or more for a decent size
    V50, V51, V52, V53, V54, V55, V56, V57, V58, V59,
    V60, V61, V62, V63, V64, V65, V66, V67, V68, V69,
    V70, V71, V72, V73, V74, V75, V76, V77, V78, V79,
    V80, V81, V82, V83, V84, V85, V86, V87, V88, V89,
    V90, V91, V92, V93, V94, V95, V96, V97, V98, V99
};

enum class SmallNonContiguousEnum { A = 10, B = 20, C = 30, D = 40, E = 50 };

enum class LargeNonContiguousEnum {
    // Create about 50-100 non-contiguous values
    LNC_V0 = 0, LNC_V1 = 2, LNC_V2 = 5, LNC_V3 = 10, LNC_V4 = 12,
    LNC_V5 = 20, LNC_V6 = 23, LNC_V7 = 30, LNC_V8 = 35, LNC_V9 = 41,
    LNC_V10 = 50, LNC_V11 = 53, LNC_V12 = 60, LNC_V13 = 66, LNC_V14 = 72,
    LNC_V15 = 81, LNC_V16 = 85, LNC_V17 = 93, LNC_V18 = 99, LNC_V19 = 105,
    LNC_V20 = 111, LNC_V21 = 120, LNC_V22 = 123, LNC_V23 = 133, LNC_V24 = 140,
    LNC_V25 = 145, LNC_V26 = 155, LNC_V27 = 160, LNC_V28 = 163, LNC_V29 = 177,
    LNC_V30 = 180, LNC_V31 = 183, LNC_V32 = 190, LNC_V33 = 195, LNC_V34 = 201,
    LNC_V35 = 210, LNC_V36 = 213, LNC_V37 = 220, LNC_V38 = 225, LNC_V39 = 231,
    LNC_V40 = 240, LNC_V41 = 243, LNC_V42 = 250, LNC_V43 = 255, LNC_V44 = 258 // Max value for default ENCHANTUM_MAX_RANGE is 256
    // Max value for default ENCHANTUM_MAX_RANGE is 256, ensure these values are within range or adjust benchmark settings.
    // Let's limit to < 256 for default enchantum behavior.
};


// Helper to prevent optimization and use results
volatile int dummy_int_sink = 0;
volatile bool dummy_bool_sink = false;
volatile size_t dummy_size_t_sink = 0;
enchantum::string_view dummy_sv_sink;


template<typename EnumType>
void benchmark_enum_to_index(const std::string& enum_name, int iterations) {
    auto start_time = std::chrono::high_resolution_clock::now();
    size_t temp_sink = 0;
    for (int i = 0; i < iterations; ++i) {
        for (const auto val : enchantum::values<EnumType>) {
            auto index = enchantum::enum_to_index(val);
            if (index) {
                temp_sink += *index;
            }
        }
    }
    dummy_size_t_sink += temp_sink; // Use the result

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << enum_name << " - enum_to_index: " << duration.count() << " us" << std::endl;
}

template<typename EnumType>
void benchmark_to_string(const std::string& enum_name, int iterations) {
    auto start_time = std::chrono::high_resolution_clock::now();
    size_t total_len = 0;
    for (int i = 0; i < iterations; ++i) {
        for (const auto val : enchantum::values<EnumType>) {
            enchantum::string_view sv = enchantum::to_string(val);
            total_len += sv.length(); // Use the result
        }
    }
    dummy_size_t_sink += total_len;

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << enum_name << " - to_string: " << duration.count() << " us" << std::endl;
}

template<typename EnumType>
void benchmark_cast_from_string(const std::string& enum_name, int iterations) {
    std::vector<enchantum::string_view> names_vec;
    for(const auto name_sv : enchantum::names<EnumType>) {
        names_vec.push_back(name_sv);
    }
    if (names_vec.empty()) {
        std::cout << enum_name << " - cast_from_string: SKIPPED (no names)" << std::endl;
        return;
    }

    auto start_time = std::chrono::high_resolution_clock::now();
    size_t cast_count = 0;
    for (int i = 0; i < iterations; ++i) {
        for (const auto& name_to_cast : names_vec) {
            auto enum_val = enchantum::cast<EnumType>(name_to_cast);
            if (enum_val) {
                cast_count++;
            }
        }
    }
    dummy_size_t_sink += cast_count;

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << enum_name << " - cast_from_string: " << duration.count() << " us" << std::endl;
}

template<typename EnumType>
void benchmark_contains_value(const std::string& enum_name, int iterations) {
    std::vector<EnumType> values_to_check;
    for(const auto val : enchantum::values<EnumType>) {
        values_to_check.push_back(val);
    }
    // Add some potentially non-existing values if underlying type allows
    if constexpr (std::is_integral_v<std::underlying_type_t<EnumType>>) {
         if (!enchantum::values<EnumType>.empty()) {
            values_to_check.push_back(static_cast<EnumType>(enchantum::to_underlying(enchantum::values<EnumType>.back()) + 100));
            values_to_check.push_back(static_cast<EnumType>(enchantum::to_underlying(enchantum::values<EnumType>.front()) - 100));
         } else {
            values_to_check.push_back(static_cast<EnumType>(1000)); // arbitrary
         }
    }
     if (values_to_check.empty()) { // Should not happen if values<E> is not empty
        std::cout << enum_name << " - contains_value: SKIPPED (no values to check)" << std::endl;
        return;
    }

    auto start_time = std::chrono::high_resolution_clock::now();
    bool temp_sink = false;
    for (int i = 0; i < iterations; ++i) {
        for (const auto val_to_check : values_to_check) {
            temp_sink ^= enchantum::contains(val_to_check);
        }
    }
    dummy_bool_sink ^= temp_sink;

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << enum_name << " - contains_value: " << duration.count() << " us" << std::endl;
}

template<typename EnumType>
void benchmark_contains_name(const std::string& enum_name, int iterations) {
    std::vector<enchantum::string_view> names_to_check;
     for(const auto name_sv : enchantum::names<EnumType>) {
        names_to_check.push_back(name_sv);
    }
    names_to_check.push_back("NonExistentName123");
    names_to_check.push_back("AnotherMissingName");


    auto start_time = std::chrono::high_resolution_clock::now();
    bool temp_sink = false;
    for (int i = 0; i < iterations; ++i) {
        for (const auto& name_s : names_to_check) {
            temp_sink ^= enchantum::contains<EnumType>(name_s);
        }
    }
    dummy_bool_sink ^= temp_sink;

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << enum_name << " - contains_name: " << duration.count() << " us" << std::endl;
}


int main() {
    const int base_iterations = 10000; // Adjust as needed

    std::cout << "--- SmallContiguousEnum ---" << std::endl;
    benchmark_enum_to_index<SmallContiguousEnum>("SmallContiguousEnum", base_iterations * 10);
    benchmark_to_string<SmallContiguousEnum>("SmallContiguousEnum", base_iterations * 10);
    benchmark_cast_from_string<SmallContiguousEnum>("SmallContiguousEnum", base_iterations * 10);
    benchmark_contains_value<SmallContiguousEnum>("SmallContiguousEnum", base_iterations * 10);
    benchmark_contains_name<SmallContiguousEnum>("SmallContiguousEnum", base_iterations * 10);
    std::cout << std::endl;

    std::cout << "--- LargeContiguousEnum ---" << std::endl;
    benchmark_enum_to_index<LargeContiguousEnum>("LargeContiguousEnum", base_iterations / 5); // Fewer iterations for larger enums
    benchmark_to_string<LargeContiguousEnum>("LargeContiguousEnum", base_iterations / 5);
    benchmark_cast_from_string<LargeContiguousEnum>("LargeContiguousEnum", base_iterations / 5);
    benchmark_contains_value<LargeContiguousEnum>("LargeContiguousEnum", base_iterations / 5);
    benchmark_contains_name<LargeContiguousEnum>("LargeContiguousEnum", base_iterations / 5);
    std::cout << std::endl;

    std::cout << "--- SmallNonContiguousEnum ---" << std::endl;
    benchmark_enum_to_index<SmallNonContiguousEnum>("SmallNonContiguousEnum", base_iterations * 10);
    benchmark_to_string<SmallNonContiguousEnum>("SmallNonContiguousEnum", base_iterations * 10);
    benchmark_cast_from_string<SmallNonContiguousEnum>("SmallNonContiguousEnum", base_iterations * 10);
    benchmark_contains_value<SmallNonContiguousEnum>("SmallNonContiguousEnum", base_iterations * 10);
    benchmark_contains_name<SmallNonContiguousEnum>("SmallNonContiguousEnum", base_iterations * 10);
    std::cout << std::endl;

    std::cout << "--- LargeNonContiguousEnum ---" << std::endl;
    benchmark_enum_to_index<LargeNonContiguousEnum>("LargeNonContiguousEnum", base_iterations);
    benchmark_to_string<LargeNonContiguousEnum>("LargeNonContiguousEnum", base_iterations);
    benchmark_cast_from_string<LargeNonContiguousEnum>("LargeNonContiguousEnum", base_iterations);
    benchmark_contains_value<LargeNonContiguousEnum>("LargeNonContiguousEnum", base_iterations);
    benchmark_contains_name<LargeNonContiguousEnum>("LargeNonContiguousEnum", base_iterations);
    std::cout << std::endl;

    // To ensure dummy sinks are not optimized away entirely by some compilers
    if (dummy_int_sink == 12345 && dummy_bool_sink && dummy_size_t_sink == 54321) {
        std::cout << "Unlikely condition met." << std::endl;
    }

    return 0;
}
