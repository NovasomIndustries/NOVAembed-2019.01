/*
***********************************************************************************************
*
* C file for S25FS512C
*
*
*
*
************************************************************************************************
*/

/* Standard C libraries */
#include    <stdio.h>
#include    <stdint.h>
#include    <stdlib.h>
#include    <string.h>

/* Include D2XX header*/
#include    "ftd2xx.h"

/* Include libMPSSE header */
#include    "libMPSSE_spi.h"
#include    "s25fs512.h"

/******************************************************************************/
/*								Global variables							  	    */
/******************************************************************************/
static FT_HANDLE *ftHandle;

unsigned char Init(FT_HANDLE *handle)
{
    ftHandle = handle;
    return 0;
}

unsigned char Flash_Write(unsigned char *write_buf , int len_data , unsigned int write_address)
{
#define PAGE_LEN    512
#define CMD_LEN     5

unsigned char   loc_tx_buff[PAGE_LEN+CMD_LEN];
unsigned int    i,sizeTransfered;
unsigned int    pagelen = 512;

    flash_WRAR(CR2V, 0x00);     // no latency
    flash_WRAR(CR3V, 0x3b);     // Setup page 512

	while (len_data > 0)
	{
        loc_tx_buff[0] = CMD_4PROGRAM_PAGE;
        loc_tx_buff[1] = (unsigned char)(write_address >> 24);
        loc_tx_buff[2] = (unsigned char)(write_address >> 16);
        loc_tx_buff[3] = (unsigned char)(write_address >> 8 );
        loc_tx_buff[4] = (unsigned char)(write_address);

        for (i = 0; i < PAGE_LEN; i++)
            loc_tx_buff[i + CMD_LEN] = *write_buf++;

        while( flash_busy_and_error_check() )
            usleep(100);

        flash_write_enable();
        if (SPI_Write(ftHandle, loc_tx_buff, PAGE_LEN+CMD_LEN, (uint32 *)&sizeTransfered,SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES|SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE) )
            exit (1);

        flash_write_disable();

		write_address += pagelen;
		len_data -= pagelen;
	}
    return 0;
}

unsigned char Flash_Read(unsigned int address, unsigned int len_buff, unsigned char *read_buffer)
{
unsigned char  cmd_buf[6];
unsigned int sizeTransfered = 0 , block_count = 0 , cnt = 0;
#define     BLOCK_SIZE      8192

    printf("Start Read %d\n",len_buff);
    while(flash_busy_and_error_check())
        usleep(100);
    while( len_buff > 0 )
    {
        cmd_buf[0] = CMD_4FAST_READ_PAGE;
        cmd_buf[1] = (unsigned char)(address >> 24);
        cmd_buf[2] = (unsigned char)(address >> 16);
        cmd_buf[3] = (unsigned char)(address >> 8);
        cmd_buf[4] = (unsigned char)address;
        if (SPI_Write(ftHandle, cmd_buf, 5, (uint32 *)&sizeTransfered,SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES|SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE ) != FT_OK)
            exit (1);
        if (SPI_Read(ftHandle, read_buffer, BLOCK_SIZE, (uint32 *)&sizeTransfered,SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES|SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE) != FT_OK)
            exit (1);

        cnt++;
        if ( cnt >= 16 )
        {
            printf("0x%08x : %d %d , left to read : %d\n",address,sizeTransfered,block_count++,len_buff );
            cnt = 0;
        }
        address += BLOCK_SIZE;
        read_buffer += BLOCK_SIZE;
        len_buff -= BLOCK_SIZE;
    }
    printf("Read done\n");
    return 0;//okay
}

unsigned char write_byte(unsigned char *buffer_out, unsigned int len_tx)
{
unsigned int sizeTransfered = 0;

    if (SPI_Write(ftHandle, buffer_out, len_tx, (uint32 *)&sizeTransfered,SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES|SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE) )
        return (1);//ko
    return (0);
}

unsigned char flash_write_enable(void)
{
unsigned int sizeTransfered;
unsigned char loc_tx_buff[2];
    loc_tx_buff[0] = CMD_WRITE_ENABLED;
    if (SPI_Write(ftHandle, loc_tx_buff, 1, (uint32 *)&sizeTransfered,SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES|SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE) )
        return (1);//ko
    return (0);
}

unsigned char flash_write_disable(void)
{
unsigned int sizeTransfered;
unsigned char loc_tx_buff[2];
    loc_tx_buff[0] = CMD_WRITE_DISABLED;
    if (SPI_Write(ftHandle, loc_tx_buff, 1, (uint32 *)&sizeTransfered,SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES|SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE) )
        return (1);//ko
    return (0);
}


unsigned char flash_busy_and_error_check(void)
{
unsigned char cmd_buf[1]    = {CMD_READ_SR1};
unsigned char loc_rx_buff[2] = {0xff, 0xff};    //dummy
unsigned int sizeTransfered,loc_status;
     /* READ PAGE COMMAND */
    cmd_buf[0] = CMD_READ_SR1;
    loc_status = SPI_Write(ftHandle, cmd_buf, 1, (uint32 *)&sizeTransfered,SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES|SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE );//| SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE
    if (loc_status != FT_OK)
        exit (1);
    loc_status = SPI_Read(ftHandle, loc_rx_buff, 2, (uint32 *)&sizeTransfered,SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES|SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
    if ( (loc_rx_buff[1] & E_ERR_BIT) )
    {
        printf("%s : E_ERR_BIT Error\n", __FUNCTION__);
        exit(1);
    }

    if (loc_rx_buff[1] & WIP_BIT)
    {
        return 1;   /* Device Busy, retry*/
    }
    else
        return 0;   /* okay */
}

void special_register_read(unsigned char reg_cmd , unsigned char *val , int len)
{
unsigned char cmd_buf[1];
unsigned int sizeTransfered;

    if ( len > 64 )
    {
        printf("%s : Len too big, max is 64 , len is %d\n",__FUNCTION__ , len);
        exit(1);
    }
    cmd_buf[0] = reg_cmd;
    if (SPI_Write(ftHandle, cmd_buf, 1, (uint32 *)&sizeTransfered,SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES|SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE ) != FT_OK)
        exit (1);
    if (SPI_Read(ftHandle, val, len, (uint32 *)&sizeTransfered,SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES|SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE) != FT_OK)
        exit (1);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ReadRegister_RDAR
//
// The Read Any Register (RDAR) command provides a way to read all device registers - non-volatile and volatile.
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
unsigned char flash_RDAR(unsigned int addr_reg_to_read, unsigned char *reg_read)
{
unsigned char loc_tx_buff[10];
unsigned int sizeTransfered;

    /* WRITE_ENABLED */

    loc_tx_buff[0] = RDAR;                       //command

    loc_tx_buff[4] =  addr_reg_to_read & 0xff;           //address
    loc_tx_buff[3] = (addr_reg_to_read >> 8)  & 0xff;
    loc_tx_buff[2] = (addr_reg_to_read >> 16) & 0xff;
    loc_tx_buff[1] = (addr_reg_to_read >> 24) & 0xff;      //address msb
    if ( SPI_Write(ftHandle, loc_tx_buff, 5, (uint32 *)&sizeTransfered,SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES|SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE ) )
    {
        printf("Error sending RDAR\n");
        exit(1);
    }

    if ( SPI_Read(ftHandle, loc_tx_buff, 1, (uint32 *)&sizeTransfered,SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES| SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE) )
        return 1;
    *reg_read = loc_tx_buff[0];
    return 0;//okay
}

unsigned char flash_IDR(unsigned char *reg_read)
{
unsigned char loc_tx_buff[16];
unsigned int sizeTransfered;

    /* WRITE_ENABLED */

    loc_tx_buff[0] = CMD_GET_IDENTIFICATION;                       //command
    if ( SPI_Write(ftHandle, loc_tx_buff, 1, (uint32 *)&sizeTransfered,SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES|SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE ) )
    {
        printf("Error sending CMD_GET_IDENTIFICATION\n");
        exit(1);
    }

    if ( SPI_Read(ftHandle, reg_read, 16, (uint32 *)&sizeTransfered,SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES| SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE) )
        return 1;
    return 0;//okay
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// WriteRegister_WRAR
//
// The Write Any Register (WRAR) command provides a way to write any device register - non-volatile or volatile.
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
unsigned char flash_WRAR(unsigned int addr_reg_to_read, unsigned char data_to_write)
{
unsigned char loc_tx_buff[10];
unsigned int  rd_wr_len  = 0;

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+     Write "Read Latency" dummy cycle
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    /* 3byte add */
    loc_tx_buff[0] = WRAR;                       //command
    loc_tx_buff[4] =  addr_reg_to_read & 0xff;           //address
    loc_tx_buff[3] = (addr_reg_to_read >> 8)  & 0xff;
    loc_tx_buff[2] = (addr_reg_to_read >> 16) & 0xff;
    loc_tx_buff[1] = (addr_reg_to_read >> 24) & 0xff;

    loc_tx_buff[4] = data_to_write;                     //data to write!!!

    rd_wr_len      = 5;

    if (write_byte(loc_tx_buff, rd_wr_len))
        return 1;


    return 0;//okay
}


unsigned char SectorErase(unsigned int sector_num)
{
unsigned char cmd_buf[6];
unsigned int  i  = 0;
unsigned int sizeTransfered;

    flash_write_enable();
    sector_num *= 256 * 1024;

    /* ERASE Sector */
    cmd_buf[0] = CMD_4ERASE_SECTOR;//command

    cmd_buf[4] = sector_num & 0xff;           //address lsb
    cmd_buf[3] = (sector_num >> 8) & 0xff;
    cmd_buf[2] = (sector_num >> 16) & 0xff;
    cmd_buf[1] = (sector_num >> 24) & 0xff;      //address msb

    if ( SPI_Write(ftHandle, cmd_buf, 5, (uint32 *)&sizeTransfered,SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES|SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE ) )
    {
        printf("Error erasing sector %d\n",i);
        exit(1);
    }
    flash_write_disable();
    return 0;
}

unsigned char AddressSectorErase(unsigned int address , int bank_size)
{
unsigned char cmd_buf[6];
unsigned int  i  = 0 , number_of_sectors;
unsigned int sizeTransfered;

    number_of_sectors = bank_size / (256*1024);
    printf("Erasing %d 256KB sectors at address 0x%08x\n",number_of_sectors,address);
    while(flash_busy_and_error_check())
        usleep(100);
    for(i=0;i<number_of_sectors;i++)
    {
        flash_write_enable();
        cmd_buf[0] = CMD_4ERASE_SECTOR;//command
        cmd_buf[4] = address & 0xff;           //address lsb
        cmd_buf[3] = (address >> 8) & 0xff;
        cmd_buf[2] = (address >> 16) & 0xff;
        cmd_buf[1] = (address >> 24) & 0xff;      //address msb
        if ( SPI_Write(ftHandle, cmd_buf, 5, (uint32 *)&sizeTransfered,SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES|SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE ) )
        {
            printf("Error erasing sector %d\n",i);
            return 1;
        }
        flash_write_disable();
    }
    return 0;
}


unsigned char BulkErase(void)
{
unsigned char loc_tx_buff[6];
unsigned int rd_wr_len  = 0;

    /* write enable */
    flash_write_enable();

    /* ERASE */
    loc_tx_buff[0] = CMD_BULK_ERASE;
    rd_wr_len      = 1;

    if (write_byte(loc_tx_buff, rd_wr_len))
         return 1;

    /* write disable */
    //flash_write_disable();
    return 0;
}

unsigned char print_file_in_buffer(const char *file_path, unsigned char *buffer_out, unsigned int offset)
{

unsigned int size_of_file;
unsigned int i;
unsigned char loc_buffer[5242880]; //5*1024*1024= 5242880 MB
FILE* file_ptr;

    // opening the file in read mode
    file_ptr = fopen(file_path, "rb");

    // checking if the file exist or not
    if (file_ptr == NULL)
    {
        printf("File %s Not Found!\n", file_path);
        return -1;
    }

    fseek(file_ptr, 0L, SEEK_END);
    size_of_file = ftell(file_ptr);
    fseek(file_ptr, 0L, SEEK_SET);
    fread(loc_buffer, size_of_file, 1, file_ptr);

    buffer_out += offset;

    for (i=0; i < size_of_file; i++)
    {

       *buffer_out++ = loc_buffer[i];

    }
    fclose(file_ptr);
    return 1;
}

unsigned char swreset(void)
{
unsigned char cmd_buf[6];
unsigned int sizeTransfered;

    cmd_buf[0] = CMD_RSTEN;
    cmd_buf[1] = CMD_RESET;

    if ( SPI_Write(ftHandle, cmd_buf, 2, (uint32 *)&sizeTransfered,SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES|SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE ) )
    {
        printf("Error RESET\n");
        exit(1);
    }
    return 0;
}
