#include "image_writer.h"

void write_table_to_file(const table_t &table, double min_range, double max_range, const std::string &filepath) {
    TIFF *out= TIFFOpen(filepath.c_str(), "w");

    int sample_per_pixel = 3;

    double_to_rgb_converter_t d2rgb_conv(min_range, max_range);
    std::vector<uint8_t> image = d2rgb_conv.table_to_rgb_array_for_tiff(table);

    TIFFSetField(out, TIFFTAG_IMAGEWIDTH, table.cols());  // set the width of the image
    TIFFSetField(out, TIFFTAG_IMAGELENGTH, table.rows());    // set the height of the image
    TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, sample_per_pixel);   // set number of channels per pixel
    TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);    // set the size of the channels
    TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);    // set the origin of the image.
    //   Some other essential fields to set that you do not have to understand for now.
    TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

    // We set the strip size of the file to be size of one row of pixels
    TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(out, table.cols() * sample_per_pixel));

    //Now writing image to the file one strip at a time
    for (uint32 row = 0; row < table.rows(); row++) {
        if (TIFFWriteScanline(out, &(image[3 * row * table.cols()]), row, 0) < 0) {
            break;
        }
    }

    (void) TIFFClose(out);
}