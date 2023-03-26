#include <stdio.h>
#include <cstdio>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>
#include <nanobind/stl/tuple.h>

#include <wavpack.h>

namespace nb = nanobind;

using namespace nb::literals;

#define CLEAR(destin) memset (&destin, 0, sizeof (destin));

typedef struct {
    uint32_t bytes_written, first_block_size;
    FILE *file;
    int error;
} write_id;


// Ref: https://pyopyopyo.hatenablog.com/entry/2019/02/08/102456
template <typename... Args>
static auto _format(const std::string &fmt, Args... args) {
    size_t len = std::snprintf(nullptr, 0, fmt.c_str(), args...);
    std::vector<char> buf(len + 1);
    std::snprintf(&buf[0], len + 1, fmt.c_str(), args...);
    return std::string(&buf[0], &buf[0] + len);
}

// Copy from cli/utils.c
static int DoWriteFile (FILE *hFile, void *lpBuffer, uint32_t nNumberOfBytesToWrite, uint32_t *lpNumberOfBytesWritten)
{
    uint32_t bcount;

    *lpNumberOfBytesWritten = 0;

    while (nNumberOfBytesToWrite) {
        bcount = (uint32_t) fwrite ((unsigned char *) lpBuffer + *lpNumberOfBytesWritten, 1, nNumberOfBytesToWrite, hFile);

        if (bcount) {
            *lpNumberOfBytesWritten += bcount;
            nNumberOfBytesToWrite -= bcount;
        }
        else
            break;
    }

    return !ferror (hFile);
}

// Copy from cli/utils.c
static int DoTruncateFile (FILE *hFile)
{
    if (hFile) {
        fflush (hFile);
#if defined(_WIN32)
        return !_chsize (_fileno (hFile), 0);
#else
        return !ftruncate(fileno (hFile), 0);
#endif
    }

    return 0;
}

// Copy from cli/utils.c
static int DoCloseHandle (FILE *hFile)
{
    return hFile ? !fclose (hFile) : 0;
}

// Copy from cli/wavpack.c
static int write_block (void *id, void *data, int32_t length)
{
    write_id *wid = (write_id *) id;
    uint32_t bcount;

    if (wid->error)
        return false;

    if (wid && wid->file && data && length) {
        if (!DoWriteFile (wid->file, data, length, &bcount) || bcount != length) {
            DoTruncateFile (wid->file);
            DoCloseHandle (wid->file);
            wid->file = NULL;
            wid->error = 1;
            return false;
        }
        else {
            wid->bytes_written += length;

            if (!wid->first_block_size)
                wid->first_block_size = bcount;
        }
    }

    return true;
}



struct WavpackRead {
  private:
    // FIXME(kamo):
    // To avoid the following error, using void* instead of WavpackDontext.
    // error: invalid use of incomplete type ‘struct WavpackContext’
    // note: forward declaration of ‘struct WavpackContext’
    void *_wpc;
    char _error[80];
    std::string _filename;

  public:
    WavpackRead(const char *infilename, int flags, int norm_offset) {
        // WavpackOpenFileInput
        //     infilename:
        //         The filename can be simply the string '-' to specify stdin as
        //         the source.
        //     flags:
        //         A parameter is a bitmask
        //     error:
        //         If the function fails for some reason (such as the WavPack
        //         file is not found or is invalid) then NULL is returned and
        //         the string pointed to by "error" is set to a message
        //         describing the prolem. Note that the string space is
        //         allocated by the caller and must be at least 80 chars
        //     norm_offset:
        //         floating point audio data to specify an alternate
        //         normalization range. The default is 0 and results
        //         in a standard range of +/-1.0; positive values increase the
        //         range and negative values decrease the range (by factors of
        //         two). For example, a value here of 15 will generate a range
        //         of -32768.0 (assuming no clipping samples).
        _error[0] = '\0';
        _wpc = WavpackOpenFileInput(infilename, _error, flags, norm_offset);
        _filename = infilename;
    };

    auto GetErrorMessage() {
        if (false) {
            // FIXME(kamo): Segmentation fault happnes.
            return WavpackGetErrorMessage(static_cast<WavpackContext *>(_wpc));
        } else {
            return _error;
        };
    };
    auto GetMode() {
        return WavpackGetMode(static_cast<WavpackContext *>(_wpc));
    };
    auto GetNumChannels() {
        return WavpackGetNumChannels(static_cast<WavpackContext *>(_wpc));
    };
    auto GetReducedChannels() {
        return WavpackGetReducedChannels(static_cast<WavpackContext *>(_wpc));
    };
    auto GetChannelMask() {
        return WavpackGetChannelMask(static_cast<WavpackContext *>(_wpc));
    };
    auto GetChannelLayout(unsigned char *reorder) {
        return WavpackGetChannelLayout(static_cast<WavpackContext *>(_wpc),
                                       reorder);
    };
    auto GetChannelIdentities(unsigned char *identities) {
        return WavpackGetChannelIdentities(static_cast<WavpackContext *>(_wpc),
                                           identities);
    };
    auto GetSampleRate() {
        return WavpackGetSampleRate(static_cast<WavpackContext *>(_wpc));
    };
    auto GetNativeSampleRate() {
        return WavpackGetNativeSampleRate(static_cast<WavpackContext *>(_wpc));
    };
    auto GetBitsPerSample() {
        return WavpackGetBitsPerSample(static_cast<WavpackContext *>(_wpc));
    };
    auto GetBytesPerSample() {
        return WavpackGetBytesPerSample(static_cast<WavpackContext *>(_wpc));
    };
    auto GetVersion() {
        return WavpackGetVersion(static_cast<WavpackContext *>(_wpc));
    };
    auto GetFileFormat() {
        return WavpackGetFileFormat(static_cast<WavpackContext *>(_wpc));
    };
    auto GetFileExtention() {
        return WavpackGetFileExtension(static_cast<WavpackContext *>(_wpc));
    };
    auto GetQualifyMode() {
        return WavpackGetQualifyMode(static_cast<WavpackContext *>(_wpc));
    };
    auto GetNumSamples() {
        return WavpackGetNumSamples(static_cast<WavpackContext *>(_wpc));
    };
    auto GetNumSamples64() {
        return WavpackGetNumSamples64(static_cast<WavpackContext *>(_wpc));
    };
    auto GetFileSize() {
        return WavpackGetFileSize(static_cast<WavpackContext *>(_wpc));
    };
    auto GetFileSize64() {
        return WavpackGetFileSize64(static_cast<WavpackContext *>(_wpc));
    };
    auto GetRatio() {
        return WavpackGetRatio(static_cast<WavpackContext *>(_wpc));
    };
    auto GetAverageBitRate(int count_wvc) {
        return WavpackGetAverageBitrate(static_cast<WavpackContext *>(_wpc),
                                        count_wvc);
    };
    auto GetFloatNormExp() {
        return WavpackGetFloatNormExp(static_cast<WavpackContext *>(_wpc));
    };
    auto GetMd5sum(unsigned char data[16]) {
        return WavpackGetMD5Sum(static_cast<WavpackContext *>(_wpc), data);
    };
    auto GetWrapperBytes() {
        return WavpackGetWrapperBytes(static_cast<WavpackContext *>(_wpc));
    };
    auto GetWrapperData() {
        return WavpackGetWrapperData(static_cast<WavpackContext *>(_wpc));
    };
    auto FreeWrapper() {
        return WavpackFreeWrapper(static_cast<WavpackContext *>(_wpc));
    };
    auto SeekTrailingWrapper() {
        return WavpackSeekTrailingWrapper(static_cast<WavpackContext *>(_wpc));
    };
    auto UnpackSamples(int32_t *buffer, uint32_t samples) {
        return WavpackUnpackSamples(static_cast<WavpackContext *>(_wpc), buffer,
                                    samples);
    };
    auto SeekSample64(uint64_t sample) {
        return WavpackSeekSample64(static_cast<WavpackContext *>(_wpc), sample);
    };
    auto SeekSample(uint32_t sample) {
        return WavpackSeekSample64(static_cast<WavpackContext *>(_wpc), sample);
    };
    auto GetSampleIndex64() {
        return WavpackGetSampleIndex64(static_cast<WavpackContext *>(_wpc));
    };
    auto GetSampleIndex() {
        return WavpackGetSampleIndex(static_cast<WavpackContext *>(_wpc));
    };
    auto GetInstantBitrate() {
        return WavpackGetInstantBitrate(static_cast<WavpackContext *>(_wpc));
    };
    auto GetNumErrors() {
        return WavpackGetNumErrors(static_cast<WavpackContext *>(_wpc));
    };
    auto GetLossyBlocks() {
        return WavpackLossyBlocks(static_cast<WavpackContext *>(_wpc));
    };
    auto GetProgress() {
        return WavpackGetProgress(static_cast<WavpackContext *>(_wpc));
    };
    auto CloseFile() {
        WavpackCloseFile(static_cast<WavpackContext *>(_wpc));
        return;
    };
};


struct WavpackWrite {
  private:
  public:
};


auto wavpack_read(const char *infilename, int flags, int norm_offset) {
    auto wpcc = WavpackRead(infilename, flags, norm_offset);
    if (wpcc.GetErrorMessage()[0] != '\0'){
        wpcc.CloseFile();
        throw std::runtime_error(wpcc.GetErrorMessage());
    }

    auto samples = wpcc.GetNumSamples64();
    auto channels = wpcc.GetNumChannels();
    auto bytes_per_sample = wpcc.GetBytesPerSample();

    // The audio data is returned right-justified in 32-bit integers in the
    // endian mode native to the executing processor. So, if the original data
    // was 16-bit in 2-bytes, then the values returned would be +/-32k. Floating
    // point data can also be returned if the source was floating point data
    // (and this can be optionally normalized to +/-1.0 by using the appropriate
    // flag in the call to WavpackOpenFileInput())
    auto *data = new int32_t[samples * channels];

    // "samples" refers to "samples per channel"
    auto ret = wpcc.UnpackSamples(data, samples);
    if (ret != samples * channels) {
        wpcc.CloseFile();
        throw std::runtime_error(
            _format("The returned value from UnpackSamples is %d", ret)
                .c_str());
    }

    size_t shape[2] = {
        static_cast<size_t>(samples),
        static_cast<size_t>(channels),
    };

    // Delete 'data' when the 'owner' capsule expires
    nb::capsule owner(data, [](void *p) noexcept { delete[](int32_t *) p; });

    auto array =
        nb::ndarray<nb::numpy, int32_t>(data, /* ndim = */ 2, shape, owner);
    auto sample_rate = wpcc.GetNativeSampleRate();
    wpcc.CloseFile();
    return std::tuple(array, sample_rate);
};


NB_MODULE(pywavpack_ext, m) {
    m.def("read", &wavpack_read, "infilename"_a, "flags"_a = OPEN_WVC,
          "norm_offset"_a = 0);
    m.def("GetLibraryVersion", &WavpackGetLibraryVersion);
    m.def("GetLibraryVersionString", &WavpackGetLibraryVersionString);

    nb::class_<WavpackRead>(m, "WavpackRead")
        .def(nb::init<const char *, int, int>(), "infilename"_a,
             "flags"_a = OPEN_WVC, "norm_offset"_a = 0)
        .def(
            "__enter__",
            [](WavpackRead &self) -> WavpackRead & {
                return self;
            },
            nb::rv_policy::reference)
        .def(
            "__exit__",
            [](WavpackRead &self, nb::object &, nb::object &,
               nb::object &) {
                // [](WavpackRead& self, void*, void*, void*) {
                self.CloseFile();
                return;
            },
            "exc_type"_a.none(), "exc_value"_a.none(), "traceback"_a.none())
        .def("GetMode", &WavpackRead::GetMode)
        .def("GetErrorMessage", &WavpackRead::GetErrorMessage)
        .def("GetMode", &WavpackRead::GetMode)
        .def("GetNumChannels", &WavpackRead::GetNumChannels)
        .def("GetReducedChannels", &WavpackRead::GetReducedChannels)
        .def("GetChannelMask", &WavpackRead::GetChannelMask)
        .def("GetChannelLayout", &WavpackRead::GetChannelLayout,
             "reorder"_a)
        .def("GetChannelIdentities", &WavpackRead::GetChannelIdentities,
             "identities"_a)
        .def("GetSampleRate", &WavpackRead::GetSampleRate)
        .def("GetNativeSampleRate", &WavpackRead::GetNativeSampleRate)
        .def("GetBitsPerSample", &WavpackRead::GetBitsPerSample)
        .def("GetBytesPerSample", &WavpackRead::GetBytesPerSample)
        .def("GetVersion", &WavpackRead::GetVersion)
        .def("GetFileFormat", &WavpackRead::GetFileFormat)
        .def("GetFileExtention", &WavpackRead::GetFileExtention)
        .def("GetQualifyMode", &WavpackRead::GetQualifyMode)
        .def("GetNumSamples", &WavpackRead::GetNumSamples)
        .def("GetNumSamples64", &WavpackRead::GetNumSamples64)
        .def("GetFileSize", &WavpackRead::GetFileSize)
        .def("GetFileSize64", &WavpackRead::GetFileSize64)
        .def("GetRatio", &WavpackRead::GetRatio)
        .def("GetAverageBitRate", &WavpackRead::GetAverageBitRate,
             "count_wvc"_a)
        .def("GetFloatNormExp", &WavpackRead::GetFloatNormExp)
        .def("GetMd5sum", &WavpackRead::GetMd5sum, "data"_a)
        .def("GetWrapperBytes", &WavpackRead::GetWrapperBytes)
        .def("GetWrapperData", &WavpackRead::GetWrapperData)
        .def("FreeWrapper", &WavpackRead::FreeWrapper)
        .def("SeekTrailingWrapper", &WavpackRead::SeekTrailingWrapper)
        .def("UnpackSamples", &WavpackRead::UnpackSamples, "samples"_a,
             "buffer"_a)
        .def("SeekSample64", &WavpackRead::SeekSample64, "sample"_a)
        .def("SeekSample", &WavpackRead::SeekSample, "sample"_a)
        .def("GetSampleIndex64", &WavpackRead::GetSampleIndex64)
        .def("GetSampleIndex", &WavpackRead::GetSampleIndex)
        .def("GetInstantBitrate", &WavpackRead::GetInstantBitrate)
        .def("GetNumErrors", &WavpackRead::GetNumErrors)
        .def("GetLossyBlocks", &WavpackRead::GetLossyBlocks)
        .def("GetProgress", &WavpackRead::GetProgress)
        .def("CloseFile", &WavpackRead::CloseFile);

    m.attr("ChunkHeaderFormat") = ChunkHeaderFormat;
    m.attr("WaveHeaderFormat") = WaveHeaderFormat;
    m.attr("WavpackHeaderFormat") = WavpackHeaderFormat;
    m.attr("BYTES_STORED") = BYTES_STORED;
    m.attr("MONO_FLAG") = MONO_FLAG;
    m.attr("HYBRID_FLAG") = HYBRID_FLAG;
    m.attr("JOINT_STEREO") = JOINT_STEREO;
    m.attr("CROSS_DECORR") = CROSS_DECORR;
    m.attr("HYBRID_SHAPE") = HYBRID_SHAPE;
    m.attr("FLOAT_DATA") = FLOAT_DATA;
    m.attr("INT32_DATA") = INT32_DATA;
    m.attr("HYBRID_BITRATE") = HYBRID_BITRATE;
    m.attr("HYBRID_BALANCE") = HYBRID_BALANCE;
    m.attr("INITIAL_BLOCK") = INITIAL_BLOCK;
    m.attr("FINAL_BLOCK") = FINAL_BLOCK;
    m.attr("SHIFT_LSB") = SHIFT_LSB;
    m.attr("SHIFT_MASK") = SHIFT_MASK;
    m.attr("MAG_LSB") = MAG_LSB;
    m.attr("MAG_MASK") = MAG_MASK;
    m.attr("SRATE_LSB") = SRATE_LSB;
    m.attr("SRATE_MASK") = SRATE_MASK;
    m.attr("FALSE_STEREO") = FALSE_STEREO;
    m.attr("NEW_SHAPING") = NEW_SHAPING;
    m.attr("MONO_DATA") = MONO_DATA;
    m.attr("HAS_CHECKSUM") = HAS_CHECKSUM;
    m.attr("DSD_FLAG") = DSD_FLAG;
    m.attr("IGNORED_FLAGS") = IGNORED_FLAGS;
    m.attr("UNKNOWN_FLAGS") = UNKNOWN_FLAGS;
    m.attr("MIN_STREAM_VERS") = MIN_STREAM_VERS;
    m.attr("MAX_STREAM_VERS") = MAX_STREAM_VERS;
    m.attr("WAVPACK_MAX_CHANS") = WAVPACK_MAX_CHANS;
    m.attr("WAVPACK_MAX_CLI_CHANS") = WAVPACK_MAX_CLI_CHANS;
    m.attr("ID_UNIQUE") = ID_UNIQUE;
    m.attr("ID_OPTIONAL_DATA") = ID_OPTIONAL_DATA;
    m.attr("ID_ODD_SIZE") = ID_ODD_SIZE;
    m.attr("ID_LARGE") = ID_LARGE;
    m.attr("ID_DUMMY") = ID_DUMMY;
    m.attr("ID_ENCODER_INFO") = ID_ENCODER_INFO;
    m.attr("ID_DECORR_TERMS") = ID_DECORR_TERMS;
    m.attr("ID_DECORR_WEIGHTS") = ID_DECORR_WEIGHTS;
    m.attr("ID_DECORR_SAMPLES") = ID_DECORR_SAMPLES;
    m.attr("ID_ENTROPY_VARS") = ID_ENTROPY_VARS;
    m.attr("ID_HYBRID_PROFILE") = ID_HYBRID_PROFILE;
    m.attr("ID_SHAPING_WEIGHTS") = ID_SHAPING_WEIGHTS;
    m.attr("ID_FLOAT_INFO") = ID_FLOAT_INFO;
    m.attr("ID_INT32_INFO") = ID_INT32_INFO;
    m.attr("ID_WV_BITSTREAM") = ID_WV_BITSTREAM;
    m.attr("ID_WVC_BITSTREAM") = ID_WVC_BITSTREAM;
    m.attr("ID_WVX_BITSTREAM") = ID_WVX_BITSTREAM;
    m.attr("ID_CHANNEL_INFO") = ID_CHANNEL_INFO;
    m.attr("ID_DSD_BLOCK") = ID_DSD_BLOCK;
    m.attr("ID_RIFF_HEADER") = ID_RIFF_HEADER;
    m.attr("ID_RIFF_TRAILER") = ID_RIFF_TRAILER;
    m.attr("ID_ALT_HEADER") = ID_ALT_HEADER;
    m.attr("ID_ALT_TRAILER") = ID_ALT_TRAILER;
    m.attr("ID_CONFIG_BLOCK") = ID_CONFIG_BLOCK;
    m.attr("ID_MD5_CHECKSUM") = ID_MD5_CHECKSUM;
    m.attr("ID_SAMPLE_RATE") = ID_SAMPLE_RATE;
    m.attr("ID_ALT_EXTENSION") = ID_ALT_EXTENSION;
    m.attr("ID_ALT_MD5_CHECKSUM") = ID_ALT_MD5_CHECKSUM;
    m.attr("ID_NEW_CONFIG_BLOCK") = ID_NEW_CONFIG_BLOCK;
    m.attr("ID_CHANNEL_IDENTITIES") = ID_CHANNEL_IDENTITIES;
    m.attr("ID_BLOCK_CHECKSUM") = ID_BLOCK_CHECKSUM;
    m.attr("CONFIG_HYBRID_FLAG") = CONFIG_HYBRID_FLAG;
    m.attr("CONFIG_JOINT_STEREO") = CONFIG_JOINT_STEREO;
    m.attr("CONFIG_CROSS_DECORR") = CONFIG_CROSS_DECORR;
    m.attr("CONFIG_HYBRID_SHAPE") = CONFIG_HYBRID_SHAPE;
    m.attr("CONFIG_FAST_FLAG") = CONFIG_FAST_FLAG;
    m.attr("CONFIG_HIGH_FLAG") = CONFIG_HIGH_FLAG;
    m.attr("CONFIG_VERY_HIGH_FLAG") = CONFIG_VERY_HIGH_FLAG;
    m.attr("CONFIG_BITRATE_KBPS") = CONFIG_BITRATE_KBPS;
    m.attr("CONFIG_SHAPE_OVERRIDE") = CONFIG_SHAPE_OVERRIDE;
    m.attr("CONFIG_JOINT_OVERRIDE") = CONFIG_JOINT_OVERRIDE;
    m.attr("CONFIG_DYNAMIC_SHAPING") = CONFIG_DYNAMIC_SHAPING;
    m.attr("CONFIG_CREATE_EXE") = CONFIG_CREATE_EXE;
    m.attr("CONFIG_CREATE_WVC") = CONFIG_CREATE_WVC;
    m.attr("CONFIG_OPTIMIZE_WVC") = CONFIG_OPTIMIZE_WVC;
    m.attr("CONFIG_COMPATIBLE_WRITE") = CONFIG_COMPATIBLE_WRITE;
    m.attr("CONFIG_CALC_NOISE") = CONFIG_CALC_NOISE;
    m.attr("CONFIG_EXTRA_MODE") = CONFIG_EXTRA_MODE;
    m.attr("CONFIG_SKIP_WVX") = CONFIG_SKIP_WVX;
    m.attr("CONFIG_MD5_CHECKSUM") = CONFIG_MD5_CHECKSUM;
    m.attr("CONFIG_MERGE_BLOCKS") = CONFIG_MERGE_BLOCKS;
    m.attr("CONFIG_PAIR_UNDEF_CHANS") = CONFIG_PAIR_UNDEF_CHANS;
    m.attr("CONFIG_OPTIMIZE_MONO") = CONFIG_OPTIMIZE_MONO;
    m.attr("QMODE_BIG_ENDIAN") = QMODE_BIG_ENDIAN;
    m.attr("QMODE_SIGNED_BYTES") = QMODE_SIGNED_BYTES;
    m.attr("QMODE_UNSIGNED_WORDS") = QMODE_UNSIGNED_WORDS;
    m.attr("QMODE_REORDERED_CHANS") = QMODE_REORDERED_CHANS;
    m.attr("QMODE_DSD_LSB_FIRST") = QMODE_DSD_LSB_FIRST;
    m.attr("QMODE_DSD_MSB_FIRST") = QMODE_DSD_MSB_FIRST;
    m.attr("QMODE_DSD_IN_BLOCKS") = QMODE_DSD_IN_BLOCKS;
    m.attr("QMODE_DSD_AUDIO") = QMODE_DSD_AUDIO;
    m.attr("QMODE_ADOBE_MODE") = QMODE_ADOBE_MODE;
    m.attr("QMODE_NO_STORE_WRAPPER") = QMODE_NO_STORE_WRAPPER;
    m.attr("QMODE_CHANS_UNASSIGNED") = QMODE_CHANS_UNASSIGNED;
    m.attr("QMODE_IGNORE_LENGTH") = QMODE_IGNORE_LENGTH;
    m.attr("QMODE_RAW_PCM") = QMODE_RAW_PCM;
    m.attr("QMODE_EVEN_BYTE_DEPTH") = QMODE_EVEN_BYTE_DEPTH;
    m.attr("MAX_WAVPACK_SAMPLES") = MAX_WAVPACK_SAMPLES;
    m.attr("OPEN_WVC") = OPEN_WVC;
    m.attr("OPEN_TAGS") = OPEN_TAGS;
    m.attr("OPEN_WRAPPER") = OPEN_WRAPPER;
    m.attr("OPEN_2CH_MAX") = OPEN_2CH_MAX;
    m.attr("OPEN_NORMALIZE") = OPEN_NORMALIZE;
    m.attr("OPEN_STREAMING") = OPEN_STREAMING;
    m.attr("OPEN_EDIT_TAGS") = OPEN_EDIT_TAGS;
    m.attr("OPEN_FILE_UTF8") = OPEN_FILE_UTF8;
    m.attr("OPEN_DSD_NATIVE") = OPEN_DSD_NATIVE;
    m.attr("OPEN_DSD_AS_PCM") = OPEN_DSD_AS_PCM;
    m.attr("OPEN_ALT_TYPES") = OPEN_ALT_TYPES;
    m.attr("OPEN_NO_CHECKSUM") = OPEN_NO_CHECKSUM;
    m.attr("MODE_WVC") = MODE_WVC;
    m.attr("MODE_LOSSLESS") = MODE_LOSSLESS;
    m.attr("MODE_HYBRID") = MODE_HYBRID;
    m.attr("MODE_FLOAT") = MODE_FLOAT;
    m.attr("MODE_VALID_TAG") = MODE_VALID_TAG;
    m.attr("MODE_HIGH") = MODE_HIGH;
    m.attr("MODE_FAST") = MODE_FAST;
    m.attr("MODE_EXTRA") = MODE_EXTRA;
    m.attr("MODE_APETAG") = MODE_APETAG;
    m.attr("MODE_SFX") = MODE_SFX;
    m.attr("MODE_VERY_HIGH") = MODE_VERY_HIGH;
    m.attr("MODE_MD5") = MODE_MD5;
    m.attr("MODE_XMODE") = MODE_XMODE;
    m.attr("MODE_DNS") = MODE_DNS;
    m.attr("WP_FORMAT_WAV") = WP_FORMAT_WAV;
    m.attr("WP_FORMAT_W64") = WP_FORMAT_W64;
    m.attr("WP_FORMAT_CAF") = WP_FORMAT_CAF;
    m.attr("WP_FORMAT_DFF") = WP_FORMAT_DFF;
    m.attr("WP_FORMAT_DSF") = WP_FORMAT_DSF;
    m.attr("WP_FORMAT_AIF") = WP_FORMAT_AIF;
};
