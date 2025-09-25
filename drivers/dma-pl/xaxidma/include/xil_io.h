/******************************************************************************
* Copyright (C) 2014 - 2021 Xilinx, Inc.  All rights reserved.
* Copyright (C) 2022 - 2023 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xil_io.h
*
* @addtogroup common_io_interfacing_apis Register IO interfacing APIs
*
* The xil_io.h file contains the interface for the general I/O component, which
* encapsulates the Input/Output functions for the processors that do not
* require any special I/O handling.
*
* @{
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who      Date     Changes
* ----- -------- -------- -----------------------------------------------
* 5.00 	pkp  	 05/29/14 First release
* 6.00  mus      08/19/16 Remove checking of __LITTLE_ENDIAN__ flag for
*                         ARM processors
* 7.20  har      01/03/20 Added Xil_SecureOut32 for avoiding blindwrite for
*                         CR-1049218
* 7.30  kpt      09/21/20 Moved Xil_EndianSwap16 and Xil_EndianSwap32 to
*                         xil_io.h and made them as static inline
*       am       10/13/20 Changed the return type of Xil_SecureOut32 function
*                         from u32 to int
* 7.50  dp       02/12/21 Fix compilation error in Xil_EndianSwap32() that occur
*                         when -Werror=conversion compiler flag is enabled
* 7.5   mus      05/17/21 Update the functions with comments. It fixes CR#1067739.
* 9.0   ml       03/03/23 Add description and remove comments to fix doxygen warnings.
* </pre>
******************************************************************************/

#ifndef XIL_IO_H           /**< prevent circular inclusions */
#define XIL_IO_H           /**< by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/

#include <linux/types.h>
#include "xil_types.h"
#include <linux/printk.h>	// #include "xil_printf.h"
#include "xstatus.h"

#if defined (__MICROBLAZE__)
#include "mb_interface.h"
#else
#include <asm/io.h> 	// #include "xpseudo_asm.h"
#endif

/************************** Function Prototypes ******************************/
#ifdef ENABLE_SAFETY
extern uint32_t XStl_RegUpdate(uint32_t RegAddr, uint32_t RegVal);
#endif

/***************** Macros (Inline Functions) Definitions *********************/
#if defined __GNUC__
#if defined (__MICROBLAZE__)
#  define INST_SYNC		mbar(0) /**< Instruction Synchronization Barrier */
#  define DATA_SYNC		mbar(1) /**<  Data Synchronization Barrier  */
# else
#  define SYNCHRONIZE_IO	dmb() /**< Data Memory Barrier */
#  define INST_SYNC		isb() /**< Instruction Synchronization Barrier */
#  define DATA_SYNC		dsb() /**<  Data Synchronization Barrier  */
# endif
#else
# define SYNCHRONIZE_IO /**< Data Memory Barrier */
# define INST_SYNC /**< Instruction Synchronization Barrier */
# define DATA_SYNC /**<  Data Synchronization Barrier  */
# define INST_SYNC /**< Instruction Synchronization Barrier */
# define DATA_SYNC /**<  Data Synchronization Barrier  */
#endif

#if defined (__GNUC__) || defined (__ICCARM__) || defined (__MICROBLAZE__)
#define INLINE inline /**< static inline keyword */
#else
#define INLINE __inline /**<static inline keyword */
#endif

/*****************************************************************************/
/**
*
* @brief    Performs an input operation for a memory location by reading
*           from the specified address and returning the 8 bit Value read from
*            that address.
*
* @param	Addr contains the address to perform the input operation
*
* @return	The 8 bit Value read from the specified input address.

*
******************************************************************************/
static INLINE uint8_t Xil_In8(UINTPTR Addr)
{
	return *(volatile uint8_t *) Addr;
}

/*****************************************************************************/
/**
*
* @brief    Performs an input operation for a memory location by reading from
*           the specified address and returning the 16 bit Value read from that
*           address.
*
* @param	Addr contains the address to perform the input operation
*
* @return	The 16 bit Value read from the specified input address.
*
******************************************************************************/
static INLINE uint16_t Xil_In16(UINTPTR Addr)
{
	return *(volatile uint16_t *) Addr;
}

/*****************************************************************************/
/**
*
* @brief    Performs an input operation for a memory location by
*           reading from the specified address and returning the 32 bit Value
*           read  from that address.
*
* @param	Addr contains the address to perform the input operation
*
* @return	The 32 bit Value read from the specified input address.
*
******************************************************************************/
static INLINE uint32_t Xil_In32(UINTPTR Addr)
{
	return *(volatile uint32_t *) Addr;
}

/*****************************************************************************/
/**
*
* @brief     Performs an input operation for a memory location by reading the
*            64 bit Value read  from that address.
*
*
* @param	Addr contains the address to perform the input operation
*
* @return	The 64 bit Value read from the specified input address.
*
******************************************************************************/
static INLINE uint64_t Xil_In64(UINTPTR Addr)
{
	return *(volatile uint64_t *) Addr;
}

/*****************************************************************************/
/**
*
* @brief    Performs an output operation for an memory location by
*           writing the 8 bit Value to the the specified address.
*
* @param	Addr contains the address to perform the output operation
* @param	Value contains the 8 bit Value to be written at the specified
*           address.
*
* @return	None
*
******************************************************************************/
static INLINE void Xil_Out8(UINTPTR Addr, uint8_t Value)
{
	/* write 8 bit value to specified address */
	volatile uint8_t *LocalAddr = (volatile uint8_t*)Addr;
	*LocalAddr = Value;
}

/*****************************************************************************/
/**
*
* @brief    Performs an output operation for a memory location by writing the
*            16 bit Value to the the specified address.
*
* @param	Addr contains the address to perform the output operation
* @param	Value contains the Value to be written at the specified address.
*
* @return	None.
*
******************************************************************************/
static INLINE void Xil_Out16(UINTPTR Addr, uint16_t Value)
{
	/* write 16 bit value to specified address */
	volatile uint16_t *LocalAddr = (volatile uint16_t*)Addr;
	*LocalAddr = Value;
}

/*****************************************************************************/
/**
*
* @brief    Performs an output operation for a memory location by writing the
*           32 bit Value to the the specified address.
*
* @param	Addr contains the address to perform the output operation
* @param	Value contains the 32 bit Value to be written at the specified
*           address.
*
* @return	None.
*
******************************************************************************/
static INLINE void Xil_Out32(UINTPTR Addr, uint32_t Value)
{
	/* write 32 bit value to specified address */
#ifndef ENABLE_SAFETY
	volatile uint32_t *LocalAddr = (volatile uint32_t*)Addr;
	*LocalAddr = Value;
#else
	XStl_RegUpdate(Addr, Value);
#endif
}

/*****************************************************************************/
/**
*
* @brief    Performs an output operation for a memory location by writing the
*           64 bit Value to the the specified address.
*
* @param	Addr contains the address to perform the output operation
* @param	Value contains 64 bit Value to be written at the specified address.
*
* @return	None.
*
******************************************************************************/
static INLINE void Xil_Out64(UINTPTR Addr, uint64_t Value)
{
	/* write 64 bit value to specified address */
	volatile uint64_t *LocalAddr = (volatile uint64_t *)Addr;
	*LocalAddr = Value;
}

/*****************************************************************************/
/**
 *
 * @brief	Performs an output operation for a memory location by writing the
 *       	32 bit Value to the the specified address and then reading it
 *       	back to verify the value written in the register.
 *
 * @param	Addr contains the address to perform the output operation
 * @param	Value contains 32 bit Value to be written at the specified address
 *
 * @return	Returns Status
 *        	- XST_SUCCESS on success
 *        	- XST_FAILURE on failure
 *
 *****************************************************************************/
static INLINE int Xil_SecureOut32(UINTPTR Addr, uint32_t Value)
{
	int Status = XST_FAILURE;
	uint32_t ReadReg;
	uint32_t ReadRegTemp;

	/* writing 32 bit value to specified address */
	Xil_Out32(Addr, Value);

	/* verify value written to specified address with multiple reads */
	ReadReg = Xil_In32(Addr);
	ReadRegTemp = Xil_In32(Addr);

	if( (ReadReg == Value) && (ReadRegTemp == Value) ) {
		Status = XST_SUCCESS;
	}

	return Status;
}

/*****************************************************************************/
/**
*
* @brief    Perform a 16-bit endian conversion.
*
* @param	Data 16 bit value to be converted
*
* @return	16 bit Data with converted endianness
*
******************************************************************************/
static INLINE __attribute__((always_inline)) uint16_t Xil_EndianSwap16(uint16_t Data)
{
	return (uint16_t) (((Data & 0xFF00U) >> 8U) | ((Data & 0x00FFU) << 8U));
}

/*****************************************************************************/
/**
*
* @brief    Perform a 32-bit endian conversion.
*
* @param	Data : 32 bit value to be converted
*
* @return	32 bit data with converted endianness
*
******************************************************************************/
static INLINE __attribute__((always_inline)) uint32_t Xil_EndianSwap32(uint32_t Data)
{
	uint16_t LoWord;
	uint32_t HiWord;

	/* get each of the half words from the 32 bit word */

	LoWord = (uint16_t) (Data & 0x0000FFFFU);
	HiWord = (uint16_t) ((Data & 0xFFFF0000U) >> 16U);

	/* byte swap each of the 16 bit half words */

	LoWord = (uint16_t)(((LoWord & 0xFF00U) >> 8U) | ((LoWord & 0x00FFU) << 8U));
	HiWord = (uint16_t)(((HiWord & 0xFF00U) >> 8U) | ((HiWord & 0x00FFU) << 8U));

	/* swap the half words before returning the value */

	return ((((uint32_t)LoWord) << (uint32_t)16U) | (int32_t)HiWord);
}

#if defined (__MICROBLAZE__)
#ifdef __LITTLE_ENDIAN__
# define Xil_In16LE	Xil_In16 /**< Register Read of 16 Bits in Little Endian */
# define Xil_In32LE	Xil_In32 /**< Register Read of 32 Bits in Little Endian */
# define Xil_Out16LE	Xil_Out16 /**< Register Write of 16 Bits in Little Endian */
# define Xil_Out32LE	Xil_Out32 /**< Register Write of 32 Bits in Little Endian */
# define Xil_Htons	Xil_EndianSwap16 /**< Endian swap of 16 bits */
# define Xil_Htonl	Xil_EndianSwap32 /**< Endian swap of 32 bits */
# define Xil_Ntohs	Xil_EndianSwap16 /**< Endian swap of 16 bits */
# define Xil_Ntohl	Xil_EndianSwap32 /**< Endian swap of 32 bits */
# else
# define Xil_In16BE	Xil_In16 /**< Register Read of 16 Bits in Big Endian */
# define Xil_In32BE	Xil_In32 /**< Register Read of 32 Bits in Big Endian */
# define Xil_Out16BE	Xil_Out16 /**< Register Write of 16 Bits in Big Endian */
# define Xil_Out32BE	Xil_Out32 /**< Register Write of 32 Bits in Big Endian */
# define Xil_Htons(Data) (Data) /**< Endian swap of 16 bits */
# define Xil_Htonl(Data) (Data) /**< Endian swap of 32 bits */
# define Xil_Ntohs(Data) (Data) /**< Endian swap of 16 bits */
# define Xil_Ntohl(Data) (Data) /**< Endian swap of 32 bits */
#endif
#else
# define Xil_In16LE	Xil_In16 /**< Register Read of 16 Bits in Little Endian */
# define Xil_In32LE	Xil_In32 /**< Register Read of 32 Bits in Little Endian */
# define Xil_Out16LE	Xil_Out16 /**< Register Write of 16 Bits in Little Endian */
# define Xil_Out32LE	Xil_Out32 /**< Register Write of 32 Bits in Little Endian */
# define Xil_Htons	Xil_EndianSwap16 /**< Endian swap of 16 bits */
# define Xil_Htonl	Xil_EndianSwap32 /**< Endian swap of 32 bits */
# define Xil_Ntohs	Xil_EndianSwap16 /**< Endian swap of 16 bits */
# define Xil_Ntohl	Xil_EndianSwap32 /**< Endian swap of 32 bits */
#endif

#if defined (__MICROBLAZE__)
#ifdef __LITTLE_ENDIAN__
static INLINE uint16_t Xil_In16BE(UINTPTR Addr) /**< Static inline function to Read
                                                Register of 16 Bits in Big Endian */
#else
static INLINE uint16_t Xil_In16LE(UINTPTR Addr) /**< Static inline function to Read
                                                Register of 16 Bits in Little Endian */
#endif
#else
/**
* Static inline function to Read Register of 16 Bits in Big Endian.
**********************************************************************************/
static INLINE uint16_t Xil_In16BE(UINTPTR Addr)
#endif
{
	uint16_t = Xil_In16(Addr);
	return Xil_EndianSwap16(value);
}

#if defined (__MICROBLAZE__)
#ifdef __LITTLE_ENDIAN__
static INLINE uint32_t Xil_In32BE(UINTPTR Addr) /**< Static inline function to Read
                                                Register of 32 Bits in Big Endian */
#else
static INLINE uint32_t Xil_In32LE(UINTPTR Addr) /**< Static inline function to Read
                                                Register of 32 Bits in Little Endian */
#endif
#else
/**
* Static inline function to Read Register of 32 Bits in Big Endian.
**********************************************************************************/
static INLINE uint32_t Xil_In32BE(UINTPTR Addr)
#endif
{
	uint32_t value = Xil_In32(Addr);
	return Xil_EndianSwap32(value);
}

#if defined (__MICROBLAZE__)
#ifdef __LITTLE_ENDIAN__
static INLINE void Xil_Out16BE(UINTPTR Addr, uint16_t Value) /**< Static inline function to write
                                                Register of 16 Bits in Big Endian */
#else
static INLINE void Xil_Out16LE(UINTPTR Addr, uint16_t Value) /**< Static inline function to write
                                                Register of 16 Bits in Little Endian */
#endif
#else
/**
* Static inline function to write Register of 16 Bits in Big Endian.
**********************************************************************************/
static INLINE void Xil_Out16BE(UINTPTR Addr, uint16_t Value)
#endif
{
	Value = Xil_EndianSwap16(Value);
	Xil_Out16(Addr, Value);
}

#if defined (__MICROBLAZE__)
#ifdef __LITTLE_ENDIAN__
static INLINE void Xil_Out32BE(UINTPTR Addr, uint32_t Value) /**< Static inline function to write
                                                Register of 32 Bits in Big Endian */
#else
static INLINE void Xil_Out32LE(UINTPTR Addr, uint32_t Value) /**< Static inline function to write
                                                Register of 32 Bits in Little Endian */
#endif
#else
/**
* Static inline function to Read Register of 16 Bits in Big Endian.
**********************************************************************************/
static INLINE void Xil_Out32BE(UINTPTR Addr, uint32_t Value)
#endif
{
	Value = Xil_EndianSwap32(Value);
	Xil_Out32(Addr, Value);
}

#ifdef __cplusplus
}
#endif

#endif /* end of protection macro */
/**
* @} End of "addtogroup common_io_interfacing_apis".
*/
