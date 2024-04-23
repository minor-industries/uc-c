# BBQueue C++/arduino code

This requires a custom arduino board definition (based on qtpy_m0). There's probably a much better way to do this, but the following works for for me:

```shell
VARIANTS=$HOME/.platformio/packages/framework-arduino-samd-adafruit/variants
cp -r $VARIANTS/qtpy_m0 $VARIANTS/bbqueue
```


Add the following pin definition lines to `const PinDescription g_APinDescription[]=` in `$VARIANTS/bbqueue/variant.cpp`:

```C++
  // CSN
  { PORTA, 14, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE},

  // RST
  { PORTA, 15, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE},

  // INTR
  { PORTA, 3, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_3},
```