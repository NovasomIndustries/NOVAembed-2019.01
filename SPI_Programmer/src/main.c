/*!
 * \file sample-static.c
 *
 * \author FTDI
 * \date 20110512
 *
 * Copyright © 2000-2014 Future Technology Devices International Limited
 *
 *
 * THIS SOFTWARE IS PROVIDED BY FUTURE TECHNOLOGY DEVICES INTERNATIONAL LIMITED ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL FUTURE TECHNOLOGY DEVICES INTERNATIONAL LIMITED
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
    need library: --->>> sudo dnf install glibc-devel.i686 glibc.i686 libstdc++.i686

Table-1: The uniform 64MB memory layout of NOR/QSPI/NAND/SD media on all QorIQ platforms
+-----------------------------+---------|--------------|
|Firmware Definition          | MaxSize | Flash Offset |
|-----------------------------|---------|--------------|
|RCW+PBI                      | 1MB     | 0x00000000   |rcw_1000_default.bin.swapped
|-----------------------------|---------|--------------|
|Boot firmware(U-Boot,UEFI)   | 2MB     | 0x00100000   |uboot
|-----------------------------|---------|--------------|
|Boot firmware Environment    | 1MB     | 0x00300000   |
|-----------------------------|---------|--------------|
|PPA firmware                 | 2MB     | 0x00400000   |ppa_rdb.itb
|-----------------------------|---------|--------------|
|Secure boot headers          | 3MB     | 0x00600000   |
|-----------------------------|---------|--------------|
|DPAA1 FMAN ucode             | 256KB   | 0x00900000   |
|-----------------------------|---------|--------------|
|QE/uQE firmware              | 256KB   | 0x00940000   |
|-----------------------------|---------|--------------|
|Ethernet PHY firmware        | 256KB   | 0x00980000   |pfe_fw_sbl.itb
|-----------------------------|---------|--------------|
|Script for flashing image    | 256KB   | 0x009C0000   |
|-----------------------------|---------|--------------|
|DPAA2-MC or PFE firmware     | 3MB     | 0x00A00000   |
|-----------------------------|---------|--------------|
|DPAA2 DPL                    | 1MB     | 0x00D00000   |
|-----------------------------|---------|--------------|
|DPAA2 DPC                    | 1MB     | 0x00E00000   |
|-----------------------------|---------|--------------|
|Device tree(needed by uefi)  | 1MB     | 0x00F00000   |
|-------------+---------------|---------|--------------|
|Kernel       |               | 16MB    | 0x01000000   |
|-------------|lsdk_linux.itb |---------|--------------|
|Ramdisk rfs  |               | 32MB    | 0x02000000   |
+-------------+---------------+---------|--------------|

run with sudo rmmod ftdi_sio ; sleep 1; LD_LIBRARY_PATH=libs:$LD_LIBRARY_PATH bin/Debug/SPI_Programmer -b
 */

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "ftd2xx.h"
#include "libMPSSE_spi.h"
#include "s25fs512.h"
#include "SPI_Programmer.h"

static FT_HANDLE ftHandle;
unsigned char read_buf[DATA_SIZE];
ChannelConfig channelConf = {0};
static unsigned char AC_BUSbit_Value = 0x0;

ls1012_files flash_files[4] =
{
    {
        .file_name = "/Devel/NOVAsdk2019.01/Utils/nxp/ls1012components/rcw_N1.bin",
        .address = 0x00000000,
        .max_len = (1024*1024)
    },
    {
        .file_name = "/Devel/NOVAsdk2019.01/Blobs/N1_u-boot-dtb.bin",
        .address = 0x00100000,
        .max_len = (2048*1024)
    },
    {
        .file_name = "/Devel/NOVAsdk2019.01/Utils/nxp/ls1012components/ppa.itb",
        .address = 0x00400000,
        .max_len = (2048*1024)
    },
    {
        .file_name = "/Devel/NOVAsdk2019.01/Utils/nxp/ls1012components/pfe_fw_sbl.itb",
        .address = 0x00a00000,
        .max_len = (3072*1024)
    },
};

unsigned char s25fs512_id[8] = {0x01, 0x02, 0x20, 0x4d, 0x00, 0x81, 0x30, 0x31};

void set_OE_Buffer_pin(unsigned char out_value)
{
    if (out_value)
        FT_WriteGPIO(ftHandle, GPIO_DIR_OUT, AC_BUSbit_Value | OE_BUFFER_PIN);
    else
        FT_WriteGPIO(ftHandle, GPIO_DIR_OUT, AC_BUSbit_Value & ~OE_BUFFER_PIN);
}

/*!
 * \brief Fault_Led handling
 *
 * This function handlin Fault_Led pin
 *
 * \param[in] Out value for FAULT_LED_PIN
 * \return none
 * \sa
 * \note
 * \warning
 */
void set_Fault_Led_pin(unsigned char out_value)
{
    if (out_value)
        FT_WriteGPIO(ftHandle, GPIO_DIR_OUT, AC_BUSbit_Value | FAULT_LED_PIN);
    else
        FT_WriteGPIO(ftHandle, GPIO_DIR_OUT, AC_BUSbit_Value & ~FAULT_LED_PIN);

}


/*!
 * \brief Reset pin handling
 *
 * This function handlin Reset pin
 *
 * \param[in] Out value for RESET_PIN
 * \return none
 * \sa
 * \note
 * \warning
 */
void set_Reset_pin(unsigned char out_value)
{
    if (out_value)
        FT_WriteGPIO(ftHandle, GPIO_DIR_OUT, AC_BUSbit_Value | RESET_PIN);
    else
        FT_WriteGPIO(ftHandle, GPIO_DIR_OUT, AC_BUSbit_Value & ~RESET_PIN);

}

/*!
 * \brief Main function / Entry point to the sample application
 *
 * This function is the entry point to the sample application. It opens the channel, writes to the
 * EEPROM and reads back.
 *
 * \param[in] none
 * \return Returns 0 for success
 * \sa
 * \note
 * \warning
 */

int main(int argc, char **argv)
{
unsigned int channels       = 0;
unsigned int i = 0 , j;
unsigned char latency       = 5;
int opflag=0,vflag = 0 , file_len;
char *cvalue = NULL;
unsigned char   rd_special , special_results[64];
int c;
FILE    *fp;
char    strc;

    while ((c = getopt (argc, argv, "bwr:v?zo")) != -1)
    {
        switch (c)
        {
            case 'b':   opflag = OP_BULKERASE;
                        break;
            case 'w':   opflag = OP_WRITEBLOB;
                        //cvalue = optarg;
                        break;
            case 'r':   opflag = OP_READFLASH;
                        cvalue = optarg;
                        break;
            case 'v':   vflag = 1;
                        break;
            case 'z':   opflag = 5;
                        break;
            case 'o':   opflag = 6;
                        break;
            case '?':   printf ("Options ?\nwas %c\n",c);
                        exit (1);
            default:
                        exit(1);
        }
    }
    if ( opflag == 0 )
    {
        printf("Otions are : b (bulk erase )  , w ( write file ), r <file> ( read file ) , v ( verbose )\n");
        printf ("c = %d , opflag = %d , cvalue = %s\n", c , opflag, cvalue);
        exit(1);
    }

    if (( opflag == OP_READFLASH ) && (strlen(cvalue) < 2))
    {
        printf("Unspecified read file!\n");
        exit (1);
    }

	FT_STATUS status = FT_OK;
	FT_DEVICE_LIST_INFO_NODE devList = {0};

	channelConf.ClockRate = 3000000;//3000000
	channelConf.LatencyTimer = latency;
	channelConf.configOptions = SPI_CONFIG_OPTION_MODE0 | SPI_CONFIG_OPTION_CS_DBUS3 | SPI_CONFIG_OPTION_CS_ACTIVELOW;
	channelConf.Pin = 0;    /*  FinalVal-FinalDir-InitVal-InitDir (for dir 0=in, 1=out) */

	status = SPI_GetNumChannels((uint32 *)&channels);
	APP_CHECK_STATUS(status);
	printf("Number of available SPI channels = %d , status= 0x%08x\n",(int)channels,status);

    if(channels > 0)
    {
        for(i=0;i<channels;i++)
        {
            status = SPI_GetChannelInfo(i,&devList);
            APP_CHECK_STATUS(status);
            if ( vflag == 1 )
            {
                printf("Information on channel number %d:\n",i);
                printf("		Flags=0x%x\n",devList.Flags);
                printf("		Type=0x%x\n",devList.Type);
                printf("		ID=0x%x\n",devList.ID);
                printf("		LocId=0x%x\n",devList.LocId);
                printf("		SerialNumber=%s\n",devList.SerialNumber);
                printf("		Description=%s\n",devList.Description);
                printf("		ftHandle=0x%x\n",(unsigned int)devList.ftHandle);/*is 0 unless open*/
            }
        }
    }
    else
    {
        printf("No channels found!\n");
        exit (1);
    }

    /* Open the first available channel */
    status = SPI_OpenChannel(CHANNEL_TO_OPEN,&ftHandle);
    APP_CHECK_STATUS(status);
    if ( vflag == 1 )
        printf("handle=0x%x status=0x%x\n",(unsigned int)ftHandle,status);
    status = SPI_InitChannel(ftHandle,&channelConf);
    APP_CHECK_STATUS(status);

    Init(ftHandle);
    status = SPI_Write(ftHandle, read_buf, 0, (uint32 *)&i,SPI_TRANSFER_OPTIONS_SIZE_IN_BITS|SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
    APP_CHECK_STATUS(status);

    if ( opflag == 5)
        set_OE_Buffer_pin(0);
    if ( opflag == 6)
        set_OE_Buffer_pin(1);
    sleep (2);

    set_Reset_pin(0);
    set_OE_Buffer_pin(0);
    set_Fault_Led_pin(0);

    usleep(10000);

    if ( vflag == 1 )
    {
        special_register_read(PPBL_CMD,special_results,1);
        printf("PPBL_CMD    : 0x%02x\n",special_results[0]);
        special_register_read(PPBAR_CMD,special_results,1);
        printf("PPBAR_CMD   : 0x%02x\n",special_results[0]);
        special_register_read(DYBAR_CMD,special_results,1);
        printf("DYBAR_CMD   : 0x%02x\n",special_results[0]);
        special_register_read(PASSWDR_CMD,special_results,8);
        printf("PASSWDR_CMD : ");
        printf("0x");
        for(i=0;i<8;i++)
            printf("%02x",special_results[i]);
        printf("\n");

        special_register_read(ASPR_CMD,special_results,2);
        printf("ASPR_CMD   : 0x%02x%02x\n",special_results[0],special_results[1]);

        flash_RDAR(SR1V, &rd_special);
        printf("SR1V : 0x%02x\n",rd_special);
        flash_RDAR(SR2V, &rd_special);
        printf("SR2V : 0x%02x\n",rd_special);
        flash_RDAR(CR1V, &rd_special);
        printf("CR1V : 0x%02x\n",rd_special);
        flash_RDAR(CR2V, &rd_special);
        printf("CR2V : 0x%02x\n",rd_special);
        flash_RDAR(CR3V, &rd_special);
        printf("CR3V : 0x%02x\n",rd_special);
        flash_RDAR(CR4V, &rd_special);
        printf("CR4V : 0x%02x\n",rd_special);
        flash_RDAR(VDLR, &rd_special);
        printf("VDLR : 0x%02x\n",rd_special);
        flash_RDAR(PPBL, &rd_special);
        printf("PPBL : 0x%02x\n",rd_special);
    }

    flash_IDR(read_buf);
    for (i=0;i<8;i++)
    {
        if ( read_buf[i] != s25fs512_id[i])
        {
            printf("Expected 0x%02x%02x%02x%02x%02x%02x%02x%02x\n",s25fs512_id[0],s25fs512_id[1],s25fs512_id[2],s25fs512_id[3],s25fs512_id[4],s25fs512_id[5],s25fs512_id[6],s25fs512_id[7] );
            printf("Got      0x%02x%02x%02x%02x%02x%02x%02x%02x\n",read_buf[0],read_buf[1],read_buf[2],read_buf[3],read_buf[4],read_buf[5],read_buf[6],read_buf[7] );
            printf("Wrong flash type\n");
            set_Fault_Led_pin(1);
            exit(1);
        }
    }
    flash_WRAR(CR2V, 0x88);     // Remove dummy byte
    flash_WRAR(CR3V, 0x3b);     //Setup page 512

    usleep(1000);

    switch ( opflag )
    {
        case OP_BULKERASE :
                                printf("OP_BULKERASE\n");
                                BulkErase();
                                printf("OP_BULKERASE cmd sent\n");
                                while (flash_busy_and_error_check())
                                {
                                    switch (strc)
                                    {
                                        case '|' : strc = '/';break;
                                        case '/' : strc = '-';break;
                                        case '-' : strc = '\\';break;
                                        case '\\' : strc = '|';break;
                                        default : strc = '|' ; break;
                                    }
                                    printf("%c\r", strc); fflush(stdout);
                                    usleep(500000);
                                }
                                break;

        case OP_WRITEBLOB :
                                printf("OP_WRITEBLOB\n");
                                for(i=0;i<4;i++)
                                {
                                    for(j=0;j<flash_files[i].max_len;j++)
                                        read_buf[j] = 0xff;
                                    fp = fopen(flash_files[i].file_name, "rb");
                                    if ( fp )
                                    {
                                        file_len = fread(read_buf, 1 , flash_files[i].max_len, fp);
                                        fclose(fp);
                                    }
                                    else
                                    {
                                        printf("File %s not found\n",flash_files[i].file_name);
                                        exit(1);
                                    }
                                    /*
                                    if (AddressSectorErase(flash_files[i].address , flash_files[i].max_len) )
                                    {
                                        printf("Flash_Write error\n");
                                        exit(1);
                                    }
                                    */
                                    printf("Writing file %s at address 0x%08x\n",flash_files[i].file_name,flash_files[i].address);
                                    if (Flash_Write(read_buf , file_len , flash_files[i].address ) )
                                    {
                                        printf("Flash_Write error\n");
                                        exit(1);
                                    }
                                    printf("File %s written at address 0x%08x\n",flash_files[i].file_name,flash_files[i].address);
                                }
                                break;

        case OP_READFLASH :
                                printf("OP_READFLASH %d\n",DATA_SIZE);
                                for(j=0;j<DATA_SIZE;j++)
                                    read_buf[j] = 0xff;
                                //if (Flash_Read(0x00, (unsigned int )DATA_SIZE, read_buf))
                                if (Flash_Read(0x00, 8192, read_buf))
                                {
                                    printf("\n Error to read a Flash!!!\n");
                                    exit(1);
                                }
                                fp = fopen(cvalue, "wb");
                                fwrite(read_buf, sizeof(read_buf), 1, fp);
                                fclose(fp);
                                break;

        default :               exit (1);

    }

    set_Reset_pin(1);       //reset pin floating
    set_OE_Buffer_pin(1);   //enable outbut buffer
    set_Fault_Led_pin(1);
    status = SPI_CloseChannel(ftHandle);
    return 0;
}

