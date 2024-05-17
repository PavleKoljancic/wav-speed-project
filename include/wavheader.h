
#include <stdio.h>
#include <stdint.h>

#define HEADER_SIZE 44

typedef struct riff_Chunk
{

    char ChunkID[4];//"RIFF"
    uint32_t ChunkSize;// fileSize - 8
    char Format[4]; //"WAVE"
} RIFF_CHUNK;

typedef struct _fmt_chunk 
{
    char Subchunk1ID [4];//"ftm "
    uint32_t Subchunk1Size; //"This is the size of the rest of the Subchunk which follows this number"
    uint16_t AudioFormat; // PMC==1
    uint16_t NumChannels; 
    uint32_t SampleRate; // Foreaxmle 44100Hz or
    uint32_t ByteRate;     // (Sample Rate * BitsPerSample * Channels) / 8
    uint16_t BlockAlign; //  == NumChannels * BitsPerSample/8
    uint16_t BitsPerSample; //8,16 itd 
}
FMT_CHUNK;

typedef struct _data_chunk 
{
    char Subchunk2ID [4];//"data"
    uint32_t Subchunk2Size; //== NumSamples * NumChannels * BitsPerSample/8

}
DATA_CHUNK;

typedef struct header_wav
{

    RIFF_CHUNK riff_chunk;
    FMT_CHUNK fmt_chunk;
    DATA_CHUNK data_chunk;

} HEADER_WAV;

RIFF_CHUNK READ_RIFF_CHUNK(FILE *inputFile);
FMT_CHUNK READ_FMT_CHUNK(FILE * inputFile);
DATA_CHUNK READ_DATA_CHUNK(FILE * inputFile);
HEADER_WAV READ_WAV_HEADER(FILE * inputFile);
void WRITE_WAV_HEADER_TO_FILE(HEADER_WAV header, FILE *inputFile);