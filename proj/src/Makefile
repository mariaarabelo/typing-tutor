# name of the program (Minix service)
PROG=proj


.PATH: ${.CURDIR}/drivers/keyboard
.PATH: ${.CURDIR}/drivers/mouse
.PATH: ${.CURDIR}/drivers/graphics
.PATH: ${.CURDIR}/drivers/timer
.PATH: ${.CURDIR}/drivers/rtc
.PATH: ${.CURDIR}/drivers/serialport
.PATH: ${.CURDIR}/drivers
.PATH: ${.CURDIR}/model
.PATH: ${.CURDIR}/view


# source code files to be compiled
SRCS = proj.c timer.c keyboard.c mouse.c video.c utils.c model.c view.c sprite.c rtc.c serialport.c wordPool.c messages.c

# Flags de compilação
CFLAGS += -pedantic -D_LCOM_OPTIMIZED_ -O2

# Dependências
DPADD += ${LIBLCF}
LDADD += -Wall -llcf -lxpm -llm
.include <minix.lcom.mk>
