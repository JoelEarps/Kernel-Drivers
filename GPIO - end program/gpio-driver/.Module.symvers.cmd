cmd_/home/pi/gpio-driver/Module.symvers := sed 's/ko$$/o/' /home/pi/gpio-driver/modules.order | scripts/mod/modpost -m -a   -o /home/pi/gpio-driver/Module.symvers -e -i Module.symvers   -T -
