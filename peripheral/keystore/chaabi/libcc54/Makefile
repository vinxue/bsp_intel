HOST_PROJ_ROOT ?= $(shell pwd)/../..
include $(HOST_PROJ_ROOT)/Makefile.defs

TARGET_LIBS = libdx_cc7
# A Makefile for building CC library (archive)

# Currently only these modules are working for CC54 project
cclib_sources = driver_interface.c dx_cclib.c crys_aes.c crys_hash.c crys_hmac.c crys_des.c crys_context_relocation.c  crys_aesccm.c crys_combined.c dx_pal_log.c

# SeP RPC stubs (generated source files)
cclib_sources += $(BUILDDIR)/crys_aes_seprpc_stub.c $(BUILDDIR)/applet_mng_seprpc_stub.c $(BUILDDIR)/crys_rsa_seprpc_stub.c $(BUILDDIR)/crys_ecc_seprpc_stub.c $(BUILDDIR)/crys_rc4_seprpc_stub.c $(BUILDDIR)/management_apis_seprpc_stub.c $(BUILDDIR)/crys_rnd_seprpc_stub.c $(BUILDDIR)/crys_kdf_seprpc_stub.c $(BUILDDIR)/crys_dh_seprpc_stub.c $(BUILDDIR)/crys_self_test_seprpc_stub.c

cclib_sources += tee_client_api.c

SOURCES_libdx_cc7 = $(filter-out $(CCLIB_EXCLUDED_SOURCES),$(cclib_sources))

INCDIRS_EXTRA = $(CODESAFE_SRCDIR)/pal/$(OS) driver_interface $(SHARED_INCDIR)/crys crys
# Add BUILDDIR for generated RPC stubs
INCDIRS_EXTRA += $(BUILDDIR)

INCDIRS_EXTRA += $(CODESAFE_SRCDIR)/crys/gen/inc $(CODESAFE_SRCDIR)/crys/rsa/llf_pki/inc $(CODESAFE_SRCDIR)/crys/ecc/llf_pki_ec/inc $(CODESAFE_SRCDIR)/crys/ecc/crys_ecc/ecc_common/inc
# Generate API stubs/wrappers for host
SEP_RPC_GEN = $(UTILS_SRCDIR)/sep_rpc_gen/sep_rpc_gen.py
# SeP RPC APIs to build (path to API header file without file extension - where its API XML is also located)
SEP_RPC_APIS += $(SHARED_INCDIR)/crys/crys_aes $(SHARED_INCDIR)/applet_mng $(SHARED_INCDIR)/crys/crys_rsa $(SHARED_INCDIR)/crys/crys_ecc $(SHARED_INCDIR)/crys/crys_rc4 $(SHARED_INCDIR)/management_apis $(SHARED_INCDIR)/crys/crys_rnd $(SHARED_INCDIR)/crys/crys_kdf $(SHARED_INCDIR)/crys/crys_dh $(SHARED_INCDIR)/crys/crys_self_test
# Generate APIs dependencies
$(foreach api,$(SEP_RPC_APIS),$(eval $(BUILDDIR)/$(notdir $(api))_seprpc_stub.c: $(api).xml $(call DEPENDENCY_ON_EXISTENCE_OF,$(BUILDDIR)) $(SEP_RPC_GEN)))
# SeP RPC stub generation rule
$(BUILDDIR)/%_seprpc_stub.c:
	@$(ECHO) [RPC] $< --\> $@
	@$(call exec_logged,python3 $(SEP_RPC_GEN) --spec=$< --o-api-h=$(BUILDDIR) --o-host-c=$(BUILDDIR))


#PUBLIC_INCLUDES =
# Additional "private" H files required because of inclusion in API h files
#PUBLIC_INCLUDES += crys7_components/LLF_AES/CC54_LLF_AES/inc/LLF_AES_context_def.h

# We should flatten the components source trees to avoid long search paths...
vpath %.c driver_interface cclib_interface crys ../pal/$(OS)

include $(HOST_PROJ_ROOT)/Makefile.rules


