//
// Created by vityha on 27.05.22.
//

#ifndef TEMPLATE_TABLE_T_H
#define TEMPLATE_TABLE_T_H

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility>

class table_t {
public:
    table_t(size_t n_rows, size_t n_cols, std::vector<double> &&data, double dx, double dy) : n_rows(n_rows),
                                                                                              n_cols(n_cols),
                                                                                              m_data(std::move(data)),
                                                                                              delta_x(dx),
                                                                                              delta_y(dy) {}

    table_t(size_t n_rows, size_t n_cols, double dx, double dy) : n_rows(n_rows), n_cols(n_cols),
                                                                  delta_x(dx), delta_y(dy),
                                                                  m_data(std::vector<double>(n_rows * n_cols, 0)) {
    }

    table_t() = default;

    table_t& operator=(const table_t &other) = default;

    double &operator()(size_t i, size_t j) {
        if (i >= n_rows || j >= n_cols) {
            throw std::runtime_error("Invalid index: " + std::to_string(i) + " , " + std::to_string(j));
        }
        return m_data[n_cols * i + j];
    }

    double operator()(size_t i, size_t j) const {
        if (i >= n_rows || j >= n_cols) {
            throw std::runtime_error("Invalid index: " + std::to_string(i) + " , " + std::to_string(j));
        }
        return m_data[n_cols * i + j];
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

    std::vector<double> &data() {
        return m_data;
    }

private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & m_data;
        ar & n_rows;
        ar & n_cols;
        ar & delta_x;
        ar & delta_y;
    }

    size_t n_rows, n_cols;
    double delta_x, delta_y;
    std::vector<double> m_data;
};


#endif //TEMPLATE_TABLE_T_H
