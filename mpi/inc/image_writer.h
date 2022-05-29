#ifndef TEMPLATE_IMAGE_WRITER_H
#define TEMPLATE_IMAGE_WRITER_H

#include "table_t.h"
#include "d2rgb_converter.h"

#include <tiffio.h>


// Inspired by http://www.libtiff.org/libtiff.html
void write_table_to_file(const table_t &table, double min_range, double max_range, const std::string &filepath);

#endif //TEMPLATE_IMAGE_WRITER_H
