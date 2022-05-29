#ifndef TEMPLATE_D2RGB_CONVERTER_H
#define TEMPLATE_D2RGB_CONVERTER_H

#include <cmath>
#include <vector>
#include "table_t.h"

class double_to_rgb_converter_t {
public:
    double_to_rgb_converter_t(double range_min, double range_max):
    range_start(range_min), //! Watch the order!!!!!!!
    range_end(range_max),
    range_median((range_min + range_max) / 2)
    {}

    template <typename OutType>
    void double_to_rgb(double val, OutType &out_red, OutType &out_green, OutType &out_blue) {
        val = (val - range_median) / (range_end - range_start) * 2;
        out_red = std::round((1 + red(val)) / 2 * 255);
        out_green = std::round((1 + green(val)) / 2 * 255);
        out_blue = std::round((1 + blue(val)) / 2 * 255);
    }

    std::vector<uint8_t> table_to_rgb_array_for_tiff(const table_t &table) {
        std::vector<uint8_t> vec(3 * table.rows() * table.cols());
        for (size_t i = 0; i < table.rows(); ++i) {
            for (size_t j = 0; j < table.cols(); ++j) {
                uint8_t r, g, b;
                double_to_rgb(table(i, j), r, g, b);
                vec[3 * (table.cols() * i + j) + 0] = r;
                vec[3 * (table.cols() * i + j) + 1] = g;
                vec[3 * (table.cols() * i + j) + 2] = b;
            }
        }
        return vec;
    }
private:
//Inspired by https://stackoverflow.com/a/7706668
    double interpolate(double val, double y0, double x0, double y1, double x1) {
        return (val - x0) * (y1 - y0) / (x1 - x0) + y0;
    }

    double base( double val ) {
        if (val <= -0.75) return 0;
        else if (val <= -0.25) return interpolate(val, 0.0, -0.75, 1.0, -0.25);
        else if (val <= 0.25) return 1.0;
        else if (val <= 0.75) return interpolate(val, 1.0, 0.25, 0.0, 0.75);
        else return 0.0;
    }

    double red(double gray) {
        return base(gray - 0.5);
    }
    double green(double gray) {
        return base(gray);
    }
    double blue(double gray) {
        return base(gray + 0.5);
    }

private:
    double range_start;
    double range_end;
    double range_median;
};

#endif //TEMPLATE_D2RGB_CONVERTER_H
