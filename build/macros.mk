# ucoolib - Microcontroller object oriented library.
#
# Usefull macros.

# Return $(A) only if defined, else return B.
# $(call defval,A,B)
defval = $(if $(filter undefined,$(origin $1)),$2,$($1))

# Filter out source file for other targets, return basenames.
# $(call filter_sources,TARGET,SOURCES)
filter_sources = $(call filter_sources_sub,$1,$(basename $2))
filter_sources_sub = $(foreach t,$1 $($1_SUBTARGETS),$(filter %.$t,$2)) \
		     $(foreach f,$2,$(if $(findstring .,$f),,$f))

# Test if one of the source is C++, in this case, return cc, else return c.
# $(call iscxx,SOURCES)
iscxx = $(if $(filter %.cc,$1),cc,c)
