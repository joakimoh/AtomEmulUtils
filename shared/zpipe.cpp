/* zpipe.c: example of proper use of zlib's inflate() and deflate()
   Not copyrighted -- provided to the public domain
   Version 1.4  11 December 2005  Mark Adler */

   /* Version history:
      1.0  30 Oct 2004  First version
      1.1   8 Nov 2004  Add void casting for unused return values
                        Use switch statement for inflate() return values
      1.2   9 Nov 2004  Add assertions to document zlib guarantees
      1.3   6 Apr 2005  Remove incorrect assertion in inf()
      1.4  11 Dec 2005  Add hack to avoid MSDOS end-of-line conversions
                        Avoid some compiler warnings for input and output buffers

   2023-04-30: MODIFIED FOR AtomTapeUtils to provide a C++ interface
   - main() test program removed as well as def() and inf()
   - C++ functions inflate() and deflate() added based on original def() and inf()
    */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "zlib.h"
#include <string>
#include <iostream>
#include <fstream>
#include "zpipe.h"

using namespace std;

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif

#define CHUNK 16384

//
// Modified version of def() that compresses a byte vector
// and writes it to an already open file.

bool encodeBytes(vector<uint8_t> &bytes, ofstream& dest)
{
    int ret, flush;
    unsigned have;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];
    int bp = 0;
    int written_bytes = 0;
    int read_bytes = 0;

    /* allocate deflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
    if (ret != Z_OK)
        return false;

    /* compress until end of file */
    do {
        if (bp < bytes.size() - CHUNK) {
            memcpy((char*)&in[0], &bytes[bp], CHUNK);
            strm.avail_in = CHUNK;
            bp += CHUNK;
            flush = Z_NO_FLUSH;
        }
        else {
            memcpy((char*)&in[0], &bytes[bp], bytes.size() - bp);
            strm.avail_in = bytes.size() - bp;
            bp = bytes.size();
            flush = Z_FINISH;

        }
        strm.next_in = in;

        read_bytes += strm.avail_in;

        /* run deflate() on input until output buffer not full, finish
           compression if all of source has been read in */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = deflate(&strm, flush);    /* no bad return value */
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            have = CHUNK - strm.avail_out;
            dest.write((char*)out, have);
            written_bytes += have;
            if (dest.fail()) {
                (void)deflateEnd(&strm);
                return false;
            }
        } while (strm.avail_out == 0);
        assert(strm.avail_in == 0);     /* all input will be used */

        /* done when last data in file processed */
    } while (flush != Z_FINISH);
    assert(ret == Z_STREAM_END);        /* stream will be complete */

    /* clean up and return */
    (void)deflateEnd(&strm);

    return true;
}

// Modified version of inf() that uncompresses bytes
// from an already open file and writes them
// into a byte vector.
bool decodeBytes(ifstream& source, vector<uint8_t>& bytes)
{
    int ret;
    unsigned have;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];
    unsigned bp = 0;
    int written_bytes = 0;
    int read_bytes = 0;

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit(&strm);
    if (ret != Z_OK)
        return false;

    /* decompress until deflate stream ends or end of file */
    do {
        source.read((char*)in, CHUNK);
        strm.avail_in = source.gcount();
        read_bytes += strm.avail_in;
        if (strm.avail_in == 0)
            break;
        strm.next_in = in;

        /* run inflate() on input until output buffer not full */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     /* and fall through */
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
                return false;
            }
            have = CHUNK - strm.avail_out;
            try {
                if (bytes.capacity() < bp + have)
                    bytes.resize(bp + have);
                memcpy(&bytes[bp], &out[0], have * sizeof(out[0]));
                written_bytes += have;
                bp += have;
            }
            catch (bad_alloc) {
                (void)inflateEnd(&strm);
                return false;
            }
        } while (strm.avail_out == 0);

        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);

    /* clean up and return */
    (void)inflateEnd(&strm);


    return ret == Z_STREAM_END ? true : false;
}