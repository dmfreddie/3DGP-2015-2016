/**
 * tcf.hpp
 *
 * Tyrone's Chunk File API.
 * An API to read and write to a chunk file format which is very much
 * inspired by the 3DS chunk file format.
 *
 * Copyright (c) 2011 Tyrone Davison.
 */

#pragma once
#ifndef __TCF__
#define __TCF__

#define TCF_MAJOR_VERSION 1
#define TCF_MINOR_VERSION 1

#include <fstream>
#include <vector>

namespace tcf {

    /**
     * Provides a type enumerate via a four character code.
     */
    class Type {
    public:
        Type(const char name[4]) {
            str[0] = name[0];
            str[1] = name[1];
            str[2] = name[2];
            str[3] = name[3];
        }

        Type() {
            hash = 0;
        }

        bool operator==(Type rhs) {
            return hash == rhs.hash;
        }

        bool operator!=(Type rhs) {
            return hash != rhs.hash;
        }

    private:
        union {
            unsigned int hash;
            char str[4];
        };
    };

    /** Error codes for the TCF API. */
    enum Error {
        kNoError      = 0, /* No error has occured. */
        kFileError    = 1, /* A file read/write error occured. */
        kLogicError   = 2  /* A command called shouldn't have been. */
    };

    /**
     * Provides methods to read a chunk file.
     */
    class Reader {
    public:
        Reader();
        ~Reader();

        /**
         * Open an existing chunk file for input.
         *
         * This command is not valid if a file is already open.
         * The requested file must exist and must be a chunk file.
         */
        void openFile(const char *path);

        /**
         * Read a buffer of data from the input file.
         *
         * The destination buffer must be large enough to receive the number
         * of bytes of data requested.
         * The return value is the number of bytes written to the destination
         * buffer.
         * This command will not read more data than is available in the chunk.
         * This command is only valid if a chunk is open for reading and
         * there is data remaining to be read.
         */
        unsigned int readData(unsigned int numberOfBytes, void *dst);

        /**
         * Get the number of bytes of data remaining in the current chunk.
         *
         * This command is only valid if a chunk is open for reading.
         */
        unsigned int remainingData();

        /**
         * Skip reading of the remaining chunk data.
         *
         * This command is only valid if a chunk is open for reading and
         * there is data remaining to be read.
         */
        void skipData();

        /**
         * Determine if another chunk is available for reading.
         *
         * This command is only valid if a file is already open.
         * This command is not valid until any chunk data is read.
         */
        bool hasChunk();

        /**
         * Skip reading onto the next peer chunk.
         *
         * This command is only valid when hasChunk returns true.
         */
        void skipChunk();

        /**
         * Open the next chunk available.
         *
         * This command is only valid when hasChunk returns true.
         */
        void openChunk();

        /**
         * Get the type of the current open chunk.
         *
         * This command is only valid if a chunk is open for reading.
         */
        Type chunkType();

        /**
         * Close the current chunk being read.
         *
         * This command is only valid if a chunk is open for reading.
         * Any data and sub-chunks not already read will be skipped.
         */
        void closeChunk();

        /**
         * Terminate reading of the current input file.
         *
         * This command is only valid if a file is already open.
         */
        void closeFile();

        /**
         * Get the last error to have occurred and clear the error flag.
         */
        Error lastError();

    private:
        std::ifstream file_;
        Error lastError_;
        struct ChunkData {
            Type type;
            unsigned int remainingChunk; // bytes to skip to reach next chunk
            unsigned int remainingData; // bytes left to read in chunk
        };
        std::vector<ChunkData> chunkStack_;
        enum {
            kBadState, // file not open
            kDataState, // data being read
            kChunkState // chunks being read
        } state_;
    };

    /**
     * Provides methods to write data to a new chunk file.
     */
    class Writer {
    public:
        Writer();
        ~Writer();

        /**
         * Create a new output chunk file.
         *
         * This command is not valid if a file is already open.
         */
        void createFile(const char *path);

        /**
         * Create and begin writing a new chunk in the open file.
         *
         * This command is only valid if a file is open for output.
         * If a chunk is currently being written, calling this command
         * ends the data section of the current chunk and creates a sub-chunk.
         * If the first chunk created isn't of type HEAD, one of this type
         * will automatically be created and inserted.
         */
        void createChunk(Type);

        /**
         * Write a buffer of data to the file.
         *
         * This command is only valid if a chunk is currently being written,
         * and no sub-chunks have been created yet.
         */
        void writeData(const void *data, unsigned int numberOfBytes);

        /**
         * Declare the end of the current chunk being written.
         *
         * This command is only valid if a chunk is being written.
         */
        void endChunk();

        /**
         * Declare the end of the output file.
         *
         * This command is only valid if a file is open for output.
         * Any chunks currently being written are ended automatically.
         * A HEAD chunk is automatically added if one hasn't been already.
         */
        void endFile();

        /**
         * Get the last error to have occurred and clear the error flag.
         */
        Error lastError();

    private:
        void startChunk(Type);
        void completeChunk();

        std::ofstream file_;
        Error lastError_;
        struct ChunkData {
            unsigned int position;
            unsigned int totalSize; // bytes to skip after the header
            unsigned int dataSize; // bytes of payload data
        };
        std::vector<ChunkData> chunkStack_;
        enum {
            kBadState, // file not open
            kDataState, // data being written
            kChunkState // chunks being written
        } state_;
        bool headWritten_;
    };

}

#endif
