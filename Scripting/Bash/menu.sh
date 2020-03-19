#!/bin/bash

E='echo -e';
e='echo -en';
TEST="":
SELECTION=0;
TITLE='Debian Server Installation Script';
NO_CONFIRM=false;
PWD=$(pwd);
PWDFILES=$PWD/setup_files;

trap _trap INT;
_trap() {
	rm -R $PWDFILES;
	_tclear;
	_unmark;
    $e "Abort by user";
  	trap - INT # restore default INT handler
  	kill -s INT "$$"
    exit 1;
}

_superUser() {
	if (( $EUID != 0 )); then
		$e "Please run script as root-user\n";
		exit 0;
	fi
}

function _decode() { echo $1 | base64 -d | gunzip > $PWDFILES/$2; }

declare MENU_OPTIONS=(
	"Update Packages"
	"Configure Server"
	"Configure iptables"
	"Configure SSH-Server"
	"Install Servertools"
	"Install Apache HTTP Webserver"
	"Install MySQL"
	"Exit"
);

declare -A OPTIONS_CALLBACKS;

OPTIONS_LEN=$((${#MENU_OPTIONS[@]}-1));


_arrow() {
	read -s -n1 key 2>/dev/null >&2
	echo $key;
    if [[ $key = $($e "\033")[A ]]; then echo up; fi
    if [[ $key = $($e "\033")[B ]]; then echo dn; fi;
}

_position() {
	if [[ $cur = A ]]; then ((SELECTION--)); fi
    if [[ $cur = B ]]; then ((SELECTION++)); fi
    if [[ SELECTION -lt 0   ]]; then SELECTION=$OPTIONS_LEN; fi
    if [[ SELECTION -gt $OPTIONS_LEN ]]; then SELECTION=0; fi;
}

_confirm() {
	if !( $NO_CONFIRM ); then
		while [[ ! ($REPLY =~ [Yy] || $REPLY =~ [Nn]) ]]; do
			read -p "  # $1 (y/n) " -n 1 -r;
			$E "";
			echo;
		done
		[[ $REPLY =~ ^[Yy]$ ]] && return 0 || return 1;
	else
		return 0;
	fi
}

_installPackage() {
	$e "\n\n  > apt-get install $1\n\n";
	if ( _confirm "Install $1" ); then
		apt-get install $1;
	fi
}

_tput() { $e "\033[${1};${2}H"; }
_tclear() { _tput 1 1; $e "\033c"; }
_mark() { $e "\033[7m"; }
_unmark() { $e "\033[27m"; }


_printHeadline() {
	_mark;
	if [[ !(-z "$2") && !(-z "$3") ]]; then
		_tput $2 $3;
	fi
    printf  "  ${1}"'%*s' $((${COLUMNS:-$(tput cols)}-2-${#1})) '' | tr ' ' " "
    _unmark
}

_printBR () { $e "\n\n"; }


_printMenu() {
	local OPTION_NR="";
	local Y_OFFSET=0;
	for each in $(seq 0 $OPTIONS_LEN); do
		OPTION_NR="$(($each+1))";
		if [[ $OPTIONS_LEN -eq ${each} ]]; then Y_OFFSET=1; fi
		_printMenuOption ${each}+Y_OFFSET "[$OPTION_NR] ${MENU_OPTIONS[each]}";
		Y_OFFSET=0;
	done;
}

_printMenuOption() {
	_tput $(($1+3)) 3;
	$e "$2";
}

_printMenuSeletion() {
	local OPTION_NR="";
	for i in "${!MENU_OPTIONS[@]}"; do
	   if [[ "${MENU_OPTIONS[$i]}" = "${MENU_OPTIONS[$1]}" ]]; then OPTION_NR="$((${i}+1))"; fi
	done

	local Y_OFFSET=0;
	if [[ $OPTIONS_LEN -eq $1 ]]; then Y_OFFSET=1; fi
	_refreshMenu;
	_mark;
	_printMenuOption $1+Y_OFFSET "[$OPTION_NR] ${MENU_OPTIONS[$1]}";
	$e " <";
	cur=`_arrow`;
}

_printBack() {
	_mark;
	_printBR;
	_printHeadline "[ENTER]=Back to menu";
	read -s;
	_refreshMenu;
}

_refreshMenu() {
	_tclear;
	_printHeadline "$TITLE";
	_printMenu;
	_printBR;
	_printHeadline "[ENTER]=Select, [UP]/[DN]=Move Up/Down $cur";
}

_runMenu() {
	_refreshMenu;
	mkdir -p $PWDFILES;
	mkdir -p $PWDFILES/scripts;

	if [[ $1 == "1" ]]; then
		_superUser;
	fi;
	
	while [[ "$O" != " " ]]; do
		_printMenuSeletion $SELECTION;
		MENU_OPTION=${MENU_OPTIONS[$SELECTION]};

		if [[ $cur > 0 && $cur < $(($OPTIONS_LEN+2)) ]]; then
			SELECTION=$(($cur-1));
			MENU_OPTION=${MENU_OPTIONS[$(($cur-1))]};
			cur="";
		fi;

		if [[ $cur = "" ]]; then
			_tclear;
			${OPTIONS_CALLBACKS[$MENU_OPTION]} "$MENU_OPTION";
			_printSubFoot;
		fi;

		_position;
	done
}

FIREWALL_SCRIPT='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';
MOTD_TEMPLATE='H4sIABPU3l0CA5WPuwrDMAxFd3+FqsUOOM2qqXNHr4YS28EpFPqADoUs/fZKdgNxt97Bko6eVgA2Zee8n5KFNSj+ltMkSWaZFABiCaIngEQuiIhaLq8FS06r7yiWFJFzWNVyVkdEhsuJyjZbZtbVNqWWyylYstwQ2U7ZYV2MngNC3XK+ZL1OqfqZ/kf7wg0c5+v1AbcFzs/LfM876GpD5Jpl26ALt28xRr+6ENgzA5gYB2GnEA5aSeU49n9IfQDpc8YblwEAAA==';
PMA_TEMPLATE='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';


OPTIONS_CALLBACKS["Update Packages"]=_updatePackages;
OPTIONS_CALLBACKS["Configure Server"]=_configureServer;
OPTIONS_CALLBACKS["Configure iptables"]=_configureIptables;
OPTIONS_CALLBACKS["Configure SSH-Server"]=_configureSSHServer;
OPTIONS_CALLBACKS["Install Servertools"]=_installServertools;
OPTIONS_CALLBACKS["Install Apache HTTP Webserver"]=_installApache;
OPTIONS_CALLBACKS["Install MySQL"]=_installMySQL;
OPTIONS_CALLBACKS["Exit"]=_exit;


_updatePackages() {
	_printHeadline "$1";

	$e "\n\n  > apt-get update\n  > apt-get upgrade\n\n";
	if ( _confirm "Run update?" ); then
		apt-get update;
		apt-get upgrade;
		_printBack;
	fi
}

_configureServer() {
	_printHeadline "$1";

	$e "\n\n  > chsh -s /bin/bash\n\n";
	if ( _confirm "Set default shell to bash?" ); then
		chsh -s /bin/bash;
		_printBack;
	fi	
}

_configureIptables() {
	_printHeadline "$1";

	$e "\n\n  > $PWDFILES/scripts/firewall.sh\n\n";
	if ( _confirm "Run iptables Configuration?" ); then
		_decode $FIREWALL_SCRIPT "scripts/firewall.sh";
		chmod -R 755 $PATH_SETUPFILES/scripts;
		$PWDFILES/scripts/firewall.sh;
	fi
}

_configureSSHServer() {
	_printHeadline "$1";

	$e "\n\n  > useradd -m sshuser\n  > passwd sshuser\n  > groupadd remote\n  > usermod -a -G remote sshuser\n  > grep remote /etc/group\n  > chsh -s /bin/bash sshuser\n\n";
	if ( _confirm "Create Remote User?" ); then
		useradd -m sshuser;
		passwd sshuser;
		groupadd remote;
		usermod -a -G remote sshuser;
		grep remote /etc/group;
		chsh -s /bin/bash sshuser;
		ssh-copy-id sshuser@aeax.net
	fi

	$e "\n\n  > sed -i -e \"s/root.*=.*/&\\\nsshuser ALL=(ALL:ALL) ALL/g\" /etc/sudoers\n\n";
	if ( _confirm "Add sshuser to sudoers?" ); then
		sed -i -e "s/root.*=.*/&\nsshuser ALL=\(ALL:ALL\) ALL/g" /etc/sudoers;
	fi

	$e "\n\n  > cp $PWDFILES/motd_template /etc/motd\n\n";
	if ( _confirm "Generate SSH Login Banner ?" ); then
		_decode $MOTD_TEMPLATE "motd_template";
		cp $PWDFILES/motd_template /etc/motd;
	fi

	$e "\n\n  > sed -i -e \"s/#*PermitRootLogin\ \(yes\|no\)/PermitRootLogin\ no/g\" /etc/ssh/sshd_config\n\n";
	if ( _confirm "Forbit Root Login?" ); then
		sed -i -e "s/#*PermitRootLogin\ \(yes\|no\)/PermitRootLogin\ no/g" /etc/ssh/sshd_config;
	fi

	$e "\n\n  > sed -i -e \"s/#*PubkeyAuthentication\ \(yes\|no\)/PubkeyAuthentication\ yes/g\" /etc/ssh/sshd_config;\n\n";
	if ( _confirm "Enable Public Key Authentication?" ); then
		sed -i -e "s/#*PubkeyAuthentication\ \(yes\|no\)/PubkeyAuthentication\ yes/g" /etc/ssh/sshd_config;
	fi
}

_installServertools() {
	_printHeadline "$1";

	_installPackage "sudo";
	_installPackage "vim";
	_installPackage "net-tools";
	_installPackage "htop";
	_installPackage "byobu";
	_installPackage "nano";

	_printBack;
}

_installApache() {
	$e "\n\n  > apt-get install apache2 apache2-doc apache2-utils\n\n";
	if ( _confirm "Install HTTP Server?" ); then
		apt-get install apache2 apache2-doc apache2-utils;
	fi

	$e "\n\n";
	if ( _confirm "Enable Prefork Module?" ); then
		echo -e "# prefork MPMls

# StartServers: number of server processes to start
# MinSpareServers: minimum number of server processes which are kept spare
# MaxSpareServers: maximum number of server processes which are kept spare
# MaxRequestWorkers: maximum number of server processes allowed to start
# MaxConnectionsPerChild: maximum number of requests a server process serves

<IfModule mpm_prefork_module>
        StartServers              4
        MinSpareServers           20
        MaxSpareServers           40
        MaxRequestWorkers         200
        MaxConnectionsPerChild    4500
</IfModule>" > /etc/apache2/mods-available/mpm_prefork.conf;
		a2dismod mpm_event;
		a2enmod mpm_prefork;
		systemctl stop apache2;
		systemctl start apache2;
	fi

	$e "\n\n";
	if ( _confirm "Create VirtualHost?" ); then
		echo -e -n "  Domainname (eg. example.com): > ";
		read APACHE_DOMAIN;

		$e "  Disabling Default Site";
		a2dissite 000-default.conf;

		$e "  Creating Virtual Host";
		echo -e "<VirtualHost *:80>
     ServerAdmin webmaster@$APACHE_DOMAIN
     ServerName $APACHE_DOMAIN
     ServerAlias www.$APACHE_DOMAIN 
     DocumentRoot /var/www/$APACHE_DOMAIN/public_html/
     ErrorLog /var/www/$APACHE_DOMAIN/logs/error.log
     CustomLog /var/www/$APACHE_DOMAIN/logs/access.log combined
</VirtualHost>" > /etc/apache2/sites-available/$APACHE_DOMAIN.conf;

		mkdir -p /var/www/$APACHE_DOMAIN/public_html;
		mkdir /var/www/$APACHE_DOMAIN/logs;

		a2ensite $APACHE_DOMAIN.conf;
		systemctl restart apache2;

		echo -e "Hello!" > /var/www/$APACHE_DOMAIN/public_html/index.html;
	fi

	$e "\n\n  > apt-get install php libapache2-mod-php php-mysql\n\n";
	if ( _confirm "Install PHP?" ); then
		apt-get install php libapache2-mod-php php-mysql;

		echo -e "<?php phpinfo(); ?>" > /var/www/$APACHE_DOMAIN/public_html/info.php;
	fi
	
}

_installMySQL() {
	_printHeadline "$1";

	$e "\n\n";
	if ( _confirm "Install MySQL?" ); then

		$e "  Downloading package\n";
		cd /tmp;
		wget https://dev.mysql.com/get/mysql-apt-config_0.8.13-1_all.deb;
		apt-get install lsb-release gnupg
		dpkg -i mysql-apt-config_0.8.13-1_all.deb;
		cd -;

		apt-get update;
		
		apt-get install mysql-server;

		sudo systemctl restart mysql; 

		mysql_secure_installation;
	fi

	$e "\n\n";
	if ( _confirm "Install phpMyAdmin?" ); then
		apt-get install unzip;
		apt-get install php-mbstring;
		apt-get install php7.3-mbstring;

		$e "  Downloading package\n";
		cd /tmp;
		wget https://files.phpmyadmin.net/phpMyAdmin/4.9.2/phpMyAdmin-4.9.2-all-languages.zip;
		unzip phpMyAdmin-4.9.2-all-languages.zip -d /opt;
		cd -;
		
		$e "  Installing\n";
		mv -v /opt/phpMyAdmin-4.9.2-all-languages /opt/phpmyadmin;
		mkdir /opt/tmp;
		mv config.sample.inc.php config.inc.php;
		chown -Rfv www-data:www-data /opt/phpmyadmin;
		
		echo -e -n "\n\n  # phpMyAdmin Port (eg. 9000): > ";
		read PMA_PORT;

		$e "  Creating Virtual Host\n";
		echo -e '<VirtualHost *:'$PMA_PORT'>
     ServerAdmin webmaster@localhost
     DocumentRoot /opt/phpmyadmin
 
     <Directory /opt/phpmyadmin>
          Options Indexes FollowSymLinks
          AllowOverride none
          Require all granted
     </Directory>

     ErrorLog ${APACHE_LOG_DIR}/error_phpmyadmin.log
     CustomLog ${APACHE_LOG_DIR}/access_phpmyadmin.log combined
</VirtualHost>' > /etc/apache2/sites-available/phpmyadmin.conf;

		sed -i -e "s/Listen 80/Listen 80\nListen $PMA_PORT/g" /etc/apache2/ports.conf;

		a2ensite phpmyadmin.conf;

		$e "  Creating conig file -> /opt/phpmyadmin/config.inc.php\n";
		_decode $PMA_TEMPLATE "phpmyadmin_template";
		cp $PWDFILES/phpmyadmin_template /opt/phpmyadmin/config.inc.php;


		$e "  Creating Configuration Storage\n";
		mysql -u root -p < /opt/phpmyadmin/sql/create_tables.sql;

		$e "\n  MySQL Root Password\n";
		mysql -u root -p -e "CREATE USER 'pma'@'localhost' IDENTIFIED BY 'c1ddf8dfbb78862a7b8fd03d53352df4c35ecc657d5e3a766aa0f6138dc845f4';";
		mysql -u root -p -e "GRANT SELECT, INSERT, UPDATE, DELETE ON phpmyadmin.* TO 'pma'@'localhost';";
		mysql -u root -p -e "GRANT USAGE ON mysql.* TO 'pma'@'localhost';";
		mysql -u root -p -e "GRANT SELECT (
		    Host, User, Select_priv, Insert_priv, Update_priv, Delete_priv,
		    Create_priv, Drop_priv, Reload_priv, Shutdown_priv, Process_priv,
		    File_priv, Grant_priv, References_priv, Index_priv, Alter_priv,
		    Show_db_priv, Super_priv, Create_tmp_table_priv, Lock_tables_priv,
		    Execute_priv, Repl_slave_priv, Repl_client_priv
		    ) ON mysql.user TO 'pma'@'localhost';";
		mysql -u root -p -e "GRANT SELECT ON mysql.db TO 'pma'@'localhost';";
		mysql -u root -p -e "GRANT SELECT (Host, Db, User, Table_name, Table_priv, Column_priv)
		    ON mysql.tables_priv TO 'pma'@'localhost';";

		systemctl stop apache2;
		systemctl start apache2;
	fi
}

_exit() {
	rm -R $PWDFILES;
	exit 0;
}

_runMenu