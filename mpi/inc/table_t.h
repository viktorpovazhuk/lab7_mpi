//
// Created by vityha on 27.05.22.
//

#ifndef TEMPLATE_TABLE_T_H
#define TEMPLATE_TABLE_T_H


#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility>

class table_t {
public:
    table_t(size_t n_rows, size_t n_cols, std::vector<double> &&data, double dx,double dy) : n_rows(n_rows), n_cols(n_cols), data(std::move(data)),
    delta_x(dx), delta_y(dy){}

    double &operator()(size_t i, size_t j) {
        if (i >= n_rows || j >= n_cols) {
            throw std::runtime_error("Invalid index: " + std::to_string(i) + " , " + std::to_string(j));
        }
        return data[n_cols * i + j];
    }

    double operator()(size_t i, size_t j) const {
        if (i >= n_rows || j >= n_cols) {
            throw std::runtime_error("Invalid index: " + std::to_string(i) + " , " + std::to_string(j));
        }
        return data[n_cols * i + j];
    }

    size_t rows() const {
        return n_rows;
    }

    size_t cols() const {
        return n_cols;
    }

    double &dx() {
        return delta_x;
    }

    double &dy() {
        return delta_y;
    }

private:
    size_t n_rows, n_cols;
    double delta_x, delta_y;
    std::vector<double> data;
};


#endif //TEMPLATE_TABLE_T_H
