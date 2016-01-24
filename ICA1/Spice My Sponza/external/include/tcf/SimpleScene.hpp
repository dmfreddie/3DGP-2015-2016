/**
 * SimpleScene.hpp
 *
 * A simple scene of multiple geometries stored via TCF.
 *
 * Copyright (c) 2012 Tyrone Davison.
 */

#pragma once
#ifndef __TCF_SIMPLESCENE__
#define __TCF_SIMPLESCENE__

#include "tcf.hpp"
#include "types.hpp"
#include <vector>
#include <string>

namespace tcf
{

struct SimpleScene
{
    enum Topology
    {
        kIndexedTriangleList = 1
    };

    struct Mesh
    {
        Topology topology{ kIndexedTriangleList };
        std::string name;
        std::vector<Vector3> vertexArray;
        std::vector<Vector3> normalArray;
        std::vector<Vector3> tangentArray;
        std::vector<Vector2> texcoordArray;
        std::vector<int> indexArray;
        std::vector<Matrix> instanceArray;
        std::vector<int> materialArray;
    };

    std::vector<Mesh> meshArray;
};

/**
 * Initialise a new SimpleScene object using the contents of a TCF file.
 * The file must be a TCF file and must contain properly formatted SimpleScene
 * data.  Use the simpleSceneToFile method to create such a file.
 * @param   filepath    The file from which to read.
 * @param   error       An error variable with which to report problems.
 *                      Use nullptr if error reporting is not required.
 * @return              The new SimpleScene object.
 */
SimpleScene
simpleSceneFromFile(std::string filepath,
                    Error *error);

/**
 * Create (or overwrite) a file in TCF format containing the data from
 * the given SimpleScene object.
 * @param   scene       A correctly initialised SimpleScene object.
 * @param   filepath    The file to which to write the data.
 * @param   error       An error variable with which to report problems.
 *                      Use nullptr if error reporting is not required.
 */
void
simpleSceneToFile(const SimpleScene& scene,
                  std::string filepath,
                  Error *error);

/**
 * Test if the currently open chunk contains data for a tcf::SimpleScene.
 * Use this method when reading tcf::SimpleScene data that has been embedded
 * within another TCF file type.
 * @param   reader  A tcf::Reader with an open file and open chunk.
 * @return          True if the chunk type matches that of a tcf::SimpleScene.
 */
bool
chunkIsSimpleScene(Reader& reader);

/**
 * Read the tcf::SimpleScene data within the open chunk.
 * It is assumed that the user has verified the chunk contains valid data.
 * The open chunk is closed before the method exits.
 * @param   reader  A tcf::Reader with an open file and open chunk.
 * @return          A tcf::SimpleScene object initialised with the chunk data.
 */
SimpleScene
readSimpleScene(Reader& reader);

/**
 * Write the given tcf::SimpleScene as a child chunk within the current scope.
 * The user is responsible for ensuring the tcf::SimpleScene data is valid.
 * The new chunk is ended before the method exits.
 * @param   scene   A valid tcf::SimpleScene object.
 * @param   writer  A tcf::Writer with an open file.
 */
void
writeSimpleScene(const SimpleScene& scene,
                 Writer& writer);

/** This method is depreciated and may be removed in future versions. */
bool readFile(const char *path, SimpleScene *out);

/** This method is depreciated and may be removed in future versions. */
bool writeFile(const char *path, const SimpleScene *in);

} // end namespace tcf

#endif
