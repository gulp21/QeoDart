#
# Regular cron jobs for the qeodart-2.0 package
#
0 4	* * *	root	[ -x /usr/bin/qeodart-2.0_maintenance ] && /usr/bin/qeodart-2.0_maintenance
