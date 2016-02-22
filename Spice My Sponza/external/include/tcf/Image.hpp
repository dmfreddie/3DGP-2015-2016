/**
 * Image.hpp
 *
 * A collection of basic images stored via TCF.
 *
 * Copyright (c) 2012 Tyrone Davison.
 */

#pragma once
#ifndef __TCF_IMAGE__
#define __TCF_IMAGE__

#include "tcf.hpp"
#include <vector>
#include <string>
#include <cstdint>

namespace tcf
{

struct Image
{
    unsigned int width{ 0 };
    unsigned int height{ 0 };
    unsigned int components_per_pixel{ 0 };
    unsigned int bytes_per_component{ 0 };
    enum ComponentType { kInteger, kFloatingPoint } component_type{ kInteger };
    std::vector<uint8_t> pixel_bytes;
    std::string name;
};

/**
 * Initialise a collection of images using the contents of a TCF file.
 * The file must be a TCF file and must contain properly formatted Image
 * data.  Use the imagesToFile method to create such a file.
 * @param   filepath    The file from which to read.
 * @param   error       An error variable with which to report problems.
 *                      Use nullptr if error reporting is not required.
 * @return              A collection of Image objects.
 */
std::vector<Image>
imagesFromFile(std::string filepath,
               Error *error);

/**
 * Create (or overwrite) a file in TCF format containing the data from
 * a collection of Image objects.
 * @param   images      A collection of correctly initialised Image objects.
 * @param   filepath    The file to which to write the data.
 * @param   error       An error variable with which to report problems.
 *                      Use nullptr if error reporting is not required.
 */
void
imagesToFile(const std::vector<Image>& images,
             std::string filepath,
             Error *error);

/**
 * Test if the currently open chunk contains data for a tcf::Image.
 * Use this method when reading tcf::Image data that has been embedded
 * within another TCF file type.
 * @param   reader  A tcf::Reader with an open file and open chunk.
 * @return          True if the chunk type matches that of a tcf::Image.
 */
bool
chunkIsImage(Reader& reader);

/**
 * Read the tcf::Image data within the open chunk.
 * It is assumed that the user has verified the chunk contains valid data.
 * The open chunk is closed before the method exits.
 * @param   reader  A tcf::Reader with an open file and open chunk.
 * @return          A tcf::Image object initialised with the chunk contents.
 */
Image
readImage(Reader& reader);

/**
 * Write the given tcf::Image data as a child chunk within the current scope.
 * The user is responsible for ensuring the tcf::Image data is valid.
 * The new chunk is ended before the method exits.
 * @param   image   A valid tcf::Image object.
 * @param   writer  A tcf::Writer with an open file.
 */
void
writeImage(const Image& image,
           Writer& writer);

} // end namespace tcf

#endif
