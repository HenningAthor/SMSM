#include "util.h"
#include <bit>

void pad(std::vector<std::string> &v,
         bool front) {
    size_t max = 0;
    for (auto &s: v) {
        max = std::max(max, s.size());
    }

    for (auto &s: v) {
        while (s.size() < max) {
            if (front) {
                s.insert(0, " ");
            } else {
                s.insert(s.size() - 1, " ");
            }
        }
    }
}

std::string double_to_string(double d,
                             int precision) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(precision) << d;
    return stream.str();
}

bool contains(std::set<uint32_t> &set,
              uint32_t x) {
    return set.find(x) != set.end();
}

std::vector<std::string> split(std::string &s,
                               char c) {
    std::vector<std::string> substrings;
    std::string sub;
    for (char c1: s) {
        if (c1 == c) {
            if (!sub.empty()) {
                substrings.push_back(sub);
            }
            sub.clear();
        } else {
            sub.push_back(c1);
        }
    }
    if (!sub.empty()) {
        substrings.push_back(sub);
    }

    return substrings;
}

bool contains(std::string &s,
              char c) {
    return s.find(c) != std::string::npos;
}

template<>
void co_sort<double, uint32_t, bool>(std::vector<double> &vec_to_sort,
                                     std::vector<uint32_t> &vec2,
                                     std::vector<bool> &vec3,
                                     size_t size,
                                     bool ascending) {
    bool sorted = false;
    for (size_t i = 0; !sorted && i < size; ++i) {
        sorted = true;
        for (size_t j = 0; j < size - i - 1; ++j) {
            bool descending_unsorted = !ascending && (vec_to_sort[j] < vec_to_sort[j + 1]);
            bool ascending_unsorted = ascending && (vec_to_sort[j] > vec_to_sort[j + 1]);

            if (descending_unsorted || ascending_unsorted) {
                std::swap(vec_to_sort[j], vec_to_sort[j + 1]);
                std::swap(vec2[j], vec2[j + 1]);
                std::vector<bool>::swap(vec3[j], vec3[j + 1]);
                sorted = false;
            }
        }
    }
}

char **string_to_argv(std::string &s, int &argc) {
    std::vector<std::string> substrings = split(s, ' ');

    char **argv = (char **) malloc(substrings.size() * sizeof(char *));
    for (size_t i = 0; i < substrings.size(); ++i) {
        argv[i] = (char *) malloc((substrings[i].size() + 1) * sizeof(char));
        for (size_t j = 0; j < substrings[i].size(); ++j) {
            argv[i][j] = substrings[i][j];
        }
        argv[i][substrings[i].size()] = '\0';
    }
    argc = (int) substrings.size();
    return argv;
}

void free_argv(int argc, char **argv) {
    for (int i = 0; i < argc; ++i) {
        free(argv[i]);
        argv[i] = nullptr;
    }
    free(argv);
}

void print_argv(int argc, char **argv) {
    for (int i = 0; i < argc; ++i) {
        std::cout << argv[i] << " ";
    }
    std::cout << std::endl;
}

unsigned long fact(unsigned long n) {
    if (n == 0)
        return 1;
    unsigned long res = 1;
    for (unsigned long i = 2; i <= n; i++)
        res = res * i;
    return res;
}

unsigned long nCk(unsigned long n,
                  unsigned long k) {
    return fact(n) / (fact(k) * fact(n - k));
}

size_t n_choose_k(size_t n, size_t k) {
    if (k == 0) {
        return 1;
    }
    return (n * n_choose_k(n - 1, k - 1)) / k;
}

size_t pow_2(size_t n){
    return 1 << n;
}

size_t ceil(size_t x, size_t y){
    return (x + y - 1) / y;
}

bool all_close(std::vector<double> &vec) {
    bool close = true;
    for (size_t i = 1; i < vec.size(); ++i) {
        close &= fabs(vec[0] - vec[i]) <= 0.000000001;
    }
    return close;
}

uint64_t random_int() {
    static std::random_device dev;
    static std::mt19937 rng(dev());
    static std::uniform_int_distribution<std::mt19937::result_type> dist(0, std::numeric_limits<uint64_t>::max());

    return dist(rng);
}

std::vector<std::string> get_directory_files(std::string &directory_path) {
    std::vector<std::string> paths;
    for (const auto &entry: std::filesystem::directory_iterator(directory_path)) {
        paths.push_back(entry.path().string());
    }
    std::sort(paths.begin(), paths.end());
    return paths;
}

void print_progress(double percentage) {
    int val = (int) (percentage * 100);
    int lpad = (int) (percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    fflush(stdout);
}

bool file_exists(const std::string &file_path) {
    // https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exists-using-standard-c-c11-14-17-c
    struct stat buffer{};
    return (stat(file_path.c_str(), &buffer) == 0);
}

bool double_eq(double a, double b, double epsilon) {
    return fabs(a - b) < epsilon;
}

std::chrono::steady_clock::time_point get_time_point() {
    return std::chrono::steady_clock::now();
}

double get_elapsed_seconds(std::chrono::steady_clock::time_point sp,
                           std::chrono::steady_clock::time_point ep) {
    return (double) std::chrono::duration_cast<std::chrono::nanoseconds>(ep - sp).count() / 1e9;
}

bool next_subset(std::vector<size_t> &set, size_t set_size, size_t n) {
    set[set_size - 1] += 1;
    if(set[set_size - 1] < n-1){
        return true;
    }
    size_t j = set_size - 1;

    for (size_t i = 0; i < set_size - 1; ++i) {
        if (set[set_size - i - 1] == n - i) {
            // this placed has reached its maximum
            set[set_size - i - 2] += 1;
            j = set_size - i - 2;
        } else {
            break;
        }
    }

    for (size_t k = j + 1; k < set_size; ++k) {
        set[k] = set[k - 1] + 1;
    }
    return set[0] <= n - set_size;
}

size_t next_subset(size_t bits, int k) {
    bits += 1;
    while (k != std::__popcount(bits)) {
        bits += 1;
    }
    return bits;
}

void generate_clique_valid_subsets(size_t max_n){
    std::string s = "//n, n_to_choose, sets\n";
    s += "const std::vector<std::vector<std::vector<size_t>>> valid_sets = {\n";

    for(size_t n = 2; n <= max_n; ++n) {
        s += "\t{\n";

        std::vector<size_t> edges;
        for(size_t i = 0; i < n; ++i){
            for(size_t j = i+1; j < n; ++j){
                edges.push_back(i);
                edges.push_back(j);
            }
        }

        for (size_t n_endpoints = 2; n_endpoints <= n; n_endpoints += 2) {
            s += "\t\t{\n";

            size_t n_to_choose = n_endpoints / 2;
            size_t n_edges = (n*(n-1)) / 2;

            std::vector<size_t> set(n_to_choose);
            std::iota(set.begin(), set.end(), 0);
            set[n_to_choose - 1] -= 1;

            std::vector<size_t> count(n, 0);

            while (next_subset(set, n_to_choose, n_edges)) {
                if (valid_set(set, edges, count)) {
                    s += "\t\t\t";
                    for(size_t i = 0; i < n_to_choose; ++i){
                        s += "" + std::to_string(set[i]) + ", ";
                    }
                    s.pop_back();
                    s.pop_back();
                    s += ",\n";
                }
            }
            s.pop_back();
            s.pop_back();
            s += "\n\t\t},\n";
        }
        s.pop_back();
        s.pop_back();
        s += "\n\t},\n";
    }
    s.pop_back();
    s.pop_back();
    s += "\n};";
    std::cout << s << std::endl;
}

bool valid_set(std::vector<size_t> &set, std::vector<size_t> &edges, std::vector<size_t> &count) {
    // reset count
    for (size_t i = 0; i < count.size(); ++i) {
        count[i] = 0;
    }

    for (size_t i = 0; i < set.size(); ++i) {
        count[edges[set[i] * 2]] += 1;
        count[edges[set[i] * 2 + 1]] += 1;
    }

    bool valid = true;
    for (size_t i = 0; i < count.size(); ++i) {
        valid &= count[i] <= 1;
    }

    return valid;
}

void div(std::vector<double> &res, const std::vector<double> &v1, const std::vector<double> &v2) {
    for (size_t i = 0; i < res.size(); ++i) {
        if (v2[i] != 0) {
            res[i] = v1[i] / v2[i];
        }
    }
}

void div(std::vector<double> &res, const std::vector<size_t> &v1, const std::vector<size_t> &v2) {
    for (size_t i = 0; i < res.size(); ++i) {
        if (v2[i] != 0) {
            res[i] = ((double) v1[i]) / ((double) v2[i]);
        }
    }
}

void div(std::vector<double> &res, const std::vector<double> &v1, const std::vector<size_t> &v2) {
    for (size_t i = 0; i < res.size(); ++i) {
        if (v2[i] != 0) {
            res[i] = v1[i] / ((double) v2[i]);
        }
    }
}

void div(std::vector<double> &res, const std::vector<size_t> &v1, const std::vector<double> &v2) {
    for (size_t i = 0; i < res.size(); ++i) {
        if (v2[i] != 0) {
            res[i] = ((double) v1[i]) / v2[i];
        }
    }
}

