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

void calculate_bound_points(table_t &old_part, table_t &new_part, std::vector<double> others_points, double alpha, double dt, mpi::communicator &comm) {
    if (comm.rank() == 0) {
        size_t i = old_part.rows() - 1;
        for (size_t j = 1; j < old_part.cols() - 1; ++j) {
            new_part(i, j) = old_part(i, j) + alpha * dt *
                                              ((old_part(i - 1, j) - 2 * old_part(i, j) + others_points[j]) /
                                               (old_part.dx() * old_part.dx()) +
                                               (old_part(i, j - 1) - 2 * old_part(i, j) + old_part(i, j + 1)) /
                                               (old_part.dy() * old_part.dy()));
        }
    } else if (comm.rank() == comm.size() - 1) {
        size_t i = 0;
        for (size_t j = 1; j < old_part.cols() - 1; ++j) {
            new_part(i, j) = old_part(i, j) + alpha * dt *
                                              ((others_points[j] - 2 * old_part(i, j) + old_part(i + 1, j)) /
                                               (old_part.dx() * old_part.dx()) +
                                               (old_part(i, j - 1) - 2 * old_part(i, j) + old_part(i, j + 1)) /
                                               (old_part.dy() * old_part.dy()));
        }
    } else {
        size_t i = 0;
        for (size_t j = 1; j < old_part.cols() - 1; ++j) {
            new_part(i, j) = old_part(i, j) + alpha * dt *
                                              ((others_points[j] - 2 * old_part(i, j) + old_part(i + 1, j)) /
                                               (old_part.dx() * old_part.dx()) +
                                               (old_part(i, j - 1) - 2 * old_part(i, j) + old_part(i, j + 1)) /
                                               (old_part.dy() * old_part.dy()));
        }
        i = old_part.rows() - 1;
        for (size_t j = 1; j < old_part.cols() - 1; ++j) {
            new_part(i, j) = old_part(i, j) + alpha * dt *
                                              ((old_part(i - 1, j) - 2 * old_part(i, j) + others_points[old_part.cols() + j]) /
                                               (old_part.dx() * old_part.dx()) +
                                               (old_part(i, j - 1) - 2 * old_part(i, j) + old_part(i, j + 1)) /
                                               (old_part.dy() * old_part.dy()));
        }
    }
}

std::vector<table_t>
read_split_file(std::string &table_path, size_t n_rows, size_t n_cols, double dx, double dy, int num_processors) {
    std::ifstream is(table_path);
    std::istream_iterator<double> start_file_iter(is), end_file_iter;
    std::vector<double> temps(start_file_iter, end_file_iter);

    size_t quotient = n_rows / num_processors;
    size_t remainder = n_rows % num_processors;

    std::vector<table_t> parts;
    parts.reserve(num_processors);
    auto end_iter = temps.begin(), start_iter = temps.begin();
    for (size_t p = 0; p < num_processors; ++p) {
        start_iter = end_iter;
        size_t cur_n_rows = (quotient + (p < remainder));
        std::advance(end_iter, n_cols * cur_n_rows);
        std::vector<double> data(start_iter, end_iter);
        parts.emplace_back(cur_n_rows, n_cols, std::move(data), dx, dy);
    }

    return parts;
}

void send_init_tables(std::vector<table_t> &parts, mpi::communicator &comm) {
    std::vector<mpi::request> reqs(comm.size() - 1);
    for (size_t p = 1; p < comm.size(); ++p) {
        reqs[p-1] = comm.isend(p, 0, parts[p]);
    }
    mpi::wait_all(reqs.begin(), reqs.end());
}

void recv_init_tables(table_t &part, mpi::communicator &comm) {
    comm.recv(0, 0, part);
}

std::vector<double> send_recv_others_points(table_t &part, mpi::communicator &comm) {
    std::vector<double> others_points;
    if (comm.rank() == 0) {
        std::vector<double> bound_points(part.data().begin() + part.cols() * (part.rows() - 1), part.data().end());
        std::vector<mpi::request> reqs(2);
        reqs[0] = comm.isend(1, 0, bound_points);
        reqs[1] = comm.irecv(1, 0, others_points);
        mpi::wait_all(reqs.begin(), reqs.end());
    }
    else if (comm.rank() == comm.size() - 1) {
        std::vector<double> bound_points(part.data().begin(), part.data().begin() + part.cols());
        std::vector<mpi::request> reqs(2);
        reqs[0] = comm.isend(comm.size() - 2, 0, bound_points);
        reqs[1] = comm.irecv(comm.size() - 2, 0, others_points);
        mpi::wait_all(reqs.begin(), reqs.end());
    }
    else {
        int rank = comm.rank();
        std::vector<double> up_bound_points(part.data().begin(), part.data().begin() + part.cols());
        std::vector<double> down_bound_points(part.data().begin() + part.cols() * (part.rows() - 1), part.data().end());
        std::vector<double> up_others_points(part.cols()), down_others_points(part.cols());
        std::vector<mpi::request> reqs(4);
        reqs[0] = comm.isend(rank - 1, 0, up_bound_points);
        reqs[1] = comm.isend(rank + 1, 0, down_bound_points);
        reqs[2] = comm.irecv(rank - 1, 0, up_others_points);
        reqs[3] = comm.irecv(rank + 1, 0, down_others_points);
        mpi::wait_all(reqs.begin(), reqs.end());
        others_points.resize(part.cols() * 2);
        std::move(up_others_points.begin(), up_others_points.end(), others_points.begin());
        std::move(down_others_points.begin(), down_others_points.end(), others_points.begin() + up_others_points.size());
    }
    return others_points;
}

void gather_table(table_t &part, std::vector<table_t> &whole_table, mpi::communicator &comm) {
    mpi::gather(comm, part, whole_table, 0);
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
    double alpha = k / (p * cp);
    size_t num_iters = 30ul;

    mpi::environment env;
    mpi::communicator comm;
    int num_processors = comm.size();

    table_t old_part{0, 0, dx, dy}, new_part{0, 0, dx, dy};
    if (comm.rank() == 0) {
        std::vector<table_t> parts = read_split_file(table_path, n_rows, n_cols, dx, dy, num_processors);

        send_init_tables(parts, comm);

        old_part = parts[0];
    } else {
        recv_init_tables(old_part, comm);
    }
    new_part = table_t{old_part.rows(), old_part.cols(), old_part.dx(), old_part.dy()};

    std::vector<table_t> whole_table(comm.size());
    for (size_t i = 0; i < num_iters; ++i) {
        calculate_inside_points(old_part, new_part, alpha, dt);

        std::vector<double> others_points = send_recv_others_points(old_part, comm);

        calculate_bound_points(old_part, new_part, others_points, alpha, dt, comm);

        std::swap(old_part, new_part);

        if (i * dt >= interval) {
            gather_table(old_part, whole_table, comm);

            for (auto &t: whole_table) {
                for (auto &d: t.data()) {
                    std::cout << d << "\n";
                }
            }

            // draw table

        }
    }

    std::atomic_thread_fence(std::memory_order_seq_cst);std::cout<<"**********"<<std::endl;std::atomic_thread_fence(std::memory_order_seq_cst);

    return 0;
}
