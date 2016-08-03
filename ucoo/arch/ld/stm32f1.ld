MEMORY
{
	rom (rx) : ORIGIN = 0x08000000, LENGTH = 256K
	ram (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
}

REGION_ALIAS("nvrammem", rom);
REGION_ALIAS("heapmem", ram);

INCLUDE common.ld

PROVIDE(_stack = ORIGIN(ram) + LENGTH(ram));
