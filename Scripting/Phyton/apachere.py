#!/usr/bin/env python

import os, re

class param:
	sudo_uid    = os.environ.get('SUDO_UID')
	a_conf_path = '/etc/apache2/sites-available/000-default.conf'
	cwd_path    = os.getcwd()
	rgx_pattern = re.compile( r'(<VirtualHost \*:80>.*?[^#]DocumentRoot\s)([^\n]*)(.*)', re.DOTALL )

class data:
	file_handler    = None
	a_conf_text_old = None
	a_conf_text_new = None


if param.sudo_uid is not None:
	data.file_handler  = file( param.a_conf_path, 'r+' )
	data.conf_text_old = data.file_handler.read()

	data.conf_text_new = param.rgx_pattern.sub( r"\1" + param.cwd_path + r"\3", data.conf_text_old )

	data.file_handler.seek(0)
	data.file_handler.truncate()
	data.file_handler.write( data.conf_text_new );
	data.file_handler.close()

	print( '\033[32mChanged DocumentRoot on 127.0.0.1:80 to \033[4m' + param.cwd_path )
	print( '\033[0mRestarting Apache Service...' )

	os.system("service apache2 restart")
else:
	print( "\033[31mPlease run as root.\033[0m" );
