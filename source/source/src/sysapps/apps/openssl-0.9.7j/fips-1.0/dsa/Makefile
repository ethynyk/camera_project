#
# OpenSSL/fips-1.0/dsa/Makefile
#

DIR=	dsa
TOP=	../..
CC=	cc
INCLUDES=
CFLAG=-g
INSTALL_PREFIX=
OPENSSLDIR=     /usr/local/ssl
INSTALLTOP=/usr/local/ssl
MAKEDEPPROG=	makedepend
MAKEDEPEND=	$(TOP)/util/domd $(TOP) -MD $(MAKEDEPPROG)
MAKEFILE=	Makefile
AR=		ar r

CFLAGS= $(INCLUDES) $(CFLAG)

GENERAL=Makefile
TEST=fips_dsatest.c fips_dssvs.c
APPS=

LIB=$(TOP)/libcrypto.a
LIBSRC=fips_dsa_ossl.c fips_dsa_gen.c fips_dsa_selftest.c
LIBOBJ=fips_dsa_ossl.o fips_dsa_gen.o fips_dsa_selftest.o

SRC= $(LIBSRC)

EXHEADER=
HEADER=	$(EXHEADER)

ALL=    $(GENERAL) $(SRC) $(HEADER)

top:
	(cd $(TOP); $(MAKE) DIRS=fips FDIRS=$(DIR) sub_all)

all:	lib

lib:	$(LIBOBJ)
	@echo $(LIBOBJ) > lib

files:
	$(PERL) $(TOP)/util/files.pl Makefile >> $(TOP)/MINFO

links:
	@$(PERL) $(TOP)/util/mklink.pl $(TOP)/include/openssl $(EXHEADER)
	@$(PERL) $(TOP)/util/mklink.pl $(TOP)/test $(TEST)
	@$(PERL) $(TOP)/util/mklink.pl $(TOP)/apps $(APPS)

install:
	@headerlist="$(EXHEADER)"; for i in $$headerlist; \
	do  \
	  (cp $$i $(INSTALL_PREFIX)$(INSTALLTOP)/include/openssl/$$i; \
	  chmod 644 $(INSTALL_PREFIX)$(INSTALLTOP)/include/openssl/$$i ); \
	done

tags:
	ctags $(SRC)

tests:

Q=../testvectors/dsa/req
A=../testvectors/dsa/rsp

fips_test:
	-rm -rf $A
	mkdir $A
	if [ -f $(Q)/PQGGen.req ]; then $(TOP)/util/shlib_wrap.sh $(TOP)/test/fips_dssvs pqg < $(Q)/PQGGen.req > $(A)/PQGGen.rsp; fi
	if [ -f $(Q)/KeyPair.req ]; then $(TOP)/util/shlib_wrap.sh $(TOP)/test/fips_dssvs keypair < $(Q)/KeyPair.req > $(A)/KeyPair.rsp; fi
	if [ -f $(Q)/SigGen.req ]; then $(TOP)/util/shlib_wrap.sh $(TOP)/test/fips_dssvs siggen < $(Q)/SigGen.req > $(A)/SigGen.rsp; fi
	if [ -f $(Q)/SigVer.req ]; then $(TOP)/util/shlib_wrap.sh $(TOP)/test/fips_dssvs sigver < $Q/SigVer.req > $A/SigVer.rsp; fi

lint:
	lint -DLINT $(INCLUDES) $(SRC)>fluff

depend:
	$(MAKEDEPEND) -- $(CFLAG) $(INCLUDES) $(DEPFLAG) -- $(SRC) $(TEST)

dclean:
	$(PERL) -pe 'if (/^# DO NOT DELETE THIS LINE/) {print; exit(0);}' $(MAKEFILE) >Makefile.new
	mv -f Makefile.new $(MAKEFILE)

clean:
	rm -f *.o *.obj lib tags core .pure .nfs* *.old *.bak fluff
# DO NOT DELETE THIS LINE -- make depend depends on it.
