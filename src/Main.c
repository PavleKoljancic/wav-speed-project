#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wavheader.h"
#include "math.h"
#include "DECENTRALIZED_BUFFER.h"
#include "matrix.h"
#include "sampleBuffer.h"
#include "samplewindow.h"
#include <omp.h>
#include <getopt.h>
void writeToFile(HEADER_WAV header_wav, FILE *result, DECENTRALIZED_BUFFER **dcb, int number_of_segments)
{
    WRITE_WAV_HEADER_TO_FILE(header_wav, result);
    for (int chanel = 0; chanel < header_wav.fmt_chunk.NumChannels; chanel++)
    {
        for (int segment = 0; segment < number_of_segments; segment++)
        {
            int i;
            for (i = 0; i + dcb[chanel][segment].subBufferCapacity < dcb[chanel][segment].writePosition; i += dcb[chanel][segment].subBufferCapacity)
                fwrite(dcb[chanel][segment].buffer[i / dcb[chanel][segment].subBufferCapacity], 1, dcb[chanel][segment].subBufferCapacity, result);

            int remaining = dcb[chanel][segment].writePosition % dcb[chanel][segment].subBufferCapacity;
            fwrite(dcb[chanel][segment].buffer[i / dcb[chanel][segment].subBufferCapacity], 1, remaining, result);
        }
    }
}

DECENTRALIZED_BUFFER **PROCESS_WAV_FILE(HEADER_WAV header_wav, int sample_buffer_size, const char *infile, int interpolation_points, double *ADJUSTMENT_FACTORS, double **SOLUTION_MATRIX, double speed_up, int num_of_threads, void (*POLYNOMIAL_WRITE_FN_PTR)(POLYNOMIAL *, double **, double *, double *))
{

    double NORMALIZATION_FACTOR = pow(2, header_wav.fmt_chunk.BitsPerSample - 1) - 1;
    int SampleSizeInBytes = (header_wav.fmt_chunk.BitsPerSample / 8);
    uint32_t SamplesPerChanel =
        ((header_wav.data_chunk.Subchunk2Size / header_wav.fmt_chunk.NumChannels) / header_wav.fmt_chunk.BitsPerSample) * 8; // num of bytes

    DECENTRALIZED_BUFFER **dcb = calloc(header_wav.fmt_chunk.NumChannels, sizeof(DECENTRALIZED_BUFFER *));

    for (int chanel = 0; chanel < header_wav.fmt_chunk.NumChannels; chanel++)
    {

        dcb[chanel] = calloc(num_of_threads, sizeof(DECENTRALIZED_BUFFER));

// Ovo bi trebalo biti unikatno u zavisnosti od threada
#pragma omp parallel num_threads(num_of_threads)
        {
            // Allocate memory space
            dcb[chanel][omp_get_thread_num()] = CREATE_DB(1024 * 1024 / num_of_threads, 1024 * 1024 * SampleSizeInBytes);
            SAMPLE_BUFFER sampleBuffer = CREATE_SAMPLE_BUFFER(SampleSizeInBytes, sample_buffer_size);

            // Shift file to appropriate sample
            FILE *wavfile = fopen(infile, "rb");
            if (omp_get_thread_num() > 0)

                fseek(wavfile, HEADER_SIZE + (SamplesPerChanel * SampleSizeInBytes) * chanel + ((SamplesPerChanel * SampleSizeInBytes) / num_of_threads) * omp_get_thread_num() - interpolation_points * SampleSizeInBytes, SEEK_SET);
            else
                fseek(wavfile, HEADER_SIZE + (SamplesPerChanel * SampleSizeInBytes) * chanel, SEEK_SET);
            // determine end and start sample
            double start = (SamplesPerChanel / num_of_threads) * omp_get_thread_num();
            double end;
            if (omp_get_thread_num() == num_of_threads - 1)
                end = SamplesPerChanel;
            else
                end = (SamplesPerChanel / num_of_threads) * (omp_get_thread_num() + 1);
            int lastSampleRead;
            if (start > 0)
                lastSampleRead = (int)start - interpolation_points;
            else
                lastSampleRead = 0;
            //
            if (SamplesPerChanel - lastSampleRead >= sampleBuffer.sampleCapacity)
                WRITE_TO_SAMPLE_BUFFER(&sampleBuffer, sampleBuffer.sampleCapacity, wavfile);
            else
                WRITE_TO_SAMPLE_BUFFER(&sampleBuffer, SamplesPerChanel - lastSampleRead, wavfile);

            lastSampleRead += sampleBuffer.numOfSamples;
            //
            //

            double *samples = calloc(interpolation_points, sizeof(double));
            for (int i = 0; i < interpolation_points; i++)
                samples[i] = GET_SAMPLE(&sampleBuffer, NORMALIZATION_FACTOR);
            SAMPLE_WINDOW sampleWindow;
            if (start > 0)
                sampleWindow = CREATE_SAMPLE_WINDOW(interpolation_points, samples, start - interpolation_points);
            else
                sampleWindow = CREATE_SAMPLE_WINDOW(interpolation_points, samples, start);
            double *Y = calloc(interpolation_points, sizeof(double));
            memcpy(Y, sampleWindow.samples, sampleWindow.windowSize * sizeof(double));
            POLYNOMIAL INTERPOLATION_POLYNOMIAL = CREATE_POLYNOMIAL(interpolation_points);
            POLYNOMIAL_WRITE_FN_PTR(&INTERPOLATION_POLYNOMIAL, SOLUTION_MATRIX, ADJUSTMENT_FACTORS, Y);
            //

            char *sampleData = calloc(SampleSizeInBytes, sizeof(char));
            for (double x = start; x < end; x += speed_up)
            {
                int windowShifted = 0;

                while (GET_SHIFT(sampleWindow, x, SamplesPerChanel) > 0)
                {
                    if (!SAMPLE_BUFFER_CAN_READ(sampleBuffer))
                    {
                        if (SamplesPerChanel - lastSampleRead >= sampleBuffer.sampleCapacity)
                            WRITE_TO_SAMPLE_BUFFER(&sampleBuffer, sampleBuffer.sampleCapacity, wavfile);
                        else
                            WRITE_TO_SAMPLE_BUFFER(&sampleBuffer, SamplesPerChanel - lastSampleRead, wavfile);
                        lastSampleRead += sampleBuffer.numOfSamples;
                    }

                    ADD_SAMPLE(&sampleWindow, GET_SAMPLE(&sampleBuffer, NORMALIZATION_FACTOR));
                    windowShifted = 1;
                }
                if (windowShifted)
                {
                    GET_SAMPLES(Y, sampleWindow);
                    POLYNOMIAL_WRITE_FN_PTR(&INTERPOLATION_POLYNOMIAL, SOLUTION_MATRIX, ADJUSTMENT_FACTORS, Y);
                }
                CONVERT_DOUBLE_SAMPLE_TO_BYTES(sampleData, header_wav.fmt_chunk.BitsPerSample, VALUE_AT(INTERPOLATION_POLYNOMIAL, x - sampleWindow.lower) * NORMALIZATION_FACTOR);
                WRITE_TO_DCB(&dcb[chanel][omp_get_thread_num()], sampleData, SampleSizeInBytes);
            }
            free(sampleData);
            FREE_SAMPLE_BUFFER(sampleBuffer);
            FREE_POLYNOMIAL(INTERPOLATION_POLYNOMIAL);
            fclose(wavfile);
            // Kraj nezavisnosti od threada
        }
    }
    return dcb;
}
void PARSE_CMD_LINE_ARGS(int argc, char *argv[], const char **inputFilePath, const char **outputFilePath,
                         double *speedUp, int *sampleBufferSize, int *numberOfThreads, void (**POLYNOMIAL_WRITE_FN_PTR)(POLYNOMIAL *, double **, double *, double *))
{   
    (*sampleBufferSize) = 1024*1024*100;//100MB read Buffer default value
    (*numberOfThreads) = omp_get_num_procs();// default value
    (*POLYNOMIAL_WRITE_FN_PTR) = WRITE_INTO_POLYNOMIAL;//defualt
    int opt;
    while ((opt = getopt(argc, argv, "i:o:s:b:t:x:")) != -1)
    {
        switch (opt)
        {
        case 'i':
            *inputFilePath = optarg;
            break;
        case 'o':
            *outputFilePath = optarg;
            break;
        case 's':
            *speedUp = atof(optarg);
            break;
        case 'b':
            *sampleBufferSize = atoi(optarg) * 1024 * 1024;
            break;
        case 't':
            *numberOfThreads = atoi(optarg);
            break;
        case 'x':
            
            if (atoi(optarg) == 1)
            {
                *POLYNOMIAL_WRITE_FN_PTR = WRITE_INTO_POLYNOMIAL_SSE;
            }
            
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    const char *inputFilePath = NULL;
    const char *outputFilePath = NULL;
    double speedUp = 0;
    int sample_buffer_size = 0;
    int number_of_threads = 0;
    void (*POLYNOMIAL_WRITE_FN_PTR)(POLYNOMIAL *, double **, double *, double *) =NULL;
    PARSE_CMD_LINE_ARGS(argc, argv, &inputFilePath, &outputFilePath, &speedUp, &sample_buffer_size, &number_of_threads, &POLYNOMIAL_WRITE_FN_PTR);
    if(speedUp<=0||inputFilePath==NULL||outputFilePath==NULL)
    {
        printf("The arguments for the speedUp, input file and out put file must be provided");
        return -1;
    }
    FILE *wav;
    wav = fopen(inputFilePath, "rb");
    HEADER_WAV header_wav = READ_WAV_HEADER(wav);
    fclose(wav);

    if (header_wav.data_chunk.Subchunk2Size < sample_buffer_size)
        sample_buffer_size = header_wav.data_chunk.Subchunk2Size;
    int InterPolationPoints = 4;
    double **SAMPLE_MATRIX = GENERATE_SAMPLE_MATRIX(InterPolationPoints);
    double *ADJUSTMENT_FACTORS = calloc(InterPolationPoints, sizeof(double));
    double **SOLUTION_MATRIX = FIND_GENERAL_SOLUTION(SAMPLE_MATRIX, InterPolationPoints, &ADJUSTMENT_FACTORS);
    // dobra podrazumjevana vrijednost

    for (int i = 0; i < InterPolationPoints; i++)
        free(SAMPLE_MATRIX[i]);
    free(SAMPLE_MATRIX);
    DECENTRALIZED_BUFFER **dcb = PROCESS_WAV_FILE(header_wav, sample_buffer_size, inputFilePath, InterPolationPoints, ADJUSTMENT_FACTORS, SOLUTION_MATRIX, speedUp, number_of_threads, POLYNOMIAL_WRITE_FN_PTR);
    FILE *res = fopen(outputFilePath, "wb");
    header_wav.data_chunk.Subchunk2Size = dcb[0][0].writePosition * header_wav.fmt_chunk.NumChannels * number_of_threads;

    free(ADJUSTMENT_FACTORS);
    for (int i = 0; i < 2 * (InterPolationPoints - 1) + 1; i++)
        free(SOLUTION_MATRIX[i]);
    free(SOLUTION_MATRIX);
    writeToFile(header_wav, res, dcb, number_of_threads);
    for (int i = 0; i < header_wav.fmt_chunk.NumChannels; i++)
        for (int j = 0; j < number_of_threads; j++)
            FREE_DB(dcb[i][j]);
    fclose(res);
}
