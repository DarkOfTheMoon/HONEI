#!/usr/bin/env bash
# vim: set sw=4 sts=4 et tw=80 :

run() {
    echo ">>> $@"
    if ! $@ ; then
        echo "oops!" 1>&2
        exit 127
    fi
}

get() {
    type ${1}-${2}    &>/dev/null && echo ${1}-${2}    && return
    type ${1}${2//.}  &>/dev/null && echo ${1}${2//.}  && return
    type ${1}         &>/dev/null && echo ${1}         && return
    echo "Could not find ${1} ${2}" 1>&2
    exit 127
}

local ACLOCAL_ARGS=
[[ -n ${ACLOCAL_INCLUDE} ]] && ACLOCAL_ARGS="-I ${ACLOCAL_INCLUDE}"

misc/do_m4.bash benchmark/Makefile.am || exit $?
misc/do_m4.bash honei/backends/cell/ppe/Makefile.am || exit $?
misc/do_m4.bash honei/backends/cell/spe/Makefile.am || exit $?
misc/do_m4.bash honei/backends/cell/spe/kernels/Makefile.am || exit $?
misc/do_m4.bash honei/graph/Makefile.am || exit $?
misc/do_m4.bash honei/la/Makefile.am || exit $?
misc/do_m4.bash honei/mpi/Makefile.am || exit $?
misc/do_m4.bash honei/math/Makefile.am || exit $?
misc/do_m4.bash honei/swe/Makefile.am || exit $?
misc/do_m4.bash honei/fem/Makefile.am || exit $?
misc/do_m4.bash honei/util/Makefile.am || exit $?
misc/do_m4.bash honei/visual/Makefile.am || exit $?
misc/do_m4.bash honei/lbm/Makefile.am || exit $?
misc/do_m4.bash honei/woolb3/Makefile.am || exit $?
run mkdir -p config
run $(get libtoolize 1.5 ) --copy --force --automake
rm -f config.cache
run $(get aclocal 1.9 ) ${ACLOCAL_ARGS}
run $(get autoheader 2.59 )
run $(get autoconf 2.59 )
run $(get automake 1.9 ) -a --copy
