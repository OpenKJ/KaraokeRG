#
# Regular cron jobs for the karaokerg package
#
0 4	* * *	root	[ -x /usr/bin/karaokerg_maintenance ] && /usr/bin/karaokerg_maintenance
