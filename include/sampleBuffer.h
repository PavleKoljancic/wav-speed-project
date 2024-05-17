typedef struct __rbuff
{
    
    char **main_buffer;
    char *sub_buffer;
    int sub_buffer_sample_capacity;
    int number_of_sub_buffers;
    int sampleCapacity;
    int currentSample;
    int numOfSamples;
    int sampleSizeInBytes;
} SAMPLE_BUFFER;

SAMPLE_BUFFER CREATE_SAMPLE_BUFFER(int SampleSizeInBytes, int bufferSize);

void CONVERT_DOUBLE_SAMPLE_TO_BYTES(char *sampleData, int BitsPerSample, double sample);

double GET_SAMPLE(SAMPLE_BUFFER* sb,double normalization_factor);

void FREE_SAMPLE_BUFFER(SAMPLE_BUFFER sb);
int SAMPLE_BUFFER_CAN_READ(SAMPLE_BUFFER sb);
void WRITE_TO_SAMPLE_BUFFER(SAMPLE_BUFFER *sb, int samplesToRead,FILE * inputFile);