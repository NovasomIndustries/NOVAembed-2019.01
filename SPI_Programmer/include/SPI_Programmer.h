/******************************************************************************/
/*								Macro and type defines							   */
/******************************************************************************/
/* Helper macros */

#define APP_CHECK_STATUS(exp) {if(exp!=FT_OK){printf("%s:%d:%s(): status(0x%x) \
!= FT_OK\n",__FILE__, __LINE__, __FUNCTION__,exp);exit(1);}else{;}};
#define CHECK_NULL(exp){if(exp==NULL){printf("%s:%d:%s():  NULL expression \
encountered \n",__FILE__, __LINE__, __FUNCTION__);exit(1);}else{;}};

/* Application specific macro definitions */
#define SPI_DEVICE_BUFFER_SIZE		256
#define SPI_WRITE_COMPLETION_RETRY		10
#define START_ADDRESS_EEPROM 	0x00 /*read/write start address inside the EEPROM*/
#define END_ADDRESS_EEPROM		0x10
#define RETRY_COUNT_EEPROM		10	/* number of retries if read/write fails */
#define CHANNEL_TO_OPEN			0	/*0 for first available channel, 1 for next... */
#define SPI_SLAVE_0				0
#define SPI_SLAVE_1				1
#define SPI_SLAVE_2				2
#define DATA_OFFSET				4
#define USE_WRITEREAD			0

#define FAULT_LED_PIN   0x02
#define OE_BUFFER_PIN   0x04
#define RESET_PIN       0x08

#define GPIO_DIR_OUT    0x0E  //ACBUS0 IN, ACBUS1/2/3 OUT

#define PAGE_SIZE_512               0x01
#define PAGE_SIZE_256               0x00

#define OP_BULKERASE                1
#define OP_WRITEBLOB                2
#define OP_READFLASH                3

#define DATA_SIZE                   (4096*1024)

extern  unsigned char Flash_Write(unsigned char *write_buf , int len_data , unsigned int write_address);
extern  unsigned char Flash_Read(unsigned int address, unsigned int len_buff, unsigned char *read_buffer);

extern unsigned char Flash_Read_File(unsigned int address, unsigned int len_buff, unsigned char *read_buffer);
extern unsigned char Init(FT_HANDLE handle);
extern unsigned char flash_busy_and_error_check(void);
extern unsigned char flash_IDR(unsigned char *reg_read);
extern  unsigned char AddressSectorErase(unsigned int address , int bank_size);
extern void flash_SetPageSize(unsigned char page_size);
extern unsigned char flash_WRAR(unsigned int addr_reg_to_read, unsigned char data_to_write);
extern unsigned char flash_RDAR(unsigned int addr_reg_to_read, unsigned char *reg_read);
extern  void special_register_read(unsigned char reg_cmd , unsigned char *val , int len);
extern  unsigned char swreset(void);

extern void test(void);
extern unsigned char SectorErase(unsigned int addr);
extern unsigned char BulkErase(void);

typedef struct file_struct {
    char  *file_name;
    int   address;
    int   max_len;
} ls1012_files;

