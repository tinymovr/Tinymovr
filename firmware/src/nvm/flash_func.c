//=============================================================================
// Copyright (C) 2018, Active-Semi International
//
// THIS SOFTWARE IS SUBJECT TO A SOURCE CODE LICENSE AGREEMENT WHICH PROVIDES,
// AMONG OTHER THINGS:  (i) THAT IT CAN BE USED ONLY TO ADAPT THE LICENSEE'S
// APPLICATION TO PAC PROCESSORS SUPPLIED BY ACTIVE-SEMI INTERNATIONAL;
// (ii) THAT  IT IS PROVIDED "AS IS" WITHOUT WARRANTY;  (iii) THAT
// ACTIVE-SEMICONDUCTOR IS NOT LIABLE FOR ANY INDIRECT DAMAGES OR FOR DIRECT
// DAMAGES EXCEEDING US$1,500;  AND (iv) THAT IT CAN BE DISCLOSED TO AND USED
// ONLY BY CERTAIN AUTHORIZED PERSONS.
//
//=============================================================================

#include "flash_func.h"

//==============================================================================
///
/// @brief  Erase the specified flash page
///
/// @param  page_num: the Flash page to erase
///
/// @retval None
///
//==============================================================================
TM_RAMFUNC void flash_erase_page(uint32_t page_num)
{
    // Enable Write Access to FLash Controller
    PAC55XX_MEMCTL->FLASHLOCK = FLASH_LOCK_ALLOW_WRITE_ERASE_FLASH;

    // Set Page to Be Written
    PAC55XX_MEMCTL->FLASHPAGE.PAGE = page_num;

    // Erase Page
    PAC55XX_MEMCTL->FLASHERASE = FLASH_START_PAGE_ERASE;

    // Wait for erase to complete
    while (PAC55XX_MEMCTL->MEMSTATUS.EBUSY) { }

    // Disable Write Access to FLash Controller
    PAC55XX_MEMCTL->FLASHLOCK = 0;
}

//==============================================================================
///
/// @brief  Perform Flash erase of type specified by key
///
/// @param  key  Flash erase Key that determines type of Flash erase
///
/// @retval None
///
//==============================================================================
TM_RAMFUNC void flash_erase_key(uint32_t key)
{
    // Enable Write Access to FLash Controller
    PAC55XX_MEMCTL->FLASHLOCK = FLASH_LOCK_ALLOW_WRITE_ERASE_FLASH;

    // Start Flash erase of type specified by key
    PAC55XX_MEMCTL->FLASHERASE = key;

    // Wait for erase to complete
    while (PAC55XX_MEMCTL->MEMSTATUS.EBUSY) { }

    // Disable Write Access to FLash Controller
    PAC55XX_MEMCTL->FLASHLOCK = 0;
}


//==============================================================================
/// @brief Write Flash with a byte data array of any size on any boundary
///
/// @param p_dest      pointer to destination FLASH to write, can be any byte address
/// @param p_src       pointer to source buffer to write, can be any byte address
/// @param size_bytes  size in bytes of the FLASH to write
///
/// @retval  None
///
//==============================================================================
#if defined(__GNUC__)
    __attribute__((optimize("-O0")))
#endif
PAC5XXX_RAMFUNC void flash_write(uint8_t *p_dest, uint8_t *p_src, uint32_t size_bytes)
{
    // The memory controller requires all flash writes to start on a 16-byte boundary and consist of 16 bytes in size
    // If the desired amount to be written is less than 16 bytes, this code writes the other bytes as 0xFF to preserve the contents.
    // The Memory Controller will automatically hold off the write of the next 16 bytes until the previous write is complete.
    // Note that reads or fetches from Flash should not take place until WBUSY=0 and an additional delay of 10 uSec has been added

    union {
        uint8_t b[16];
        uint32_t w[4];
    }buff;
    uint8_t i;
    uint32_t src_index;
    uint8_t buff_index;
    uint32_t *p_flash_write;

    // Clear WRITEWORDCNT in case it's not 0; must set FLASHLOCK to allow write to MEMCTL
    PAC55XX_MEMCTL->FLASHLOCK = FLASH_LOCK_ALLOW_WRITE_MEMCTL;
    PAC55XX_MEMCTL->MEMCTL.WRITEWORDCNT = 0;

    // Set FLASHLOCK to allow Writes to Flash
    PAC55XX_MEMCTL->FLASHLOCK = FLASH_LOCK_ALLOW_WRITE_ERASE_FLASH;

    // Set pointer for Flash writes to 16 byte boundary;  All flash writes must start on a 16-byte boundary and consist of 16 bytes in size
    p_flash_write = (uint32_t *)((uint32_t)p_dest & ~0xF);

    // buff_index points to the first byte to contain valid data
    buff_index = (uint32_t)p_dest & 0xF;

    // Fill 16 byte temp buffer with FFs up to buff_index (must write FFs to bytes that shouldn't change)
    for(i=0; i < buff_index; i++)
    {
        buff.b[i] = 0xFF;
    }

    src_index = 0;
    // Fill the temp buffer and write when it's full
    while(size_bytes--)
    {
        // Copy enough bytes from source buffer into temp buffer to reach 16 bytes
        buff.b[buff_index++] = p_src[src_index++];

        // If we have 16 bytes in temp buff, then write the 16 bytes of data
        if(buff_index == 16)
        {
            // Write 4 words (16 bytes) to Flash
            for(i=0; i < 4; i++)
            {
                *p_flash_write++ = buff.w[i];
            }
            buff_index = 0;                     // Reset buff_index for next time through
        }
    }

    // If index is non zero then we need to fill remaining buffer with FFs and write last bytes
    if(buff_index)
    {
        // Fill remainder of buffer with FFs
        for(i=buff_index; i < 16; i++)
        {
             buff.b[i] = 0xFF;
        }

        // Write final 4 32-bit words (16 bytes) to Flash
        for(i=0; i < 4; i++)
        {
            *p_flash_write++ = buff.w[i];
        }
    }

    // Make sure were not still busy writing and add delay before allowing read/fetch access to flash
    while(PAC55XX_MEMCTL->MEMSTATUS.WBUSY) { }  // wait for Flash Write WBUSY bit to be 0
    pac_delay_asm_ramfunc(320);                 // delay 20uS after WBUSY=0

    // Return FLASHLOCK to locked state
    PAC55XX_MEMCTL->FLASHLOCK =0;

}

//================================================================================================
/// @brief  Write to Flash location that is 16 byte aligned with a mulitple of 4 words
///
/// @note  Pay careful attention to alignment destination and source addresses along with 
///        the number of words to write.  If these are not correct and Error will be returned
///
/// @param  p_dest: pointer to destination Flash to write, must be 16 byte aligned
/// @param  p_src:  pointer to source buffer to write, must be 32-bit aligned
/// @param  size_32bit_words: size in 32-bit words of the Flash to write, must be a multiple of 4
///
/// @retval PAC5XXX_OK: All is OK, PAC5XXX_ERROR: An error occurred
///
//=================================================================================================
TM_RAMFUNC uint32_t flash_write_16byte_aligned(uint32_t *p_dest, uint32_t *p_src, uint32_t size_32bit_words)
{

    // If destination is not 16-byte aligned OR source isn't 32-bit aligned OR size is not a multiple of 16 bytes, then return an error
	if( ((uint32_t)p_dest & 0xF) || ((uint32_t)p_src & 0x3) || (size_32bit_words % 4) )
		return PAC5XXX_ERROR;

	// Enable Write Access to FLash Controller
	PAC55XX_MEMCTL->FLASHLOCK = FLASH_LOCK_ALLOW_WRITE_ERASE_FLASH;

	// Ensure Flash write Word Count starts at 0
	PAC55XX_MEMCTL->MEMCTL.WRITEWORDCNT = 0;

	// Write data buffer to FLASH
	for(uint32_t i = 0; i < size_32bit_words; i++)
	{
		p_dest[i] = p_src[i];
	}

    // Make sure were not still busy writing and add delay before allowing read/fetch access to flash
    while(PAC55XX_MEMCTL->MEMSTATUS.WBUSY) { }  // wait for Flash Write WBUSY bit to be 0
    pac_delay_asm_ramfunc(320);                 // delay 20uS after WBUSY=0

    // Disable Write Access to FLash Controller
	PAC55XX_MEMCTL->FLASHLOCK = 0;

	return PAC5XXX_OK;
}

//==============================================================================
///
/// @brief  Write Flash with a single 32bit value
///
/// @param  dest_addr: Flash Destination address ( Should be A multiple of 4 )
/// @param  data: 32-bit data to be programmed
///
/// @retval None
///
//==============================================================================
TM_RAMFUNC void flash_write_word(uint32_t * p_dest, uint32_t value)
{
    flash_write((uint8_t *)p_dest, (uint8_t *)&value, 4);     // Destination, Source, size_bytes
}

