cmd_/home/pi/gpio-driver/modules.order := {   echo /home/pi/gpio-driver/lll-gpio-driver.ko; :; } | awk '!x[$$0]++' - > /home/pi/gpio-driver/modules.order
