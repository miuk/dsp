
OBJS = \
	ACELP_CA.o COD_LD8A.o DEC_LAG3.o FILTER.o \
	LSPDEC.o POSTFILT.o P_PARITY.o TAMING.o \
	BASIC_OP.o COR_FUNC.o DEC_LD8A.o GAINPRED.o \
	LSPGETQ.o POST_PRO.o QUA_GAIN.o UTIL.o \
	BITS.o DE_ACELP.o LPC.o \
	OPER_32B.o PRED_LT3.o QUA_LSP.o \
	DEC_GAIN.o DSPFUNC.o LPCFUNC.o \
	PITCH_A.o PRE_PROC.o TAB_LD8A.o

CFLAGS = -O2 -D__unix__

TARGET = libg729a.a

$(TARGET):	$(OBJS)
	ar -r $@ $+

$(OBJS):	%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f *.o *~ $(TARGET)
