#!/bin/bash

function printHelp {
    echo "USE genpass [-c <number-of-chars>] [-s]"
}

function genPass {
	echo $(cat /dev/urandom | tr -dc "$2" | fold -w $1 | head -n 1)
}

NUMCHARS=63
CHARSET="a-zA-Z0-9"

while [[ $# > 0 ]]
do
key="$1"

case $key in
    -h|--help)
		printHelp
		exit 1;
    	;;
   	-s|--s)
		CHARSET="a-zA-Z0-9@#$%^&*()_+?><~\`;'"
		;;
    -c|--c)
	    NUMCHARS="$2"
	    shift
	    ;;
    *)
		printHelp
		exit 1;
    	;;
esac
shift # past argument or value
done

if [ -z "${NUMCHARS}" ]; then
    printHelp
else
	genPass ${NUMCHARS} ${CHARSET}
fi


