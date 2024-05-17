#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "sampleBuffer.h"

SAMPLE_BUFFER CREATE_SAMPLE_BUFFER(int SampleSizeInBytes, int bufferSize)
{
    SAMPLE_BUFFER sb;
    sb.sampleSizeInBytes = SampleSizeInBytes;
    sb.sampleCapacity = bufferSize / sb.sampleSizeInBytes;
    sb.sub_buffer_sample_capacity = 1024 * 1024 / sb.sampleSizeInBytes; // 1MB
    sb.number_of_sub_buffers = sb.sampleCapacity / sb.sub_buffer_sample_capacity;
    if (sb.sampleCapacity % sb.sub_buffer_sample_capacity != 0)
    {
        sb.number_of_sub_buffers++;
        sb.sampleCapacity = sb.number_of_sub_buffers * sb.sub_buffer_sample_capacity;
    }
    sb.main_buffer = calloc(sb.number_of_sub_buffers, sizeof(char *));
    for (int i = 0; i < sb.number_of_sub_buffers; i++)
        sb.main_buffer[i] = calloc(sb.sub_buffer_sample_capacity, sb.sampleSizeInBytes);
    sb.currentSample = sb.numOfSamples = -1;
    return sb;
}
void FREE_SAMPLE_BUFFER(SAMPLE_BUFFER sb)
{

    for (int i = 0; i < sb.number_of_sub_buffers; i++)
        free(sb.main_buffer[i]);
    free(sb.main_buffer);
    sb.main_buffer == NULL;
    sb.currentSample = sb.numOfSamples = -1;
}

int SAMPLE_BUFFER_CAN_READ(SAMPLE_BUFFER sb)
{
    return sb.currentSample >= 0 && sb.currentSample < sb.numOfSamples;
}

void WRITE_TO_SAMPLE_BUFFER(SAMPLE_BUFFER *sb, int samplesToRead, FILE *inputFile)
{
    (*sb).numOfSamples = 0;
    int sub_buffer_to_write_to = samplesToRead / (*sb).sub_buffer_sample_capacity;
    if (samplesToRead < (*sb).sub_buffer_sample_capacity)
        sub_buffer_to_write_to = 1;
    for (int i = 0; i < sub_buffer_to_write_to; i++)
    {
        int num_of_samples_to_read = (*sb).sub_buffer_sample_capacity;
        if (samplesToRead - (*sb).numOfSamples < (*sb).sub_buffer_sample_capacity)
            num_of_samples_to_read =  samplesToRead - (*sb).numOfSamples;
        int readTotal = 0;
        int read = 0;
        while (read != EOF && readTotal < num_of_samples_to_read)
        {

            read = fread(&(*sb).main_buffer[i][read * (*sb).sampleSizeInBytes], (*sb).sampleSizeInBytes, num_of_samples_to_read - readTotal, inputFile);
            if (read != EOF)
                readTotal += read;
        }
        (*sb).numOfSamples += readTotal;
    }
    (*sb).currentSample = 0;
}
double CONVERT_SAMPLE_BYTES_TO_DOUBLE(char *sampleData, int BitsPerSample)
{
    double sample;
    switch (BitsPerSample)
    {
    case 8:
        sample = (*(int8_t *)(sampleData));
        break;
    case 16:
        sample = (*(int16_t *)(sampleData));
        break;
    case 32:
        sample = (*(int32_t *)(sampleData));
        break;

    default:
        sample = 0;
        break;
    }
    return sample;
}

void CONVERT_DOUBLE_SAMPLE_TO_BYTES(char *sampleData, int BitsPerSample, double sample)
{

    switch (BitsPerSample)
    {
    case 8:
        int8_t SampleInt8 = (int8_t)sample;
        memcpy(sampleData, &SampleInt8, sizeof(int8_t));
        break;
    case 16:
        int16_t SampleInt16 = (int16_t)sample;
        memcpy(sampleData, &SampleInt16, sizeof(int16_t));
        break;
    case 32:
        int32_t SampleInt32 = (int32_t)sample;
        memcpy(sampleData, &SampleInt32, sizeof(int32_t));
        break;
    }
}

double GET_SAMPLE(SAMPLE_BUFFER *sb, double normalization_factor)
{
    int main_buffer_pos = (*sb).currentSample / (*sb).sub_buffer_sample_capacity;
    int sub_buffer_pos = (*sb).currentSample % (*sb).sub_buffer_sample_capacity;
    double result = CONVERT_SAMPLE_BYTES_TO_DOUBLE(&(*sb).main_buffer[main_buffer_pos][sub_buffer_pos * (*sb).sampleSizeInBytes], (*sb).sampleSizeInBytes * 8);
    (*sb).currentSample++;
    return result / normalization_factor;
}