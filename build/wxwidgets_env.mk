################################################################################
#	MODULE	:	tools_env.mk
#	PROJECT	:	Build Scripts
#	AUTHOR	:	Michael Uman
#	DATE	:	April 22, 2013
################################################################################

WX_CONFIG ?= wx-config

ifdef DEBUG
	WX_CFLAGS=$(shell $(WX_CONFIG) --cflags --unicode=no --debug=yes)
	WX_LDFLAGS=$(shell $(WX_CONFIG) --libs --unicode=no --debug=yes)
else
	WX_CFLAGS=$(shell $(WX_CONFIG) --cflags --unicode=no --debug=no)
	WX_LDFLAGS=$(shell $(WX_CONFIG) --libs --unicode=no --debug=no)
endif

