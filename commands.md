### connect to wifi
`particle serial wifi`

### compile
`particle compile argon --saveTo src/firmware.bin`
`particle compile argon src/exit_listening_mode.cpp --saveTo src/exit_lm.bin`

### serial flash
`particle flash e00fce6840686fbfe628c8ca src/firmware.bin`

### cloud flash
`particle cloud flash my_argon src/firmware-alarm.bin`

### call cloud function
`particle token create --expires-in 1800`
`curl https://api.particle.io/v1/devices/e00fce6840686fbfe628c8ca/setColor -d arg="green" -H "Authorization: Bearer 66756c888153909efedf7b41b48dd2d706f26068"`

### serial log monitor
`particle serial monitor`

### install lib
`particle library add <library name>` then `particle library copy <library name>`