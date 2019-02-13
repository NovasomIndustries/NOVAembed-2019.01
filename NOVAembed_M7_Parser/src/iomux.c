#include "../include/NOVAembed_M7_parser.h"

iomux_cfg       *iomux;
iomux_spi       *spi;
iomux_i2c2      *i2c2;

void dump_iomux(void)
{
    printf("\nPeripherals usage ( 1 means IN USE ):\n");
    printf("**********************************************\n");
    printf("spi      : %d\n",iomux->spi);
    printf("i2c2     : %d\n",iomux->i2c2);
    printf("**********************************************\n\n");
}

void copy_and_resize(char *dest,char *source)
{
char *dup = strdup(source);
int i;
    *index(dup,0x0a)=0;
    if ( strlen(dup)<10)
    {
        if (( dup[0] == '0' ) && (dup[1] == 'x' ))
            dup += 2;
        sprintf(dest,"%8s", dup);
        for(i=0;i<8;i++)
            if ( dest[i] == ' ')
                dest[i]='0';
    }
    else
        snprintf(dest,9,"%s",dup+2);
}


void process_spi(void)
{
    spi = calloc(1,sizeof(iomux_spi));
    sprintf(spi->grp_name,"ecspi4grp");
    sprintf(spi->pinctrl0_name,"pinctrl_ecspi4");
    sprintf(spi->mosi_pin_name,"MX6QDL_PAD_EIM_D28__ECSPI4_MOSI");
    sprintf(spi->miso_pin_name,"MX6QDL_PAD_EIM_D22__ECSPI4_MISO");
    sprintf(spi->sclk_pin_name,"MX6QDL_PAD_EIM_D21__ECSPI4_SCLK");
    sprintf(spi->ss0_pin_name, "MX6QDL_PAD_EIM_D29__GPIO3_IO29");
    sprintf(spi->pin_config[0],"100b1");
    sprintf(spi->pin_config[1],"100b1");
    sprintf(spi->pin_config[2],"100b1");
    sprintf(spi->pin_config[3],"100b1");
    return;
}


void process_i2c2(void)
{
    i2c2 = calloc(1,sizeof(iomux_i2c2));
    sprintf(i2c2->grp_name,"i2c2grp");
    sprintf(i2c2->pinctrl0_name,"pinctrl_i2c2");
    sprintf(i2c2->sda_pin_name,"MX6QDL_PAD_EIM_D28__I2C1_SDA");
    sprintf(i2c2->scl_pin_name,"MX6QDL_PAD_EIM_D21__I2C1_SCL");
    sprintf(i2c2->pin_config[0],"4001b8b1");
    sprintf(i2c2->pin_config[1],"4001b8b1");
    return;
}


void parse_iomux(void)
{
    iomux = calloc(1,sizeof(iomux_cfg));

    if ( strstr(file_contents,"M7_GPIO3_A1_comboBox=SPI_TXD"))
    {
        iomux->spi = 1;
        process_spi();
    }


    if ( strstr(file_contents,"M7_GPIO2_D1_comboBox=SDA"))
    {
        process_i2c2();
        iomux->i2c2 = 1;
    }
}

