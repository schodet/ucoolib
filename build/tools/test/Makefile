CONFIG_GEN = ../config-gen
MODULES_CONFIG = a-Config b-Config

TARGETS := host:host stm32f4:stm32f4 stm32f4:arm

TESTS = ok nok-unknown nok-nodefault nok-missdefault nok-toomany nok-badtarget

all: $(TESTS:%=test-%)

test-%: Config-% $(MODULES_CONFIG) $(CONFIG_GEN)
	expect=$(if $(filter ok,$*),0,1); \
	$(CONFIG_GEN) -p $< -H out-$*/%.hh -T "$(TARGETS)" $(MODULES_CONFIG); \
	test $$expect -eq $$?

clean:
	rm -rf $(TESTS:%=out-%)
