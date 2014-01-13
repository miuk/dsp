#include <iostream>
using namespace std;
#include "wav.hxx"

int
main(int ac, char* av[])
{
    WAV wav;
    string errmsg;
    if (wav.open(av[1], errmsg) == false) {
        cerr << errmsg << endl;
        exit(1);
    }
    WAVFmt fmt = wav.getFmt();
    int nbyte = wav.getDataBytes();
    cout << "type: " << fmt.type << endl
         << "nch: " << fmt.nch << endl
         << "samples_per_sec: " << fmt.samples_per_sec << endl
         << "bytes_per_sec: " << fmt.bytes_per_sec << endl
         << "block_size: " << fmt.block_size << endl
         << "bits_per_sample: " << fmt.bits_per_sample << endl
         << "data bytes: " << nbyte << endl
    ;
    int nsample = fmt.calcSamples(nbyte);
    cout << "calcSamples: " << nsample << endl;
    cout << "calcBytes: " << fmt.calcBytes(nsample) << endl;
    char* buf = new char[nbyte];
    int len = wav.read(buf, nsample);
    cout << "read bytes: " << len << endl;
    wav.close(errmsg);
}
