# rye register abstraction library

## Goal

Rye's goal is to provide easy, safe register abstraction that can be used when
developing drivers for microcontrollers.


## Concepts

When working with microcontrollers, one usually interacts with registers in
peripherals. These registers control how the hardware behaves. For example,
UART might have a configuration register, that lets developers set the parity
and stop bit for the serial connection. Let's have a look at the UART control
register 1 of the STM32F407, a popular device. See [Ref. manual](https://www.st.com/content/ccc/resource/technical/document/reference_manual/3d/6d/5a/66/b4/99/40/d4/DM00031020.pdf/files/DM00031020.pdf/jcr:content/translations/en.DM00031020.pdf)
for reference.

Now this register is 32 bits wide and has 16 fields of 1 bit length, the rest is
reserved (meaning that a write to those bits could result in undefined behaviour).

Bit 13 is called "UE" and enables or disables the UART periphal. A classic approach
on setting this bit would be to use something like this:

`#define UART4_BASE (0x40000000)
#define UART_CR1(instance) (*(volatile uint32_t *)(instance + 0x0))

USART_CR1 USART_CR1(usart) |= USART_CR1_UE;`

Now this of course works, but it's neither very safe nor nice to use.
These declarations allow for no write protection on individual fields and are
easily misused.
What if accidentally a value bigger than the fields bitwidth allows for is OR'd in?
That might induce some very hard to find bugs because other fields following the
intended one are being overwritten.

Rye introduces the type Bitfield that is used to abstract access to fields in
registers.
A bitfield is a templated structure and looks like this:

`Bitfield<PARENT_REGISTER, DATA_TYPE, OFFSET_IN_REG, WIDTH, ACCESS_TYPE>`

The parameters in detail:
    PARENT_REGISTER - the register this field belongs to, providing information
                      on it's width (it's underlying datatype, for example uint32_t)
                      and address
    DATA_TYPE - the type of data this field accepts, could be a plain uint8_t or
                some enum class that specifies valid values.
    OFFSET_IN_REG - offset in bits from the start of the register
    WIDTH - width in bits
    ACCESS_TYPE - might be read, write or read and write

An associated register declaration might look like this:

`struct ControlRegister1 {
    using WidthType = std::uint32_t; // this is used by bitfield to create a mask
    static constexpr WidthType* Address = PeripheralAddress + 0x0; // '0x0' is the offset
    ...
    using Ue = Bitfield<ControlRegister1, bool, 13, 1, ReadWrite>;
    ...
};`

Note that Ue is a type, not a value. In rye, everything hardware is
represented as a type rather than a value - values are used for settings etc.

So, usage of our ControlRegister1 in a driver is fairly straightforward (assuming
you know C++):

Cr1::Ue::set(); // enables the UART

Cr1::Ue::clear() // disables the UART

In this case, 'set' and 'clear' are used. The Bitfield class implements those for
1 bit fields (type bool). Other fields use 'write'
