// dpmi.h -- audiolib locked what its interrupt handlers touch, so DOS's
// extender would not page it out. There is nothing to lock here.
#ifndef __C4ROTT_DPMI_H
#define __C4ROTT_DPMI_H
enum DPMI_Errors { DPMI_Warning = -2, DPMI_Error = -1, DPMI_Ok = 0 };
#define DPMI_LockMemoryRegion(start, end) DPMI_Ok
#define DPMI_UnlockMemoryRegion(start, end) DPMI_Ok
#define DPMI_LockMemory(address, length) DPMI_Ok
#define DPMI_UnlockMemory(address, length) DPMI_Ok
#define DPMI_Lock(variable) DPMI_Ok
#define DPMI_Unlock(variable) DPMI_Ok
#endif
