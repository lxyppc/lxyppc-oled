#ifndef CONFIG_H
#define CONFIG_H
#define   DEBUG_BOARD     0

typedef   struct _TaskContext
{
  /// Auto protected context
  u32 R0;
  u32 R1;
  u32 R2;
  u32 R3;
  u32 R12;          /// R12
  u32 LR;           /// LR
  u32 EntryPoint;   /// PC
  u32 xPSR;         /// xPSR
  
  /// Remain context
  /*
  u32 R11;
  u32 R10;
  u32 R9;
  u32 R8;
  u32 R7;
  u32 R6;
  u32 R5;
  u32 R4;
  */
}TaskContext , *PTaskContext;

/// Get the variable's correspond bit banding
#define   GetBit(var, bitnum)     \
  (*((vu32*)(( (u32)&(var) & 0xF0000000)+0x2000000+(((u32)&(var) & 0xFFFFF)<<5)+(bitnum<<2))))
/// Task context structure in the stack

#define   RING_SIZE        8      /// size of the ring buffer
#define   RING_MASK        0x07   /// Mask of the ring buffer
#define   RECV_BUF_SIZE    64     /// size of the recieve buffer
extern  u32     readPtr;          /// read pointer of the recieve ring buffer
extern  vu32    writePtr;         /// write pointer of the recieve ring buffer
extern  u8      recvBuffer[RING_SIZE][RECV_BUF_SIZE];

#define   DEV_VERSION       0x0100

#define   USER_GET          0xF0

#endif

