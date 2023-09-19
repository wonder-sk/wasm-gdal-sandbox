#include <iostream>
#include <chrono>

#define USE_GDAL

#ifdef USE_GDAL
#include <gdal_priv.h>
#endif

#include <errno.h>
#include <assert.h>

#include <pthread.h>

using namespace std;




class Timer
{
private:
    // Type aliases to make accessing nested type easier
    using Clock = std::chrono::steady_clock;
    using Second = std::chrono::duration<double, std::ratio<1> >;

    std::chrono::time_point<Clock> m_beg { Clock::now() };

public:
    void reset()
    {
        m_beg = Clock::now();
    }

    double elapsed() const
    {
        return std::chrono::duration_cast<Second>(Clock::now() - m_beg).count();
    }
};

#include <emscripten/fetch.h>


//void downloadSucceeded(emscripten_fetch_t *fetch) {
//    printf("Finished downloading %llu bytes from URL %s.\n", fetch->numBytes, fetch->url);
//    // The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];
//    //emscripten_fetch_close(fetch); // Free data associated with the fetch.
//}

//void downloadFailed(emscripten_fetch_t *fetch) {
//    printf("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
//    //emscripten_fetch_close(fetch); // Also free data on failure.
//}


bool getContentLength(emscripten_fetch_t *fetch, uint64_t &contentLength)
{
    size_t headersLengthBytes = emscripten_fetch_get_response_headers_length(fetch) + 1;
    char *headerString = new char[headersLengthBytes];
    emscripten_fetch_get_response_headers(fetch, headerString, headersLengthBytes);
    char **responseHeaders = emscripten_fetch_unpack_response_headers(headerString);
    delete[] headerString;

    bool res = false;
    for(int numHeaders = 0; responseHeaders[numHeaders * 2]; ++numHeaders)
    {
        // Check both the header and its value are present.
        std::string key = responseHeaders[numHeaders * 2];
        if (key == "content-length")
        {
            contentLength = atoi(responseHeaders[(numHeaders * 2) + 1]);
            res = true;
        }
    }

    emscripten_fetch_free_unpacked_response_headers(responseHeaders);
    return res;
}


static void *run(void *arg) {
    size_t job = *(size_t*)arg;

    cout << "RUN!" << endl;

    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "HEAD");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY | EMSCRIPTEN_FETCH_SYNCHRONOUS;

//    strcpy(attr.requestMethod, "GET");
//    const char * xx[] = { "Range", "bytes=0-511" };
//    attr.requestHeaders = xx;

//    attr.onsuccess = downloadSucceeded;
//    attr.onerror = downloadFailed;

    emscripten_fetch_t *fetch = emscripten_fetch(&attr, "http://localhost:8080/64abc23964adbc00012e0e90.tif"); // Blocks here until the operation is complete.

    if ( !fetch )
    {
        printf("uh oh: fetch returned null\n");
        return nullptr;
    }

    // show response headers
    size_t headersLengthBytes = emscripten_fetch_get_response_headers_length(fetch) + 1;
    char *headerString = new char[headersLengthBytes];
    emscripten_fetch_get_response_headers(fetch, headerString, headersLengthBytes);
    printf("response headers:\n%s\n", headerString);
    delete[] headerString;

    uint64_t len = 0;
    getContentLength( fetch, len );

    //    EMSCRIPTEN_RESULT ee = emscripten_fetch_wait(fetch, 10000);
    //    printf("res: %d\n", ee);

    if (fetch->status == 200 || fetch->status == 206) {
        printf("Finished downloading %llu bytes from URL %s / len %lld %lld %lld.\n", fetch->numBytes, fetch->url, fetch->numBytes, fetch->totalBytes, len);
        // The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];
    } else {
        // TODO: when not found, status==0 instead of 404
        printf("Downloading %s failed, HTTP failure status code: %d (%s).\n", fetch->url, fetch->status, fetch->statusText);
    }
    emscripten_fetch_close(fetch);

    return nullptr;
}



int main(int argc, const char* argv[])
{
    cerr << "Hello! (stderr)" << endl;
    cout << "Hello! (stdout)" << endl;

#ifndef USE_GDAL
    run((void*)123);
#endif

    // check for the patch that avoids warning in the console: unsupported syscall: __syscall_prlimit64
    cout << "physical ram " << CPLGetUsablePhysicalRAM() << endl;

#ifdef USE_GDAL
    //const char* pszFilename = "/vsicurl/https://oin-hotosm.s3.us-east-1.amazonaws.com/64abc23964adbc00012e0e8f/0/64abc23964adbc00012e0e90.tif";
    const char* pszFilename = "/vsicurl/http://localhost:8080/64abc23964adbc00012e0e90.tif";

    setenv("GDAL_DISABLE_READDIR_ON_OPEN", "EMPTY_DIR", 1);

    Timer t0;


    GDALDatasetUniquePtr poDataset;
    GDALAllRegister();
    const GDALAccess eAccess = GA_ReadOnly;
    poDataset = GDALDatasetUniquePtr(GDALDataset::FromHandle(GDALOpen( pszFilename, eAccess )));
    if( !poDataset )
    {
        cerr << "failed to open " << pszFilename;
        return 1;
    }

    cout << "open time:" << t0.elapsed() << " seconds" << endl;

#if 1
    double        adfGeoTransform[6];
    printf( "Driver: %s/%s\n",
           poDataset->GetDriver()->GetDescription(),
           poDataset->GetDriver()->GetMetadataItem( GDAL_DMD_LONGNAME ) );
    printf( "Size is %dx%dx%d\n",
           poDataset->GetRasterXSize(), poDataset->GetRasterYSize(),
           poDataset->GetRasterCount() );
    // throws - probably due to PROJ db missing
//    if( poDataset->GetProjectionRef()  != NULL )
//        printf( "Projection is `%s'\n", poDataset->GetProjectionRef() );
    if( poDataset->GetGeoTransform( adfGeoTransform ) == CE_None )
    {
        printf( "Origin = (%.6f,%.6f)\n",
               adfGeoTransform[0], adfGeoTransform[3] );
        printf( "Pixel Size = (%.6f,%.6f)\n",
               adfGeoTransform[1], adfGeoTransform[5] );
    }


    GDALRasterBand  *poBand;
    int             nBlockXSize, nBlockYSize;
    int             bGotMin, bGotMax;
    double          adfMinMax[2];
    poBand = poDataset->GetRasterBand( 1 );
    poBand->GetBlockSize( &nBlockXSize, &nBlockYSize );
    printf( "Block=%dx%d Type=%s, ColorInterp=%s\n",
           nBlockXSize, nBlockYSize,
           GDALGetDataTypeName(poBand->GetRasterDataType()),
           GDALGetColorInterpretationName(
               poBand->GetColorInterpretation()) );
    adfMinMax[0] = poBand->GetMinimum( &bGotMin );
    adfMinMax[1] = poBand->GetMaximum( &bGotMax );
    if( ! (bGotMin && bGotMax) )
        GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);
    printf( "Min=%.3fd, Max=%.3f\n", adfMinMax[0], adfMinMax[1] );
    if( poBand->GetOverviewCount() > 0 )
        printf( "Band has %d overviews.\n", poBand->GetOverviewCount() );
    if( poBand->GetColorTable() != NULL )
        printf( "Band has a color table with %d entries.\n",
               poBand->GetColorTable()->GetColorEntryCount() );
#endif

    {
        Timer t;

        GDALRasterBand *poBand = poDataset->GetRasterBand( 1 );
        int   nXSize = 512;
        int   nYSize = 512;
        float *data = (float *) CPLMalloc(sizeof(float)*nXSize*nYSize);
        CPLErr err = poBand->RasterIO( GF_Read, 4*512, 4*512, nXSize, nYSize,
                         data, nXSize, nYSize, GDT_Float32,
                         0, 0, nullptr );
        assert(err == CE_None);
        //cout << data[0] << " " << data[1];

        cout << "Time reading: " << t.elapsed() << " seconds" << endl;
    }

#endif

    return 0;
}
