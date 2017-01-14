APP=libgnusocial
VERSION=1.00
RELEASE=1
SONAME=${APP}.so.0
LIBNAME=${APP}-${VERSION}.so.0.0.${RELEASE}
ARCH_TYPE=`uname -m`
PREFIX?=/usr/local
LIBDIR=lib

ifeq ($(shell if [ -d /usr/lib64 ]; then echo "found"; fi;), "found")
LIBDIR = lib64
endif

all:
	gcc -shared -Wl,-soname,${SONAME} -std=c99 -pedantic -fPIC -O3 -o ${LIBNAME} src/*.c -Isrc
debug:
	gcc -shared -Wl,-soname,${SONAME} -std=c99 -pedantic -fPIC -g -o ${LIBNAME} src/*.c -Isrc
source:
	tar -cvf ../${APP}_${VERSION}.orig.tar ../${APP}-${VERSION} --exclude-vcs
	gzip -f9n ../${APP}_${VERSION}.orig.tar
install:
	mkdir -p ${DESTDIR}/usr
	mkdir -p ${DESTDIR}${PREFIX}/${LIBDIR}/${APP}
	mkdir -p ${DESTDIR}${PREFIX}/include/${APP}
	cp src/*.h ${DESTDIR}${PREFIX}/include/${APP}
	install -m 755 ${LIBNAME} ${DESTDIR}${PREFIX}/${LIBDIR}
	ln -sf ${DESTDIR}${PREFIX}/${LIBDIR}/${LIBNAME} ${DESTDIR}${PREFIX}/${LIBDIR}/${SONAME}
	ln -sf ${DESTDIR}${PREFIX}/${LIBDIR}/${LIBNAME} ${DESTDIR}/${LIBDIR}/${SONAME}
	ln -sf ${DESTDIR}${PREFIX}/${LIBDIR}/${LIBNAME} ${DESTDIR}${PREFIX}/${LIBDIR}/${APP}.so
	ldconfig
	mkdir -m 755 -p ${DESTDIR}${PREFIX}/share/man/man1
	install -m 644 man/${APP}.1.gz ${DESTDIR}${PREFIX}/share/man/man1
uninstall:
	rm -f ${PREFIX}/share/man/man1/${APP}.1.gz
	rm -f ${PREFIX}/${LIBDIR}/${LIBNAME}
	rm -f ${PREFIX}/${LIBDIR}/${APP}.so
	rm -f ${PREFIX}/${LIBDIR}/${SONAME}
	rm -f /${LIBDIR}/${SONAME}
	rm -rf ${PREFIX}/include/${APP}
	ldconfig
clean:
	rm -f ${LIBNAME} \#* \.#* src/*.plist
