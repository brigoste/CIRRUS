#pragma once

#include <vector>
#include <utility>

class LinearSystem
{
public:
    explicit LinearSystem(int N = 0);

    void resize(int N);

    int size() const;

    // -----------------------------
    // Assembly
    // -----------------------------
    void addCoeff(int i, int j, double a);
    void addDiag(int i, double aP);

    void setRHS(int i, double b);
    void addRHS(int i, double b);

    void clearRow(int i);

    // -----------------------------
    // Sparse structure access
    // -----------------------------
    const std::vector<std::pair<int,double>>& row(int i) const;

    double diag(int i) const;

    const std::vector<int>& neighbors(int i) const;

    // -----------------------------
    // Solution vectors
    // -----------------------------
    std::vector<double>& rhs();
    const std::vector<double>& rhs() const;

    std::vector<double>& solution();
    const std::vector<double>& solution() const;

    std::vector<double>& solutionOld();

    // --- compatibility layer for solvers ---
    double coeff(int i, int j) const;
    const std::vector<double>& diagonal() const;

    void setSolution(const std::vector<double>& x);

private:
    int N_;

    std::vector<double> aP_;
    std::vector<double> b_;
    std::vector<double> x_;
    std::vector<double> x_old_;

    std::vector<std::vector<std::pair<int,double>>> A_;
    std::vector<std::vector<int>> nbrs_;
};
