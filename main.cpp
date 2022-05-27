// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include "options_parser.h"
#include "table_t.h"

void calculate_inside_points(table_t &old_part, table_t &new_part, double alpha, double dt) {
    for (size_t i = 1; i < old_part.rows() - 1; ++i) {
        for (size_t j = 1; j < old_part.cols() - 1; ++j) {
            new_part(i, j) = old_part(i, j) + alpha * dt *
                                              ((old_part(i - 1, j) - 2 * old_part(i, j) + old_part(i + 1, j)) /
                                               (old_part.dx() * old_part.dx()) +
                                               (old_part(i, j - 1) - 2 * old_part(i, j) + old_part(i, j + 1)) /
                                               (old_part.dy() * old_part.dy()));
        }
    }
}

void swap_bound_points(size_t bound_length, double *this_bound_points, double *neighbours_bound_points) {

}

int main(int argc, char *argv[]) {


    return 0;
}
