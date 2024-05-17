#include "wavheader.h"
#include <string.h>
#include <stdio.h>

RIFF_CHUNK READ_RIFF_CHUNK(FILE *inputFile)
{
    RIFF_CHUNK result;

    fread(result.ChunkID, sizeof(char), 4, inputFile);
    fread(&result.ChunkSize, sizeof(__uint32_t), 1, inputFile);
    fread(result.Format, sizeof(char), 4, inputFile);
    return result;
}


FMT_CHUNK READ_FMT_CHUNK(FILE *inputFile)
{
    FMT_CHUNK ftm;
    fread(ftm.Subchunk1ID, sizeof(char), 4, inputFile);
    fread(&ftm.Subchunk1Size, sizeof(__uint32_t), 1, inputFile);
    fread(&ftm.AudioFormat, sizeof(__uint16_t), 1, inputFile);
    fread(&ftm.NumChannels, sizeof(__uint16_t), 1, inputFile);
    fread(&ftm.SampleRate, sizeof(__uint32_t), 1, inputFile);
    fread(&ftm.ByteRate, sizeof(__uint32_t), 1, inputFile);
    fread(&ftm.BlockAlign, sizeof(__uint16_t), 1, inputFile);
    fread(&ftm.BitsPerSample, sizeof(__uint16_t), 1, inputFile);

    return ftm;
}

DATA_CHUNK READ_DATA_CHUNK(FILE *inputFile)
{
    DATA_CHUNK data_chunk;
    fread(data_chunk.Subchunk2ID, sizeof(char), 4, inputFile);
    fread(&data_chunk.Subchunk2Size, sizeof(__uint32_t), 1, inputFile);
    return data_chunk;
}

HEADER_WAV READ_WAV_HEADER(FILE *inputFile)
{
    HEADER_WAV header_wav;
    header_wav.riff_chunk = READ_RIFF_CHUNK(inputFile);
    header_wav.fmt_chunk = READ_FMT_CHUNK(inputFile);
    header_wav.data_chunk = READ_DATA_CHUNK(inputFile);
    return header_wav;
}

void WRITE_RIFF_CHUNK_TO_FILE(RIFF_CHUNK riff, FILE *inputFile)
{
    // Riff
    fwrite(riff.ChunkID, sizeof(char), 4, inputFile);
    fwrite(&riff.ChunkSize, sizeof(__uint32_t), 1, inputFile);
    fwrite(riff.Format, sizeof(char), 4, inputFile);
}

void WRITE_FMT_CHUNK_TO_FILE(FMT_CHUNK fmt, FILE *inputFile)
{

    // ftm
    fwrite(fmt.Subchunk1ID, sizeof(char), 4, inputFile);
    fwrite(&fmt.Subchunk1Size, sizeof(__uint32_t), 1, inputFile);
    fwrite(&fmt.AudioFormat, sizeof(__uint16_t), 1, inputFile);
    fwrite(&fmt.NumChannels, sizeof(__uint16_t), 1, inputFile);
    fwrite(&fmt.SampleRate, sizeof(__uint32_t), 1, inputFile);
    fwrite(&fmt.ByteRate, sizeof(__uint32_t), 1, inputFile);
    fwrite(&fmt.BlockAlign, sizeof(__uint16_t), 1, inputFile);
    fwrite(&fmt.BitsPerSample, sizeof(__uint16_t), 1, inputFile);
}

void WRITE_DATA_CHUNK_TO_FILE(DATA_CHUNK data_chunk, FILE *inputFile)
{

    // data
    fwrite(data_chunk.Subchunk2ID, sizeof(char), 4, inputFile);
    fwrite(&data_chunk.Subchunk2Size, sizeof(__uint32_t), 1, inputFile);
}

void WRITE_WAV_HEADER_TO_FILE(HEADER_WAV header, FILE *inputFile)
{
    WRITE_RIFF_CHUNK_TO_FILE(header.riff_chunk, inputFile);
    WRITE_FMT_CHUNK_TO_FILE(header.fmt_chunk,inputFile);
    WRITE_DATA_CHUNK_TO_FILE(header.data_chunk,inputFile);
}