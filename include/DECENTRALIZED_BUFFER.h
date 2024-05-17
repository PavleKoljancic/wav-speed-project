#include <stdint.h>

typedef struct dcb
{
    int mainBufferCapacity;
    int subBufferCapacity;
    uint64_t writePosition;
    char **buffer;
    int numOfSubBuffers;

} DECENTRALIZED_BUFFER;

DECENTRALIZED_BUFFER CREATE_DB(int mainBufferCapacity, int subBufferCapacity);
int WRITE_TO_DCB(DECENTRALIZED_BUFFER  *dcb, char* data, int DataSizeInBytes);
void FREE_DB(DECENTRALIZED_BUFFER dcb);