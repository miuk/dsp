
FRAMEWORKS =\
	-framework CoreAudio \
	-framework AudioToolbox \
	-framework CoreFoundation

OBJS = \
	QueueAudioPlayer.o \
	QueueAudioRecorder.o \
	DirectAudio.o

DirectAudio:	$(OBJS)
	gcc -o $@ $+ $(FRAMEWORKS)

clean:
	rm -f *.o *~ a.out DirectAudio
