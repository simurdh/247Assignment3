//<Define an enum called StatusEnus with the enumerations "INVALID", "VALID" and "CONSUMED">
typedef enum StatusEnum {
  INVALID,
  VALID,
  CONSUMED,
} Status;

//<Define a typedef structure with the enum above and an "int" variable called "data">
typedef struct ShmDataType {
  Status status;
  int data;
} ShmData;

const char* name = "/sharedMemoryObject1 ";
