#include "devices.h"

extern uint32_t __state_vector_start;
extern uint32_t __state_vector_end;

StateHandle_t StateHandle_getHandle(char name[STATE_NAME_LENGTH]) {
	StateHandle_t *handleRef = StateHandle_getHandleRef(name);
  return (handleRef == NULL) ? (StateHandle_t){"NULL", NULL} : *handleRef;
}

StateHandle_t *StateHandle_getHandleRef(char name[STATE_NAME_LENGTH]) {
  // Iterate through all handles in State vector
  for (uint8_t *i = (uint8_t *)&__state_vector_start; i < (uint8_t *)&__state_vector_end; i += sizeof(StateHandle_t)) {
    StateHandle_t *handle = (StateHandle_t *)i;
    // Return State handle if names match
    if (!strcmp(handle->name, name)) {
      handle->ref = handle;
			return handle;
		}
  }
  // Return NULL pointer if no matching State is found
  return NULL;
}

extern uint32_t __device_vector_start;
extern uint32_t __device_vector_end;

DeviceHandle_t DeviceHandle_getHandle(char name[DEVICE_NAME_LENGTH]) {
	DeviceHandle_t *handleRef = DeviceHandle_getHandleRef(name);
  return (handleRef == NULL) ? (DeviceHandle_t){"NULL", NULL} : *handleRef;
}

DeviceHandle_t *DeviceHandle_getHandleRef(char name[DEVICE_NAME_LENGTH]) {
  // Iterate through all handles in device vector
  for (uint8_t *i = (uint8_t *)&__device_vector_start; i < (uint8_t *)&__device_vector_end; i += sizeof(DeviceHandle_t)) {
    DeviceHandle_t *handle = (DeviceHandle_t *)i;
    // Return device handle if names match
    if (!strcmp(handle->name, name)) {
      handle->ref = handle;
			return handle;
		}
  }
  // Return NULL pointer if no matching device is found
  return NULL;
}
