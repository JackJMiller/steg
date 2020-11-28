# steg - .ppm image steganography

This is a program for encoding strings in .ppm images (P3 format only). Each message is hidden using a key specified by the user. To extract the message the same key must be entered.

Messages are encoded bit-by-bit in the least significant bit of each encoding pixel's colour value. This results in no visible change. The sequence of pixels used to encode the message is determined by the key.

## Usage

Compile the C program using `gcc src/*.c -o steg`.

There are two modes for steg: 'e' for hiding messages in images and 'd' for reading them.

To hide a message use `./steg e <source> <target>` replacing <target> with the name of the file in which you would like to hide your message and replacing <source> with the file of the image that you would like to use. You will be prompted for your key, which must be an integer, followed by the message to be hidden.

To extract a message use `./steg d <source>` replacing <source> with the name of the file that the message is hidden in. You will be prompted for the key that was used to hide it. If you enter the correct key then the message will be succesfully extracted and displayed.

## Usage Example

Download the sample file marvin.ppm at https://jackjmillerpersonal.s3.amazonaws.com/res/marvin.ppm. To hide a message using this image move it into the project directory and use `./steg e marvin.ppm marvin2.ppm`. You will be prompted for a key. Enter 12345 as your key. Now you will be prompted for your message. After entering your message it will be hidden in marvin2.ppm

To read the message hidden in marvin2.ppm use `./steg d marvin2.ppm`. You will be prompted for a key. Enter 12345, the key we used to hide the message, and your message will now be displayed.
