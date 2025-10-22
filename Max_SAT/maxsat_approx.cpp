// maxsat_approx.cpp
// Single-file tool for approximating weighted Max-SAT.
//
// Input format (stdin):
// n m
// For each of m lines:
// w k l1 l2 ... lk
// where w is a non-negative weight (double or integer), k is number of literals,
// and each li is a signed integer in range [-n,-1] or [1,n]. Positive means x_i,
// negative means ¬x_i.
//
// The program prints two deterministic assignments found by derandomizing:
// 1) the 1/2-approximation obtained by derandomizing uniform random assignment,
// 2) the (1 - 1/e)-style algorithm: solve an LP relaxation, randomized-rounding
// probabilities, then derandomize by conditional expectation.
//
// Compile: g++ -O2 -std=c++17 maxsat_approx.cpp -o maxsat_approx.exe
// Example:
// 2 3
// 1 2 1 2
// 2 1 -1
// 3 1 2

#include <bits/stdc++.h>
using namespace std;

struct Clause {
    double w;
    vector<pair<int,int>> lits; // (var, sign) sign = +1 for positive, -1 for neg
};

// Evaluate total weight satisfied by assignment assign[1..n] (0/1)
double eval_assignment(const vector<int>& assign, const vector<Clause>& clauses) {
    double total = 0.0;
    int nclauses = clauses.size();
    for (int i = 0; i < nclauses; ++i) {
        const Clause &C = clauses[i];
        bool sat = false;
        for (auto &p : C.lits) {
            int var = p.first;
            int sign = p.second;
            int val = assign[var];
            if (val == -1) val = 0; // treat unassigned as false
            if ((sign == 1 && val == 1) || (sign == -1 && val == 0)) { sat = true; break; }
        }
        if (sat) total += C.w;
    }
    return total;
}

// Compute expected satisfied weight when some variables are fixed (assign[index]= -1 unassigned, 0/1 assigned)
// and remaining variables are randomized: for derand_half use p = 0.5; for LP rounding use p = probs[var]
double expected_weight(const vector<int>& assign, const vector<Clause>& clauses, const vector<double>& probs, double default_p) {
    double total = 0.0;
    int m = clauses.size();
    for (int i = 0; i < m; ++i) {
        const Clause &C = clauses[i];
        bool already_sat = false;
        double prod_unsat = 1.0;
        bool has_unassigned = false;
        for (auto &lit : C.lits) {
            int var = lit.first;
            int sign = lit.second;
            int a = assign[var];
            if (a != -1) {
                // fixed
                if ((sign == 1 && a == 1) || (sign == -1 && a == 0)) { already_sat = true; break; }
                else continue; // this literal cannot satisfy
            } else {
                has_unassigned = true;
                double p;
                if (!probs.empty()) p = probs[var]; else p = default_p;
                double lit_prob_true = (sign == 1) ? p : (1.0 - p);
                prod_unsat *= (1.0 - lit_prob_true);
            }
        }
        double prob_sat = 0.0;
        if (already_sat) prob_sat = 1.0;
        else {
            if (!has_unassigned) prob_sat = 0.0;
            else prob_sat = 1.0 - prod_unsat;
        }
        total += C.w * prob_sat;
    }
    return total;
}

// Derandomized 1/2 algorithm: variables set greedily by conditional expectation where unassigned vars are uniform p=1/2
vector<int> derand_half(int n, const vector<Clause>& clauses) {
    vector<int> assign(n+1, -1);
    for (int v = 1; v <= n; ++v) {
        assign[v] = 1;
        double e1 = expected_weight(assign, clauses, {}, 0.5);
        assign[v] = 0;
        double e0 = expected_weight(assign, clauses, {}, 0.5);
        if (e1 > e0) assign[v] = 1; else assign[v] = 0;
    }
    return assign;
}

// Simplex solver for LP in standard form: maximize c^T x subject to A x <= b, x >= 0
const double EPS = 1e-9;

struct Simplex {
    int m, n; // m constraints, n variables
    vector<vector<double>> a; // tableau (m+1) x (n+1), last column is RHS
    vector<int> B, N;
    Simplex(const vector<vector<double>>& A, const vector<double>& b, const vector<double>& c) {
        m = (int)A.size();
        n = (int)A[0].size();
        a.assign(m+1, vector<double>(n+1));
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) a[i+1][j+1] = A[i][j];
            a[i+1][0] = b[i];
        }
        for (int j = 0; j < n; ++j) a[0][j+1] = -c[j];
        a[0][0] = 0;
        B.assign(m+1, 0);
        N.assign(n+1, 0);
        for (int i = 1; i <= m; ++i) B[i] = n + i;
        for (int j = 1; j <= n; ++j) N[j] = j;
    }

    void pivot(int r, int s) {
        double inv = 1.0 / a[r][s];
        for (int j = 0; j <= n; ++j) if (j != s) a[r][j] *= inv;
        a[r][s] = inv;
        for (int i = 0; i <= m; ++i) if (i != r) {
            double coef = a[i][s];
            for (int j = 0; j <= n; ++j) if (j != s) a[i][j] -= coef * a[r][j];
            a[i][s] = -coef * inv;
        }
        swap(B[r], N[s]);
    }

    // returns pair (maxValue, solution vector x of size n)
    pair<double, vector<double>> solve() {
        while (true) {
            int s = 0;
            for (int j = 1; j <= n; ++j) if (a[0][j] < -EPS) { s = j; break; }
            if (s == 0) break;
            int r = 0;
            double minRatio = 1e300;
            for (int i = 1; i <= m; ++i) {
                if (a[i][s] > EPS) {
                    double ratio = a[i][0] / a[i][s];
                    if (ratio < minRatio - EPS) { minRatio = ratio; r = i; }
                }
            }
            if (r == 0) {
                // unbounded
                return {numeric_limits<double>::infinity(), vector<double>()};
            }
            pivot(r, s);
        }

        vector<double> x(n+1, 0.0);
        for (int i = 1; i <= m; ++i) if (B[i] <= n) x[B[i]] = a[i][0];
        vector<double> sol(n);
        for (int j = 1; j <= n; ++j) sol[j-1] = x[j];
        return {a[0][0], sol};
    }
};

// Build and solve LP relaxation, return vector<double> x_probs for variables 1..n
vector<double> solve_lp_relaxation(int n, const vector<Clause>& clauses) {
    int m = clauses.size();
    // Variables: x_1..x_n, y_1..y_m  => total n + m
    int Nvars = n + m;
    // Constraints:
    // For each clause C: y_C - sum_{pos in C} x_i + sum_{neg in C} x_i <= (#neg)
    // For each x_i: x_i <= 1
    // For each y_C: y_C <= 1
    int mcons = m + n + m;
    vector<vector<double>> A(mcons, vector<double>(Nvars, 0.0));
    vector<double> b(mcons, 0.0);
    int row = 0;
    for (int ci = 0; ci < m; ++ci) {
        const Clause &C = clauses[ci];
        int negcnt = 0;
        for (auto &lit : C.lits) {
            int var = lit.first;
            int sign = lit.second;
            if (sign == 1) {
                // coefficient -1 for x_var
                A[row][var-1] += -1.0;
            } else {
                // neg literal contributes +1 for x_var and constant +1 on RHS
                A[row][var-1] += +1.0;
                negcnt += 1;
            }
        }
        // y_C coefficient +1 at position n + ci
        A[row][n + ci] = 1.0;
        b[row] = (double)negcnt;
        row++;
    }
    // x_i <= 1
    for (int i = 0; i < n; ++i) {
        A[row][i] = 1.0;
        b[row] = 1.0;
        row++;
    }
    // y_C <= 1
    for (int ci = 0; ci < m; ++ci) {
        A[row][n + ci] = 1.0;
        b[row] = 1.0;
        row++;
    }

    // objective: maximize sum w_C * y_C => c vector of length Nvars
    vector<double> c(Nvars, 0.0);
    for (int ci = 0; ci < m; ++ci) c[n + ci] = clauses[ci].w;

    Simplex solver(A, b, c);
    auto res = solver.solve();
    if (!isfinite(res.first)) {
        // fallback: uniform 1/2
        vector<double> fallback(n+1, 0.5);
        return fallback;
    }
    vector<double> sol = res.second; // size Nvars
    vector<double> x_probs(n+1, 0.0);
    for (int i = 0; i < n; ++i) {
        double v = sol[i];
        if (!isfinite(v)) v = 0.0;
        if (v < 0) v = 0.0; if (v > 1) v = 1.0;
        x_probs[i+1] = v;
    }
    return x_probs;
}

// Derandomize randomized rounding with probabilities x_probs using conditional expectation
vector<int> derand_lp_rounding(int n, const vector<Clause>& clauses, const vector<double>& x_probs) {
    vector<int> assign(n+1, -1);
    for (int v = 1; v <= n; ++v) {
        assign[v] = 1;
        double e1 = expected_weight(assign, clauses, x_probs, 0.0);
        assign[v] = 0;
        double e0 = expected_weight(assign, clauses, x_probs, 0.0);
        if (e1 > e0) assign[v] = 1; else assign[v] = 0;
    }
    return assign;
}

// Brute-force exact solver: checks all 2^n assignments and returns best value and assignment.
pair<double, vector<int>> brute_force_opt(int n, const vector<Clause>& clauses) {
    if (n >= 64) {
        cerr << "Brute-force enumeration for n >= 64 is not supported (would overflow 64-bit loop).\n";
        return { -1.0, vector<int>() };
    }
    uint64_t total = (n == 64) ? 0ULL : (1ULL << n);
    double bestVal = -1.0;
    vector<int> bestAssign(n+1, 0);
    vector<int> assign(n+1, 0);
    const uint64_t PROGRESS_STEP = max<uint64_t>(1ULL << 20, total / 100ULL); 
    for (uint64_t mask = 0; mask < total; ++mask) {
        if ((mask & PROGRESS_STEP) == 0) {
            if (mask != 0) {
                double pct = (double)mask / (double)total * 100.0;
                cerr << "brute-force progress: " << pct << "% (mask=" << mask << ")\r";
            }
        }
        for (int i = 0; i < n; ++i) assign[i+1] = ((mask >> i) & 1ULL) ? 1 : 0;
        double val = eval_assignment(assign, clauses);
        if (val > bestVal) {
            bestVal = val;
            bestAssign = assign;
        }
    }
    if (total > 0) cerr << "brute-force progress: 100%\n";
    return { bestVal, bestAssign };
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // input filename (placed in the same folder as the executable)
    const char *FNAME = "input1.txt";
    ifstream fin(FNAME);
    if (!fin) {
        cerr << "Failed to open input file: " << FNAME << "\n";
        return 1;
    }
    istream &in = fin;

    int n, m;
    if (!(in >> n >> m)) {
        cerr << "Failed to read n m from " << FNAME << ". Expected: n m then m lines of: w k lit...\n";
        return 1;
    }
    vector<Clause> clauses;
    clauses.reserve(m);
    for (int i = 0; i < m; ++i) {
    double w; int k;
    in >> w >> k;
        Clause C; C.w = w;
        for (int j = 0; j < k; ++j) {
            int lit; in >> lit;
            int var = abs(lit);
            int sign = (lit > 0) ? 1 : -1;
            if (var < 1 || var > n) {
                cerr << "Literal variable index out of range: " << lit << "\n";
                return 1;
            }
            C.lits.emplace_back(var, sign);
        }
        clauses.push_back(move(C));
    }

    // 1) derandomized 1/2
    using Clock = chrono::high_resolution_clock;
    auto t1 = Clock::now();
    auto assign_half = derand_half(n, clauses);
    auto t2 = Clock::now();
    double val_half = eval_assignment(assign_half, clauses);
    auto t3 = Clock::now();

    // 2) LP relaxation and derandomized rounding (measure LP build+solve separately from rounding)
    auto t_lp_start = Clock::now();
    auto x_probs = solve_lp_relaxation(n, clauses);
    auto t_lp_end = Clock::now();

    auto t_rnd_start = Clock::now();
    auto assign_lp = derand_lp_rounding(n, clauses, x_probs);
    auto t_rnd_end = Clock::now();
    double val_lp = eval_assignment(assign_lp, clauses);

    cout.setf(ios::fixed); cout<<setprecision(6);
    auto dur_derand_half_s = chrono::duration<double>(t2 - t1).count();
    auto dur_eval_after_half_s = chrono::duration<double>(t3 - t2).count();
    auto dur_lp_solve_s = chrono::duration<double>(t_lp_end - t_lp_start).count();
    auto dur_derand_round_s = chrono::duration<double>(t_rnd_end - t_rnd_start).count();
    auto dur_derand_half_us = chrono::duration_cast<chrono::microseconds>(t2 - t1).count();
    auto dur_eval_after_half_us = chrono::duration_cast<chrono::microseconds>(t3 - t2).count();
    auto dur_lp_solve_us = chrono::duration_cast<chrono::microseconds>(t_lp_end - t_lp_start).count();
    auto dur_derand_round_us = chrono::duration_cast<chrono::microseconds>(t_rnd_end - t_rnd_start).count();

    cout << "Derandomized 1/2 assignment (value = " << val_half << "):\n";
    for (int i = 1; i <= n; ++i) cout << assign_half[i] << (i==n? '\n' : ' ');
    cout << "Derandomized LP-rounding assignment (value = " << val_lp << "):\n";
    for (int i = 1; i <= n; ++i) cout << assign_lp[i] << (i==n? '\n' : ' ');

    // print probabilities from LP for reference
    cout << "LP variable probabilities:\n";
    for (int i = 1; i <= n; ++i) cout << x_probs[i] << (i==n? '\n' : ' ');

    // Run brute-force (if n small) to get optimal for comparison
    // measure brute-force time
    auto t_brute_start = Clock::now();
    auto brute = brute_force_opt(n, clauses);
    auto t_brute_end = Clock::now();
    auto dur_brute_s = chrono::duration<double>(t_brute_end - t_brute_start).count();
    auto dur_brute_us = chrono::duration_cast<chrono::microseconds>(t_brute_end - t_brute_start).count();

    if (brute.first < 0.0) {
        cout << "Brute-force exact solution skipped (n too large).\n";
    } else {
        cout << "Brute-force optimal value = " << brute.first << "\n";
        cout << "Brute-force optimal assignment:\n";
        for (int i = 1; i <= n; ++i) cout << brute.second[i] << (i==n? '\n' : ' ');
    }
    cout << "\nTimings:\n";
    // print seconds with higher precision, and microseconds as integer to avoid 0.000000 display
    cout << setprecision(9);
    cout << "  derandomized 1/2 build time: "  << dur_derand_half_us << " us\n";
    // cout << "  eval time after derandomized 1/2: " << dur_eval_after_half_s << " s (" << dur_eval_after_half_us << " us)\n";
    cout << "  LP build+solve time: " <<  dur_lp_solve_us << " us\n";
    cout << "  derandomized LP-rounding time: " << dur_derand_round_us << " us\n";
    cout << "  brute-force time: " << dur_brute_us << " us\n";
    cout << setprecision(6);


    // If all measured microsecond counts are zero, offer a quick averaged micro-benchmark (cheap fallback)
    if (dur_derand_half_us == 0 && dur_eval_after_half_us == 0 && dur_lp_solve_us == 0 && dur_derand_round_us == 0) {
        cout << "Measurements are all below 1 microsecond (too small to measure on this input).\n";
        cout << "Running a short averaged micro-benchmark (1000 repeats) to get measurable times...\n";
        const int REPEATS = 1000;
        // measure derand_half repeated
        auto tA1 = Clock::now();
        for (int r=0;r<REPEATS;++r) { auto tmp = derand_half(n, clauses); (void)tmp; }
        auto tA2 = Clock::now();
        auto avg_derand_half_us = chrono::duration_cast<chrono::microseconds>(tA2 - tA1).count() / (double)REPEATS;

        // measure LP build+solve repeated
        auto tB1 = Clock::now();
        for (int r=0;r<REPEATS;++r) { auto tmp = solve_lp_relaxation(n, clauses); (void)tmp; }
        auto tB2 = Clock::now();
        auto avg_lp_us = chrono::duration_cast<chrono::microseconds>(tB2 - tB1).count() / (double)REPEATS;

        // measure derand rounding repeated
        auto tC1 = Clock::now();
        for (int r=0;r<REPEATS;++r) { auto tmp = derand_lp_rounding(n, clauses, x_probs); (void)tmp; }
        auto tC2 = Clock::now();
        auto avg_round_us = chrono::duration_cast<chrono::microseconds>(tC2 - tC1).count() / (double)REPEATS;

        cout << setprecision(6);
        cout << "Averaged micro-benchmark (per repeat):\n";
        cout << "  derandomized 1/2 (avg): " << avg_derand_half_us << " us\n";
        cout << "  LP build+solve (avg):    " << avg_lp_us << " us\n";
        cout << "  derand rounding (avg):  " << avg_round_us << " us\n";
    }

    return 0;
}
