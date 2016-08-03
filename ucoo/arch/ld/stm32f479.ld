MEMORY
{
	rom (rx) : ORIGIN = 0x08000000, LENGTH = 2048K
	ccm (rwx) : ORIGIN = 0x10000000, LENGTH = 64K
	/* SRAM1: 112K
	 * SRAM2: 16K
	 * SRAM3: 128K */
	ram (rwx) : ORIGIN = 0x20000000, LENGTH = 256K
}

REGION_ALIAS("nvrammem", rom);
REGION_ALIAS("heapmem", ram);

INCLUDE common.ld

PROVIDE(_stack = ORIGIN(ccm) + LENGTH(ccm));
