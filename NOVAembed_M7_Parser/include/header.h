#define dts_header "\n\
/dts-v1/;\n\
#include \"rk3328.dtsi\"\n\
#include <dt-bindings/input/input.h>\n\
\n\
/ {\n\
	model = \"Rockchip RK3328 EVB\";\n\
	compatible = \"rockchip,rk3328-evb\", \"rockchip,rk3328\";\n\
    chosen {\n\
		bootargs = \"rockchip_jtag earlyprintk=uart8250-32bit,0xff130000\";\n\
	};\n\
\n\
	fiq-debugger {\n\
		compatible = \"rockchip,fiq-debugger\";\n\
		rockchip,serial-id = <2>;\n\
		rockchip,signal-irq = <159>;\n\
		rockchip,wake-irq = <0>;\n\
		/* If enable uart uses irq instead of fiq */\n\
		rockchip,irq-mode-enable = <0>;\n\
		rockchip,baudrate = <115200>;  /* Only 115200 and 1500000 */\n\
		interrupts = <GIC_SPI 127 IRQ_TYPE_LEVEL_LOW>;\n\
		status = \"okay\";\n\
	};\n\
\n\
	leds {\n\
		compatible = \"gpio-leds\";\n\
		wlan-green {\n\
			label = \"wlan\";\n\
			gpios = <&rk805 2 GPIO_ACTIVE_LOW>;\n\
			linux,default-trigger = \"heartbeat\";\n\
			default-state = \"off\";\n\
			mode = <0x05>;\n\
		}; /* \"rc-feedback\" */\n\
		ir-fbk {\n\
            label = \"rc-feedback\";\n\
            gpios = <&gpio2 18 GPIO_ACTIVE_LOW>;\n\
            linux,default-trigger = \"rc-feedback\";\n\
            pinctrl-names = \"default\";\n\
            pinctrl-0 = <&ir_fbk>;\n\
            default-state = \"off\";\n\
		};\n\
	};\n\
\n\
    ir: ir-receiver {\n\
        compatible = \"gpio-ir-receiver\";\n\
        gpios = <&gpio2 2 GPIO_ACTIVE_LOW>;\n\
        pinctrl-names = \"default\";\n\
        pinctrl-0 = <&ir_int>;\n\
    };\n\
\n\
	gmac_clkin: external-gmac-clock {\n\
        compatible = \"fixed-clock\";\n\
        clock-frequency = <125000000>;\n\
        clock-output-names = \"gmac_clkin\";\n\
        #clock-cells = <0>;\n\
	};\n\
\n\
	sdio_pwrseq: sdio-pwrseq {\n\
		compatible = \"mmc-pwrseq-simple\";\n\
		pinctrl-names = \"default\";\n\
		pinctrl-0 = <&wifi_enable_h>;\n\
		reset-gpios = <&gpio1 18 GPIO_ACTIVE_LOW>;\n\
	};\n\
\n\
	sound {\n\
		compatible = \"simple-audio-card\";\n\
		simple-audio-card,format = \"i2s\";\n\
		simple-audio-card,mclk-fs = <256>;\n\
		simple-audio-card,name = \"rockchip,rk3328\";\n\
		simple-audio-card,cpu {\n\
			sound-dai = <&i2s1>;\n\
		};\n\
		simple-audio-card,codec {\n\
			sound-dai = <&codec>;\n\
		};\n\
	};\n\
\n\
	hdmi-sound {\n\
		compatible = \"simple-audio-card\";\n\
		simple-audio-card,format = \"i2s\";\n\
		simple-audio-card,mclk-fs = <128>;\n\
		simple-audio-card,name = \"rockchip,hdmi\";\n\
		simple-audio-card,cpu {\n\
			sound-dai = <&i2s0>;\n\
		};\n\
		simple-audio-card,codec {\n\
			sound-dai = <&hdmi>;\n\
		};\n\
	};\n\
\n\
	spdif-sound {\n\
		compatible = \"simple-audio-card\";\n\
		simple-audio-card,name = \"rockchip,spdif\";\n\
		simple-audio-card,cpu {\n\
			sound-dai = <&spdif>;\n\
		};\n\
		simple-audio-card,codec {\n\
			sound-dai = <&spdif_out>;\n\
		};\n\
	};\n\
\n\
	spdif_out: spdif-out {\n\
		compatible = \"linux,spdif-dit\";\n\
		#sound-dai-cells = <0>;\n\
	};\n\
\n\
	vcc_host_vbus: host-vbus-regulator {\n\
		compatible = \"regulator-fixed\";\n\
		gpio = <&gpio0 RK_PA0 GPIO_ACTIVE_HIGH>;\n\
		pinctrl-names = \"default\";\n\
		pinctrl-0 = <&host_vbus_drv>;\n\
		regulator-name = \"vcc_host_vbus\";\n\
		regulator-min-microvolt = <5000000>;\n\
		regulator-max-microvolt = <5000000>;\n\
		enable-active-high;\n\
	};\n\
\n\
	vcc_otg_vbus: otg-vbus-regulator {\n\
		compatible = \"regulator-fixed\";\n\
		gpio = <&gpio0 RK_PD3 GPIO_ACTIVE_HIGH>;\n\
		pinctrl-names = \"default\";\n\
		pinctrl-0 = <&otg_vbus_drv>;\n\
		regulator-name = \"vcc_otg_vbus\";\n\
		regulator-min-microvolt = <5000000>;\n\
		regulator-max-microvolt = <5000000>;\n\
		enable-active-high;\n\
	};\n\
\n\
	vcc_phy: vcc-phy-regulator {\n\
		compatible = \"regulator-fixed\";\n\
		regulator-name = \"vcc_phy\";\n\
		regulator-always-on;\n\
		regulator-boot-on;\n\
	};\n\
\n\
	vcc_sd: sdmmc-regulator {\n\
		compatible = \"regulator-fixed\";\n\
		gpio = <&gpio0 30 GPIO_ACTIVE_LOW>;\n\
		pinctrl-names = \"default\";\n\
		pinctrl-0 = <&sdmmc0m1_gpio>;\n\
		regulator-name = \"vcc_sd\";\n\
		regulator-min-microvolt = <3300000>;\n\
		regulator-max-microvolt = <3300000>;\n\
		vin-supply = <&vcc_io>;\n\
	};\n\
\n\
	wireless-bluetooth {\n\
                compatible = \"bluetooth-platdata\";\n\
                clocks = <&rk805 1>;\n\
                clock-names = \"ext_clock\";\n\
                uart_rts_gpios = <&gpio1 10 GPIO_ACTIVE_LOW>;\n\
                pinctrl-names = \"default\", \"rts_gpio\";\n\
                pinctrl-0 = <&uart0_rts>;\n\
                pinctrl-1 = <&uart0_gpios>;\n\
                BT,power_gpio = <&gpio1 21 GPIO_ACTIVE_HIGH>;\n\
                BT,wake_host_irq = <&gpio1 26 GPIO_ACTIVE_HIGH>;\n\
                status = \"okay\";\n\
	};\n\
\n\
	wireless-wlan {\n\
		compatible = \"wlan-platdata\";\n\
		rockchip,grf = <&grf>;\n\
		wifi_chip_type = \"rtl8723bs\";\n\
		sdio_vref = <3300>;\n\
		WIFI,host_poweren_gpio = <&gpio1 18 GPIO_ACTIVE_HIGH>;\n\
		WIFI,host_wake_irq = <&gpio1 19 GPIO_ACTIVE_HIGH>;\n\
		pinctrl-names = \"default\";\n\
		pinctrl-0 = <&bt_clk>;\n\
		status = \"okay\";\n\
	};\n\
};\n\
\n\
&codec {\n\
	#sound-dai-cells = <0>;\n\
	status = \"okay\";\n\
};\n\
\n\
&cpu0 {\n\
	cpu-supply = <&vdd_arm>;\n\
};\n\
\n\
&dfi {\n\
	status = \"okay\";\n\
};\n\
\n\
&dmc {\n\
	center-supply = <&vdd_logic>;\n\
	status = \"okay\";\n\
};\n\
\n\
&emmc {\n\
	bus-width = <8>;\n\
	cap-mmc-highspeed;\n\
	mmc-hs200-1_8v;\n\
	supports-emmc;\n\
	disable-wp;\n\
	non-removable;\n\
	num-slots = <1>;\n\
	pinctrl-names = \"default\";\n\
	pinctrl-0 = <&emmc_clk &emmc_cmd &emmc_bus8>;\n\
	status = \"okay\";\n\
};\n\
\n\
&gmac2io {\n\
	status = \"disabled\";\n\
};\n\
\n\
&gmac2phy {\n\
	phy-supply = <&vcc_phy>;\n\
	clock_in_out = \"output\";\n\
	assigned-clocks = <&cru SCLK_MAC2PHY_SRC>;\n\
	assigned-clock-rate = <50000000>;\n\
	assigned-clocks = <&cru SCLK_MAC2PHY>;\n\
	assigned-clock-parents = <&cru SCLK_MAC2PHY_SRC>;\n\
	status = \"okay\";\n\
};\n\
\n\
&gpu {\n\
	status = \"okay\";\n\
	mali-supply = <&vdd_logic>;\n\
};\n\
\n\
&hdmi {\n\
	#sound-dai-cells = <0>;\n\
	ddc-i2c-scl-high-time-ns = <9625>;\n\
	ddc-i2c-scl-low-time-ns = <10000>;\n\
	status = \"okay\";\n\
};\n\
\n\
&hdmiphy {\n\
	status = \"okay\";\n\
};\n\
\n\
&i2c1 {\n\
	status = \"okay\";\n\
\n\
	rk805: rk805@18 {\n\
		compatible = \"rockchip,rk805\";\n\
		status = \"okay\";\n\
		reg = <0x18>;\n\
		interrupt-parent = <&gpio2>;\n\
		interrupts = <6 IRQ_TYPE_LEVEL_LOW>;\n\
		pinctrl-names = \"default\";\n\
		pinctrl-0 = <&pmic_int_l>;\n\
		wakeup-source;\n\
		gpio-controller;\n\
		#gpio-cells = <2>;\n\
		#clock-cells = <1>;\n\
		clock-output-names = \"rk805-clkout1\", \"rk805-clkout2\";\n\
\n\
		rtc {\n\
			status = \"okay\";\n\
		};\n\
\n\
		pwrkey {\n\
			status = \"disabled\";\n\
		};\n\
\n\
        rk805_default: pinmux {\n\
            gpio01 {\n\
                pins = \"gpio0\", \"gpio1\";\n\
                function = \"gpio\";\n\
                output-high;\n\
                };\n\
        };\n\
\n\
		gpio {\n\
			status = \"okay\";\n\
		};\n\
\n\
		regulators {\n\
			compatible = \"rk805-regulator\";\n\
			status = \"okay\";\n\
			#address-cells = <1>;\n\
			#size-cells = <0>;\n\
\n\
			vdd_logic: RK805_DCDC1 {\n\
				regulator-compatible = \"RK805_DCDC1\";\n\
				regulator-name = \"vdd_logic\";\n\
				regulator-min-microvolt = <712500>;\n\
				regulator-max-microvolt = <1450000>;\n\
				regulator-initial-mode = <0x1>;\n\
				regulator-ramp-delay = <12500>;\n\
				regulator-boot-on;\n\
				regulator-always-on;\n\
				regulator-state-mem {\n\
					regulator-mode = <0x2>;\n\
					regulator-on-in-suspend;\n\
					regulator-suspend-microvolt = <1000000>;\n\
				};\n\
			};\n\
\n\
			vdd_arm: RK805_DCDC2 {\n\
				regulator-compatible = \"RK805_DCDC2\";\n\
				regulator-name = \"vdd_arm\";\n\
				regulator-init-microvolt = <1225000>;\n\
				regulator-min-microvolt = <712500>;\n\
				regulator-max-microvolt = <1450000>;\n\
				regulator-initial-mode = <0x1>;\n\
				regulator-ramp-delay = <12500>;\n\
				regulator-boot-on;\n\
				regulator-always-on;\n\
				regulator-state-mem {\n\
					regulator-mode = <0x2>;\n\
					regulator-on-in-suspend;\n\
					regulator-suspend-microvolt = <950000>;\n\
				};\n\
			};\n\
\n\
			vcc_ddr: RK805_DCDC3 {\n\
				regulator-compatible = \"RK805_DCDC3\";\n\
				regulator-name = \"vcc_ddr\";\n\
				regulator-initial-mode = <0x1>;\n\
				regulator-boot-on;\n\
				regulator-always-on;\n\
				regulator-state-mem {\n\
					regulator-mode = <0x2>;\n\
					regulator-on-in-suspend;\n\
				};\n\
			};\n\
\n\
			vcc_io: RK805_DCDC4 {\n\
				regulator-compatible = \"RK805_DCDC4\";\n\
				regulator-name = \"vcc_io\";\n\
				regulator-min-microvolt = <3300000>;\n\
				regulator-max-microvolt = <3300000>;\n\
				regulator-initial-mode = <0x1>;\n\
				regulator-boot-on;\n\
				regulator-always-on;\n\
				regulator-state-mem {\n\
					regulator-mode = <0x2>;\n\
					regulator-on-in-suspend;\n\
					regulator-suspend-microvolt = <3300000>;\n\
				};\n\
			};\n\
\n\
			vdd_18: RK805_LDO1 {\n\
				regulator-compatible = \"RK805_LDO1\";\n\
				regulator-name = \"vdd_18\";\n\
				regulator-min-microvolt = <1800000>;\n\
				regulator-max-microvolt = <1800000>;\n\
				regulator-boot-on;\n\
				regulator-always-on;\n\
				regulator-state-mem {\n\
					regulator-on-in-suspend;\n\
					regulator-suspend-microvolt = <1800000>;\n\
				};\n\
			};\n\
\n\
			vcc_18emmc: RK805_LDO2 {\n\
				regulator-compatible = \"RK805_LDO2\";\n\
				regulator-name = \"vcc_18emmc\";\n\
				regulator-min-microvolt = <1800000>;\n\
				regulator-max-microvolt = <1800000>;\n\
				regulator-boot-on;\n\
				regulator-always-on;\n\
				regulator-state-mem {\n\
					regulator-on-in-suspend;\n\
					regulator-suspend-microvolt = <1800000>;\n\
				};\n\
			};\n\
\n\
			vdd_11: RK805_LDO3 {\n\
				regulator-compatible = \"RK805_LDO3\";\n\
				regulator-name = \"vdd_11\";\n\
				regulator-min-microvolt = <1100000>;\n\
				regulator-max-microvolt = <1100000>;\n\
				regulator-boot-on;\n\
				regulator-always-on;\n\
				regulator-state-mem {\n\
					regulator-on-in-suspend;\n\
					regulator-suspend-microvolt = <1100000>;\n\
				};\n\
			};\n\
		};\n\
	};\n\
};\n\
\n\
&h265e {\n\
	status = \"okay\";\n\
};\n\
\n\
&i2s0 {\n\
	#sound-dai-cells = <0>;\n\
	rockchip,bclk-fs = <128>;\n\
	status = \"okay\";\n\
};\n\
\n\
&i2s1 {\n\
	#sound-dai-cells = <0>;\n\
	status = \"okay\";\n\
};\n\
\n\
&iep {\n\
	status = \"okay\";\n\
};\n\
\n\
&iep_mmu {\n\
	status = \"okay\";\n\
};\n\
\n\
&io_domains {\n\
	status = \"okay\";\n\
\n\
	vccio1-supply = <&vcc_io>;\n\
	vccio2-supply = <&vcc_18emmc>;\n\
	vccio3-supply = <&vcc_io>;\n\
	vccio4-supply = <&vdd_18>;\n\
	vccio5-supply = <&vcc_io>;\n\
	vccio6-supply = <&vcc_io>;\n\
	pmuio-supply = <&vcc_io>;\n\
};\n\
\n\
&pinctrl {\n\
        clkout32k {\n\
                bt_clk: bt-clk {\n\
                rockchip,pins =\n\
                        <1 RK_PD4 RK_FUNC_1 &pcfg_pull_up>;\n\
                };\n\
        }; \n\
	pmic {\n\
		pmic_int_l: pmic-int-l {\n\
		rockchip,pins =\n\
			<2 RK_PA6 RK_FUNC_GPIO &pcfg_pull_up>;\n\
		};\n\
	};\n\
\n\
	sdio-pwrseq {\n\
		wifi_enable_h: wifi-enable-h {\n\
		rockchip,pins =\n\
			<1 18 RK_FUNC_GPIO &pcfg_pull_none>;\n\
		};\n\
	};\n\
\n\
	usb {\n\
		host_vbus_drv: host-vbus-drv {\n\
			rockchip,pins =\n\
				<0 RK_PA0 RK_FUNC_GPIO &pcfg_pull_none>;\n\
		};\n\
\n\
		otg_vbus_drv: otg-vbus-drv {\n\
			rockchip,pins =\n\
				<0 RK_PD3 RK_FUNC_GPIO &pcfg_pull_none>;\n\
		};\n\
	};\n\
\n\
	bt-clock {\n\
		blue_tooth_clock: blue-tooth-clock {\n\
		rockchip,pins =\n\
			<1 RK_PD4 RK_FUNC_2 &pcfg_pull_none>;\n\
		};\n\
	};\n\
	wireless-bluetooth {\n\
		uart0_gpios: uart0-gpios {\n\
		rockchip,pins =\n\
			<1 10 RK_FUNC_GPIO &pcfg_pull_none>;\n\
		};\n\
	};\n\
};\n\
\n\
&pwm3 {\n\
	status = \"okay\";\n\
	compatible = \"rockchip,remotectl-pwm\";\n\
	remote_pwm_id = <3>;\n\
	handle_cpu_id = <1>;\n\
	remote_support_psci = <1>;\n\
\n\
	ir_key1 {\n\
		rockchip,usercode = <0x4040>;\n\
		rockchip,key_table =\n\
			<0xf2	KEY_REPLY>,\n\
			<0xba	KEY_BACK>,\n\
			<0xf4	KEY_UP>,\n\
			<0xf1	KEY_DOWN>,\n\
			<0xef	KEY_LEFT>,\n\
			<0xee	KEY_RIGHT>,\n\
			<0xbd	KEY_HOME>,\n\
			<0xea	KEY_VOLUMEUP>,\n\
			<0xe3	KEY_VOLUMEDOWN>,\n\
			<0xe2	KEY_SEARCH>,\n\
			<0xb2	KEY_POWER>,\n\
			<0xbc	KEY_MUTE>,\n\
			<0xec	KEY_MENU>,\n\
			<0xbf	0x190>,\n\
			<0xe0	0x191>,\n\
			<0xe1	0x192>,\n\
			<0xe9	183>,\n\
			<0xe6	248>,\n\
			<0xe8	185>,\n\
			<0xe7	186>,\n\
			<0xf0	388>,\n\
			<0xbe	0x175>;\n\
	};\n\
\n\
	ir_key2 {\n\
		rockchip,usercode = <0xff00>;\n\
		rockchip,key_table =\n\
			<0xf9	KEY_HOME>,\n\
			<0xbf	KEY_BACK>,\n\
			<0xfb	KEY_MENU>,\n\
			<0xaa	KEY_REPLY>,\n\
			<0xb9	KEY_UP>,\n\
			<0xe9	KEY_DOWN>,\n\
			<0xb8	KEY_LEFT>,\n\
			<0xea	KEY_RIGHT>,\n\
			<0xeb	KEY_VOLUMEDOWN>,\n\
			<0xef	KEY_VOLUMEUP>,\n\
			<0xf7	KEY_MUTE>,\n\
			<0xe7	KEY_POWER>,\n\
			<0xfc	KEY_POWER>,\n\
			<0xa9	KEY_VOLUMEDOWN>,\n\
			<0xa8	KEY_PLAYPAUSE>,\n\
			<0xe0	KEY_VOLUMEDOWN>,\n\
			<0xa5	KEY_VOLUMEDOWN>,\n\
			<0xab	183>,\n\
			<0xb7	388>,\n\
			<0xe8	388>,\n\
			<0xf8	184>,\n\
			<0xaf	185>,\n\
			<0xed	KEY_VOLUMEDOWN>,\n\
			<0xee	186>,\n\
			<0xb3	KEY_VOLUMEDOWN>,\n\
			<0xf1	KEY_VOLUMEDOWN>,\n\
			<0xf2	KEY_VOLUMEDOWN>,\n\
			<0xf3	KEY_SEARCH>,\n\
			<0xb4	KEY_VOLUMEDOWN>,\n\
			<0xa4	KEY_SETUP>,\n\
			<0xbe	KEY_SEARCH>;\n\
	};\n\
\n\
	ir_key3 {\n\
		rockchip,usercode = <0x1dcc>;\n\
		rockchip,key_table =\n\
			<0xee	KEY_REPLY>,\n\
			<0xf0	KEY_BACK>,\n\
			<0xf8	KEY_UP>,\n\
			<0xbb	KEY_DOWN>,\n\
			<0xef	KEY_LEFT>,\n\
			<0xed	KEY_RIGHT>,\n\
			<0xfc	KEY_HOME>,\n\
			<0xf1	KEY_VOLUMEUP>,\n\
			<0xfd	KEY_VOLUMEDOWN>,\n\
			<0xb7	KEY_SEARCH>,\n\
			<0xff	KEY_POWER>,\n\
			<0xf3	KEY_MUTE>,\n\
			<0xbf	KEY_MENU>,\n\
			<0xf9	0x191>,\n\
			<0xf5	0x192>,\n\
			<0xb3	388>,\n\
			<0xbe	KEY_1>,\n\
			<0xba	KEY_2>,\n\
			<0xb2	KEY_3>,\n\
			<0xbd	KEY_4>,\n\
			<0xf9	KEY_5>,\n\
			<0xb1	KEY_6>,\n\
			<0xfc	KEY_7>,\n\
			<0xf8	KEY_8>,\n\
			<0xb0	KEY_9>,\n\
			<0xb6	KEY_0>,\n\
			<0xb5	KEY_BACKSPACE>;\n\
	};\n\
};\n\
\n\
&rga {\n\
	status = \"okay\";\n\
};\n\
\n\
&rkvdec {\n\
	status = \"okay\";\n\
	vcodec-supply = <&vdd_logic>;\n\
	/* COMMENT */\n\
};\n\
\n\
&uart0 {\n\
	pinctrl-names = \"default\";\n\
	pinctrl-0 = <&uart0_xfer &uart0_cts>;\n\
	status = \"okay\";\n\
};\n\
\n\
&sdio {\n\
	bus-width = <4>;\n\
	cap-sd-highspeed;\n\
	cap-sdio-irq;\n\
	disable-wp;\n\
	keep-power-in-suspend;\n\
	max-frequency = <25000000>;\n\
	mmc-pwrseq = <&sdio_pwrseq>;\n\
	non-removable;\n\
	num-slots = <1>;\n\
	pinctrl-names = \"default\";\n\
	pinctrl-0 = <&sdmmc1_bus4 &sdmmc1_cmd &sdmmc1_clk>;\n\
	supports-sdio;\n\
    broken-cd;	\n\
	sd-uhs-sdr104;\n\
	status = \"okay\";\n\
};\n\
\n\
&sdmmc {\n\
	bus-width = <4>;\n\
	cap-mmc-highspeed;\n\
	cap-sd-highspeed;\n\
	disable-wp;\n\
	max-frequency = <150000000>;\n\
	num-slots = <1>;\n\
	pinctrl-names = \"default\";\n\
	pinctrl-0 = <&sdmmc0_clk &sdmmc0_cmd &sdmmc0_dectn &sdmmc0_bus4>;\n\
	supports-sd;\n\
	status = \"okay\";\n\
	vmmc-supply = <&vcc_sd>;\n\
};\n\
\n\
&spdif {\n\
	#sound-dai-cells = <0>;\n\
	pinctrl-names = \"default\";\n\
	pinctrl-0 = <&spdifm2_tx>;\n\
	status = \"okay\";\n\
};\n\
\n\
&threshold {\n\
	temperature = <90000>; /* millicelsius */\n\
};\n\
\n\
&target {\n\
	temperature = <105000>; /* millicelsius */\n\
};\n\
\n\
&soc_crit {\n\
	temperature = <110000>; /* millicelsius */\n\
};\n\
\n\
&tsadc {\n\
	rockchip,hw-tshut-temp = <120000>;\n\
	status = \"okay\";\n\
};\n\
\n\
&tve {\n\
	status = \"okay\";\n\
};\n\
\n\
&u2phy {\n\
	status = \"okay\";\n\
\n\
	u2phy_host: host-port {\n\
		status = \"okay\";\n\
	};\n\
\n\
	u2phy_otg: otg-port {\n\
		vbus-supply = <&vcc_otg_vbus>;\n\
		status = \"okay\";\n\
	};\n\
};\n\
\n\
&u3phy {\n\
	vbus-supply = <&vcc_host_vbus>;\n\
	status = \"okay\";\n\
};\n\
\n\
&u3phy_utmi {\n\
	status = \"okay\";\n\
};\n\
\n\
&u3phy_pipe {\n\
	status = \"okay\";\n\
};\n\
\n\
&usb20_otg {\n\
	status = \"okay\";\n\
};\n\
\n\
&usb_host0_ehci {\n\
	status = \"okay\";\n\
};\n\
\n\
&usb_host0_ohci {\n\
	status = \"okay\";\n\
};\n\
\n\
&usbdrd3 {\n\
	status = \"okay\";\n\
};\n\
\n\
&usbdrd_dwc3 {\n\
	status = \"okay\";\n\
};\n\
\n\
&vepu {\n\
	status = \"okay\";\n\
};\n\
\n\
&vop {\n\
	status = \"okay\";\n\
};\n\
\n\
&vop_mmu {\n\
	status = \"okay\";\n\
};\n\
\n\
&vpu_service {\n\
	status = \"okay\";\n\
};\n\
"

#define i2c2_defs_top "\n\
&i2c2 {\n\
	status = \"okay\";\n\
"

#define i2c2_defs_bottom "\n\
        polytouch1: eeti@04 {\n\
                compatible = \"eeti,egalax_ts\";\n\
                reg = <0x04>;\n\
                pinctrl-names = \"default\";\n\
                pinctrl-0 = <&pinctrl_i2ctouch_irq>;\n\
                interrupt-parent = <&gpio3>;\n\
                interrupts = <4 0>;\n\
                wakeup-gpios = <&gpio4 31 GPIO_ACTIVE_HIGH>;\n\
                linux,wakeup;\n\
        };\n\
        polytouch2: edt-ft5x06@38 {\n\
                compatible = \"edt,edt-ft5x06\";\n\
                reg = <0x38>;\n\
                pinctrl-names = \"default\";\n\
                pinctrl-0 = <&pinctrl_i2ctouch_irq>;\n\
                interrupt-parent = <&gpio3>;\n\
                interrupts = <4 0>;\n\
                reset-gpios = <&gpio4 30 GPIO_ACTIVE_LOW>;\n\
                wake-gpios = <&gpio4 31 GPIO_ACTIVE_HIGH>;\n\
        };\n\
        polytouch3: gt911@5d {\n\
                compatible = \"goodix,gt911\";\n\
                pinctrl-names = \"default\";\n\
                pinctrl-0 = <&pinctrl_i2ctouch_irq>;\n\
                reg = <0x5d>;\n\
                substitute-i2c-address = <0x2c>;\n\
                esd-recovery-timeout-ms = <2000>;\n\
                interrupt-parent = <&gpio3>;\n\
                interrupts-extended = <&gpio3 4 IRQ_TYPE_LEVEL_HIGH>;\n\
                irq-gpios = <&gpio3 4 GPIO_ACTIVE_HIGH>;\n\
                reset-gpios = <&gpio4 30 GPIO_ACTIVE_LOW>;\n\
        };\n\
        polytouch4: ilitek@41 {\n\
                compatible = \"ilitek_ts\";\n\
                reg = <0x41>;\n\
                pinctrl-names = \"default\";\n\
                pinctrl-0 = <&pinctrl_i2ctouch_irq>;\n\
                interrupt-parent = <&gpio1>;\n\
                interrupts = <6 0>;\n\
                ilitek,irq-gpio = <&gpio1 6 GPIO_ACTIVE_HIGH>;\n\
                ilitek,reset-gpio = <&gpio4 30 GPIO_ACTIVE_LOW>;\n\
        };\n\
        touchscreen: tsc2007@48 {\n\
                compatible = \"ti,tsc2007\";\n\
                reg = <0x48>;\n\
                pinctrl-names = \"default\";\n\
                pinctrl-0 = <&pinctrl_tsc2007>;\n\
                interrupt-parent = <&gpio4>;\n\
                interrupts = <23 0>;\n\
                gpios = <&gpio4 23 GPIO_ACTIVE_LOW>;\n\
                ti,x-plate-ohms = <660>;\n\
                linux,wakeup;\n\
        };\n\
\n\
"

#define i2c2_footer "\n\
};\n\
"


#define spi_defs "\n\
&spi {\n\n\
        status = \"okay\";\n\
};\n\
"

#define spi_pins "\n\
/* PLACEHOLDER PLACEHOLDER : spi is enabled PLACEHOLDER PLACEHOLDER*/ \n\
"

#define dts_footer "\n\
};\n\
"
