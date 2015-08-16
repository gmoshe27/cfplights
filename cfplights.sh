#! /bin/sh
### BEGIN INIT INFO
# Provides:          cfplights
# Required-Start:    $remote_fs $syslog
# Required-Stop:     $remote_fs $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Starts cfplights daemon at boot time
# Description:       Enable service provided by daemon
### END INIT INFO

name=`basename $0`
pid_file="/var/run/$name.pid"
stdout_log="/var/log/$name.log"
stderr_log="/var/log/$name.err"

# init the lsb functions, otherwise this script won't work
. /lib/lsb/init-functions

get_pid () {
    cat "$pid_file"
}

is_running () {
    [ -f "$pid_file" ] && ps `get_pid` > /dev/null 2>&1
}

do_start () {
    [ -f /usr/local/sbin/cfplightsd ]
    /usr/local/sbin/cfplightsd &
    #echo $! > "$pid_file"
    #if ! is_running; then
    #    echo "Unable to start cfplights"
    #    exit 1
    #fi
}

case "$1" in
  start|"")
	do_start
	;;
  restart|reload|force-reload)
	echo "Error: argument '$1' not supported" >&2
	exit 3
	;;
  stop)
	# No-op
	;;
  status)
    # NO-op
	;;
  *)
	echo "Usage: cfplights.sh [start|stop]" >&2
	exit 3
	;;
esac

