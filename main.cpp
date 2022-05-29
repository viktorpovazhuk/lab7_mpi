// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "options_parser.h"
#include "table_t.h"
#include "errors.h"
#include "image_writer.h"

#include "boost/mpi.hpp"

#include <vector>
#include <iostream>
#include <iterator>
#include <fstream>
#include <algorithm>


namespace mpi = boost::mpi;

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

void calculate_bound_points(table_t &old_part, table_t &new_part, double alpha, double dt) {

}

void swap_bound_points(size_t bound_length, double *this_bound_points, double *neighbours_bound_points) {

}

std::vector<table_t> read_split_file(std::string &table_path, size_t n_rows, size_t n_cols, double dx, double dy, int num_processors) {
    std::ifstream is(table_path);
    std::istream_iterator<double> start_iter(is), end_iter;

//    for (double i: temps) {
//        std::cout << i;
//    }

    size_t quotient = n_rows / num_processors;
    size_t remainder = n_rows % num_processors;

    std::vector<table_t> parts;
    parts.reserve(num_processors);
    for (size_t p = 0; p < num_processors; ++p) {
        start_pointer = end_pointer;
        end_pointer += n_cols*(quotient + (p < remainder));
        parts.emplace_back(n_rows, n_cols, start_pointer, dx, dy);
    }

    return parts;
}

int main(int argc, char *argv[]) {
    std::string config_path;
    if (argc < 2) {
        config_path = "../configs/index.cfg";
    } else {
        std::unique_ptr<command_line_options_t> command_line_options;
        try {
            command_line_options = std::make_unique<command_line_options_t>(argc, argv);
        }
        catch (std::exception &ex) {
            std::cerr << ex.what() << std::endl;
            return errors::OPTIONS_PARSER;
        }
        config_path = command_line_options->config_file;
    }

    std::unique_ptr<config_file_options_t> config_file_options;
    try {
        config_file_options = std::make_unique<config_file_options_t>(config_path);
    }
    catch (OpenConfigFileException &ex) {
        std::cerr << ex.what() << std::endl;
        return errors::OPEN_CFG_FILE;
    } catch (std::exception &ex) {
        std::cerr << ex.what() << std::endl;
        return errors::READ_CFG_FILE;
    }

    std::string table_path = config_file_options->table_f;
    double cp = config_file_options->cp;
    double k = config_file_options->k;
    double p = config_file_options->p;
    double height = config_file_options->height;
    double width = config_file_options->width;
    double dx = config_file_options->dx;
    double dy = config_file_options->dy;
    double dt = config_file_options->dt;
    double interval = config_file_options->interval;
    size_t n_rows = height / dy + 1;
    size_t n_cols = width / dx + 1;

    mpi::environment env;
    mpi::communicator comm;

    std::vector<table_t> parts = read_split_file(table_path, n_rows, n_cols, dx, dy, comm.size());




    return 0;
}
